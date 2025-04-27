## 项目概述

Blaster 是一款基于 Unreal Engine 5.4 开发的多人在线第三人称射击游戏。本项目实现了完整的多人联网对战系统，包括团队竞技、夺旗模式等多种游戏模式，并集成了 AWS GameLift 分布式服务器架构。

## 主要功能

### 角色系统
- 完整的第三人称角色控制系统
- 角色动画系统，包括行走、奔跑、跳跃、蹲伏、瞄准等动作
- 先进的动画蒙太奇系统用于武器操作、射击和装弹

### 战斗系统
- 多种武器类型：近战、远程、霰弹枪等
- 弹药和装弹系统
- 投掷物（如手雷）系统
- 伤害计算和生命值系统
- 护盾系统

### 多人游戏模式
- 死亡竞赛模式
- 团队竞技模式
- 夺旗模式
- 游戏内积分和战绩追踪

### 网络功能
- 基于 Unreal 网络架构的多人联机
- 延迟补偿系统
- Steam 在线子系统集成
- 玩家会话管理

### 云服务集成
- AWS GameLift 服务器集成
- 专用服务器支持
- 玩家认证与会话管理
- 游戏数据统计和排行榜系统

### 用户界面
- 游戏内 HUD 显示生命值、护盾、弹药等信息
- 主菜单系统
- 游戏会话创建和加入界面
- 比赛结果显示

## 技术实现

### 核心模块
- `BlasterCharacter`: 玩家角色类，处理移动、动画和交互
- `CombatComponent`: 处理武器、射击和战斗相关逻辑
- `BuffComponent`: 处理各种能力增强效果
- `LagCompensationComponent`: 处理网络延迟补偿

### 多人游戏架构
- `BlasterGameMode`: 定义游戏规则和流程
- `TeamGameMode`: 团队模式的游戏规则
- `CaptureTheFlagGameMode`: 夺旗模式的游戏规则
- `BlasterPlayerState`: 跟踪玩家分数和数据
- `BlasterGameState`: 管理整体游戏状态

### AWS GameLift 集成
- `DS_LobbyGameMode`: 处理游戏大厅和玩家匹配
- `DS_GameInstanceSubsystem`: 管理与 AWS GameLift 的通信
- `GameSessionsManager`: 处理游戏会话的创建和加入

### 玩家认证和会话
- `MultiplayerSessionsSubsystem`: 会话创建、搜索和加入
- `PortalManager`: 玩家注册、登录和认证
- `HTTPRequestManager`: 处理与后端 API 的通信

## 文件结构

```
Source/
├── Blaster/              # 游戏核心代码
│   ├── Character/        # 玩家角色相关
│   ├── GameMode/         # 游戏模式定义
│   ├── PlayerController/ # 玩家控制器
│   ├── HUD/             # 界面显示
│   ├── Weapon/          # 武器系统
│   ├── BlasterComponents/ # 组件系统
│   └── ...
└── DedicatedServers/     # 专用服务器实现
    ├── Private/
    │   ├── Game/        # 服务器游戏逻辑
    │   └── UI/          # 用户界面
    └── Public/
        └── ...
```

## 配置和设置

### 联网设置
游戏使用 Steam 在线子系统进行联网，相关设置位于 DefaultEngine.ini:

```ini
[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=false
SteamDevAppId=480
bInitServerOnClient=true
```

### AWS 配置
AWS GameLift 集成需要设置适当的认证和服务参数:

```
AUTH_TOKEN - 从 AWS GameLift 获取的认证令牌
HOST_ID - GameLift Anywhere 实例的主机名
FLEET_ID - Anywhere 舰队 ID
WEBSOCKET_URL - GameLift 服务 SDK 终端点
```

## 开发注意事项

1. 项目需要 Unreal Engine 5.4 或更高版本
2. 构建项目需要安装 Visual Studio 2022 或更高版本
3. 云部署需要配置 AWS 账户和 GameLift 服务
4. 联机功能测试需要配置 Steam SDK

## 构建和运行

使用 Unreal Engine 编辑器打开 Blaster.uproject 文件，然后可以在编辑器中构建和运行项目。

### 本地测试

1. 使用编辑器的 PIE (Play In Editor) 模式测试单人游戏
2. 使用 "Launch" 菜单中的多玩家选项测试本地多人游戏

### 专用服务器部署

1. 构建专用服务器版本：`BuildCookRun -project=Blaster.uproject -noP4 -clientconfig=Development -serverconfig=Development -server -serverplatform=Win64`
2. 配置 AWS GameLift 并上传服务器构建
3. 使用 AWS GameLift 控制台创建舰队和别名
