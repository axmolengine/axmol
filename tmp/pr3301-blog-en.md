# Axmol v3 Lua bindings: from toolchain replacement to runtime optimization

Axmol v3 replaces the historical Python and tolua++ pipeline with a single entry point:

```powershell
./tools/cmdline/axmol genbindings
```

PowerShell discovers the toolchain, a C# generator reads the C++ AST through libclang, and the generator emits reviewable C++ registration code plus JSON API manifests. sol2 handles ordinary Lua stack conversion while the Axmol runtime remains responsible for object identity, peer tables, inheritance, lifetime, and callback safety.

The boundary is intentional. Normal C++ APIs belong in generated code. Lua tables, special ownership, variadic factories, and platform bridges that cannot be inferred safely from a C++ signature stay in adapters. This removes most handwritten bindings without replacing Axmol's object model with a generic userdata wrapper.

## Generated inheritance

The generator covers 15 modules, including classes, constructors, inheritance, overloads, default arguments, enums, selected fields, and `std::function` callbacks. It also uses Clang's override relationships to suppress truly redundant virtual bindings. A derived same-name overload group is retained when it adds a new overload, so base overloads are not hidden. `Sprite` can therefore use inherited `Node` methods while C++ virtual dispatch and existing Lua call shapes remain intact.

## Runtime safety

Every callback belongs to an owning Lua VM and thread. Coroutines normalize to their main VM, independent VMs can shut down separately, and foreign-thread calls are rejected. Lua failures go through protected calls and return safe native results. When a native object dies, userdata in every VM is invalidated, while repeated pushes preserve object identity.

The old Node handler transport, Python generator, tolua++ runtime, and large `auto/` tree are gone. Literal strings in adapters use their compile-time length with `lua_pushlstring`; dynamic strings use length-aware paths as well. These changes preserve the Lua API while removing avoidable scans.

## How the performance work was measured

We used the same Windows Release/O3 Binding Performance Test for each change. The old normal `sprite:method()` path reached roughly 12,500–13,000 stars, while the new path initially reached about 9,000–9,500. Cached methods reached about 17,000, showing that the main gap was ordinary method lookup rather than the C++ wrappers themselves.

Each iteration changed one measurable factor and required both a benchmark result and functional acceptance. An accessor-interleaved prepass dropped the result to about 6,500 and was rejected. Removing lifetime checks only in Release reached about 14,500 but weakened safety, so it was reverted; closure upvalue caching showed no gain. The accepted path performs a member-only prepass for the class and registered bases, falls back to the full accessor/sol2 path on misses, and uses an owner-thread invalidation fast path. It reaches about 11,000–12,500 while preserving dynamic class edits, expired-userdata rejection, and multi-VM correctness.

The lookup path and virtual override deduplication work together: inherited methods can use the base fast binding, while derived methods that introduce a same-name overload remain fully available. The benchmark keeps `sprite:setPosition()` and `sprite:setOpacity()` unchanged so old and new results stay comparable.

## What projects need to do

Most Lua source code does not change. Projects should switch generation to `axmol genbindings` and place non-inferable table and ownership APIs in adapters. The Lua exports for `ax.Controller` and the obsolete OpenGL test were removed because they were not part of the v3 registered runtime API; the migration guide records these boundaries and compatibility rules.

The result is a binding tree that can be regenerated and reviewed, with performance work that can proceed without sacrificing lifetime or VM safety. Star counts vary by platform and build mode, so the benchmark remains a regression signal rather than a cross-device promise.
