# fetch pkg by url or manifest.json path
param(
    $uri, # the pkg uri
    $prefix, # the prefix to store
    $manifest_file = $null,
    $name = $null,
    $version = $null, # version hint
    $revision = $null, # revision hint
    [switch]$pull_branch
)

# content of _1kiss with yaml format
# ver: 1.0
# branch: 1.x
# commits: 2802
# rev: 29b0b28

Set-Alias println Write-Host

if (!$uri -or !$prefix) {
    throw 'fetch.ps1: missing parameters'
}

function download_file($uri, $out) {
    if (Test-Path $out -PathType Leaf) { return }
    println "Downloading $uri to $out ..."
    Invoke-WebRequest -Uri $uri -OutFile $out
}

function mkdirs($path) {
    if (!(Test-Path $path -PathType Container)) {
        New-Item $path -ItemType Directory 1>$null
    }
}

# ensure cachedir
$cache_dir = Join-Path (Resolve-Path $PSScriptRoot/..).Path 'cache'
if (!(Test-Path $cache_dir -PathType Container)) {
    mkdirs $cache_dir
}

function fetch_repo($url, $name, $dest, $ext) {
    if ($ext -eq '.git') {
        git clone --progress $url $dest | Out-Host
    }
    else {
        $out = Join-Path $cache_dir "${name}$ext"
        download_file $url $out
        try {
            if ($ext -eq '.zip') {
                Expand-Archive -Path $out -DestinationPath $prefix -Force
            }
            else {
                tar xf "$out" -C $prefix
            }
        }
        catch {
            Remove-Item $out -Force
            throw "fetch.ps1: extract $out failed, try again"
        }

        if (!(Test-Path $dest -PathType Container)) {
            $original_lib_src = Join-Path $prefix $Script:url_pkg_name
            if (Test-Path $original_lib_src -PathType Container) {
                Rename-Item $original_lib_src $dest 
            }
            else {
                throw "fetch.ps1: the package name mismatch for $out"
            }
        }
    }
}

# parse url from $uri
$uriInfo = [array]$uri.Split('#')
$uri = $uriInfo[0]
if (!$version) {
    $version = $uriInfo[1]
}

$url = $null
if ($uri -match '^([a-z]+://|git@)') {
    $url = $uri
}
elseif ($uri.StartsWith('gh:')) {
    $url = "https://github.com/$($uri.substring(3))"
    if (!$url.EndsWith('.git')) { $url += '.git' }
}
elseif ($uri.StartsWith('gl:')) {
    $url = "https://gitlab.com/$($uri.substring(3))"
    if (!$url.EndsWith('.git')) { $url += '.git' }
}
else {
    $name = $uri
}

# simple match url/ssh schema
if (!$url) {
    # fetch package from manifest config
    $lib_src = Join-Path $prefix $name
    $mirror = if (!(Test-Path (Join-Path $PSScriptRoot '.gitee') -PathType Leaf)) { 'github' } else { 'gitee' }
    $url_base = @{'github' = 'https://github.com/'; 'gitee' = 'https://gitee.com/' }[$mirror]

    $manifest_map = ConvertFrom-Json (Get-Content $manifest_file -raw)

    if (!$version) {
        $version_map = $manifest_map.versions
        $version = $version_map.PSObject.Properties[$name].Value
    }
    if ($version) {
        $url_path = $manifest_map.mirrors.PSObject.Properties[$mirror].Value.PSObject.Properties[$name].Value
        if ($url_path) {
            $url = "$url_base/$url_path"
            if (!$url.EndsWith('.git')) { $url += '.git' }
        }
    }
}

if (!$url) {
    throw "fetch.ps1: can't determine package url of '$name'"
}

$url_pkg_ext = $null
$Script:url_pkg_name = $null
$match_info = [Regex]::Match($url, '(\.git)|(\.zip)|(\.tar\.(gz|bz2|xz))$')
if ($match_info.Success) {
    $url_pkg_ext = $match_info.Value
    $url_file_name = Split-Path $url -Leaf
    $Script:url_pkg_name = $url_file_name.Substring(0, $url_file_name.Length - $url_pkg_ext.Length)
    if (!$name) {
        $name = $Script:url_pkg_name
    }
}
else {
    throw "fetch.ps1: invalid url, must be endswith .git, .zip, .tar.xx"
}

