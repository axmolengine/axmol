param(
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$srcDir,
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$destDir,
    [Parameter(Mandatory=$false, ValueFromPipeline=$true)]
    [PSDefaultValue(Value=$null)]
    $linkOnly,
    $wasm = $null
)


function ParseBoolFuzzy($value) {
    $value = "$value".ToLower()
    return $value.startsWith('1') -or $value.StartsWith('t') -or $value.StartsWith('y')
}

$wasm = ParseBoolFuzzy($wasm)
$linkOnly = ParseBoolFuzzy($linkOnly)

if ($wasm -and $linkOnly) {
    Write-Host "sync_folder.ps1: Skipping symlink folder for target platform 'wasm'"
    return 
}

# 0: windows, 1: linux, 2: macos
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

# convert to native path style
if ($IsWin) {
    $srcDir = $srcDir.Replace('/', '\')
    $destDir = $destDir.Replace('/', '\')
} else {
    $srcDir = $srcDir.Replace('\', '/')
    $destDir = $destDir.Replace('\', '/')
}

if(!$srcDir -or !(Test-Path $srcDir -PathType Container)) {
    throw "sync_folder.ps1: The source directory $srcDir not exist"
}

if (Test-Path $destDir -PathType Container) { # dest already exist
    if ($linkOnly) { # is symlink and dest exist
        $directoryInfo = (Get-Item $destDir)
        if ($directoryInfo.Target -eq $srcDir) {
            Write-Host "sync_folder.ps1: Symlink $destDir ===> $($directoryInfo.Target) exists"
            return
        }
        Write-Host "sync_folder.ps1: Removing old link target $($directoryInfo.Target)"
        # Remove-Item -Path $destDir
        $directoryInfo.Delete($false)
    }
}

if ($linkOnly) {
    Write-Host "sync_folder.ps1: Linking $srcDir to $destDir ..."
    if ($IsWin) {
        cmd.exe /c mklink /J $destDir $srcDir
    }
    else {
        # ln -s $srcDir $destDir
        New-Item -ItemType SymbolicLink -Path $destDir -Target $srcDir
    }
}
else { # copy directory, remove first?
    Write-Host "sync_folder.ps1: Copying $srcDir to $destDir ..."
    if (!(Test-Path $destDir -PathType Container)) {
        Copy-Item $srcDir $destDir -Recurse -Force
    } else {
        Copy-Item $srcDir/* $destDir/ -Recurse -Force
    }
}
