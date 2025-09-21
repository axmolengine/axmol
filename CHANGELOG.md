# axmol-2.9.0 ?? 2025

## Significant changes relative to 2.8.x:

### 3D Audio & Basic Effects

- Add support for audio panning by @rh101 in [#2719](https://github.com/axmolengine/axmol/pull/2719)
- Add support for playing audio at any position in 3D space by @rh101 in [#2740](https://github.com/axmolengine/axmol/pull/2740)
- Initial implementation adding support for audio effects and filters using OpenAL by @rh101 in [#2772](https://github.com/axmolengine/axmol/pull/2772)
- Fix reverb properties values by @rh101 in [#2779](https://github.com/axmolengine/axmol/pull/2779)

### Other Changes

- Add screen orientation control for mobile devices by @halx99 in [#2784](https://github.com/axmolengine/axmol/pull/2784)
- Add new API `EventKeyboard::isRepeat` by @halx99 in [#2735](https://github.com/axmolengine/axmol/pull/2735)
- Add new API `ZipFile::createWithData` and mark `ZipFile::createWithBuffer` as deprecated by @halx99
- Enhance Base64 decoder to skip whitespace/newlines and prevent premature termination by @halx99

## Bug fixes

- Fix `ui::MediaPlayer` may crash on Apple platforms by @halx99 in [#2704](https://github.com/axmolengine/axmol/pull/2704)
- Fix occasional missing Android assets in AAB build by @paulocoutinhox in [#2713](https://github.com/axmolengine/axmol/pull/2713)
- Fix format specifiers in logging calls by @rh101 in [#2749](https://github.com/axmolengine/axmol/pull/2749)

## Improvements

- Destroy `ScriptEngine` instance before `_scheduler` to respect dependency by @halx99
- Fix SpineTest aim y-axis by @halx99
- Replace deprecated calls with new API in engine by @halx99
- Update `controller.cpp`: rename "Audio - NewAudioEngine" to "AudioEngine" by @aismann in [#2731](https://github.com/axmolengine/axmol/pull/2731)
- Fix addressed several non-critical issues for lua-tests by @halx99
- Remove non-existent yaml-cpp from template cmake modules by @halx99
- Remove CI pull-request trigger event: `ready_for_review` by @halx99
- Update kcp to resolve cmake error by @halx99

## SDK & Tools updates

- webview2: 1.0.3405.78 => 1.0.3485.44

## 3rdparty updates

- curl: 8.15.0 => 8.16.0
- freetype: 2.13.3 => 2.14.1
- jpeg-turbo: 3.1.1 => 3.1.2
- luajit: 2.1-f9140a6 => 2.1-871db2c

## Notes

### Add new API: `ZipFile::createWithData`

**Rationale**  
The existing API `ZipFile::createWithBuffer` has a design flaw. It accepts a `const char*` buffer without clarifying ownership, which makes the API dependent on the external buffer’s lifetime. This can lead to undefined behavior and hard-to-trace bugs.

**Improvement**  
The new API `ZipFile::createWithData` takes a `Data` object as its parameter. Since `Data` supports move semantics, ownership and lifetime are explicit. This ensures safer usage, reduces the risk of misuse, and provides more predictable behavior for developers.

---

### Add screen orientation control for mobile devices

**Rationale**  
Previously, screen orientation could only be configured statically through platform settings  
(e.g., `info.plist` on iOS or `AndroidManifest.xml` on Android). This limited flexibility,  
as the rendering orientation could not be changed at runtime once the app was launched.

**Improvement**  
A new core API `Device::setPreferredOrientation` has been introduced.  
- When orientation support is already declared in `info.plist` (iOS) or `AndroidManifest.xml` (Android),  
  this API allows dynamic modification of the rendering orientation at runtime.  
- Developers can now switch between portrait and landscape modes programmatically,  
  adapting to gameplay, UI flow, or user preferences.

**Benefit**  
- Consistent cross-platform orientation handling.  
- Eliminates the need for platform-specific code to adjust orientation dynamically.  
- Improves user experience by adapting rendering orientation seamlessly during runtime.

**Developer Note**  
The engine only performs **basic adaptation** internally.  
It is still recommended that developers handle their own resolution and layout adjustment strategies  
inside `AppDelegate::applicationScreenSizeChanged` to ensure proper scaling and UI behavior  
across different devices and orientations.

# axmol-2.8.1 Sep.5 2025

## Bug fixes

- **Fix a regression introduced by 2.8.0 that improper Content-Length setting in HTTP requests, see [#2686](https://github.com/axmolengine/axmol/issues/2686)** by @halx99
- Fix compile error when using AX_CORE_PROFILE option by @martinking71 in [#2677](https://github.com/axmolengine/axmol/pull/2677)
- Fix lua-tests PhysicsTest not work by @halx99
- Fix missing processor endif in platform/mac/GL-mac.h by @halx99
- Fix package name for lua-tests, live2d-tests by @halx99
- Fix AccelerometerTest of lua-tests not work by @halx99
- Fix wasm downloader by @AlexandreK38 in [#2676](https://github.com/axmolengine/axmol/pull/2676)

## Deprecated

- Mark Director::setGLDefaultValues as deprecated; use setRenderDefaults instead by @halx99
- Mark messageBox as deprecated; use showAlert instead by @halx99

## Improvements

- **Revert DrawNode color param to Color4F to fix Lua test issues** by @halx99
- Add API ax::showAlert by @halx99
- Add API HttpResponse::getStatusText by @halx99
- Remove entries for non-existent Lua test cases by @halx99
- Fix incorrect fmt::format format string by @halx99
- Remove "CC" from the menu entries (cpp-/lua-tests) by @aismann in [#2688](https://github.com/axmolengine/axmol/pull/2688)
- Remove unnecessary Lua files: LayerEx.lua, DrawPrimitives.lua by @halx99
- Use lua length operator `#` instead `table.getn` in CocoStudio.lua by @halx99
- Update url of CurlTeset by @halx99
- Ensure lua-tests RenderTextureTest case work as expected by @halx99
- Add API PointArray::clear() by @aismann in [#2692](https://github.com/axmolengine/axmol/pull/2692)

## Notes

- **Revert DrawNode color parameter type**

  **Reason**:  
  In axmol-2.2.0, the `DrawNode` color parameter was changed from `Color4F` to `Color4B`. This change prevented certain Lua binding test cases from detecting color updates correctly.

  **Impact**:  
  As a result, some Lua tests failed to produce the expected rendering behavior, leading to inconsistent test outcomes.

  **Solution**:  
  The color parameter has been reverted to `Color4F`. Since `DrawNode` internally already uses `Color4F` to pass color data to the GPU, this change restores expected Lua test behavior without any noticeable performance loss.


# axmol-2.8.0 Sep.1 2025

## Bug fixes

- Fix #2669: Windows UWP not running on Windows 11 due to DriverGL initialization failure by @halx99
- Fix #2655: Multi-touch not work on android scene contains imgui layer by @halx99
- Fix #2674: OpenSSL not linkage for final app on Linux by @halx99 in https://github.com/axmolengine/axmol/pull/2675
- Fix incorrect FontAtlas when switching label rendering mode from normal to SDF by @halx99 in https://github.com/axmolengine/axmol/pull/2629
- Fix wasm Device::getTextureDataForText will report exception when process whitespace text by @halx99
- Fix linux label color incorrect when LabelType is STRING_TEXTURE by @halx99
- Fix incorrect fmt::format_to use case by @halx99 in https://github.com/axmolengine/axmol/pull/2612
- Fix llvm ver trim in genbindings.ps1 by @halx99
- Fix Label SDF rendering issues with font resizing and other bugs by @halx99 in https://github.com/axmolengine/axmol/pull/2625
- Fix compile error when project contains swift code by @halx99 in https://github.com/axmolengine/axmol/pull/2632
- Fix wasm Downloader open file with wrong permision (#2666) by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2666
- Fix failure in CMake 4.1 when targeting Windows: assembler not found by @halx99

## Improvements

- Improve apple ios platform render view creation by @halx99 in https://github.com/axmolengine/axmol/pull/2600
- Rename GLView to RenderView by @halx99 in https://github.com/axmolengine/axmol/pull/2602
- Mention emsdk downgrading notice in version 2.6.0 release notes by @halx99
- Improve wasm dev setup notice by @halx99
- Rename `GLContextAttrs` to `GfxContextAttrs` by @halx99
- Update pull_request_template.md by @aismann in https://github.com/axmolengine/axmol/pull/2609
- Improve utf8 chars counting, remove strlen call by @halx99 in https://github.com/axmolengine/axmol/pull/2614
- Improve utf8 text processing by @halx99
- Remove sprintf/snprintf stubs by @halx99 in https://github.com/axmolengine/axmol/pull/2615
- Improve android sdk setup, axmol spec android sdk dir priority:  `android project local properties` > `ANDROID_HOME` > `ANDROID_SDK_ROOT` > `axmol/tools/external/adt/sdk` by @halx99
- Use pragma once for more header files by @halx99 in https://github.com/axmolengine/axmol/pull/2616
- Add support set window size limits by @NgVThangBz in https://github.com/axmolengine/axmol/pull/2618
- Renamed cpp-tests "Node: Label - New API" to "Node: Label" by @aismann in https://github.com/axmolengine/axmol/pull/2627
- Update ImGui mult-viewports hint message by @halx99
- Improve tests (Naming of menu entries on cpp-/lua- tests) by @aismann in https://github.com/axmolengine/axmol/pull/2640
- Move SIMD availability detection into its own CMake file by @j-jorge in https://github.com/axmolengine/axmol/pull/2642
- Setting CMAKE_MSVC_DEBUG_INFORMATION_FORMAT to Embedded to resolve pdb too large problem by @halx99
- Improve NuGet package management by @halx99 in https://github.com/axmolengine/axmol/pull/2671

## SDK & Tools updates

- agp: 8.11.0 => 8.11.1
- gradle: 8.14.3 => 9.0.0
- cmake: 4.0.3 => 4.1.1

## 3rdparty updates

- curl: 8.13.0 => 8.15.0

# axmol-2.7.1 Jul.15 2025

## Bug fixes

- Fix .axproj not load on non windows platforms by @halx99
- Fix missing extensions spine,fairygui preprocessors for axlua by @halx99
- Fix #2594 link error when build win32 exe with cmake generator Ninja by @halx99 in https://github.com/axmolengine/axmol/pull/2595

## Improvements

- Make msvc delay load mfmedia dlls also works with clang-cl or clang++ by @halx99 in https://github.com/axmolengine/axmol/pull/2595
- Improve plainlua CMakeLists.txt, make it compatibile with lua-5.1~5.5 by @halx99
- Remove unnecessary preprocessor: `AX_ENABLE_BULLET_INTEGRATION` by @halx99
- Ensure package name consistent for `axmol new -r` by @halx99
- Add parameter `-r` for `axmol new` command to regenerates the missing common and platform spec files by @halx99 in https://github.com/axmolengine/axmol/pull/2590
- Remove redundant `cc` prefix by @halx99 in https://github.com/axmolengine/axmol/pull/2587
- Improve android sdk root selection strategy by @halx99
- Improve android sdk setup by @halx99 in https://github.com/axmolengine/axmol/pull/2588, load `sdk.dir` from `proj.android/local.properties` as preferred android sdk root if it's present
- Improve lua bindings generator, make it can works with ndk versions: r23~latest by @halx99 in https://github.com/axmolengine/axmol/pull/2592
- Update imgui to 1.92.1 & tidy sources structure by @halx99 in https://github.com/axmolengine/axmol/pull/2593
- Re-indent all axmol spec cmake scripts with 2 spaces by @halx99 in https://github.com/axmolengine/axmol/pull/2599

## SDK & Tools updates

- cppwinrt: 2.0.240405.15 => 2.0.250303.1

## 3rdparty updates

- imgui: 1.92.0 => 1.92.1
- webp: 1.5.0 => 1.6.0

# axmol-2.7.0 Jul.6 2025

## Significant changes relative to 2.6.x

- The minimum required version of Android Studio has been updated to 2025.1.1
- Add mouse events listener for Widgets by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2580 (the callback function return type was changed from `void` to `bool`)
- Refactor imgui axmol backend by @halx99 in https://github.com/axmolengine/axmol/pull/2584, now both android and PC platform backends share it.
- The `ImGuiPresenter`'s APIs `mergeFontGlyphs`,`clearGlyphRanges`,`removeGlyphRanges`,`addGlyphRanges`,`getGlyphRangesId` have been removed(no longer needed), benefit from imgui-1.92.0 dynamic font atlas support, see imgui 1.92.0 release notes: https://github.com/ocornut/imgui/releases/tag/v1.92.0
- Add `ImGuiPresenter::getMainScale`, mark `ImGuiPresenter::getContentZoomFactor` as DEPRECATED
- Remove all overload stubs of API `ImGuiPresenter::addFont`, now only `void ImGuiPresenter::addFont(std::string_view fontFile, float fontSize = DEFAULT_FONT_SIZE);` available

## Bug fixes

- Fix Calling Label::setString after Label::getLetter causes layout issues by @halx99 in https://github.com/axmolengine/axmol/pull/2572
- Fix Label wrapping, overflow and alignment issues by @rh101 in https://github.com/axmolengine/axmol/pull/2567
- Fix https://github.com/axmolengine/axmol/issues/2569 build fail on ubuntu-22.04 by @halx99
- Fix statsLabel not show after Director::restart() by @remtori in https://github.com/axmolengine/axmol/pull/2581
- Fix linux crash on exiting cpp-tests after running Scene3DTest by @halx99 in https://github.com/axmolengine/axmol/pull/2582

## Improvements

- Improve system font text rendering under WASM by @halx99, also fix issue: https://github.com/axmolengine/axmol/issues/2349
- Improve prompt when cmd 'xcodebuild' missing by @halx99
- Fix Bug-350.cpp of cpp-tests by @aismann in https://github.com/axmolengine/axmol/pull/2568
- Update Clipper2 to 1.5.4 by @aismann in https://github.com/axmolengine/axmol/pull/2563
- Update 3rdparty README.md (Clipper2 1.5.4) by @aismann in https://github.com/axmolengine/axmol/pull/2564
- Update and re-enable Scene3D test by @rh101 in https://github.com/axmolengine/axmol/pull/2562
- Update SDFGen.cpp (fix typo) by @aismann in https://github.com/axmolengine/axmol/pull/2565
- Change wasm initial memory to 128MB, allow memory increase by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2575
- Add locating visual studio build tools without full vs installed by @martinking71 in https://github.com/axmolengine/axmol/pull/2576
- Add click listener for WASM (without touches AKA on desktop) to cancel the current 'touches' by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2577
- `WASM` Option to use main loop timing 'timeout' instead of 'requestAnimationFrame' (RAF) by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2578
- Remove Director redundant release statements by @halx99,@remtori
- Add close code and reason for Websocket close / onClose by @AlexandreK38 https://github.com/axmolengine/axmol/pull/2579
- Fix typo for 1kiss.ps1 by @halx99

## SDKs & Tools updates

- AGP: 8.10.0 => 8.11.0
- gradle: 8.13 => 8.14.3

## 3rdparty updates

- Clipper2: 1.5.3 => 1.5.4
- libpng: 1.6.48 => 1.6.50
- imgui: 1.91.6 => 1.92.0
- jpeg-turbo: 3.1.0 => 3.1.1
- ogg: 1.3.5 => 1.3.6
- openssl: 3.0.16 => 3.0.17
- plainlua: 5.4.7 => 5.4.8

# axmol-2.6.1 Jun.6 2025

## Bug fixes

- Fix https://github.com/axmolengine/axmol/issues/2559 crash on android armv7 by @halx99
- Fix blending so that the fairygui animation clip renders with the correct blend function by @thienphuoc in https://github.com/axmolengine/axmol/pull/2560
- Fix opus not link when linking prebuilt engine to axmol apps by @halx99
- Fix build win32 by clang fail due to openal set incorrect preprocessors by @halx99
- Fix gradle can't lookup cmake from `axmol/tools/external/cmake`

## Improvements

- No longer use android legacy cmake toolchain file by @halx99
- Add build options to game cmake module for easier access by @rh101 in https://github.com/axmolengine/axmol/pull/2553
- Move AXGameFinalSetup.cmake to cpp and lua templates because of differences in code by @rh101 in https://github.com/axmolengine/axmol/pull/2557
- Add win prebuilt workflow check ci by @halx99 in https://github.com/axmolengine/axmol/pull/2558
- Improve gradle scripts by @halx99 in https://github.com/axmolengine/axmol/pull/2561
- Holding a VM reference to the android asset manager jobject to prevent its being garbage collected while the native object is in use by @halx99
- Remove unused workaround code by @halx99
- Ensure setup correct version for android buildtools and target_sdk by @halx99
- Move `axmol.gradle` outside the `libaxmol` gradle project by @halx99

## SDKs & Tools updates

- cmdlinetools: 12.0 => 19.0

## 3rdparty updates

- astcenc: 5.2.0 => 5.3.0
- luajit: 2.1-eec7a80 => 2.1-f9140a6
- openssl: 3.5.0 => 3.0.16

## New Contributors

- @thienphuoc made their first contribution in https://github.com/axmolengine/axmol/pull/2560

# axmol-2.6.0 May.23 2025

## Significant changes relative to 2.5.0:

- Update android devenv to support android 16 by @halx99 in https://github.com/axmolengine/axmol/pull/2546
- Update Android Studio minimum required version to `2024.3.2`
- **Downgrade emsdk from `4.0.6` to `3.1.73` to ensure that the developer can debug axmol wasm apps properly on all platforms. If you're using windows, you won't be able to debug the app properly with emsdk version 4.0.0 or higher. If you use linux or macos, you can configure and use emsdk version 4.0.0 or higher in your `.axproj` file without any debugging issues, for example `emsdk=4.0.11`.**

## Bug fixes

- Fix #2548 Downloader-curl may crash or hang in download thread in https://github.com/axmolengine/axmol/pull/2549 by @halx99
- Fix #2545 install pwsh fail on ubuntu-25.04 by @halx99
- Fix wasm build fail on windows by @halx99 in https://github.com/axmolengine/axmol/pull/2511
- Fix PNG_ARM_NEON_OPT flag by @halx99 in https://github.com/axmolengine/axmol/pull/2512
- Fix #2504 play opus audio fail when build with Apple OpenAL.framework by @halx99 in https://github.com/axmolengine/axmol/pull/2506
- Fix typo in pkg_check_modules for GTK3. by @j-jorge in https://github.com/axmolengine/axmol/pull/2534
- Fix character, underline and strikethrough drawing over label boundary for Overflow::CLAMP mode by @rh101 in https://github.com/axmolengine/axmol/pull/2515
- Fix for clamped and left-aligned text with certain character sets by @rh101 in https://github.com/axmolengine/axmol/pull/2518
- Fix wasm raise runtime error due to HEAPU8 was not exported by @halx99
- Fix decompressGZ infinite-loop when input data invalid by @halx99 in https://github.com/axmolengine/axmol/pull/2544

## Improvements

- Improve ZipUtils::decompressGZ by @halx99 in https://github.com/axmolengine/axmol/pull/2544
  - Parsing uncompress size and reserve exactly avoid waste memory
  - Valid input size
- Improve ogg audio files, detect codec via file header by @halx99 in https://github.com/axmolengine/axmol/pull/2500
- Clarify required execution policy settings in PowerShell for IDE builds by @rh101 in https://github.com/axmolengine/axmol/pull/2505
- Enable verbose logging for android projects by @sarooptech in https://github.com/axmolengine/axmol/pull/2509
- Link prebuilt opus by @halx99 in https://github.com/axmolengine/axmol/pull/2514
- DevSetup, added mention of NDK r26+ for C++20 support under Android Studio by @danialias in https://github.com/axmolengine/axmol/pull/2524
- Update 3rd libs by @halx99 in https://github.com/axmolengine/axmol/pull/2527
- [FairyGUI] Add support for blending by @rh101 in https://github.com/axmolengine/axmol/pull/2532
- Committing luabindings for commit 8dab60e by @axmol-bot in https://github.com/axmolengine/axmol/pull/2533
- Undef the pre-processor definitions to stay unity build friendly by @j-jorge in https://github.com/axmolengine/axmol/pull/2535
- Rename ccBezierConfig as BezierConfig. by @j-jorge in https://github.com/axmolengine/axmol/pull/2536
- Committing luabindings for commit 876b5e4 by @axmol-bot in https://github.com/axmolengine/axmol/pull/2537
- Clipper2 1.5.3 by @aismann in https://github.com/axmolengine/axmol/pull/2541
- Set preferred emsdk version to 3.1.73 for chrome can debug axmol wasm app properly by @halx99
- Improve android sdk root lookup prompt, refer https://github.com/axmolengine/axmol/discussions/2494, https://github.com/axmolengine/axmol/discussions/2493 by @halx99
- Fix gradle lint warnings by @halx99
- Improve utils::computeDigest to support base64 digest present by @halx99
- Improve linux android studio support by @halx99
- Improve Improve linux env setup by @halx99
- Improve isolate project axslcc find prompt by @halx99
- Move simd intrinsics detection from 3rd into core by @halx99
- Add minimum version to build docs support by @halx99, now the ci will only build & deploy axmol-2.3+ docs
- Remove deprecated DSL config: renderscriptDebuggable by @halx99 in https://github.com/axmolengine/axmol/pull/2546
- Implement ui::MediaPlayer::getVirtualRenderer by @halx99

## SDKs & Tools updates

- AGP: 8.7.3 => 8.10.0
- android target sdk: 35 => 36
- android sdk build tools: 34.0.0 => 35.0.0
- emsdk: 4.0.6 => 3.1.73

## 3rdparty updates

- cares: 1.34.4 => 1.34.5
- Clipper2: 1.5.2 => 1.5.3
- doctest: 2.4.11 => 2.4.12
- fmtlib: 11.1.4 => 11.2.0
- llhttp: 9.2.1 => 9.3.0
- luajit: 2.1-a4f56a4 => 2.1-eec7a80
- libpng: 1.6.47 => 1.6.48
- openssl: 3.0.16 => 3.5.0

# axmol-2.5.0 Apr.5 2025

## Bug fixes

- Fix for invalid event listener ID lookup by @rh101 in https://github.com/axmolengine/axmol/pull/2445
- Fix #2415 c++23 compile error by @halx99
- Fix #2470 wasm CommandBufferGL::readPixels not work by @halx99

## Improvements

- Ensure fseeko available on ndk > 23 by @halx99 in https://github.com/axmolengine/axmol/pull/2490
- Add play .opus audio files support by @halx99 in https://github.com/axmolengine/axmol/pull/2488
- Auto install missing msvc toolset with specified version by @halx99
- make some warn message can suppressed by @halx99
- Fix winuwp ci due to github action vm disk space insufficient by @halx99
- [Android] FileUtils::listFiles implementation that works with empty folders by @rh101 in https://github.com/axmolengine/axmol/pull/2463
- [WASM] set _APP_RES_FOLDER if not defined to be able to specify directories of Resources by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2483
- Fix setup and BUILD_SHARED_LIBS for Linux by @IamSanjid in https://github.com/axmolengine/axmol/pull/2481
- Add supporting 'closed' Splines for DrawNode by @aismann in https://github.com/axmolengine/axmol/pull/2476
- Add GLView::setInteractive by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2465
- Fix BUILD_SHARED_LIBS for Android by @IamSanjid in https://github.com/axmolengine/axmol/pull/2462
- Fix some deprecated methods not available in gradle 9.0 by @halx99
- Fix #2457 support overwrite build profiles by .axproj by @halx99 in https://github.com/axmolengine/axmol/pull/2460
- Prevent getCurrentTime() from returning 0 at the end of the audio track playback by @rh101 in https://github.com/axmolengine/axmol/pull/2452
- Improve RichText XML parsing by @rh101 in https://github.com/axmolengine/axmol/pull/2420
- Renamed forgotten ccConfig.h to Config.h by @aismann in https://github.com/axmolengine/axmol/pull/2422
- Allow the use of em and % units for font size in RichText XML by @rh101 in https://github.com/axmolengine/axmol/pull/2421
- Minor FastTMXLayer performance optimization by @hasikill in https://github.com/axmolengine/axmol/pull/2437
- Add production ready wasm page by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/2428
- [Template] Add missing override specifier to destructor by @rh101 in https://github.com/axmolengine/axmol/pull/2433
- [Android] Ensure c++ object model consistency by @halx99 in https://github.com/axmolengine/axmol/pull/2434
- Update spine runtime to 4.2 by @halx99 in https://github.com/axmolengine/axmol/pull/2435
- Committing luabindings for commit fe9d98c by @axmol-bot in https://github.com/axmolengine/axmol/pull/2439
- Simplify project CMakeLists.txt by moving code sections to cmake modules by @rh101 in https://github.com/axmolengine/axmol/pull/2426
- Ensure custom button label is released correctly by @rh101 in https://github.com/axmolengine/axmol/pull/2446
- Refactor SpriteFrameCache to use uint64_t as map key by @halx99 in https://github.com/axmolengine/axmol/pull/2444
- Committing luabindings for commit 7f669db by @axmol-bot in https://github.com/axmolengine/axmol/pull/2449
- Log frame names instead of hash values in the call to removeUnusedSpriteFrames() by @rh101 in https://github.com/axmolengine/axmol/pull/2450
- Improve setup.ps1: traits undetermined shell as bash by @halx99
- Add ci `verify.yml` to verify axmol cmdline commands: `axmol new`, `axmol build` by @halx99
- Allow trigger build ci to build with specified c++ standard manually by @halx99
- [CI] Make run name dynamic with c++ standard mark
- [CI] Improve sites build

## SDKs & Tools updates

- gradle: 8.11.1 => 8.13
- cmake: 3.31.5 => 4.0.0
- emsdk: 4.0.3 => 4.0.6

## 3rdparty updates

- yasio: 4.3.1 => 4.3.2
- fmtlib: 11.1.3 => 11.1.4
- flatbuffers: 24.12.23 => 25.2.10
- robin_map: 1.2.1 => 1.4.0

# axmol-2.4.0 Feb.23 2025

## Bug fixes

- Fix build wasm fail with `-O0` by @halx99
- Fix #2370 module 'bitop' not found by @halx99
- Fix #2371 Video freezes on the first frame when played on Apple TV by @halx99
- Fix tile animation logic to use correct time per frame by @rh101 in https://github.com/axmolengine/axmol/pull/2345
- Fix: when these extensions set to OFF, compiles by @blairwang in https://github.com/axmolengine/axmol/pull/2347
- Fix tilemap culling by @DelinWorks in https://github.com/axmolengine/axmol/pull/2403

## Improvements

- Improve build system, support select msvc toolset version by @halx99
- Improve Label: underline, strikethrough and color by @aismann in https://github.com/axmolengine/axmol/pull/2382
- Disable Spine references if Spine is not enabled by @rh101 in https://github.com/axmolengine/axmol/pull/2346
- Increase MAX_AUDIOINSTANCES value to 128 in AudioEngine by @MartinLutherSu in https://github.com/axmolengine/axmol/pull/2348
- Clipper2 ver. 1.5.0 by @aismann in https://github.com/axmolengine/axmol/pull/2350
- Remove folder DrawNodeExTest by @aismann in https://github.com/axmolengine/axmol/pull/2352
- Clipper2 1.5.2 by @aismann in https://github.com/axmolengine/axmol/pull/2360
- [Android] Allow project to specify Axmol engine root path by @rh101 in https://github.com/axmolengine/axmol/pull/2361
- Improve gradle by @halx99 in https://github.com/axmolengine/axmol/pull/2363
- Add zip in memory support by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2368
- Use axmol proguard file in template and clean  by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2366
- [Windows] Ensure vertical scrolling is enabled when EditBox is in multiline mode by @rh101 in https://github.com/axmolengine/axmol/pull/2374
- Add Android event for Warm Start by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2367
- [Windows] Ensure EditBox shows text correctly when it receives focus for the first time by @rh101 in https://github.com/axmolengine/axmol/pull/2376
- Add Image flip Raw data vertically by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2369
- Committing luabindings for commit 0656f87 by @axmol-bot in https://github.com/axmolengine/axmol/pull/2377
- Improve site and add donate page with donate channels and sponsors list by @halx99
- Improve build system by @halx99
  - Install android cmdline-tools into android sdk root
  - Improve setup.ps1 on macOS
  - Update live2d SDK download url
  - Improve gradle scripts
- Remove unused 3rd lib: yaml-cpp by @halx99
- Fix wasm compile warnings
- Added methods to modify audio pitch by @Janglee123 in https://github.com/axmolengine/axmol/pull/2381
- Improve webgl render fps by @halx99 in https://github.com/axmolengine/axmol/pull/2379
- Add missing const qualifiers for DrawNode by @crazyhappygame in https://github.com/axmolengine/axmol/pull/2394
- Ensure opacity is applied to DrawNode used in Label by @rh101 in https://github.com/axmolengine/axmol/pull/2396
- Update button size if title renderer is changed and ignore size is true by @rh101 in https://github.com/axmolengine/axmol/pull/2401
- Calculate correct positions for underline and strikethrough by @rh101 in https://github.com/axmolengine/axmol/pull/2398
- [Android] Add a way to disable interactions on the SurfaceView by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/2410


## SDKs updates

- emsdk: 3.1.73 => 4.0.3

## 3rdparty updates

- astcenc: 5.1.0 => 5.2.0
- clipper2: 1.4.0 => 1.5.2
- curl: 8.11.1 => 8.12.1
- libpng: 1.6.45 => 1.6.47
- fmtlib: 11.1.2 => 11.1.3
- openssl: 3.0.15 => 3.0.16

# axmol-2.3.2 Jan.21 2025

## Bug fixes

- Fix bit op error on lua < 5.4 by @halx99
- Fix luajit luaL_openlibs crash on android by @halx99
- Fix setup.ps1 Error in parsing PowerShell and OS versions by @halx99
- Fix cpp template scene listeners leaked by @halx99

## Improvements

- Fix clang19 compile warnings
- Improve 7z cmdline compatibility
- DrawNode: new method: drawColoredTriangle(const Vec2* vertices3, const Color4B* color3) by @aismann in https://github.com/axmolengine/axmol/pull/2305
- Committing luabindings for commit 5c84ccd by @axmol-bot in https://github.com/axmolengine/axmol/pull/2307
- Correctly load resources that exist in a different folder to TMX file by @rh101 in https://github.com/axmolengine/axmol/pull/2316
- Update DevSetup.md for Android by @rh101 in https://github.com/axmolengine/axmol/pull/2319
- Add support for the creation of layers that are set to invisible in tilemap by @rh101 in https://github.com/axmolengine/axmol/pull/2321
- Committing luabindings for commit 2212048 by @axmol-bot in https://github.com/axmolengine/axmol/pull/2323
- Issue Template: Add Documentation_report.md  by @aismann in https://github.com/axmolengine/axmol/pull/2326
- Use correct color type to fix debug drawing in PhysicsWorld by @rh101 in https://github.com/axmolengine/axmol/pull/2334
- Committing luabindings for commit 3da481f by @axmol-bot in https://github.com/axmolengine/axmol/pull/2335
- Add more fine-grained callbacks supports for MenuItem by @MartinLutherSu in https://github.com/axmolengine/axmol/pull/2342

## 3rdparty updates

- libpng: 1.6.44 => 1.6.45
- fmtlib: 11.1.1 => 11.1.2
- luajit: 2.1-f73e649 => 2.1-a4f56a4

# axmol-2.3.1 Jan.5 2025

## Bug fixes

- Fix RenderTexture internal sprite positioning and test cases by @rh101 in https://github.com/axmolengine/axmol/pull/2301
- Fix imgui compile and link error for wasm by @halx99 in https://github.com/axmolengine/axmol/pull/2297
- Fix continues drawCardinalSplines() drawing outside the control points on last control point by @aismann in https://github.com/axmolengine/axmol/pull/2303

## Improvements

- Improve CI, raise error when build fail by @halx99
- Improve DevSetup.md by @rh101 in https://github.com/axmolengine/axmol/pull/2298

## 3rdparty updates

- xxhash: 0.8.2 => 0.8.3

# axmol-2.3.0 Jan.1 2025

## Significant changes relative to 2.2.x:

- Add android15 16KB page size support by @halx99 in https://github.com/axmolengine/axmol/pull/2268
- Improve build profiles by @halx99 in https://github.com/axmolengine/axmol/pull/2273
- Rename `org.axmol*` to `dev.axmol*` by @halx99 in https://github.com/axmolengine/axmol/pull/2272
- Rename `glslcc` to `axslcc` by @halx99 in https://github.com/axmolengine/axmol/pull/2271
- Add support to create a minmal `axmol-bs` package for common cross platform build purpose by @halx99
- Add vs2022 preview support in 1kiss.ps1 by @halx99
- Update libclang to 19.1.6 for luabinding generator by @halx99
- Download package to `cache` instead `tools/external`
- Add build android `.aab` support via `axmol -p android -aab`

## Bug fixes

- Fix Label create with long string cause random crash in windows by @halx99
- Fix DrawNode::drawPie(DrawMode::Line, thickness =1) draw a closed line (like DrawMode::Semi) wrong by @aismann in https://github.com/axmolengine/axmol/pull/2217
- Fix for Android build issue by @rh101 in https://github.com/axmolengine/axmol/pull/2225
- Fix media player by @ paulocoutinhox in https://github.com/axmolengine/axmol/pull/2257
- Fix android class name paths incorrect by @rh101 in https://github.com/axmolengine/axmol/pull/2276
- Fix renderer test case batching by @rh101 in https://github.com/axmolengine/axmol/pull/2291

## Improvements

- Improve DrawNode::drawPie() by @aismann in https://github.com/axmolengine/axmol/pull/2222
- Metal: enable mipmap support by @smilediver in https://github.com/axmolengine/axmol/pull/2235
- Add support for selecting all text in editbox via CTRL+A by @rh101 in https://github.com/axmolengine/axmol/pull/2238
- Ensure that git ignores build folders created in test projects by @rh101 in https://github.com/axmolengine/axmol/pull/2247
- Detect CTRL+A key combination to select all text in EditBox by @rh101 in https://github.com/axmolengine/axmol/pull/2251
- Prevent excessive calls to deleteBackward method which may cause crashes on Android by @rh101 in https://github.com/axmolengine/axmol/pull/2248
- Updated website. by @danialias in https://github.com/axmolengine/axmol/pull/2274
- Make win32 messageBox always TOPMOST by @aismann in https://github.com/axmolengine/axmol/pull/2221
- Add printLeaks call to alternative Win32 main method by @rh101 in https://github.com/axmolengine/axmol/pull/2278
- Add printLeaks call in appropriate location to help developers by @rh101 in https://github.com/axmolengine/axmol/pull/2275
- Remove TTF Font charCode 65535 limit by @halx99
- Allow publish from specified commitish for ci

## SDKs updates

- emsdk: 3.1.67 => 3.1.73
- AGP: 8.4.0 => 8.7.3
- gradle: 8.8.0 => 8.11.1
- android target sdk: 34 => 35

## 3rdparty updates

- astcenc: 4.8.0 => 5.1.0
- c-ares: 1.34.1 => 1.34.4
- curl: 8.10.1 => 8.11.1
- flatbuffers: 24.3.25 => 24.12.23
- fmtlib: 11.0.1 => 11.1.1
- oboe: 1.9.0 => 1.9.3
- webp: 1.4.0 => 1.5.0
- jpeg-turbo: 3.0.4 => 3.1.0
- luajit: 2.1-97813fb => 2.1-f73e649
- imgui: 1.90.6 => 1.91.6

# axmol-2.2.1 Oct.22 2024

## Bug fixes

- Fix curl link error on macOS

# axmol-2.2.0 Oct.20 2024

## Significant changes relative to 2.1.5:

- Enable vsync by default by @halx99 in https://github.com/axmolengine/axmol/pull/2118
- Improve android media player, also fix #2101 by @halx99 in https://github.com/axmolengine/axmol/pull/2109
- Add build on ubuntu 24.04 support by @halx99 in https://github.com/axmolengine/axmol/pull/2152
- No longer add sprite as child of RenderTexture by @halx99 in https://github.com/axmolengine/axmol/pull/2149
- API change too: remove all DrawNode setLineWidth() stuff by @aismann in https://github.com/axmolengine/axmol/pull/2172

## Bug fixes

- Fix invalidated iterator access in `Scheduler::unscheduleAllWithMinPriority()` by @smilediver in https://github.com/axmolengine/axmol/pull/2194
- Fix label in PauseTest. by @Tosik86 in https://github.com/axmolengine/axmol/pull/2157
- Fix #2147 full screen incorrect on macOS by @halx99 in https://github.com/axmolengine/axmol/pull/2150
- Fix RenderTargetGL FBO not recreate when recv EVENT_RENDERER_RECREATED by @halx99 in https://github.com/axmolengine/axmol/pull/2148
- Fix EventMouse coords by @halx99 in https://github.com/axmolengine/axmol/pull/2141
- Fix for incorrect bits per pixel set on texture reload by @rh101 in https://github.com/axmolengine/axmol/pull/2138
- Fix move constructor comment in Data class by @Tosik86 in https://github.com/axmolengine/axmol/pull/2132
- Fix Node `setPosition()` and `setPositionNormalized()` not switching normalized mode if positions match by @smilediver in https://github.com/axmolengine/axmol/pull/2102
- Fix issue with widget positioning when using normalized coordinates by @rh101 in https://github.com/axmolengine/axmol/pull/2110
- Fix RenderTexture data needs to be saved to the cache before the GLSurfaceView context is lost by @rh101 in https://github.com/axmolengine/axmol/pull/2146
- Fix incorrect static value usage in JNI array methods by @rh101 in https://github.com/axmolengine/axmol/pull/2178
- Fix #2199 by @halx99 in https://github.com/axmolengine/axmol/pull/2200
- Fix LuaJit Compilations by @IamSanjid in https://github.com/axmolengine/axmol/pull/2159
- Fix #2163 by @halx99 in https://github.com/axmolengine/axmol/pull/2166
- Fix #2169 by @halx99 in https://github.com/axmolengine/axmol/pull/2170
- fix Crash Sprite when _texture null by @NgVThangBz in https://github.com/axmolengine/axmol/pull/2164
- Fix dangling ref in `SpriteFrameCache::removeSpriteFramesFromTexture()` by @smilediver in https://github.com/axmolengine/axmol/pull/2106
- Fix android fps issue when certain animation intervals are set by @rh101 in https://github.com/axmolengine/axmol/pull/2162
- Fix DownloaderCURL too many open files and other fixes by @smilediver in https://github.com/axmolengine/axmol/pull/2182
 
## Improvements

- [ImGui] Allow setting of custom font config to enable merging of multiple fonts and other features by @rh101 in https://github.com/axmolengine/axmol/pull/2085
- Update DevSetup.md with iOS simulator build commands by @rh101 in https://github.com/axmolengine/axmol/pull/2087
- Remove couple bad warnings by @smilediver in https://github.com/axmolengine/axmol/pull/2088
- Trim trailing whitespace for files in `core` folder by @smilediver in https://github.com/axmolengine/axmol/pull/2089
- Remove the NS_AX_BEGIN and NS_AX_END macros. by @j-jorge in https://github.com/axmolengine/axmol/pull/2100
- Remove the USING_NS_AX and NS_AX macros. by @j-jorge in https://github.com/axmolengine/axmol/pull/2103
- Remove usage of the NS_AX macro. by @j-jorge in https://github.com/axmolengine/axmol/pull/2107
- Sprite: show stretch warning only if stretching is enabled by @smilediver in https://github.com/axmolengine/axmol/pull/2113
- Delay load mf.dll, mfplat.dll by @halx99 in https://github.com/axmolengine/axmol/pull/2116
- Remove unreferenced sprite from test scene by @rh101 in https://github.com/axmolengine/axmol/pull/2119
- Set RenderTexture content size on creation by @rh101 in https://github.com/axmolengine/axmol/pull/2120
- Update sweep.h  (fix invalid UTF-8) by @aismann in https://github.com/axmolengine/axmol/pull/2121
- Math optimizations by @smilediver in https://github.com/axmolengine/axmol/pull/2115
- DrawNode V2  by @aismann in https://github.com/axmolengine/axmol/pull/2124
- USe gradle-8.8-bin.zip everywhere by @crazyhappygame in https://github.com/axmolengine/axmol/pull/2134
- Refine contributing.md guide by @Tosik86 in https://github.com/axmolengine/axmol/pull/2135
- Allow turn on/off deprecated features by @halx99 in https://github.com/axmolengine/axmol/pull/2136
- Math: remove and replace GP_ASSERT with AX_ASSERT by @smilediver in https://github.com/axmolengine/axmol/pull/2143
- Enable cache render data for webgl by default by @halx99 in https://github.com/axmolengine/axmol/pull/2151
- Fix some compiling deprecated issues by @halx99 in https://github.com/axmolengine/axmol/pull/2154
- Update DrawNode.cpp (fix #2140) by @aismann in https://github.com/axmolengine/axmol/pull/2158
- Add (missing) parameter 'drawLineToCenter' to 'drawSolidCircle(...)' by @aismann in https://github.com/axmolengine/axmol/pull/2160
- Optimize DrawNode by @smilediver in https://github.com/axmolengine/axmol/pull/2165
- remove DrawPrimitivesTests (Cocos2Dx) (redundant mit DrawNodeTest) by @aismann in https://github.com/axmolengine/axmol/pull/2179
- move ZwoptexTest to SpriteTest (better add it to the SpriteTest section as removing) by @aismann in https://github.com/axmolengine/axmol/pull/2180
- Update README.md (thirdparty) by @aismann in https://github.com/axmolengine/axmol/pull/2183
- `AxmolActivity` refactoring and fixes by @smilediver in https://github.com/axmolengine/axmol/pull/2185
- Update controller.cpp Add small hint that the DrawNode tests be "not finished": "Node: Draw (WIP)" by @aismann in https://github.com/axmolengine/axmol/pull/2189
- DrawNode calls: replacing Color4F with Color4B (without the 'test' folder!) by @aismann in https://github.com/axmolengine/axmol/pull/2192

## SDKs updates

- emsdk: 3.1.63 => 3.1.67

## 3rdparty updates

- glad: 2.0.6 => 2.0.8
- curl: 8.9.1 => 8.10.1
- simdjson: 3.10.0 => 3.10.1
- libpng: 1.6.43 => 1.6.44
- c-ares: 1.33.0 => 1.34.1
- openssl: 3.3.1 => 3.0.15
- yasio: 4.2.4 => 4.3.1
- jpeg-turbo: 3.0.3 => 3.0.4
- luajit: 2.1-04dca79 => 2.1-97813fb
- simdjson: 3.10.0 => 3.10.1
- rapidjson: 1.1.0-b4a6da3 => 1.1.0-815e6e7
- freetype: 2.13.2 => 2.13.3

# axmol-2.1.5 Aug.11 2024

## Significant changes relative to 2.1.4:

- Add render I420 video support by @halx99 in https://github.com/axmolengine/axmol/pull/2050 , also fix issue #2049 reported by @SimplyJpk
- Refactor math simd by @halx99 in https://github.com/axmolengine/axmol/pull/2070, proposed by @huiguochen and @smilediver
  - Implement NEON transformVertices & transformIndices by @smilediver in https://github.com/axmolengine/axmol/pull/2065
  - Implement SSE transformVertices & transformIndices, proposed by @huiguochen
  - Add cmake option `AX_WASM_ISA_SIMD` to support build wasm with SIMD support, supports values `sse`, `neon`
  - Rewrite all neon inline asm code with NEON intrinsics, this will allow WASM and winuwp-arm64 can compile with NEON intrinsic
- Improve FileUtils by @halx99
  - Rename FileUtils::createDirectory to FileUtils::createDirectories
  - Use splitpath_cb to optimize FileUtils::createDirectories
  - Rename FileUtils::getFileShortName to FileUtils::getPathBaseName
  - Add FileUtils::getPathDirName
  - Add FileUtils::getPathBaseNameNoExtension
  - Mark all renamed FileUtils stubs old name deprecated
  - Mark all FileUtils offthread APIs deprecated
- Remove arch suffix of host default build output directory for axmol cmdline

## Bug Fixes

- Fix checkReallySupportsASTC does not work on ios device reported by @BIGCATDOG in https://github.com/axmolengine/axmol/issues/2078
- Fix ImGui index draw info incorrect by @rh101 in https://github.com/axmolengine/axmol/pull/2052
- Fix ArchLinux setup dependencies always fail by @halx99
- Fix android VolatileTexture memory leak when addImage(img,k,fmt)
- Fix cmake prebuilt link error by @IamSanjid in https://github.com/axmolengine/axmol/pull/2058
- Fix prebuilt openssl dll path incorrect for winuwp by @halx99
- Fix #2060, make http request/respose reference counter thread-safe by @halx99 in https://github.com/axmolengine/axmol/pull/2064, reported by @qq461787436
- Fix Curl downloader not observing set task limits by @smilediver in https://github.com/axmolengine/axmol/pull/2067
- Fix a regression introduced by 2.1.4 that live2d compile error by @halx99, reported by @Sevael
- Fix unexpected libpng from system used
- Fix #1751, use coroutine control AutoTest flow

## Improvements

- Enhance Color4F and method naming change by @rh101 in https://github.com/axmolengine/axmol/pull/2072
- Improve FastRNG implementation by @DelinWorks in https://github.com/axmolengine/axmol/pull/2056
  - Move FastRNG to ax ns and more improvements by @DelinWorks in https://github.com/axmolengine/axmol/pull/2057
  - Add missing include for AXASSERT by @smilediver in https://github.com/axmolengine/axmol/pull/2081
- Handle premultiplied alpha for grayscale PNGs by @j-jorge in https://github.com/axmolengine/axmol/pull/2047
- Optimize ProgramState::setTexture() to avoid vector (de)allocations by @smilediver in https://github.com/axmolengine/axmol/pull/2061
- Add OpenType font (.otf) to the noCompress list by @danialias in https://github.com/axmolengine/axmol/pull/2077
- Update DrawNodeEx aka DrawNodeV2 to 0.95.1 by @aismann in https://github.com/axmolengine/axmol/pull/2079

## SDKs updates

- cppwinrt: 2.0.240111.5 => 2.0.240405.15

## 3rdparty updates

- box2d: 2.4.1 => 2.4.2
- curl: 8.8.0 => 8.9.1
- simdjson: 3.9.5 => 3.10.0
- lz4: 1.9.4 => 1.10.0
- c-ares: 1.32.2 => 1.33.0

# axmol-2.1.4 Jun.20 2024

## Significant changes relative to 2.1.3:

- Add JobSystem support by @halx99
- Add cmake option AX_WASM_INITIAL_MEMORY
- Add Linux WebView implementation by @IamSanjid in https://github.com/axmolengine/axmol/pull/1985
- Improve curl download implementation by @halx99 in https://github.com/axmolengine/axmol/pull/1999
  - Use `curl_multi_poll` instead select
  - Use `curl_xferinfo_callback` instead deprecated `curl_progress_callback`
  - Don't use CURL_HEADER to get content length, get it in xferinfo_callback
- Improve RenderTarget by @smilediver in https://github.com/axmolengine/axmol/pull/1996
- Make apple controller to support four players by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/2009

## Deprecated

- `AsyncTaskPool`, use `JobSystem` instead.
- `AXLOG`, `AXLOGINFO`, `AXLOGWARN`, `AXLOGERROR`, use `AXLOGD`, `AXLOGI`, `AXLOGE` instead.
  - Mark AXLOG, AXLOGINFO, AXLOGWARN, AXLOGERROR deprecated in https://github.com/axmolengine/axmol/pull/2017
  - Rename all AXLOG/printf to AXLOGD (without .\core, .\3rdparty) by @aismann in https://github.com/axmolengine/axmol/pull/2020
  - Set several log messages to verbose level by @rh101 in https://github.com/axmolengine/axmol/pull/2021
  - Change AXLOGI to more appropriate AXLOGD for several log calls by @rh101 in https://github.com/axmolengine/axmol/pull/2028
- `StringUtils::format`, use `fmt::format` instead.

## Bug Fixes

- Fix call to incorrect java method by @rh101 in https://github.com/axmolengine/axmol/pull/2015
- Fix Callback and store Renderer Image type by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/1945
- Fix char8_t not disable on msvc by @halx99
- Fix Renderer recreated by @smilediver in https://github.com/axmolengine/axmol/pull/1988
- Fix AvfMediaEngine assetion when play some HLS video files by @halx99 in https://github.com/axmolengine/axmol/issues/1984
- Fix compiling errors when on linux ubuntu2204 by @bwaiox in https://github.com/axmolengine/axmol/pull/2012
- Fix prebuilt, add ax_link_ext macro by @iAndyHD3 in https://github.com/axmolengine/axmol/pull/2022
- Fix #1943, Make extension luabinding can be controlled by AX_EXT_HINT by @halx99, reported by @iAndyHD3

## Improvements

- Add ability to change Inspector's font size by @IamSanjid in https://github.com/axmolengine/axmol/pull/1932
- Calculate safe area inset values for android devices with rounded corners by @rh101 in https://github.com/axmolengine/axmol/pull/1934
- Update documentation for axmol build commands when targetting the iOS simulator by @rh101 in https://github.com/axmolengine/axmol/pull/1938
- Added glyph ranges manager for ImGuiPresenter, added ability to use glyph ranges for Inspector's font by @IamSanjid in https://github.com/axmolengine/axmol/pull/1936
- Add support for necessary events to be sent regardless of event dispatcher enabled state by @rh101 in https://github.com/axmolengine/axmol/pull/1940
- Enable DrawNodeEx by default by @aismann in https://github.com/axmolengine/axmol/pull/1950
- Apply child stencils correctly to the clipping node by @rh101 in https://github.com/axmolengine/axmol/pull/1957
- Fix some compiler warnings on windows by @IamSanjid in https://github.com/axmolengine/axmol/pull/1954
- Update DevSetup.md by @caryyu in https://github.com/axmolengine/axmol/pull/1965
- Sprite and TextureCache from ax::Data by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/1967
- Remove duplicate cmake options by @halx99
- Count run action finish to call final finish when two run actions are performed by @AlexandreK38 in https://github.com/axmolengine/axmol/pull/1946
- Only pause GLSurfaceView if activity is moved to the background, and not if it just loses focus by @rh101 in https://github.com/axmolengine/axmol/pull/1942
- Update spine README.md by @rh101 in https://github.com/axmolengine/axmol/pull/1975
- Update llvm setup
  - Remove llvm setup from gh action yml config, since 1kiss will setup it
  - Update 1kiss.ps1 llvm min requirement to 17.0.6 for msvc14.40 support
  - Remove llvm version config from 1k/manifest.ps1 since 1kiss.ps1 already set properly for msvc14.40 support
- Update TLD to `axmol.dev` in sources by @halx99
- Don't output log when custom ILogOutput was set by @halx99
- Use new logging system for audio by @halx99
- Update DrawNodeV2 0.94 by @aismann in https://github.com/axmolengine/axmol/pull/2008
- Ensure the RenderTexture reference is valid on threaded saveToFile task completion by @rh101 in https://github.com/axmolengine/axmol/pull/2024
- Some warning and other minor fixes by @smilediver in https://github.com/axmolengine/axmol/pull/2038


## sdks & tools updates

- gradle: 8.7 => 8.8
- AGP: 8.2.2 => 8.4.0
- emsdk: 3.1.59 => 3.1.63

## 3rdparty updates

- openssl: 3.0.13 => 3.3.1
- Clipper2: 1.3.0 => 1.4.0
- simdjson: 3.9.2 => 3.9.5
- fmtlib: 10.2.1 => 11.0.1
- yasio: 4.2.2 => 4.2.3
- oboe: 1.8.1 => 1.9.0
- plainlua: 5.4.6 => 5.4.7
- c-ares: 1.29.0 => 1.32.2
- luajit: 2.1-d06beb0 => 2.1-04dca79

# axmol-2.1.3 May.26 2024

## Significant changes relative to 2.1.2:

- Implement cross-platform media controller for video playback by @rh101 in https://github.com/axmolengine/axmol/pull/1845
  - Adjust media control positioning if video aspect ratio is maintained by @rh101 in https://github.com/axmolengine/axmol/pull/1851
  - Allow media controls to be rotated left or right by 90 degrees by @rh101 in https://github.com/axmolengine/axmol/pull/1910
- Allow certain code modules to be removed from build process by @rh101 in https://github.com/axmolengine/axmol/pull/1769, i.e. remove 3d features by `-DAX_ENABLE_3D=OFF`
- New logging system with general log level and colored support
  - Implement new axmol log system based on fmtlib by @halx99 in https://github.com/axmolengine/axmol/pull/1732
  - Logging colorful support by @halx99 in https://github.com/axmolengine/axmol/pull/1735
  - Improve logging color style by @halx99 in https://github.com/axmolengine/axmol/pull/1739
  - Add support for verbose trace level logging by @rh101 in https://github.com/axmolengine/axmol/pull/1764
  - Removed the '\n' which creates a second (empty) line. by @aismann in https://github.com/axmolengine/axmol/pull/1746
- Add wasm EditBox support
- Simplify axmol cmdlines build command, now you can use command `axmol` instead `axmol build` to build your project
- Linking prebuilt xcframework for apple platforms, please do clean `AX_ROOT` by `axmol -c -u` if you tracking axmol engine git repo
- Add unit tests by @smilediver in https://github.com/axmolengine/axmol/pull/1862
  - https://github.com/axmolengine/axmol/pull/1870
  - https://github.com/axmolengine/axmol/pull/1871
- Add AXLOGD,AXLOGI,... functions for lua
- Add axmol cmdline build option to specify build optimize flag: `-O0`(Debug), `-O1`(MinSizeRel), `-O2`(RelWithDebInfo), `-O3`(Release)

## Breaking Changes

- Rename `ax::Ref` => `ax::Object`
- Remove `ax::log`, use `AXLOGD` instead
- Remove cmake function: `ax_uwp_set_all_targets_deploy_min_version`
- Remove deprecated pixel formats L8, A8, LA8 in https://github.com/axmolengine/axmol/pull/1839
- Move `axmol/build.ps1` to `axmol/tools/cmdline/build.ps1`
- Rename ax::ccMessageBox to ax::messageBox
- Move ax::ccNextPOT to ax::utils::nextPOT
- Remove unused API: ax::LuaLog
- Change parameter 'sharedRT' of RenderTexture::create to false

## Mark as deprecated

- `2d/TextFieldTTF`
- `ui/UITextField`, use `ui/TextFieldEx` instead

## BugFixes

- Fix fullscreen incorrect in retina display by @halx99, reported by @TyelorD
- Fix same uniforms in fragment and vertex shaders not working in Metal by @smilediver in https://github.com/axmolengine/axmol/pull/1808
- Fix android build by @smilediver in https://github.com/axmolengine/axmol/pull/1723
- Fix EditBox error by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1755
- Fix InputEvent in fairyGUI onKeyDown and onKeyUp by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1757
- Fix wasm FPS control not working by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1762
- Fix texture recreation when GL context is lost by @smilediver in https://github.com/axmolengine/axmol/pull/1763
- Fix DebugDraw issue in 3D physics by @halx99
- Fix OpenGLState cache error, reported by @solan-solan in https://github.com/axmolengine/axmol/issues/1759
- Fix linux crash after close window by @halx99
- Fix setup.ps1 not update AX_ROOT in unix profile by @halx99
- Fix a regression introduced by 2.1.2 that template gradlew permission incorrect
- Fix build android with axmol cmdline arch not working
- Fix crash on iOS by @smilediver in https://github.com/axmolengine/axmol/pull/1799
- Fix typo: BLEND_CLOLOR => BLEND_COLOR
- Fix typo: RESERVE_SUBTRACT => REVERSE_SUBTRACT
- Fix video player crash on apple platforms, reported by @asnagni in https://github.com/axmolengine/axmol/issues/1815
- Fix an issue where ProgressTimer didn't properly set or cascade its opacity by @TyelorD in https://github.com/axmolengine/axmol/pull/1830
- Fix ClippingNode stencil needing global z set manually by @TyelorD in https://github.com/axmolengine/axmol/pull/1831
- Fix FileUtilsApple::getPathForDirectory() checking wrong path by smilediver in https://github.com/axmolengine/axmol/pull/1828
- Fix issue where UIWidget couldn't be reselected by @TyelorD in https://github.com/axmolengine/axmol/pull/1827
- Fix glfw crash on macOS for large mouse button ids by @smilediver in https://github.com/axmolengine/axmol/pull/1835
- Fix 1k/fetch.cmake not working on powershell 5.1
- Fix spine two color tint data not being copied across to backend buffer on first frame by @rh101 in https://github.com/axmolengine/axmol/pull/1875
- Fix FileUtils issues on macOS by @smilediver in https://github.com/axmolengine/axmol/pull/1863
- Fix memory corruption bug and resulting crash in the TextFieldTTF by @TyelorD in https://github.com/axmolengine/axmol/pull/1890
- Fix shaders not copying to final build on macOS for non Xcode builds by @smilediver in https://github.com/axmolengine/axmol/pull/1908
- Fix system font blurring by in @DelinWorks in https://github.com/axmolengine/axmol/pull/1907
- Fix android ImGui crash due to invalid pointer by @rh101 in https://github.com/axmolengine/axmol/pull/1920

## Improvements

- Update ios/tvos LaunchScreen.storyboard to 11.0+, also fix xcode15.4 build error
- Update prebuilt tool `glslcc` to 1.9.5, macos-arm64 support and built for macos-10.15+
- Improve MediaPlayer: handle video rotation properly
- Disable c++20 char8_t
- Improve build system, set rpath properly for platforms: linux, apple
- HttpClient: add support for PATCH method by @smilediver in https://github.com/axmolengine/axmol/pull/1722
- Add -f switch info to axmol command help output by @rh101 in https://github.com/axmolengine/axmol/pull/1729
- Adapt DrawNodeEx to the changes on: Move color types to math/Color.h axmolengine#1741 by @aismann in https://github.com/axmolengine/axmol/pull/1760
- Add Node World Space Positioning by @DelinWorks in https://github.com/axmolengine/axmol/pull/1743
- Move color types to math/Color.h by @halx99 in https://github.com/axmolengine/axmol/pull/1741
- Removing EditBox should not result in logging an error message by @rh101 in https://github.com/axmolengine/axmol/pull/1740
- Update LUA config for new build options by @rh101 in https://github.com/axmolengine/axmol/pull/1772
- Update TLD axmol.org -> axmol.dev
- Rename folder thirdparty => 3rdparty
- Update spine to 4.1-54fac9d
- Update axmol cmdline preferred cmake to 3.29.3+ for supress xcode 15 duplicated linking warnings
- Remove unnecessary cmake option: `AX_VS_DEPLOYMENT_TARGET`
- Make FileStream open file for write share flags same with cstd
- Fix some compile warnings
- Add ttf fallback render support
- Add cmake option: `AX_EXT_HINT` to set default extensions state
- Add axmol cmdline option `-dm` to dump compiler preprocessors before build
- Display cmake command used by build script in console by @rh101 in https://github.com/axmolengine/axmol/pull/1834
- Add Borderless Windowed Mode by @TyelorD in https://github.com/axmolengine/axmol/pull/1826
- Add CC_USE_CULLING Alias to cocos2d.h by @TyelorD in https://github.com/axmolengine/axmol/pull/1825
- Prevent repeated loading of plist files when loading csb files by @tkzcfc in https://github.com/axmolengine/axmol/pull/1844
- Remove Info.plist from mac Resources by @martinking71 https://github.com/axmolengine/axmol/pull/1849
- Add libvlc prebuilt entry CMakeLists.txt
- Add ability to create console apps by @smilediver in https://github.com/axmolengine/axmol/pull/1859
- Add support for ensuring sprite content size does not change on new texture by @rh101 in https://github.com/axmolengine/axmol/pull/1897
- Remove obsolete `box2d-optimized` support by @aismann in https://github.com/axmolengine/axmol/pull/1913
- Add macOS, Linux support for `tools/ci/genbindings.ps1`
- Fix `axmol` cmdline not raise error when cmake build fail
- Migrate wasm ci from appveyor to github actions
- Set AX_USE_METAL and AX_USE_GL to 1 if defined by @smilediver in https://github.com/axmolengine/axmol/pull/1916
- Update the CMake minimum version requirement to 3.23.0+ by @rh101 in https://github.com/axmolengine/axmol/pull/1918
- Add x86_64 to example list of possible target architectures for android by @rh101 in https://github.com/axmolengine/axmol/pull/1919
- Enable lua `bit` module for all lua versions supported by axmol
- Fix passing non null terminated strings for null terminated params by @smilediver in https://github.com/axmolengine/axmol/pull/1924
- Fix sol problem in emplace method by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/1927
- Fix descriptor typo by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/1928
- Do image processing to reverse premultiplied alpha in separate thread by @rh101 in https://github.com/axmolengine/axmol/pull/1929


## sdks & tools updates

- emsdk: 3.1.53 => 3.1.60
- AGP: 8.2.1 => 8.2.2
- androidx.media3: 1.0.2 => 1.2.1
- glslcc: 1.9.4 => 1.9.5

## 3rdparty updates

- simdjson: 3.7.0 => 3.9.2
- flatbuffers: 2.0.8 => 24.3.25
- curl: 8.6.0 => 8.8.0
- glad: 2.0.5 => 2.0.6
- yasio: 4.2.1 => 4.2.2
- llhttp: 9.2.0 => 9.2.1
- libwebp: 1.3.2 => 1.4.0
- astcenc: 4.7.0 => 4.8.0
- stb_image: 2.28 => 2.29
- luajit: 2.1-9cc2e42 => 2.1-d06beb0
- c-ares: 1.25.0 => 1.29.0
- imgui: 1.90.4 => 1.90.6

# axmol-2.1.2 Feb.25 2024

## BugFixes

- Fix screen size incorrect in retina display by @halx99 in https://github.com/axmolengine/axmol/pull/1692, reported by @IcemarkUK
- Fix view size incorrect when frameZoomFactor not 1.0 by @halx99 in https://github.com/axmolengine/axmol/pull/1694
- Fix lua build for Android, iOS and Windows by @rh101 in https://github.com/axmolengine/axmol/pull/1650
- Fix missing return statement by @rh101 in https://github.com/axmolengine/axmol/pull/1657
- Fix cross fade transition by @rh101 in https://github.com/axmolengine/axmol/pull/1664
- Fix Update CONTRIBUTING.md by @grif-on in https://github.com/axmolengine/axmol/pull/1673
- Fix crash during scene transition due to invalid camera by @rh101 in https://github.com/axmolengine/axmol/pull/1676
- Fix trim leading and trailing whitespace characters for RichText XML text by @rh101 in https://github.com/axmolengine/axmol/pull/1687
- Fix can't start lua project on iOS by @halx99
- Fix lua build for Android, iOS by @bintester in https://github.com/axmolengine/axmol/pull/1709
- Fix #1704, control android cmake options by engine axutils.gradle by @halx99 in https://github.com/axmolengine/axmol/pull/1705
- Fix #1703, unzip don't rely on zlib DLL export by @halx99
- Fix OSX sandbox network issue by @halx99, reported by @IcemarkUK
- Fix c-ares link issue
- Fix DrawNode luabinding color type mismatch by @halx99, reported by @qq461787436

## Improvements

- Change extension folder structure to reduce file name conflicts by @rh101 in https://github.com/axmolengine/axmol/pull/1647
- Committing luabindings for commit 9f8962d by @axmol-bot in https://github.com/axmolengine/axmol/pull/1648
- Use different global names and #undef the #defines to stay unity build friendly. by @j-jorge in https://github.com/axmolengine/axmol/pull/1653
- Add AX_USE_ALSOFT_STATIC CMake option to compile and link OpenAL Soft as a static library. by @j-jorge in https://github.com/axmolengine/axmol/pull/1654
- Ability to disable default implementations of HTTP and WebSocket clients by @rh101 in https://github.com/axmolengine/axmol/pull/1656
- Remove duplicate CODE_SIGN_IDENTITY that is setting invalid value by @rh101 in https://github.com/axmolengine/axmol/pull/1666
- Disable unused extensions in the FairyGUI and Live2D tests to speed up builds by @rh101 in https://github.com/axmolengine/axmol/pull/1665
- Update README.md by @danialias in https://github.com/axmolengine/axmol/pull/1668
- Spelling and formatting corrections for .md files by @danialias in https://github.com/axmolengine/axmol/pull/1671
- Update CONTRIBUTING.md by @grif-on in https://github.com/axmolengine/axmol/pull/1670
- Solved broken links in README.md by @danialias in https://github.com/axmolengine/axmol/pull/1677
- Ensure correct video mode settings are applied when creating view by @rh101 in https://github.com/axmolengine/axmol/pull/1678
- Solved more broken links in README.md by @grif-on in https://github.com/axmolengine/axmol/pull/1680
- Some additions and corrections for readme files. by @danialias in https://github.com/axmolengine/axmol/pull/1682
- RichText enhancements by @rh101 in https://github.com/axmolengine/axmol/pull/1683
- Add -j to specify build cpu count  support by @halx99 in https://github.com/axmolengine/axmol/pull/1688
- Ensure valid whitespace is not trimmed when html parsing mode is used by @rh101 in https://github.com/axmolengine/axmol/pull/1689
- Add support for window close button calback by @lich426 in https://github.com/axmolengine/axmol/pull/1697
- RichText and ScrollView enhancements by @rh101 in https://github.com/axmolengine/axmol/pull/1696
- Improve project templates by @halx99 in https://github.com/axmolengine/axmol/pull/1713
- [Android] Allow setting certain defines without modifying engine source by @rh101 in https://github.com/axmolengine/axmol/pull/1710
- Improve dll support by @halx99 in https://github.com/axmolengine/axmol/pull/1706
- Don't use fmt header only by @halx99 in https://github.com/axmolengine/axmol/pull/1707
- Don't use yasio header only by @halx99

## SDKs updates

- cppwinrt: 2.0.230706.1 => 2.0.240111.5

## 3rdparty updates

- glad: 2.0.4 => 2.0.5
- yasio: 4.2.0 => 4.2.1
- llhttp: 9.1.3 == > 9.2.0
- oboe: 1.8.0 => 1.8.1
- libpng: 1.6.42 => 1.6.43
- glfw: git b4c3ef9 => 3.4
- simdjson: 3.6.4 => 3.7.0
- imgui: 1.90.1 => 1.90.4

# axmol-2.1.1 Feb.2 2024

## BugFixes

- Fix shaders not copying to final build on macOS for non Xcode builds by @smilediver
- Fix `Scheduler::pauseTarget` can't timer targets, reported by @shaniamjad6
- Fix build lua projects with engine prebuilts lib doesn't work, reported by @grif-on
- Fix GUI ScrollView clipping bug and add lua support by @bintester
- Ensure scissor state is set to the correct value prior to a buffer clear by @rh101
- Fix memory leaks when app exit on non-embed systems by @halx99 and @lich426
- Fix can't generate lua bindings for set/get Node localZOrder, reporeted by @bintester
- Fix lua template include incorrect module path, reported by @grif-on
- Fix build win32 with clang error
- Fix ci build-site download unexpected artifacts from appveyor

## Breaking changes

- Rename android entrypoint: `cocos_android_app_init` => `axmol_android_app_init`

## Improvements

- Add some utils functions and lua support by @bintester

## 3rdparties updates

- simdjson: 3.6.3 => 3.6.4
- curl: 8.5.0 => 8.6.0
- openssl: 3.0.12 => 3.0.13
- yasio: 4.1.4 => 4.2.0(5.0.0)
- zlib: 1.3 => 1.3.1
- jpeg-turbo: 3.0.1 => 3.0.2
- libpng: 1.6.40 => 1.6.42
- luajit: 2.1-43d0a19 => 2.1-9cc2e42


# axmol-2.1.0 Jan.20 2024

## Significant changes relative to 2.0.0:

- Migrate Windows UWP from C++/CX to [cppwinrt](https://github.com/microsoft/cppwinrt) for c++20 codebase by @halx99
- Refactor Target Platforms Enums
- Update AGP to 8.2.1 Android Studio 2023.1.1+
  - Update android API to 34
- Add support for enabling TTF SDF rendering globally by @halx99
- Add extension: SDFGen to generate prebaked font atlas by @halx99
- Add API: `FontAtlasCache::preloadFontAtlas` to support label SDF rendering with prebaked atlas by @halx99
- Add API `Device::getPixelRatio` by @halx99
- Add ImGui android DPI scaling support by @halx99
- Add winuwp FPS control support by @halx99
- Add API: `compressGZ`, `decompressGZ` by @halx99
- Combine `backend::Device` and `backend::DeviceInfo` into `backend::DriverBase` by @halx99
- Improve axmol console scripts by @halx99
- Rebase main repo, repo size reduce from 1.1GB+ to ~80MB by @halx99
  - Add simple cmake pkg manager aka `1k/fetch.cmake` to download all prebuilts,optionals,sample-assets at cmake config step
  - pkg config is in `$AX_ROOT/manifest.json`
  - Download cache dir is `$AX_ROOT/cache`
  - Also support simple mirror to gitee.com for china region via create a empty file name with `.gitee` in dir `$AX_ROOT/1k/`
- Add extension: JSONDefault by @sarooptech
- Add virtual file system support to WICImageLoader by @rh101
- Add support for Richtext vertical alignment by @rh101
- Add support for ActionCoroutine like Unity by @lich426
- Add API: `DrawNode::drawPie` by @aismann
- Add support for window resized and positioned by @lich426
- Add extension: Inspector by @iAndyHD3
- Add *experimental* WebSocket support for both wasm and native platforms by @halx99 & @rh101
- Add extension: DrawNodeEx by @aismann
- Add fairygui lua support by @Samoongeer
- Enable Mat4 SSE Istructions acceleration by @halx99
- Add dotnet API styled class `JsonWriter` based on rapidjson by @halx99
- Add donation page
- Add multi UserDefault support by @armanhossiny
- Add fixedUpdate method to Scene by @lich426

## Mark as deprecated, and will be removed in next release axmol-2.2.0
- 32bit build
- Extensions
    - DragonBones
    - GUI
- Zwoptex (Commented on cpp-test)

## BugFixes

- Fix translated metal shader texture order does not follow GLSL binding order
- Fix GLES3 shader scalar array layout incorrect
- Fix Android ImGUI coord conversions
- Fix Lua build on windows
- Fix `axmol new` not support absolute path
- Fix some ImGui issues
  - Fix crash
  - Fix wasm touch event not handle properly
  - Fix add/remove render loop problem
- Fix wasm HelloLua/lua_tests error
- Fix wasm render system font aliasing problem, caused by incorrect premultiply alpha
- Fix text rendering layout incorrect due to kernings not multiply content scale factor
- Fix wasm RandomHelper always return 0 problem
- Fix label outline not work on metal graphics backend
- Fix command `axmol build` can't build android on macos
- Fix cmake may geneate 2 targets for sync axslc folder
- Fix windows prebuilt cache issue
- Fix Lua `FileUtils getDataFromFile` may return path of file
- Fix issue where RichText URL touch listener does not activate on touch by @rh101
- Fix for missing scripts and resources in lua tests project by @rh101
- Fix for UWP build issues by @rh101
- Fix UWP issues in `EditBoxWinRT` by @rh101
- Fix for TextField keyboard not opening on receiving focus on android by @rh101
- Fix plist data and version by @paulocoutinhox
- Fix build cannot correctly handle '-' on the project name
- Fix TMX Camera Culling by @DelinWorks
- Fix cmake project generation on platforms other than Android by @rh101
- Fix UILayout crash problem by @aismann
- Fix replace scene missing nullcheck for next scene by @armanhossiny
- Fix a crash caused by missing android view nullcheck by @Arvant
- Fix shader cache leak by @j-jorge
- Fix AXASSERT() not working with more complex msg arguments by @smilediver
- Fix assertion fail when re-create axmol imgui backend by @Samoongeer
- Fix AudioPlayer seek to incorrect frame after setTime invoked by @GuoliangWang


## Improvements

- Improve `setup.ps1`, auto set windows `powershell.exe` execution policy to `Bypass`
- Reduce memory usage when using getFileMD5Hash function by @rh101
- Improve install-pwsh.sh
- Apple platforms require cmake-3.28.0
- Rename `AX_USE_SHARED_PREBUILT` to `BUILD_SHARED_LIBS` in AXLinkHelper.cmake
- Rename GLFW view name to `axmol2`
- Remove Apple Controller deprecated .gamePad check branch
- Remove dep: libzip-dev for linux
- Auto setup llvm for win32 target platform
- Optimize Label memory size, reduce 40 bytes
- Support adding custom properties to the inspector for derived types by @rh101
- Use std containers instead uthash
- Imgui and Inspector enhancements by @rh101
- [Android] Reset the OpenGL state cache if the context is lost by @rh101
- [Android] Fix corrupted FPS stats font textures on OpenGL context lost by @rh101
- Update Live2D to work with Axmol 2.x by @rh101
- [Android] Enable support for restarting application process if OpenGL context is lost by @rh101
- Make AxmolEditBox invisible by default by @rh101
- [UWP] Use Package to get app version information by @rh101
- Improve `class DrawNode` by @aismann
- [CMake] Use execute_process instead of deprecated exec_program by @rh101
- Move fmt folder to Thirdparty folder within project by @rh101
- Remove duplicated libz for ios/tvos
- Improve info.plist for apple platforms by @paulocoutinhox
- Improve android build.gradle by @paulocoutinhox
- Fix large number of generated warnings by @rh101
- Add useful chipmunk function to `axmol's internal physics engine` by @DelinWorks
- Replace 'CCAssert' with 'AXASSERT' by @aismann
- Improve the default templates by @aismann
- Call Android native renderer while opengl context is still valid by @rh101
- Make `FileUtils::openFileStream` a const method by @rh101
- Improve cmake scripts for managing 3rd libs
- Add `Get TMXTilesetInfo` by name support by @lich426
- Include hpp and c files in CMakeLists.txt of cpp template by @@lich426
- Improve gradle scripts, config each build-variants for easy add custom build-variant, refer #1571
- Allow to override AX_USE_COMPAT_GL from user's `CMakeLists.txt` by @smilediver
- Fix android gradle AGP8 warnings, and rename keystore props:
  - RELEASE_STORE_FILE => KEY_STORE_FILE
  - RELEASE_KEY_STORE_PASSWORD => KEY_STORE_PASSWORD
  - RELEASE_KEY_ALIAS => KEY_ALIAS
  - RELEASE_KEY_PASSWORD => KEY_PASSWORD
- Enable template projects' `aidl` by default for In-app purchases by @armanhossiny

## Breaking changes

- Rename glview to correct representative name: `OpenGLView` => `GLView` by @paulocoutinhox

## 3rdparties updates

- astcenc: 4.5.0 => 4.7.0
- c-ares: 1.19.1 => 1.25.0
- clipper2: 1.2.2 => 1.3.0
- curl: 8.2.1 => 8.5.0
- jpeg-turbo: 3.0.0 => 3.0.1
- llhttp: 9.0.1 => 9.1.3
- ntcvt: 1.0.0 => 1.0.1
- oboe: 1.7.0 => 1.8.0
- openssl: 3.0.10 => 3.0.12
- rapidjson: 1.1.0 => 1.1.0-b4a6da3
- simdjson(NEW): 3.6.3
- webp: 1.3.1 => 1.3.2
- yasio: 4.1.0 => 4.1.4
- fmtlib: 10.1.1 => 10.2.1
- imgui: 1.89.8 => 1.90.1

Notes:

- WASM require win8.1+ on windows

# axmol-2.0.0 Sep.9 2023

-  Add *experimental* WebAssembly build support(WebGL 2.0) by [@nowasm](https://github.com/nowasm)
    - List of known issues maybe *help wanted*
        - WebGL context lost not handled yet
     - A demo avaiable on axmol gh-pages: [`cpp-tests.html`](https://axmol.dev/axmol/wasm/cpp_tests/cpp_tests.html), build from branch `dev`
     - Note:  Follow functionals require wasm pthread support, and needs server enable https://web.dev/coop-coep/ response with specified header, and by default thread support was enabled by axmol client build commands
         - AudioEngine
         - ASTC multi-thread software decoder
         - `TextureCache::addImageAsync`
         - `MeshRenderer::createAsync`
         -  Extension: live2d not support due to it's sdk core not opensource and not contains wasm prebuilt libs
     - Quick build and run `cpp_tests` on local machine, the command use `emrun` to preview, and it's response with specified headers which wasm pthread support required, so above functionals with multi-threading works well.
         - `axmol run -p wasm -xb "--target,cpp_tests"`
     - Add 2 cmake options: `AX_WASM_THREADS`, `AX_WASM_SHELL_FILE`, refer to [CMakeOptions.md](CMakeOptions.md)
- OpenGL3/GLES3 support, add new cmake option `AX_GLES_PROFILE` to control GLES profile, refer to [#1279](https://github.com/axmolengine/axmol/issues/1279)
- Mesh instancing draw support
- Use [glslcc](https://github.com/axmolengine/glslcc)(a spriv-corss & glslang wrapper tool) as new shader workflow, write shader by ESSL310, build to target platforms: 
    - MSL for Apple(macOS/iOS/tvOS)
    - ESSL300 for Android, ANGLE
    - GLSL330 for Desktop GL
    - GLSL100 for Android old devices e.g android-4.2
    - Restrict ESSL310 input shader: please only define 1 uniform block at per shader stage
- Improve ProgramManager for loading custom shader program more easy
- Improve text rendering, label SDF outline support
- Improve FileStream, handle large 4GB+ files
- Improve ASTC hardware support detection
- Use ANGLE(d3d11) as default renderer library on win32
- Thirdparty Updates
    - ANGLE: 113.0.5672.128 => 115.0.5790.173
    - curl: 8.1.2 => 8.2.1
    - fmtlib: 10.0.0 => 10.1.0
    - FreeType: 2.13.0 => 2.13.2
    - llhttp: 8.1.1 => 9.0.1
    - OpenSSL: 3.0.9 => 3.0.10
    - webp: 1.3.0 => 1.3.1
    - xxHash: 0.8.1 => 0.8.2
    - zlib: 1.2.13 => 1.3
- API changes:
    - CHANGED: The prototype of [`ProgramManager::registerCustomProgram`](https://axmol.dev/manual/latest/d1/db2/classax_1_1backend_1_1_program_manager.html#ab0c9fa9f0ebef5f20a868e2ea6cdc631) was changed
    - ADDED: `ProgramManager::loadProgram`
    - REMOVED: `ProgramManager::getCustomProgram`
    - REMOVED: `CommandBuffer::setLineWidth`
- Many other improvements
  
  
# axmol-1.0.1 Sep.8 2023

- [NEW] Add [`install-pwsh.sh`](https://github.com/axmolengine/axmol/blob/dev/install-pwsh.sh) to make installing powershell on `macOS`, `Ubuntu`, `ArchLinux` more easier
- [NEW] **Axmol console next**, now almost powershell (except luabinding generator required python3)
  - The axmol console commands `axmol run`, `axmol deploy` aslo support `winuwp`
  - *The axmol console commands not only used by axmol, it's could be used to build any other project based on cmake build system*, i.e, goto any cmake based project directory, then run command: ```axmol build -p win32 -xb "--target, <your_project_cmake_target>"```
  
- [NEW] `ArchLinux` now official supported by axmol community, means, with 2 steps: 
  1. `install-pwsh.sh`
  2. `pwsh .\setup.ps1`
then you can compile for targets: `linux` and `android` (apk) by commands in your axmol's project root directory
  - `axmol build -p linux` for linux
  - `axmol build -p android -a arm64` for android
- [REFINE] Lock android ndk revision to **r23c**
  
  
axmol-1.0.0  Aug.9 2023

- [HIGHLIGHT] Reimplemented MediaPlayer support play media(video/audio) files for all platforms, and ui::VideoPlayer was renamed to ui::MediaPlayer
- [HIGHLIGHT] Windows Store UWP apps support
- [HIGHLIGHT] Add build windows with llvm-clang + ninja support
- [HIGHLIGHT] Add extensions `Effekseer` support
- [HIGHLIGHT] Replace all string map with `robin_map` which support heterogeneous lookup
- [HIGHLIGHT] Add precompile_headers all platforms support, thanks to @crazyhappygame
- [HIGHLIGHT] Refactor `VertexLayout` sharing mechanism
- [HIGHLIGHT] Refactor shader program management: link when use, rename `ProgramCache` to `ProgramManager`
- [HIGHLIGHT] Improve MeshRenderer #879 thanks @solan-solan (#904) 
- [HIGHLIGHT] Improve model loader, auto set texture for .obj file when texture path not specified
- [HIGHLIGHT] Add apple M1, android x64 support, contributed by @pietpukkel
- [HIGHLIGHT] Add apple tvos support, thanks to @paulocoutinhox
- [HIGHLIGHT] Add Particle System Animations, Alpha Masks & Improvements, thanks to @DelinWorks
- [HIGHLIGHT] Add wireframe rendering and function classification improvements, thanks to @DelinWorks
- [HIGHLIGHT] Improve text rendering
- [HIGHLIGHT] Improve windows workflow, support linking with engine prebuilt libs
- [HIGHLIGHT] Improve spine RTTI compare performance when compiler string pooling enabled
- [HIGHLIGHT] Implement Windows WebView using WebView2 Edge Chromium, thanks to @rh101
- [HIGHLIGHT] Windows x64 build support
- [HIGHLIGHT] Support custom texture atlas formats, thanks to @rh101
- [HIGHLIGHT] Downloader realtime md5 checksum calculation support
- [HIGHLIGHT] Decompress astc parallel support
- [HIGHLIGHT] Reimplement HttpClient based on yasio for concurrent http request support
- [HIGHLIGHT] Improve Lua RTTI performance and less memory cost
- [HIGHLIGHT] Virtual File System support, thanks to @rh101 
- [HIGHLIGHT] Refactor lua loader, speed up 30%+
- [HIGHLIGHT] Update plainlua version to 5.4.4
- [HIGHLIGHT] Use Openal-Soft for all platform
- [HIGHLIGHT] Refactor UserDefault with mio, speed up 100x+
- [HIGHLIGHT] Implement all .wav formats supported by Openal-Soft, such as MS-ADPCM, ADPCM
- [HIGHLIGHT] Use modern GL loader glad (v2.0.2)
- [HIGHLIGHT] Add google angle (version 5672) renderer backend support for windows
- [HIGHLIGHT] Update codebase to C++ 17/20 standard
- [HIGHLIGHT] Remove tinyxml2
- [HIGHLIGHT] Use fast pugixml
- [HIGHLIGHT] Spine-3.6~4.1 support, default is 4.1
- [HIGHLIGHT] ASTC 4x4/6x6/8x8 support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ETC2 RGB/RGBA support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ImGui integrated for Android platform, thanks to @rh101
- [HIGHLIGHT] ImGui integrated for PC platforms
- [HIGHLIGHT] Add extension `FairyGUI` support
- [HIGHLIGHT] Add extension `Live2D` support
- [HIGHLIGHT] Use curl for transferring data with URL syntax
- [HIGHLIGHT] Modularize all optional extension, move from engine core to folder extensions
- [HIGHLIGHT] Improve thirdparty libs building, 100% of them build from sources or github actions with latest toolchain, see also: `simdsoft/1kiss`
- [HIGHLIGHT] Add new API `Director::setChildrenIndexerEnabled` for speed up getChildByTag & getChildByName support
- [HIGHLIGHT] Add new API `FontFreeType::setStreamParsingEnabled` for stream parsing support . It's very useful for reducing memory costs when loading a large .ttf font file
- [HIGHLIGHT] Remove all unnecessary `std::nothrow` stubs
- [HIGHLIGHT] Use c++17 string_view instead `const std::string&`
- [HIGHLIGHT] Improve code quality (stage1.Oct) (#916) 
- [HIGHLIGHT] Remove CC prefix from code file name
- [NEW] Add closeKeyboard() method for EditBox, thanks to @theunwisewolf
- [NEW] Added automake and automake to dependency list, thanks to @clawfinger
- [NEW] Add primitive type support for mesh materials, thanks to @DelinWorks
- [NEW] Add int64_t/uint64_t support for `cocos2d::Value`
- [NEW] Add support for scaling for RichElementImage, thanks to @theunwisewolf
- [NEW] Add fmtlib 10.0.0
- [NEW] Add llhttp 8.1.0
- [NEW] Add return type 'ResourceData' for lua bindings, thanks to @tkzcfc
- [NEW] Add spine binary support for lua bindings, thanks to @tkzcfc
- [NEW] Add GL_RGBA32F GL backend support by @solan-solan
- [NEW] CI: add upload android apk support by @crazyhappygame
- [NEW] Add Anchored Sprite Class by @DelinWorks
- [NEW] Update app icon to Axmol logo by @rh101
- [NEW] Add HSV/HSL types support by @DelinWorks
- [NEW] Add support for haptic feedback on iOS and Android devices by @maksg
- [FIX] Fix for UIEditBox password on Mac/iOS (Montery+/15+), thanks to @Niborski
- [Fix] Fix to ensure Component::onEnter() is called (#867), thanks to @rh101
- [Fix] Fix Label overflow shrink bug
- [FIX] Fix uniform location mismatch when more than 1 spine with different shaders  
- [FIX] Fix imgui draw frame cause afterimage when game scene nothing to draw
- [FIX] Set global Z value of label debug layer to be the same as the parent label to fix display issue
- [FIX] Use TTF scaled metrics to calculate line height, thanks to @rh101
- [FIX] Fix Memory leak in ShaderModuleGL::getErrorLog(), thanks to @Xrysnow
- [FIX] Fix console output text encoding for win32
- [FIX] Fix charset process for fontName on win32
- [FIX] Fix crash on ParticleBatchNode::updateProgramStateTexture() 
- [FIX] Fix crash on SpriteBatchNode::appendChild when CC_SPRITE_DEBUG_DRAW==1
- [FIX] Fix Lua can't get `unsigned char` or `unsigned int` values of `ValueMap`
- [FIX] Fix crash on AudioEngine::end after the `Director` was destroyed
- [FIX] Fix font atlas will leak when it does not exist in the atlas cache, thanks to @rh101
- [FIX] Fix ScrollView shows incorrect position in vertical direction some time, thanks to @wzhengsen
- [FIX] Fix Spine ClippingAttachment doesn't work
- [FIX] Fix render problem with boss.obj
- [FIX] Fix downloader crash in lua coroutine, thanks to @tkzcfc
- [FIX] Fix memory leak (VertexLayout) at `Program` constructor
- [FIX] Fix missed edge case when resizing the memorymapped file in UserDefault that could cause a crash, thanks to  @theunwisewolf
- [FIX] Fix http post with uri params
- [FIX] Fix extension issue - FairyGUI BMFont don't load, reported by @theSlyest
- [FIX] Fix download task creation failure Program crash, reported by @tkzcfc
- [FIX] Fix charset process in glfwCharCallback (cocos2dx-all)
- [FIX] Fix positionColorLengthTexture.vert shader compilation, thanks to @DelinWorks
- [FIX] Fixed resize on Windows by @Niborski
- [FIX] Trim leading space from wrapped lines of text in RichText by @rh101
- [FIX] Fix TileMap OpenGL pixel-tolerance by @DelinWorks
- [FIX] Fix TileMap flipped/rotated animated tiles incorrect rendering by @DelinWorks
- [FIX] Fix TiledMap sprite position by @rh101
- [FIX] Fix for incorrect tile sprite setup on creation by @rh101
- [FIX] Fix return reference to local temporary value by @rh101
- [FIX] Fix TMX Tiled Map CSV, BASE64, & ZLIB incorrect parsing by @DelinWorks
- [FIX] Fix to ensure new line is correctly added to RichText by @rh101
- [FIX] Fix tilemap camera move and zoom culling by @DelinWorks
- [FIX] Fix some audio test case can't hear sound
- [FIX] Fix RenderTexture memory leak and increased memory usage when initWithWidthAndHeight is called by @rh101
- [REFINE] Improved error handling in shader initialization by @CodeAndWeb
- [REFINE] Move getWroldPosition from Widget to Node class, thanks to @rh101
- [REFINE] Improve `FileUtils::addSearchPath`, thanks to @tkzcfc
- [REFINE] Allow the forced reformatting of RichText contents, thanks to @rh101
- [REFINE] Upgrade android toolsets to ndk-r23 LTS
- [REFINE] CCValue move construct and assign with std::string support
- [REFINE] Improve windows dev workflow, use working directory instead copy resources to build binary directory
- [REFINE] Add lua debug project file to lua project template
- [REFINE] Remove lua 64bit spec search path, since we can compile compatible bytecode for both plainlua and luajit
- [REFINE] Put stats labels to safe origin to make sure we can see it integral at some mobile device
- [REFINE] Improve cmake scripts
- [REFINE] Replace deprecated repo `jcenter` with `mavenCentral`
- [REFINE] Lua-5.4 compatible
- [REFINE] Improve cmdline-tools
- [REFINE] Ensure label underline node has the same global Z value as the label node to fix visibility issue when global Z is not 0
- [REFINE] Improve android astc support check
- [REFINE] Improve the "Physics part" (#379), thanks to @aismann 
- [REFINE] Rename command line to to `axmol`
- [REFINE] Update imgui to 1.88
- [REFINE] Rename thirdparty folder `external` to `thirdparty`
- [REFINE] Building plainlua as dll on windows always for debugger happy to debug app quickly
- [REFINE] Avoid OS_WINDOWS conflict with system macro
- [REFINE] Update OpenSSL to v3.0.x latest
- [REFINE] Update FreeType to latest
- [REFINE] Use Freetype SDF render when label distance field enabled
- [REFINE] Always enable string pooling for msvc on cmake
- [REFINE] Change DrawNode api color parameters from `Color4F` to low `Color4B`
- [REFINE] Improve DrawNode GC allocs
- [REFINE] Use `jni.hpp` to improve jni call without method signature GC allocs
- [REFINE] Rename is64BitIOSDevice to is64BitMobileDevice
- [REFINE] Fix Android screen blank issue after unlocking device with application running, thanks to @rh101
- [REFINE] Update zlib to v1.2.13
- [REFINE] Add explicit cmake options
- [REFINE] Migrate cross platform ci checks to github actions
- [REFINE] Replace `push_back` with `emplace_back`
- [REFINE] Improve Camera, thanks to @DelinWorks
- [REFINE] Use jpeg-turbo for jpg image decoding
- [REFINE] Update clipper1 to clipper2 1.x
- [REFINE] Update flatbuffers to v2.0.8
- [REFINE] Change Program::getActiveAttributes return type to reference
- [REFINE] Rename `Sprite3D` to `MeshRenderer`
- [REFINE] Improve AutoPolygon::generateTriangles performance (#905)
- [REFINE] Add many box2d testbed, thanks to @aismann
- [REFINE] Improve c++11 for-loop performance with Universal References ([#760](https://github.com/axmolengine/axmol/pull/760)), thanks to @aismann
- [REFINE] New engine logo & splash
- [REFINE] Improve gradle scripts
- [REFINE] Replace use of deprecated sprintf with snprintf, thanks to @euler0
- [REFINE] Fix some warnings compilation in linux gcc 12.2.0, thanks to @Joilnen
- [REFINE] Add linux prebuilt support, thanks to @AsakuraMizu
- [REFINE] Fix wanings implicit conversion loses and use of deprecated function, thanks to @euler0
- [REFINE] Clarify the order of steps required to create and build new projects, thanks to @rh101
- [REFINE] Add a pool for GroupCommand rendering commands, thanks to @rh101
- [REFINE] Improve ci scripts, thanks to @crazyhappygame
- [REFINE] Improve lua and android cmake, thanks to @crazyhappygame
- [REFINE] Update libpng to 1.6.39
- [REFINE] Improve cpp template, thanks to @aismann
- [REFINE] Improve drawCircle rendering for big ring, thanks to @aismann
- [REFINE] Add windows dll build checks for GH actions
- [REFINE] Improve test naming by @DelinWorks
- [REFINE] Scrollbar visibility fix for when auto hide is disabled by @rh101
- [REFINE] Improve richtext word wrapping by @rh101
- [REFINE] Tidy project tree and full symlink support
- [REFINE] Impore Metal Renderer Dirty Detection by @Yehsam23
- [REFINE] Improve Android performance
- [REFINE] Add 1 overload AudioEngine::play2d for time offset support
- [REFINE] Improve exit flow
- [REFINE] Use ipv4 address preferred for console command server
- [REFINE] Reopen alsoft device when default device changed
- [REFINE] Factor in protected nodes when creating global Z node priority map for event dispatcher by @rh101
  
  
cocos2d-x-4.0  Dec.3 2019

- [HIGHLIGHT]     Support metal
- [HIGHLIGHT]     Use CMake for all platforms
- [REFINE]        Update glfw to 3.3
- [REFINE]        Update minizip to 1.2
- [CHANGE]        remove deprecated functions
- [CHANGE]        remove h5 engine and JSB
- [CHANGE]        remove tiff
- [CHANGE]        remove `experimental` namespace
- [FIX]           system font can not work correctly on macOS 15
- [FIX]           TextField can not work if using system input to get Chinese characters with iOS13
- [FIX]           UIWebView uses WKWebView instead
- [FIX]           VideoPlayer uses AVPlayerController instead
- [FX]            lua crashes on 64it devcices 
