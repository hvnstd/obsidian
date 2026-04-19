#!/bin/bash

# Synthesizer V Studio Pro 无头运行脚本
# Headless runner for Synthesizer V Studio Pro

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SYNTHV_DIR="${SCRIPT_DIR}/Synthesizer V Studio Pro"
SYNTHV_EXE="${SYNTHV_DIR}/synthv-studio"

# 检查主程序是否存在
if [ ! -f "${SYNTHV_EXE}" ]; then
    echo "错误：找不到 synthv-studio 主程序"
    echo "Error: synthv-studio executable not found"
    exit 1
fi

# 设置环境变量以禁用音频（可选）
export PULSE_SERVER=/dev/null
export JACK_NO_START_SERVER=1

# 使用 Xvfb 提供虚拟显示环境
# 如果需要更小的内存占用，可以使用 Xvnc 或 xvfb-run 的特定参数
exec xvfb-run -a --server-args="-screen 0 1024x768x24" "${SYNTHV_EXE}" "$@"
