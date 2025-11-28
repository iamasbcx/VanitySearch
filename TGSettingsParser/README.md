# TGSettingsParser - Telegram Desktop Settings File Parser

基于 Telegram Desktop (https://github.com/telegramdesktop/tdesktop) 的设置文件解析工具。

## 功能

- 读取和解密 Telegram Desktop 的 settings 配置文件
- 解析并显示设置数据
- 修改代理配置（添加/删除/启用/禁用）
- 支持从 Telegram 代理链接解析代理配置
- 修改归档设置（折叠/展开/显示在主菜单）
- 重新加密并保存修改后的设置

## 支持的代理链接格式

工具支持解析以下格式的 Telegram 代理链接：

```
# MTProto 代理
https://t.me/proxy?server=1.2.3.4&port=443&secret=...
tg://proxy?server=1.2.3.4&port=443&secret=...

# SOCKS5 代理
https://t.me/socks?server=1.2.3.4&port=1080&user=xxx&pass=xxx
https://t.me/socks?server=1.2.3.4&port=1080
tg://socks?server=1.2.3.4&port=1080&user=xxx&pass=xxx
```

## 加密机制说明

### Settings 文件 (tdata/settings)
- 使用 salt + 空密码 生成的密钥进行 AES-256-IGE 加密
- **不需要** `key_datas` 文件
- 密钥生成: `CreateLegacyLocalKey("", salt)` 
- 使用 PBKDF2-HMAC-SHA1 派生密钥

### 账户数据 (tdata/D877F783D5D3EF8C/map 等)
- 需要 `_localKey` (来自 key_datas 或用户密码)
- 用于存储用户特定数据

## 编译要求

### VS2010 编译
需要 OpenSSL 1.0.2 (使用 VS2010 编译的版本)

1. 下载 OpenSSL 1.0.2 源码或预编译库
2. 设置环境变量 `OPENSSL_DIR` 指向 OpenSSL 安装目录
3. 打开 `TGSettingsParser.sln`
4. 编译 Release|Win32 或 Release|x64

### 依赖库
- OpenSSL 1.0.2 (libeay32.lib, ssleay32.lib)

## 使用方法

```
TGSettingsParser.exe <command> [options]

Commands:
  read <tdata_path>              - 读取并显示所有设置
  
  # 代理命令
  proxy list <tdata_path>        - 列出所有代理配置
  proxy add <tdata_path> <type> <host> <port> [user] [pass]
                                 - 添加新代理
                                 - type: socks5, http, mtproto
  proxy addurl <tdata_path> <url>
                                 - 从 Telegram 链接添加代理
  proxy remove <tdata_path> <index>
                                 - 按索引删除代理
  proxy select <tdata_path> <index>
                                 - 选择活动代理
  proxy enable <tdata_path>      - 启用代理
  proxy disable <tdata_path>     - 禁用代理 (使用系统设置)
  
  # 归档设置命令
  archive collapsed <tdata_path> <0|1>
                                 - 设置归档折叠状态 (0=展开, 1=折叠)
  archive menu <tdata_path> <0|1>
                                 - 设置归档显示在主菜单 (0=否, 1=是)
  archive expand <tdata_path>    
                                 - 显示归档在主菜单 + 取消折叠
```

## 示例

```cmd
# 读取设置
TGSettingsParser.exe read "C:\Users\User\AppData\Roaming\Telegram Desktop\tdata"

# 添加 SOCKS5 代理
TGSettingsParser.exe proxy add "C:\tdata" socks5 127.0.0.1 1080

# 添加 HTTP 代理（带认证）
TGSettingsParser.exe proxy add "C:\tdata" http proxy.example.com 8080 user password

# 添加 MTProto 代理
TGSettingsParser.exe proxy add "C:\tdata" mtproto mtproxy.example.com 443

# 从链接添加代理
TGSettingsParser.exe proxy addurl "C:\tdata" "https://t.me/socks?server=192.168.1.115&port=7890&user=123&pass=123"
TGSettingsParser.exe proxy addurl "C:\tdata" "https://t.me/proxy?server=47.86.28.109&port=443&secret=ee1bdbc4..."
TGSettingsParser.exe proxy addurl "C:\tdata" "tg://proxy?server=216.234.140.145&port=8443&secret=..."

# 列出代理
TGSettingsParser.exe proxy list "C:\tdata"

# 选择第一个代理
TGSettingsParser.exe proxy select "C:\tdata" 0

# 启用代理
TGSettingsParser.exe proxy enable "C:\tdata"

# 禁用代理
TGSettingsParser.exe proxy disable "C:\tdata"

# 归档设置 - 显示在主菜单并展开
TGSettingsParser.exe archive expand "C:\tdata"

# 归档设置 - 单独设置
TGSettingsParser.exe archive menu "C:\tdata" 1      # 显示在主菜单
TGSettingsParser.exe archive collapsed "C:\tdata" 0  # 取消折叠
```

## 设置文件格式

### 文件结构
```
[TDF$]           - 4 bytes magic
[version]        - 4 bytes int32
[data]           - variable
[md5]            - 16 bytes signature
```

### 加密数据结构
```
[salt]           - 32 bytes
[encrypted_data] - variable, AES-256-IGE encrypted
```

### 解密后数据格式
```
[data_length]    - 4 bytes
[block_id]       - 4 bytes
[block_data]     - variable
...
[padding]        - aligned to 16 bytes
```

## Block IDs

主要的设置块 ID:
- `0x06` - dbiAutoStart
- `0x07` - dbiStartMinimized
- `0x0c` - dbiAutoUpdate
- `0x16` - dbiScalePercent
- `0x4c` - dbiConnectionType (代理设置)
- `0x51` - dbiThemeKey
- `0x60` - dbiFallbackProductionConfig
- `0x99` - dbiApplicationSettings (包含归档设置)

## 注意事项

1. 修改设置前建议备份 tdata 文件夹
2. 确保 Telegram Desktop 未运行时进行修改
3. 代理密码以明文形式存储在设置文件中

## 许可证

本项目参考了 Telegram Desktop 的代码，遵循相同的许可证:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
