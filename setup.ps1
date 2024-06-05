#!/bin/bash
` # \
# PowerShell Param statement : every line must end in #\ except the last line must with <#\
# And, you can't use backticks in this section        #\
# refer https://gist.github.com/ryanmaclean/a1f3135f49c1ab3fa7ec958ac3f8babe #\
param( [string]$gradlewVersion,                       #\
    [switch]$setupCMake                               #\
    )                                                <#\
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ `
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Bash Start ------------------------------------------------------------
scriptdir="`dirname "${BASH_SOURCE[0]}"`"
if ! which pwsh > /dev/null; then
    $scriptdir/1k/install-pwsh.sh
fi
pwsh $scriptdir/setup.ps1 "$@"
# Bash End --------------------------------------------------------------
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# emit powershell code, in bash:
# the here document key can be any, but out-null is valid powershell nop cmdlet
# echo > /dev/null don't output line-end to termainl
echo > /dev/null <<"out-null"
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Powershell Start ----------------------------------------------------#>

$myRoot = $PSScriptRoot
$AX_ROOT = $myRoot

$Global:is_axmol_app = $true

function println($message) { Write-Host "axmol: $message" }

# import VersionEx
. (Join-Path $PSScriptRoot '1k/extensions.ps1')

$pwsh_ver = [VersionEx]$PSVersionTable.PSVersion.ToString()

function mkdirs([string]$path) {
    if (!(Test-Path $path -PathType Container)) {
        if ($pwsh_ver -ge [VersionEx]'5.0') {
            New-Item $path -ItemType Directory 1>$null
        }
        else {
            mkdir $path
        }
    }
}

if ($pwsh_ver -lt [VersionEx]'5.0') {
    # try setup WMF5.1, require reboot, try run setup.ps1 several times
    Write-Host "Installing WMF5.1 ..."
    $osVer = [System.Environment]::OSVersion.Version
    
    if ($osVer.Major -ne 6) {
        throw "Unsupported OSVersion: $($osVer.ToString())"
    }
    if ($osVer.Minor -ne 1 -and $osVer -ne 3) {
        throw "Only win7 SP1 or win8 supported"
    }
    
    $is_win7 = $osVer.Minor -eq 1
    
    # [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12 5.1 non-win10
    
    $prefix = Join-Path (Get-Location).Path 'tmp'
    
    mkdirs $prefix
    $curl = (New-Object Net.WebClient)
    
    # .net 4.5.2 prereq by WMF5.1
    $pkg_out = Join-Path $prefix 'NDP452-KB2901907-x86-x64-AllOS-ENU.exe'
    if (!(Test-Path $pkg_out -PathType Leaf)) {
        Write-Host "Downloading $pkg_out ..."
        $curl.DownloadFile('https://download.microsoft.com/download/E/2/1/E21644B5-2DF2-47C2-91BD-63C560427900/NDP452-KB2901907-x86-x64-AllOS-ENU.exe', $pkg_out)
        if (!$?) {
            del $pkg_out
        }
    }
    .\tmp\NDP452-KB2901907-x86-x64-AllOS-ENU.exe /q /norestart
    
    # WMF5.1: https://learn.microsoft.com/en-us/powershell/scripting/windows-powershell/wmf/setup/install-configure?view=powershell-7.3&source=recommendations#download-and-install-the-wmf-51-package
    if ($is_win7) {
        $wmf_pkg = 'Win7AndW2K8R2-KB3191566-x64.zip'
    }
    else {
        $wmf_pkg = 'Win8.1AndW2K12R2-KB3191564-x64.msu'
    }
    
    $pkg_out = Join-Path $prefix "$wmf_pkg"
    if (!(Test-Path $pkg_out -PathType Leaf)) {
        Write-Host "Downloading $pkg_out ..."
        $curl.DownloadFile("https://download.microsoft.com/download/6/F/5/6F5FF66C-6775-42B0-86C4-47D41F2DA187/$wmf_pkg", $pkg_out)
        if (!$?) {
            del $pkg_out
        }
    }
    if ($is_win7) {
        echo "Expanding $pkg_out to $prefix"
        function Expand-Zip($Path, $DestinationPath) {
            mkdirs $DestinationPath
            $shell = new-object -com shell.application
            $zip = $shell.NameSpace($Path)
            foreach ($item in $zip.items()) {
                $shell.Namespace($DestinationPath).copyhere($item)
            }
        }
        Expand-Zip -Path $pkg_out -DestinationPath $prefix\WMF51
        & "$prefix\WMF51\Install-WMF5.1.ps1"
    }
    else {
        wusa.exe $pkg_out /quiet /norestart
    }

    throw "PowerShell 5.0+ required, installed is: $pwsh_ver, after install WMF5.1 and restart computer, try again"
}

