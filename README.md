# Arena Cleanup

<p align="center">
  <img src="blob/main/image.png" alt="Arena Cleanup cover" width="820">
</p>
<p align="center">
  一款使用 Unreal Engine 5.7 制作的第一人称合作竞技场射击游戏<br>
  A cooperative first-person arena shooter built with Unreal Engine 5.7
</p>

<p align="center">
  <a href="#中文">中文</a> ·
  <a href="#english">English</a> ·
  <a href="https://github.com/GaniHung/UE5_ArenaCleanup/releases">Releases</a>
</p>

---

# 中文

## 项目简介

Arena Cleanup 是一款使用 Unreal Engine 5.7 制作的第一人称合作竞技场射击游戏。

玩家需要在三轮逐渐增强的敌人攻势中生存并清理竞技场。项目同时支持单人模式和双人局域网合作模式，并实现了战斗状态、玩家状态和界面信息的网络同步。

这个项目重点实践了 Unreal Engine 中的第一人称战斗、敌人行为、Listen Server 联机、状态同步以及 Windows 和 Android 多平台构建。

## 游戏截图

## 主要功能

### 波次战斗

游戏包含三轮递进式敌人攻势：

| 波次  | 敌人数量 |
| --- | ---: |
| 第一轮 |    3 |
| 第二轮 |    5 |
| 第三轮 |    7 |

玩家需要清除当前波次的全部敌人后才能进入下一轮。完成第三轮后取得胜利。

### 单人和双人合作

项目支持：

* 单人游戏
* 双人局域网合作
* Listen Server 主机模式
* 直接 IP 地址连接
* 同一 Wi-Fi 或手机热点下联机

双方玩家均可：

* 移动和瞄准
* 使用武器射击
* 受到敌人和另一玩家造成的伤害
* 死亡后重新加入战斗
* 查看同步后的生命、弹药、分数和波次信息

### 战斗系统

* 有限弹匣和备用弹药
* 手动换弹
* 武器切换
* 玩家生命与死亡机制
* 敌人追踪与攻击
* 击败敌人后累计分数
* 三轮波次与胜利判定

### 网络同步

以下状态会在主机与客户端之间同步：

* 玩家生命值
* 当前弹匣
* 备用弹药
* 当前武器
* 玩家分数
* 当前波次
* 剩余敌人数量
* 战斗胜负状态

### 多平台界面

项目针对不同平台提供了独立的操作方式：

* Windows：键盘和鼠标
* Android：虚拟摇杆和触摸按钮

界面支持中文和英文切换。

## 操作方式

### Windows

| 操作   | 按键              |
| ---- | --------------- |
| 移动   | `W` `A` `S` `D` |
| 瞄准   | 鼠标移动            |
| 开火   | 鼠标左键            |
| 换弹   | `R`             |
| 切换武器 | `Shift`         |

### Android

| 操作   | 方式       |
| ---- | -------- |
| 移动   | 左侧虚拟摇杆   |
| 瞄准   | 拖动右侧区域   |
| 开火   | 点击子弹按钮   |
| 换弹   | 点击换弹按钮   |
| 切换武器 | 点击武器切换按钮 |

## 局域网联机

### 主机

1. 将两台设备连接到同一 Wi-Fi 或手机热点。
2. 启动游戏。
3. 选择“建立作战房间”。
4. 查询主机在局域网中的 IP 地址。

### 客户端

1. 启动游戏。
2. 输入主机的局域网地址，例如：

```text
192.168.1.20:7777
```

3. 选择“接入战局”。

项目采用直接 IP 地址连接，不依赖 Steam 或 Epic Online Services。

如果客户端无法连接，请检查：

* 两台设备是否处于同一局域网
* IP 地址和端口是否正确
* Windows 防火墙是否允许游戏访问专用网络
* 主机游戏是否已经建立 Listen Server

## 开发环境

| 项目          | 配置                                  |
| ----------- | ----------------------------------- |
| 游戏引擎        | Unreal Engine 5.7                   |
| 默认地图        | `/Game/Variant_Shooter/Lvl_Shooter` |
| 默认 GameMode | `BP_ShooterGameMode`                |
| 网络模式        | Listen Server / Direct IP LAN       |
| Windows 架构  | x86-64                              |
| Android 架构  | ARM64                               |

## 运行项目

1. 安装 Unreal Engine 5.7。
2. 克隆仓库：

```bash
git clone https://github.com/GaniHung/UE5_ArenaCleanup.git
```

3. 打开：

```text
UE5_ArenaCleanup.uproject
```

4. 在 Unreal Editor 中加载默认地图并运行。

Windows 和 Android 的独立构建请查看：

