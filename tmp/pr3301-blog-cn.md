# Axmol v3 Lua 绑定重构：从工具链替换到运行时优化

Axmol v3 用新的 Lua 绑定系统替换了历史上的 Python + tolua++ 流程。新的入口仍然只有一个命令：

```powershell
./tools/cmdline/axmol genbindings
```

PowerShell 负责工具发现和配置，C# 生成器通过 libclang 读取 C++ AST，生成可审查的 C++ 注册代码和 JSON API manifest。sol2 负责常规 Lua stack 转换，Axmol runtime 继续负责对象身份、peer table、继承、生命周期和回调安全。

这次重构的边界很明确：普通 C++ API 交给生成器；Lua table、特殊所有权、变参 factory 和平台 bridge 等无法从签名安全推导的行为保留在 adapter。这样既减少手写绑定，也避免通用 userdata 包装破坏 Axmol 的对象模型。

## 生成代码与继承

生成器现在覆盖 15 个模块。它处理 class、构造函数、继承、重载、默认参数、enum、选定 field 和 `std::function` 回调。生成阶段还会根据 Clang 的 override 关系去掉真正重复的虚函数绑定，同时保留同名的新重载，避免隐藏基类 API。因此 `Sprite` 可以直接继承 `Node` 的 Lua 方法，Lua 端原有的调用方式和 C++ 虚派发都保持不变。

## 运行时安全

每个 callback 都绑定到所属 Lua VM 和线程。协程会归一到主 VM，多个独立 VM 可以分别关闭；跨线程调用会被拒绝，Lua 错误通过 protected call 返回安全结果。原生对象析构时，所有 VM 中的 userdata 都会失效，重复 push 仍保持对象 identity。

旧的 Node handler transport、Python generator、tolua++ runtime 和大段 `auto/` 目录已经删除。常量字符串 adapter 使用编译期长度调用 `lua_pushlstring`，动态字符串也走带长度的接口；这类优化不改变 Lua API。

## 性能优化是如何完成的

我们用同一份 Windows Release/O3 Binding Performance Test 逐步测量。旧绑定的普通 `sprite:method()` 约为 12,500–13,000 个星星，新绑定初始约为 9,000–9,500；缓存方法约 17,000，说明主要差距来自普通方法查找，而不是 C++ wrapper 本身。

优化过程每次只改变一个因素，并同时做性能和功能验收。一次把 accessor 查找插入预热路径后性能降到约 6,500，因此没有保留。只在 Release 删除生命周期检查虽然达到约 14,500，但会削弱安全性，也被撤回；closure upvalue 缓存没有收益。最终采用只针对 class member 和已注册 base member 的快速预查，未命中时回退到完整 accessor/sol2 路径，再配合 owner-thread invalidation fast path。结果回到约 11,000–12,500，同时保留动态 class 修改、过期 userdata 拒绝和多 VM 正确性。

这套查找路径与虚函数去重结合后，继承方法可以复用基类快速绑定，派生类新增的同名重载仍然完整保留。性能测试继续使用原来的 `sprite:setPosition()` 和 `sprite:setOpacity()`，方便新旧系统比较。

## 对项目的影响

大多数 Lua 代码无需修改。项目需要把生成入口切换到 `axmol genbindings`，并将无法自动生成的 table/ownership API 放入 adapter。`ax.Controller` Lua 导出和旧 OpenGL 测试已移除，因为它们不属于 v3 已注册的运行时 API；迁移文档记录了这些边界和兼容规则。

这次重构让绑定代码更容易再生、更容易审查，也让性能优化可以在保持生命周期和 VM 安全的前提下逐步进行。平台和构建模式不同，星星数量不能直接横向承诺，但 benchmark 会作为后续回归信号继续保留。
