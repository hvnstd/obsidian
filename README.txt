=== YumeKey 3 发布版 ===

YumeKey 3（代号：黑曜石/Obsidian）可禁用 Synthesizer V Studio Pro
及歌声库的激活要求。

您无需拥有 Synthesizer V Studio Pro 或任何歌声库。

有关 YumeKey 的更多信息，请访问 https://jinpwnsoft.re 或发送电子邮件至 support@jinpwnsoft.re
YumeKey 现在是并且将来也永远是免费的。如果您为此软件付费，那么您已被诈骗。

安装说明：

1. 将 "obsidian.exe" 和 "obsidian.dll" 复制到 Synthesizer V Studio Pro 的安装路径，
   通常为 "C:\Program Files\Synthesizer V Studio Pro" 或类似路径。

2. （可选）在同一路径下创建 "obsidian.ini" 以进行配置（见下文）。

3. 使用 "obsidian.exe" 打开 Synthesizer V Studio Pro 并安装任何您想要的歌声数据库。

限制：

* 在此之前，您应该停用已购买的歌声库，以避免因意外卸载而失去激活码使用次数。

* 已在 Synthesizer V Studio Pro 1.11.0 至 1.11.2 版本上测试。

* 暂不支持 VST。计划在未来添加。

配置（高级）：

您可以在 obsidian.ini 中使用以下任何选项，或者在启动 Synthesizer V Studio 之前
将它们设置为环境变量。

; SynthV 模块基地址（1 - 使用默认值；否则使用指定值）
OBSIDIAN_BASE_ADDR=1
; 调试模式（0 - 无，1 - 启用，2 - 详细）
OBSIDIAN_DEBUG_MODE=0
; 控制台文件，用于在启用调试模式时保存日志。
OBSIDIAN_CONSOLE_FILE=C:\path\to\log\file.txt
; 阻止 SynthV 访问互联网。
OBSIDIAN_BLOCK_INTERNET=1
; 检查 SynthV 版本以确保兼容性。
OBSIDIAN_CHECK_SYNTHV_VERSION=1

如果您要在环境中设置这些选项，可以通过 Windows 设置全局设置，或在命令提示符处设置：

C:\Program Files\Synthesizer V Studio Pro> set OBSIDIAN_DEBUG_MODE=2

C:\Program Files\Synthesizer V Studio Pro> obsidian.exe

环境变量会覆盖配置文件选项。

