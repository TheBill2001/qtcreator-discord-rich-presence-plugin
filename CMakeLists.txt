cmake_minimum_required(VERSION 3.24)

project(DiscordRichPresencePlugin VERSION 1.0.1 LANGUAGES CXX)

if(WIN32 OR (UNIX AND APPLE))
    option(BUILD_PORTABLE "Build plugin as portable with Discord Game SDK also deployed" ON)
    if(NOT BUILD_PORTABLE)
        set(BUILD_PORTABLE ON) # Force BUILD_PORTABLE to ON
    endif()
else()
    option(BUILD_PORTABLE "Build plugin as portable with Discord Game SDK also deployed" OFF)
endif()

set(DISCORD_CLIENT_ID "1346349331996999680"
    CACHE STRING "Discord API client ID."
)

if(NOT DISCORD_CLIENT_ID)
    message(FATAL_ERROR "Discord client ID is not found")
endif()

set(QT6_MIN_VERSION 6.8.0)
set(KF6_MIN_VERSION 6.8.0)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(KDEClangFormat)
include(KDEGitCommitHooks)

if(BUILD_PORTABLE AND NOT DISCORD_GAMESDK_PATH)
    include(FetchContent)
    FetchContent_Declare(DiscordGameSDKDownload
        URL "https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip"
    )
    FetchContent_MakeAvailable(DiscordGameSDKDownload)
    set(DISCORD_GAMESDK_PATH "${discordgamesdkdownload_SOURCE_DIR}")
endif()

find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED COMPONENTS
    Core
    LinguistTools
)

qt_standard_project_setup(
    REQUIRES ${QT6_MIN_VERSION}
    I18N_TRANSLATED_LANGUAGES
        vi
)

find_package(QtCreator REQUIRED COMPONENTS
    Core
    ProjectExplorer
)

find_package(DiscordGameSDK REQUIRED COMPONENTS)

configure_file(src/pluginconstants.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/plugin/pluginconstants.hpp" @ONLY)

add_qtc_plugin(${PROJECT_NAME}
    PLUGIN_DEPENDS
        QtCreator::Core
        QtCreator::ProjectExplorer
    DEPENDS
        QtCreator::ExtensionSystem
        QtCreator::Utils
    SOURCES
        src/plugincommon.hpp src/pluginconstants.hpp.in
        src/plugin.hpp src/plugin.cpp
)

ecm_setup_version("${PROJECT_VERSION}"
    VARIABLE_PREFIX QVANCED
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/plugin/version.hpp"
)

ecm_qt_declare_logging_category(${PROJECT_NAME}
    HEADER "${CMAKE_CURRENT_BINARY_DIR}/plugin/loggingcategory.hpp"
    IDENTIFIER "QTCREATOR_DRP_LOGGING"
    CATEGORY_NAME "com.thebill2001.qtcreator.discordrichpresence"
    DESCRIPTION "Qt Creator Discord Rich Presence plugin logs"
    EXPORT ${PROJECT_NAME}_logging
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_BINARY_DIR}/plugin"
        "${DiscordGameSDK_INCLUDE_DIR}"
)

qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lang
)

if(BUILD_PORTABLE)
    install(IMPORTED_RUNTIME_ARTIFACTS Discord::GameSDK
        LIBRARY DESTINATION "${IDE_PLUGIN_PATH}"
        RUNTIME DESTINATION "${IDE_PLUGIN_PATH}"
    )
endif()

if(LINUX OR (UNIX AND NOT APPLE))
    ecm_qt_install_logging_categories(
        EXPORT ${PROJECT_NAME}_logging
        DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
    )
endif()

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.hpp)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
