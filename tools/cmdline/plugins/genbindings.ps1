#requires -Version 7.4

param(
    [switch]$Verify,
    [string]$ClangSharpRoot = $env:AXMOL_CLANGSHARP_ROOT,
    [string]$LibClangRoot = $env:AXMOL_LIBCLANG_ROOT,
    [string]$GeneratorAssembly = $env:AXMOL_LUA_GENERATOR_ASSEMBLY,
    [Alias('m')]
    [string]$Module = 'all',
    [string]$OutputDirectory,
    [string[]]$ExtraClangArguments = @(),
    [string]$NdkRoot = $env:ANDROID_NDK,
    [string]$TargetTriple = $(if ($env:AXMOL_CLANG_TARGET) { $env:AXMOL_CLANG_TARGET } else { 'armv7-none-linux-androideabi' }),
    [int]$AndroidApi = $(if ($env:AXMOL_ANDROID_API) { [int]$env:AXMOL_ANDROID_API } else { 21 })
)

$ErrorActionPreference = 'Stop'
$axmolRoot = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$OutputDirectory = if ($OutputDirectory) { (New-Item -ItemType Directory -Force -Path $OutputDirectory).FullName } else { Join-Path $axmolRoot 'extensions/scripting/lua-bindings/generated' }
$generatorRoot = Join-Path $axmolRoot 'tools/bindings-generator'
$adapterAllowlistPath = Join-Path $axmolRoot 'tools/lua-bindings/manual-adapters.json'

function Get-PlatformLibClangName {
    if ($IsWindows) { return 'libclang.dll' }
    if ($IsMacOS) { return 'libclang.dylib' }
    return 'libclang.so'
}

function Get-1kissLlvmVersion {
    $buildProfiles = Join-Path $axmolRoot '1k/build.profiles'
    $match = Select-String -LiteralPath $buildProfiles -Pattern '^\s*llvm\s*=\s*([^#\s]+)' | Select-Object -First 1
    if (-not $match) { throw "The LLVM version is not declared in $buildProfiles." }
    return ($match.Matches[0].Groups[1].Value -replace '[+~].*$', '')
}

function Get-1kissMirror {
    $envFile = Join-Path $axmolRoot '1k/.env'
    if (Test-Path $envFile -PathType Leaf) {
        $match = Select-String -LiteralPath $envFile -Pattern '^active_mirror=(.+)$' | Select-Object -First 1
        if ($match) { return $match.Matches[0].Groups[1].Value.Trim() }
    }
    return 'origin'
}

function Get-1kissLibClangPackage([string]$Version) {
    $sourcesPath = Join-Path $axmolRoot '1k/sources.json'
    $sources = Get-Content -LiteralPath $sourcesPath -Raw | ConvertFrom-Json
    $definition = $sources.devtools.libclang
    if (-not $definition) { throw "1kiss does not define the libclang devtool in $sourcesPath." }
    $mirror = Get-1kissMirror
    $baseUrl = $definition.sources.$mirror
    if (-not $baseUrl) {
        Write-Warning "1kiss mirror '$mirror' is not configured for libclang; falling back to origin."
        $baseUrl = $definition.sources.origin
    }
    $artifact = ([string]$definition.artifacts).Replace('${ver}', $Version)
    return @{ Url = "$baseUrl$artifact"; FileName = $artifact }
}

