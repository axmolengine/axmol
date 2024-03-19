## How to sync sources

- recast source repo: https://github.com/recastnavigation/recastnavigation
- copy sources from `DebugUtils`,`Detour`, `DetourCrowd`, `DetourTileCache` to `3rdparty/recast` without subfolders
- patch: Tge API `dtCrowdAgentAnimation* dtCrowd::getEditableAgentAnim(const int idx)`