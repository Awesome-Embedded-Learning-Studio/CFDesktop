---
title: libcommon.sh
description: libcommon.sh 的详细文档
---

# lib_common.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/lib/bash/lib_common.sh
```bash

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `log` | 统一日志输出函数，支持多级别和颜色 |
| `log_info` | 输出信息级别日志（青色） |
| `log_success` | 输出成功级别日志（绿色） |
| `log_warn` | 输出警告级别日志（黄色） |
| `log_error` | 输出错误级别日志（红色） |
| `log_cyan` | 输出青色文本 |
| `log_separator` | 打印分隔线 |
| `log_debug` | 调试输出（仅在 DEBUG=true 时显示） |
| `log_progress` | 显示进度信息（当前数/总数/百分比） |

### 颜色常量

| 常量名 | 值 | 说明 |
|--------|-----|------|
| `RED` | `\033[0;31m` | 红色 |
| `GREEN` | `\033[0;32m` | 绿色 |
| `YELLOW` | `\033[0;33m` | 黄色 |
| `CYAN` | `\033[0;36m` | 青色 |
| `GRAY` | `\033[0;90m` | 灰色 |
| `NC` | `\033[0m` | 无颜色（重置） |

---

## 函数详细说明

### log(message, level)

统一的日志输出函数，自动添加时间戳和颜色。

**参数：**
- `$1` - 消息内容
- `$2` - 日志级别（INFO, SUCCESS, WARNING, ERROR），默认为 INFO

**示例：**
```bash
log "构建开始" "INFO"
log "操作成功" "SUCCESS"
log "警告信息" "WARNING"
log "发生错误" "ERROR"
```text

### log_info(message)

输出信息级别日志。

**参数：**
- `$1` - 消息内容

**示例：**
```bash
log_info "正在处理文件..."
```text

### log_success(message)

输出成功级别日志。

**参数：**
- `$1` - 消息内容

**示例：**
```bash
log_success "构建完成！"
```text

### log_warn(message)

输出警告级别日志。

**参数：**
- `$1` - 消息内容

**示例：**
```bash
log_warn "配置文件不存在，使用默认值"
```text

### log_error(message)

输出错误级别日志。

**参数：**
- `$1` - 消息内容

**示例：**
```bash
log_error "构建失败，请检查日志"
```text

### log_cyan(message)

输出青色文本。

**参数：**
- `$*` - 消息内容（支持多个参数）

**示例：**
```bash
log_cyan "这是重要提示"
```text

### log_separator(char, width)

打印分隔线。

**参数：**
- `$1` - 分隔字符（可选，默认 `=`）
- `$2` - 宽度（可选，默认 40）

**示例：**
```bash
log_separator              # 输出 40 个 =
log_separator "-" 60       # 输出 60 个 -
```text

### log_debug(message)

调试输出，仅在 `DEBUG=true` 环境变量设置时显示。

**参数：**
- `$1` - 消息内容

**示例：**
```bash
DEBUG=true log_debug "调试信息"
```text

### log_progress(current, total, message)

显示进度信息，包含当前数、总数和百分比。

**参数：**
- `$1` - 当前数量
- `$2` - 总数量
- `$3` - 消息描述（可选，默认 "Processing"）

**示例：**
```bash
log_progress 5 10 "处理文件中"
# 输出: [5/10] (50%) 处理文件中
```yaml

---

## 使用示例

```bash
#!/bin/bash
source "scripts/lib/bash/lib_common.sh"

log_separator
log_info "开始构建项目"
log_separator "-" 50

# 处理文件
total=10
for i in $(seq 1 $total); do
    log_progress $i $total "处理中"
    sleep 1
done

log_success "所有文件处理完成！"
log_separator
```text
