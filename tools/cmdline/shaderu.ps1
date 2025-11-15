param(
    $ScanDirs,
    [switch]$Fix
)

$ErrorActionPreference = 'Stop'

# ----- CONFIGURATION -----
$AX_ROOT = Resolve-Path $env:AX_ROOT
if (!$ScanDirs) {
    $ScanDirs = @("axmol", "extensions", "tests") # relative to repo root
}

# Rule definitions:
# Vertex UBO -> set=0, binding=0
# Fragment UBO -> set=0, binding=1
# Sampler -> set=1, binding=keep source binding or default 0

# Supported file extensions for shader stage detection
$vertexExts = @(".vert", ".vs", ".vsh")
$fragmentExts = @(".frag", ".fs", ".fsh")

$samplerRegex = 'uniform\s+(sampler2D|sampler2DArray|samplerCube)\s+'
$uniformRegex = '"uniform\s+\w+\s*\{"'

# Scan target directory
foreach ($scanDir in $ScanDirs) {
    $TargetDir = (Resolve-Path $scanDir -ErrorAction SilentlyContinue)
    if ($TargetDir) {
        $TargetDir = $TargetDir.Path
    }
    else {
        $TargetDir = Join-Path $AX_ROOT $scanDir
        $TargetDir = (Resolve-Path $TargetDir).Path
    }
    if (-not (Test-Path $TargetDir)) { continue }
    $files = Get-ChildItem -Path $TargetDir -Recurse -Include *.vert, *.vs, *.vsh, *.frag, *.fs, *.fsh

    foreach ($file in $files) {
        echo "Scaning $file ..."
        $stage = ""
        if ($vertexExts -contains $file.Extension.ToLower()) {
            $stage = "vertex"
        }
        elseif ($fragmentExts -contains $file.Extension.ToLower()) {
            $stage = "fragment"
        }
        else {
            continue
        }

        $lines = Get-Content $file.FullName
        $newLines = @()
        $changed = $false

        foreach ($line in $lines) {
            $newLine = $line

            # Handle sampler2D, sampler2DArray, samplerCube
            if ($line -match $samplerRegex) {
                if ($line -match "layout\s*\(") {
                    if($line -notmatch "set\s*=") {
                        if ($line -match "binding\s*=") {
                            $newLine = $line -replace "layout\s*\(([^)]*)\)", "layout(set = 1, `$1)"
                            $changed = $true
                        }
                        else {
                            throw "(1)Invalid shader: $file"
                        }
                    }
                }
                else {
                    throw "(2)Invalid shader: $file"
                }
            }
            elseif ($line -match $uniformRegex) {
                if ($stage -eq "vertex" -and $line -notmatch "set\s*=") {
                    if ($line -match "layout\s*\(") {
                        # Merge into existing layout(...)
                        $newLine = $line -replace "layout\s*\(([^)]*)\)", "layout(`$1, set = 0, binding = 0)"
                    }
                    else {
                        # Add new layout(...)
                        $newLine = "layout(std140, set = 0, binding = 0) " + $line.Trim()
                    }
                    $changed = $true
                }
                elseif ($stage -eq "fragment" -and $line -notmatch "set\s*=") {
                    if ($line -match "layout\s*\(") {
                        $newLine = $line -replace "layout\s*\(([^)]*)\)", "layout(std140, set = 0, binding = 1)"
                    }
                    else {
                        $newLine = "layout(std140, set = 0, binding = 1) " + $line.Trim()
                    }
                    $changed = $true
                }
            }

            $newLines += $newLine
        }

        if ($changed) {
            if ($Fix) {
                $newLines | Set-Content $file.FullName
                Write-Host "[$stage] Fixed: $($file.FullName)"
            }
            else {
                Write-Host "[$stage] Would fix: $($file.FullName)"
                # Print modification details
                for ($i = 0; $i -lt $lines.Count; $i++) {
                    if ($lines[$i] -ne $newLines[$i]) {
                        Write-Host "  Line $($i+1):"
                        Write-Host "    Old: $($lines[$i])"
                        Write-Host "    New: $($newLines[$i])"
                    }
                }
            }
        }
    }

}
