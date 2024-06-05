/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/Console.h"

#include <thread>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
#    include <io.h>
#    define bzero(a, b) memset(a, 0, b)
#endif

#include "base/Director.h"
#include "base/Scheduler.h"
#include "platform/PlatformConfig.h"
#include "base/Configuration.h"
#include "2d/Scene.h"
#include "platform/FileUtils.h"
#include "renderer/TextureCache.h"
#include "base/Utils.h"
#include "base/UTF8.h"

#include "yasio/xxsocket.hpp"

NS_AX_BEGIN

extern const char* axmolVersion(void);

#define PROMPT                    "> "
#define DEFAULT_COMMAND_SEPARATOR '|'

static const size_t SEND_BUFSIZ = 512;

//
//  Utility code
//

std::string Console::Utility::_prompt(PROMPT);

// TODO: these general utils should be in a separate class
//
//  Trimming functions were taken from: http://stackoverflow.com/a/217605
//  Since c++17, some parts of the standard library were removed, include "ptr_fun".

// trim from start

std::string& Console::Utility::ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    return s;
}

// trim from end
std::string& Console::Utility::rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

// trim from both ends
std::string& Console::Utility::trim(std::string& s)
{
    return Console::Utility::ltrim(Console::Utility::rtrim(s));
}

std::vector<std::string>& Console::Utility::split(std::string_view s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss;
    ss << s;
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.emplace_back(item);
    }
    return elems;
}

std::vector<std::string> Console::Utility::split(std::string_view s, char delim)
{
    std::vector<std::string> elems;
    Console::Utility::split(s, delim, elems);
    return elems;
}

// isFloat taken from http://stackoverflow.com/questions/447206/c-isfloat-function
bool Console::Utility::isFloat(std::string_view myString)
{
    std::stringstream ss;
    ss << myString;
    float f;
    ss >> std::noskipws >> f;  // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return ss.eof() && !ss.fail();
}

ssize_t Console::Utility::sendToConsole(int fd, const void* buffer, size_t length, int flags)
{
    if (_prompt.length() == length)
    {
        if (strncmp(_prompt.c_str(), static_cast<const char*>(buffer), length) == 0)
        {
            fprintf(stderr, "bad parameter error: a buffer is the prompt string.\n");
            return 0;
        }
    }

    const char* buf = static_cast<const char*>(buffer);
    ssize_t retLen  = 0;
    for (size_t i = 0; i < length;)
    {
        size_t len = length - i;
        if (SEND_BUFSIZ < len)
            len = SEND_BUFSIZ;
        retLen += send(fd, buf + i, len, flags);
        i += len;
    }
    return retLen;
}

// dprintf() is not defined in Android
// so we add our own 'dpritnf'
ssize_t Console::Utility::mydprintf(int sock, const char* format, ...)
{
    va_list args;
    char buf[16386];

    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return sendToConsole(sock, buf, strlen(buf));
}

void Console::Utility::sendPrompt(int fd)
{
    const char* prompt = _prompt.c_str();
    send(fd, prompt, strlen(prompt), 0);
}

void Console::Utility::setPrompt(std::string_view prompt)
{
    _prompt = prompt;
}

std::string_view Console::Utility::getPrompt()
{
    return _prompt;
}

//
// Command code
//

Console::Command::Command() : _callback(nullptr) {}

Console::Command::Command(std::string_view name, std::string_view help) : _name(name), _help(help), _callback(nullptr)
{}

Console::Command::Command(std::string_view name, std::string_view help, const Callback& callback)
    : _name(name), _help(help), _callback(callback)
{}

Console::Command::Command(const Command& o)
{
    *this = o;
}

Console::Command::Command(Command&& o) noexcept
{
    *this = std::move(o);
}

Console::Command::~Command()
{
    for (const auto& e : _subCommands)
    {
        delete e.second;
    }
}

Console::Command& Console::Command::operator=(const Command& o)
{
    if (this != &o)
    {
        _name     = o._name;
        _help     = o._help;
        _callback = o._callback;

        for (const auto& e : _subCommands)
            delete e.second;

        _subCommands.clear();
        for (const auto& e : o._subCommands)
        {
            Command* subCommand   = e.second;
            auto newCommand       = new Command(*subCommand);
            _subCommands[e.first] = newCommand;
        }
    }

    return *this;
}

Console::Command& Console::Command::operator=(Command&& o) noexcept
{
    if (this != &o)
    {
        _name       = std::move(o._name);
        _help       = std::move(o._help);
        _callback   = std::move(o._callback);
        o._callback = nullptr;

        for (const auto& e : _subCommands)
            delete e.second;

        _subCommands.clear();
        _subCommands = std::move(o._subCommands);
    }

    return *this;
}

void Console::Command::addCallback(const Callback& callback)
{
    _callback = callback;
}

