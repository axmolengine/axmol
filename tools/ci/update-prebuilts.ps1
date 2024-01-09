$VER=$args[0]

function println($msg) {
    Write-Host $msg
}

println "VER=$VER"

$AX_ROOT=(Resolve-Path "$PSScriptRoot/../..").Path
println "AX_ROOT=$AX_ROOT"

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

function mkdirs([string]$path) {
    if (!(Test-Path $path -PathType Container)) {
        New-Item $path -ItemType Directory 1>$null
    }
}

function download_file($url, $out, $force = $false) {
    if(Test-Path $out -PathType Leaf) { 
        if (!$force) {
            return
        }
        Remove-Item $out
    }
    Write-Host "Downloading $url to $out ..."
    if ([System.Version]$pwsh_ver -ge [System.Version]'7.0.0.0') {
        curl -L $url -o $out
    }
    else {
        Invoke-WebRequest -Uri $url -OutFile $out
    }
}

function download_and_expand($url, $out, $dest) {
    download_file $url $out
    if($out.EndsWith('.zip')) {
        Expand-Archive -Path $out -DestinationPath $dest
    } elseif($out.EndsWith('.tar.gz')) {
        if (!$dest.EndsWith('/')) {
            mkdirs $dest
        }
        tar xvf "$out" -C $dest
    } elseif($out.EndsWith('.sh')) {
        chmod 'u+x' "$out"
        mkdirs $dest
    }
}

# download version manifest
Set-Location $AX_ROOT

$cwd = $(Get-Location).Path

$prefix = Join-Path $cwd "tmp/1kdist_$VER"

mkdirs $prefix

# ensure yaml parser module
if ($null -eq (Get-Module -ListAvailable -Name powershell-yaml)) {
    Install-Module -Name powershell-yaml -Force -Repository PSGallery -Scope CurrentUser
}

# check upstream prebuilts version
download_file "https://github.com/simdsoft/1kiss/releases/download/$VER/_1kiss.yml" "./tmp/_1kiss.yml" $true
$newVerList = ConvertFrom-Yaml -Yaml (Get-Content './tmp/_1kiss.yml' -raw)
if ($newVerList.GetType() -eq [string]) {
    throw "Download version manifest file _1kiss.yml fail"
}

$manifest_old_hash = Get-FileHash -Path './manifest.json' -Algorithm MD5
$readme_old_hash = Get-FileHash -Path './thirdparty/README.md.in' -Algorithm MD5

$myVerList = ConvertFrom-Json (Get-Content './manifest.json' -raw)

println "Updating manifest.json, thirdparty/README.md ..."

# update README.md
$content = $(Get-Content -Path ./thirdparty/README.md.in -raw)
foreach ($item in $newVerList.GetEnumerator() )
{
    $key = ([Regex]::Replace($item.Name, '-', '_')).ToUpper()
    $key = "${key}_VERSION"
    $content = $content -replace "\$\{$key\}",$item.Value
}

Set-Content -Path ./thirdparty/README.md -Value "$content"

$myVerList.versions.PSObject.Properties['1kdist'].Value = $VER.TrimStart('v')
$content = (ConvertTo-Json $myVerList).Replace("`r`n", "`n")
$content += "`n"
Set-Content -Path './manifest.json' -Value "$content" -NoNewline

$manifest_new_hash = Get-FileHash -Path './manifest.json' -Algorithm MD5
$readme_new_hash = Get-FileHash -Path './thirdparty/README.md.in' -Algorithm MD5

$modified = ($manifest_new_hash.Hash -ne $manifest_old_hash.Hash) -or ($readme_new_hash.Hash -ne $readme_old_hash.Hash)

if($modified) {
    if ("$env.RUNNER_OS" -ne "") {
        Write-Output "AX_PREBUILTS_NO_UPDATE=true" >> $GITHUB_ENV
    }
}
