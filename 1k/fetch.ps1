param(
    $name,
    $dest,
    $cfg
)

if (!$name -or !$dest) {
    throw 'fetch.ps1: missing parameters'
}

$mirror = if (!(Test-Path (Join-Path $PSScriptRoot '.gitee') -PathType Leaf)) {'github'} else {'gitee'}
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

    $sentry = Join-Path $dest '_1kiss'
    # if sentry file missing, re-clone
    if (!(Test-Path $sentry -PathType Leaf)) {
        if (Test-Path $dest -PathType Container) {
            Remove-Item $dest -Recurse -Force
        }
        git clone $url $dest
        if ($? -and (Test-Path $(Join-Path $dest '.git') -PathType Container)) {
            [System.IO.File]::WriteAllText($sentry, "$(git -C $dest rev-parse HEAD)")
        } else {
            throw "fetch.ps1: execute git clone $url failed"
        }
    }

    $pkg_ver = $pkg_ver.Split('-')
    $use_hash = $pkg_ver.Count -gt 1
    $revision = $pkg_ver[$use_hash].Trim()
    $tag_info = git -C $dest tag | Select-String $revision
    if ($tag_info) {
        git -C $dest checkout ([array]$tag_info.Line)[0] 1>$null 2>$null
    }
    else {
        git -C $dest checkout $revision 1>$null 2>$null
    }
} else {
    throw "fetch.ps1: not found version for package ${name}"
}
