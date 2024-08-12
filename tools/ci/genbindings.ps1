param(
    $stage = 3
)
$myRoot = $PSScriptRoot

$AX_ROOT = (Resolve-Path $myRoot/../..).Path

$succeed = $true
if (($stage -band 1)) {
    # ensure prebuilt lib downloaded
    Push-Location $AX_ROOT
    ./setup.ps1
    axmol -c
    Pop-Location

    $pip_cmd = @('pip3', 'pip')[$IsWin]
    $python_cmd = @('python3', 'python')[$IsWin]

    &$pip_cmd install PyYAML Cheetah3
    ## setup ndk
    $setup_script = (Resolve-Path $AX_ROOT/setup.ps1).Path

    . $setup_script -p android
    echo "after setup py_ver: $(&$python_cmd -V), PATH=$env:PATH"

    echo "$ndk_root=$ndk_root"

    ## download win64 libclang.dll
    $lib_name = @('libclang.dll', 'libclang.so', 'libclang.dylib')[$HOST_OS]
    $lib_path = Join-Path $AX_ROOT "tools/bindings-generator/libclang/$lib_name"
    if (!(Test-Path $lib_path -PathType Leaf)) {
        setup_7z
        $llvm_ver = '15.0.7'
        $llvm_pkg = "llvm-$llvm_ver.7z"

        $prefix = Join-Path $AX_ROOT "cache/devtools"
        $llvm_url = devtool_url $llvm_pkg
        $llvm_out = Join-Path $prefix $llvm_pkg

        $1k.mkdirs($prefix)
        download_and_expand -url $llvm_url -out $llvm_out -dest $prefix
        $suffix = @('win32/x64/libclang.dll', 'linux/libclang.so', 'mac/libclang.dylib')[$HOST_OS]
        Copy-Item "$prefix/llvm-$llvm_ver/lib/$suffix" -Destination $lib_path
    }

    ## ensure $env:AX_ROOT/core/axmolver.h exists
    echo "AX_ROOT=$AX_ROOT"
    if (!(Test-Path "$AX_ROOT/core/axmolver.h" -PathType Leaf)) {
        $content = [System.IO.File]::ReadAllText("$AX_ROOT/core/axmolver.h.in")
        $content = $content.Replace('@AX_GIT_PRESENT@', '0')
        [System.IO.File]::WriteAllText("$AX_ROOT/core/axmolver.h")
    }

    ## run genbindings.py
    Push-Location $AX_ROOT/tools/tolua

    &$python_cmd genbindings.py --ndk_root "$ndk_root"
    $succeed = $?

    Pop-Location
}

# strip empty lines
if ($stage -band 2) {
    echo 'Stripping header empty lines ...'
    $UTF8Encoding = [System.Text.Encoding]::UTF8
    $eol = $UTF8Encoding.GetBytes("`n")
    $autogen_dir = Join-Path $AX_ROOT 'extensions/scripting/lua-bindings/auto/*.hpp'
    $header_files = Get-ChildItem $autogen_dir
    
    foreach ($file in $header_files) {
        $file_path = "$file"
        $code_lines = Get-Content $file_path
        $fs = [System.IO.File]::Open($file_path, [System.IO.FileMode]::Truncate)
        $empty_lines = 0
        $write_lines = 0
        foreach ($line in $code_lines) {
            if ($line) {
                ++$write_lines
                $buf = $UTF8Encoding.GetBytes($line)
                $fs.Write($buf, 0, $buf.Length)
                $fs.Write($eol, 0, $eol.Length)
            }
            else {
                ++$empty_lines
            }
        }

        # $fs.Write($eol, 0, $eol.Length)
        $fs.Dispose()
        
        echo "Removed $empty_lines empty lines for file $file_path, valid lines: $write_lines"
    }
}

# check modified files
Push-Location $AX_ROOT
if ($succeed -and $env:GITHUB_ACTIONS -eq 'true') {
    $git_status = "$(git status)"
    $no_changes = $git_status.IndexOf('modified:') -eq -1 # -and $git_status.IndexOf('deleted:') -eq -1 -and $git_status.IndexOf('Untracked', [StringComparison]::OrdinalIgnoreCase) -eq -1
    if ($no_changes) {
        echo "BINDING_NO_CHANGES=true" >> ${env:GITHUB_ENV}
    }
    else {
        echo "LAST_COMMIT_HASH=$(git rev-parse --short=7 HEAD)" >> ${env:GITHUB_ENV}
    }
}
Pop-Location

if (!$succeed) {
    throw "Generating lua bindings fails"
}
