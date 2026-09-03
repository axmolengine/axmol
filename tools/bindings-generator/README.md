# Axmol Lua binding generator

Lua API changes and application migration notes are documented in
[`extensions/scripting/lua-bindings/MIGRATION.md`](../../extensions/scripting/lua-bindings/MIGRATION.md).

The binding toolchain is intentionally split into three layers:

1. PowerShell owns repository and host-toolchain discovery.
2. `Axmol.LuaBindings.Generator` owns module configuration, policy validation,
   Clang AST inspection, and C++ emission.
3. `lua-bindings/runtime/axlua_runtime.*` owns Lua object identity, peer tables,
   lifecycle invalidation, and Lua-version compatibility.

Generated files only describe the native class surface and use sol2's checked-in
`std::function` adapter for ordinary C++ callbacks; userdata identity and
lifecycle are never reimplemented in a generated module.

Overload groups are emitted as `axlua::overload` candidates, not
`sol::overload`.  The runtime first selects a candidate by Lua argument count
and checked stack signature, then invokes that candidate through sol2. This
keeps overload dispatch deterministic while respecting Axmol's independent
inheritance and userdata-identity registry.

## Runtime compatibility

The runtime uses the Lua C API compatibility branches for Lua 5.1 through 5.5
and LuaJIT 2.1+.  The sol2 dependency is Axmol's checked-in, Lua 5.5-capable
3.5.0 fork under `3rdparty/lua/sol`; it is used for stack conversion and
callable adaptation, not as the owner of Axmol userdata identity.

Every `ax::Object` has a process-local `uint64_t` diagnostic ID exposed through
`getObjectID()`.  The ID is not exported to Lua and is never used as a registry
key. Native pointers index the identity cache, while each persistent userdata
has its own `ax::WeakPtr<ax::Object>` in a weak-key side table. The WeakPtr's
slot generation prevents address reuse from reviving an old userdata, even
after the address-indexed exposure record is replaced or removed. Generated
and adapter access share this lifetime check. Borrowed callback Events instead
expire at the end of their callback scope. `WeakPtr` remains in the runtime
layer; the core ScriptEngine interface remains Lua-agnostic. Native Object
reference counting and WeakPtr access still require external synchronization
when ownership is handed between threads.

Callbacks share a registry for each VM lifetime. Worker destruction only marks
references for release; the VM owner drains them before updates, at LuaStack
cleanup, or before a callback. Shutdown closes this registry under the same
lock, so late releases cannot touch a closed or reused Lua state. Callback
bookkeeping is removed on release rather than accumulated as weak entries.

Lua Scheduler callbacks use owning functions directly. The public native
integer-handler overload remains supported through the schedule-event bridge.
The headless `lua-binding-smoke` test covers both paths, object identity/address
reuse, worker callback release, borrowed Events, VM reopening, and the primary
LuaStack destructor with a `lua_close` finalizer sentinel.

The runtime keeps sol2's usertype metatables intact. Class inheritance is
stored in a separate Axmol registry, which preserves sol2 constructors and
static lookup while still allowing inherited instance methods and the public
`axlua.iskindof` check.

The generator preserves the historical distinction between a module table
and a type namespace.  RHI classes and their type names use `axr.*`; UI,
Spine, and FairyGUI keep their existing `axui.*`, `sp.*`, and `fgui.*`
spellings.

Each module also declares its accepted C++ namespace or namespaces. This keeps
umbrella headers from exporting unrelated transitive declarations. Duplicate
Lua class names are rejected both within a module and across a complete run.
Nested C++ enums are emitted under their owning Lua class and declarations seen
through multiple headers are deduplicated by fully qualified C++ name.

Public C++ data members are generated only from each module JSON's `fields`
whitelist. The runtime emits getter/setter closures through its native
`.get/.set` dispatch tables, so assignments such as `profile.name = "music"`
remain regular Lua property operations without hand-written wrappers.

## libclang runtime

The default `genbindings.ps1` flow downloads the platform `libclang` archive
from the repository's 1kiss `libclang` source, caches the archive under
`cache/devtools`, and copies only the required library to the historical path
`tools/bindings-generator/clang/prebuilt/libclang.<platform>`. A matching
library already at that path is reused. `-LibClangRoot <directory>` remains an
explicit override for CI tool caches and custom deployments.

The C# collector uses only the libclang C API through `ClangSharp.Interop`; it
does not load the higher-level ClangSharp AST, `clangcsharp`, or the
`libClangSharp` native shim. Native runtime provisioning is owned by this
entry point, as it was in the historical generator.

## Toolchain baseline

The reproducible managed dependency is the pinned .NET 8
`ClangSharp.Interop` assembly, which matches the PowerShell 7.4 runtime
baseline. `ClangSharpRoot` is only an explicit search-directory override for a
compatible prebuilt copy of that assembly; the default flow neither discovers
nor compiles a neighboring ClangSharp source checkout. Native libclang is
downloaded by the entry point from the checked-in 1kiss source configuration;
the C# generator itself does not compile or package that artifact.

