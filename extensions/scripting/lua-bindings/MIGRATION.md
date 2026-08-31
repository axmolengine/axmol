# Lua binding migration guide

Axmol's Lua bindings now use the PowerShell-hosted C# generator, libclang's C
API, and the checked-in sol2 3.5 runtime. Python, tolua++, and the historical
`auto` output directory are no longer part of the binding workflow.

Most existing Lua code keeps the same module and class names. The exceptions
below are intentional: they either expose the current native API accurately or
reflect a feature that is no longer present in Axmol.

Axmol v3 no longer creates the historical global `cc`, `ccui`, `ccb`, or
`ccexp` aliases. Use `ax`, `axui`, `axr`, and `axexp` directly. The bundled
framework and test scripts have already been migrated; applications must make
the same namespace change.

The v3 generator now owns ordinary `Node` value-type methods such as
`setContentSize` and `setRotationQuat`; pass the native table forms
(`ax.size(...)`, `ax.p(...)`, or `{x=..., y=..., z=..., w=...}`).
`setAnchorPoint` additionally keeps the compatibility forms
`setAnchorPoint(ax.p(x, y))` and `setAnchorPoint(x, y)`. The historical
two-number `Node:getPosition()` result is retained temporarily for existing
test/framework scripts. New code should prefer the native `Vec2` accessors
where available.

## Regenerating bindings

Run the generator from the repository root:

```powershell
./tools/cmdline/axmol genbindings
```

Configuration is stored as JSON under `tools/lua-bindings`. Generated C++ is
checked in as `*_gen.h/.cpp` under `extensions/scripting/lua-bindings/generated`.
The API manifests are checked in under its `manifests/` subdirectory. Do not
edit generated files directly.

## Particle classes

`ax.ParticleSystem` now represents the native `ax::ParticleSystem` class and
`ax.ParticleSystemQuad` represents `ax::ParticleSystemQuad`.

The former generator renamed `ParticleSystemQuad` to `ParticleSystem`. That
alias cannot coexist with automatically binding both native classes and has
been removed. Code that creates plist-backed 2D particles should use:

```lua
local particle = ax.ParticleSystemQuad:create("Particles/SmallSun.plist")
```

## Asynchronous jobs

`ax.AsyncTaskPool` is not exported because the corresponding native class was
removed from Axmol. Do not call `stopTasks` before leaving a scene. Code that
starts asynchronous work should instead own its callback state and ignore or
cancel completion at the API that created the operation. Native code that
needs general background work should use `JobSystem` or `Director::runAsync`;
there is no global Lua replacement for the old IO task-pool stop operation.

## Pointer capture

`PointerEventListener` follows the current `InputSystem` capture model. Its
PointerDown callback must return `true` when the listener needs subsequent
PointerMove and PointerUp events for that pointer:

```lua
listener.onPointerDown = function(event)
    return true
end
listener.onPointerMove = function(event)
    -- …
end
listener.onPointerUp = function(event)
    -- …
end
```

Use the current position accessors `getPoint`, `getPrevPoint`, `getWorldPoint`,
and `getPrevWorldPoint`. The old `getDelta`, `getStartLocation`, and
`getPreviousScreenLocation` methods no longer exist; calculate a screen-space
delta with `ax.pSub(event:getPoint(), event:getPrevPoint())`, and use
`getStartPoint` or `getStartWorldPoint` for the corresponding start position.
The old `getPreviousLocation` spelling remains an alias during migration but
new tests and applications should not use it.

The global `Director:setProjection` API was also removed. The active Scene
default camera now owns the projection; use `scene:getDefaultCamera()` and the
Camera `configurePerspective`/`configureOrthographic` APIs when a test needs a
custom projection. Ordinary 2D tests should leave the Scene's default camera
unchanged.

## Callbacks and scheduling

New code must use generated callback parameters and fields rather than
`registerScriptHandler`.  Public `std::function` fields such as
`PointerEventListener.onPointerDown`, `PointerEventListener.onPointerMove`,
`PointerEventListener.onPointerUp`, and the `KeyboardEventListener.onKey*`
fields accept ordinary Lua functions directly.

Physics contact listeners follow the same rule. Assign
`ContactEventListener2D.onContactBegin`, `onContactEnd`, `onCollisionHit`,
`onSensorBegin`, `onSensorEnd`, or `onContactFilter` directly; the 3D listener
has the corresponding fields.

Event arguments passed to these callbacks are borrowed userdata. They are
valid only while the callback is running and must not be retained in Lua for
later use. Persistent Axmol objects returned by the callback continue to use
the normal identity-preserving userdata behavior.

`Node:registerScriptHandler`, `Node:unregisterScriptHandler`,
`Node:scheduleUpdateWithPriorityLua`, and Pointer/Keyboard listener
`registerScriptHandler` overloads have been removed.  Use the generated
callback fields, `setLifecycleCallback`, or `schedule` as appropriate.

For Node lifecycle callbacks that still use the old event-name convention,
use the framework bridge below. It is implemented with Node's native callback
hooks and creates no ScriptHandler id:

```lua
node:setLifecycleCallback(function(event)
    if event == "enter" then
        -- …
    elseif event == "exit" then
        -- …
    end
end)
```

