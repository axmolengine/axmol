
## 2D Physics Engines informations:
- **Chipmunk2D** engine (Cpp Test: Chipmunk2D - Basic) 
  - Use class PhysicsSpriteChipmunk2D 
  
- **Box2D** engine (Cpp Test: Box2D - Basic)
  - Use class PhysicsSpriteBox2D 
  
- **Box2D-optimized** (This project is currently in alpha)
  - axmol add Box2D-optimized for your own tests 
  - Use cmake option: OPT_BOX2D_OPTIMIZED=ON to enable it on "axmol\thirdparty\CMakeLists.txt")

  
- **axmol 2D physics integration API** (Cpp Test: Node: Physics)
  - It using Chipmunk2D as internal 2D physics engine

- **Outdated/Abandom/Cocos2d-x**:
  - class PhysicsSprite: Be only part of the axmol for backwards compatibility with Cocos2d-x.
Use AX_ENABLE_BOX2D_INTEGRATION 1 (using Box2D) or AX_ENABLE_CHIPMUNK_INTEGRATION 1 (using Chipmunk2D)

## Chipmunk2D-/Box2D - Testbeds:
The goals where: 
- Using the original source files of the testbeds as possible. 
- Let the original source files be the original sources as much as possible.
- Give "a small view" to the Testbed examples.
- Please remind: Not all 'Box2D - TestBed' demos working full at the moment (because of different keyboard implementation, mouse buttons has some lags also).

## Support for custom texture atlas (sprite sheet) formats
- Default sprite sheet format is PLIST v3.  This is the format that the `TexturePacker` application outputs when you select the "Cocos2d-x" option.
- Custom sprite sheet format reader is created by implementing the `SpriteSheetLoader` interface, then registering this implementation by calling `SpriteFrameCache::registerSpriteSheetLoader()`.
- Custom SpriteSheetLoader must have a unique identifier, which is assigned by using the `SpriteSheetFormat::CUSTOM` as the base value. For example, `MyFormatId = SpriteSheetFormat::CUSTOM + 10`.
- In cpp-tests, SpriteFrameCache test has an implementation of a JSON sprite sheet format loader, implemented with the name `GenericJsonArraySpriteSheetLoader`.  Use the `GenericJsonArraySpriteSheetLoader` and the default `PlistSpriteSheetLoader` as examples on how to implement the `SpriteSheetLoader` interface.
- **Sprite sheets must always contain unique identifiers for the sprite frames.**  Loading more than one sprite sheet that uses the exact same frame identifers will result in undefined behaviour.  There are several ways to handle this, one being to prefix the graphic filenames, so for example, if two different files are named `image1.png`, one being for scene1 and another for scene of a game, then you would name them `scene1_image1.png`, `scene2_image1.png`.  The other, and possibly the better method, is to use sub-folders folders to differentiate between frame names, such as `scene1/image1.png` and `scene2/image1.png`, and ensure that the sub-folder name is not stripped out during sprite sheet creation.


## TTF font resources

- https://fonts.google.com/
