# /next-step — 开发指引

根据项目阶段文档和当前进度，推荐下一步开发任务。

## 触发方式

- `/next-step` — 推荐下一步开发任务
- 用户说"下一步开发什么"

## 工作流程

### Step 1: 评估当前状态

1. 读取 `document/status/current.md`（项目进度唯一事实来源）获取当前进度与下一步路线
2. 识别哪些 Phase / 模块未完成

### Step 2: 映射设计文档

对每个未完成阶段，读取对应的设计文档获取具体任务列表：

| Phase | 设计文档 |
|-------|----------|
| 1 | `document/design_stage/01_phase1_hardware_probe.md` |
| 2 | `document/design_stage/02_phase2_base_library.md` |
| 3 | `document/design_stage/03_phase3_input_layer.md` |
| 6 | `document/design_stage/04_phase6_simulator.md` |
| 8 | `document/design_stage/05_phase8_testing.md` |

### Step 3: 优先排序

按以下顺序推荐：

1. **阻塞依赖项** — 其他模块依赖此任务才能推进
2. **接近完成的模块** (90%+) — 收尾效率最高
3. **当前活跃阶段** — 保持开发连续性
4. **测试覆盖缺口** — 为已完成的代码补充测试

### Step 4: 输出格式 (中文)

对每个推荐任务：

```markdown
## 推荐任务 N: <任务名称>

**优先级**: 高/中/低
**所属阶段**: Phase X — <阶段名>

### 目标文件
- `path/to/file.h` — 修改说明
- `path/to/file.cpp` — 修改说明
- `path/to/new_file.h` — 新建说明

### 改动要点
1. ...
2. ...

### 风险评估
- **风险等级**: 低/中/高
- **风险说明**: ...
- **缓解措施**: ...

### 依赖关系
- 前置依赖: ...
- 后续可解锁: ...

### 参考文档
- `document/design_stage/XX_phaseX.md` — 具体章节
```

## 规则

- 推荐前验证目标文件是否存在（避免推荐已完成的任务）
- 不推荐违反层级依赖规则的任务
- 每次推荐 2-4 个任务，按优先级排序
- 如果有近完成的模块，优先推荐收尾
