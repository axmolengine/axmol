
option(AX_ENABLE_HTTP "Build HTTP client based on yasio" ON)
option(AX_ENABLE_WEBSOCKET "Build Websocket client based on yasio" ON)

if(EMSCRIPTEN)
    set(_AX_NETWORK_HEADER
        network/Downloader-wasm.h
        network/IDownloaderImpl.h
        network/Downloader.h
        network/Uri.h
    )

    set(_AX_NETWORK_SRC
        network/Downloader.cpp
        network/Downloader-wasm.cpp
        network/Uri.cpp
    )

    if (AX_ENABLE_HTTP)
        list(APPEND _AX_NETWORK_HEADER
            network/HttpClient.h
            network/HttpResponse.h
            network/HttpRequest.h
            network/HttpCookie.h
        )

        list(APPEND _AX_NETWORK_SRC
            network/HttpClient-wasm.cpp
            network/HttpCookie.cpp
        )
    endif()

    if (AX_ENABLE_WEBSOCKET)
        list(APPEND _AX_NETWORK_HEADER
            network/WebSocket.h
        )

        list(APPEND _AX_NETWORK_SRC
            network/WebSocket-wasm.cpp
        )
    endif()

else()
    set(_AX_NETWORK_HEADER
        network/Downloader-curl.h
        network/IDownloaderImpl.h
        network/Downloader.h
        network/Uri.h
    )

    set(_AX_NETWORK_SRC
        network/Downloader.cpp
        network/Downloader-curl.cpp
        network/Uri.cpp
    )

    if (AX_ENABLE_HTTP)
        list(APPEND _AX_NETWORK_HEADER
            network/HttpClient.h
            network/HttpResponse.h
            network/HttpRequest.h
            network/HttpCookie.h
        )

        list(APPEND _AX_NETWORK_SRC
            network/HttpClient.cpp
            network/HttpCookie.cpp
        )
    endif()

    if (AX_ENABLE_WEBSOCKET)
        list(APPEND _AX_NETWORK_HEADER
            network/WebSocket.h
        )

        list(APPEND _AX_NETWORK_SRC
            network/WebSocket.cpp
        )
    endif()
endif()


