/*
no c/c++ objc files is resource file, needs sync manually in cpp-tests
processing file 1: /home/vmroot/dev/axmol/tests/cpp-tests/Content/extensions/CCControlColourPickerSpriteSheet.plist,
len=4137 processing file 2:
/home/vmroot/dev/axmol/tests/cpp-tests/Content/extensions/CCControlColourPickerSpriteSheet.png, len=38149 processing
file 3: /home/vmroot/dev/axmol/tests/cpp-tests/Content/hd/extensions/CCControlColourPickerSpriteSheet.plist, len=2860
processing file 4: /home/vmroot/dev/axmol/tests/cpp-tests/Content/hd/extensions/CCControlColourPickerSpriteSheet.png,
len=83570
*/

// ./core/**/*.h,./core/**/*.cpp,./core/**/*.inl,./core/**/*.mm,./core/**/*.m
// ./extensions/**/*.h,./extensions/**/*.cpp,./extensions/**/*.inl,./extensions/**/*.mm,./extensions/**/*.m
// ./tests/**/*.h,./tests/**/*.cpp,./tests/**/*.inl,./tests/**/*.mm,./tests/**/*.m
// ./templates/**/*.h,./templates/**/*.cpp,./templates/**/*.inl,./templates/**/*.mm,./templates/**/*.m
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include "yasio/stl/string_view.hpp"


namespace stdfs = std::filesystem;

#define O_READ_FLAGS O_RDONLY
#define O_WRITE_FLAGS O_CREAT | O_RDWR | O_TRUNC, S_IRWXU
#define O_APPEND_FLAGS O_APPEND | O_CREAT | O_RDWR, S_IRWXU

#define O_OVERLAP_FLAGS O_CREAT | O_RDWR, S_IRWXU
#define posix_open_cxx(path, ...) ::open(path.data(), ##__VA_ARGS__)

int totals = 0;
int replaced_totals = 0;
std::vector<std::string_view> chunks;

const std::regex include_re(R"(#(\s)*include(\s)*(.)*((CC)|(cc)))", std::regex_constants::ECMAScript);
const std::regex cmake_re(R"(/CC)", std::regex_constants::ECMAScript | std::regex_constants::icase);

std::string load_file(std::string_view path)
{
    auto fd = posix_open_cxx(path, O_READ_FLAGS);
    if (fd != -1)
    {
        struct auto_handle
        {
            ~auto_handle() { close(_fd); }
            int _fd;
        } _h{fd};
        size_t len = lseek(fd, 0, SEEK_END);
        if (len > 0)
        {
            lseek(fd, 0, SEEK_SET);
            std::string content;
            content.reserve(len);
            char buf[512];
            ssize_t nb = -1;
            while ((nb = read(fd, buf, sizeof(buf))) > 0)
            {
                content.append(buf, static_cast<size_t>(nb));
            }
            return content;
        }
    }
    return {};
}

void save_file(std::string_view path, const std::vector<std::string_view>& chunks) {
    auto fp = fopen(path.data(), "wb");
    if(!fp) {
        throw std::runtime_error("open file fail");
    }
    for(auto& chunk : chunks)
        fwrite(chunk.data(), chunk.length(), 1, fp);
    fclose(fp);
}

