#!/usr/bin/env pwsh
# This script is only for linux distro: debian or ubuntu
# llvm package list: https://github.com/opencollab/llvm-jenkins.debian.net/blob/master/llvm.sh
param(
  $action = '',
  $ver = 21,
  $tool = 'all'
)

$ver = [int]$ver

if (!$tool -or $tool -eq 'all') {
  $llvm_packages = @("clang-$ver", "lldb-$ver", "lld-$ver", "clangd-$ver")
  $llvm_binaries = @(
    'clang'
    'clang++'
    'lldb'
    'llvm-ar'
    'llvm-ranlib'
    'llvm-nm'
    'llvm-objdump'
    'llvm-config'
  )
  if ($tool -eq 'all') {
    $llvm_packages += @(
      "clang-tidy-$ver"
      "clang-format-$ver"
      "clang-tools-$ver"
      "llvm-$ver-dev"
      "lld-$ver"
      "lldb-$ver"
      "llvm-$ver-tools"
      "libomp-$ver-dev"
      "libc++-$ver-dev"
      "libc++abi-$ver-dev"
      "libclang-common-$ver-dev"
      "libclang-$ver-dev"
      "libclang-cpp$ver-dev"
      "libunwind-$ver-dev"
    )
    if ($ver -gt 14) {
        $llvm_packages += "libclang-rt-$ver-dev", "libpolly-$ver-dev"
    }
    $llvm_binaries += @(
      'clang-format'
      'clang-tidy'
      'clangd'
      'lldb-dap'
    )
  }
}
else {
  $llvm_packages = @("$tool-$ver")
  $llvm_binaries = @($tool)
}

if (!$tool -or $tool -eq 'all') {
  $llvm_tool = 'clang'
  $llvm_tool_real = "clang-$ver"
}
else {
  $llvm_tool = $tool
  $llvm_tool_real = "$tool-$ver"
}

function find_llvm_tool() {
  $verStr = $(& $llvm_tool --version 2>$null) | Select-Object -First 1
  $matchInfo = [Regex]::Match($verStr, '(\d+\.)+(\*|\d+)(\-[a-z0-9]+)?')
  $foundVer = $matchInfo.Value
  return $foundVer
}

function active_llvm($ver) {
  echo "Activating llvm-$ver ..."


  # list available llvm versions
  sudo update-alternatives --display $llvm_tool

  $actived_ver = [Version]$(find_llvm_tool)

  if ($actived_ver.Major -ne $ver) {
    # force set llvm to the specific version
    echo "Forcing switch actived llvm $($actived_ver.Major) => $ver ..."
    foreach ($exe_name in $llvm_binaries) {
      echo "Active alternative: $exe_name /usr/bin/$exe_name-$ver"
      sudo update-alternatives --set $exe_name /usr/bin/$exe_name-$ver
    }

    $actived_ver = [Version]$(find_llvm_tool)
  }

  # check result llvm version
  $llvm_tool_cmd = Get-Command $llvm_tool -ErrorAction SilentlyContinue
  echo "Activated llvm tool: $($llvm_tool_cmd.Source), version: $actived_ver"
}

# install
if ($action -eq 'install') {
  $llvm_tool_cmd = Get-Command $llvm_tool_real -ErrorAction SilentlyContinue
  if (!$llvm_tool_cmd) {
    echo "Installing llvm $llvm_tool_real..."
    curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
    $codename = (lsb_release -cs)
    $repoName = "deb http://apt.llvm.org/$codename/ llvm-toolchain-$codename-$ver main"
    sudo add-apt-repository -y $repoName
    sudo apt-get update
    sudo apt-get install --allow-unauthenticated --yes $llvm_packages
  }

  # config installed llvm to alternatives
  $priority = $ver * 10
  $success_count = 0
  foreach ($exe_name in $llvm_binaries) {
    echo "Install alternative: /usr/bin/$exe_name $exe_name /usr/bin/$exe_name-$ver $priority"
    $actual_path = "/usr/bin/$exe_name-$ver"
    if (Test-Path $actual_path -PathType Leaf) {
      sudo update-alternatives --install /usr/bin/$exe_name $exe_name $actual_path $priority
      ++$success_count
    }
    else {
      Write-Warning "llvm.ps1: warning: the executable: $actual_path not exist"
    }
  }
  if ($success_count -eq 0) {
    throw 'llvm.ps1: error: all packages install failed!'
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
  sudo apt-get remove -y $llvm_packages
  sudo apt-get autoremove -y

  echo "llvm packages: $llvm_packages has been uninstalled."
}
elseif ($action -eq 'list') {
  foreach ($exe_name in $llvm_binaries) {
    sudo update-alternatives --display $exe_name
  }
}