$lib_src = Join-Path $prefix $name
$is_git_repo = $url_pkg_ext -eq '.git'
if (!$is_git_repo) {
    $match_info = [Regex]::Match($url, '(\d+\.)+(-)?(\*|\d+)')
    if ($match_info.Success) {
        $version = $match_info.Value
    }
}

if (!$version) {
    throw "fetch.ps1: can't determine package version of '$name'"
}

Set-Variable -Name "${name}_src" -Value $lib_src -Scope global

$sentry = Join-Path $lib_src '_1kiss'

$need_update = $false
# if sentry file missing, re-clone
if (!(Test-Path $sentry -PathType Leaf)) {
    if (Test-Path $lib_src -PathType Container) {
        Remove-Item $lib_src -Recurse -Force
    }

    fetch_repo -url $url -name $name -dest $lib_src -ext $url_pkg_ext
    
    if (Test-Path $lib_src -PathType Container) {
        New-Item $sentry -ItemType File 1>$null
        $need_update = $true
    }
    else {
        throw "fetch.ps1: fetch content from $url failed"
    }
}

# re-check does valid local git repo
if (!(Test-Path "$lib_src/.git" -PathType Container)) { $is_git_repo = $false }

# checkout revision for git repo
if (!$revision) {
    $ver_pair = [array]$version.Split('-')
    $use_hash = $ver_pair.Count -gt 1
    $revision = $ver_pair[$use_hash].Trim()
    $version = $ver_pair[0]
}

$branch_name = $null
if ($is_git_repo) {
    $old_rev_hash = $(git -C $lib_src rev-parse HEAD)
    
    $cur_rev_hash = $(git -C $lib_src rev-parse --verify --quiet "$revision^{}")
    if (!$cur_rev_hash) {
        git -C $lib_src fetch
        $cur_rev_hash = $(git -C $lib_src rev-parse --verify --quiet "$revision^{}")
        if (!$cur_rev_hash) {
            throw "fetch.ps1: Could not found commit hash of $revision"
        }
    }

    if ($old_rev_hash -ne $cur_rev_hash) {
        git -C $lib_src checkout $revision 1>$null 2>$null
        $new_rev_hash = $(git -C $lib_src rev-parse HEAD)
        println "fetch.ps1: Checked out to $revision@$new_rev_hash"
        
        if (!$need_update) {
            $need_update = $old_rev_hash -ne $new_rev_hash
        }

        if ((Test-Path (Join-Path $lib_src '.gitmodules') -PathType Leaf)) {
            git -C $lib_src submodule update --recursive --init
        }
    }

    if (!$need_update) { 
        $branch_name = $(git -C $lib_src branch --show-current)
        $need_update = ($branch_name -eq $revision) -and $pull_branch
    }

    if(!$need_update) {
        println "fetch.ps1: HEAD is now at $revision@$cur_rev_hash"
    }
}

if ($need_update) {
    $sentry_content = "ver: $version"
    if ($is_git_repo) {
        if ($branch_name) {
            # track branch
            git -C $lib_src pull
            $commits = $(git -C $lib_src rev-list --count HEAD)
            $sentry_content += "`nbranch: $branch_name"
            $sentry_content += "`ncommits: $commits"
            $revision = $(git -C $lib_src rev-parse --short=7 HEAD)
            $sentry_content += "`nrev: $revision"
            println "fetch.ps1: HEAD is now at $branch_name@$revision"
        }
    }

    [System.IO.File]::WriteAllText($sentry, $sentry_content)

    if ($is_git_repo) { git -C $lib_src add '_1kiss' }
}

# google gclient spec
if (Test-Path (Join-Path $lib_src '.gn') -PathType Leaf) {
    # the repo use google gn build system manage deps and build
    Push-Location $lib_src
    # angle (A GLES native implementation by google)
    if (Test-Path 'scripts/bootstrap.py' -PathType Leaf) {
        python scripts/bootstrap.py
    }
    # darwin (A WebGPU native implementation by google)
    if (Test-Path 'scripts/standalone.gclient' -PathType Leaf) {
        Copy-Item scripts/standalone.gclient .gclient -Force
    }
    gclient sync -D
    Pop-Location
}
