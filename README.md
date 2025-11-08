# RLX Teleport

RLX Teleport 是RLX服务器的自用插件，用于传送玩家到指定位置。

该插件基于 [LeviLamina](https://github.com/LeviLamina/LeviLamina) 开发。

该插件包含以下功能：

- Warp 公共传送点
- Home 家传送点
- Tpa 玩家之间的传送
- 自杀和返回上一次死亡位置
- 物品触发菜单（通过使用特定物品打开图形化传送菜单）

## 安装

### 使用lip安装
```bash
lip install github.com/carefly/RLXTeleport
```

### 手动安装

根据LeviLamina版本选择合适的[Releases](https://github.com/carefly/RLXTeleport/releases)版本

安装好LeviLamina后，在游戏根目录下会有plugins文件夹（如果游戏根目录下没有plugins文件夹，请自行创建）

将解压后的RLXTeleport文件夹放入plugins文件夹中

## 使用

### 命令

#### warp 命令
  
| 命令                             | 描述                                                                                                   | 权限     |
| -------------------------------- | ------------------------------------------------------------------------------------------------------ | -------- |
| warp                             | 打开图形化菜单选择要传送的传送点                                                                       | 普通玩家 |
| warp [name]                      | 传送到[name]传送点                                                                                     | 普通玩家 |
| warp ls                          | 列出所有传送点                                                                                         | 普通玩家 |
| warpop add [name] [description]  | 添加名称为[name]，描述为[description]的传送点<br>传送点的位置为管理员当前位置<br>description为可选参数 | 管理员   |
| warpop del [name]                | 删除名称为[name]的传送点                                                                               | 管理员   |
| warpop edit [name] [description] | 更新名称为[name]的传送点的描述为[description]                                                          | 管理员   |
| warpop reload                    | 重新加载warp文件                                                                                       | 管理员   |

#### home 命令

| 命令            | 描述                                         | 权限     |
| --------------- | -------------------------------------------- | -------- |
| home            | 打开图形化菜单选择要传送的家                 | 普通玩家 |
| home go [name]  | 传送到名称为[name]的家                       | 普通玩家 |
| home ls         | 列出所有家                                   | 普通玩家 |
| home add [name] | 添加名称为[name]的家，家的位置为玩家当前位置 | 普通玩家 |
| home del [name] | 删除名称为[name]的家                         | 普通玩家 |


#### tpa命令

| 命令              | 描述                   | 权限     |
| ----------------- | ---------------------- | -------- |
| tpa to [player]   | 请求传送到指定玩家     | 普通玩家 |
| tpa call [player] | 请求指定玩家传送到自己 | 普通玩家 |
| tpa accept        | 接受玩家的传送请求     | 普通玩家 |
| tpa reject        | 拒绝玩家的传送请求     | 普通玩家 |
| tpa cancel        | 取消自己的传送请求     | 普通玩家 |
| tpa query         | 查询传送请求           | 普通玩家 |

#### 物品触发菜单

玩家可以通过使用特定物品来打开图形化传送菜单。默认情况下，物品名称包含 "sword" 关键词的物品会触发菜单（可在配置文件中修改 `trigger_item_keyword`）。

使用触发物品后，会显示一个主菜单，包含以下选项：
- **家 (Home)**：打开家列表表单，选择要传送的家
- **传送点 (Warp)**：打开传送点列表表单，选择要传送的传送点

#### 自杀和返回上一次死亡位置

| 命令    | 描述               | 权限     |
| ------- | ------------------ | -------- |
| suicide | 自杀               | 普通玩家 |
| back    | 返回上一次死亡位置 | 普通玩家 |

### 配置文件说明

插件里面不包含配置文件，启动游戏后会自动生成配置文件

配置文件路径：plugins/RLXModeResources/config/config.json

配置文件示例：

```
{
    "home_limit": 16, // 家数量限制
    "tpa_timeout": 60, // 传送请求超时时间（秒）
    "trigger_item_keyword": "sword" // 触发传送菜单的物品关键词（物品名称包含此关键词时，使用该物品会打开传送菜单）
}
```

### 数据文件说明

- warp：传送点数据
  - 数据文件路径：plugins/RLXModeResources/data/warps/warp.json
  - 命令行里面description如果有空格，请用双引号，description里面不能使用双引号
  - 数据文件示例：

```
[
    {
        "d": 0,  // 维度
        "description": "this is warp1",  // 描述
        "name": "warp1",  // 名称
        "x": 917.0,  // x坐标
        "y": 67.0,  // y坐标
        "z": 1788.0  // z坐标
    },
    {
        "d": 0,
        "description": "this is warp2",
        "name": "warp2",
        "x": 986.0,
        "y": 74.0,
        "z": 1893.0
    }
]
```

- home：家配置
  - 数据文件路径：plugins/RLXModeResources/data/homes/
  - 数据文件说明：
  
    在plugins/RLXModeResources/data/homes/ 目录下，每个玩家一个文件，文件名为 **xuid - name.json**，插件根据xuid识别玩家，name为玩家的nameTag，仅为展示，方便管理员管理文件

  - 数据文件示例：

```
[
    {
        "name": "home1", // 家的名称
        "pos": {
            "x": 156.0, // x坐标
            "y": 78.0, // y坐标
            "z": 156.0 // z坐标
        },
        "d": 0 // 维度
    },
    {
        "name": "home2",
        "pos": {
            "x": -584.0,
            "y": 56.0,
            "z": -156.0
        },
        "d": 0
    }
]
```