For updates, prefer `node:schedule(callback, interval, key)` and
`node:unschedule(key)`. `Node:onUpdate(callback)` remains a convenience helper
that uses the stable `axlua.node.update` key.

Passing `nil` to a generated callback setter clears the native callback.  The
runtime keeps callback invocation protected: a Lua error is reported and the
native call receives its default result instead of unwinding through C++.

## 2D physics fixed-step API

`PhysicsWorld2D:setUpdateRate` and `setFixedUpdateRate` were removed when the
physics update loop was unified with `Scene`. There is no per-world update-rate
replacement for the old default value of `1`. Configure the simulation at the
scene level with `setFixedDeltaTime`, `setMaxFixedStepsPerFrame`, and
`setFixedUpdateEnabled` when needed. For manually stepped worlds, use
`world:setAutoStep(false)` and `world:stepSimulation(delta)`; use
`world:setSubsteps(steps)` for multiple solver substeps within one automatic
physics update.

## Video and WebView

`MediaPlayer` was the old name and has been removed. The current video class is
exported as `axui.VideoPlayer`; migrate `axui.MediaPlayer` references to
`axui.VideoPlayer`. Use the native method spellings `setFullscreen`,
`isFullscreen`, `setKeepAspectRatio`, and `isKeepAspectRatio`.

Video and WebView are registered only when the target has a native backend.
Video is available on macOS, iOS, Android, and enabled Windows/Linux builds.
WebView is available on iOS, Android, enabled Windows builds, and Linux builds
with WebKit support. Neither feature is exposed by the WebAssembly target;
tests and applications must feature-detect `axui.VideoPlayer` or `axui.WebView`.

## Nested enums

Native enums declared inside a class are now exposed under that class instead
of being merged into the module table. This prevents unrelated enums named
`EventType`, `State`, or `Mode` from overwriting each other. For example:

```lua
local playing = axui.VideoPlayer.EventType.PLAYING
```

Historical named constant tables such as `ax.CameraFlag` remain available from
the Lua compatibility scripts. New bindings should use the native class-owned
enum path when one exists.

## Legacy OpenGL test

The unregistered Lua `OpenGLTest` was removed. It depended on the former
`GLProgram`, `GLNode`, and `GLProgramCache` APIs, which are not part of Axmol's
RHI-based runtime. RHI shader and program tests should use the current
`ax.ProgramManager`, `axr.Program`, and `axr.ProgramState` APIs instead.

## Object behavior

The new runtime preserves the behavior relied on by Axmol's Lua class helper:
Lua fields can be attached to native objects, Lua subclass methods override
native methods, repeated pushes of the same `ax::Object` preserve identity,
and inherited type checks use Axmol's native class graph. Object references are
invalidated through `WeakPtr` when native lifetime ends.

The runtime supports Lua 5.1 through 5.5 and LuaJIT 2.1+. Ordinary native
`std::function` callbacks are generated automatically; a manual binding is
needed only for APIs with ownership, coroutine, variadic, or other semantics
that cannot be represented by a normal callback parameter.

## Native C++ migration notes

The binding rewrite also tightened a few native-only contracts. `ax::Object`
is intentionally non-copyable and non-movable, its diagnostic ID is now a
private `uint64_t` exposed only through `getObjectID()`, and `Acceleration` is
no longer an `Object`. C++ callers that copied an `Object`, accessed `_ID` or
`_luaID`, or relied on `Acceleration` inheritance must migrate; these members
were never part of the supported Lua API. Generated bindings omit native
implementation records without a registered table converter (for example
`SceneRenderState`, `ContactInfo2D`, and the 3D asset loader records) rather
than exposing them as incompatible userdata.

For numeric overloads, Axmol deliberately keeps the historical tolua
behavior: integral overloads accept any Lua number and convert it natively.
When integer and floating overloads overlap, registration order therefore
remains observable. Prefer explicit integer/float values or an unambiguous
overload when adding new APIs.

# Controller Lua API removed

`ax.Controller`, `ax.ControllerEvent`, and `ax.ControllerEventListener` are no
longer exported to Lua. They were never registered by the v3 runtime and had
no in-tree Lua consumers. Keep game-controller input in C++, or add a new,
explicitly designed input API rather than restoring the legacy ScriptHandler
bridge.

## Compatibility debt retained intentionally

The rewrite removes the obsolete tolua++/Python pipeline and dead Node handler
paths, but it does not remove contracts that are still used by existing Lua
applications. The following are retained until a future breaking release:

* the peer/table and identity bridge used by adapter classes and old Lua object
  tables;
* the public `registerScriptHandler` method names where Lua compatibility still
  requires them; their implementations now own protected callbacks directly;
* `NodeEx.lua`'s fluent helpers, lifecycle event-name bridge, and compatibility
  aliases;
* deprecated Lua method names and the historical numeric-overload conversion
  rule (`SOL_NO_CHECK_NUMBER_PRECISION`).

These items cannot be deleted while preserving the current Lua API. They are
tracked as compatibility debt rather than expanded into new bindings. Removal
requires a separately announced breaking release, a migration period, and a
full Lua application audit. New APIs must use generated bindings and must not
add another compatibility adapter without documenting its contract.