[GitHub Releases](https://github.com/GaniHung/UE5_ArenaCleanup/releases)

## 项目目标

本项目主要用于实践：

* Unreal Engine 第一人称游戏开发
* Blueprint 与项目资源集成
* 敌人波次和基础 AI
* Listen Server 网络架构
* 多人游戏状态同步
* Windows 和 Android 多平台适配
* 游戏测试、构建和交付流程

项目规模不大，但完整覆盖了从玩法设计、功能实现、联机测试到独立构建和文档整理的过程。

## 开发说明

项目在开发过程中使用了 AI 编程工具辅助代码实现和问题排查。项目需求、功能设计、任务拆分、集成测试、验收标准和最终交付由项目维护者组织完成。

## 第三方内容与许可

项目基于 Unreal Engine 第一人称模板及 Shooter Variant，并参考了部分采用 MIT 许可证发布的实现。

完整的第三方声明和许可证见：

```text
LICENSES/
```

Unreal Engine 相关内容同时受 Epic Games 对应许可证约束。

---

# English

## Overview

Arena Cleanup is a cooperative first-person arena shooter built with Unreal Engine 5.7.

Players must survive three increasingly difficult enemy waves and clear the arena. The project supports both single-player and two-player LAN cooperative modes, with synchronized combat, player, scoring, and wave states.

The project was developed as coursework and focuses on first-person combat, enemy behavior, Listen Server multiplayer, replicated game state, and Windows/Android deployment.

## Screenshots

<p align="center">
  <img src="docs/images/gameplay-01.png" alt="Arena Cleanup gameplay" width="48%">
  <img src="docs/images/gameplay-02.png" alt="Enemy wave combat" width="48%">
</p>

<p align="center">
  <img src="docs/images/interface.png" alt="Arena Cleanup interface" width="72%">
</p>

## Features

### Wave-based combat

The game contains three enemy waves:

| Wave   | Number of enemies |
| ------ | ----------------: |
| Wave 1 |                 3 |
| Wave 2 |                 5 |
| Wave 3 |                 7 |

Players advance after defeating all enemies in the current wave. Clearing the third wave completes the game.

### Single-player and cooperative multiplayer

The project supports:

* Single-player mode
* Two-player LAN cooperative mode
* Listen Server hosting
* Direct-IP connection
* Multiplayer over the same Wi-Fi network or mobile hotspot

Both players can:

* Move and aim
* Fire weapons
* Receive damage
* Die and rejoin combat
* View synchronized health, ammunition, score, and wave information

### Combat system

* Limited magazine capacity
* Reserve ammunition
* Manual reloading
* Weapon switching
* Player health and death
* Enemy pursuit and attacks
* Score accumulation
* Wave progression and victory conditions

### Replicated state

The following information is synchronized between the host and client:

* Player health
* Magazine ammunition
* Reserve ammunition
* Current weapon
* Player score
* Current wave
* Remaining enemies
* Match result

### Platform-specific interfaces

The project includes separate control schemes for:

* Windows keyboard and mouse
* Android virtual joystick and touch controls

Both Chinese and English interfaces are available.

## Controls

### Windows

| Action        | Input             |
| ------------- | ----------------- |
| Move          | `W` `A` `S` `D`   |
| Aim           | Mouse             |
| Fire          | Left mouse button |
| Reload        | `R`               |
| Switch weapon | `Shift`           |

### Android

| Action        | Input                  |
| ------------- | ---------------------- |
| Move          | Left virtual joystick  |
| Aim           | Drag on the right side |
| Fire          | Fire button            |
| Reload        | Reload button          |
| Switch weapon | Weapon-switch button   |

## LAN Multiplayer

### Host

1. Connect both devices to the same Wi-Fi network or mobile hotspot.
2. Launch the game.
3. Select **Create Battle Room**.
4. Find the host device's local IP address.

### Client

1. Launch the game.
2. Enter the host address, for example:

```text
192.168.1.20:7777
```

3. Select **Join Battle**.

The game uses direct-IP networking and does not depend on Steam or Epic Online Services.

If the client cannot connect, verify:

* Both devices are on the same local network
* The IP address and port are correct
* Windows Firewall allows private-network access
* The host has already created the Listen Server

## Development Environment

| Item                 | Configuration                       |
| -------------------- | ----------------------------------- |
| Engine               | Unreal Engine 5.7                   |
| Default map          | `/Game/Variant_Shooter/Lvl_Shooter` |
| Default GameMode     | `BP_ShooterGameMode`                |
| Networking           | Listen Server / Direct IP LAN       |
| Windows architecture | x86-64                              |
| Android architecture | ARM64                               |

## Running the Project

1. Install Unreal Engine 5.7.
2. Clone the repository:

```bash
git clone https://github.com/GaniHung/UE5_ArenaCleanup.git
```

3. Open:

```text
UE5_ArenaCleanup.uproject
```

4. Load the default map and run the project in Unreal Editor.

Standalone Windows and Android builds are available from:

[GitHub Releases](https://github.com/GaniHung/UE5_ArenaCleanup/releases)

## Project Scope

This project was created to practise:

* First-person game development in Unreal Engine
* Blueprint and asset integration
* Wave-based enemy AI
* Listen Server networking
* Multiplayer state replication
* Windows and Android adaptation
* Testing, packaging, and delivery workflows

Although relatively small in scope, the project covers the complete process from gameplay planning and implementation to multiplayer testing, packaging, and documentation.

## Development Note

AI coding tools were used to assist implementation and debugging. Requirements, feature design, task planning, integration testing, acceptance criteria, and final delivery were coordinated by the project maintainer.

## Third-party Licenses

The project is based on the Unreal Engine First Person template and Shooter Variant, and includes references to implementations released under the MIT License.

See the following directory for complete notices:

```text
LICENSES/
```

Unreal Engine content is additionally governed by the applicable Epic Games license.
