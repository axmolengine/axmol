# Default manifest, refer in 1k/1kiss.ps1
# For maintaining axmol, halx99 contributed some PRs to https://gitlab.kitware.com/cmake
# 3.27.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8319
# 3.28.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/8632
#         https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9008
# 3.28.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9014
# 3.30.0: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/9478
#

# load cross platform build.profiles
$build_profiles_file = Join-Path $PSScriptRoot 'build.profiles'
$build_profiles = ConvertFrom-Props (Get-Content $build_profiles_file)

$manifest['emsdk'] = $build_profiles['emsdk']
$manifest['jdk'] = $build_profiles['jdk']
$manifest['ninja'] = $build_profiles['ninja']
$manifest['ndk'] = $build_profiles['ndk']
$manifest['cmake'] = $build_profiles['cmake']
$manifest['vs'] = $build_profiles['vs']
$manifest['llvm'] = $build_profiles['llvm']

$Global:build_profiles = $build_profiles

$Global:download_path = $1k.realpath("$PSScriptRoot/../cache")
	
# add or overwrite tool version like follow
if ($Global:is_axmol_app -or $Global:is_axmol_engine) {
    $manifest['axslcc'] = $build_profiles['axslcc']
    $manifest['nuget'] = $build_profiles['nuget']
}
