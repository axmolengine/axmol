param(
    $name,
    $cfg
)

$mirror = if (!(Test-Path (Join-Path $PSScriptRoot '.gitee') -PathType Leaf)) {'github'} else {'gitee'}
$url_base = @{'github' = 'https://github.com/'; 'gitee' = 'https://gitee.com/' }[$mirror]

$manifest_map = ConvertFrom-Json (Get-Content $cfg -raw)
$ver = $manifest_map.versions.PSObject.Properties[$name].Value
$url_path = $manifest_map.mirrors.PSObject.Properties[$mirror].Value.PSObject.Properties[$name].Value

Write-Host "$url_base$url_path#$ver" -NoNewline