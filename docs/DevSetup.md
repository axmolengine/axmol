# Axmol Development Setup

## 🚀 Quick Start

### 1. **Install PowerShell 7**

- **Windows**
  - Windows 10+ includes **PowerShell 5.x** by default, which works.  
    👉 [PowerShell 7](https://github.com/powershell/powershell/releases) is recommended.
  - **Execution Policy (required on Windows)**  
    - For cloned Axmol repository:  
      ```pwsh
      Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned -Force
      ```
    - For downloaded ZIP release (recommended to verify MD5 or SHA256 digest after download):  
      ```pwsh
      Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass -Force
      ```
    ⚠️ Note: `RemoteSigned` is more secure than `Bypass`.

- **macOS / Linux**
  - Run `setup.ps1` directly. (TIPS: `setup.ps1` can also be executed in **bash mode** on macOS/Linux.)

### 2. **Install Compiler Toolchain**

- **Windows**: Install **Visual Studio 2022 or 2026** with the **`Desktop development with C++`** workload.  
  - For UWP development, also enable the **`WinUI application development tools`** workload and include the optional component **`C++ Universal Windows Platform tools`**.

- **macOS**:
  - For `axmol-v2`: Install **Xcode 14.2 or later** — note that Xcode 14.2 is only supported on **macOS 12.5 ~ macOS 13.x**.  
  - For `axmol-v3`: Install **Xcode 16.4 or later**, which requires **macOS 15.3 or newer**.

- **Linux**: Ensure GCC/G++ is installed (Ubuntu 22.04+, Debian 12+, ArchLinux).  

### 3. **Run the setup script**
```pwsh
./setup.ps1
```
This will install dependencies and configure environment variables. Restart your terminal after completion.

### 4. **Create a new project**
```pwsh
axmol new -p dev.axmol.hellocpp -d ./projects -l cpp HelloCpp
```
- `-l` can be `cpp` or `lua`
- `--portrait` optional, generates a portrait project

### 5. **Build the project**
```pwsh
axmol
```
By default, this builds the host target on your development machine.  
Use the `-p` option to cross‑compile for a different platform.

---

## 📦 Common Build Commands

| Platform               | Example Command |
|------------------------|-----------------|
| Windows                | `axmol -p win32` |
| UWP                    | `axmol -p winuwp` |
| Linux                  | `axmol -p linux` |
| macOS x64              | `axmol -p osx -a x64` |
| macOS arm64            | `axmol -p osx -a arm64` |
| iOS (simulator)        | `axmol -p ios -a x64 -sdk sim` |
| iOS (device)           | `axmol -p ios -a arm64 -c` |
| tvOS (simulator)       | `axmol -p tvos -a arm64 -sdk sim` |
| tvOS (device)          | `axmol -p tvos -a arm64 -c` |
| Android APK            | `axmol -p android -a arm64` |
| Android AAB            | `axmol -p android -a arm64 -aab` |
| WASM                   | `axmol -p wasm` |
| WASM64(requires v3)    | `axmol -p wasm64` |

#### 🔑 Common Parameters

- **`-p <platform>`**  
  Specify the target platform, e.g. `win32`, `linux`, `osx`, `ios`, `android`, `wasm`, etc.

- **`-a <arch>`**  
  Specify the target architecture, e.g. `x64`, `arm64`. This is especially important on multi‑architecture platforms such as macOS, iOS, tvOS, and Android.

- **`-c`**  
  Generate IDE project files (e.g. Visual Studio `.sln` or Xcode `.xcodeproj`). For iOS/tvOS real device builds this flag is mandatory because code signing is required.

- **`-sdk <name>`**  
  Specify the SDK type, e.g. `sim` for simulator builds.

- **`-aab`**  
  Generate an Android AAB package (required by Google Play). By default, APK is generated.

- **`-O<n>`**  
  Specify the optimization level for builds:  
  - `-O0` → Debug  
  - `-O1` → MinSizeRel  
  - `-O2` → RelWithDebInfo (default)  
  - `-O3` → Release  

#### 👉 Notes: 

- **You may notice that for iOS (device) and tvOS (device) we added the `-c` flag. This is required because real device builds must be code‑signed. At present, you need to open the generated Xcode project and configure signing manually.**
- **The `axmol` command is a shorthand for `axmol build`, use `axmol build -h` to see all available options and details.**
- **Wasm**: Supported on all development OS (macOS, Windows, Linux). Please ensure **Python 3.13 or later** is installed.  
  👉 Run `python -V` in the PowerShell terminal to verify your current Python version, then use `axmol run -p wasm` to build and automatically launch your game project in the browser.

---

## 🛠️ IDE Integration

Axmol CLI covers all build needs for most platforms.  
- For **Windows / Linux / macOS (simulator)**, IDEs are optional.  
- For **iOS/tvOS real devices**, IDEs are required because builds must be code‑signed in Xcode.

- **Visual Studio (Windows)**  
  Run `axmol -c`, then open the generated solution file:  
  - **VS2022** → `build/ProjectName.sln`  
  - **VS2026** → `build/ProjectName.slnx`

- **Xcode (macOS/iOS/tvOS)**  
  Run `axmol -p osx/ios/tvos -c`, then open the generated `.xcodeproj`  
  > ⚠️ For iOS/tvOS device builds, you must open the project in Xcode and configure signing manually.

- **Android Studio**  
  Run `axmol -p android -c`, then open `proj.android`

---

## 📦 Build Signed Release (APK / AAB)

Useful for CI/CD workflows:  
```pwsh
axmol -p android -a arm64 -xc "-PKEY_STORE_FILE=/path/to/your-app.jks,-PKEY_STORE_PASSWORD=<your_key_store_passwd>,-PKEY_ALIAS=<your_key_alias>,-PKEY_PASSWORD=<your_key_passwd>"
```

To generate an AAB instead of an APK, simply add the **`-aab`** option.

---

## ⚙️ Advanced Configuration / FAQ

- **Windows Build Tools**: Axmol supports building without installing the full Visual Studio IDE by using [BuildTools](https://aka.ms/vs/stable/vs_BuildTools.exe).  
  However, we strongly recommend installing **Visual Studio 2022 or 2026 IDE** for the best Windows development experience — in our view, it’s the most powerful IDE available.
- **Windows**: Clang is also supported for building the **Win32** target platform.  
  👉 Use the command: `axmol -cc clang`
- **Android SDK/NDK**: `setup.ps1 -p android` installs the minimum NDK version — **r23d** for `axmol-v2`, and **r27d** for `axmol-v3`.
- **Linux**: Use **VSCode** with the **C++** and **CMake Tools** extensions for the best development experience.  
  👉 Ensure your system has **GCC/G++** and **CMake** installed (Ubuntu 22.04+, Debian 12+, ArchLinux recommended).
- **WASM Debugging**: Use Chrome DevTools for debugging.

---

## 🎯 Summary

Minimal workflow is just three steps:

1. **`./setup.ps1`**  
2. **`axmol new`**  
3. **`axmol build`**

Everything else is optional or advanced.

---

## 📚 More Help

For more common build issues, please check:  
[Axmol FAQ – Building](https://github.com/axmolengine/axmol/wiki/FAQ#building)
