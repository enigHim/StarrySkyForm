# StarrySkyForm

基于 LeviLamina 开发、适用于 BDS 的星空表单系统。

## 功能特性

- 表单物品
  - [x] 使用闹钟作为表单触发器
  - [x] 自定义物品名称和附魔
  - [x] 消失诅咒防止物品丢失
- 物品锁定机制
  - [x] 锁定后无法移动、丢弃、合成
  - [x] 解锁后仍无法丢弃、合成，仅解除移动限制
  - [x] 支持动态锁定/解锁
- 玩家管理
  - [x] 新玩家默认获得锁定表单
  - [x] 物品自动放置在指定槽位
- 表单系统
  - [x] JSON 配置文件定义表单
  - [x] 支持按钮图片和命令
  - [x] 表单可嵌套跳转
- 配置管理
  - [x] 主配置文件 (config.json)
  - [x] 表单配置文件 (forms/*.json)
  - [x] 热重载支持

## 安装

### 前置要求

| 前置组件 | 项目地址 | 依赖等级 |
|:---------|:--------|:--------:|
| LeviLamina | [GitHub](https://github.com/LiteLDev/LeviLamina) | 必须 |

### 安装步骤

1. 将插件文件放入服务器的 `plugins` 文件夹
2. 启动服务器，插件会自动创建默认配置文件
3. 根据需要修改配置文件
4. 重启服务器使配置生效

## 配置文件

> 配置文件均为 `json` 格式，位于 `plugins/StarrySkyForm/` 目录下。
>
> 配置文件为 `json` 格式，请勿使用记事本等不支持 `json` 格式的文本编辑器进行编辑，否则会导致配置文件损坏。

### 主配置文件 `config.json`

```json
{
    "commandOpenForm": "openform",
    "commandGiveForm": "giveform",
    "commandSetForm": "setform",
    "item": {
        "id": 720,
        "damage": 0,
        "count": 1,
        "slot": 8,
        "name": "§b星空§b菜§b§b单",
        "enchanted": true,
        "lockedByDefault": true
    }
}
```

#### 配置项说明

| 配置项 | 类型 | 默认值 | 说明 |
|:-------|:-----|:-------|:-----|
| `commandOpenForm` | 字符串 | `openform` | 打开表单的指令名 |
| `commandGiveForm` | 字符串 | `giveform` | 获取表单的指令名 |
| `commandSetForm` | 字符串 | `setform` | 设置表单状态的指令名 |
| `item.id` | 整数 | `720` | 表单物品的 ID (720=闹钟) |
| `item.damage` | 整数 | `0` | 物品耐久/特殊值 |
| `item.count` | 整数 | `1` | 物品数量 |
| `item.slot` | 整数 | `8` | 表单物品放置的槽位 (0-35, 8=第九格) |
| `item.name` | 字符串 | `§b星空§b菜§b§b单` | 物品显示名称 |
| `item.enchanted` | 布尔值 | `true` | 是否附魔消失诅咒 |
| `item.lockedByDefault` | 布尔值 | `true` | 新玩家是否默认获得锁定的表单 |

### 表单配置文件 `forms/*.json`

表单文件位于 `plugins/StarrySkyForm/forms/` 目录下，所有 `.json` 文件都会被识别为表单配置文件。

#### 表单文件格式

```json
{
    "title": "§b表单标题",
    "content": "表单内容描述",
    "buttons": {
        "按钮文本&图片路径": ["命令1", "命令2"],
        "纯文本按钮": ["命令"]
    }
}
```

#### 按钮格式

| 格式 | 示例 |
|:-----|:-----|
| 带图片 | `"传送&textures/items/ender_pearl.png": ["warp"]` |
| 纯文本 | `"返回主城": ["spawn"]` |

#### 常用图片路径

| 用途 | 路径 |
|:-----|:-----|
| 末影珍珠 | `textures/items/ender_pearl.png` |
| 绿宝石 | `textures/items/emerald.png` |
| 箱子 | `textures/blocks/chest_front.png` |
| 家 | `textures/ui/icon_home.png` |
| 设置 | `textures/ui/settings_glyph_color_2x.png` |
| 取消 | `textures/ui/cancel.png` |
| 玩家 | `textures/ui/icon_steve.png` |

#### 命令格式

按钮对应的命令支持：

| 类型 | 示例 |
|:-----|:-----|
| 普通命令 | `["spawn", "say 欢迎回家"]` |
| 打开其他表单 | `["openform 传送"]` |
| 占位符 | `["tp {player} 0 100 0"]` |

#### 示例表单

**main.json (主菜单)**

```json
{
    "title": "§b星空菜单",
    "content": "选择你需要的功能~",
    "buttons": {
        "传送&textures/items/ender_pearl.png": ["openform 传送"],
        "经济&textures/items/emerald.png": ["openform 经济"],
        "返回主城&textures/ui/icon_home.png": ["spawn"]
    }
}
```

**传送.json (子菜单)**

```json
{
    "title": "§b传送菜单",
    "content": "选择传送目的地",
    "buttons": {
        "回家&textures/ui/icon_home.png": ["home"],
        "返回主城&textures/ui/icon_steve.png": ["spawn"],
        "返回&textures/ui/cancel.png": ["openform main"]
    }
}
```

## 命令列表

### `/openform`

**打开表单菜单**

| 项目 | 说明 |
|:-----|:-----|
| 执行主体 | 玩家 |
| 所需权限 | / |

```
/openform          # 打开主菜单
/openform <名称>   # 打开指定名称的表单
```

> 配置文件中的 `commandOpenForm` 可修改指令名

---

### `/giveform`

**获得一个表单物品**

| 项目 | 说明 |
|:-----|:-----|
| 执行主体 | 玩家 |
| 所需权限 | / |

> 如果背包中已有表单物品，则无法获得
>
> 根据配置 `item.lockedByDefault` 决定给予锁定或非锁定的表单

---

### `/setform`

**设置表单状态**

| 项目 | 说明 |
|:-----|:-----|
| 执行主体 | 玩家 / 控制台 |
| 所需权限 | `lock` / `unlock` / `clear`: /，`reload`: OP |

```
/setform lock    # 锁定背包中的所有表单物品（无法移动、丢弃、合成）
/setform unlock  # 解锁背包中的所有表单物品（可移动，但仍无法丢弃、合成）
/setform reload  # 重新加载表单配置文件（仅限管理员）
/setform clear   # 清除背包中的所有表单物品
```

> **锁定**的表单物品：**无法移动、丢弃、合成**
>
> **解锁**的表单物品：**可以移动，但仍然无法丢弃、合成**，右键仍会触发表单
>
> `/setform reload` 仅限管理员 (OP) 或控制台执行

## 工程结构

```bash
StarrySkyForm
├─assets                    # 插件资源文件
│  ├─config.json           # 主配置文件
│  └─forms                 # 表单配置文件
│     └─main.json          # 主菜单表单
│
├─src
│  └─starryskyform
│     ├─Global.h           # 全局常量和声明
│     ├─StarrySkyForm.h/.cc # 插件主类
│     ├─config
│     │  ├─ConfigManager.h  # 配置管理
│     │  └─ConfigManager.cc
│     ├─form
│     │  ├─FormManager.h    # 表单系统
│     │  └─FormManager.cc
│     ├─item
│     │  ├─ItemManager.h    # 物品管理
│     │  └─ItemManager.cc
│     ├─command
│     │  ├─CommandRegistry.h # 指令注册
│     │  └─CommandRegistry.cc
│     └─event
│        ├─EventListener.h  # 事件监听
│        └─EventListener.cc
└─xmake.lua                # 构建配置
```

## 构建

```bash
cd StarrySkyForm
xmake f -m release
xmake
```

## 开源协议

本项目采用 MIT 开源协议。

> 开发者不对使用本软件造成的任何后果负责，当您使用本项目以及衍生版本时，您需要自行承担风险。

## 常见问题

### Q: 如何修改表单物品的位置？

修改 `config.json` 中的 `item.slot`：
- `0-8`: 快捷栏第 1-9 格
- `9-35`: 背包其他位置

### Q: 如何关闭新手自动获得表单？

将 `config.json` 中的 `item.lockedByDefault` 改为 `false`。

### Q: 如何添加新的表单？

1. 在 `forms/` 目录下创建新的 `.json` 文件
2. 按照表单配置格式编写内容
3. 执行 `/setform reload` 热重载（或重启服务器）
4. 使用 `/openform 文件名` 打开

### Q: 表单物品可以修改成其他物品吗？

可以，修改 `config.json` 中的 `item.id`：
- `720` = 闹钟 (推荐)
- `339` = 纸
- `340` = 书
- 其他物品 ID 请查询 Minecraft 物品 ID 表

### Q: 如何防止玩家丢弃表单物品？

表单物品**始终无法丢弃和合成**，无论锁定还是解锁状态。
- 锁定状态：无法移动 + 丢弃 + 合成
- 解锁状态：可以移动，但仍无法丢弃 + 合成

### Q: 配置文件修改后需要重启吗？

**不需要**。执行 `/setform reload` 即可热重载配置文件。

### Q: 为什么右键表单物品没有反应？

检查以下几点：
1. 物品名称是否完全匹配配置
2. 是否手持物品右键点击
3. 查看控制台是否有错误日志

## 未实现功能

以下功能在原版 StarrySkyForm (Nukkit) 中存在，但当前版本未实现：

| 命令 | 功能说明 | 状态 |
|:-----|:---------|:-----|
| `okform` | 确认表单，带确认/取消按钮的模态表单 | 未实现 |
| `argform` | 带输入框参数的表单 | 未实现 |
| `spform` | 特殊表单类型 | 未实现 |
| `tipsform` | 提示类表单，带提示文本 | 未实现 |
| `spargform` | 特殊参数表单 | 未实现 |
| `cleargfi` | 清空背包中所有表单物品（现由 `/setform clear` 替代） | 已替代 |
| `clearitem` | 清除指定物品 | 未实现 |
| `reloadform` | 重载表单配置（现由 `/setform reload` 替代） | 已替代 |

### 未实现功能详细说明

#### okform
模态确认表单，弹出对话框让玩家选择"是"或"否"。常用于确认危险操作。

#### argform  
带输入框的表单，玩家可以在表单中输入文字。用于收集玩家输入的信息。

#### tipsform
提示类表单，用于显示提示信息或公告。类似公告板功能。

#### cleargfi (已替代)
清空玩家背包中所有表单物品的命令。现由 `/setform clear` 替代。

#### clearitem
清除玩家背包中指定物品的命令。比 cleargfi 更通用，可指定任意物品。

---

> 以上未实现功能可根据需求添加，如有需要请提交 Issue。
