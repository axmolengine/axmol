#!/usr/bin/env pwsh

param($ver = '')

if ($ver) {
    $Script:tool_cmd = "clang-format-$ver"
}
else {
    $Script:tool_cmd = 'clang-format'
}

&$tool_cmd --version

$AX_ROOT = $(Resolve-Path $PSScriptRoot/../..).Path
Push-Location $AX_ROOT

$sourceDirs = @('./axmol', './extensions', './tests', './templates')
$excludes = @(
    '3rdparty',
    'extensions/ImGui/**/im*',
    'extensions/fairygui',
    'extensions/Live2D',
    'extensions/Effekseer',
    'extensions/scripting/lua-bindings/auto',
    'extensions/spine',
    'tests/cpp-tests/Source/Box2DTestBed/samples',
    'tests/fairygui-tests',
    'tests/live2d-tests',
    'extensions/**/*_generated.h'
)
$extensions = @('*.h', '*.hpp', '*.cpp', '*.c', '.m', '*.mm')

# Build regex from exclude patterns (normalised to full path)
$excludeRegex = $excludes | ForEach-Object {
    # Convert glob-like pattern to regex: replace * with .*, / with \/ etc.
    [regex]::Escape($_) -replace '\\\*', '.*' -replace '\\\?', '.' -replace '\\\[', '[' -replace '\\\]', ']'
} | Join-String -Separator '|'

$files = Get-ChildItem -Path $sourceDirs -Include $extensions -Recurse -File |
    Where-Object { $_.FullName -replace '\\', '/' -notmatch $excludeRegex } |
    ForEach-Object { $_.FullName }

if (-not $files) {
    Write-Error "No files found to format"
    exit 1
}

Write-Output "=== Start formatting files with clang-format ==="

$runas_ci = "$env:GITHUB_ACTIONS" -eq 'true'
if ($runas_ci) {
    foreach($file in $files) {
        &$tool_cmd -i "$file"
    }
}
else {
    foreach($file in $files) {
        Write-Output "Formatting file: $file ..."
        &$tool_cmd -i "$file"
    }
}

Write-Output "=== Finished formatting $($files.Count) files ==="

Pop-Location
