$myRoot = $PSScriptRoot
$AX_ROOT = $myRoot

$build1kPath = Join-Path $myRoot '1k/build1k.ps1'
$prefix = Join-Path $myRoot 'tools/external'
if (!(Test-Path $prefix -PathType Container)) {
    mkdir $prefix | Out-Null
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
        $profileContent += '$env:PATH = "${env:AX_ROOT}/tools/console:${env:PATH}"{0}' -f "`n"
        ++$profileMods
    }

    $profileDir = Split-Path $PROFILE -Parent
    if (!(Test-Path $profileDir -PathType Container)) {
        mkdir $profileDir | Out-Null
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
    $sudo_pinfo = (Get-Command 'sudo' -ErrorAction SilentlyContinue)
    if ($sudo_pinfo) {
        Write-Host "Are you continue install linux dependencies for axmol? (y/n) " -NoNewline
        $answer = Read-Host
        if ($answer -like 'y*') {
            b1k_print "It will take few minutes"
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
            $DEPENDS += 'libbsd-dev'
            $DEPENDS += 'libasound2-dev'
            $DEPENDS += 'libxxf86vm-dev'
            $DEPENDS += 'libvlc-dev', 'libvlccore-dev', 'vlc'

            # if vlc encouter codec error, install
            # sudo apt install ubuntu-restricted-extras

            sudo apt install --allow-unauthenticated --yes $DEPENDS > /dev/null

            b1k_print "Installing latest freetype for linux ..."
            mkdir buildsrc
            Set-Location buildsrc
            git clone 'https://github.com/freetype/freetype.git'
            Set-Location freetype
            git checkout 'VER-2-13-0'
            sh autogen.sh
            ./configure '--prefix=/usr' '--enable-freetype-config' '--disable-static'
            sudo make install
            Set-Location ..
            Set-Location ..
        }
    } else {
        $b1k.println("The sudo command not present in current linux distro")
    }
}

$b1k.pause("setup successfully")
