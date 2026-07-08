<#
.SYNOPSIS
  Combined source code checks: forbidden C++ wrapper C headers & UTF-8 BOM.

.DESCRIPTION
  Check 1 - C++ wrapper C headers (<c*>) forbidden in core engine & community extensions.
  Check 2 - UTF-8 BOM (byte order mark) forbidden in source files.

  Prints all violations and throws if any are found (for CI failure).
#>

$ErrorActionPreference = 'Stop'

$AX_ROOT = Resolve-Path "$PSScriptRoot/../.."

# ============================================================
# Check 1: Forbidden C++ wrapper C headers
# ============================================================
function Check-CStdHeaders {
    $searchDirs = @(
        (Join-Path $AX_ROOT "axmol"),
        (Join-Path $AX_ROOT "tests/cpp-tests/Source"),
        (Join-Path $AX_ROOT "tests/live2d-tests/Source"),
        (Join-Path $AX_ROOT "tests/fairygui-tests/Source"),
        (Join-Path $AX_ROOT "tests/lua-tests/Source"),
        (Join-Path $AX_ROOT "tests/unit-tests/Source"),
        (Join-Path $AX_ROOT "templates"),
        (Join-Path $AX_ROOT "extensions")
    )

    $excludePatterns = @(
        [regex]::Escape((Join-Path $AX_ROOT "extensions/spine")),
        [regex]::Escape((Join-Path $AX_ROOT "extensions/Live2D")),
        [regex]::Escape((Join-Path $AX_ROOT "extensions/Effekseer"))
    )

    $fileTypes = @("*.c", "*.cc", "*.cpp", "*.h", "*.hpp", "*.hh", "*.inl", "*.mm", "*.m")

    $cHeaders = @(
        'cassert', 'cctype', 'cerrno', 'cfenv', 'cfloat', 'cinttypes',
        'climits', 'clocale', 'csetjmp', 'csignal',
        'cstdarg', 'cstdint', 'cstdio', 'cstdlib',
        'cstring', 'ctime', 'cuchar', 'cwchar', 'cwctype'
    )

    $pattern = '#include\s+<(' + ($cHeaders -join '|') + ')>'

    $violations = @()

    foreach ($dir in $searchDirs) {
        if (-not (Test-Path $dir)) { continue }
        Get-ChildItem -Path $dir -Recurse -Include $fileTypes | ForEach-Object {
            $file = $_.FullName
            foreach ($ex in $excludePatterns) {
                if ($file -match $ex) { return }
            }
            $content = Get-Content $file -Raw
            if ($content -match $pattern) {
                $violations += $file
                Write-Host "[CSTD] VIOLATION: $file"
            }
        }
    }

    return $violations
}

# ============================================================
# Check 2: UTF-8 BOM
# ============================================================
function Check-BOM {
    $searchDirs = @(
        (Join-Path $AX_ROOT "axmol"),
        (Join-Path $AX_ROOT "tests"),
        (Join-Path $AX_ROOT "templates"),
        (Join-Path $AX_ROOT "extensions")
    )

    $excludePatterns = @(
        [regex]::Escape((Join-Path $AX_ROOT "extensions/spine")),
        [regex]::Escape((Join-Path $AX_ROOT "extensions/Live2D")),
        [regex]::Escape((Join-Path $AX_ROOT "extensions/Effekseer"))
    )

    $fileTypes = @("*.h", "*.cpp", "*.hpp", "*.hh", "*.inl", "*.mm", "*.m", "*.json")

    $bomBytes = [byte[]]@(0xEF, 0xBB, 0xBF)
    $violations = @()

    foreach ($dir in $searchDirs) {
        if (-not (Test-Path $dir)) { continue }
        Get-ChildItem -Path $dir -Recurse -Include $fileTypes | ForEach-Object {
            $file = $_.FullName
            foreach ($ex in $excludePatterns) {
                if ($file -match $ex) { return }
            }
            # Read first 3 raw bytes and compare to UTF-8 BOM
            $stream = [System.IO.File]::OpenRead($file)
            $header = [byte[]]::new(3)
            if ($stream.Read($header, 0, 3) -eq 3) {
                $hasBom = ($header[0] -eq $bomBytes[0]) -and ($header[1] -eq $bomBytes[1]) -and ($header[2] -eq $bomBytes[2])
            } else {
                $hasBom = $false
            }
            $stream.Close()
            if ($hasBom) {
                $violations += $file
                Write-Host "[BOM] VIOLATION: $file"
            }
        }
    }

    return $violations
}

# ============================================================
# Main
# ============================================================
$cstdViolations = Check-CStdHeaders
$bomViolations  = Check-BOM
$totalCount     = $cstdViolations.Count + $bomViolations.Count

if ($totalCount -gt 0) {
    Write-Host ""
    Write-Host "=============================="
    Write-Host "Found $totalCount violation(s):"
    Write-Host "  C++ wrapper C headers: $($cstdViolations.Count)"
    Write-Host "  UTF-8 BOM:             $($bomViolations.Count)"
    Write-Host "=============================="
    throw "CI FAILURE: $totalCount source violation(s) found."
} else {
    Write-Host "No violations found. All good."
}
