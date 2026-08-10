if(NOT ASIO_FOUND AND NOT ASIO_FIND_IN_PROGRESS)
    set(ASIO_FIND_IN_PROGRESS TRUE)
    CPMFindPackage(
        NAME asio
        GITHUB_REPOSITORY chriskohlhoff/asio
        GIT_TAG asio-1-32-0
        DOWNLOAD_ONLY YES
    )
    unset(ASIO_FIND_IN_PROGRESS)

    if(asio_ADDED)
        add_library(asio INTERFACE)
        target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
        add_library(ASIO::ASIO ALIAS asio)
        set(ASIO_FOUND TRUE)
    endif()
endif()