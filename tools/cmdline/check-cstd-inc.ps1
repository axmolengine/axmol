<#
.SYNOPSIS
  Check for C++ wrapper C headers (<c*>) forbidden in core engine & community extensions.

.DESCRIPTION
  Scans axmol/ and extensions/ (excluding spine, Live2D, Effekseer) for C++ source
  files that #include any <c*> header (C++ wrappers of C standard headers).
  In axmol code style these headers are forbidden — only the corresponding C headers
  (<*.h>) are allowed.

  Prints violating files and saves the list to tools/cmdline/check-cstd-files.txt.
  Throws an error if any violations are found (for CI failure).
#>

param(
)

$ErrorActionPreference = 'Stop'

$AX_ROOT = Resolve-Path "$PSScriptRoot/../.."

$SearchDirs = @(
    (Join-Path $AX_ROOT "axmol"),
    (Join-Path $AX_ROOT "tests/cpp-tests/Source"),
    (Join-Path $AX_ROOT "tests/live2d-tests/Source"),
    (Join-Path $AX_ROOT "tests/fairygui-tests/Source"),
    (Join-Path $AX_ROOT "tests/lua-tests/Source"),
    (Join-Path $AX_ROOT "tests/unit-tests/Source"),
    (Join-Path $AX_ROOT "templates"),
    (Join-Path $AX_ROOT "extensions")
)

$ExcludePatterns = @(
    [regex]::Escape((Join-Path $AX_ROOT "extensions/spine")),
    [regex]::Escape((Join-Path $AX_ROOT "extensions/Live2D")),
    [regex]::Escape((Join-Path $AX_ROOT "extensions/Effekseer"))
)

$FileTypes = @("*.c", "*.cc", "*.cpp", "*.h", "*.hpp", "*.hh", "*.inl", "*.mm", "*.m")

# C++ wrapper C headers (C++17 retained, excluding those removed in C++17)
$CHeaders = @(
    'cassert', 'cctype', 'cerrno', 'cfenv', 'cfloat', 'cinttypes',
    'climits', 'clocale', 'csetjmp', 'csignal',
    'cstdarg', 'cstdint', 'cstdio', 'cstdlib',
    'cstring', 'ctime', 'cuchar', 'cwchar', 'cwctype'
)

$Pattern = '#include\s+<(' + ($CHeaders -join '|') + ')>'

$Violations = @()

foreach ($dir in $SearchDirs) {
    if (-not (Test-Path $dir)) { continue }
    Get-ChildItem -Path $dir -Recurse -Include $FileTypes | ForEach-Object {
        $file = $_.FullName
        foreach ($ex in $ExcludePatterns) {
            if ($file -match $ex) { return }
        }
        $content = Get-Content $file -Raw
        if ($content -match $Pattern) {
            $Violations += $file
            Write-Host "VIOLATION: $file"
        }
    }
}

if ($Violations.Count -gt 0) {
    Write-Host ""
    Write-Host "Found $($Violations.Count) file(s) using forbidden C++ wrapper C headers."
    Write-Host "List saved to: $OutputFile"
    throw "CI FAILURE: $($Violations.Count) file(s) violate the <c*> header ban."
} else {
    Write-Host "No violations found. All good."
}
