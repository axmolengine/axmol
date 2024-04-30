# Axmol Engine - Development setup

## Common requirements

- **PowerShell**: used to install Axmol. PowerShell 7 is recommended, it supports Windows, macOS and Linux.
  - Quick installation: 
     - macOS, Ubuntu, ArchLinux: run `setup.ps1` in `axmol` root directory (recommended).
     - Windows 10+: system installed PowerShell 5.x should work, but in that case you'll need to run the command `Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass -Force` in order to allow PowerShell script file to run.
  - Manual installation: [Instructions](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell) /  [Download](https://github.com/PowerShell/PowerShell/releases)
- **CMake 3.28.1+**
    - Manual installation is recommended ([download](https://cmake.org/download/)). Make sure to add CMake bin to the system `PATH`, otherwise `axmol build` will auto-setup it to `tools/external/cmake`.

## Prerequisites

  1. Download or clone Axmol from GitHub ([https://github.com/axmolengine/axmol](https://github.com/axmolengine/axmol)).
  2. Enter `axmol` root directory.
  3. Run `setup.ps1` in windows powershell or (macOS/Linux/ArchLinux terminal). Restart the console after it has finished for environment variables to take effect.
  4. Ensure that the C / C++ compiler toolset is installed on your host machine.
     - Windows: Visual Studio 2022 with desktop workflow
     - macOS: XCode 14.2+
     - Linux: GCC (G++)

## Creating a new project

Open any terminal(cmd.exe/powershell/pwsh/bash/zsh/WindowsTerminal/iTerm2). This is the command to generate a new project:

```axmol new -p YOUR.UNIQUE.ID -d PROJECT_PATH -l [cpp|lua] [--portrait] PROJECT_NAME```

Note: Avoid using special characters in `YOUR.UNIQUE.ID`

Type `axmol new --help` at the command line for the latest options. The options at the time of updating this document:
```
options:
    -h                  Show this help message and exit
    -p PACKAGE_NAME
                        Set a package name for project.
    -d DIRECTORY
                        Set the path where to place the new project.
    -l {cpp,lua}
                        Major programming language you want to use, should be [cpp | lua]
    --portrait
                        Set the project be portrait.
    -i[solated]
                        optionl, if present, will copy full engine sources to path/to/project/axmol
```

Examples:

- Cpp: `axmol new -p org.axmol.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `axmol new -p org.axmol.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

## Quick build for all target platforms (recommended)

The `axmol build` command will auto-setup the general toolsets, so you'll be able to easily build your project for any platform.

- Win32: 
  - To generate project and build in console: `axmol build -p win32`
  - To just generate project: `axmol build -p win32 -c`
- WinUWP: `axmol build -p winuwp`
  - To generate project and build in console: `axmol build -p winuwp`
  - To just generate project: `axmol build -p winuwp -c`
- Linux: `axmol build`
- OSX:
  - for Intel (x64): `axmol build -p osx -a x64 -c` (generate a xcodeproj, open with XCode to setup the code sign cert and build)
  - for Apple Silicon (arm64): `axmol build -p ios -a arm64 -c` (generate a xcodeproj, open with XCode to setup the code sign cert and build)
- Android: `axmol build -p android -a arm64` (can run on Windows, Linux and macOS, and script will auto setup Android SDK)
- iOS:
  - for devices: `axmol build -p ios -a arm64 -c` (generate a xcodeproj, open with XCode to setup the code sign cert and build)
  - for simulators:
      - for Intel (x64): `axmol build -p ios -a x64 -c`
      - for Apple Silicon (arm64): `axmol build -p ios -a arm64 -c`
- tvOS:
  - for devices: `axmol build -p tvos -a arm64 -c` (generate a xcodeproj, open with XCode to setup code sign cert and build)
  - for simulator: `axmol build -p tvos -a x64`
- WASM: `axmol build -p wasm` (it can run on Windows 8.1+, Linux and macOS, it requires a preinstalled [python3](https://www.python.org/) in env `PATH`)


### Supported options for `axmol build`

Type `axmol build --help` at the command line for the latest options. The options at the time of updating this document:
```
To print out the help in console: axmol build -h

options:
  -h: show this help message and exit
  -p: build target platform, valid value are: win32,winuwp(winrt),linux,android,osx,ios,tvos,wasm
      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first,
      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
  -xb: additional cross build options: i.e. -xb '--config','Release'
  -c: no build, only generate native project files (vs .sln, xcodeproj)
  -d: specify project dir to compile, i.e. -d /path/your/project/
  -f: force generate native project files. Useful if no changes are detected, such as with resource updates.
  -u: request upgrade prebuilt 3rd
 examples:
   - win32:
     - axmol [build] -p win32
     - axmol [build] -p win32 -cc clang
   - winuwp: axmol build -p winuwp
   - linux: axmol build -p linux
   - android:
     - axmol [build] -p android -a arm64
     - axmol [build] -p android -a 'arm64;x64'
   - osx:
     - axmol [build] -p osx -a x64
     - axmol [build] -p osx -a arm64
   - ios: axmol [build] -p ios -a x64
   - tvos: axmol [build] -p tvos -a x64
   - wasm: axmol [build] -p wasm
```

## How to quick build a test project (e.g. 'cpp-tests')

Using a PowerShell console window (command `pwsh`), go to `axmol\tests\<testdir e.g. 'cpp-tests'>` directory and perform `axmol build -p android -a arm64`. It will build `cpp-tests` for Android.

## Manually build with CMake

### Windows (Visual Studio 2022)

  1. Install [CMake 3.27.4+](https://cmake.org/download/).
  2. Install Visual Studio 2022 (VS 2019 should be supported, but VS 2022 is recommended).
  3. Create a new project as shown [here](#creating-a-new-project).
  4. In a console window, navigate into the root directory of the project you created in the previous step.
  5. Generate the relevant Visual Studio project using the cmake command:
  
     ```axmol build``` command described in the previous sections (preferred method)
     or
     ```cmake -S SOURCE_DIR -B BUILD_DIR -G VISUAL_STUDIO_VERSION_STRING -A [Win32|x64]```

     For example, let's say `SOURCE_DIR` is the current path `"."`, and `BUILD_DIR` (out-of-source build directory) is named `"build"`:

     (Since Axmol 2.1 c++20 is required for all platforms)
        - 32 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
        - 64 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
        - 32 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
        - 64 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```

  6. Use Visual Studio to open the newly created solution file. For example, `./build/ProjectName.sln`.

#### Windows UWP (Visual Studio 2022)
    
~~It only supports C++17.~~ Since Axmol 2.1 migration to CppWinRT it has C++20 support.

  ```cmake -B build_uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0"```  

#### Creating the Visual Studio solution for all Axmol test projects (Win / UWP)

First, perform the steps 1. to 6., or the Windows UWP step above (if not is already done).

  7. Open the solution (".\build\axmol.sln" or ".\build_uwp\axmol.sln") in Visual Studio and build any of the test projects via the IDE.

#### Improving the Visual Studio workflow, supporting linking with engine prebuilt libraries

Please see the [Windows workflow guide](https://github.com/axmolengine/axmol/issues/564).

### Android (Android Studio)

  1. Install [Android Studio 2023.1.1+](https://developer.android.com/studio).
  2. When starting Android Studio for the first time, it will guide you through the installation of the SDK and other tools. Please make sure that you do install them.
  3. Start Android Studio and choose [Open an existing Android Studio Project] and select your project. For example, the existing `cpp-test` project located in `axmol\tests\cpp-tests\proj.android`.
  4. Start Android Studio and open 'Tools' -> 'SDKManager', then switch to 'SDK Tools', check the 'Show Package Details' field, and choose the following tools clicking the button 'Apply' to install them:  
     - Android SDK Platform 34  
     - Android Gradle Plugin (AGP) 8.2.1  
     - Android SDK Build-Tools 34.0.0 match with AGP, refer to: <https://developer.android.com/studio/releases/gradle-plugin>
     - Gradle 8.5  
     - NDK r23c  
  5. Wait for the `Gradle sync` to finish.

Note: if you use non-SDK provided CMake, you will need to download `ninja` from <https://github.com/ninja-build/ninja/releases>, and copy `ninja.exe` to CMake's bin directory.
  
### Android Studio (without Android Studio)

  1. Download [Android command-tools](https://developer.android.com/studio#command-tools).
  2. Install Android devtools. Example in Windows:

  ```bat
  # unzip command-tools at D:\dev\adt\
  # Install android devtools
  cd D:\dev\adt\
  mkdir sdk
  .\cmdline-tools\bin\sdkmanager.bat --verbose --sdk_root=D:\dev\adt\sdk "platform-tools" "cmdline-tools;latest" "platforms;android-34" "build-tools;34.0.0" "ndk;23.2.8568313"
  set ANDROID_HOME=D:\dev\adt\sdk
  
  # Goto xxx\proj.android
  .\gradlew.bat assembleRelease -P__1K_ARCHS=arm64-v8a --parallel --info
  ```

### iOS, tvOS and macOS

  1. Ensure that XCode 13+ is installed.
  2. Create a new project as shown [here](#creating-a-new-project).
  3. In a console window, navigate into the root directory of the project you created in the previous step.
  4. Execute the following command:
   ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```
  5. Generate the relevant XCode project using one of the following commands:
     - for iOS arm64:  
     ```axmol build -p ios -a arm64 -c```
     - for iOS simulator x86_64:  
     ```axmol build -p ios -a x64 -c```
     - for tvOS arm64:  
     ```axmol build -p tvos -a arm64 -c```
     - for tvOS simulator x86_64:  
     ```axmol build -p tvos -a x64 -c```
     - for macOS x86_64(Intel)
     ```axmol build -p osx -c```
     - for macOS arm64(M1)
     ```axmol build -p osx -a arm64 -c```

  6. After CMake finishes generating, you can open the XCode project at `build_${plat}_${arch}` folder and run cpp-tests or other test targets. For OSC x64 should be `build_x64`.
  7. Notes:  
     - **Code signing is required to run the iOS / tvOS app on your device. Just change the bundle identifier until the auto manage signing is solved.**  
     - **Axmol only provides arm64, x86_64 prebuilt libraries for iOS / tvOS.**

### Linux (VSCode)

1. Run `pwsh ./setup.ps1`.
2. Open axmol source folder with VSCode.
3. Install C++, CMake extensions for VSCode.
4. VSCode will auto prompt you to choose the toolset for building. Select the gcc matching with your system installed default gcc:
    ```sh
    # check gcc version
    gcc -v
    ```
    i.e. `gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04)`
5. VSCode will run CMake config automatically. After done, click `build` in taskbar. You can also choose specific target
  to build and run.

Notes: if you need debug in VSCode, remember to choose `CMake: [Debug]` in the WSCode taskbar.
