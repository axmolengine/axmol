# the setup script of axmol
# for powershell <= 5.1: please execute command 'Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Bypass -Force' in PowerShell Terminal
param(
    # whether sync gradle wrapper & plugin version from template to test projects
    [switch]$updateGradle 
)
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
$Global:AX_CONSOLE_ROOT = Join-Path $AX_ROOT 'tools/console'

# https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

if ($IsWin) {
    if ("$env:AX_ROOT" -ne "$AX_ROOT") {
        $env:AX_ROOT = $AX_ROOT
        [Environment]::SetEnvironmentVariable('AX_ROOT', $AX_ROOT, 'User')
    }

    #  checking evaluated env:PATH with system + user
    $Global:isMeInPath = $env:PATH.Contains($AX_CONSOLE_ROOT)
    $Global:oldCmdRoot = $null
    $Global:cmdInfo = Get-Command 'axmol' -ErrorAction SilentlyContinue
    if ($cmdInfo) {
        $cmdRootTmp = Split-Path $cmdInfo.Source -Parent
        if ($cmdRootTmp -ne $AX_CONSOLE_ROOT) {
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
            if ($pathList[0] -ne $Global:AX_CONSOLE_ROOT) {
                $pathList.Remove($Global:AX_CONSOLE_ROOT)
                $pathList.Insert(0, $Global:AX_CONSOLE_ROOT)
            }
        }
        else {
            $pathList.Insert(0, $Global:AX_CONSOLE_ROOT)
        }
        return $pathList -join ';'
    }
    
    if (!$isMeInPath -or $oldCmdRoot) {
        # Add console bin to User PATH
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
        println "Great, the system installed powershell execution policy is '$execPolicy'"
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

    if (!$profileContent.Contains('$env:PATH = ') -or !($axmolCmdInfo = (Get-Command axmol -ErrorAction SilentlyContinue)) -or $axmolCmdInfo.Source -ne "$AX_CONSOLE_ROOT/axmol") {
        $profileContent += "# Add axmol console tool to PATH`n"
        $profileContent += '$env:PATH = "${env:AX_ROOT}/tools/console:${env:PATH}"'
        $profileContent += "`n"
        ++$profileMods
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

        if (!$profileContent.Contains('export PATH=$AX_ROOT/tools/console:')) {
            $profileContent += "# Add axmol console tool to PATH`n"
            $profileContent += 'export PATH=$AX_ROOT/tools/console:$PATH' -f "`n"
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
                'vlc'
            )
            sudo pacman -S --needed --noconfirm @DEPENDS
        }
        else {
            println "Warning: current Linux distro isn't officially supported by axmol community"
        }
    }
}

$build1kPath = Join-Path $myRoot '1k/build.ps1'
$prefix = Join-Path $myRoot 'tools/external'
if (!(Test-Path $prefix -PathType Container)) {
    mkdirs $prefix
}

# setup toolchains: glslcc, cmake, ninja, ndk, jdk, ...
. $build1kPath -setupOnly -prefix $prefix @args

if ($updateGradle) {
    # sync gradle
    $aproj_source_root = Join-Path $AX_ROOT 'templates/common/proj.android'
    $aproj_source_gradle = Join-Path $aproj_source_root 'build.gradle'
    $aproj_source_gradle_wrapper = Join-Path $aproj_source_root 'gradle/wrapper/*'
    function update_gradle_for_test($testName) {
        $aproj_root = Join-Path $AX_ROOT "tests/$testName/proj.android"
        Copy-Item $aproj_source_gradle $aproj_root -Force
        Copy-Item $aproj_source_gradle_wrapper (Join-Path $aproj_root 'gradle/wrapper') -Force
    }

    update_gradle_for_test 'cpp-tests'
    update_gradle_for_test 'fairygui-tests'
    update_gradle_for_test 'live2d-tests'
    update_gradle_for_test 'lua-tests'
}

$b1k.pause("setup successfully, please restart the terminal to make added system variables take effect")
