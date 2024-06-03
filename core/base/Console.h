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

#ifndef __CCCONSOLE_H__
#define __CCCONSOLE_H__
/// @cond DO_NOT_SHOW

#if defined(_MSC_VER) || defined(__MINGW32__)
#    include <basetsd.h>
#    ifndef __SSIZE_T
#        define __SSIZE_T
typedef SSIZE_T ssize_t;
#    endif  // __SSIZE_T
#endif

#include <thread>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <mutex>
#include <array>
#include <stdarg.h>
#include "yasio/io_watcher.hpp"

#include "base/Object.h"
#include "base/Macros.h"

NS_AX_BEGIN

/** Console is helper class that lets the developer control the game from TCP connection.
 Console will spawn a new thread that will listen to a specified TCP port.
 Console has a basic token parser. Each token is associated with an std::function<void(int)>.
 If the std::function<> needs to use the cocos2d API, it needs to call

 ```
 scheduler->runOnAxmolThread( ... );
 ```
 */

class AX_DLL Console : public Object
{
public:
    /** Console Utils */
    class Utility
    {
    public:
        // Trimming functions
        static std::string& ltrim(std::string& s);
        static std::string& rtrim(std::string& s);
        static std::string& trim(std::string& s);

        // split
        static std::vector<std::string>& split(std::string_view s, char delim, std::vector<std::string>& elems);
        static std::vector<std::string> split(std::string_view s, char delim);

        /** Checks myString is a floating-point type. */
        static bool isFloat(std::string_view myString);

        /** send a message to console */
        static ssize_t sendToConsole(int fd, const void* buffer, size_t length, int flags = 0);

        /** my dprintf() */
        static ssize_t mydprintf(int sock, const char* format, ...);

        /** send prompt string to console */
        static void sendPrompt(int fd);

        /** set a new string for the prompt. */
        static void setPrompt(std::string_view prompt);

        /** get the prompt string. */
        static std::string_view getPrompt();

    private:
        static std::string _prompt; /*!< prompt */
    };

    /** Command Struct */
    class AX_DLL Command
    {
    public:
        using Callback = std::function<void(int fd, std::string_view args)>;
        /** Constructor */
        Command();
        Command(std::string_view name, std::string_view help);
        Command(std::string_view name, std::string_view help, const Callback& callback);

        /** Copy constructor */
        Command(const Command& o);

        /** Move constructor */
        Command(Command&& o) noexcept;

        /** Destructor */
        ~Command();

        /** Copy operator */
        Command& operator=(const Command& o);

        /** Move operator */
        Command& operator=(Command&& o) noexcept;

        /** add callback */
        void addCallback(const Callback& callback);

        /** add sub command */
        void addSubCommand(const Command& subCmd);

        /** get sub command */
        const Command* getSubCommand(std::string_view subCmdName) const;

        /** delete sub command */
        void delSubCommand(std::string_view subCmdName);

        /** help command handler */
        void commandHelp(int fd, std::string_view args);

        /** generic command handler */
        void commandGeneric(int fd, std::string_view args);

        /** Gets the name of the current command */
        std::string_view getName() const { return _name; }

        /** Gets the help information of the current command */
        std::string_view getHelp() const { return _help; }

    private:
        std::string _name;
        std::string _help;

        Callback _callback;
        hlookup::string_map<Command*> _subCommands;
    };

    /** Constructor */
    Console();

    /** Destructor */
    virtual ~Console();

    /** starts listening to specified TCP port */
    bool listenOnTCP(int port);

    /** starts listening to specified file descriptor */
    bool listenOnFileDescriptor(int fd);

    /** stops the Console. 'stop' will be called at destruction time as well */
    void stop();

    /** add custom command */
    void addCommand(const Command& cmd);
    void addSubCommand(std::string_view cmdName, const Command& subCmd);
    void addSubCommand(Command& cmd, const Command& subCmd);

    /** get custom command */
    const Command* getCommand(std::string_view cmdName);
    const Command* getSubCommand(std::string_view cmdName, std::string_view subCmdName);
    const Command* getSubCommand(const Command& cmd, std::string_view subCmdName);

    /** delete custom command */
    void delCommand(std::string_view cmdName);
    void delSubCommand(std::string_view cmdName, std::string_view subCmdName);
    void delSubCommand(Command& cmd, std::string_view subCmdName);