# powershell 7 require mark as global explicit if want access in function via $Global:xxx
$Global:AX_CLI_ROOT = Join-Path $AX_ROOT 'tools/cmdline'

# https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

if ($IsWin) {
    if ("$env:AX_ROOT" -ne "$AX_ROOT") {
        $env:AX_ROOT = $AX_ROOT
        [Environment]::SetEnvironmentVariable('AX_ROOT', $AX_ROOT, 'User')
    }

    #  checking evaluated env:PATH with system + user
    $Global:isMeInPath = $env:PATH.Contains($AX_CLI_ROOT)
    $Global:oldCmdRoot = $null
    $Global:cmdInfo = Get-Command 'axmol' -ErrorAction SilentlyContinue
    if ($cmdInfo) {
        $cmdRootTmp = Split-Path $cmdInfo.Source -Parent
        if ($cmdRootTmp -ne $AX_CLI_ROOT) {
            $oldCmdRoot = $cmdRootTmp
        }
    }
    
    function RefreshPath ($strPathList) {
        if ($strPathList) { 
            $pathList = [System.Collections.ArrayList]($strPathList.Split(';', [System.StringSplitOptions]::RemoveEmptyEntries))
        }
        else { 
            $pathList = New-Object System.Collections.ArrayList 
        }
        
        if ($Global:oldCmdRoot) {
            $pathList.Remove($Global:oldCmdRoot)
        }
        
        if ($Global:isMeInPath) {
            if ($pathList[0] -ne $Global:AX_CLI_ROOT) {
                $pathList.Remove($Global:AX_CLI_ROOT)
                $pathList.Insert(0, $Global:AX_CLI_ROOT)
            }
        }
        else {
            $pathList.Insert(0, $Global:AX_CLI_ROOT)
        }
        return $pathList -join ';'
    }
    
    if (!$isMeInPath -or $oldCmdRoot) {
        # Add cmdline bin to User PATH
        $strPathList = [Environment]::GetEnvironmentVariable('PATH', 'User')
        $strPathList = RefreshPath $strPathList 
        [Environment]::SetEnvironmentVariable('PATH', $strPathList, 'User')

        # Re-eval env:PATH to system + users
        $env:PATH = RefreshPath $env:PATH # sync to PowerShell Terminal
    }

    $execPolicy = powershell -Command 'Get-ExecutionPolicy'
    if ($execPolicy -ne 'Bypass') {
        println "Setting system installed powershell execution policy '$execPolicy'==>'Bypass', please click 'YES' on UAC dialog"
        Start-Process powershell -ArgumentList '-Command "Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass -Force"' -WindowStyle Hidden -Wait -Verb runas
    } else {
        println "Nice, the system installed powershell execution policy is '$execPolicy'"
    }
}
else {
    # update pwsh profile
    if (Test-Path $PROFILE -PathType Leaf) {
        $profileContent = Get-Content $PROFILE -raw
    }
    else {
        $profileContent = ''
    }

    $profileMods = 0
    $matchRet = [Regex]::Match($profileContent, "env\:AX_ROOT\s+\=\s+.*")
    if (!$matchRet.Success) {
        $profileContent += "# Add environment variable AX_ROOT for axmol`n"
        $profileContent += '$env:AX_ROOT = "{0}"{1}' -f $AX_ROOT, "`n"
        ++$profileMods
    }
    elseif ($env:AX_ROOT -ne $AX_ROOT) {
        # contains AX_ROOT statement, but not equal us
        Write-Host "Updating env AX_ROOT from ${env:AX_ROOT} to $AX_ROOT"
        $profileContent = [Regex]::Replace($profileContent, "env\:AX_ROOT\s+\=\s+.*", "env:AX_ROOT = '$AX_ROOT'")
        ++$profileMods
    }
    if ($profileMods) { $env:AX_ROOT = $AX_ROOT }

    if (!($axmolCmdInfo = (Get-Command axmol -ErrorAction SilentlyContinue)) -or $axmolCmdInfo.Source -ne "$AX_CLI_ROOT/axmol") {
        $stmt_export = '$env:PATH = "${env:AX_ROOT}/tools/cmdline:${env:PATH}"'
        if(!$profileContent.Contains($stmt_export)) {
            $profileContent += "# Add axmol cmdline tool to PATH`n"
            $profileContent += '$env:PATH = "${env:AX_ROOT}/tools/cmdline:${env:PATH}"'
            $profileContent += "`n"
            ++$profileMods
        }
        $env:PATH = "${env:AX_ROOT}/tools/cmdline:${env:PATH}"
    }

    $profileDir = Split-Path $PROFILE -Parent
    if (!(Test-Path $profileDir -PathType Container)) {
        mkdirs $profileDir
    }

    if ($profileMods) {
        Set-Content $PROFILE -Value $profileContent
    }

    # update ~/.bashrc, ~/.zshrc
    function updateUnixProfile($profileFile) {
        $profileMods = 0
        $profileContent = Get-Content $profileFile -raw
        $matchRet = [Regex]::Match($profileContent, "export AX_ROOT\=.*")
        if (!$matchRet.Success) {
            $profileContent += "# Add environment variable AX_ROOT for axmol`n"
            $profileContent += 'export AX_ROOT="{0}"{1}' -f $AX_ROOT, "`n"
            ++$profileMods
        }
        else {
            $stmtLine = 'export AX_ROOT="{0}"' -f $AX_ROOT
            if ($matchRet.Value -ne $stmtLine) {
                $profileContent = [Regex]::Replace($profileContent, "export AX_ROOT\=.*", $stmtLine)
                ++$profileMods
            }
        }

        if (!$profileContent.Contains('export PATH=$AX_ROOT/tools/cmdline:')) {
            $profileContent += "# Add axmol cmdline tool to PATH`n"
            $profileContent += 'export PATH=$AX_ROOT/tools/cmdline:$PATH' -f "`n"
            ++$profileMods
        }
        if ($profileMods) {
            Set-Content $profileFile -Value $profileContent
        }
    }

    if (Test-Path ~/.bashrc -PathType Leaf) {
        updateUnixProfile ~/.bashrc
    }
    
    if (Test-Path ~/.zshrc -PathType Leaf) {
        updateUnixProfile ~/.zshrc
    }

    # update macos launchctl
    if ($IsMacOS) {
        # for GUI app, android studio can find AX_ROOT
        launchctl setenv AX_ROOT $env:AX_ROOT
    }
}


