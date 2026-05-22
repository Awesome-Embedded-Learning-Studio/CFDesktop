---
title: libconfig.sh
description: libconfig.sh 的详细文档
---

# lib_config.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/lib/bash/lib_config.sh
```bash

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `get_ini_config` | 读取 INI 配置文件，输出可 eval 的变量赋值语句 |
| `get_ini_value` | 获取特定配置项的值 |
| `has_ini_value` | 检查配置项是否存在 |
| `get_default_config_file` | 获取默认配置文件路径 |

---

## 函数详细说明

### get_ini_config(filepath)

读取 INI 配置文件并生成可通过 eval 使用的配置变量。

**支持的格式：**
- 节格式：`[section_name]`
- 键值对格式：`key=value`
- 注释：以 `#` 或 `;` 开头的行
- 值支持引号包裹，会自动去除

**变量命名规则：**
生成的变量名为 `config_<section>_<key>`

**参数：**
- `$1` - INI 文件路径

**输出：**
- 可通过 eval 使用的配置变量赋值语句

**返回值：**
- `0` - 成功
- `1` - 失败（文件不存在）

**示例：**

假设 `config.ini` 内容为：
```ini
[cmake]
generator = Ninja
build_type = Release

[paths]
build_dir = build
output_dir = out
```text

使用方式：
```bash
eval "$(get_ini_config config.ini)"
echo "$config_cmake_generator"    # 输出: Ninja
echo "$config_paths_build_dir"    # 输出: build
```text

### get_ini_value(filepath, section, key)

获取特定配置项的值。

**参数：**
- `$1` - INI 文件路径
- `$2` - Section 名称
- `$3` - Key 名称

**输出：**
- 配置值，如果未找到则输出空字符串

**示例：**
```bash
value=$(get_ini_value "config.ini" "cmake" "generator")
echo "$value"    # 输出: Ninja
```text

### has_ini_value(filepath, section, key)

检查配置项是否存在。

**参数：**
- `$1` - INI 文件路径
- `$2` - Section 名称
- `$3` - Key 名称

**返回值：**
- `0` - 存在（值非空）
- `1` - 不存在（值为空）

**示例：**
```bash
if has_ini_value "config.ini" "cmake" "generator"; then
    log_info "Generator 已配置"
fi
```bash

### get_default_config_file(mode)

获取默认配置文件的绝对路径。

**参数：**
- `$1` - 配置模式（develop, deploy, ci），默认为 develop

**输出：**
- 配置文件的绝对路径

**模式映射：**
| 模式 | 配置文件 |
|------|----------|
| `deploy` | `build_deploy_config.ini` |
| `ci` | `build_ci_config.ini` |
| `develop` 或其他 | `build_develop_config.ini` |

**示例：**
```bash
config_file=$(get_default_config_file "develop")
eval "$(get_ini_config "$config_file")"
```yaml

---

## 使用示例

### 完整示例

```bash
#!/bin/bash
source "scripts/lib/bash/lib_config.sh"

# 获取配置文件路径
CONFIG_FILE=$(get_default_config_file "develop")

# 读取整个配置
eval "$(get_ini_config "$CONFIG_FILE")"
echo "Generator: $config_cmake_generator"
echo "Build Type: $config_cmake_build_type"

# 获取单个值
GENERATOR=$(get_ini_value "$CONFIG_FILE" "cmake" "generator")
echo "Generator: $GENERATOR"

# 检查配置是否存在
if has_ini_value "$CONFIG_FILE" "cmake" "generator"; then
    echo "使用配置的生成器: $GENERATOR"
else
    echo "使用默认生成器"
fi
```text

### 条件配置示例

```bash
#!/bin/bash
source "scripts/lib/bash/lib_config.sh"

# 根据模式选择配置
MODE="${1:-develop}"
CONFIG_FILE=$(get_default_config_file "$MODE")

eval "$(get_ini_config "$CONFIG_FILE")"

# 使用配置
BUILD_DIR="$config_paths_build_dir"
BUILD_TYPE="$config_cmake_build_type"

log_info "配置模式: $MODE"
log_info "构建目录: $BUILD_DIR"
log_info "构建类型: $BUILD_TYPE"
```text

### INI 配置文件示例

```ini
# CMake 配置
[cmake]
generator = Ninja
build_type = Release

# 路径配置
[paths]
build_dir = build
output_dir = out
install_dir = /usr/local

# 构建选项
[build]
parallel = true
jobs = 8
verbose = false
```text
