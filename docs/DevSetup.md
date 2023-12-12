# Development setup

## Common Requirements

- [PowerShell](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell)
  - powershell-7 is recommended, it's support Windows,macOS,Linux
  - How to Install:
      - Quick Install 
          - macOS, Ubuntu, ArchLinux: you can simply run `install-pwsh.sh` in axmol root directory [`Recommended`]
          - win10+, system installed PowerShell 5.x should works, but you need run command `Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass -Force` to allow run powershell script file
      - Manually Install: [installing-powershell](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell) or download from [github release](https://github.com/PowerShell/PowerShell/releases)

## Prerequisites

  1. Enter `axmol` root directory
  2. Run `pwsh setup.ps1`, restart the console after it has finished for environment variables to take effect
  3. Ensure C/C++ compiler toolset installed on your host machine
     - Windows: visual studio 2022 with desktop workflow
     - macOS: xcode14.2
     - Linux: GCC(G++)

## Creating A New Project

Using a powershell console window, an example of a command to generate a new project is as follows:

```axmol new -p YOUR.UNIQUE.ID -d PROJECT_PATH -l [cpp|lua] [--portrait] PROJECT_NAME```

Type `axmol new --help` at the command line for more options you can pass to `axmol new`

Examples:

- Cpp: `axmol new -p org.axmol.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `axmol new -p org.axmol.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

## Quick build your new project by `axmol build` for all target platforms [`Recommended`]

The `axmol build` command will auto setup general depended toolsets, so you can simply build your project for all platform targets, i.e.

- win32: `axmol build -p win32`
- winuwp: `axmol build -p winuwp`
- linux: `axmol build`
- osx: `axmol build -p osx -a x64`
- android: `axmol build -p android -a arm64` can runs on Windows,Linux,macOS and script will auto setup android sdk
- ios:
  - for device: `axmol build -p ios -a arm64 -c`, generate xcodeproj, open by xcode to setup code sign cert and build
  - for simulator: `axmol build -p ios -a x64`
- tvos:
  - for device: `axmol build -p tvos -a arm64 -c`, generate xcodeproj, open by xcode to setup code sign cert and build
  - for simulator: `axmol build -p tvos -a x64`
- wasm: `axmol build -p wasm` can runs on Windows 8.1+, Linux, macOS, require a preinstalled [python3](https://www.python.org/) should be in env `PATH`

### Supported options for `axmol build`
```
-p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos,wasm
    for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
    if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
-a: build arch: x86,x64,armv7,arm64
-d: the build workspace, i.e project root which contains root CMakeLists.txt, empty use script run working directory aka cwd
-cc: The C/C++ compiler toolchain: clang, msvc, gcc(mingw) or empty use default installed on current OS
    msvc: msvc-120, msvc-141
    ndk: ndk-r16b, ndk-r16b+
-xt: cross build tool, default: cmake, for android can be gradlew, can be path of cross build tool program
-xc: cross build tool configure options: i.e.  -xc '-Dbuild'
-xb: cross build tool build options: i.e. -xb '--config','Release'
-prefix: the install location for missing tools in system, default is "$HOME/build1k"
-sdk: specific windows sdk version, i.e. -sdk '10.0.19041.0', leave empty, cmake will auto choose latest avaiable
-setupOnly: this param present, only execute step: setup 
-configOnly: if this param present, will skip build step
```

## Quick build engine for host targets?

Goto axmol root directory, double click or run `build.ps1` without any parameters, it will build `HelloCpp` by default

## Quick build a test project e.g. 'cpp-tests'?
Using a powershell console window, goto `axmol\tests\<testdir e.g. 'cpp-tests'>` directory, perform`pwsh ../../build.ps1  -p android -a arm64`, it will build `cpp-tests` for android.

## Manually build with cmake

### Windows (Visual Studio)

  1. Install [CMake](https://cmake.org/)  3.27.4+  
  2. Install Visual Studio 2022 (VS2019 should be support, but VS2022 is recommended)  
  3. Create a new project as shown [here](#creating-a-new-project)
  4. In a console window, navigate into the root directory of the project you created in the previous step
  5. Generate the relevant Visual Studio project using the cmake command:
  
     ```cmake -S SOURCE_DIR -B BUILD_DIR -G VISUAL_STUDIO_VERSION_STRING -A [Win32|x64]```

     For example, say `SOURCE_DIR` is the current path `"."`, and `BUILD_DIR` (out-of-source build directory) is named `"build"`:

     since axmol-2.1 c++20 required for all platforms
        - for 32 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
        - for 64 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
        - for 32 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
        - for 64 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```

  6. Use Visual Studio to open the newly created solution file. For example, `./build/ProjectName.sln`

#### Windows UWP (Visual Studio 2022), ~because microsoft limit, only support C++17~, since axmol-2.1, migrated to cppwinrt, so c++20 supportedn
  ```cmake -B build_uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0" "-DAX_VS_DEPLOYMENT_TARGET=10.0.17763.0"```  

##### Creating the Visual Studio solution for all axmol test projects (Win/UWP)

  - Perform steps 1.-6. or the Windows UWP step above (if not done)
  7. Open the solution (".\build\axmol.sln" or ".\build_uwp\axmol.sln") in Visual Studio and build any of the test projects via the IDE.

#### Improve 'Visual Studio' workflow, support linking with engine prebuilt libs

See [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)

### Android (Android Studio)

  1. Install Android Studio 2022.3.1+
  2. When starting Android Studio for the first time, it will guide you to install the SDK and other tools, so ensure that you do install them.
  3. Start Android and choose [Open an existing Android Studio Project] and select your project. For example, the existing cpp-test project located in ```axmol\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose the following tools and click the button ```Apply``` to install them:  
     - Android SDK Platform 33  
     - Android Gradle Plugin (AGP) 8.1.1  
     - Android SDK Build-Tools 33.0.1 match with AGP, refer to: <https://developer.android.com/studio/releases/gradle-plugin>
     - Gradle 8.3  
     - NDK r23c  
     - CMake 3.22.1+  
  5. Wait for ```Gradle sync``` finish.
  6. Note: If you use non-sdk provided CMake edition, you will need to download ```ninja``` from <https://github.com/ninja-build/ninja/releases>, and copy ```ninja.exe``` to cmake's bin directory
  
### Android Studio (without Android Studio)

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

### iOS, tvOS and macOS

  1. Install [CMake](https://cmake.org/)  3.28+  
  2. Ensure xcode 13+ are installed
  3. Create a new project as shown [here](#creating-a-new-project)
  4. In a console window, navigate into the root directory of the project you created in the previous step
  5. Execute the following command
   ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```
  6. Generate the relevant xcode project using one of the following commands:
     - for ios arm64:  
     ```axmol build -p ios -a arm64 -c```
     - for ios simulator x86_64:  
     ```axmol build -p ios -a x64 -c```
     - for tvos arm64:  
     ```axmol build -p tvos -a arm64 -c```
     - for tvos simulator x86_64:  
     ```axmol build -p tvos -a x64 -c```
     - for macos x86_64(Intel)
     ```axmol build -p osx -c```
     - for macos arm64(M1)
     ```axmol build -p osx -a arm64 -c```

  7. After cmake finishes generating, you can open the xcode project at ```build_${plat}_${arch}``` folder and run cpp-tests or other test targets, for osx x64 should be `build_x64`
  8. Notes  
     - **The code signing is required to run the ios/tvos app on your device, just change the bundle identifier until the auto manage signing is solved**  
     - **axmol only provides arm64, x86_64 prebuilt libraries for ios/tvos**

### Linux (VSCode)

1. run `pwsh ./setup.ps1`
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