function Ensure-1kissLibClang {
    $libraryName = Get-PlatformLibClangName
    $prebuiltDirectory = Join-Path $axmolRoot 'tools/bindings-generator/clang/prebuilt'
    $libraryPath = Join-Path $prebuiltDirectory $libraryName
    $llvmVersion = Get-1kissLlvmVersion

    if (Test-Path $libraryPath -PathType Leaf) {
        $versionScript = Join-Path $PSScriptRoot 'clang-ver.ps1'
        $versionText = (& pwsh -NoProfile -File $versionScript -DllPath $libraryPath | Select-Object -Last 1).ToString()
        $versionMajor = if ($versionText -match '(\d+)\.') { [int]$Matches[1] } else { 0 }
        $requiredMajor = [int]($llvmVersion.Split('.')[0])
        if ($versionMajor -eq $requiredMajor) {
            return $libraryPath
        }
        Write-Warning "Ignoring stale $libraryPath (libclang $versionText; 1kiss requires LLVM $llvmVersion)."
        Remove-Item -LiteralPath $libraryPath -Force
    }

    $package = Get-1kissLibClangPackage $llvmVersion
    $cacheDirectory = Join-Path $axmolRoot 'cache/devtools'
    $packagePath = Join-Path $cacheDirectory $package.FileName
    New-Item -ItemType Directory -Force -Path $cacheDirectory | Out-Null
    New-Item -ItemType Directory -Force -Path $prebuiltDirectory | Out-Null
    if (-not (Test-Path $packagePath -PathType Leaf)) {
        Write-Host "1kiss: downloading libclang $llvmVersion from $($package.Url)"
        Invoke-WebRequest -Uri $package.Url -OutFile $packagePath
    }
    if (-not (Get-Command 7z -ErrorAction SilentlyContinue)) {
        throw '7z is required to extract the 1kiss libclang archive. Run 1kiss setup first or install 7-Zip.'
    }

    $extractDirectory = Join-Path $cacheDirectory "llvm-$llvmVersion"
    if (-not (Test-Path $extractDirectory -PathType Container)) {
        New-Item -ItemType Directory -Force -Path $extractDirectory | Out-Null
        & 7z x $packagePath "-o$extractDirectory" -y | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "1kiss: failed to extract $packagePath." }
    }
    $archiveRelativePath = if ($IsWindows) { 'lib/win32/x64/libclang.dll' } elseif ($IsMacOS) { 'lib/mac/libclang.dylib' } else { 'lib/linux/libclang.so' }
    $archiveLibraries = @(
        (Join-Path $extractDirectory "llvm-$llvmVersion/$archiveRelativePath"),
        (Join-Path $extractDirectory $archiveRelativePath))
    $archiveLibrary = $archiveLibraries | Where-Object { Test-Path $_ -PathType Leaf } | Select-Object -First 1
    if (-not $archiveLibrary) {
        throw "1kiss libclang archive does not contain ${archiveRelativePath}: $packagePath"
    }
    Copy-Item -LiteralPath $archiveLibrary -Destination $libraryPath -Force
    return $libraryPath
}

$managedInteropVersion = '21.1.8.3'

function Find-ClangSharpInteropAssembly {
    $candidates = @()
    if ($ClangSharpRoot -and (Test-Path $ClangSharpRoot -PathType Container)) {
        $candidates += Get-ChildItem -LiteralPath $ClangSharpRoot -Filter 'ClangSharp.Interop.dll' -File -Recurse -ErrorAction SilentlyContinue
    }
    $candidates += Get-ChildItem -LiteralPath (Join-Path $generatorRoot 'bin') -Filter 'ClangSharp.Interop.dll' -File -Recurse -ErrorAction SilentlyContinue
    $userProfile = if ($env:USERPROFILE) { $env:USERPROFILE } else { [Environment]::GetFolderPath([Environment+SpecialFolder]::UserProfile) }
    $nugetRoot = if ($env:NUGET_PACKAGES) { $env:NUGET_PACKAGES } else { Join-Path $userProfile '.nuget/packages' }
    $candidates += Get-ChildItem -LiteralPath (Join-Path $nugetRoot "clangsharp.interop/$managedInteropVersion/lib") -Filter 'ClangSharp.Interop.dll' -File -Recurse -ErrorAction SilentlyContinue
    return ($candidates | Select-Object -First 1).FullName
}

function Ensure-ClangSharpInteropAssembly {
    $existing = Find-ClangSharpInteropAssembly
    if ($existing) { return (Resolve-Path $existing).Path }

    $cacheDirectory = Join-Path $axmolRoot 'cache/devtools'
    $packageName = "clangsharp.interop.$managedInteropVersion.nupkg"
    $packagePath = Join-Path $cacheDirectory $packageName
    $packageUrl = "https://api.nuget.org/v3-flatcontainer/clangsharp.interop/$managedInteropVersion/$packageName"
    New-Item -ItemType Directory -Force -Path $cacheDirectory | Out-Null
    if (-not (Test-Path $packagePath -PathType Leaf)) {
        Write-Host "Axmol Lua binding generator: downloading ClangSharp.Interop $managedInteropVersion"
        Invoke-WebRequest -Uri $packageUrl -OutFile $packagePath
    }

    Add-Type -AssemblyName System.IO.Compression
    $extractDirectory = Join-Path $cacheDirectory "clangsharp.interop-$managedInteropVersion"
    $interopPath = Join-Path $extractDirectory 'lib/net8.0/ClangSharp.Interop.dll'
    if (-not (Test-Path $interopPath -PathType Leaf)) {
        New-Item -ItemType Directory -Force -Path (Split-Path $interopPath -Parent) | Out-Null
        $archive = [IO.Compression.ZipFile]::OpenRead($packagePath)
        try {
            $entry = $archive.GetEntry('lib/net8.0/ClangSharp.Interop.dll')
            if (-not $entry) { throw "ClangSharp.Interop $managedInteropVersion does not contain its net8.0 assembly." }
            [IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $interopPath, $true)
        }
        finally {
            $archive.Dispose()
        }
    }
    if (-not (Test-Path $interopPath -PathType Leaf)) {
        throw "ClangSharp.Interop assembly was not found after extracting $packagePath."
    }
    return (Resolve-Path $interopPath).Path
}

