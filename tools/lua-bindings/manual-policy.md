# Lua binding maintenance policy

The Lua binding tree has three deliberately different parts:

* `extensions/scripting/lua-bindings/generated/` is disposable output. It is
  produced by `tools/cmdline/plugins/genbindings.ps1` and must not be edited by
  hand. Each module emits `axlua_<module>_gen.h/.cpp`; `generated/manifests/`
  contains the complete API-review snapshot.
* `extensions/scripting/lua-bindings/runtime/` owns Lua state lifetime, object
  identity, peer tables, WeakPtr invalidation, registration bootstrap, and the
  callback factory.
* `extensions/scripting/lua-bindings/adapters/` and `platform/` contain the
  narrow API and OS-bound bridges that cannot be represented by a C++ signature
  alone. They are not a second general-purpose binding generator.

## What belongs in generated output

Public classes, constructors, inheritance, ordinary methods, overloads,
default arguments, enums, and `std::function`/callback parameters belong in
the C# generator. A new API should first be made visible through a module JSON
configuration and generated output.

The checked-in sol2 3.5.0 fork receives `std::function` values through the
Axmol callback factory. This covers ordinary C++ lambdas without a hand-written
Lua closure bridge.

## What may remain specialized

Adapter code is justified only when at least one of these applies:

1. The API is a Lua compatibility surface rather than a native C++ API, such
   as `registerScriptHandler`, peer tables, or legacy type
   names.
2. The API converts a dynamic Lua table/value into an Axmol aggregate or
   container and has no stable C++ parameter representation.
3. The API exposes a platform/browser/Java/Objective-C boundary or a native
   event protocol whose callback is not an ordinary `std::function` parameter.
4. The API needs custom ownership, cloning, object invalidation, or result
   shaping that cannot be expressed by the generated sol2 registration.

The generator rejects `va_list` and native implementation records that do not
have a registered Lua table/value converter (for example `SceneRenderState`,
`ContactInfo2D`, and the 3D asset-loader records).  Add the converter first or
add an explicit module skip; never rely on sol2's generic userdata fallback for
these types.

When adding such code, keep it in the relevant `adapters/` or `platform/`
subdirectory and add a short comment naming the unsupported signature or
compatibility contract. Do not add a new generic `*_gen`/template layer outside
the generator.

An adapter must never overwrite a generated method.  Put every overridden
method in that module's `skip` list first; generation fails when a configured
skip no longer matches the selected C++ declaration.  This makes the JSON
policy the reviewable allowlist for the small number of compatibility adapters
that remain.

The repository-wide file allowlist is
`tools/lua-bindings/manual-adapters.json`.  Every adapter translation unit must
be listed there with a concrete reason, and `genbindings` rejects both
unlisted files and duplicate entries.  This is intentionally a file-level
guardrail: individual methods still require a matching `skip` rule when they
override generated output.  If a bridge becomes expressible by the generator,
remove its manual implementation and its allowlist entry in the same change.

## Verification

From the repository root:

```powershell
./tools/cmdline/axmol genbindings -Verify
./tools/cmdline/axmol -p wasm -O0 -t lua-tests
```

`axmol genbindings` generates all modules by default. Use `-m base` (or
`-m ax_base`) to generate one module, and `-m all` to explicitly select all
modules. `-Verify` is an optional parse-only switch; omit it when intentionally
regenerating C++ output and the API manifests. The
generator requires PowerShell 7.4; its `Add-Type` compiler
produces the C# assembly in-process, so no separately installed .NET SDK is
required. It never falls back to old Python output. By default
`genbindings.ps1` downloads the libclang artifact
from the checked-in 1kiss source configuration into the historical
`tools/bindings-generator/clang/prebuilt` directory. `AXMOL_LIBCLANG_ROOT`
selects an explicit libclang directory override. `AXMOL_CLANGSHARP_ROOT` can
point at a local ClangSharp checkout or artifact directory; otherwise the
pinned managed interop package is used.
