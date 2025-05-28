
# Can runs on Windows,Linux
param(
    $site_dist = $null,
    $min_ver = '2.3' # The minimum version to build docs
)

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
    if (Test-Path $out -PathType Leaf) { return }
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
    $doxygen_ver = '1.13.2'

    $doxygen_pkg_name = if ($isWin) { "doxygen-$doxygen_ver.windows.x64.bin.zip" } else { "doxygen-$doxygen_ver.linux.bin.tar.gz" }
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
        $envPathSep = if ($isWin) { ';' } else { ':' }
        $env:PATH = "$doxygen_bin$envPathSep$env:PATH"
    }
}

setup_doxygen

Write-Host "Using doxygen $(doxygen --version)"

$axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
$axver_content = $(Get-Content -Path $axver_file)
function parse_axver($part) {
    return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
}

function query_axmol_latest() {
    
    $axver = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR').$(parse_axver 'PATCH')"

    $git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
    if ($git_prog) {
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
if (!$site_dist) {
    $site_dist = Join-Path $site_src 'dist'
}
mkdirs $site_dist

$store_cwd = (Get-Location).Path
Set-Location $site_src

function  configure_file($infile, $outfile, $vars) {
    $content = $(Get-Content $infile -raw)
    foreach ($var in $vars.GetEnumerator()) {
        $content = [Regex]::Replace($content, $var.Key, $var.Value)
    }
    Set-Content -Path $outfile -Value "$content"
}

# build manuals

# query version map to build docs
$release_tags = $(git tag)
$verMap = @{}
foreach ($item in $release_tags) {
    if ([Regex]::Match($item, '^v[0-9]+\.[0-9]+\.[0-9]+$').Success) {
        $docVer = $($item.Split('.')[0..1] -join '.').TrimStart('v')
        if ($docVer -lt $min_ver) {
            continue
        }
        $verMap[$docVer] = $item
    }
}

$verList = $verMap.GetEnumerator() | Sort-Object Value -Descending | ForEach-Object { $_.Key }

$verMap['latest'] = $null

$strVerList = "'latest','$($verList -join "','")'"
Write-Host "$(Out-String -InputObject $verMap)"

# set default doc ver to 'latest'
mkdirs "$site_dist/manual"
configure_file './doc_index.html.in' "$site_dist/manual/index.html" @{'@VERSION@' = 'latest' }

$branches = $(git branch -a)
$canon_branches = @{}
foreach ($branch in $branches) {
    $canon_branches[$branch.Trim()] = $true
}

# build manuals
foreach ($item in $verMap.GetEnumerator()) {
    $ver = $item.Key
    $html_out = Join-Path $site_dist "manual/$ver"
    mkdirs $html_out
    $release_tag = $item.Value

    if ($ver -eq 'latest') {
        git checkout dev
        $release_tag = query_axmol_latest
    }
    elseif ($canon_branches.Contains($ver)) {
        # prefer LTS branch
        git checkout $ver
    }
    else {
        git checkout $release_tag
    }
    configure_file './Doxyfile.in' './Doxyfile' @{'@VERSION@' = $release_tag; '@OUTPUT_DIR@' = $site_dist; './dist' = $site_dist; '@HTML_OUTPUT@' = "manual/$ver" }

    Write-Host "Generating docs for $ver ..." -NoNewline
    doxygen "./Doxyfile" # 1>$null 2>$null
    Write-Host "done"

    Copy-Item './hacks.js' $html_out
    Copy-Item './doc_style.css' "$html_out/stylesheet.css"
    configure_file './menu_version.js.in' "$html_out/menu_version.js" @{'@VERLIST@' = $strVerList; '@VERSION@' = $ver }
}

# checkout back to dev
git checkout dev

Set-Location $store_cwd
