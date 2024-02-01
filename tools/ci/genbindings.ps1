$myRoot = $PSScriptRoot

$llvm_ver = '15.0.7'

$AX_ROOT = (Resolve-Path $myRoot/../..).Path

# ensure prebuilt lib downloaded
Push-Location $AX_ROOT
$build_script = Join-Path $AX_ROOT 'build.ps1'
&$build_script -c
Pop-Location

(Get-Command python.exe).Path

pip install PyYAML Cheetah3

## setup ndk
$setup_script = (Resolve-Path $AX_ROOT/setup.ps1).Path

echo "before setup py_ver: $(python -V), PATH=$env:PATH"
. $setup_script -p android
echo "after setup py_ver: $(python -V), PATH=$env:PATH"

echo "$ndk_root=$ndk_root"

$prefix = Join-Path $AX_ROOT 'tmp'
$llvm_out = Join-Path $prefix "llvmorg-$llvm_ver.zip"

## download win64 libclang.dll 
$b1k.mkdirs($prefix)
download_and_expand -url "https://github.com/axmolengine/buildware/releases/download/llvmorg-$llvm_ver/llvmorg-$llvm_ver.zip" -out "$llvm_out" -dest $prefix
Copy-Item "$prefix/llvmorg-$llvm_ver/llvm/prebuilt/windows/x64/libclang.dll" -Destination "$AX_ROOT/tools/bindings-generator/libclang"

## ensure $env:AX_ROOT/core/axmolver.h exists
echo "AX_ROOT=$AX_ROOT"
if (!(Test-Path "$AX_ROOT/core/axmolver.h" -PathType Leaf)) {
    $content = [System.IO.File]::ReadAllText("$AX_ROOT/core/axmolver.h.in")
    $content = $content.Replace('@AX_GIT_PRESENT@', '0')
    [System.IO.File]::WriteAllText("$AX_ROOT/core/axmolver.h")
}

## run genbindings.py
Push-Location $AX_ROOT/tools/tolua

python genbindings.py --ndk_root "$ndk_root"
$succeed = $?

Pop-Location

Push-Location $AX_ROOT

if ($succeed -and $env:GITHUB_ACTIONS -eq 'true') {
    $git_status = "$(git status)"
    $no_changes = $git_status.IndexOf('modified:') -eq -1 # -and $git_status.IndexOf('deleted:') -eq -1 -and $git_status.IndexOf('Untracked', [StringComparison]::OrdinalIgnoreCase) -eq -1
    if ($no_changes) {
        echo "BINDING_NO_CHANGES=true" >> ${env:GITHUB_ENV}
    } else {
        echo "LAST_COMMIT_HASH=$(git rev-parse --short=7 HEAD)" >> ${env:GITHUB_ENV}
    }
}

Pop-Location

if(!$succeed) {
    throw "Generating lua bindings fails"
}
