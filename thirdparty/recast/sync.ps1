$recast_root = $args[0]

$my_root = $PSScriptRoot

Copy-Item "$recast_root/DebugUtils/Include/*.h" "$my_root/"
Copy-Item "$recast_root/DebugUtils/Source/*.cpp" "$my_root/"

Copy-Item "$recast_root/Detour/Include/*.h" "$my_root/"
Copy-Item "$recast_root/Detour/Source/*.cpp" "$my_root/"

Copy-Item "$recast_root/DetourCrowd/Include/*.h" "$my_root/"
Copy-Item "$recast_root/DetourCrowd/Source/*.cpp" "$my_root/"

Copy-Item "$recast_root/DetourTileCache/Include/*.h" "$my_root"
Copy-Item "$recast_root/DetourTileCache/Source/*.cpp" "$my_root"
