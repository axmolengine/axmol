# engine-v5
This is another more radical fork of cocos2d-x game engine, use opanal for all platforms, single texture multi gpu texture handler, c++17...  
  
The readme of cocos2d-x v4: https://github.com/simdsoft/engine-v5/blob/master/README.ccv4.md  
  
  
### Goals summary:
1. C++17
2. forcus on native game dev only
3. Remove unnecessary sources
4. build engine as static or dynamic library support
5. If you have any other excellent goals, welcome
6. Excellent PRs from any guys are welcome, I will review & merge ASAP

### preprocessors notes: 

```txt
CC_STATIC  
  
_USRDLL  
_USEGUIDLL  
_USREXDLL  
_EXPORT_DLL_  
  
_USRSTUDIODLL  
_USRSTUDIOPDLL  
```

### Refactor core & extensions libs
- [ ] engine-v5-core.dll(node tree, multi-renderer-backends, filesystem, audio, platform spec, and other):
  - [x] 1. Refactor audio engine, OpenAL for all platforms.
  - [x] 2. Remove SimpleAudio
  - [ ] 3. Remove offthread file operation
  - [x] 4. Remove getSutiableFOpen, avoid reduce performance, there is another better solution for support: ```just doesn't convert path to utf-8 in FileUtils manually implementation```
  - [x] 5. Optimize CCDownloader, curl for all platforms
  - [ ] 7. Refactor engine-v5-deps, all compile as vs2019 MSVC 19.22.27905.0
  - [x] 8. minizip-1.2.0
  - [x] 9. ccMessageBox
  - [x] 10. BMP support with stb_image
  - [x] 11. Remove unused libuv
  - [ ] 12. Reorder child support
  - [x] 13. Remove tinyxml2 dep, use pugixml instead
  - [x] 14. Use xsxml for SAX parser only
  - [x] 15. Multi meta textures v1.0
  - [x] 16. ProgramStateRegistry
  - [ ] 17. Optimize CCFileUtils
  - [ ] 18. ANGLE support.
  - [x] 19. Use curl for HttpClient all platforms
  - [ ]  20. AES encrypt system
  - [x] 21. Remove xxtea
  - [x] 22. Remove rapidxml dep
  - [x] 23. fix etc1 alpha test case, content size incorrect
  - [ ] 24. Since getSuitableFopen removed, don't convert to utf-8 for win32, or in the future, windows 10 will support utf-8 file path, such as: For Chinese system language users:
在 控制面板-区域-更改系统区域设置 中
勾选 Beta版：使用 Unicode UTF=8 提供全球语言支持(U) 选项
  - [ ] XX1. Cache ProgramState or use object_pool
  - [ ] XX2. Remove socket.io websockets, use yasio for socket connection solution
  - [ ] XX3. Fully streaming play for all media file: .mp4, .mp3, .ogg, .wav
  - [ ] XX4. Streaming load for .ttf with freetype2
  - [x] XX5. Remove CCB(already removed in v4)
  - [ ] XX6. Sync 3rd bin
  - [ ] XX7. Purely offthread load texture, use shared gl context
  - [ ] XX8. Use git submodule to manage 3rd libs.  
  
- [ ] Upgrade 3rd stubs to latest stable edition for all platforms
  - [ ] zlib to 1.2.11  
  
--------------------------------------------------------------------------    
- [ ] engine-v5.ui.dll: The original v3-ui framework may remove in the future
- [ ] engine-v5.ext.lib: old GUI ui(could be remove), physics-nodes, pu3d
- [ ] engine-v5.dragonbones.lib
- [ ] engine-v5.ccs20.dll: x-studio
- [ ] engine-v5.ccs21.dll: x-studio
- [ ] engine-v5.xs.dll: x-studio
- [ ] engine-v5.spine.lib  
----------------------------------------------------------------------------  
- [ ] engine-v5-lua.lib:
  - [x] Lua53 compat
  - [ ] make lua test works well
  - [ ] Remvoe luasocket, use yasio for socket connection solution
---------------------------------------------------------------------------  
!!! Custom APK support, don't compress .mp4, .mp3, .ogg, .wav, .ttf
---------------------------------------------------------------------------  
!!!! future of future:
   - [ ] Use sol2 for binding framework instead tolua++
   - [ ] Use bgfx for multi renderer backend support
