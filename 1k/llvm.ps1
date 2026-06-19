#!/usr/bin/env pwsh
# This script is only for linux distro: debian or ubuntu
param(
  $action = '',
  $ver = 21,
  $tool = 'all'
)

$ver = [int]$ver

if ($tool -eq 'all') {
  $llvm_binaries = @(
    'clang'
    'clang++'
    'clang-format'
    'clang-tidy'
    'clangd'
    'lldb'
    'lldb-dap'
    'llvm-ar'
    'llvm-ranlib'
    'llvm-nm'
    'llvm-objdump'
    'llvm-config'
  )
}
else {
  $llvm_binaries = @($tool)
}

function find_clang() {
  $verStr = $(. clang --version 2>$null) | Select-Object -First 1
  $matchInfo = [Regex]::Match($verStr, '(\d+\.)+(\*|\d+)(\-[a-z0-9]+)?')
  $foundVer = $matchInfo.Value
  return $foundVer
}

function active_llvm($ver) {
  echo "Activating llvm-$ver ..."


  # list available llvm versions
  sudo update-alternatives --display clang

  $actived_ver = [Version]$(find_clang)

  if ($actived_ver.Major -ne $ver) {
    # force set llvm to the specific version
    echo "Forcing switch actived llvm $($actived_ver.Major) => $ver ..."
    foreach ($exe_name in $llvm_binaries) {
      echo "Active alternative: $exe_name /usr/bin/$exe_name-$ver"
      sudo update-alternatives --set $exe_name /usr/bin/$exe_name-$ver
    }

    $actived_ver = [Version]$(find_clang)
  }

  # check result llvm version
  $clang_cmd = Get-Command "clang" -ErrorAction SilentlyContinue
  echo "Activated llvm-clang: $($clang_cmd.Source), version: $actived_ver"
}

# install
if ($action -eq 'install') {
  $clang_cmd = Get-Command "clang-$ver" -ErrorAction SilentlyContinue
  if (!$clang_cmd) {
    echo "Installing llvm-$ver ..."
    $llvm_script = "$PSScriptRoot/llvm.sh"
    curl -L https://apt.llvm.org/llvm.sh -o $llvm_script
    chmod +x $llvm_script
    sudo $llvm_script $ver $tool
  }

  # config installed llvm to alternatives
  $priority = $ver * 10
  foreach ($exe_name in $llvm_binaries) {
    echo "Install alternative: /usr/bin/$exe_name $exe_name /usr/bin/$exe_name-$ver $priority"
    $actual_path = "/usr/bin/$exe_name-$ver"
    if (Test-Path $actual_path -PathType Leaf) {
      sudo update-alternatives --install /usr/bin/$exe_name $exe_name $actual_path $priority
    }
    else {
      echo "llvm.ps1: warning: the executable: $actual_path not exist"
    }
  }
}
elseif ($action -eq 'uninstall') {
  # uninstall
  echo "Uninstalling llvm-$ver ..."

  # remove alternatives
  foreach ($exe_name in $llvm_binaries) {
    echo "Remove alternative: $exe_name /usr/bin/$exe_name-$ver"
    sudo update-alternatives --remove $exe_name /usr/bin/$exe_name-$ver
  }

  # uninstall llvm packages via apt
  echo "Removing llvm-$ver packages ..."
  sudo apt-get remove -y "llvm-$ver*" "clang-$ver*" "lldb-$ver*"
  sudo apt-get autoremove -y

  echo "llvm-$ver has been uninstalled."
}
elseif ($action -eq 'list') {
  foreach ($exe_name in $llvm_binaries) {
    sudo update-alternatives --display $exe_name
  }
}
