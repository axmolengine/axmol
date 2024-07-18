# Default manifest, refer in 1k/1kiss.ps1
# For maintaining axmol, halx99 contributed some PRs to https://gitlab.kitware.com/cmake
# 3.27.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8319
# 3.28.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8632
#         https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9008
# 3.28.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9014
# 3.30.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9478
#

# add or overwrite tool version like follow
if ($Global:is_axmol_app -or $Global:is_axmol_engine) {
    $manifest['nuget'] = '5.5.1' # since 5.6.0, requires .net 4.0
    $manifest['glslcc'] = '1.9.5+'
    $manifest['cmake'] = '3.29.3+'
    $manifest['emsdk'] = '3.1.63+'
    $manifest['jdk'] = '17.0.10+'
}

# android sdk tools
$android_sdk_tools['build-tools'] = '34.0.0' # match with AGP-8.2.1+, android studio 2023.1.1+
$android_sdk_tools['platforms'] = 'android-34'
