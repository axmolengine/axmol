#!/usr/bin/env pwsh

param($ver = '')

$ErrorActionPreference = "Stop"

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
$extensions = @('*.h', '*.hpp', '*.cpp', '*.c', '*.m', '*.mm')

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

$batchSize = 100
$formated_cnt = 0
for ($i = 0; $i -lt $files.Count; $i += $batchSize) {
    $end = [Math]::Min($i + $batchSize - 1, $files.Count - 1)
    $batch = $files[$i..$end]

    Write-Output "Formatting $($batch.Count) files starting from index $i ..."
    &$tool_cmd -i $batch
    $formated_cnt += $batch.Count
}

Write-Output "=== Finished formatting $formated_cnt/$($files.Count) files ==="

Pop-Location