    /**
     * set bind address
     *
     * @address : 127.0.0.1
     */
    void setBindAddress(std::string_view address);

    /** Checks whether the server for console is bound with ipv6 address */
    bool isIpv6Server() const;

    /** The command separator */
    AX_SYNTHESIZE(char, _commandSeparator, CommandSeparator);

protected:
    // Main Loop
    void loop();

    // Helpers
    ssize_t readline(socket_native_type fd, char* buf, size_t maxlen);
    ssize_t readBytes(socket_native_type fd, char* buffer, size_t maxlen, bool* more);
    bool parseCommand(socket_native_type fd);
    void performCommand(socket_native_type fd, std::string_view command);

    void addClient();

    // create a map of command.
    void createCommandAllocator();
    void createCommandConfig();
    void createCommandDebugMsg();
    void createCommandDirector();
    void createCommandExit();
    void createCommandFileUtils();
    void createCommandFps();
    void createCommandHelp();
    void createCommandProjection();
    void createCommandResolution();
    void createCommandSceneGraph();
    void createCommandTexture();
    void createCommandTouch();
    void createCommandUpload();
    void createCommandVersion();

    // Add commands here
    void commandAllocator(socket_native_type fd, std::string_view args);
    void commandConfig(socket_native_type fd, std::string_view args);
    void commandDebugMsg(socket_native_type fd, std::string_view args);
    void commandDebugMsgSubCommandOnOff(socket_native_type fd, std::string_view args);
    void commandDirectorSubCommandPause(socket_native_type fd, std::string_view args);
    void commandDirectorSubCommandResume(socket_native_type fd, std::string_view args);
    void commandDirectorSubCommandStop(socket_native_type fd, std::string_view args);
    void commandDirectorSubCommandStart(socket_native_type fd, std::string_view args);
    void commandDirectorSubCommandEnd(socket_native_type fd, std::string_view args);
    void commandExit(socket_native_type fd, std::string_view args);
    void commandFileUtils(socket_native_type fd, std::string_view args);
    void commandFileUtilsSubCommandFlush(socket_native_type fd, std::string_view args);
    void commandFps(socket_native_type fd, std::string_view args);
    void commandFpsSubCommandOnOff(socket_native_type fd, std::string_view args);
    void commandHelp(socket_native_type fd, std::string_view args);
    void commandProjection(socket_native_type fd, std::string_view args);
    void commandProjectionSubCommand2d(socket_native_type fd, std::string_view args);
    void commandProjectionSubCommand3d(socket_native_type fd, std::string_view args);
    void commandResolution(socket_native_type fd, std::string_view args);
    void commandResolutionSubCommandEmpty(socket_native_type fd, std::string_view args);
    void commandSceneGraph(socket_native_type fd, std::string_view args);
    void commandTextures(socket_native_type fd, std::string_view args);
    void commandTexturesSubCommandFlush(socket_native_type fd, std::string_view args);
    void commandTouchSubCommandTap(socket_native_type fd, std::string_view args);
    void commandTouchSubCommandSwipe(socket_native_type fd, std::string_view args);
    void commandUpload(socket_native_type fd);
    void commandVersion(socket_native_type fd, std::string_view args);
    // file descriptor: socket, console, etc.
    socket_native_type _listenfd;
    std::vector<socket_native_type> _fds;
    std::thread _thread;

    yasio::io_watcher _watcher;

    bool _running;
    bool _endThread;
    bool _isIpv6Server;

    hlookup::string_map<Command*> _commands;

    // strings generated by cocos2d sent to the remote console
    bool _sendDebugStrings;
    std::mutex _DebugStringsMutex;
    std::vector<std::string> _DebugStrings;

    intptr_t _touchId;

    std::string _bindAddress;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(Console);

    // helper functions
    int printSceneGraph(socket_native_type fd, Node* node, int level);
    void printSceneGraphBoot(socket_native_type fd);
    void printFileUtils(socket_native_type fd);

    /** send help message to console */
    static void sendHelp(socket_native_type fd, const hlookup::string_map<Command*>& commands, const char* msg);
};

NS_AX_END

/// @endcond
#endif /* defined(__CCCONSOLE_H__) */