void Console::Command::addSubCommand(const Command& subCmd)
{
    auto iter = _subCommands.find(subCmd._name);
    if (iter != _subCommands.end())
    {
        delete iter->second;
        _subCommands.erase(iter);
    }

    Command* cmd               = new Command();
    *cmd                       = subCmd;
    _subCommands[subCmd._name] = cmd;
}

const Console::Command* Console::Command::getSubCommand(std::string_view subCmdName) const
{
    auto it = _subCommands.find(subCmdName);
    if (it != _subCommands.end())
    {
        auto& subCmd = it->second;
        return subCmd;
    }
    return nullptr;
}

void Console::Command::delSubCommand(std::string_view subCmdName)
{
    auto iter = _subCommands.find(subCmdName);
    if (iter != _subCommands.end())
    {
        delete iter->second;
        _subCommands.erase(iter);
    }
}

void Console::Command::commandHelp(int fd, std::string_view /*args*/)
{
    if (!_help.empty())
    {
        Console::Utility::mydprintf(fd, "%s\n", _help.c_str());
    }

    if (!_subCommands.empty())
    {
        sendHelp(fd, _subCommands, "");
    }
}

void Console::Command::commandGeneric(int fd, std::string_view args)
{
    // The first argument (including the empty)
    std::string key(args);
    auto pos = args.find(' ');
    if ((pos != std::string::npos) && (0 < pos))
    {
        key = args.substr(0, pos);
    }

    // help
    if (key == "help" || key == "-h")
    {
        commandHelp(fd, args);
        return;
    }

    // find sub command
    auto it = _subCommands.find(key);
    if (it != _subCommands.end())
    {
        auto subCmd = it->second;
        if (subCmd->_callback)
        {
            subCmd->_callback(fd, args);
        }
        return;
    }

    // can not find
    if (_callback)
    {
        _callback(fd, args);
    }
}

//
// Console code
//

Console::Console()
    : _commandSeparator(DEFAULT_COMMAND_SEPARATOR)
    , _listenfd(-1)
    , _running(false)
    , _endThread(false)
    , _isIpv6Server(false)
    , _sendDebugStrings(false)
    , _bindAddress()
{
    createCommandAllocator();
    createCommandConfig();
    createCommandDebugMsg();
    createCommandDirector();
    createCommandExit();
    createCommandFileUtils();
    createCommandFps();
    createCommandHelp();
    createCommandProjection();
    createCommandResolution();
    createCommandSceneGraph();
    createCommandTexture();
    createCommandTouch();
    createCommandUpload();
    createCommandVersion();
}

Console::~Console()
{
    stop();

    for (auto&& e : _commands)
        delete e.second;
}

bool Console::listenOnTCP(int port)
{
    using namespace yasio;
    xxsocket sock;
    int ipsv = 0;
    xxsocket::traverse_local_address([&](const ip::endpoint& ep) {
        if (ep.af() == AF_INET)
            ipsv |= ipsv_ipv4;
        else if (ep.af() == AF_INET6)
            ipsv |= ipsv_ipv6;
        return ipsv == ipsv_dual_stack;
    });

    const char* finalBindAddr =
        this->_bindAddress.empty() ? (ipsv != ipsv_ipv6 ? "0.0.0.0" : ("::")) : this->_bindAddress.c_str();
    ip::endpoint ep{finalBindAddr, static_cast<u_short>(port)};
    if (sock.pserve(ep) != 0)
    {
        int ec = xxsocket::get_last_errno();
        AXLOGW("Console: open server failed, ec:{}", ec);
        return false;
    }

    if (ep.af() == AF_INET)
        AXLOGI("Console: IPV4 server is listening on {}", ep.to_string());
    else if (ep.af() == AF_INET6)
        AXLOGI("Console: IPV6 server is listening on {}", ep.to_string());

    return listenOnFileDescriptor(sock.release_handle());
}

bool Console::listenOnFileDescriptor(int fd)
{
    if (_running)
    {
        AXLOGW("Console already started. 'stop' it before calling 'listen' again");
        return false;
    }

    _listenfd = fd;
    _thread   = std::thread(std::bind(&Console::loop, this));

    return true;
}

void Console::stop()
{
    if (_running)
    {
        _endThread = true;
        _watcher.wakeup();
        if (_thread.joinable())
        {
            _thread.join();
        }
    }
}

void Console::addCommand(const Command& cmd)
{
    Command* newCommand = new Command(cmd);
    auto iter           = _commands.find(cmd.getName());
    if (iter != _commands.end())
    {
        delete iter->second;
        _commands.erase(iter);
    }
    _commands.emplace(cmd.getName(), newCommand);  // _commands[cmd.getName()] = newCommand;
}

void Console::addSubCommand(std::string_view cmdName, const Command& subCmd)
{
    auto it = _commands.find(cmdName);
    if (it != _commands.end())
    {
        auto& cmd = it->second;
        addSubCommand(*cmd, subCmd);
    }
}

