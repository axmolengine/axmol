# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [4-r.4] - 2021-12-09

### Added

* Add the rendering options on Metal:
  * `USE_RENDER_TARGET`
  * `USE_MODEL_RENDER_TARGET`

* Add anisotropic filtering to Metal.
* Add a macro to toggle the precision of floating point numbers in OpenGL fragment shaders.
* Add a function to check `.cdi3.json` exists from `.model3.json`.
* Add `CubismJsonHolder`, a common class used to instantiate and check the validity of `CubismJson`.
  * Each Json parser will now warn if an class of `CubismJson` is invalid.

### Changed

* Change each Json parser inherits a common class `CubismJsonHolder`.

### Fixed

* Fix renderer for Cocos2d-x v4.0.
   * `RenderTexture` was empty when using `USE_MODEL_RENDER_TARGET`.
* Fix motions with different fade times from switching properly.
* Fix a bug that motions currently played do not fade out when play a motion.


## [4-r.4-beta.1] - 2021-10-07

### Added

* Add a function to parse the opacity from `.motion3.json`.
* Add a Renderer for Metal API in iOS.
  * There are some restrictions, see [NOTICE.md](https://github.com/Live2D/CubismNativeSamples/blob/e4144053d1546473d2e76d30779e26d84b00d9f9/NOTICE.md).

### Fixed

* Fix return correct error values for out-of-index arguments in cubismjson by [@cocor-au-lait](https://github.com/cocor-au-lait).
* Fix a warning when `SegmentType` could not be obtained when loading motion.
* Fix renderer for Cocos2d-x v4.0.
  * Rendering didn't work when using `USE_RENDER_TARGET` and high precision masking.


## [4-r.3] - 2021-06-10

## [4-r.3-beta.1] - 2021-05-13

### Added

* Add a Renderer for Cocos2d-x v4.0.
* Implement a function to get the correct value when the time axis of the Bezier handle cannot be linear.
* Add an argument to the function `SetClippingMaskBufferSize` to set the height and width of the clipping mask buffer.

### Changed

* Improve the quality of Clipping Mask on high precision masking.


## [4-r.2] - 2021-02-17

### Added

* Implement anisotropic filtering for DirectX-based Renderer.
* Implement get pixel size and `PixelsPerUnit` of the model

### Changed

* Check pointer before use to avoid crash by [@Xrysnow](https://github.com/Xrysnow)

### Fixed

* Fix Physics input reflect flag on evaluate.
* Fix renderer for OpenGL.
  * Add delete mask buffer when renderer instance is destroyed.
* Fix delay in starting fade-out for expressions.
* Fix memory bug causing segmentation fault when reallocating memory by [@adrianiainlam](https://github.com/adrianiainlam)
* Fix reference size of model matrix.
* Fix memory leaking the color buffer on destroyed `CubismOffscreenFrame_OpenGLES2`.
* Fix argument name typo at `CubismEyeBlink::SetBlinkingInterval()`.


## [4-r.1] - 2020-01-30

### Added

* Add the callback function called on finished motion playback.

### Changed

* Include header files in CMake.
* `<GL/glew>` is not included on macOS if `CSM_TARGET_COCOS` is defined.

### Fixed

* Fix rendering not working properly on Android devices with Tegra.

### Deprecated

* Use `target_include_directories` instead of using `FRAMEWORK_XXX_INCLUDE_PATH` variable in application CMake.
* Use `target_compile_definitions` instead of using `FRAMEWORK_DEFINITIOINS` variable in application CMake.
* Specify `FRAMEWORK_SOURCE` variable also in OpenGL application CMake.


## [4-beta.2] - 2019-11-14

### Added

* Add the includes to `Framework` for Linux build.

### Changed

* Refactoring `CMakeLists.txt`

### Fixed

* Fix renderer for DirectX 9 / 11.
  * Add missing implementation: Check the dynamic flags.


## [4-beta.1] - 2019-09-04

### Added

* Support new Inverted Masking features.
* Add `.editorconfig` and `.gitattributes` to manage file formats.
* Add `.gitignore`.
* Add `CHANGELOG.md`.

### Changed

* Convert all file formats according to `.editorconfig`.

### Fixed

* Fix typo of `CubismCdiJson`.
* Fix invalid expressions of `CubismCdiJson`.


[4-r.4-beta.1]: https://github.com/Live2D/CubismNativeFramework/compare/4-r.3...4-r.4-beta.1
[4-r.3]: https://github.com/Live2D/CubismNativeFramework/compare/4-r.3-beta.1...4-r.3
[4-r.3-beta.1]: https://github.com/Live2D/CubismNativeFramework/compare/4-r.2...4-r.3-beta.1
[4-r.2]: https://github.com/Live2D/CubismNativeFramework/compare/4-r.1...4-r.2
[4-r.1]: https://github.com/Live2D/CubismNativeFramework/compare/4-beta.2...4-r.1
[4-beta.2]: https://github.com/Live2D/CubismNativeFramework/compare/4-beta.1...4-beta.2
[4-beta.1]: https://github.com/Live2D/CubismNativeFramework/compare/0f5da4981cc636fe3892bb94d5c60137c9cf1eb1...4-beta.
