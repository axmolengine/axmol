# Development setup

## Common Requirement [Python](https://www.python.org/downloads/)

- Python-3.7+
- PowerShell if you want use the simply build script `tools/ci/build.ps1`
  - win7+, no needs install the system preinstalled PowerShell 5.0 should works
  - macos,linux or you want install latest powershell for windows, visit: https://github.com/PowerShell/PowerShell/releases

## Prerequisites

  1. Enter `axmol` root directory
  2. Run `python setup.py`, restart the console after it has finished for environment variables to take effect

## Creating A New Project

Using a console window, an example of a command to generate a new project is as follows:

```axmol new -p YOUR.UNIQUE.ID -d PROJECT_PATH -l [cpp|lua] [--portrait] PROJECT_NAME```

Type `axmol new --help` at the command line for more options you can pass to `axmol new`

Examples:

- Cpp: `axmol new -p org.axmol.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `axmol new -p org.axmol.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

## Quick building your new project by `build.ps1` for all target platforms

If you use latest commits after 6/29/2023 18:50, once you crate a new project, there is a build script `build.ps1` in your project root directory,
then you can simply build your project for all platform targets, i.e.

- win32: `pwsh .\build.ps1 -p win32 -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on Windows with vs2022 installed
- winuwp: `pwsh .\build.ps1 -p winuwp -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on Windows with vs2022 installed
- linux: run the setup script `install-deps-linux.sh` once, then `pwsh .\build.ps1 -p linux -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on Linux with g++ installed
- osx: `pwsh .\build.ps1 -p osx -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on macOS with xcode13~14.2 installed
- android: `pwsh .\build.ps1 -p android -a arm64` can runs on Windows,Linux,macOS and script will auto setup android sdk
- ios: `pwsh .\build.ps1 -p ios -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on macOS with xcode13~14.2 installed
- tvos: `pwsh .\build.ps1 -p tvos -a x64 -xc '--config','Debug','--target','HelloCpp'` can runs on macOS with xcode13~14.2 installed

## Windows (Visual Studio)

  1. Install [CMake](https://cmake.org/)  3.22.1+  
  2. Install Visual Studio 2022 (VS2019 should be support, but VS2022 is recommended)  
  3. Create a new project as shown [here](#creating-a-new-project)
  4. In a console window, navigate into the root directory of the project you created in the previous step
  5. Generate the relevant Visual Studio project using the cmake command:
  
     ```cmake -S SOURCE_DIR -B BUILD_DIR -G VISUAL_STUDIO_VERSION_STRING -A [Win32|x64]```

     For example, say `SOURCE_DIR` is the current path `"."`, and `BUILD_DIR` (out-of-source build directory) is named `"build"`:

     a) C++ 20:
        - for 32 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
        - for 64 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
        - for 32 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
        - for 64 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```

     b) C++17
         - add `-DCMAKE_CXX_STANDARD=17` for C++17 on your command line like:
         cmake -S . -B build `-DCMAKE_CXX_STANDARD=17` -G "Visual Studio 17 2022" -A x64


  6. Use Visual Studio to open the newly created solution file. For example, `./build/ProjectName.sln`

### Windows UWP (Visual Studio 2022), because microsoft limit, only support C++17
  ```cmake -B build_uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0" "-DAX_VS_DEPLOYMENT_TARGET=10.0.17763.0"```  

#### Creating the Visual Studio solution for all axmol test projects (Win/UWP)

  - Perform steps 1.-6. or the Windows UWP step above (if not done)
  7. Open the solution (".\build\axmol.sln" or ".\build_uwp\axmol.sln") in Visual Studio and build any of the test projects via the IDE.

### Improve 'Visual Studio' workflow, support linking with engine prebuilt libs

See [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)

## Android (Android Studio)

  1. Install Android Studio 2021.1.1+
  2. When starting Android Studio for the first time, it will guide you to install the SDK and other tools, so ensure that you do install them.
  3. Start Android and choose [Open an existing Android Studio Project] and select your project. For example, the existing cpp-test project located in ```axmol\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose the following tools and click the button ```Apply``` to install them:  
     - Android SDK Platform 33  
     - Android Gradle Plugin (AGP) 7.4.2  
     - Android SDK Build-Tools 30.0.3 match with AGP, refer to: <https://developer.android.com/studio/releases/gradle-plugin>
     - Gradle 8.1  
     - NDK r23c+  
     - CMake 3.22.1+  
  5. Wait for ```Gradle sync``` finish.
  6. Note: If you use non-sdk provided CMake edition, you will need to download ```ninja``` from <https://github.com/ninja-build/ninja/releases>, and copy ```ninja.exe``` to cmake's bin directory
  
## Android Studio (without Android Studio)

  1. Download command-tools from <https://developer.android.com/studio#command-tools>, for example: https://dl.google.com/android/repository/commandlinetools-win-9477386_latest.zip
  2. Install Android devtools (for example in windows)

  ```bat
  # unzip command-tools at D:\dev\adt\
  # Install android devtools
  cd D:\dev\adt\
  mkdir sdk
  .\cmdline-tools\bin\sdkmanager.bat --verbose --sdk_root=D:\dev\adt\sdk "platform-tools" "cmdline-tools;latest" "platforms;android-33" "build-tools;30.0.3" "cmake;3.22.1" "ndk;23.2.8568313"
  set ANDROID_HOME=D:\dev\adt\sdk
  
  # Goto xxx\proj.android
  .\gradlew.bat assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=arm64-v8a --parallel --info
  ```

## iOS, tvOS and macOS

  1. Ensure xcode12+ & [cmake3.21+](https://github.com/Kitware/CMake/releases) are installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Create a new project as shown [here](#creating-a-new-project)
  3. In a console window, navigate into the root directory of the project you created in the previous step
  4. Execute the following command
   ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```
  5. Generate the relevant xcode project using one of the following commands:
     - for ios arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS64```
     - for ios armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS```
     - for ios simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR64```
     - for tvos arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=TVOS```
     - for tvos simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR_TVOS```
     - for macos x86_64(Intel)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=x86_64```
     - for macos arm64(M1)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=arm64```

  6. After cmake finishes generating, you can open the xcode project at ```build``` folder and run cpp-tests or other test targets.  
  7. Notes  
     - **The code signing is required to run the ios/tvos app on your device, just change the bundle identifier until the auto manage signing is solved**  
     - **axmol only provides aarm64, x86_64 prebuilt libraries for ios/tvos**

## Linux (VSCode)

1. run `echo -e "y" | $AX_ROOT/install-deps-linux.sh`
2. Open axmol source folder with vscode
3. Install C++, CMake extensions for vscode
4. vscode will auto prompt you to choose toolset for building, just select gcc match with your system installed default gcc
    ```sh
    # check gcc version
    gcc -v
    ```
    i.e. `gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04)`
5. vscode will run cmake config automatically, after done, click `build` in taskbar, you can also choose specific target
  to build and run


Notes: if you needs debug by vscode, remember ensure choose `CMake: [Debug]` in vscode taskbar