void Console::addSubCommand(Command& cmd, const Command& subCmd)
{
    cmd.addSubCommand(subCmd);
}

const Console::Command* Console::getCommand(std::string_view cmdName)
{
    auto it = _commands.find(cmdName);
    if (it != _commands.end())
    {
        auto& cmd = it->second;
        return cmd;
    }
    return nullptr;
}

const Console::Command* Console::getSubCommand(std::string_view cmdName, std::string_view subCmdName)
{
    auto it = _commands.find(cmdName);
    if (it != _commands.end())
    {
        auto& cmd = it->second;
        return getSubCommand(*cmd, subCmdName);
    }
    return nullptr;
}

const Console::Command* Console::getSubCommand(const Command& cmd, std::string_view subCmdName)
{
    return cmd.getSubCommand(subCmdName);
}

void Console::delCommand(std::string_view cmdName)
{
    auto it = _commands.find(cmdName);
    if (it != _commands.end())
    {
        delete it->second;
        _commands.erase(it);
    }
}

void Console::delSubCommand(std::string_view cmdName, std::string_view subCmdName)
{
    auto it = _commands.find(cmdName);
    if (it != _commands.end())
    {
        auto& cmd = it->second;
        delSubCommand(*cmd, subCmdName);
    }
}

void Console::delSubCommand(Command& cmd, std::string_view subCmdName)
{
    cmd.delSubCommand(subCmdName);
}

void Console::setBindAddress(std::string_view address)
{
    _bindAddress = address;
}

bool Console::isIpv6Server() const
{
    return _isIpv6Server;
}

//
// Main Loop
//
void Console::loop()
{
    _running = true;

    _watcher.mod_event(_listenfd, yasio::socket_event::read, 0);

    const int64_t timeout_usec = 300 * std::micro::den;

    while (!_endThread)
    {
        int nready = _watcher.poll_io(timeout_usec);

        if (nready == -1)
        {
            /* error */
            if (errno != EINTR)
                AXLOGW("Abnormal error in poll_io()\n");
            continue;
        }
        else if (nready == 0)
        {
            /* timeout. do something ? */
        }
        else
        {
            if (_endThread)
                break;

            /* new client */
            if (_watcher.is_ready(_listenfd, yasio::socket_event::read))
            {
                addClient();
                if (--nready <= 0)
                    continue;
            }

            /* data from client */
            std::vector<socket_native_type> to_remove;
            for (const auto& fd : _fds)
            {
                if (_watcher.is_ready(fd, yasio::socket_event::read))
                {
                    // fix Bug #4302 Test case ConsoleTest--ConsoleUploadFile crashed on Linux
                    // On linux, if you send data to a closed socket, the sending process will
                    // receive a SIGPIPE, which will cause linux system shutdown the sending process.
                    // Add this ioctl code to check if the socket has been closed by peer.

#if defined(_WIN32)
                    u_long n = 0;
                    ioctlsocket(fd, FIONREAD, &n);
#else
                    int n = 0;
                    if (ioctl(fd, FIONREAD, &n) < 0)
                    {
                        AXLOGE("Abnormal error in ioctl()\n");
                        break;
                    }
#endif
                    if (n == 0)
                    {
                        // no data received, or fd is closed
                        // fix #18620. readable and no pending data means that the fd is closed.
                        to_remove.emplace_back(fd);
                        continue;
                    }

                    if (!parseCommand(fd))
                    {
                        to_remove.emplace_back(fd);
                    }
                    if (--nready <= 0)
                        break;
                }
            }

            /* remove closed connections */
            for (socket_native_type fd : to_remove)
            {
                _watcher.mod_event(fd, 0, yasio::socket_event::read);
                _fds.erase(std::remove(_fds.begin(), _fds.end(), fd), _fds.end());
            }
        }

        /* Any message for the remote console ? send it! */
        if (!_DebugStrings.empty())
        {
            if (_DebugStringsMutex.try_lock())
            {
                for (const auto& str : _DebugStrings)
                {
                    for (auto&& fd : _fds)
                    {
                        Console::Utility::sendToConsole(fd, str.c_str(), str.length());
                    }
                }
                _DebugStrings.clear();
                _DebugStringsMutex.unlock();
            }
        }
    }

    // clean up: ignore stdin, stdout and stderr
    for (const auto& fd : _fds)
        closesocket(fd);

    closesocket(_listenfd);

    _running = false;
}

//
// Helpers
//

