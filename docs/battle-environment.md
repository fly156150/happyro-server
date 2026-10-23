# 地图服视野与行走环境变量

地图服直接读取以下进程环境变量，原生服务与 Docker 使用相同实现。

| 环境变量 | battle 配置 | 默认值 | 环境变量允许范围 |
| --- | --- | --- | --- |
| `HAPPYRO_AREA_SIZE` | `area_size` | 18 格 | 5–50，十进制整数 |
| `HAPPYRO_MAX_WALK_PATH` | `max_walk_path` | 17 格 | 1–32，十进制整数 |

未设置或空值保留 battle 配置文件的值；非空变量在全部 import 读取完成后覆盖文件配置，随后执行既有配置调整。环境变量拒绝负数、小数、符号、空白、布尔值、溢出和越界值；错误会明确打印变量名并终止地图服。

默认视野同时在 `conf/battle/client.conf` 与内置配置表中设为 18。优先级为：非空环境变量 > battle 配置文件（包括导入文件）> 内置默认值。已有导入配置显式设置为 14 时，应移除该覆盖或设置 `HAPPYRO_AREA_SIZE=18`。

更改进程环境变量需要重启地图服（Docker 需要重新创建 map 容器）。重载 battle 配置会重新应用当前进程的环境变量，因此管理后台对同名文件配置的修改不能覆盖非空环境变量。GM `@setbattleflag` 仍是临时运行时修改，下次配置重载或进程重启重新按上述优先级取值。

systemd 可通过 `systemctl edit happyro-map-server` 设置：

```ini
[Service]
Environment=HAPPYRO_AREA_SIZE=18
Environment=HAPPYRO_MAX_WALK_PATH=17
```

视野还参与怪物活跃 AI、事件广播及部分跟随逻辑。环境变量范围是输入校验范围，不是性能保证。提高视野不会自动提高寻怪范围或行走上限；绕障碍后的路径仍可能超过单次行走上限。客户端寻怪与活动范围由 Gateway 的 `CLIENT_AUTO_COMBAT_SEARCH_RANGE` 和 `CLIENT_AUTO_COMBAT_ACTIVITY_RANGE` 独立提供。
