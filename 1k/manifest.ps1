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
$manifest['glslcc'] = '1.9.3+'
$manifest['jdk'] = '17.0.3+'

if ($IsMacOS) {
    # refer to: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8632
    # once cmake-3.28.0 released, can simply specify cmake to '3.28.0+'
    $manifest['cmake'] = '3.27.20230808'
    $channels['cmake'] = 'g94eae40'
}
elseif ($options.p -eq 'winuwp') {
    # refer to: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8319
    $manifest['cmake'] = '3.27.2+'
}

# [void]$manifest
