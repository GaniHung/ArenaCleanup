# Arena Cleanup

一款使用 Unreal Engine 5.7 制作的第一人称合作竞技场射击游戏。

## 游戏内容

- 三轮递进式敌人攻势：3、5、7 名敌人
- 单人模式与双人局域网 Listen Server 联机
- 双方玩家均可射击、受伤、死亡并重新加入战斗
- 有限弹匣、备用弹药、换弹与武器切换
- 生命、弹药、分数、波次和剩余敌人的网络同步
- Windows 与 Android 独立操作界面
- 中文和英文界面切换

## 操作

### Windows

| 操作 | 按键 |
| --- | --- |
| 移动 | `WASD` |
| 瞄准 | 鼠标 |
| 开火 | 鼠标左键 |
| 换弹 | `R` |
| 切换武器 | `Shift` |

### Android

- 左侧摇杆移动
- 右侧区域拖动瞄准
- 子弹按钮开火
- 圆形按钮换弹和切换武器

## 局域网联机

1. 两台设备连接到同一 Wi-Fi 或手机热点。
2. 主机选择“建立作战房间”。
3. 客户端输入主机局域网地址，例如 `192.168.1.20:7777`。
4. 客户端选择“接入战局”。

Windows 防火墙需要允许游戏访问专用网络。项目使用直接 IP 联机，不依赖 Steam 或 EOS。

## 开发环境

| 项目 | 配置 |
| --- | --- |
| 引擎 | Unreal Engine 5.7 |
| 默认地图 | `/Game/Variant_Shooter/Lvl_Shooter` |
| 默认 GameMode | `BP_ShooterGameMode` |
| 网络模式 | Listen Server / Direct IP LAN |
| Android 架构 | ARM64 |

打开 `UE5_ArenaCleanup.uproject` 后即可在编辑器中运行。Windows 和 Android 的可安装构建请从 GitHub Releases 下载。

## 第三方许可

项目基于 Unreal Engine 第一人称模板与 Shooter Variant，并包含 MIT 许可项目的参考实现。完整声明见 `LICENSES/`。
