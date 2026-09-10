/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#if defined(_WIN32)

// A inline helper class for anyone want enable win32 console in main.cpp
class EmbedConsole
{
public:
    EmbedConsole()
    {
        ::AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        ::SetConsoleOutputCP(CP_UTF8);

        configConsoleMode(STD_OUTPUT_HANDLE);
        configConsoleMode(STD_ERROR_HANDLE);
    }

    ~EmbedConsole() { ::FreeConsole(); }

private:
    void configConsoleMode(DWORD id)
    {
        auto handle = ::GetStdHandle(id);
        if (handle)
        {
            DWORD mode{};
            ::GetConsoleMode(handle, &mode);
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            ::SetConsoleMode(handle, mode);
        }
    }
};

EmbedConsole __win32_embed_console;

#endif
