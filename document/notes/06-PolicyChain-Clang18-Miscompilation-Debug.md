# PolicyChain Clang 18 Miscompilation — Debug Progress

## 问题描述

CI 中 Clang 18 (Ubuntu 24.04 Docker, Release/-O3) 有 4 个 `policy_chain_test` 测试失败，
GCC 和 Clang 22 均通过。

**失败的测试：**
- `PolicyChainTest.Fallback_SecondPolicy` — `execute(-3)` 返回 `-6`（期望 `3`）
- `PolicyChainTest.Fallback_MultiplePolicies` — `execute(-5)` 返回 `-10`（期望 `0`）
- `PolicyChainTest.MakePolicyChain_Basic` — `execute(-3)` 返回 `-6`（期望 `3`）
- `PolicyChainTest.Builder_Basic` — `execute(-3)` 返回 `-6`（期望 `3`）

**共同模式：** 第一个 policy 的 `if (x > 0)` 条件被 Clang 18 优化掉，
`x * 2` 被无条件执行，`std::nullopt` 返回路径被消除。

## 复现命令（Docker）

```bash
# 构建
docker run --rm -v "$PWD":/project -w /project \
  ghcr.io/awesome-embedded-learning-studio/cfdesktop-build-env:latest \
  bash -lc 'ccache -C; bash scripts/build_helpers/linux_fast_develop_build.sh ci -c build_ci_clang_config.ini'

# 跑单个失败测试
docker run --rm -v "$PWD":/project -w /project \
  ghcr.io/awesome-embedded-learning-studio/cfdesktop-build-env:latest \
  bash -lc './out/build_ci_clang/test/bin/policy_chain_test --gtest_filter="PolicyChainTest.Fallback_SecondPolicy"'

# 跑全部 policy_chain 测试
docker run --rm -v "$PWD":/project -w /project \
  ghcr.io/awesome-embedded-learning-studio/cfdesktop-build-env:latest \
  bash -lc 'ctest --test-dir out/build_ci_clang/test -R policy_chain --output-on-failure'
```

## 关键文件

- **实现：** `base/include/base/policy_chain/policy_chain.hpp`
- **测试：** `test/base/policy_chain/policy_chain_test.cpp`

## 已有的 Workaround（不够）

文件中已有 `invoke_policy` barrier（第 30-41 行）：

```cpp
#if defined(__clang__) && (__clang_major__ < 19)
#    define CF_POLICY_CHAIN_INVOKE_BARRIER __attribute__((noinline, optnone))
#else
#    define CF_POLICY_CHAIN_INVOKE_BARRIER
#endif

template <typename Policy, typename... CallArgs>
[[nodiscard]] CF_POLICY_CHAIN_INVOKE_BARRIER auto
invoke_policy(Policy const& policy, CallArgs&&... args)
    -> decltype(policy(std::forward<CallArgs>(args)...)) {
    return policy(std::forward<CallArgs>(args)...);
}
```

这个 barrier 无效——问题不在 `execute()` 的 `has_value()` 检查，而在更深层。

## 调试发现（通过 fprintf 日志定位）

### 实验 1：在 `execute()` 和 `PolicyModel::invoke()` 都加 fprintf → PASS
### 实验 2：只在 `execute()` 加 fprintf，`PolicyModel::invoke()` 不加 → FAIL

**关键日志（失败时）：**
```
execute(-3):
  [DEBUG] invoke_policy returned, has_value=1    ← 只调了一次！
  [DEBUG] returning value: -6
```

**正确行为（加 fprintf 后）：**
```
execute(-3):
  [DEBUG] PolicyModel::invoke(), has_value=0, val=0   ← 第一个 policy 返回 nullopt
  [DEBUG] PolicyModel::invoke(), has_value=1, val=3   ← 第二个 policy 返回 3
  [DEBUG] returning value: 3
```

**结论：** 第一个 policy 的 lambda 被错误编译——`if (x > 0)` 条件被消除。
fprintf 在 `PolicyModel::invoke()` 中作为副作用阻止了该错误优化。

## 已尝试但无效的修复

1. `__attribute__((noinline, optnone))` on `PolicyModel::invoke()` — 编译错误（与 `[[nodiscard]]` 冲突）
2. `[[clang::noinline]]` on `PolicyModel::invoke()` — 编译通过但测试仍失败
3. `[[clang::optnone]]` on `PolicyModel::invoke()` — 编译通过但测试仍失败
4. `[[gnu::noinline, gnu::optnone]]` — Clang 不认识 `[[gnu::optnone]]`，被忽略

## 调用链分析

```
PolicyChain::execute(args)
  → policy_chain_detail::invoke_policy(policy, args)   [noinline, optnone] ← 无效
    → PolicyEntry::operator()(args)
      → PolicyConcept::invoke(args)                     [virtual dispatch]
        → PolicyModel<Lambda>::invoke(args)              ← lambda 在这里被调用
          → std::invoke(policy_, args...)                ← lambda body 被错误优化
```

**根因：** Clang 18 `-O3` 在编译 `PolicyModel<Lambda>::invoke()` 时，
将 lambda body 内联后错误优化掉了条件分支。`invoke_policy` 的 barrier 在调用链外层，
无法保护 `invoke()` 内部的 lambda 编译。

## 根因分析（2026-05-23 反汇编确认）

反汇编显示，lambda 的条件分支并不是简单被删除，而是 Clang 18 在 `-O3`
下把 `std::optional<int>` 的小对象返回值打包进寄存器时**污染了 engaged byte**。

失败用例的第一条 policy 生成了类似逻辑：

```asm
xor    %eax,%eax
test   %edi,%edi
setg   %al
shl    $0x20,%rax        ; has_value 放到 bit 32
mov    %edi,%ecx
lea    (%rax,%rcx,2),%rax
```

当 `x == -3` 时，`x * 2` 的 64-bit `lea` 结果为 `0x00000001fffffffa`，
bit 32 被算术进位置 1，导致 disengaged optional 被调用方看成
`has_value == true`，值为 `-6`。

### 试过但无效

- 把 lambda 调用包进新的 `noinline, optnone invoke_stored_policy()`：无效，因为
  lambda `operator()` 仍会作为单独 O3 函数生成坏的返回打包。
- `std::function<std::optional<int>(int)>` 最小复现：同样失败。
- `asm volatile("" : "+m"(result) : : "memory")`：无效，只是把已经污染的
  packed register 存回内存。

## 有效修复

在 `PolicyModel::invoke()` 中先接住 `std::invoke()` 的结果，再针对 Clang < 19
显式重建 nullopt 路径：

```cpp
auto result = std::invoke(policy_, args...);
#if defined(__clang__) && (__clang_major__ < 19)
if (!result.has_value()) {
    return std::nullopt;
}
#endif
return result;
```

这会让 Clang 18 保留 disengaged 分支，避免直接复用被污染的 packed register。

## 验证

- Docker/CI Clang 18：`ctest --test-dir out/build_ci_clang/test -R policy_chain --output-on-failure`
  全部通过，1/1 test passed。
- 本地 develop 构建：`./out/build_develop/test/bin/policy_chain_test`
  全部通过，29/29 tests passed。
