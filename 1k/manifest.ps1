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
# [void]$manifest
