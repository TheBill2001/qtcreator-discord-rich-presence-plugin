set(DISCORD_GAMESDK_PATH "" CACHE STRING "Custom Discord Game SDK path.")

if(NOT DISCORD_GAMESDK_PATH STREQUAL "")
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" CMAKE_SYSTEM_PROCESSOR_LOWER)
    if(CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "(x86_64)|(amd64)")
        set(DiscordGameSDK_ARCH "x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "(x86)")
        set(DiscordGameSDK_ARCH "x86")
    elseif(CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "(aarch64)")
        set(DiscordGameSDK_ARCH "aarch64")
    else()
        message(FATAL_ERROR "Unknown system architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    set(DiscordGameSDK_LIBDIR_HINTS "${DISCORD_GAMESDK_PATH}/lib/${DiscordGameSDK_ARCH}")
    set(DiscordGameSDK_INCLUDE_DIR_HINTS "${DISCORD_GAMESDK_PATH}/c")
    set(DiscordGameSDK_Cpp_SOURCE_DIR_HINTS "${DISCORD_GAMESDK_PATH}/cpp")
    set(DiscordGameSDK_NO_CMAKE_SYSTEM_PATH NO_DEFAULT_PATH NO_CMAKE_SYSTEM_PATH)
endif()

find_library(DiscordGameSDK_LIBRARY
    NAMES discord_game_sdk discord_game_sdk.dll.lib discord_game_sdk.dylib discord_game_sdk.so
    HINTS "${DiscordGameSDK_LIBDIR_HINTS}"
    ${DiscordGameSDK_NO_CMAKE_SYSTEM_PATH}
)

find_path(DiscordGameSDK_INCLUDE_DIR
    NAMES discord_game_sdk.h
    HINTS "${DiscordGameSDK_INCLUDE_DIR_HINTS}"
    ${DiscordGameSDK_NO_CMAKE_SYSTEM_PATH}
)

find_path(DiscordGameSDK_Cpp_SOURCE_DIR
    NAMES discord.h
    HINTS "${DiscordGameSDK_Cpp_SOURCE_DIR_HINTS}"
    PATHS "/usr/src"
    PATH_SUFFIXES "discord-game-sdk"
    ${DiscordGameSDK_NO_CMAKE_SYSTEM_PATH}
)

if(DiscordGameSDK_Cpp_SOURCE_DIR)
    set(DiscordGameSDK_Cpp_FOUND YES)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DiscordGameSDK
    FOUND_VAR
        DiscordGameSDK_FOUND
    REQUIRED_VARS
        DiscordGameSDK_LIBRARY
        DiscordGameSDK_INCLUDE_DIR
    HANDLE_COMPONENTS
)

if(DiscordGameSDK_FOUND AND NOT TARGET Discord::GameSDK)
    add_library(Discord::GameSDK SHARED IMPORTED)
    if(WIN32)
        cmake_path(REMOVE_EXTENSION DiscordGameSDK_LIBRARY LAST_ONLY OUTPUT_VARIABLE DiscordGameSDK_DLL)
        if(NOT EXISTS "${DiscordGameSDK_DLL}")
            message(FATAL_ERROR "Missing library discord_game_sdk.dll")
        endif()
        set_target_properties(Discord::GameSDK PROPERTIES
            IMPORTED_LOCATION "${DiscordGameSDK_DLL}"
            IMPORTED_IMPLIB "${DiscordGameSDK_LIBRARY}"
        )
    else()
        set_target_properties(Discord::GameSDK PROPERTIES
            IMPORTED_LOCATION "${DiscordGameSDK_LIBRARY}"
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${DiscordGameSDK_INCLUDE_DIR}"
            # IMPORTED_NO_SONAME TRUE # IMPORTANT!!
        )
    endif()
endif()

if(DiscordGameSDK_Cpp_FOUND)
    # Copy to staging area to fix include dir
    file(COPY "${DiscordGameSDK_Cpp_SOURCE_DIR}/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/discord-game-sdk/discord-game-sdk"
        FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.cpp"
    )
    set(DiscordGameSDK_Cpp_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/discord-game-sdk/discord-game-sdk")
    set(DiscordGameSDK_Cpp_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/discord-game-sdk")

    # Fix header. Might only needed on Linux.
    file(READ ${DiscordGameSDK_Cpp_SOURCE_DIR}/types.h FILE_CONTENTS)
    string(FIND "${FILE_CONTENTS}" "#include <cstdint>" FILE_CONTENTS_OK)
    if("${FILE_CONTENTS_OK}" EQUAL "-1")
        string(REPLACE "#pragma once" "#pragma once\n#include <cstdint>" FILE_CONTENTS "${FILE_CONTENTS}")
        file(WRITE "${DiscordGameSDK_Cpp_SOURCE_DIR}/types.h" "${FILE_CONTENTS}")
    endif()

    file(GLOB_RECURSE DiscordGameSDK_Cpp_SOURCES "${DiscordGameSDK_Cpp_SOURCE_DIR}/*.cpp")
    file(GLOB_RECURSE DiscordGameSDK_Cpp_HEADERS "${DiscordGameSDK_Cpp_SOURCE_DIR}/*.h")
endif()

mark_as_advanced(
    DiscordGameSDK_INCLUDE_DIR
    DiscordGameSDK_LIBRARY
    DiscordGameSDK_Cpp_INCLUDE_DIR
    DiscordGameSDK_Cpp_HEADERS
    DiscordGameSDK_Cpp_SOURCES
)

include(FeatureSummary)
set_package_properties(DiscordGameSDK PROPERTIES
    URL "https://discord.com/developers/docs/developer-tools/game-sdk"
    DESCRIPTION "Discord Game SDK helps you develop your 3rd party game or app, and integrate it with Discord"
)
