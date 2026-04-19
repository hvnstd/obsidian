#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

int main(int argc, char *argv[]) {
    char *exePath = NULL;
    char *soPath = NULL;
    
    // 获取环境变量
    exePath = getenv("SYNTHV_STUDIO_EXE");
    if (!exePath) {
        // 获取当前程序路径
        char path[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len == -1) {
            perror("readlink");
            return 1;
        }
        path[len] = '\0';
        
        // 获取目录
        char *dir = dirname(path);
        static char newPath[PATH_MAX];
        snprintf(newPath, sizeof(newPath), "%s/synthv-studio", dir);
        exePath = newPath;
    }
    
    soPath = getenv("OBSIDIAN_SO");
    if (!soPath) {
        soPath = "./obsidian.so";
    }
    
    printf("[Obsidian Launcher] 目标程序：%s\n", exePath);
    printf("[Obsidian Launcher] 库文件：%s\n", soPath);
    
    // 获取库文件的绝对路径
    char absSoPath[PATH_MAX];
    if (realpath(soPath, absSoPath) == NULL) {
        perror("realpath");
        return 1;
    }
    
    // 设置 LD_PRELOAD 环境变量
    setenv("LD_PRELOAD", absSoPath, 1);
    
    // 直接执行目标程序（替换当前进程）
    execvp(exePath, argv);
    
    // 如果 execvp 返回，说明出错了
    perror("execvp");
    return 1;
}
