# fetch repo directly
param(
    [Alias("name")]
    $uri, # uri
    $prefix, # fetch dest repo prefix
    $version = $null, # version hint
    $revision = $null, # revision
    $cfg = $null
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


$cache_dir = Join-Path (Resolve-Path $PSScriptRoot/..).Path 'cache'

if (!(Test-Path $cache_dir -PathType Container)) {
    mkdirs $cache_dir
}

if (!$cfg) {
    # fetch by url directly
    $url = $uri
    $folder_name = (Split-Path $url -leafbase)
    if ($folder_name.EndsWith('.tar')) {
        $folder_name = $folder_name.Substring(0, $folder_name.length - 4)
    }

    $lib_src = Join-Path $prefix $folder_name

    function fetch_repo($url, $out) {
        if (!$url.EndsWith('.git')) {
            download_file $url $out
            if ($out.EndsWith('.zip')) {
                Expand-Archive -Path $out -DestinationPath $prefix
            }
            elseif ($out.EndsWith('.tar.gz')) {
                tar xf "$out" -C $prefix
            }
        }
        else {
            git clone $url $lib_src
            if (!(Test-Path $(Join-Path $lib_src '.git')) -and (Test-Path $lib_src -PathType Container)) {
                Remove-Item $lib_src -Recurse -Force 
            }
        }
    }

    $is_git_repo = $url.EndsWith('.git')
    $sentry = Join-Path $lib_src '_1kiss'

    $is_rev_modified = $false
    # if sentry file missing, re-clone
    if (!(Test-Path $sentry -PathType Leaf)) {
        if (Test-Path $lib_src -PathType Container) {
            Remove-Item $lib_src -Recurse -Force
        }

        if ($url.EndsWith('.tar.gz')) {
            $out_file = Join-Path $cache_dir "${folder_name}.tar.gz"
        }
        elseif ($url.EndsWith('.zip')) {
            $out_file = Join-Path $cache_dir "${folder_name}.zip"
        }
        else {
            $out_file = $null
        }

        fetch_repo -url $url -out $out_file
        
        if (Test-Path $lib_src -PathType Container) {
            New-Item $sentry -ItemType File
            $is_rev_modified = $true
        }
        else {
            throw "fetch.ps1: fetch content from $url failed"
        }
    }

    # checkout revision for git repo
    if ($is_git_repo) {
        $old_rev_hash = $(git -C $lib_src rev-parse HEAD)

        $pred_rev_hash = $(git -C $lib_src rev-parse --verify --quiet "$revision^{}")

        if(!$pred_rev_hash) {
            git -C $lib_src pull
            $pred_rev_hash = $(git -C $lib_src rev-parse --verify --quiet "$revision^{}")
            if(!$pred_rev_hash) {
                throw "Could not found commit hash of $revision"
            }
        }
        git -C $lib_src checkout $revision --force 1>$null 2>$null

        $new_rev_hash = $(git -C $lib_src rev-parse HEAD)
        
        if (!$is_rev_modified) {
            $is_rev_modified = $old_rev_hash -ne $new_rev_hash
        }
    }

    if ($is_rev_modified) {
        $sentry_content = "ver: $version"

        if ($is_git_repo) {
            $branch_name = $(git -C $lib_src branch --show-current)
            if ($branch_name) {
                # track branch
                git -C $lib_src pull
                $commits = $(git -C $lib_src rev-list --count HEAD)
                $sentry_content += "`nbranch: $branch_name"
                $sentry_content += "`ncommits: $commits"
                $revision = $(git -C $lib_src rev-parse --short=7 HEAD)
                $sentry_content += "`nrev: $revision"
            }
        }

        [System.IO.File]::WriteAllText($sentry, $sentry_content)

        git -C $lib_src add '_1kiss'
    }

    if (Test-Path (Join-Path $lib_src '.gn') -PathType Leaf) {
        # the repo use google gn build system manage deps and build
        Push-Location $lib_src
        if (Test-Path 'scripts/bootstrap.py' -PathType Leaf) {
            python scripts/bootstrap.py
        }
        gclient sync -D
        Pop-Location
    }
}
else {
    # fetch by config file
    $name = $uri
    $lib_src = Join-Path $prefix $name
    $mirror = if (!(Test-Path (Join-Path $PSScriptRoot '.gitee') -PathType Leaf)) { 'github' } else { 'gitee' }
    $url_base = @{'github' = 'https://github.com/'; 'gitee' = 'https://gitee.com/' }[$mirror]

    $manifest_map = ConvertFrom-Json (Get-Content $cfg -raw)
    $version_map = $manifest_map.versions
    $pkg_ver = $version_map.PSObject.Properties[$name].Value
    if ($pkg_ver) {
        $url_path = $manifest_map.mirrors.PSObject.Properties[$mirror].Value.PSObject.Properties[$name].Value
        if (!$url_path) {
            throw "fetch.ps1 missing mirror config for package: '$name'"
        }

        $url = "$url_base/$url_path"

        $sentry = Join-Path $lib_src '_1kiss'
        # if sentry file missing, re-clone
        if (!(Test-Path $sentry -PathType Leaf)) {
            if (Test-Path $lib_src -PathType Container) {
                Remove-Item $lib_src -Recurse -Force
            }
            git clone $url $lib_src
            if ($? -and (Test-Path $(Join-Path $lib_src '.git') -PathType Container)) {
                [System.IO.File]::WriteAllText($sentry, "$(git -C $lib_src rev-parse HEAD)")
            }
            else {
                throw "fetch.ps1: execute git clone $url failed"
            }
        }

        $pkg_ver = $pkg_ver.Split('-')
        $use_hash = $pkg_ver.Count -gt 1
        $revision = $pkg_ver[$use_hash].Trim()
        $tag_info = git -C $lib_src tag | Select-String $revision
        if ($tag_info) {
            git -C $lib_src checkout ([array]$tag_info.Line)[0] --force 1>$null 2>$null
        }
        else {
            git -C $lib_src checkout $revision --force 1>$null 2>$null
        }
        git -C $lib_src add '_1kiss'
    }
    else {
        throw "fetch.ps1: not found version for package ${name}"
    }
}
