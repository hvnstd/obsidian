# Synthesizer V Studio Pro Linux 无头运行指南

## 概述
本目录包含 Synthesizer V Studio Pro (Linux64) 的无头运行配置。

## 文件结构
```
/workspace/
├── Synthesizer V Studio Pro/    # 主程序目录
│   ├── synthv-studio            # 主执行文件
│   ├── *.dnni                   # 深度学习模型文件
│   ├── fonts/                   # 字体文件
│   └── ...                      # 其他资源文件
├── run_headless.sh              # 无头运行启动脚本
└── README_LINUX.md              # 本说明文件
```

## 系统要求

### 必需依赖
- Linux x86_64 系统
- Xvfb (X Virtual Framebuffer)
- ALSA 库 (libasound2)
- OpenGL 支持
- 其他标准库 (已在 ldd 检查中确认)

### 安装依赖 (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install -y xvfb xauth libasound2 libgl1 libstdc++6
```

## 使用方法

### 方法 1: 使用启动脚本 (推荐)
```bash
cd /workspace
./run_headless.sh
```

### 方法 2: 直接使用 xvfb-run
```bash
cd "/workspace/Synthesizer V Studio Pro"
xvfb-run -a ./synthv-studio
```

### 方法 3: 设置虚拟显示器后运行
```bash
# 启动 Xvfb
Xvfb :99 -screen 0 1024x768x24 &
export DISPLAY=:99

# 运行程序
cd "/workspace/Synthesizer V Studio Pro"
./synthv-studio

# 结束后清理
killall Xvfb
```

## 环境变量

### 音频相关
```bash
# 禁用 PulseAudio
export PULSE_SERVER=/dev/null

# 禁用 JACK 自动启动
export JACK_NO_START_SERVER=1

# 使用 ALSA null 输出
export ALSA_DEFAULT_PCM_DEVICE=null
export ALSA_DEFAULT_SEQ_DEVICE=null
```

### 显示相关
```bash
# 指定显示编号
export DISPLAY=:99

# 设置屏幕分辨率 (通过 xvfb-run 参数)
xvfb-run --server-args="-screen 0 1920x1080x24" ./synthv-studio
```

## 注意事项

1. **GUI 程序**: Synthesizer V Studio 是图形界面程序，即使"无头"运行也需要 X Server 环境。Xvfb 提供虚拟显示环境。

2. **音频输出**: 在无头环境中，音频输出可能不可用或需要特殊配置。程序可能会输出 ALSA/JACK 警告，但通常不影响核心功能。

3. **命令行参数**: 当前版本似乎没有公开的命令行参数用于批处理或自动化。程序启动后会进入 GUI 界面。

4. **自动化**: 如需自动化操作，可能需要:
   - 使用 X11 自动化工具 (如 xdotool)
   - 逆向工程内部 API
   - 等待官方提供 CLI 模式

5. **性能优化**: 
   - 降低虚拟显示器分辨率可减少内存占用
   - 使用 `xvfb-run -a` 自动选择可用的显示编号
   - 考虑使用更轻量的 X server 如 Xvnc

## 故障排除

### 问题：找不到 synthv-studio
```bash
# 确保在正确的目录
ls -la "/workspace/Synthesizer V Studio Pro/synthv-studio"
```

### 问题：xvfb-run 错误
```bash
# 安装 xauth
sudo apt-get install xauth

# 检查 Xvfb 是否可用
which xvfb-run
```

### 问题：ALSA/JACK 警告
这些警告在无音频设备的环境中是正常的，通常可以忽略。

### 问题：OpenGL 错误
```bash
# 安装 OpenGL 库
sudo apt-get install libgl1-mesa-glx libglu1-mesa

# 或使用软件渲染
export LIBGL_ALWAYS_SOFTWARE=1
```

## 许可证
请遵守 Synthesizer V Studio 的最终用户许可协议 (EULA)。
详见 `Synthesizer V Studio Pro/license-en.txt`

## 版本信息
- Synthesizer V Studio Pro 1.11.2
- Linux64 版本
- 发布日期：2024 年 9 月
