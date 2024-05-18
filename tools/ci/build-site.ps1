
# Can runs on Windows,Linux
$wasm_artifact_dir = (Resolve-Path $args[0]).Path
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

$axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
$axver_content = $(Get-Content -Path $axver_file)
function parse_axver($part) {
    return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
}

function query_axmol_latest() {
    
    $axver = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR').$(parse_axver 'PATCH')"

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

# build site_dist2 aka isolated site wasm demos preview with pthread support
$site_dist2 = Join-Path $site_src 'dist2'
$wasm_dist2 = Join-Path $site_dist2 'wasm/'
mkdirs $wasm_dist2
Copy-Item $(Join-Path $site_src 'isolated.html') $(Join-Path $site_dist2 'index.html')
Copy-Item $(Join-Path $site_src '_headers') $site_dist2
function copy_tree_if($source, $dest) {
    if (Test-Path $source) {
        Copy-Item $source $dest -Container -Recurse
    }
}

$cpp_tests_dir = $(Join-Path $wasm_artifact_dir 'cpp-tests')
if (!(Test-Path $cpp_tests_dir -PathType Container)) {
    throw "Missing wasm cpp-tests, caused by last wasm ci build fail."
}
copy_tree_if $cpp_tests_dir $wasm_dist2
copy_tree_if $(Join-Path $wasm_artifact_dir 'fairygui-tests') $wasm_dist2
copy_tree_if $(Join-Path $wasm_artifact_dir 'HelloLua') $wasm_dist2

# build manuals

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


# set default doc ver to 'latest'
mkdirs "$site_dist/manual"
configure_file './doc_index.html.in' "$site_dist/manual/index.html" @{'@VERSION@' = 'latest'}

# build home site
mkdirs "$site_dist/assets/css"
Copy-Item './style.css'  "$site_dist/assets/css/style.css"
Copy-Item './index.html' "$site_dist/index.html"

# copy logo used by site home page
Copy-Item './logo.png' "$site_dist/logo.png"

$branches = $(git branch -a)
$canon_branches = @{}
foreach($branch in $branches) {
    $canon_branches[$branch.Trim()] = $true
}

# build manuals
foreach($item in $verMap.GetEnumerator()) {
    $ver = $item.Key
    $html_out = Join-Path $site_dist "manual/$ver"
    mkdirs $html_out
    $release_tag = $item.Value

    if ($ver -eq 'latest') {
        git checkout dev
        $release_tag = query_axmol_latest
    } elseif($canon_branches.Contains($ver)) { # prefer LTS branch
        git checkout $ver
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

# redirect cpp_tests.html to isolated site
# don't modify, released news: gitee, github releases inuse
$fake_cpp_tests = Join-Path $site_dist 'axmol/wasm/cpp_tests/'
mkdirs $fake_cpp_tests
$fake_cpp_tests_html = Join-Path $fake_cpp_tests 'cpp_tests.html'
$redirect_content = @'
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Redirecting</title>
  <noscript>
    <meta http-equiv="refresh" content="1; url={0}" />
  </noscript>
  <script>
    window.location.href = '{0}';
  </script>
</head>
<body>
  Redirecting to <a href="{0}">{0}</a>
</body>
</html>
'@ -f 'https://axmol.netlify.app/wasm/cpp-tests/cpp-tests'
Set-Content -Path $fake_cpp_tests_html -Value $redirect_content

Set-Location $store_cwd
