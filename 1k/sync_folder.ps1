# 0: windows, 1: linux, 2: macos
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

$options = @{
    s         = $null; 
    d         = $null;
    l         = $null;
}

$optName = $null
foreach ($arg in $args) {
    if (!$optName) {
        if ($arg.StartsWith('-')) { 
            $optName = $arg.SubString(1)
        }
    }
    else {
        if ($options.Contains($optName)) {
            $options[$optName] = $arg
        }
        else {
            $b1k.println("Warning: ignore unrecognized option: $optName")
        }
        $optName = $null
    }
}

if(!$options.s -or !(Test-Path $options.s -PathType Container)) {
    throw "The source directory $($options.s) not exist"
}

if(!$options.d) {
    throw "Missing dest directory"
}

if (Test-Path $options.d -PathType Container) { # dest already exist
    if ($options.l) { # is symlink and dest exist
        return
    }
}

# convert to native path style
if ($IsWin) {
    $srcDir = $options.s.Replace('/', '\')
    $destDir = $options.d.Replace('/', '\')
} else {
    $srcDir = $options.s.Replace('\', '/')
    $destDir = $options.d.Replace('\', '/')
}

if ($options.l) {
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
    Copy-Item $srcDir $destDir -Recurse -Force
}