function Get-PowerShellReferenceAssemblies {
    $referenceDirectory = Join-Path $PSHOME 'ref'
    if (-not (Test-Path $referenceDirectory -PathType Container)) { return @() }
    return @(Get-ChildItem -LiteralPath $referenceDirectory -Filter '*.dll' -File | ForEach-Object FullName)
}

function Build-GeneratorAssemblyWithPowerShell([string]$OutputPath, [System.IO.FileInfo[]]$Sources) {
    # Keep this bootstrap independent of the .NET SDK.  PowerShell 7 ships
    # the Roslyn compiler used by Add-Type, which is the supported clean-host
    # path for genbindings.
    $interopAssembly = Ensure-ClangSharpInteropAssembly
    $temporaryOutput = Join-Path ([IO.Path]::GetTempPath()) "Axmol.LuaBindings.Generator.$PID.dll"
    if (Test-Path $temporaryOutput -PathType Leaf) { Remove-Item -LiteralPath $temporaryOutput -Force }
    $references = @(Get-PowerShellReferenceAssemblies) + @($interopAssembly)
    Write-Host 'Axmol Lua binding generator: compiling C# sources with PowerShell Add-Type'
    try {
        $sourcePaths = @($Sources | Where-Object Extension -EQ '.cs' | ForEach-Object FullName)
        Add-Type -Path $sourcePaths `
            -ReferencedAssemblies $references `
            -OutputAssembly $temporaryOutput `
            -OutputType Library `
            -CompilerOptions @('/unsafe', '/langversion:latest', '/nullable:enable', '/nowarn:1701') `
            -IgnoreWarnings
        if (-not (Test-Path $temporaryOutput -PathType Leaf)) {
            throw 'PowerShell Add-Type completed without producing the generator assembly.'
        }
        New-Item -ItemType Directory -Force -Path (Split-Path $OutputPath -Parent) | Out-Null
        Copy-Item -LiteralPath $temporaryOutput -Destination $OutputPath -Force
        $interopDestination = Join-Path (Split-Path $OutputPath -Parent) (Split-Path $interopAssembly -Leaf)
        if ([IO.Path]::GetFullPath($interopAssembly) -ne [IO.Path]::GetFullPath($interopDestination)) {
            Copy-Item -LiteralPath $interopAssembly -Destination $interopDestination -Force
        }
    }
    finally {
        if (Test-Path $temporaryOutput -PathType Leaf) { Remove-Item -LiteralPath $temporaryOutput -Force }
    }
}

if ([string]::IsNullOrWhiteSpace($GeneratorAssembly)) {
    $localGeneratorAssembly = Join-Path $axmolRoot 'tools/bindings-generator/bin/Release/net8.0/Axmol.LuaBindings.Generator.dll'
    $localAssemblyExists = Test-Path $localGeneratorAssembly -PathType Leaf
    $generatorSources = @(Get-ChildItem -LiteralPath (Join-Path $generatorRoot 'src') -Filter '*.cs' -File -Recurse)
    $generatorNeedsBuild = -not $localAssemblyExists
    if ($localAssemblyExists) {
        $assemblyTimestamp = (Get-Item -LiteralPath $localGeneratorAssembly).LastWriteTimeUtc
        $generatorNeedsBuild = $null -ne ($generatorSources | Where-Object { $_.LastWriteTimeUtc -gt $assemblyTimestamp } | Select-Object -First 1)
    }

    if ($generatorNeedsBuild) {
        Build-GeneratorAssemblyWithPowerShell $localGeneratorAssembly $generatorSources
    }
    $GeneratorAssembly = $localGeneratorAssembly
}
function Configure-ClangRuntime([string]$Root) {
    if ([string]::IsNullOrWhiteSpace($Root)) {
        $Root = Ensure-1kissLibClang
        $library = (Resolve-Path $Root).Path
        $resolvedRoot = Split-Path $library -Parent
        $runtimePath = $resolvedRoot
        if ($IsWindows) { $env:PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:PATH" }
        elseif ($IsMacOS) { $env:DYLD_LIBRARY_PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:DYLD_LIBRARY_PATH" }
        else { $env:LD_LIBRARY_PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:LD_LIBRARY_PATH" }
        Write-Host "libclang runtime: $library"
        return $library
    }
    $resolvedRoot = (Resolve-Path $Root).Path
    if (-not (Test-Path $resolvedRoot -PathType Container)) {
        throw "AXMOL_LIBCLANG_ROOT is not a directory: $Root"
    }
    # The generator calls libclang directly through ClangSharp.Interop. There
    # is deliberately no libClangSharp native shim in this deployment path.
    $libraryName = Get-PlatformLibClangName
    $librarySearchRoots = @($resolvedRoot)
    foreach ($subdirectory in @('lib', 'bin')) {
        $candidate = Join-Path $resolvedRoot $subdirectory
        if (Test-Path $candidate -PathType Container) { $librarySearchRoots += $candidate }
    }
    $library = $null
    foreach ($libraryRoot in $librarySearchRoots) {
        $library = Get-ChildItem $libraryRoot -Filter $libraryName -File -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($library) { break }
    }
    if (-not $library) {
        throw "libclang runtime was not found in $resolvedRoot. Expected $libraryName."
    }
    # Keep both the 1kiss root and the directory containing libclang in the
    # search path.  Packaged libclang builds may keep dependent LLVM libraries
    # beside libclang under a `lib` or `bin` subdirectory.
    $runtimeRoots = @($library.DirectoryName, $resolvedRoot) | Select-Object -Unique
    $runtimePath = [string]::Join([IO.Path]::PathSeparator, $runtimeRoots)
    if ($IsWindows) { $env:PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:PATH" }
    elseif ($IsMacOS) { $env:DYLD_LIBRARY_PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:DYLD_LIBRARY_PATH" }
    else { $env:LD_LIBRARY_PATH = "$runtimePath$([IO.Path]::PathSeparator)$env:LD_LIBRARY_PATH" }
    Write-Host "libclang runtime: $resolvedRoot"
    return $library.FullName
}

$libClangLibrary = Configure-ClangRuntime $LibClangRoot

if (-not (Test-Path $GeneratorAssembly -PathType Leaf)) {
    throw "Generator assembly was not found: $GeneratorAssembly"
}
$GeneratorAssembly = (Resolve-Path $GeneratorAssembly).Path

$loadedGeneratorTypes = @(Add-Type -Path $GeneratorAssembly -PassThru)
$generatorType = $loadedGeneratorTypes | Where-Object FullName -EQ 'Axmol.LuaBindings.BindingGenerator' | Select-Object -First 1
$batchType = $loadedGeneratorTypes | Where-Object FullName -EQ 'Axmol.LuaBindings.BatchBindingGenerator' | Select-Object -First 1
if (-not $generatorType -or -not $batchType) {
    throw "The generator assembly does not expose the required Axmol.LuaBindings API: $GeneratorAssembly"
}
if ($libClangLibrary) {
    $generatorType::ConfigureLibClang($libClangLibrary)
}

Write-Host "Axmol Lua binding generator loaded from $GeneratorAssembly"
Write-Host "Verify=$Verify Module=$Module"

function Resolve-Ndk([string]$Root) {
    if ($Root -and (Test-Path $Root -PathType Container)) { return (Resolve-Path $Root).Path }
    $sdkRoots = @($env:ANDROID_HOME, $env:ANDROID_SDK_ROOT,
        (Join-Path $axmolRoot 'tools/external/android-sdk')) |
        Where-Object { $_ -and (Test-Path $_ -PathType Container) } |
        Select-Object -Unique
    foreach ($sdkRoot in $sdkRoots) {
        $candidate = Get-ChildItem (Join-Path $sdkRoot 'ndk') -Directory -ErrorAction SilentlyContinue |
            Sort-Object Name -Descending | Select-Object -First 1
        if ($candidate) { return $candidate.FullName }
    }
    return $null
}

function Ensure-Ndk {
    $ndk = Resolve-Ndk $NdkRoot
    if ($ndk) { return $ndk }

    $setupScript = Join-Path $axmolRoot 'setup.ps1'
    if (-not (Test-Path $setupScript -PathType Leaf)) {
        throw "Axmol setup script was not found: $setupScript"
    }
    Write-Host 'Android NDK was not found; invoking setup.ps1 -p android as the legacy genbindings flow did.'
    & pwsh -NoProfile -File $setupScript -p android
    if ($LASTEXITCODE -ne 0) {
        throw "Axmol Android toolchain setup failed with exit code $LASTEXITCODE."
    }
    $ndk = Resolve-Ndk $NdkRoot
    if (-not $ndk) {
        throw 'setup.ps1 completed but no Android NDK was found. Set ANDROID_NDK or pass -NdkRoot explicitly.'
    }
    return $ndk
}

function Find-LlvmInclude([string]$Ndk) {
    $prebuilt = Get-ChildItem (Join-Path $Ndk 'toolchains/llvm/prebuilt') -Directory -ErrorAction Stop |
        Where-Object { $_.Name -match '^(darwin|linux|windows)' } | Select-Object -First 1
    if (-not $prebuilt) { throw "LLVM prebuilt directory was not found below $Ndk." }
    $stdarg = Get-ChildItem $prebuilt.FullName -Filter stdarg.h -File -Recurse -ErrorAction Stop | Select-Object -First 1
    if (-not $stdarg) { throw "Clang builtin include directory was not found below $($prebuilt.FullName)." }
    return @{ Root = $prebuilt.FullName; Builtin = $stdarg.DirectoryName }
}

function Ensure-AxmolConfiguration {
    $versionHeader = Join-Path $axmolRoot 'axmol/axmolver.h'
    if (Test-Path $versionHeader -PathType Leaf) { return }

    $axmolScript = Join-Path $axmolRoot 'tools/cmdline/axmol.ps1'
    if (-not (Test-Path $axmolScript -PathType Leaf)) {
        throw "Axmol command script was not found: $axmolScript"
    }

    Write-Host 'Axmol Lua binding generator: configuring Axmol to generate axmolver.h'
    Push-Location $axmolRoot
    try {
        & pwsh -NoProfile -File $axmolScript -c
        if ($LASTEXITCODE -ne 0) {
            throw "Axmol CMake configuration failed with exit code $LASTEXITCODE."
        }
    }
    finally {
        Pop-Location
    }
    if (-not (Test-Path $versionHeader -PathType Leaf)) {
        throw "Axmol configuration completed but did not generate $versionHeader."
    }
}

    $ndk = Ensure-Ndk
    Ensure-AxmolConfiguration
    $llvm = Find-LlvmInclude $ndk
    $includeRoots = @(
        $axmolRoot,
        (Join-Path $axmolRoot 'platform/android'),
        (Join-Path $axmolRoot 'extensions'),
        (Join-Path $axmolRoot 'extensions/scripting'),
        (Join-Path $axmolRoot '3rdparty'),
        (Join-Path $axmolRoot '3rdparty/yasio'),
        (Join-Path $axmolRoot '3rdparty/fmt/include'),
        (Join-Path $axmolRoot '3rdparty/robin-map/include'),
        (Join-Path $axmolRoot '3rdparty/jni.hpp/include'),
        (Join-Path $axmolRoot '3rdparty/glad/include'),
        (Join-Path $axmolRoot '3rdparty/box2d/include'),
        (Join-Path $axmolRoot 'extensions/Particle3D/src'),
        (Join-Path $axmolRoot 'extensions/spine/runtime/include'),
        (Join-Path $axmolRoot 'extensions/spine/src'),
        (Join-Path $axmolRoot 'extensions/sceneext/src'),
        (Join-Path $axmolRoot 'extensions/sceneio/src'),
        (Join-Path $axmolRoot 'extensions/fairygui/src'),
        (Join-Path $axmolRoot 'extensions/fairygui/src/fairygui'),
        (Join-Path $axmolRoot 'extensions/GUI/src'),
        (Join-Path $axmolRoot '3rdparty/lua'),
        (Join-Path $axmolRoot '3rdparty/lua/plainlua'),
        (Join-Path $axmolRoot '3rdparty/lua/lua-cjson'),
        (Join-Path $axmolRoot '3rdparty/llhttp/include'),
        (Join-Path $axmolRoot '3rdparty/websocket-parser'),
        (Join-Path $axmolRoot '3rdparty/bullet'),
        (Join-Path $axmolRoot '3rdparty/jolt'),
        (Join-Path $llvm.Root 'sysroot/usr/include/c++/v1'),
        $llvm.Builtin,
        (Join-Path $llvm.Root 'sysroot/usr/include'),
        (Join-Path $llvm.Root 'sysroot/usr/include/arm-linux-androideabi')
    )
    $clangArguments = [System.Collections.Generic.List[string]]::new()
    $clangArguments.AddRange([string[]]@(
        '-nostdinc', '-x', 'c++', '-std=c++23', '-fsigned-char', '-target', $TargetTriple,
        '-Wno-pragma-once-outside-header', '-Wno-unknown-attributes'))
    $clangArguments.AddRange([string[]]@(
        '-D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS', '-DANDROID', "-D__ANDROID_API__=$AndroidApi",
        '-DAX_ENABLE_MEDIA=1', '-D_AX_GEN_SCRIPT_BINDINGS=1', '-D__cpp_coroutines=201703',
        '-D__builtin_neon_vbslq_f16(...)=(float16x8_t{})',
        '-D__builtin_neon_vbsl_f16(...)=(float16x4_t{})',
        '-D__builtin_neon_vtrnq_f16(...)', '-D__builtin_neon_vtrn_f16(...)',
        '-D__builtin_neon_vuzpq_f16(...)', '-D__builtin_neon_vuzp_f16(...)',
        '-D__builtin_neon_vzipq_f16(...)', '-D__builtin_neon_vzip_f16(...)',
        '-D__builtin_neon_vceqzq_f16(...)=(uint16x8_t{})',
        '-D__builtin_neon_vcgez_f16(...)=(uint16x4_t{})',
        '-D__builtin_neon_vcgtzq_f16(...)=(uint16x8_t{})',
        '-D__builtin_neon_vcgtz_f16(...)=(uint16x4_t{})',
        '-D__builtin_neon_vclez_f16(...)=(uint16x4_t{})',
        '-D__builtin_neon_vcltzq_f16(...)=(uint16x8_t{})',
        '-D__builtin_neon_vceqz_f16(...)=(uint16x4_t{})',
        '-D__builtin_neon_vcgezq_f16(...)=(uint16x8_t{})',
        '-D__builtin_neon_vclezq_f16(...)=(uint16x8_t{})',
        '-D__builtin_neon_vcltz_f16(...)=(uint16x4_t{})'))
    if ($ExtraClangArguments.Count -gt 0) { $clangArguments.AddRange($ExtraClangArguments) }
    foreach ($includeRoot in $includeRoots) { $clangArguments.Add('-isystem'); $clangArguments.Add($includeRoot) }

$batchRequest = [Axmol.LuaBindings.BatchGenerationRequest]::new()
$batchRequest.RepositoryRoot = $axmolRoot
$batchRequest.ConfigurationDirectory = Join-Path $axmolRoot 'tools/lua-bindings'
$batchRequest.OutputDirectory = $OutputDirectory
$batchRequest.AdapterAllowlistPath = $adapterAllowlistPath
$batchRequest.Module = $Module
$batchRequest.ClangArguments = [string[]]$clangArguments.ToArray()
$batchRequest.Verify = $Verify

$batchResult = $batchType::Generate($batchRequest)
foreach ($result in $batchResult.Results) {
    $errors = @($result.Diagnostics | Where-Object Severity -eq 'error')
    Write-Host "$($result.Module): $($result.Classes.Count) classes, $($result.GeneratedFiles.Count) files, $($errors.Count) errors"
    $result.Diagnostics |
        Where-Object Severity -ne 'error' |
        ForEach-Object { Write-Warning "$($_.Severity): $($_.Message)" }
}
foreach ($diagnostic in $batchResult.Diagnostics | Where-Object Severity -eq 'error') {
    Write-Host "error: $($diagnostic.Message)"
}
if (@($batchResult.Diagnostics | Where-Object Severity -eq 'error').Count -gt 0) {
    throw 'Lua binding generation failed. Existing generated files were not used as a fallback.'
}
