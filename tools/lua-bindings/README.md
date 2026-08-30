Lua binding module configuration
================================

The active entry point is tools/cmdline/plugins/genbindings.ps1. The JSON
files in this directory define module headers, namespaces, skip rules, rename
rules, and conditional compilation expressions for the PowerShell/C#
generator. No Python or INI parser is involved. The entry point accepts only
the typed JSON arrays and objects described by `schema.json`; legacy scalar
encodings are intentionally rejected.

Configurations use arrays and objects instead of encoded whitespace or
comma-separated strings:

```json
{
  "headers": ["%(axdir)s/axmol/2d/Node.h"],
  "clang_flags": ["-DAX_ENABLE_3D=1"],
  "classes": ["Node", "Sprite"],
  "skip": [
    { "class": "Node", "methods": ["setUserObject", "getUserObject"] }
  ],
  "fields": [
    { "class": "AudioProfile", "names": ["name", "maxInstances", "minDelay"] }
  ],
  "rename_functions": [
    { "class": "AudioEngine", "method": "^end$", "luaName": "endToLua" }
  ],
  "rename_classes": [
    { "native": "Node", "luaName": "Node" }
  ]
}
```

`fields` is an opt-in whitelist for mutable public data members. It is never
inferred from all public fields, which keeps ownership and ABI-sensitive data
private unless a module explicitly adopts it. `abstract_classes`,
`no_constructors`, `classes_have_no_parents`, and `base_classes_to_skip` also
accept arrays.
The default generation mode writes a complete generated API manifest for
each module under `generated/manifests`; it is an audit and review snapshot,
not a build input. The compatibility schema is in `schema.json`. The boundary
between generated output and justified specialized code is documented in
`manual-policy.md`.

## From a fresh Axmol checkout

1. Install PowerShell 7.4+. The entry point compiles the generator assembly
   in-process with PowerShell `Add-Type`; no separate .NET SDK is required.
   CI may instead provide the prebuilt `Axmol.LuaBindings.Generator.dll`
   together with its adjacent `ClangSharp.Interop.dll` for offline use.
2. The first generator run downloads the platform `libclang` archive using the
   repository's 1kiss configuration, caches it under `cache/devtools`, and
   copies the library to `tools/bindings-generator/clang/prebuilt`. The
   generator uses that native library directly; it does not install ClangSharp
   native shims. Pass `-LibClangRoot` only when an explicit override is needed.
3. Verify the headers without changing checked-in output:

   ```powershell
   pwsh ./tools/cmdline/axmol.ps1 genbindings -Verify
   ```

4. Regenerate all checked-in C++ bindings when the C++ headers or JSON policy
   changes:

   ```powershell
   pwsh ./tools/cmdline/axmol.ps1 genbindings
   ```

The generated `*_gen.h/.cpp` files are written to
`extensions/scripting/lua-bindings/generated`; complete API manifests are
written to its `manifests/` subdirectory.
The entry point reuses `ANDROID_NDK` or `-NdkRoot`, and otherwise invokes
`setup.ps1 -p android` before collecting against the Android target headers.
