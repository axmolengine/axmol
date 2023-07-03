
# Can runs on Windows,Linux
$DIR = $PSScriptRoot

$isWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$tools_dir = $(Resolve-Path $PSScriptRoot/..).Path # the tools install dir if not found in system
$tools_dir = Join-Path $tools_dir 'external'
if (!(Test-Path "$tools_dir" -PathType Container)) {
    mkdir $tools_dir
}

function setup_doxygen() {
    $doxygen_ver = '1.9.7'

    $doxygen_pkg_name = if ($isWin) {"doxygen-$doxygen_ver.windows.x64.bin.zip"} else {"doxygen-$doxygen_ver.linux.bin.tar.gz"}
    $doxygen_pkg_path = Join-Path $tools_dir $doxygen_pkg_name
    
    if (!(Test-Path $doxygen_pkg_path -PathType Leaf)) {
        $doxygen_ver_ul = $doxygen_ver.Replace('.', '_')
        Invoke-WebRequest -Uri "https://github.com/doxygen/doxygen/releases/download/Release_$doxygen_ver_ul/$doxygen_pkg_name" -OutFile $doxygen_pkg_path | Out-Host
    }

    $doxygen_root = Join-Path $tools_dir "doxygen-$doxygen_ver"
    $doxygen_bin = $doxygen_root
    if (!(Test-Path $doxygen_root -PathType Container)) {
        if ($isWin) {
            mkdir $doxygen_root
            Expand-Archive -Path $doxygen_pkg_path -DestinationPath $doxygen_root
        }
        else {
            tar xvf $doxygen_pkg_path -C $tools_dir
        }
    }

    if (!$isWin) {
        $doxygen_bin += '/bin'
    }

    if ($env:PATH.IndexOf($doxygen_bin) -eq -1) {
        $envPathSep = if($isWin) {';'} else {':'}
        $env:PATH = "$doxygen_bin$envPathSep$env:PATH"
    }
}

setup_doxygen

Write-Host "Using doxygen $(doxygen --version)"

$CONFIG_NAME = 'doxygen'
if ($args.Count -gt 0) {
    $CONFIG_NAME = $args[0]
}

$AX_ROOT = (Resolve-Path $DIR/../..)
$axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
$content = ($(Get-Content -Path $axver_file) | Select-String 'AX_VERSION_STR')
$axver = $content[0].Line.Split(' ')[2].Replace('"', '')

$git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
if($git_prog) {
    Write-Host "Found git: $git_prog"
    $branchName = $(git -C $AX_ROOT branch --show-current)
    if ($branchName -eq 'dev') {
        $commitHash = $(git -C $AX_ROOT rev-parse --short=7 HEAD)
        $axver += "-$commitHash"
    }
}

$docsRoot = (Resolve-Path "$DIR/../../docs").Path

$store_cwd = (Get-Location).Path
Set-Location $docsRoot

$content = [Regex]::Replace($(Get-Content doxygen.config -raw), '\bv1.0\b', "v$axver")
Move-Item 'doxygen.config' 'doxygen.config.bak' -Force
Set-Content -Path doxygen.config -Value "$content"

doxygen "$CONFIG_NAME.config"

#restore the old doxygen.config
Move-Item 'doxygen.config.bak' 'doxygen.config' -Force

Set-Location $store_cwd

