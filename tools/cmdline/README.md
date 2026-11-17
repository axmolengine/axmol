# axmol-cmdline


## Vision of axmol-cmdline


A command line tool that lets you create, run, publish, debug, etc… your game. It is the swiss-army knife for axmol.

Examples:

```
# starts a new project called "My Game" for multi-platform

$ axmol new MyGame -l cpp -p dev.axmol.mygame

$ cd MyGame

# Will deploy the project to device and run it
$ axmol run -p android


```

## Machine-readable build results

After a successful CMake or Android Gradle build, the CLI prints one line with the
literal prefix `AXMOL_BUILD_RESULT=` followed by compact JSON. This also applies to
the build stage of `deploy` and `run`. For example:

```text
AXMOL_BUILD_RESULT={"version":1,"buildDir":"D:\\game\\build","config":"Release","targetOS":"win32","targetCPU":"x64","artifacts":[{"target":"MyGame","path":"D:\\game\\build\\bin\\MyGame\\Release\\MyGame.exe"}]}
```

Consumers such as axmol-hub should match the prefix at the start of the line,
parse the remainder as JSON, and also check the process exit code. Do not parse
human-readable success messages. A build result does not indicate that a later
deploy or launch operation succeeded.

| Field | Meaning |
| --- | --- |
| `version` | Protocol version, currently `1`. |
| `buildDir` | Absolute native build directory, or `null` for Gradle builds. |
| `config` | Requested build configuration after resolving CLI options. |
| `targetOS`, `targetCPU` | Resolved target platform and architecture. |
| `artifacts` | Array of objects with a `target` name and an existing absolute `path`. Apple app paths refer to `.app` directories. |

Artifact paths are defined once in `axmol.ps1` by `axmol_artifact_paths`, shared
by build-result reporting, `run` and `deploy`. Direct invocation of the internal
`plugins/build.ps1` script does not print this protocol. Standard layouts:

- Windows: `bin/<target>/<config>/<target>.exe` for Visual Studio and
  Ninja Multi-Config; `bin/<target>/<target>.exe` for single-config Ninja.
- Linux: `bin/<target>/<target>` (with a configuration subdirectory for
  multi-config generators).
- macOS/iOS/tvOS: `build/<config>/<target>.app`.
- wasm/wasm64: `bin/<target>/<target>.html` (with a configuration subdirectory
  for multi-config generators).
- Android Gradle APK: `proj.android/app/build/outputs/apk/<variant>/`, using
  `output-metadata.json` to locate all output APKs.
- Android Gradle AAB: `.aab` files directly in
  `proj.android/app/build/outputs/bundle/<variant>/`.

The Android variant is `debug` only for the Debug configuration; otherwise it is
`release`, matching the actual Gradle task. Multiple CMake targets and multiple
Android packages can produce multiple artifact entries.

Only existing artifacts in these standard layouts are reported. Custom output
names/directories, WinRT packages, native-only Android builds and library or
aggregate targets such as `axmol-sdk` are not resolved. An empty `artifacts`
array means the build succeeded but no supported artifact was located, not that
the build failed. No recursive artifact search or CMake File API query is used.
Lookup errors produce a warning without changing the original build exit code.

No result line is printed for failed builds, configuration-only invocations or
toolchain-only preparation.

# Devel Info

## Internals

`axmol.ps1` is an script whose only responsability is to call its plugins.
`axmol.bat` will invoke `axmol.ps1` on windows
`axmol` will invoke `axmol.ps1` on Mac OS X and linux

To get a list of all the registered plugins:

```
$ axmol -h
```

To run the "new" plugin:

```
$ axmol new
``` 
