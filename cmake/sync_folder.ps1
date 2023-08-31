param(
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$srcDir,
    [Parameter(Mandatory=$true, ValueFromPipeline=$true)]
    [string]$destDir,
    [Parameter(Mandatory=$false, ValueFromPipeline=$true)]
    [PSDefaultValue(Value=$null)]
    $linkOnly
)

function ParseBoolFuzzy($value) {
    return $value.startsWith('1') -or $value.StartsWith('t') -or $value.StartsWith('y')
}

# 0: windows, 1: linux, 2: macos
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$linkOnly = ParseBoolFuzzy("$linkOnly".ToLower())

# convert to native path style
if ($IsWin) {
    $srcDir = $srcDir.Replace('/', '\')
    $destDir = $destDir.Replace('/', '\')
} else {
    $srcDir = $srcDir.Replace('\', '/')
    $destDir = $destDir.Replace('\', '/')
}

if(!$srcDir -or !(Test-Path $srcDir -PathType Container)) {
    throw "The source directory $srcDir not exist"
}

if (Test-Path $destDir -PathType Container) { # dest already exist
    if ($linkOnly) { # is symlink and dest exist
        $directoryInfo = (Get-Item $destDir)
        if ($directoryInfo.Target -eq $srcDir) {
            Write-Host "Symlink $destDir ===> $($directoryInfo.Target) exists"
            return
        }
        Write-Host "Removing old link target $($directoryInfo.Target)"
        # Remove-Item -Path $destDir
        $directoryInfo.Delete($false)
    }
}

if ($linkOnly) {
    Write-Host "Linking $srcDir to $destDir ..."
    if ($IsWin) {
        cmd.exe /c mklink /J $destDir $srcDir
    }
    else {
        # ln -s $srcDir $destDir
        New-Item -ItemType SymbolicLink -Path $destDir -Target $srcDir
    }
}
else { # copy directory, remove first?
    Write-Host "Copying $srcDir to $destDir ..."
    if (!(Test-Path $destDir -PathType Container)) {
        Copy-Item $srcDir $destDir -Recurse -Force
    } else {
        Copy-Item $srcDir/* $destDir/ -Recurse -Force
    }
}