if ($IsLinux) {
    Write-Host "Are you continue install linux dependencies for axmol? (y/n) " -NoNewline
    $answer = Read-Host
    if ($answer -like 'y*') {
        if ($(Get-Command 'dpkg' -ErrorAction SilentlyContinue)) {
            println "It will take few minutes"
            sudo apt update
            # for vm, libxxf86vm-dev also required

            $DEPENDS = @()

            $DEPENDS += 'libx11-dev'
            $DEPENDS += 'automake'
            $DEPENDS += 'libtool'
            $DEPENDS += 'cmake'
            $DEPENDS += 'libxmu-dev'
            $DEPENDS += 'libglu1-mesa-dev'
            $DEPENDS += 'libgl2ps-dev'
            $DEPENDS += 'libxi-dev'
            $DEPENDS += 'libfontconfig1-dev'
            $DEPENDS += 'libgtk-3-dev'
            $DEPENDS += 'libwebkit2gtk-4.0-dev'
            $DEPENDS += 'binutils'
            $DEPENDS += 'g++'
            $DEPENDS += 'libasound2-dev'
            $DEPENDS += 'libxxf86vm-dev'
            $DEPENDS += 'libvlc-dev', 'libvlccore-dev', 'vlc'

            # if vlc encouter codec error, install
            # sudo apt install ubuntu-restricted-extras

            sudo apt install --allow-unauthenticated --yes $DEPENDS > /dev/null
        }
        elseif ($(Get-Command 'pacman' -ErrorAction SilentlyContinue)) {
            $DEPENDS = @(
                'git',
                'cmake',
                'make',
                'libx11', 
                'libxrandr',
                'libxinerama',
                'libxcursor',
                'libxi',
                'fontconfig',
                'gtk3',
                'webkit2gtk4.0',
                'vlc'
            )
            sudo pacman -S --needed --noconfirm @DEPENDS
        }
        else {
            println "Warning: current Linux distro isn't officially supported by axmol community"
        }
    }
}