ssize_t Console::readline(socket_native_type fd, char* ptr, size_t maxlen)
{
    size_t n, rc;
    char c;

    for (n = 0; n < maxlen - 1; n++)
    {
        if ((rc = recv(fd, &c, 1, 0)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
            {
                break;
            }
        }
        else if (rc == 0)
        {
            return 0;
        }
        else if (errno == EINTR)
        {
            continue;
        }
        else
        {
            return -1;
        }
    }

    *ptr = 0;
    return n;
}

ssize_t Console::readBytes(socket_native_type fd, char* buffer, size_t maxlen, bool* more)
{
    size_t n, rc;
    char c, *ptr = buffer;
    *more = false;
    for (n = 0; n < maxlen; n++)
    {
        if ((rc = recv(fd, &c, 1, 0)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
            {
                return n;
            }
        }
        else if (rc == 0)
        {
            return 0;
        }
        else if (errno == EINTR)
        {
            continue;
        }
        else
        {
            return -1;
        }
    }
    *more = true;
    return n;
}

bool Console::parseCommand(socket_native_type fd)
{
    char buf[512];
    bool more_data;
    auto h = readBytes(fd, buf, 6, &more_data);
    if (h < 0)
    {
        return false;
    }
    if (strncmp(buf, "upload", 6) == 0)
    {
        char c = '\0';
        recv(fd, &c, 1, 0);
        if (c == ' ')
        {
            commandUpload(fd);
            Console::Utility::sendPrompt(fd);
            return true;
        }
        else
        {
            const char err[] = "upload: invalid args! Type 'help' for options\n";
            Console::Utility::sendToConsole(fd, err, strlen(err));
            Console::Utility::sendPrompt(fd);
            return true;
        }
    }
    if (!more_data)
    {
        buf[h] = 0;
    }
    else
    {
        char* pb = buf + 6;
        auto r   = readline(fd, pb, sizeof(buf) - 6);
        if (r < 0)
        {
            const char err[] = "Unknown error!\n";
            Console::Utility::sendPrompt(fd);
            Console::Utility::sendToConsole(fd, err, strlen(err));
            return false;
        }
    }
    std::string cmdLine;
    cmdLine       = std::string(buf);
    auto commands = Console::Utility::split(cmdLine, _commandSeparator);
    try
    {
        for (auto&& command : commands)
        {
            performCommand(fd, Console::Utility::trim(command));
        }
    }
    catch (const std::runtime_error& e)
    {
        Console::Utility::sendToConsole(fd, e.what(), strlen(e.what()));
    }

    Console::Utility::sendPrompt(fd);

    return true;
}

void Console::performCommand(socket_native_type fd, std::string_view command)
{
    std::vector<std::string> args = Console::Utility::split(command, ' ');
    if (args.empty())
    {
        throw std::runtime_error("Unknown command. Type 'help' for options\n");
    }

    auto it = _commands.find(Console::Utility::trim(args[0]));
    if (it != _commands.end())
    {
        std::string args2;
        for (size_t i = 1; i < args.size(); ++i)
        {
            if (i > 1)
            {
                args2 += ' ';
            }
            args2 += Console::Utility::trim(args[i]);
        }
        auto cmd = it->second;
        cmd->commandGeneric(fd, args2);
    }
    else
    {
        throw std::runtime_error(std::string{"Unknown command "}.append(command).append(". Type 'help' for options\n"));
    }
}

void Console::addClient()
{
    struct sockaddr_in6 ipv6Addr;
    struct sockaddr_in ipv4Addr;
    struct sockaddr* addr = _isIpv6Server ? (struct sockaddr*)&ipv6Addr : (struct sockaddr*)&ipv4Addr;
    socklen_t addrLen     = _isIpv6Server ? sizeof(ipv6Addr) : sizeof(ipv4Addr);

    /* new client */
    socket_native_type fd = accept(_listenfd, addr, &addrLen);

    // add fd to list of FD
    if (fd != -1)
    {
        _watcher.mod_event(fd, yasio::socket_event::read, 0);
        _fds.emplace_back(fd);

        Console::Utility::sendPrompt(fd);

        /**
         * A SIGPIPE is sent to a process if it tried to write to socket that had been shutdown for
         * writing or isn't connected (anymore) on iOS.
         *
         * The default behaviour for this signal is to end the process.So we make the process ignore SIGPIPE.
         */
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS
        int set = 1;
        setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
    }
}

//
// create commands
//

void Console::createCommandAllocator()
{
    addCommand({"allocator", "Display allocator diagnostics for all allocators. Args: [-h | help | ]",
                AX_CALLBACK_2(Console::commandAllocator, this)});
}

void Console::createCommandConfig()
{
    addCommand({"config", "Print the Configuration object. Args: [-h | help | ]",
                AX_CALLBACK_2(Console::commandConfig, this)});
}

void Console::createCommandDebugMsg()
{
    addCommand({"debugmsg",
                "Whether or not to forward the debug messages on the console. Args: [-h | help | on | off | ]",
                AX_CALLBACK_2(Console::commandDebugMsg, this)});
    addSubCommand("debugmsg",
                  {"on", "enable debug logging", AX_CALLBACK_2(Console::commandDebugMsgSubCommandOnOff, this)});
    addSubCommand("debugmsg",
                  {"off", "disable debug logging", AX_CALLBACK_2(Console::commandDebugMsgSubCommandOnOff, this)});
}

void Console::createCommandDirector()
{
    addCommand({"director", "director commands, type -h or [director help] to list supported directives"});
    addSubCommand("director",
                  {"pause", "pause all scheduled timers, the draw rate will be 4 FPS to reduce CPU consumption",
                   AX_CALLBACK_2(Console::commandDirectorSubCommandPause, this)});
    addSubCommand("director", {"resume", "resume all scheduled timers",
                               AX_CALLBACK_2(Console::commandDirectorSubCommandResume, this)});
    addSubCommand("director", {"stop", "Stops the animation. Nothing will be drawn.",
                               AX_CALLBACK_2(Console::commandDirectorSubCommandStop, this)});
    addSubCommand(
        "director",
        {"start", "Restart the animation again, Call this function only if [director stop] was called earlier",
         AX_CALLBACK_2(Console::commandDirectorSubCommandStart, this)});
    addSubCommand("director", {"end", "exit this app.", AX_CALLBACK_2(Console::commandDirectorSubCommandEnd, this)});
}

void Console::createCommandExit()
{
    addCommand(
        {"exit", "Close connection to the console. Args: [-h | help | ]", AX_CALLBACK_2(Console::commandExit, this)});
}

void Console::createCommandFileUtils()
{
    addCommand({"fileutils", "Flush or print the FileUtils info. Args: [-h | help | flush | ]",
                AX_CALLBACK_2(Console::commandFileUtils, this)});
    addSubCommand("fileutils", {"flush", "Purges the file searching cache.",
                                AX_CALLBACK_2(Console::commandFileUtilsSubCommandFlush, this)});
}

void Console::createCommandFps()
{
    addCommand(
        {"fps", "Turn on / off the FPS. Args: [-h | help | on | off | ]", AX_CALLBACK_2(Console::commandFps, this)});
    addSubCommand("fps", {"on", "Display the FPS on the bottom-left corner.",
                          AX_CALLBACK_2(Console::commandFpsSubCommandOnOff, this)});
    addSubCommand("fps", {"off", "Hide the FPS on the bottom-left corner.",
                          AX_CALLBACK_2(Console::commandFpsSubCommandOnOff, this)});
}

void Console::createCommandHelp()
{
    addCommand({"help", "Print this message. Args: [ ]", AX_CALLBACK_2(Console::commandHelp, this)});
}

void Console::createCommandProjection()
{
    addCommand({"projection", "Change or print the current projection. Args: [-h | help | 2d | 3d | ]",
                AX_CALLBACK_2(Console::commandProjection, this)});
    addSubCommand("projection", {"2d", "sets a 2D projection (orthogonal projection).",
                                 AX_CALLBACK_2(Console::commandProjectionSubCommand2d, this)});
    addSubCommand("projection", {"3d", "sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.",
                                 AX_CALLBACK_2(Console::commandProjectionSubCommand3d, this)});
}

void Console::createCommandResolution()
{
    addCommand({"resolution",
                "Change or print the window resolution. Args: [-h | help | width height resolution_policy | ]",
                AX_CALLBACK_2(Console::commandResolution, this)});
    addSubCommand("resolution", {"", "", AX_CALLBACK_2(Console::commandResolutionSubCommandEmpty, this)});
}

void Console::createCommandSceneGraph()
{
    addCommand({"scenegraph", "Print the scene graph", AX_CALLBACK_2(Console::commandSceneGraph, this)});
}

void Console::createCommandTexture()
{
    addCommand({"texture", "Flush or print the TextureCache info. Args: [-h | help | flush | ] ",
                AX_CALLBACK_2(Console::commandTextures, this)});
    addSubCommand("texture", {"flush", "Purges the dictionary of loaded textures.",
                              AX_CALLBACK_2(Console::commandTexturesSubCommandFlush, this)});
}

void Console::createCommandTouch()
{
    addCommand({"touch", "simulate touch event via console, type -h or [touch help] to list supported directives"});
    addSubCommand("touch", {"tap", "touch tap x y: simulate touch tap at (x,y).",
                            AX_CALLBACK_2(Console::commandTouchSubCommandTap, this)});
    addSubCommand("touch", {"swipe", "touch swipe x1 y1 x2 y2: simulate touch swipe from (x1,y1) to (x2,y2).",
                            AX_CALLBACK_2(Console::commandTouchSubCommandSwipe, this)});
}

void Console::createCommandUpload()
{
    addCommand(
        {"upload", "upload file. Args: [filename base64_encoded_data]", AX_CALLBACK_1(Console::commandUpload, this)});
}

void Console::createCommandVersion()
{
    addCommand({"version", "print version string ", AX_CALLBACK_2(Console::commandVersion, this)});
}

//
// commands
//

void Console::commandAllocator(socket_native_type fd, std::string_view /*args*/)
{
#if AX_ENABLE_ALLOCATOR_DIAGNOSTICS
    auto info = allocator::AllocatorDiagnostics::instance()->diagnostics();
    Console::Utility::mydprintf(fd, info.c_str());
#else
    Console::Utility::mydprintf(
        fd, "allocator diagnostics not available. AX_ENABLE_ALLOCATOR_DIAGNOSTICS must be set to 1 in ccConfig.h\n");
#endif
}

void Console::commandConfig(socket_native_type fd, std::string_view /*args*/)
{
    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread([=]() {
        Console::Utility::mydprintf(fd, "%s", Configuration::getInstance()->getInfo().c_str());
        Console::Utility::sendPrompt(fd);
    });
}

void Console::commandDebugMsg(socket_native_type fd, std::string_view /*args*/)
{
    Console::Utility::mydprintf(fd, "Debug message is: %s\n", _sendDebugStrings ? "on" : "off");
}

void Console::commandDebugMsgSubCommandOnOff(socket_native_type /*fd*/, std::string_view args)
{
    _sendDebugStrings = (args.compare("on") == 0);
}

void Console::commandDirectorSubCommandPause(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director    = Director::getInstance();
    Scheduler* sched = director->getScheduler();
    sched->runOnAxmolThread([]() { Director::getInstance()->pause(); });
}

void Console::commandDirectorSubCommandResume(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director = Director::getInstance();
    director->resume();
}

void Console::commandDirectorSubCommandStop(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director    = Director::getInstance();
    Scheduler* sched = director->getScheduler();
    sched->runOnAxmolThread([]() { Director::getInstance()->stopAnimation(); });
}

void Console::commandDirectorSubCommandStart(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director = Director::getInstance();
    director->startAnimation();
}

void Console::commandDirectorSubCommandEnd(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director = Director::getInstance();
    director->end();
}

void Console::commandExit(socket_native_type fd, std::string_view /*args*/)
{
    _watcher.mod_event(fd, 0, yasio::socket_event::read);
    _fds.erase(std::remove(_fds.begin(), _fds.end(), fd), _fds.end());
    closesocket(fd);
}

void Console::commandFileUtils(socket_native_type fd, std::string_view /*args*/)
{
    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread(std::bind(&Console::printFileUtils, this, fd));
}

void Console::commandFileUtilsSubCommandFlush(socket_native_type /*fd*/, std::string_view /*args*/)
{
    FileUtils::getInstance()->purgeCachedEntries();
}

void Console::commandFps(socket_native_type fd, std::string_view /*args*/)
{
    Console::Utility::mydprintf(fd, "FPS is: %s\n", Director::getInstance()->isStatsDisplay() ? "on" : "off");
}

void Console::commandFpsSubCommandOnOff(socket_native_type /*fd*/, std::string_view args)
{
    bool state       = (args.compare("on") == 0);
    Director* dir    = Director::getInstance();
    Scheduler* sched = dir->getScheduler();
    sched->runOnAxmolThread(std::bind(&Director::setStatsDisplay, dir, state));
}

void Console::commandHelp(socket_native_type fd, std::string_view /*args*/)
{
    sendHelp(fd, _commands, "\nAvailable commands:\n");
}

void Console::commandProjection(socket_native_type fd, std::string_view /*args*/)
{
    auto director = Director::getInstance();
    char buf[20];
    auto proj = director->getProjection();
    switch (proj)
    {
    case ax::Director::Projection::_2D:
        snprintf(buf, sizeof(buf), "2d");
        break;
    case ax::Director::Projection::_3D:
        snprintf(buf, sizeof(buf), "3d");
        break;
    case ax::Director::Projection::CUSTOM:
        snprintf(buf, sizeof(buf), "custom");
        break;

    default:
        snprintf(buf, sizeof(buf), "unknown");
        break;
    }
    Console::Utility::mydprintf(fd, "Current projection: %s\n", buf);
}

void Console::commandProjectionSubCommand2d(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director    = Director::getInstance();
    Scheduler* sched = director->getScheduler();
    sched->runOnAxmolThread([=]() { director->setProjection(Director::Projection::_2D); });
}

void Console::commandProjectionSubCommand3d(socket_native_type /*fd*/, std::string_view /*args*/)
{
    auto director    = Director::getInstance();
    Scheduler* sched = director->getScheduler();
    sched->runOnAxmolThread([=]() { director->setProjection(Director::Projection::_3D); });
}

void Console::commandResolution(socket_native_type /*fd*/, std::string_view args)
{
    int policy;
    float width, height;
    std::stringstream stream;
    stream << args;
    stream >> width >> height >> policy;

    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread([=]() {
        Director::getInstance()->getGLView()->setDesignResolutionSize(width, height,
                                                                      static_cast<ResolutionPolicy>(policy));
    });
}

void Console::commandResolutionSubCommandEmpty(socket_native_type fd, std::string_view /*args*/)
{
    auto director        = Director::getInstance();
    Vec2 points          = director->getWinSize();
    Vec2 pixels          = director->getWinSizeInPixels();
    auto glView          = director->getGLView();
    Vec2 design          = glView->getDesignResolutionSize();
    ResolutionPolicy res = glView->getResolutionPolicy();
    Rect visibleRect     = glView->getVisibleRect();

    Console::Utility::mydprintf(fd,
                                "Window size:\n"
                                "\t%d x %d (points)\n"
                                "\t%d x %d (pixels)\n"
                                "\t%d x %d (design resolution)\n"
                                "Resolution Policy: %d\n"
                                "Visible Rect:\n"
                                "\torigin: %d x %d\n"
                                "\tsize: %d x %d\n",
                                (int)points.width, (int)points.height, (int)pixels.width, (int)pixels.height,
                                (int)design.width, (int)design.height, (int)res, (int)visibleRect.origin.x,
                                (int)visibleRect.origin.y, (int)visibleRect.size.width, (int)visibleRect.size.height);
}

void Console::commandSceneGraph(socket_native_type fd, std::string_view /*args*/)
{
    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread(std::bind(&Console::printSceneGraphBoot, this, fd));
}

void Console::commandTextures(socket_native_type fd, std::string_view /*args*/)
{
    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread([=]() {
        Console::Utility::mydprintf(fd, "%s",
                                    Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
        Console::Utility::sendPrompt(fd);
    });
}

void Console::commandTexturesSubCommandFlush(socket_native_type /*fd*/, std::string_view /*args*/)
{
    Scheduler* sched = Director::getInstance()->getScheduler();
    sched->runOnAxmolThread([]() { Director::getInstance()->getTextureCache()->removeAllTextures(); });
}

void Console::commandTouchSubCommandTap(socket_native_type fd, std::string_view args)
{
    auto argv = Console::Utility::split(args, ' ');

    if ((argv.size() == 3) && (Console::Utility::isFloat(argv[1]) && Console::Utility::isFloat(argv[2])))
    {

        float x = (float)utils::atof(argv[1].c_str());
        float y = (float)utils::atof(argv[2].c_str());

        std::srand((unsigned)time(nullptr));
        _touchId         = rand();
        Scheduler* sched = Director::getInstance()->getScheduler();
        sched->runOnAxmolThread([&]() {
            Director::getInstance()->getGLView()->handleTouchesBegin(1, &_touchId, &x, &y);
            Director::getInstance()->getGLView()->handleTouchesEnd(1, &_touchId, &x, &y);
        });
    }
    else
    {
        const char msg[] = "touch: invalid arguments.\n";
        Console::Utility::sendToConsole(fd, msg, strlen(msg));
    }
}

void Console::commandTouchSubCommandSwipe(socket_native_type fd, std::string_view args)
{
    auto argv = Console::Utility::split(args, ' ');

    if ((argv.size() == 5) && (Console::Utility::isFloat(argv[1])) && (Console::Utility::isFloat(argv[2])) &&
        (Console::Utility::isFloat(argv[3])) && (Console::Utility::isFloat(argv[4])))
    {

        float x1 = (float)utils::atof(argv[1].c_str());
        float y1 = (float)utils::atof(argv[2].c_str());
        float x2 = (float)utils::atof(argv[3].c_str());
        float y2 = (float)utils::atof(argv[4].c_str());

        std::srand((unsigned)time(nullptr));
        _touchId = rand();

        Scheduler* sched = Director::getInstance()->getScheduler();
        sched->runOnAxmolThread([x1, y1, this]() {
            float tempx = x1, tempy = y1;
            Director::getInstance()->getGLView()->handleTouchesBegin(1, &_touchId, &tempx, &tempy);
        });

        float dx  = std::abs(x1 - x2);
        float dy  = std::abs(y1 - y2);
        float _x_ = x1, _y_ = y1;
        if (dx > dy)
        {
            while (dx > 1)
            {

                if (x1 < x2)
                {
                    _x_ += 1;
                }
                if (x1 > x2)
                {
                    _x_ -= 1;
                }
                if (y1 < y2)
                {
                    _y_ += dy / dx;
                }
                if (y1 > y2)
                {
                    _y_ -= dy / dx;
                }
                sched->runOnAxmolThread([_x_, _y_, this]() {
                    float tempx = _x_, tempy = _y_;
                    Director::getInstance()->getGLView()->handleTouchesMove(1, &_touchId, &tempx, &tempy);
                });
                dx -= 1;
            }
        }
        else
        {
            while (dy > 1)
            {
                if (x1 < x2)
                {
                    _x_ += dx / dy;
                }
                if (x1 > x2)
                {
                    _x_ -= dx / dy;
                }
                if (y1 < y2)
                {
                    _y_ += 1;
                }
                if (y1 > y2)
                {
                    _y_ -= 1;
                }
                sched->runOnAxmolThread([_x_, _y_, this]() {
                    float tempx = _x_, tempy = _y_;
                    Director::getInstance()->getGLView()->handleTouchesMove(1, &_touchId, &tempx, &tempy);
                });
                dy -= 1;
            }
        }

        sched->runOnAxmolThread([x2, y2, this]() {
            float tempx = x2, tempy = y2;
            Director::getInstance()->getGLView()->handleTouchesEnd(1, &_touchId, &tempx, &tempy);
        });
    }
    else
    {
        const char msg[] = "touch: invalid arguments.\n";
        Console::Utility::sendToConsole(fd, msg, strlen(msg));
    }
}

static char invalid_filename_char[] = {':', '/', '\\', '?', '%', '*', '<', '>', '"', '|', '\r', '\n', '\t'};

void Console::commandUpload(socket_native_type fd)
{
    ssize_t n, rc;
    char buf[512] = {0};
    char c        = 0;
    char* ptr     = buf;
    // read file name
    for (n = 0; n < sizeof(buf) - 1; n++)
    {
        if ((rc = recv(fd, &c, 1, 0)) == 1)
        {
            for (char x : invalid_filename_char)
            {
                if (c == x)
                {
                    const char err[] = "upload: invalid file name!\n";
                    Console::Utility::sendToConsole(fd, err, strlen(err));
                    return;
                }
            }
            if (c == ' ')
            {
                break;
            }
            *ptr++ = c;
        }
        else if (rc == 0)
        {
            break;
        }
        else if (errno == EINTR)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    *ptr = 0;

    static std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string filepath            = writablePath + std::string(buf);

    auto fs = FileUtils::getInstance()->openFileStream(filepath, IFileStream::Mode::WRITE);
    if (!fs)
    {
        const char err[] = "can't create file!\n";
        Console::Utility::sendToConsole(fd, err, strlen(err));
        return;
    }

    while (true)
    {
        char data[4];
        for (int i = 0; i < 4; i++)
        {
            data[i] = '=';
        }
        bool more_data;
        readBytes(fd, data, 4, &more_data);
        if (!more_data)
        {
            break;
        }
        unsigned char* decode;
        unsigned char* in = (unsigned char*)data;
        int dt            = utils::base64Decode(in, 4, &decode);
        if (dt > 0)
        {
            fs->write(decode, dt);
        }
        free(decode);
    }
}

void Console::commandVersion(socket_native_type fd, std::string_view /*args*/)
{
    Console::Utility::mydprintf(fd, "%s\n", axmolVersion());
}

// helper free functions

int Console::printSceneGraph(socket_native_type fd, Node* node, int level)
{
    int total = 1;
    for (int i = 0; i < level; ++i)
        Console::Utility::sendToConsole(fd, "-", 1);

    Console::Utility::mydprintf(fd, " %s\n", node->getDescription().c_str());

    for (const auto& child : node->getChildren())
        total += printSceneGraph(fd, child, level + 1);

    return total;
}

void Console::printSceneGraphBoot(socket_native_type fd)
{
    Console::Utility::sendToConsole(fd, "\n", 1);
    auto scene = Director::getInstance()->getRunningScene();
    int total  = printSceneGraph(fd, scene, 0);
    Console::Utility::mydprintf(fd, "Total Nodes: %d\n", total);
    Console::Utility::sendPrompt(fd);
}

void Console::printFileUtils(socket_native_type fd)
{
    FileUtils* fu = FileUtils::getInstance();

    Console::Utility::mydprintf(fd, "\nSearch Paths:\n");
    auto& list = fu->getSearchPaths();
    for (const auto& item : list)
    {
        Console::Utility::mydprintf(fd, "%s\n", item.c_str());
    }

    Console::Utility::mydprintf(fd, "\nWritable Path:\n");
    Console::Utility::mydprintf(fd, "%s\n", fu->getWritablePath().c_str());

    Console::Utility::mydprintf(fd, "\nFull Path Cache:\n");
    auto& cache = fu->getFullPathCache();
    for (const auto& item : cache)
    {
        Console::Utility::mydprintf(fd, "%s -> %s\n", item.first.c_str(), item.second.c_str());
    }
    Console::Utility::sendPrompt(fd);
}

void Console::sendHelp(socket_native_type fd, const hlookup::string_map<Command*>& commands, const char* msg)
{
    Console::Utility::sendToConsole(fd, msg, strlen(msg));
    for (auto&& it : commands)
    {
        auto command = it.second;
        if (command->getHelp().empty())
            continue;

        Console::Utility::mydprintf(fd, "\t%s", command->getName().data());
        ssize_t tabs = command->getName().length() / 8;
        tabs         = 3 - tabs;
        for (int j = 0; j < tabs; j++)
        {
            Console::Utility::mydprintf(fd, "\t");
        }
        Console::Utility::mydprintf(fd, "%s\n", command->getHelp().data());
    }
}

NS_AX_END