The entry point accepts `-GeneratorAssembly` (or
`AXMOL_LUA_GENERATOR_ASSEMBLY`) to load a deployed C# assembly directly. If it
is omitted, PowerShell 7 compiles the generator sources with its built-in
`Add-Type` compiler; no dotnet SDK is required. The pinned
`ClangSharp.Interop` managed assembly is downloaded as a NuGet package only
when it is not already available in the generator directory or the local NuGet
cache. Generation is transactional per module: Clang errors and write failures
never intentionally replace the last valid generated files.
Platform-specific modules accept extra clang defines
through `-ExtraClangArguments`, for example `-DAX_ENABLE_VIDEO=1` or
`-DANDROID=1`; generated C++ also preserves each module's conditional guard.

Modules with a large template-instantiation workload can set `cpp_chunks` in
their JSON configuration. The stable `axlua_<module>_gen.cpp` then dispatches
to independently compiled shards: every shard creates its types first, all
members are bound in a second pass, and enums are registered last. This keeps
the existing public registration entry point and class-registration ordering
while avoiding a single oversized compiler job. Successful regeneration also
removes obsolete shards for that module.

## Clean checkout and CI workflow

The repository contains the generator source and an optional SDK project file
for Windows/Visual Studio debugging. The project file is not part of the
supported generation path: `genbindings.ps1` compiles the sources directly
with PowerShell `Add-Type`, so a clean machine
needs PowerShell 7.4 or newer and access to the NuGet feed containing the
pinned managed package. The first run downloads the exact
`ClangSharp.Interop` managed package version declared in `genbindings.ps1`,
compiles the generator with PowerShell itself, copies only that DLL beside the
generator, downloads the native libclang archive via the checked-in 1kiss
source configuration, and never restores or ships `libClangSharp` or the NuGet
`libclang` package.

`axmol genbindings` generates all configured modules by default. Use
`-m base` (or `-m ax_base`) for one module, or `-m all` to explicitly select
all modules. For a verification run:

```powershell
pwsh ./tools/cmdline/axmol.ps1 genbindings `
  -Verify
```

Verification parses every selected header without writing output. For an
intentional regeneration of checked-in `*_gen.h/.cpp` files and complete API
manifests, omit `-Verify`. Manifests are written below
`lua-bindings/generated/manifests` and record the module, classes, and both
class-owned and standalone enums for code review; the build does not consume
them.
For offline or network-isolated CI, publish the generator directory as an
artifact (or cache the NuGet package) before generation. CI can skip the SDK
and NuGet restore entirely by publishing the generator directory and passing
`-GeneratorAssembly <path-to-Axmol.LuaBindings.Generator.dll>`; the adjacent
`ClangSharp.Interop.dll` must be published with it. In both cases, the native
artifact is placed automatically under
`tools/bindings-generator/clang/prebuilt`. Android generation uses the same
managed generator; when `ANDROID_NDK` (or `-NdkRoot`) is absent, the entry
point invokes `setup.ps1 -p android` to preserve the historical workflow.

## Native callbacks

`std::function` parameters, including typedefs and aliases such as the UI
event callbacks and `ccMenuCallback`, are generated automatically. The
generated code passes the Lua function to the native API through the Axmol
runtime callback factory; manual lambda bridges are only retained for legacy
APIs whose native C++ surface is not a callback parameter.

sol2 stores the callback in a Lua registry-backed `unsafe_function`; the Lua
state teardown path invalidates that state before the native callback can be
used again. A `nil` callback becomes an empty `std::function`, so existing code
that clears callbacks keeps its old meaning. `ax::Coroutine` callbacks remain
explicitly excluded because their native return/ownership semantics are not
ordinary Lua callbacks.

## Virtual overrides and lookup fast paths

Generation uses Clang's `OverriddenCursors` to identify an actual C++ virtual
override. A derived declaration is omitted only when its exported base is also
selected and the Lua-visible name, return type, constness, parameter types, and
default arguments are equivalent. A derived method that introduces a same-name
overload keeps the complete overload group so the base API is not hidden.

At runtime, ordinary member lookup first checks the class and registered bases,
then falls back to accessor and sol2 lookup. The fast path does not cache a
closure or bypass runtime class edits. Owner-thread invalidation lets Release
avoid a repeated weak-pointer lookup while stale userdata still fails safely;
Debug retains the diagnostic lookup. These choices keep inherited methods fast
without weakening lifetime, virtual dispatch, or multi-VM behavior.

The Binding Performance Test is the regression signal for this work. Compare
runs using the same platform, optimization level, scene, and unchanged
`sprite:setPosition()`/`sprite:setOpacity()` calls. Results are hardware and
build dependent; the generator does not promise a fixed star count.
