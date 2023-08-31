
# Can runs on Windows,Linux
$DIR = $PSScriptRoot

$isWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$tools_dir = $(Resolve-Path $PSScriptRoot/..).Path # the tools install dir if not found in system
$tools_dir = Join-Path $tools_dir 'external'
if (!(Test-Path "$tools_dir" -PathType Container)) {
    mkdir $tools_dir
}

function mkdirs([string]$path) {
    if (!(Test-Path $path)) {
        New-Item $path -ItemType Directory 1>$null
    }
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

$AX_ROOT = (Resolve-Path $DIR/../..)

function query_axmol_latest() {
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
    return $axver
}

$site_src = (Resolve-Path "$DIR/../../docs").Path
$site_dist = Join-Path $site_src 'dist'

mkdirs $site_dist

$docsOut = Join-Path $docsRoot 'out'
mkdirs $docsOut

$store_cwd = (Get-Location).Path
Set-Location $site_src

function  configure_file($infile, $outfile, $vars) {
    $content = $(Get-Content $infile -raw)
    foreach($var in $vars.GetEnumerator()) {
        $content = [Regex]::Replace($content, $var.Key, $var.Value)
    }
    Set-Content -Path $outfile -Value "$content"
}

# building latest
$verMap = @{
    'latest' = $null;
    '1.0' = "v1.0.0"
}

$strVerList = "'$($verMap.Keys -join "','")'"

foreach($item in $verMap.GetEnumerator()) {
    $ver = $item.Key
    $html_out = Join-Path $site_dist "manual/$ver"
    mkdirs $html_out
    $release_tag = $item.Value

    if ($ver -eq 'latest') {
        git checkout dev
        $release_tag = query_axmol_latest
    } elseif($ver -eq '1.0') {
        git checkout '1.x' # 1.x branch now for v1.0
    } else {
        git checkout $release_tag
    }
    configure_file './Doxyfile.in' './Doxyfile' @{'@VERSION@'=$release_tag; '@HTML_OUTPUT@' = "manual/$ver"}

    doxygen "./Doxyfile"

    Copy-Item './hacks.js' $html_out
    Copy-Item './doc_style.css' "$html_out/stylesheet.css"
    configure_file './menu_version.js.in' "$html_out/menu_version.js" @{'@VERLIST@' = $strVerList; '@VERSION@' = $ver}
}

# set default doc ver to 'latest'
configure_file './doc_index.html.in' "$site_dist/manual/index.html" @{'@VERSION@' = 'latest'}

# build home site
mkdirs "$site_dist/assets/css"
Copy-Item './style.css'  "$site_dist/assets/css/style.css"
Copy-Item './index.html' "$site_dist/index.html"

Set-Location $store_cwd

# build html targets to docs
./build.ps1 -p wasm

Copy-Item 'build_wasm/bin/'