$1k_script = Join-Path $myRoot '1k/1kiss.ps1'
$prefix = Join-Path $myRoot 'tools/external'
if (!(Test-Path $prefix -PathType Container)) {
    mkdirs $prefix
}

# setup toolchains: glslcc, cmake, ninja, ndk, jdk, ...
. $1k_script -setupOnly -prefix $prefix @args

if ($setupCMake) {
    setup_cmake -scope 'global'
}

if ($gradlewVersion) {
    $aproj_source_root = Join-Path $AX_ROOT 'templates/common/proj.android'
    $aproj_source_gradle = Join-Path $aproj_source_root 'build.gradle'
    $aproj_source_gradle_wrapper = Join-Path $aproj_source_root 'gradle/wrapper/'
    $vernums = $gradlewVersion.Split('.')
    if($vernums.Count -lt 3) {
        $gradle_tag = "v$gradlewVersion.0"
    } else {
        $gradle_tag = "v$gradlewVersion"
    }

    $gradle_settings_file = Join-Path $aproj_source_gradle_wrapper 'gradle-wrapper.properties'
    $settings_content = [System.IO.File]::ReadAllText($gradle_settings_file)
    $settings_content = [Regex]::Replace($settings_content, 'gradle-.+-bin.zip', "gradle-$gradlewVersion-bin.zip")
    [System.IO.File]::WriteAllText($gradle_settings_file, $settings_content)

    # download gradle-wrapper.jar gradlew and gradlew.bat from upstream
    download_file "https://raw.githubusercontent.com/gradle/gradle/$gradle_tag/gradlew" $aproj_source_root
    download_file "https://raw.githubusercontent.com/gradle/gradle/$gradle_tag/gradlew.bat" $aproj_source_root
    download_file "https://github.com/gradle/gradle/raw/$gradle_tag/gradle/wrapper/gradle-wrapper.jar" $aproj_source_gradle_wrapper

    function update_gradle_for_test($testName) {
        $aproj_root = Join-Path $AX_ROOT "tests/$testName/proj.android"
        Copy-Item $aproj_source_gradle $aproj_root -Force
        Copy-Item "${aproj_source_gradle_wrapper}*" (Join-Path $aproj_root 'gradle/wrapper') -Force

        Copy-Item (Join-Path $aproj_source_root 'gradlew') $aproj_root -Force
        Copy-Item (Join-Path $aproj_source_root 'gradlew.bat') $aproj_root -Force
    }

    update_gradle_for_test 'cpp-tests'
    update_gradle_for_test 'fairygui-tests'
    update_gradle_for_test 'live2d-tests'
    update_gradle_for_test 'lua-tests'
}

if ($IsLinux -and (Test-Path '/etc/wsl.conf' -PathType Leaf)) {
    Write-Host "Are want remove host windows path from wsl? (y/n) " -NoNewline
    $answer = Read-Host
    if ($answer -like 'y*') {
        $wsl_conf = [System.IO.File]::ReadAllText('/etc/wsl.conf')
        if (!$wsl_conf.Contains('appendWindowsPath')) {
            $wsl_conf += "`n[interop]`nappendWindowsPath = false"
            $wsl_conf_tmp_file = (Join-Path $AX_ROOT 'wsl.conf')
            [System.IO.File]::WriteAllText($wsl_conf_tmp_file, $wsl_conf)
            sudo mv -f $wsl_conf_tmp_file /etc/wsl.conf
            println "Update /etc/wsl.conf success, please run 'wsl --shutdown' on your host windows, then re-enter wsl"
        }
    }
}

$1k.pause("setup successfully, please restart the terminal to make added system variables take effect")

# Powershell End -------------------------------------------------------
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
out-null
