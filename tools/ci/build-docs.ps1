
# Can runs on Windows,Linux
param(
    $site_dist = $null,
    $min_ver = '2.4' # The minimum version to build docs
)

$myRoot = $PSScriptRoot

$ErrorActionPreference = 'Stop'

$isWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

$AX_ROOT = (Resolve-Path $myRoot/../..)

$git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
if (!$git_prog) {
    throw 'build-docs: requires git installed'
}

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
    $doxygen_ver = '1.14.0'

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

# begin generate docs

$latest_branch = $(git -C $AX_ROOT branch --show-current)
if (!$latest_branch) {
    throw "please checkout a branch as latest doc version"
}

$remote_default_branch = (git symbolic-ref refs/remotes/origin/HEAD) -replace '^refs/remotes/origin/', ''
Write-Host "latest_branch=$latest_branch, remote_default_branch=$remote_default_branch"

# parse revision of current head ref to x.y.z-beef123
function parse_current_rev() {
    $axver_file = Join-Path $AX_ROOT 'axmol/axmolver.h.in'
    if (!(Test-Path $axver_file -PathType Leaf)) {
        $axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
    }
    $axver_content = $(Get-Content -Path $axver_file)

    $parse_axver = {
        param($part)
        return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
    }

    $axver = "$(&$parse_axver 'MAJOR').$(&$parse_axver 'MINOR').$(&$parse_axver 'PATCH')"

    $branch_name = $(git -C $AX_ROOT branch --show-current)
    if ($branch_name.StartsWith('dev/') -or $branch_name.StartsWith('release/')) {
        $short_sha = $(git -C $AX_ROOT rev-parse --short=7 HEAD)
        $axver += "-$short_sha"
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

# collection ver_map
# key   (doc_ver)
# value (head_ref)
$release_tags = $(git tag)
$ver_map = @{}

$ver_map['latest'] = $latest_branch

foreach ($item in $release_tags) {
    if ([Regex]::Match($item, '^v[0-9]+\.[0-9]+\.[0-9]+$').Success) {
        $doc_ver = $($item.Split('.')[0..1] -join '.').TrimStart('v')
        if ($doc_ver -lt $min_ver) {
            continue
        }
        $ver_map[$doc_ver] = $item
    }
}

$ver_list = $ver_map.GetEnumerator() | Sort-Object Key -Descending | ForEach-Object {
    [PSCustomObject]@{
        doc_ver  = $_.Key
        head_ref = $_.Value
    }
}
$ver_list | Format-Table doc_ver, head_ref -AutoSize

$menu_ver_list = ($ver_list | ForEach-Object { "'$($_.doc_ver)'" }) -join ','

# set default doc ver to 'latest'
mkdirs "$site_dist/manual"
configure_file './doc_index.html.in' "$site_dist/manual/index.html" @{'@VERSION@' = 'latest' }

function generate_docs($ver_info) {
    $doc_ver = $ver_info.doc_ver
    $head_ref = $ver_info.head_ref

    # checkout
    Write-Host "Checking out to ${doc_ver}:${head_ref} ..."
    git checkout "$head_ref" --force | Out-Host
    $current_ref = git symbolic-ref --short HEAD 2>/dev/null || git describe --tags --exact-match 2>/dev/null
    if ($current_ref -ne $head_ref -and $LASTEXITCODE -ne 0) {
        throw "git checkout `"$head_ref`" failed or did not switch HEAD"
    }
    if ($doc_ver -eq 'latest') {
        $real_ver = parse_current_rev
    }
    else {
        $real_ver = $head_ref
    }

    Write-Host "Generating docs for ${doc_ver}:${head_ref} ..."

    # prepare html_out
    $html_out = Join-Path $site_dist "manual/$doc_ver"
    mkdirs $html_out

    # configure file
    configure_file './Doxyfile.in' './Doxyfile' @{'@VERSION@' = $real_ver; '@OUTPUT_DIR@' = $site_dist; './dist' = $site_dist; '@HTML_OUTPUT@' = "manual/$doc_ver" }

    # generate docs
    doxygen "./Doxyfile" # 1>$null 2>$null

    Copy-Item './hacks.js' $html_out
    Copy-Item './doc_style.css' "$html_out/stylesheet.css"
    configure_file './menu_version.js.in' "$html_out/menu_version.js" @{'@VERLIST@' = $menu_ver_list; '@VERSION@' = $doc_ver }

    Write-Host "Generating docs for ${doc_ver}:${real_ver} done"
}

# build manuals
foreach ($ver in $ver_list) {
    generate_docs $ver
}

# checkout back to latest branch
git checkout $latest_branch

Set-Location $store_cwd
