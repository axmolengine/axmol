<#
.SYNOPSIS
  Scan and optionally fix C/C++ source includes to match the new 'axmol/' convention.

.DESCRIPTION
  - Detects outdated include paths that should be rewritten to 'axmol/<relative>' form.
  - Handles:
      * Prefix-less includes that point to files under axmol/
      * Wrong includes containing 'axmol/core/...' (remove 'core/' segment)
  - Optionally scans <> includes.
  - Works from repository root, scanning target folders recursively.
  - Never processes 'core/...', as these never existed.

.PARAMETER Fix
  Actually rewrite files instead of only reporting.

.PARAMETER Backup
  When fixing, save a .bak file before modification.

.PARAMETER AngleBrackets
  Also scan #include <...> lines.
#>

param(
    $ScanDirs,
    [switch]$Fix,
    [switch]$Backup,
    [switch]$AngleBrackets
)

$ErrorActionPreference = 'Stop'

# ----- CONFIGURATION -----
$AX_ROOT    = Resolve-Path $env:AX_ROOT

$AxmolDir    = Join-Path $AX_ROOT "axmol"
if(!$ScanDirs) {
    $ScanDirs = @("axmol", "extensions", "templates", "tests") # relative to repo root
}
$FileTypes   = "*.c","*.cc","*.cpp","*.mm","*.m","*.h","*.hpp","*.hh","*.inl"

# Old include search paths — mimic original compiler search order (adjust as needed)
$OldIncludePaths = @(
    "$AX_ROOT",
    "$AX_ROOT/3rdparty",
    "$AX_ROOT/axmol",
    "$AX_ROOT/axmol/platform",
    "$AX_ROOT/axmol/audio"
)

# Matches both #include and #import
$IncludePattern = if ($AngleBrackets) {
    '^\s*#\s*(?:include|import)\s*[<"]([^">]+)[">]'
} else {
    '^\s*#\s*(?:include|import)\s*"([^"]+)"'
}

# Track report
$Report = @()

foreach ($folder in $ScanDirs) {
    $TargetDir = (Resolve-Path $folder -ErrorAction SilentlyContinue)
    if($TargetDir) {
        $TargetDir = $TargetDir.Path
    }
    else {
        $TargetDir = Join-Path $AX_ROOT $folder
        $TargetDir = (Resolve-Path $TargetDir).Path
    }
    if (-not (Test-Path $TargetDir)) { continue }
    Get-ChildItem -Path $TargetDir -Recurse -Include $FileTypes | ForEach-Object {
        $FilePath = $_.FullName
        $Lines = Get-Content $FilePath
        $Changed = $false

        echo "Scaning $FilePath ..."

        for ($i=0; $i -lt $Lines.Count; $i++) {
            if ($Lines[$i] -match $IncludePattern) {
                $IncludePath = $Matches[1]

                # Resolve to actual file
                $Resolved = $null
                $IncludePaths = @((Split-Path $FilePath))
                $IncludePaths += $OldIncludePaths
                foreach ($searchDir in $IncludePaths) {
                    $Candidate = Join-Path $searchDir $IncludePath
                    if (Test-Path $Candidate) {
                        $Resolved = (Resolve-Path $Candidate).Path
                        break
                    }
                }

                # not found — skip
                if (-not $Resolved) { 
                    # throw "incorrect include dir: $IncludePath in $FilePath"
                    continue
                }

                # Check if it's under axmol/
                if ($Resolved -like "$AxmolDir*") {
                    $RelPath = $Resolved.Substring($AxmolDir.Length + 1) -replace '\\','/'

                    # Normalize: strip any leading 'core/' after axmol/
                    if ($RelPath -match '^core/') {
                        $RelPath = $RelPath.Substring(5)
                    }

                    $NewInclude = "axmol/$RelPath"

                    if ($IncludePath -ne $NewInclude) {
                        # Log
                        $Report += [pscustomobject]@{
                            File     = $FilePath
                            Line     = $i + 1
                            Old      = $IncludePath
                            New      = $NewInclude
                        }

                        if ($Fix) {
                            if ($Backup -and -not $Changed) {
                                Copy-Item $FilePath "$FilePath.bak" -Force
                            }
                            $Lines[$i] = $Lines[$i] -replace [regex]::Escape($IncludePath), $NewInclude
                            $Changed = $true
                        }
                    }
                }
                # else {
                #     Write-Host "Skip include dir: $IncludePath not in axmol/"
                # }
            }
        }

        if ($Fix -and $Changed) {
            Set-Content -Path $FilePath -Value $Lines -Encoding UTF8
        }
    }
}

# ----- REPORT -----
if ($Report.Count -gt 0) {
    $Report | Format-Table -AutoSize
    Write-Host "$($Report.Count) files scaned, preform fix: $Fix"
} else {
    Write-Host "No outdated includes found."
}
