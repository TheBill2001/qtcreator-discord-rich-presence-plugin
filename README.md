# Discord Rich Presence plugin for Qt Creator

This plugin enables Discord Rich Presence plugin for Qt Creator, utilizing Discord Game SDK.

![Screenshot](Screenshot.png)

## 1. Installing

## 2. Building
### 2.1 Requirement

This plugin requires [Discord Game SDK](https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip) and Qt Creator development package. For the SDK, you can download it from Discord, or install it from your distro repos. Additionally, Extra CMake Modules is required as a build-time dependency.

### 2.2 Installing dependencies

1. Arch Linux
    
    [Discord Game SDK](https://aur.archlinux.org/packages/discord-game-sdk) is available in the AUR. Qt Creator development package can be install from the `Extra` repo.
    ```
    pacman -S qtcreator-devel extra-cmake-modules
    ```

### 2.3 Configuration

Below are CMake variables that can affect how the plugin is built.

- `DISCORD_CLIENT_ID`: Set this variable to use your own client ID rather using the one provided.
- `DISCORD_GAMESDK_PATH`: Set this variable to use your own Discord Game SDK.
- `BUILD_PORTABLE`: Set this variable to build the plugin as portable with Discord Game SDK also deployed. If `DISCORD_GAMESDK_PATH` is not set, CMake will download a copy from Discord. On Windows and MacOS, this variable is always `ON`. On Linux, if your distro does not ship Discord Game SDK in its repos, it is recommended to turn on this variable.

### 2.3 Build steps
- Create and enter build directory.

   ```
   mkdir build
   cd build
   ```

- Build the program.

   ```
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make
   ```

### 2.4 Install

   ```
   make install
   ```
   
> **A note on installation:** You should not mix portable and non-portable build. Under Linux, you should also not install the portable build as a system package. For portable build, set `CMAKE_INSTAL_PATH` to a temporary folder, and install the plugin via Qt Creator's dialog by selecting the `.so` files.

### 2.5 Uninstall.
   ```
   make uninstall
   ```
   
## 5. License
The plugin is licensed under [GPL V3](LICENSE).
