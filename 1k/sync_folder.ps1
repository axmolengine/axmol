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

if(!$srcDir -or !(Test-Path $srcDir -PathType Container)) {
    throw "The source directory $srcDir not exist"
}

if (Test-Path $destDir -PathType Container) { # dest already exist
    if ($linkOnly) { # is symlink and dest exist
        return
    }
}

# convert to native path style
if ($IsWin) {
    $srcDir = $srcDir.Replace('/', '\')
    $destDir = $destDir.Replace('/', '\')
} else {
    $srcDir = $srcDir.Replace('\', '/')
    $destDir = $destDir.Replace('\', '/')
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
    $destLoc = Join-Path $(Split-Path $destDir -Parent) '/'
    Write-Host "Copying $srcDir into $destLoc ..."
    Copy-Item $srcDir $destLoc -Recurse -Force
}
