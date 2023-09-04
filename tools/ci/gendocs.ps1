
# Can runs on Windows,Linux
$myRoot = $PSScriptRoot

$isWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

$AX_ROOT = (Resolve-Path $myRoot/../..)

function mkdirs([string]$path) {
    if (!(Test-Path $path)) {
        New-Item $path -ItemType Directory 1>$null
    }
}

function download_file($url, $out) {
    if(Test-Path $out -PathType Leaf) { return }
    Write-Host "Downloading $url to $out ..."
    if ($pwsh_ver -ge '7.0') {
        curl -L $url -o $out
    }
    else {
        Invoke-WebRequest -Uri $url -OutFile $out
    }
}

function download_zip_expand($url, $out, $dest = $null) {
    if (!$dest) {
        $dest = $(Split-Path $out -Parent)
    }
    mkdirs $dest
    download_file $url $out
    Expand-Archive -Path $out -DestinationPath $dest
}

$prefix = Join-Path $AX_ROOT 'tools/external'
if (!(Test-Path "$prefix" -PathType Container)) {
    mkdirs $prefix
}

function setup_doxygen() {
    $doxygen_ver = '1.9.7'

    $doxygen_pkg_name = if ($isWin) {"doxygen-$doxygen_ver.windows.x64.bin.zip"} else {"doxygen-$doxygen_ver.linux.bin.tar.gz"}
    $doxygen_pkg_path = Join-Path $prefix $doxygen_pkg_name
    
    if (!(Test-Path $doxygen_pkg_path -PathType Leaf)) {
        $doxygen_ver_ul = $doxygen_ver.Replace('.', '_')
        Invoke-WebRequest -Uri "https://github.com/doxygen/doxygen/releases/download/Release_$doxygen_ver_ul/$doxygen_pkg_name" -OutFile $doxygen_pkg_path | Out-Host
    }

    $doxygen_root = Join-Path $prefix "doxygen-$doxygen_ver"
    $doxygen_bin = $doxygen_root
    if (!(Test-Path $doxygen_root -PathType Container)) {
        if ($isWin) {
            mkdirs $doxygen_root
            Expand-Archive -Path $doxygen_pkg_path -DestinationPath $doxygen_root
        }
        else {
            tar xvf $doxygen_pkg_path -C $prefix
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

$site_src = (Resolve-Path "$myRoot/../../docs").Path
$site_dist = Join-Path $site_src 'dist'

mkdirs $site_dist

$store_cwd = (Get-Location).Path
Set-Location $site_src

function  configure_file($infile, $outfile, $vars) {
    $content = $(Get-Content $infile -raw)
    foreach($var in $vars.GetEnumerator()) {
        $content = [Regex]::Replace($content, $var.Key, $var.Value)
    }
    Set-Content -Path $outfile -Value "$content"
}

# query version map to build docs
$release_tags = $(git tag)
$verMap = @{'latest' = $null; }
foreach($item in $release_tags) {
    if ([Regex]::Match($item, '^v[0-9]+\.[0-9]+\.[0-9]+$').Success) {
        $docVer = $($item.Split('.')[0..1] -join '.').TrimStart('v')
        $verMap[$docVer] = $item
    }
}
$strVerList = "'$($verMap.Keys -join "','")'"
Write-Host "$(Out-String -InputObject $verMap)"

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

    Write-Host "Generating docs for $ver ..." -NoNewline
    doxygen "./Doxyfile" # 1>$null 2>$null
    Write-Host "done"

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

# deploy wasm cpp_tests demo
download_zip_expand 'https://ci.appveyor.com/api/projects/halx99/axmol/artifacts/build_wasm.zip?branch=dev' $(Join-Path $AX_ROOT 'tmp/build_wasm.zip')
$wasm_dist = Join-Path $site_dist 'wasm/'
mkdirs $wasm_dist
Copy-Item $(Join-Path $AX_ROOT 'tmp/build_wasm/bin/cpp_tests') $wasm_dist -Container -Recurse

Set-Location $store_cwd
