# Axmol Engine AI Coding Instructions

This document provides system-level rules and context for AI coding assistants, LLMs, and agentic workflows (e.g., Cursor, Copilot, Claude, Windsurf, Aider) interacting with the Axmol Engine codebase.

> **Scope:** Unless stated otherwise, all instructions in this document apply to the stable **`release/2.x`** branch (current LTS branch).
> Sections specific to the **`dev`** development branch are explicitly marked with the **[v3]** tag.

---

## 1. Engine Identity & Namespaces (CRITICAL)
- **Namespace:** Always use the `ax::` namespace for all core engine types (e.g., `ax::Node`, `ax::Sprite`, `ax::Vec2`, `ax::Scene`, `ax::Director`).
- **Legacy Naming:** Do not use the legacy `cocos2d::` namespace or `CC_*` prefixes in new code. Axmol provides compatibility aliases (`namespace cocos2d = ax;`, `CC_*` macros), so existing code will compile, but all new code must use `ax::` and `AX_*` exclusively.

---

## 2. Modern C++ Standard & Architecture

### release/2.x (stable)
- **Standard:** Implement features using clean **C++17/C++20** standards. Leverage `auto`, `nullptr`, `override`, `constexpr`, and structured bindings where appropriate.

### [v3] dev
- **Standard:** The v3 branch targets **C++23**. Use C++23 features freely (e.g., `std::expected`, `if consteval`, deducing `this`).

### Two-Stage Initialization (both branches)
- Implement a static `create()` method combined with the `CREATE_FUNC(ClassName)` macro.
- Implement a virtual `bool init()` method that returns `true` on success and explicitly calls its parent's init method (e.g., `Node::init()`).
- **No Smart Pointers for Nodes (CRITICAL):** NEVER wrap `ax::Ref` or scene graph nodes (e.g., `Sprite`, `Scene`) into `std::shared_ptr` or `std::unique_ptr`. Use `Class::create()` and let the engine's reference counting handle lifetime.

### Callbacks (both branches)
- Use `AX_CALLBACK_*` macros for lambda assignments, scheduling, and event handling.

---

## 3. Memory Management & Ownership
- **Autorelease Pool:** Objects instantiated via `::create()` are automatically managed by Axmol's reference-counting autorelease pool. Do not call `delete` on them.
- **Scene Graph & Delayed Attachment:** Adding a node via `addChild()` increments its reference count; removing it via `removeChild()` or `removeFromParent()` decrements it. If an autoreleased object is created but not immediately added to the scene graph, it will be destroyed when the autorelease pool is drained (typically at the end of the current frame). Always call `obj->retain()` when storing a node as a class member outside the scene graph, and balance it with `obj->release()` in the destructor.
- **Raw Ownership:** For memory blocks outside the scene graph, prefer modern standard smart pointers (`std::unique_ptr`, `std::shared_ptr`) or Axmol-specific containers (`ax::Vector<T>`, `ax::Map<K, V>`). Use `AX_SAFE_DELETE` for raw pointers when manual cleanup is necessary.

---

## 4. Guarded and High-Risk Areas

### Third-Party Code (both branches)
- Do not modify anything inside the `3rdparty/` directory unless explicitly instructed.

### Generated Templates (both branches)
- Do not alter platform template structures or boilerplate files in `templates/` without explicit isolation.

### Platform Glue (both branches)
- Use `AX_TARGET_PLATFORM` preprocessor guards for platform-specific C++, Objective-C++ (`.mm`), or Android JNI implementations. Keep JNI signatures strictly synchronized.

### Public APIs (both branches)
- Maintain strict backward compatibility when modifying public headers in `extensions/` or `core/`.
- **[v3]** The engine core directory is `axmol/` instead of `core/`. Update include paths accordingly.

---

## 5. Verification & Builds

### Requirements
- Use CMake out-of-tree builds for testing: `cmake -S . -B build`

### Build Commands (Axmol CLI)
If the Axmol CLI environment is active, use the following entry points:

| Platform | Command |
|----------|---------|
| Linux Desktop | `axmol build -p linux` |
| Windows | `axmol build -p win32` |
| Android | `axmol build -p android` |

### [v3] Additional Build Notes
- The v3 branch introduces experimental **Direct3D 11**, **Direct3D 12** and **Vulkan** RHI backends. To test them, refer to the [About-RHI-in-axmol-v3](https://github.com/axmolengine/axmol/wiki/About-RHI-in-axmol-v3) wiki page.
- arm64 builds for Linux and Windows are available starting from v3.
