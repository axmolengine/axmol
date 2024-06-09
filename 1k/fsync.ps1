# sync file or directory
# .\fsync.ps1 -s srcPath -d destPath -l 1
param(
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$srcPath,
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$destPath,
    [Parameter(Mandatory=$false, ValueFromPipeline=$true)]
    [PSDefaultValue(Value=$null)]
    $linkOnly
)


function ParseBoolFuzzy($value) {
    $value = "$value".ToLower()
    return $value.startsWith('1') -or $value.StartsWith('t') -or $value.StartsWith('y')
}

$linkOnly = ParseBoolFuzzy($linkOnly)

# 0: windows, 1: linux, 2: macos
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

# convert to native path style
if ($IsWin) {
    $srcPath = $srcPath.Replace('/', '\')
    $destPath = $destPath.Replace('/', '\')
} else {
    $srcPath = $srcPath.Replace('\', '/')
    $destPath = $destPath.Replace('\', '/')
}

if(!$srcPath -or !(Test-Path $srcPath -PathType Any)) {
    throw "fsync.ps1: The source directory $srcPath not exist"
}

if (Test-Path $destPath -PathType Any) { # dest already exist
    if ($linkOnly) { # is symlink and dest exist
        $fileItem = (Get-Item $destPath)
        if ($fileItem.Target -eq $srcPath) {
            Write-Host "fsync.ps1: Symlink $destPath ===> $($fileItem.Target) exists"
            return
        }
        Write-Host "fsync.ps1: Removing old link target $($fileItem.Target)"
        # force delete if exist dest not symlink
        if ($fileItem.PSIsContainer -and !$fileItem.Target) {
            $fileItem.Delete($true)
        } else {
            $fileItem.Delete()
        }
    }
}

$destLoc = Split-Path $destPath -Parent
if (!(Test-Path $destLoc -PathType Container)) {
    New-Item $destLoc -ItemType Directory 1>$null
}

if ($linkOnly) {
    Write-Host "fsync.ps1: Symlink $srcPath to $destPath ..."
    if ($IsWin -and (Test-Path $srcPath -PathType Container)) {
        cmd.exe /c mklink /J $destPath $srcPath
    }
    else {
        # ln -s $srcPath $destPath
        New-Item -ItemType SymbolicLink -Path $destPath -Target $srcPath 2>$null
    }
}
else { # copy
    Write-Host "fsync.ps1: Copying $srcPath to $destPath ..."
    if (Test-Path $srcPath -PathType Container) {
        if (!(Test-Path $destPath -PathType Container)) {
            Copy-Item $srcPath $destPath -Recurse -Force
        } else {
            Copy-Item $srcPath/* $destPath/ -Recurse -Force
        }
    } else {
        Copy-Item $srcPath $destPath -Force
    }
}

if(!$?) { # $Error array
    exit 1
}
