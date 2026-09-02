# Axmol Lua Binding Performance Follow-up

## Current evidence

All figures below come from the same Windows Release/O3 Binding Performance Test.

| Path | Stars at about 55 FPS |
| --- | ---: |
| Calls removed | about 31,000 |
| Cached methods, old and new bindings | about 17,000 |
| Old binding, normal `sprite:method()` | about 12,500-13,000 |
| New binding before lookup work | about 9,000-9,500 |
| New binding after current lookup work | about 11,000 |

The cached result shows that the generated native wrappers are already close to the old wrappers in this benchmark. The remaining material gap is primarily normal userdata method resolution.

Reaching 13,000 from 11,000 requires roughly a 15% reduction in total per-Sprite cost, or about a 40-45% reduction in the remaining method-lookup overhead. Small scalar-conversion changes cannot close that gap by themselves.

## Revised target

- Primary target: stable 12,500-13,000 at about 55 FPS, with existing Lua behavior preserved.
- Stretch target: exceed 13,000 when the result is repeatable and does not weaken runtime guarantees.
- Do not use 14,000-16,000 as a merge requirement. That range is unlikely while retaining peer behavior, dynamic class modification, accessors, object identity, lifetime validation, coroutine behavior, and multi-VM correctness.
- Cached performance should remain near 17,000.

## Working rules

1. Change one independently measurable behavior at a time.
2. Stop after each step for Release/O3 benchmark and functional acceptance.
3. Keep the benchmark's ordinary `sprite:setPosition()` and `sprite:setOpacity()` calls unchanged.
4. Preserve a compatibility fallback whenever a fast path cannot resolve a member.
5. Do not permanently cache a resolved function unless runtime class writes have a complete invalidation mechanism.
6. Do not trade cross-thread or multi-VM lifetime correctness for benchmark results.
7. Revert changes that do not improve this benchmark or correctness.

## Recommended coding model

- Use GPT-5.6 Sol with High reasoning as the default for the remaining work. It provides enough depth for the step-by-step implementation while preserving the five-hour working budget.
- Temporarily switch to XHigh only for a lookup-precedence design decision or a compatibility regression that remains unexplained after focused inspection and profiling.
- Reserve Max reasoning for an exceptional final architecture review; it is not part of the normal execution plan.

The lookup path combines Lua metatable precedence, sol2 compatibility, generated inheritance, object lifetime, and benchmark-sensitive stack operations. Keep each High-reasoning task narrowly scoped, benchmark it immediately, and avoid spending a large reasoning budget on several speculative optimizations at once.

## Step 0 - Remove the irrelevant pending boolean change

Do not retain the uncommitted Release `fast_boolean` type-check removal as a performance change for this benchmark. The benchmark does not call a boolean fast binding, so the change cannot explain an improvement and unnecessarily relaxes Release argument behavior.

## Step 1 - Direct public-method prepass

An initial public-class/base traversal interleaved with accessor lookup was tested and reverted. Performance fell from about 11,000 to about 6,500 stars.

The tested loop interleaved each raw member miss with `lookup_class_accessor`. The accessor lookup uses `lua_getfield` for `.get`, which can invoke the class table's `__index`. That caused the proposed fast traversal to overlap with the existing full traversal instead of replacing its cost.

Do not restore that implementation. In particular, do not interleave accessor lookup with a native-base prepass.

A corrected member-only prepass is now implemented:

```text
existing peer and lifetime validation
    -> raw public class member lookup
    -> raw registered-base member lookup
    -> existing complete class/accessor/sol2 lookup on a miss
```

The prepass inspects concrete method entries only. It does not query `.get`, invoke a new metatable fallback, or cache resolved closures. Runtime modifications to public class tables remain visible, while compatibility behavior remains in the unchanged fallback.

Release/O3 result: normal Binding Performance Test improved from about 11,000 to about 12,500 stars at approximately 55 FPS. This reaches the primary target's lower bound.

## Step 2 - Measure lifetime validation separately

`class_index` currently checks the userdata environment and the WeakPtr lifetime registry on every method lookup. The old binding does not perform an equivalent WeakPtr lookup.

The temporary Release-only A/B experiment improved the benchmark from about 12,500 to about 14,500 stars. The unsafe variant was reverted and must not be committed.

The lookup is O(1), but two method calls per Sprite at 55 FPS produce roughly 1.37 million lifetime checks per second at 12,500 stars. The registry-table lookup, userdata-key lookup, Lua stack traffic, and WeakPtr access therefore have a material aggregate cost.

Caching the lifetime table in a `class_index` closure upvalue was tested. The result remained about 12,500 stars, so the registry-table lookup is not a material part of the remaining cost. The closure change was reverted.

The final implementation uses owner-thread invalidation as the Release fast path. Lua VM access and userdata invalidation are confined to each VM's owner thread; native objects may still be shared when their own synchronization contract is honored. Destruction of an object exposed to Lua must be marshalled to the owner thread, where canonical userdata is synchronously marked invalid and its native pointer is cleared. Release therefore skips the per-access WeakPtr lookup, while Debug retains the full diagnostic lookup. No foreign-thread Lua-state fallback is maintained because Lua state access and the binding's invalidation bookkeeping are not thread-safe.

The fast object extractor still rejects cleared userdata before entering generated native code, so cached calls cannot dereference a stale handle. Multi-VM behavior remains valid when each VM is used from its owning thread. Benchmark and lifecycle tests must pass before retaining the change.

Any further design must preserve:

- owner-thread destruction invalidation;
- multiple Lua VMs;
- address reuse protection;
- borrowed userdata invalidation;
- canonical userdata identity.

The generator's virtual override pass compares the C++ signature (including
parameter types and constness) but intentionally ignores default arguments.
When a derived class adds another overload with the same Lua name, the entire
Lua overload group is retained so the inherited overloads remain reachable.

Possible designs must be evaluated against Lua 5.1/LuaJIT user-environment constraints. Merely deleting the WeakPtr lookup in Release is not an acceptable final implementation.

## Step 3 - Re-evaluate native wrapper cost

After class lookup is optimized, compare these paths independently:

```text
normal fast method
cached fast method
normal generic generated method
cached generic generated method
```

Keep the current targeted `fast_bindings` list unless this comparison shows a substantial wrapper-level difference. Do not broadly convert generated bindings to direct Lua C wrappers in this refactor.

Scalar conversion changes should be retained only when they improve a relevant fast binding and preserve the intended Release argument semantics.

## Step 4 - Correctness and generator completion

Performance work is complete only after validating:

- inherited fast methods;
- dynamic Lua class replacement;
- peer fields and peer method overrides;
- getter and setter accessors;
- expired and address-reused userdata;
- coroutine access;
- multi-VM behavior;
- C++ virtual dispatch;
- same-name derived overloads;
- virtual override deduplication across deep inheritance chains;
- deterministic generated output.

Generator architecture, override-deduplication coverage, and deterministic generation remain required for finalization, but they are separate from the method-lookup performance target.

## Stop conditions

Stop performance work when either condition is met:

1. Normal calls are consistently within the old binding's 12,500-13,000 range with correctness tests passing.
2. The remaining measured gap comes from required lifetime/dynamic semantics and closing it would require weakening behavior or a substantially larger runtime redesign.

Exceeding the old binding is a stretch result, not a correctness or merge criterion.
