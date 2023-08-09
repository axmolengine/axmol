# Default manifest in main script build1k.ps1
# $manifest = @{
#     msvc         = '143+';
#     ndk          = 'r23c+';
#     xcode        = '13.0.0~14.2.0'; # range
#     clang        = '15.0.0+';
#     gcc          = '9.0.0+';
#     cmake        = '3.26.4+';
#     ninja        = '1.11.1+';
#     jdk          = '11.0.19+';
#     cmdlinetools = '7.0+'; # android cmdlinetools
# }

# add or overwrite tool version like follow
$manifest['nuget'] = '*' # any
$manifest['glslcc'] = '1.9.0'

if ($IsMacOS) {
    $manifest['cmake'] = '3.27.20230808'
    $channels['cmake'] = 'g94eae40' # https://cmake.org/files/dev/cmake-3.27.20230808-g94eae40-windows-i386.msi
}
elseif ($options.p -eq 'winuwp') {
    $manifest['cmake'] = '3.27.1+'
}

# [void]$manifest
