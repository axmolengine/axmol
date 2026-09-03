# PR title

`refactor: replace tolua++ Lua bindings with a C# generator and sol2 runtime`

## Review status

**Implementation and follow-up validation complete.**

This PR replaces Axmol's Python/tolua++ binding pipeline with a PowerShell entry point, a C# generator driven by libclang, generated C++ registration code, and an Axmol-owned sol2 runtime. The current diff also contains the callback, adapter, test, generator determinism, and lookup-path follow-up work required for merge.

The generated tree is reproducible across a clean regeneration. The remaining manual code is limited to behavior that cannot be inferred safely from an ordinary C++ signature: Lua tables, ownership, variadic factories, platform bridges, and compatibility shims.

## Completed follow-up work

- Callback state is scoped to its owning VM and owner thread. Coroutines normalize to the main VM; independent VMs can shut down independently.
- Generated callbacks use protected calls, keep callback state alive during re-entry, reject foreign-thread invocation, accept `nil` for clearing, and invalidate borrowed event userdata after dispatch.
- `ComponentLua:getScriptObject()` and NavMeshAgent table/value APIs remain explicit adapters; unsupported `void*` and `va_list` signatures are not generated.
- The old writerless Node lifecycle handler path and `HandlerType::NODE` transport are removed.
- Literal strings in extension code use `axlua::adapter::push_literal`, which forwards the compile-time length to `lua_pushlstring`; dynamic strings use length-aware paths and sceneext adapters follow underscore naming.
- AudioEngine slider events ignore uncaptured moves/releases, avoiding arithmetic on an uninitialized drag coordinate.
- The generator now suppresses redundant virtual overrides when an exported derived declaration has the same Lua-visible signature, defaults, rename, and constness as an exported base method. Same-name derived overload groups are retained so base overloads are not hidden.
- Runtime lookup uses the member-only class/base prepass before the full accessor and sol2 fallback. The Release owner-thread invalidation fast path avoids a repeated `WeakPtr` lookup while preserving stale-userdata rejection and multi-VM safety.
- Binding smoke coverage includes inherited methods, cached calls, overload groups, callback behavior, coroutine/VM ownership, and object invalidation.

## What changes for users

- Run `axmol genbindings` instead of the removed Python/tolua++ scripts.
- Binding configuration is typed JSON under `tools/lua-bindings`; generated code and API manifests are checked in for review.
- Ordinary classes, inheritance, constructors, overloads, default arguments, enums, selected fields, and `std::function` callbacks are generated.
- `ax::Object` identity and lifetime remain engine-owned. Lua peers, subclass overrides, inherited methods, and invalidation continue to work.
- The old numeric ScriptHandler transport is removed from migrated tests; native callback fields and setters are preferred.
- `ax.Controller` Lua exports and the obsolete OpenGL binding test are removed because their native APIs were not registered by the v3 runtime.

## Performance evidence

All numbers below come from the same Windows Release/O3 Binding Performance Test. They are comparison points, not promises across hardware or build modes.

| Path | Approximate result |
| --- | ---: |
| Calls removed | ~31,000 stars at 55 FPS |
| Cached methods | ~17,000 |
| Old normal `sprite:method()` | ~12,500–13,000 |
| New initial lookup path | ~9,000–9,500 |
| New path after member-only prepass and invalidation fast path | ~11,000–12,500 |

The optimization process was deliberately incremental: the accessor interleaved prepass was rejected after measuring ~6,500 stars, an unsafe Release-only lifetime-check removal (~14,500) was reverted, and closure upvalue caching was dropped because it did not help. The accepted path keeps runtime class edits visible, falls back on misses, and does not trade away lifetime or VM correctness. The benchmark scene keeps `sprite:setPosition()` and `sprite:setOpacity()` unchanged so each result remains comparable.

## Validation

- `axmol genbindings -m all -Verify`: all configured modules parse successfully with zero generator errors.
- Generated output verification is a required no-diff CI gate.
- Source-lint passes for the changed C++ and Lua binding files.
- WASM Lua tests build in the supported local workflow, including the optimized extension-heavy configuration and `lua-binding-smoke` CTest coverage.
- The checked-in generated output, manifests, adapter names, and migration docs are deterministic after regeneration.

## Merge recommendation

The implementation and the performance follow-up are complete for this PR. Keep the benchmark as a regression signal and continue platform-specific measurements after merge; no separate `docs/lua-binding-performance-followup.md` file is required.