bool regex_search_for_replace(const std::string& content, const std::regex& re)
{
    // scan line by line, and put to chunks
    const char* cur_line = content.c_str();
    const char* ptr   = cur_line;

    int line_count = 0; // for stats only
    int hints = 0;

    chunks.clear();

    for(;;) {
        ++line_count;

        while (*ptr && *ptr != '\n')
            ++ptr;
        
        auto next_line = *ptr == '\n' ? ptr + 1 : ptr;
        std::string_view line{cur_line, next_line};  // ensure line contains '\n' if not '\0'

        if (line.length() > 1)
        {
            std::match_results<std::string_view::const_iterator> results;
            if (std::regex_search(line.begin(), line.end(), results, re))
            {
                auto& match = results[0];
                auto first = match.first;
                auto last = match.second;
                assert(first >= line.data() && first <= &line.back());
                std::string_view word{first, last};
                std::string_view chunk1{cur_line, last - 2};
                chunks.push_back(chunk1);
                auto chunk2_first = last;
                if(chunk2_first < next_line) {
                    std::string_view chunk2{chunk2_first, next_line};
                    chunks.push_back(chunk2);
                }
                ++hints;
            }
            else {
                chunks.push_back(line);
            }
        }
        else
        {  // put empty line directly
            chunks.push_back(line);
        }

        if (*next_line != '\0') {
            ptr = cur_line = next_line;
        }
        else {
            break;
        }
    }

    return !!hints;
}

void replace_cmake_lists(std::string& content) {}

void process_file(std::string_view file_path, std::string_view file_name, bool is_cmake)
{
    auto content = load_file(file_path);
    if (content.empty())
    {
        throw std::runtime_error("found empty file!");
    }

    
    if (!is_cmake)
    {
        if (cxx20::ic::ends_with(file_path, ".h") || cxx20::ic::ends_with(file_path, ".cpp") ||
            cxx20::ic::ends_with(file_path, ".mm") || cxx20::ic::ends_with(file_path, ".m") ||
            cxx20::ic::ends_with(file_path, ".inl"))
        {
            
            // replacing file include stub from CCxxx to xxx, do in editor is better
            if(regex_search_for_replace(content, include_re)) {
                printf("replacing c/c++,objc file %d: %s, len=%zu\n", ++totals, file_path.data(), content.size());
                save_file(file_path, chunks);
                ++replaced_totals;
            } else {
                printf("skipping c/c++,objc file %d: %s, len=%zu\n", ++totals, file_path.data(), content.size());
            }

            auto new_file_name = file_name.substr(2);
            std::string new_file_path {file_path.data(), file_path.length() - file_name.length()};
            new_file_path += new_file_name;

            // rename
            int ret = ::rename(file_path.data(), new_file_path.c_str());
            if (ret != 0) {
                throw std::runtime_error("rename file fail");
            }
        }
        else
        {
            printf("skip file %d: %s, len=%zu\n", ++totals, file_path.data(), content.size());
            return;
        }
    }
    else {
        if(regex_search_for_replace(content, cmake_re)) {
            printf("replacing cmake file %d: %s, len=%zu\n", ++totals, file_path.data(), content.size());
            save_file(file_path, chunks);
            ++replaced_totals;
        } else {
            printf("skip cmake %s not part of axmol engine!", file_path.data());
        }
    }
}

void process_folder(std::string_view sub_path)
{
    for (const auto& entry : stdfs::recursive_directory_iterator(sub_path))
    {
        const auto isDir = entry.is_directory();
        if (entry.is_regular_file())
        {
            auto& path    = entry.path();
            auto& strPath = path.native();
            auto pathname = path.filename();
            auto strName  = pathname.native();

            if (cxx20::ic::starts_with(strName, "CC"))
                process_file(strPath, strName, false);
            else if (cxx20::ic::ends_with(strPath, "CMakeLists.txt"))
                process_file(strPath, strName, true);
        }
    }
}

int main(int argc, const char** argv)
{
    auto axroot = getenv("AX_ROOT");
    if (!axroot)
    {
        perror("please run python setup.py first");
        return -1;
    }

    auto start = std::chrono::steady_clock::now();

    // 921 .h, .cpp, .mm, .m
    process_folder(std::string{axroot} + "/core");
    process_folder(std::string{axroot} + "/extensions");
    process_folder(std::string{axroot} + "/tests");

    auto diff = std::chrono::steady_clock::now() - start;
    printf("replaced totals: %d, total cost: %.3lf(ms)\n", replaced_totals, std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000.0);

    return 0;
}
