param(
    $url,
    $name,
    $dest,
    $cfg
)

if (!$url -or !$name -or !$dest) {
    throw 'fetch.ps1: missing parameters'
}

$version_map = ConvertFrom-Json (Get-Content $cfg -raw)

$pkg_ver = $version_map.PSObject.Properties[$name].Value
if ($pkg_ver) {
    if (Test-Path $dest -PathType Container) {
        if (!(Test-Path $(Join-Path $dest '.git') -PathType Container)) {
            Remove-Item $dest -Recurse -Force
            git clone $url $dest
        }
    }
    else {
        git clone $url $dest
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
