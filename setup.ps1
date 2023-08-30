$myRoot = $PSScriptRoot
$AX_ROOT = $myRoot

function mkdirs([string]$path) {
    if (!(Test-Path $path -PathType Container)) {
        New-Item $path -ItemType Directory 1>$null
    }
}

$build1kPath = Join-Path $myRoot '1k/build1k.ps1'
$prefix = Join-Path $myRoot 'tools/external'
if (!(Test-Path $prefix -PathType Container)) {
    mkdirs $prefix
}

# setup toolchains: glslcc, cmake, ninja, ndk, jdk, ...
. $build1kPath -setupOnly -prefix $prefix

$AX_CONSOLE_ROOT = Join-Path $AX_ROOT 'tools/console'

# https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

if ($IsWin) {
    if ($env:AX_ROOT -ne $AX_ROOT) {
        [Environment]::SetEnvironmentVariable('AX_ROOT', $AX_ROOT, 'User')
    }

    $pathList = [System.Collections.ArrayList]$env:PATH.Split(';')
    if ($pathList.IndexOf($AX_CONSOLE_ROOT) -eq -1) {
        $pathList = [System.Collections.ArrayList][Environment]::GetEnvironmentVariable('PATH', 'User').Split(';')
        $pathList.Insert(0, $AX_CONSOLE_ROOT)
        $PATH = $pathList -join ';'
        [Environment]::SetEnvironmentVariable('PATH', $PATH, 'User')
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
    $matchRet = [Regex]::Match($profileContent,"env\:AX_ROOT\s+\=\s+.*")
    if (!$matchRet.Success) {
        $profileContent += "# Add environment variable AX_ROOT for axmol`n"
        $profileContent += '$env:AX_ROOT = "{0}"{1}' -f $AX_ROOT, "`n"
        ++$profileMods
    }
    elseif($env:AX_ROOT -ne $AX_ROOT) { # contains AX_ROOT statement, but not equal us
        Write-Host "Updating env AX_ROOT from ${env:AX_ROOT} to $AX_ROOT"
        $profileContent = [Regex]::Replace($profileContent,"env\:AX_ROOT\s+\=\s+.*", "env:AX_ROOT = '$AX_ROOT'")
        ++$profileMods
    }

    if ($profileContent.IndexOf('$env:PATH = ') -eq -1 -or !($axmolCmdInfo = (Get-Command axmol -ErrorAction SilentlyContinue)) -or $axmolCmdInfo.Source -ne "$AX_CONSOLE_ROOT/axmol") {
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
        $matchRet = [Regex]::Match($profileContent,"export AX_ROOT\=.*")
        if (!$matchRet.Success) {
            $profileContent += "# Add environment variable AX_ROOT for axmol`n"
            $profileContent += 'export AX_ROOT="{0}"{1}' -f $AX_ROOT, "`n"
            ++$profileMods
        } else {
            $stmtLine = 'export AX_ROOT="{0}"' -f $AX_ROOT
            if($matchRet.Value -ne $stmtLine) {
                $profileContent = [Regex]::Replace($profileContent,"export AX_ROOT\=.*", $stmtLine)
            }
        }

        if($profileContent.IndexOf('export PATH=$AX_ROOT/tools/console:')) {
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
    
    if(Test-Path ~/.zshrc -PathType Leaf) {
        updateUnixProfile ~/.zshrc
    }

    # update macos launchctl
    if ($IsMacOS) { # for GUI app, android studio can find AX_ROOT
        launchctl setenv AX_ROOT $env:AX_ROOT
    }
}


if ($IsLinux) {
    Write-Host "Are you continue install linux dependencies for axmol? (y/n) " -NoNewline
    $answer = Read-Host
    if ($answer -like 'y*') {
        if ($(Get-Command 'dpkg' -ErrorAction SilentlyContinue)) {
            $b1k.println("It will take few minutes")
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
            $DEPENDS += 'libzip-dev'
            $DEPENDS += 'libpng-dev'
            $DEPENDS += 'libfontconfig1-dev'
            $DEPENDS += 'libgtk-3-dev'
            $DEPENDS += 'binutils'
            # $DEPENDS += 'libbsd-dev'
            $DEPENDS += 'libasound2-dev'
            $DEPENDS += 'libxxf86vm-dev'
            $DEPENDS += 'libvlc-dev', 'libvlccore-dev', 'vlc'

            # if vlc encouter codec error, install
            # sudo apt install ubuntu-restricted-extras

            sudo apt install --allow-unauthenticated --yes $DEPENDS > /dev/null
        } elseif($(Get-Command 'pacman' -ErrorAction SilentlyContinue)) {
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
            $b1k.println("Warning: current Linux distro isn't officially supported by axmol community")
        }

        b1k_print "Installing axmol freetype into linux system directory ..."
        Set-Location "$AX_ROOT/thirdparty/freetype"
        cmake -B build '-DCMAKE_BUILD_TYPE=Release' '-DCMAKE_INSTALL_PREFIX=/usr' '-DDISABLE_FORCE_DEBUG_POSTFIX=ON' '-DFT_DISABLE_HARFBUZZ=ON' '-DFT_DISABLE_BROTLI=ON' '-DFT_DISABLE_BZIP2=ON' '-DBUILD_SHARED_LIBS=ON'
        sudo cmake --build build --config Release --target install
        Set-Location -
    }
}

$b1k.pause("setup successfully, please restart the terminal to make added system variables take effect")
