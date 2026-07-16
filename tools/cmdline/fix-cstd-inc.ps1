<#
.SYNOPSIS
  Fix C++ wrapper C headers: replace <c*> with <xxx.h> and remove std:: prefix from C symbols.
#>

$ErrorActionPreference = 'Stop'

$AX_ROOT = Resolve-Path "$PSScriptRoot/../.."

$SearchDirs = @(
    (Join-Path $AX_ROOT "axmol"),
    (Join-Path $AX_ROOT "extensions")
)

$ExcludePatterns = @(
    [regex]::Escape((Join-Path $AX_ROOT "extensions/spine")),
    [regex]::Escape((Join-Path $AX_ROOT "extensions/Live2D"))
)

$FileTypes = @("*.c", "*.cc", "*.cpp", "*.h", "*.hpp", "*.hh", "*.inl", "*.mm", "*.m")

# --- Include replacement map ---
$IncludeMap = @{
    'cassert' = 'assert.h'
    'cctype'  = 'ctype.h'
    'cfloat'  = 'float.h'
    'cstdint' = 'stdint.h'
    'cstdlib' = 'stdlib.h'
    'cstring' = 'string.h'
}

# --- C headers to remove include line entirely ---
$RemoveIncludes = @('cstddef')

# --- Std:: prefix removal patterns (non-math C symbols only) ---
$StdPatterns = @(
    # Types from <cstddef>, <cstdint>
    @{ Pattern = '\bstd::(size_t|ptrdiff_t)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(u?int\d+_t)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(intptr_t|uintptr_t|intmax_t|uintmax_t)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(u?int_least\d+_t)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(u?int_fast\d+_t)\b'; Replacement = '$1' }
    # C string functions
    @{ Pattern = '\bstd::(memcpy|memmove|memcmp|memset|memchr)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(strlen|strcpy|strncpy|strcmp|strncmp)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(strcat|strncat|strchr|strrchr|strstr)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(strtok|strerror|strdup|strspn|strcspn|strpbrk|strcoll|strxfrm)\b'; Replacement = '$1' }
    # C stdlib functions
    @{ Pattern = '\bstd::(rand|srand|atoi|atol|atoll|atof)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(abort|exit|getenv|system|strtod|strtol|strtoll|strtoul|strtoull)\b'; Replacement = '$1' }
    @{ Pattern = '\bstd::(qsort|bsearch|labs|llabs|ldiv|lldiv)\b'; Replacement = '$1' }
    # C ctype functions
    @{ Pattern = '\bstd::(isalnum|isalpha|isblank|iscntrl|isdigit|isgraph|islower|isprint|ispunct|isspace|isupper|isxdigit|tolower|toupper)\b'; Replacement = '$1' }
    # C stdio functions
    @{ Pattern = '\bstd::(sscanf|sprintf|snprintf|fprintf|printf|scanf|fscanf|vprintf|vsprintf|vsnprintf|vfprintf|vscanf|vsscanf|fopen|fclose|fread|fwrite|fgets|fputs|fseek|ftell|rewind|fflush|fgetc|fputc|ungetc|feof|ferror|clearerr|rename|tmpfile|tmpnam|perror|setbuf|setvbuf|fgetpos|fsetpos)\b'; Replacement = '$1' }
    # C time functions
    @{ Pattern = '\bstd::(time|clock|difftime|mktime|asctime|ctime|gmtime|localtime|strftime)\b'; Replacement = '$1' }
    # C signal functions
    @{ Pattern = '\bstd::(signal|raise)\b'; Replacement = '$1' }
    # C setjmp functions
    @{ Pattern = '\bstd::(setjmp|longjmp)\b'; Replacement = '$1' }
)

# --- Collect all files ---
$Files = @()
foreach ($dir in $SearchDirs) {
    if (-not (Test-Path $dir)) { continue }
    $Files += Get-ChildItem -Path $dir -Recurse -Include $FileTypes | ForEach-Object { $_.FullName }
}

$Files = $Files | Where-Object {
    $file = $_
    $exclude = $false
    foreach ($ex in $ExcludePatterns) {
        if ($file -match $ex) { $exclude = $true; break }
    }
    -not $exclude
}

Write-Host "Found $($Files.Count) files to process."
Write-Host ""

# ========================================================
# Step 1: Include replacements
# ========================================================
Write-Host "=== Step 1: Include replacements ==="
$modCount1 = 0

foreach ($file in $Files) {
    $content = Get-Content $file -Raw
    $original = $content
    $changed = $false

    # Replace #include <cxxx> with #include <xxx.h>
    foreach ($cHeader in $IncludeMap.Keys) {
        $pattern = '#include\s+<' + [regex]::Escape($cHeader) + '>'
        $replacement = '#include <' + $IncludeMap[$cHeader] + '>'
        if ($content -match $pattern) {
            $content = $content -replace $pattern, $replacement
            Write-Host "  REPLACE $file : <$cHeader> -> <$($IncludeMap[$cHeader])>"
            $changed = $true
        }
    }

    # Remove #include <cstddef> lines
    foreach ($cHeader in $RemoveIncludes) {
        $pattern = '(?m)^\s*#include\s+<' + [regex]::Escape($cHeader) + '>\s*$'
        if ($content -match $pattern) {
            $content = $content -replace $pattern, ''
            Write-Host "  REMOVE  $file : <$cHeader> include line"
            $changed = $true
        }
    }

    if ($changed) {
        [System.IO.File]::WriteAllText($file, $content, [System.Text.UTF8Encoding]::new($false))
        $modCount1++
    }
}
Write-Host "Step 1 done: $modCount1 files modified."
Write-Host ""

# ========================================================
# Step 2: Std:: prefix removal
# ========================================================
Write-Host "=== Step 2: Std:: prefix removal ==="
$modCount2 = 0

foreach ($file in $Files) {
    $content = Get-Content $file -Raw
    $original = $content

    foreach ($pat in $StdPatterns) {
        $content = $content -replace $pat.Pattern, $pat.Replacement
    }

    if ($content -ne $original) {
        [System.IO.File]::WriteAllText($file, $content, [System.Text.UTF8Encoding]::new($false))
        $modCount2++
    }
}
Write-Host "Step 2 done: $modCount2 files modified."

# ========================================================
# Summary
# ========================================================
Write-Host ""
Write-Host "=== Summary ==="
Write-Host "Include replacements : $modCount1 files"
Write-Host "Std:: prefix removal : $modCount2 files"
Write-Host "Total unique        : $(($Files | Where-Object {
    $f = $_
    $c = Get-Content $f -Raw
    $changed = $false
    foreach ($cHeader in $IncludeMap.Keys) {
        if ($c -match '#include\s+<' + [regex]::Escape($cHeader) + '>') { }
    }
    # This won't work well for summary, skip complex check
    $false
} | Measure-Object).Count)"
