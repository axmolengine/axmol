param(
    [switch]$help,
    [switch]$wait
)

# Set-StrictMode -Version Latest

# pwsh function alias
function println($message) { Write-Host "axmol: $message" }

$myRoot = $PSScriptRoot

# 0: windows, 1: linux, 2: macos
# $IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

# parse engine version
$AX_ROOT = (Resolve-Path $myRoot/../..).Path
$axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
$content = $(Get-Content -Path $axver_file)

function parse_axver($part) {
    return ($content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
}
function realpath($path) {
    return $Global:ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($path)
}

$axmolVersion = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR').$(parse_axver 'PATCH')"

$git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
if ($git_prog) {
    $branchName = $(git -C $AX_ROOT branch --show-current 2>$null)
    if ($branchName -eq 'dev') {
        $commitHash = $(git -C $AX_ROOT rev-parse --short=7 HEAD 2>$null)
        $axmolVersion += "-$commitHash"
    }
}

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

# tool usage
$tool_usage = @'
axmol-{0} console: A command line tool for axmol engine.

Using PowerShell {1}

Available commands:
        new          Creates a new project.
        build        Compile projects to binary.
        deploy       Compile and deploy a project to a device/simulator.
        run          Compiles, deploy and run project on the target.

Available arguments:
        -h           Show this help information.

Example:
        axmol new --help
        axmol run --help
'@ -f $axmolVersion, $pwsh_ver

$cmdName = $args[0]

if ($IsMacOS) {
    function find_simulator_id($plat, $deviceName = '') {
        $selStr = @{ios = 'iPhone'; tvos = 'Apple TV' }[$plat]
        [array]$emulators = (xcrun xctrace list devices | Select-String $selStr)
        foreach ($emulator in $emulators) {
            $emuInfoStr = $emulator.Line
            if ($emuInfoStr.StartsWith($deviceName) -or !$deviceName) {
                return ($emuInfoStr -split "\(|\)")[3], $emuInfoStr
            }
        }
    }

    function get_bundle_id($appPath) {
        $plistFile = Join-Path $appPath 'Info.plist'
        $appInfo = ConvertFrom-Json (plutil -convert json -o - $plistFile)
        return $appInfo.CFBundleIdentifier
    }
}

function axmol_build() {
    $sub_args = $args
    println $sub_args
    $build_script = Join-Path $PSScriptRoot 'build.ps1'
    if ("$args".Contains('-d')) {
        # have proj dir
        . $build_script @sub_args
    }
    else {
        . $build_script @sub_args -d (Get-Location).Path
    }
}

function axmol_deploy() {
    $sub_args = $args
    . axmol_build @sub_args
    if ($TARGET_OS -eq 'winrt') {
        $appxManifestFile = Join-Path $BUILD_DIR "bin/$cmake_target/$optimize_flag/Appx/AppxManifest.xml"

        # deploy by visual studio major program: devenv.exe
        $vswherePath = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
        $devenvPath = &$vswherePath -latest -requires Microsoft.Component.MSBuild -find Common*\IDE\devenv.exe | select-object -first 1

        if (Test-Path $devenvPath -PathType Leaf) {
            $devenvRoot = Split-Path $devenvPath -Parent
            $env:PATH = "$devenvRoot;$env:PATH"
            $slnDir = $BUILD_DIR
            $slnFileName = (Get-ChildItem $slnDir *.sln).Name
            $slnFilePath = Join-Path $slnDir $slnFileName
            devenv $slnFilePath /deploy $optimize_flag /project $cmake_target /projectconfig $optimize_flag
        }

        [XML]$appxManifest = Get-Content $appxManifestFile
        $appxIdentity = $appxManifest.Package.Identity.Name
        # $appxPkgFullName = (powershell -Command "(Get-AppxPackage -Name '$appxIdentity' | Select-Object -Unique 'PackageFullName').PackageFullName")
        # if ($appxPkgFullName ) {
        #     if ($uninst) {
        #         println "Uninstalling $appxPkgFullName ..."
        #         powershell -Command "Remove-AppxPackage -Package '$appxPkgFullName'"
        #         powershell -Command "Add-AppxPackage -Register '$appxManifestFile'"
        #     }
        # }
        # else {
        #     powershell -Command "Add-AppxPackage -Register '$appxManifestFile'"
        # }
        $appxPkgName = (powershell -Command "(Get-AppxPackage -Name '$appxIdentity' | Select-Object -Unique 'PackageFamilyName').PackageFamilyName")
        println "Deploy $cmake_target done: $appxPkgName"
    }
    elseif ($TARGET_OS -eq 'win32') {
        $win32exePath = Join-Path $BUILD_DIR "bin/$cmake_target/$optimize_flag/$cmake_target.exe"
        println "Deploy $cmake_target done: $win32exePath"
    }
    elseif ($TARGET_OS -eq 'android') {
        $optimize_flag_lower = $optimize_flag.ToLower()
        $apkDir = Join-Path $proj_dir "proj.android/app/build/outputs/apk/$optimize_flag_lower"
        $apkName = (Get-ChildItem -Path "$apkDir/*.apk").Name
        $apkFullPath = Join-Path $apkDir $apkName

        # read applicationId aka package name
        $outputMetaFile = Join-Path $apkDir 'output-metadata.json'
        $outputMeta = ConvertFrom-Json $(Get-Content $outputMetaFile -raw)
        $androidAppId = $outputMeta.applicationId 

        # read activityName
        $androidManifestFile = Join-Path $proj_dir 'proj.android/app/AndroidManifest.xml'
        [XML]$androidManifest = Get-Content $androidManifestFile
        $androidActivity = $androidManifest.manifest.application.activity.name

        adb install -t -r $apkFullPath
        if ($?) {
            println "Deploy $cmake_target done: $androidAppId/$androidActivity"
        }
    }
    elseif ($TARGET_OS -eq 'ios' -or $TARGET_OS -eq 'tvos') {
        if ($options.a -eq 'x64') {
            $ios_app_path = Join-Path $BUILD_DIR "bin/$cmake_target/$optimize_flag/$cmake_target.app"
            $ios_bundle_id = get_bundle_id($ios_app_path)

            println 'Finding avaiable simualtor ...'
            $ios_simulator_id, $simulator_info = find_simulator_id($TARGET_OS)

            println "Booting $simulator_info ..."
            xcrun simctl boot $ios_simulator_id '--arch=x86_64'
            println "Installing $ios_app_path ..."
            xcrun simctl install $ios_simulator_id $ios_app_path
            println "Deploy $cmake_target done: $ios_bundle_id"
        }
        else {
            # ios device

        }
    }
}

function is_console_app($path) {
    # $peFile = [System.IO.FileStream]::new($path, 'Open', 'Read')
    # $peReader = New-Object System.Reflection.PortableExecutable.PEReader -ArgumentList $peFile
    # $isConsole = $peReader.PEHeaders.PEHeader.Subsystem -eq 3
    # $peFile.Close()
    # return $isConsole
    $path = $1k.realpath($path)
    $buffer = New-Object Byte[] 248
    $fileStream = [System.IO.FileStream]::new($path, [IO.FileMode]::Open, [IO.FileAccess]::Read)
    $fileStream.Seek(0x3C, [System.IO.SeekOrigin]::Begin) | Out-Null
    $fileStream.Read($buffer, 0, 4) | Out-Null
    $peHeaderOffset = [BitConverter]::ToInt32($buffer, 0)
    $fileStream.Seek($peHeaderOffset, [System.IO.SeekOrigin]::Begin) | Out-Null
    $fileStream.Read($buffer, 0, 248) | Out-Null
    $fileStream.Seek($peHeaderOffset + 0x5C, [System.IO.SeekOrigin]::Begin) | Out-Null
    $fileStream.Read($buffer, 0, 2) | Out-Null
    $buffer[2] = 0
    $fileStream.Close()
    $subsystem = [BitConverter]::ToInt16($buffer, 0)
    return $subsystem -eq 3
}

function axmol_run() {
    $sub_args = $args
    . axmol_deploy @sub_args
    if($is_host_target -and $TARGET_CPU -ne $HOST_CPU) {
        println "Skip launch $cmake_target, due to $TARGET_OS-$TARGET_CPU apps can't runs on $HOST_OS_NAME-$HOST_CPU"
        return
    }
    if ($TARGET_OS -eq 'winrt') {
        explorer.exe shell:AppsFolder\$appxPkgName!App
    }
    elseif ($TARGET_OS -eq 'win32') {
        $workdir = $(Split-Path $win32exePath -Parent)
        $additional_args = @{}
        if ($wait -or (is_console_app $win32exePath)) { $additional_args = @{NoNewWindow = $true; Wait = $true } }
        Start-Process -FilePath $win32exePath -WorkingDirectory $workdir @additional_args
    }
    elseif ($TARGET_OS -eq 'android') {
        adb shell am start -n "$androidAppId/$androidActivity"
    }
    elseif ($TARGET_OS -eq 'ios') {
        if ($ios_bundle_id) {
            println "Launching $cmake_target ..."
            xcrun simctl launch $ios_simulator_id $ios_bundle_id
        }
    }
    elseif ($TARGET_OS -eq 'osx') {
        $launch_macapp = Join-Path $BUILD_DIR "bin/$cmake_target/$optimize_flag/$cmake_target.app/Contents/MacOS/$cmake_target"
        & $launch_macapp
    }
    elseif ($TARGET_OS -eq 'linux') {
        $launch_linuxapp = Join-Path $BUILD_DIR "bin/$cmake_target/$cmake_target"
        println "Launching $launch_linuxapp ..."
        $additional_args = @()
        if ($wait) { $additional_args = @{NoNewWindow = $true; Wait = $true } }
        Start-Process -FilePath $launch_linuxapp -WorkingDirectory $(Split-Path $launch_linuxapp -Parent) @additional_args
    }
    elseif ($TARGET_OS.startsWith('wasm')) {
        $launch_wasmapp = Join-Path $BUILD_DIR "bin/$cmake_target/$cmake_target.html"
        println "Launching $launch_wasmapp ..."
        emrun $launch_wasmapp
    }
    println "Launch $cmake_target done, target platform is $($TARGET_OS)"
}

$builtinPlugins = @{
    new    = @{
        proc  = (Join-Path $myRoot 'axmol_new.ps1');
        usage = @"
usage: axmol new -p org.axmol.hellocpp -d path/to/project -l cpp --portrait <ProjectName>
Creates a new project.

positional arguments:
    PROJECT_NAME          Set the project name.

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
"@;
    };
    build  = @{
        proc  = ${function:axmol_build};
        usage = @"
usage: axmol [build] -p win32 -a x64

Build projects to binary.

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
"@;
    };
    deploy = @{
        proc  = ${function:axmol_deploy};
        usage = @"
usage: axmol install -p win32 -a x64

Build and install a project to a device/simulator.

options:
  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,wasm
      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first,
      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
  -xb: additional cross build options: i.e. -xb '--config','Release'
  -c: no build, only generate native project files (vs .sln, xcodeproj)
  -d: specify project dir to compile, i.e. -d /path/your/project/
  -f: force generate native project files. Useful if no changes are detected, such as with resource updates.
"@;
    };
    run    = @{
        proc  = ${function:axmol_run};
        usage = @"
usage: axmol run -p win32 -a x64
Build, deploy and run project on the target.

options:
  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,wasm
      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first,
      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
  -xb: additional cross build options: i.e. -xb '--config','Release'
  -c: no build, only generate native project files (vs .sln, xcodeproj)
  -d: specify project dir to compile, i.e. -d /path/your/project/
  -f: force generate native project files. Useful if no changes are detected, such as with resource updates.
"@
    }
}

# print version message
if ($cmdName -eq '--version' -or $cmdName -eq '-V') {
    $version_msg = @"
axmol version {0}

Axmol Engine maintained and supported by axmol community (axmol.dev)
"@ -f $axmolVersion
    Write-Host $version_msg
    return
}

$plugin = $builtinPlugins["$cmdName"]
if ($plugin) {
    # -h will consumed by param
    # !!!Note: 3 condition statement: 'xxx = if(c) { v1 } else { v2 }' will lost array type if source array only 1 element
    if ($args.Count -gt 1) { 
        $sub_args = $args[1..($args.Count - 1)] 
    } 
    else {
        $sub_args = @()
    }
}
else {
    $sub_args = $args
}

if ($help -or ($sub_args -and $sub_args[0] -eq '--help')) {
    if ($plugin) {
        println $plugin.usage
    }
    else {
        println $tool_usage
    }
    return 
}

if (!$plugin) { $plugin = $builtinPlugins['build'] }

# force convert sub_args to array if it's a single string
if ($sub_args -isnot [array]) {
    $sub_args = @($sub_args)
}

$sub_opts = @{}
if (!$sub_args.Contains('-d')) {
    $sub_opts['d'] = $(Get-Location).Path
}

if ($args[0] -eq 'new') {
    if (!$sub_args.Contains('-p')) {
        $sub_opts['p'] = 'org.axmol.demo'
    }
    if (!$sub_args.Contains('-l')) {
        $sub_opts['l'] = 'cpp'
    }
}

. $plugin.proc @sub_args @sub_opts

exit $LASTEXITCODE
