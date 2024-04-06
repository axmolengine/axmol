# fetch repo url by name
param(
    $name,
    $manifest_file
)

if(Test-Path $manifest_file -PathType Leaf) {
    $mirror = if (!(Test-Path (Join-Path $PSScriptRoot '.gitee') -PathType Leaf)) {'github'} else {'gitee'}
    $url_base = @{'github' = 'https://github.com/'; 'gitee' = 'https://gitee.com/' }[$mirror]

    $manifest_map = ConvertFrom-Json (Get-Content $manifest_file -raw)
    $ver = $manifest_map.versions.PSObject.Properties[$name].Value
    $url_path = $manifest_map.mirrors.PSObject.Properties[$mirror].Value.PSObject.Properties[$name].Value

    Write-Host "$url_base$url_path#$ver" -NoNewline
}
