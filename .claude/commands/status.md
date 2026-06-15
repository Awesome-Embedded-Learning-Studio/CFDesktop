# /status — 项目现状快照

合成并输出项目当前状态的精炼摘要，让任何人或新 AI 会话 **30 秒看懂**「现在到哪了、下一步干什么」。

## 触发方式

- `/status` — 输出项目现状快照
- 用户说「项目现状 / 现在什么进度 / 快速了解项目 / onboarding / 帮我上手」

## 工作流程（全部自动合成，禁止臆造）

### Step 1: 读取进度真相源

1. 读取 `document/status/current.md`（项目进度的唯一事实来源）
2. 抽取：当前阶段、三层进度状态、Phase 状态、下一步路线

### Step 2: 采集近期变更

```bash
git log -10 --oneline           # 最近 10 次提交
git rev-parse --abbrev-ref HEAD # 当前分支
```

### Step 3: 验证架构健康度

```bash
grep -r '#include.*\(ui/\|desktop/\)' base/   # 期望 0（base 不可向上依赖）
grep -r '#include.*desktop/' ui/              # 期望 0（ui 不可依赖 desktop）
```

- 任一返回非 0 = 三层依赖违规，必须在输出中标注 ⚠️ 并列出违规文件。

### Step 4: 合成输出

按下方「输出格式」拼装，保持一屏可读。

## 输出格式（中文）

```markdown
# CFDesktop 现状快照

**版本 / 分支**: 0.19.0 / <branch>
**校准**: <current.md 顶部校准日期>

## 当前阶段
<取自 current.md「当前阶段」>

## 进度状态（定性）
### 三层架构
<base / ui / desktop，取自 current.md>
### Phase
<取自 current.md>

## 下一步路线
<取自 current.md「下一步路线」，列出 MS2-MS5 与首要任务>

## 最近变更（最近 10 提交）
<git log oneline 列表>

## 架构健康度
- 三层依赖: ✅ 0 违规  /  ❌ N 处违规（列出文件）

## 新人入口
<指向 README / CLAUDE.md / current.md 下一步路线>
```

## 规则

- **数据来源强制**：所有状态必须取自 `document/status/current.md` 或实时 `git` / `grep`，**禁止凭记忆臆造**，**禁止编造百分比**（current.md 本身用定性状态）。
- **断链报警**：若 `current.md` 导航表指向的文件不存在，必须在输出中以 ⚠️ 显式报警（这正是本命令要守护的「进度漂移 / 指针断裂」问题）。
- **精炼优先**：一屏可读，不展开 `CLAUDE.md` 的构建命令细节，指向即可。
- **只读操作**：本命令只读取与合成，不修改任何文件。
