# --err-format=msvc --lang=hlsl --profile=50 --no-suffix --auto-map-bindings --auto-map-locations "--defines=AXSLC_TARGET_HLSL" --include-dirs=D:/dev/simdsoft/axmol3/axmol/renderer/shaders --frag=D:/dev/simdsoft/axmol3/axmol/renderer/shaders/videoTextureYUY2.frag --output=D:/dev/simdsoft/axmol3-hlsl/videoTextureYUY2.frag
param(
    [string] $BaseDir,
    [string[]] $InputDirs
)

$AX_ROOT = (Resolve-Path "$PSScriptRoot/../..").Path

$IncludeDir = Join-Path $AX_ROOT "axmol/renderer/shaders"

if (-not $BaseDir) {
    $BaseDir = $AX_ROOT
}
Push-Location $BaseDir

# TODO:
$NormMapShaders = @{
    'colorNormalTexture.frag'        = $true
    'positionNormalTexture.vert'     = $true
    'skinPositionNormalTexture.vert' = $true
}

$OuputFolder = Split-Path $BaseDir -Leaf

# Path to axslcc.exe (relative to engine root)
$axslcc = Join-Path $AX_ROOT 'tools/external/axslcc/axslcc.exe'

# Default input directories if none are provided
if (-not $InputDirs -or $InputDirs.Count -eq 0) {
    $InputDirs = @("axmol", "extensions", "tests")
}

# Output root directory (parallel to engine root)
$OutputParent = (Resolve-Path "$BaseDir/..").Path
$outputRoot = Join-Path $OutputParent "$OuputFolder-hlsl"

foreach ($dir in $InputDirs) {
    $searchPath = (Resolve-Path $dir -ErrorAction SilentlyContinue)

    # Find shader files with specific extensions
    Get-ChildItem -Path $searchPath -Recurse -Include *.frag, *.vert, *.vsh, *.fsh | ForEach-Object {
        $inFile = $_.FullName

        # Compute relative path from engine root
        $relPath = $inFile.Substring($BaseDir.Length).TrimStart('\', '/')

        # Output file path (preserve directory structure and filename)
        $outFile = Join-Path $outputRoot $relPath

        # Ensure output directory exists
        $outDir = Split-Path $outFile
        if (!(Test-Path $outDir)) {
            New-Item -ItemType Directory -Path $outDir | Out-Null
        }

        Write-Host "Converting $relPath -> $outFile"

        $compileArgs = @(
            '-g'
            '--silent'
            '--err-format=msvc'
            '--lang=hlsl'
            '--profile=51'
            '--no-suffix'
            '--auto-map-bindings'
            '--auto-map-locations'
            "--defines=AXSLC_TARGET_HLSL=1,MAX_DIRECTIONAL_LIGHT_NUM=1,MAX_POINT_LIGHT_NUM=1,MAX_SPOT_LIGHT_NUM=1"
            "--include-dirs=$IncludeDir"
        )

        if ($outFile.EndsWith('.frag') -or $outFile.EndsWith(".fsh")) {
            $compileArgs += "--frag=$inFile"
        }
        else {
            $compileArgs += "--vert=$inFile"
        }

        $compileArgs += "--output=$outFile"

        # Call axslcc.exe with required arguments
        & $axslcc $compileArgs
    }
}

Pop-Location
