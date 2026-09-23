#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <dlfcn.h>
#include <frida-gum.h>

#define DEBUG

#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_PATCH 0

struct FunctionBase;

struct FunctionBase_vtable {
  size_t _reserved[2];
  union {
    void *operator_fnPtr;
    void (*operator_fn)(struct FunctionBase *thisPtr);
  };
  size_t _reserved1[32];
};

struct FunctionBase {
  struct FunctionBase_vtable *vtable;
  size_t _reserved[4];
};

struct ProductBase_vtable {
  size_t _reserved[8];
};

struct ProductBase {
  struct ProductBase_vtable *vtable;
  size_t _reserved[1];
  const char *name;
  const char *vendor;
  size_t _reserved2[7];
  union {
    struct {
      bool isActivated;
      bool isFree;
      bool _reserved1;
      bool hasNoProductKey;
    };
    uint8_t activationFlags[4];
  };
};

#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf(...)
#endif

static const char *_lifeCycle = "UNKNOWN";
#define LIFECYCLE(x) (_lifeCycle = x)

#define die() _die(__LINE__)
void _Noreturn _die();

#define SV_CHECKED_BASE 0x0

static uintptr_t fptrAddBase = 1;
static void *getActivationStatusFPtr = NULL;
static void *voiceDBVerifyProductKeyFPtr = NULL;
static void *appVerifyProductKeyFPtr = NULL;
static void *setOkFlagFPtr = NULL;

static uint8_t getActivationStatusFSig_[] = {
  0x4c, 0x89, 0x44, 0x24, 0x18, 0x48, 0x89, 0x54, 0x24, 0x10, 0x53, 0x55, 0x56, 0x57, 0x41, 0x56,
  0x48, 0x83, 0xec, 0x60, 0x49, 0x8b, 0xf8, 0x48, 0x8b, 0xf2, 0x48, 0x8b, 0xd9, 0xe8, 255 , 255 ,
};
static uint8_t *getActivationStatusFSig = getActivationStatusFSig_;
static size_t getActivationStatusFSigSize = sizeof(getActivationStatusFSig_);

static uint8_t voiceDBVerifyProductKeyFSig_[] = {
  0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
  0x48, 0x8d, 0xac, 0x24, 0x00, 0xff, 0xff, 0xff, 0x48, 0x81, 0xec, 0x00, 0x02, 0x00, 0x00, 255 ,
  255 , 255 , 255 , 255 , 255 , 0x00, 0x48, 0x33, 0xc4, 0x48, 0x89, 0x85, 0xf0, 0x00, 0x00, 0x00,
  0x4d, 0x8b, 0xf0, 0x4c, 0x8b, 0xfa, 0x48, 0x8b, 0xf9, 0x48, 0x89, 0x54, 0x24, 0x48, 0x4c, 0x89,
  0x44, 0x24, 0x50, 0x45, 0x33, 0xed, 0x44, 0x89, 0x6c, 0x24, 0x38, 0xe8, 255 , 255 , 255 , 255 ,
};
static uint8_t *voiceDBVerifyProductKeyFSig = voiceDBVerifyProductKeyFSig_;
static size_t voiceDBVerifyProductKeyFSigSize = sizeof(voiceDBVerifyProductKeyFSig_);

static uint8_t appVerifyProductKeyFSig_[] = {
  0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
  0x48, 0x8d, 0xac, 0x24, 0x00, 0xff, 0xff, 0xff, 0x48, 0x81, 0xec, 0x00, 0x02, 0x00, 0x00, 255 ,
  255 , 255 , 255 , 255 , 255 , 0x00, 0x48, 0x33, 0xc4, 0x48, 0x89, 0x85, 0xf0, 0x00, 0x00, 0x00,
  0x4d, 0x8b, 0xf0, 0x4c, 0x8b, 0xfa, 0x48, 0x8b, 0xf9, 0x48, 0x89, 0x54, 0x24, 0x48, 0x4c, 0x89,
};
static uint8_t *appVerifyProductKeyFSig = appVerifyProductKeyFSig_;
static size_t appVerifyProductKeyFSigSize = sizeof(appVerifyProductKeyFSig_);

static uint8_t setOkFlagFSig_[] = {
  0xcc, 0xc6, 255 , 255 , 255 , 255 , 255 , 0x01, 0xc3,
};
static uint8_t *setOkFlagFSig = setOkFlagFSig_;
static size_t setOkFlagFSigSize = sizeof(setOkFlagFSig_);

static bool blockInternet = true;

static bool debugMode = false;

static char *consoleFile = NULL;

static const char *synthVSupportedVersions =
  "Pro 1.11.0b1\n"
  "Pro 1.11.0b2\n"
  "Pro 1.11.0\n"
  "Pro 1.11.1\n"
  "Pro 1.11.2\n"
  "";

static bool checkSynthVVersion = true;

static bool inDllMain = false;
static unsigned int injectTime = 0;

static bool hasAllocatedConsole = false;
static const char *synthVDetected;

void allocConsoleOnce() {
  if (!hasAllocatedConsole) {
    if (!consoleFile) {
      // Linux 下没有 AllocConsole，直接输出到 stdout
      hasAllocatedConsole = true;
    } else {
      FILE *fp = fopen(consoleFile, "at");
      if (fp) {
        fprintf(fp, "=== 新会话 ===\n");
        fclose(fp);
      }
    }

    hasAllocatedConsole = true;
  }
}

void _Noreturn _die(int line) {
  if (consoleFile) {
    consoleFile = NULL;
    hasAllocatedConsole = false;
  }

  allocConsoleOnce();

  printf("YumeKey Obsidian 遇到了一个致命错误。\n\n");
  printf("请在您的报告中包含以下信息：\n");
  printf("@ lifeCycle: %s\n", _lifeCycle);
  printf("+ line: %d\n", line);
  printf("inDllMain = %s\n", inDllMain ? "true" : "false");
  
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  unsigned int currentTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  printf("timeSinceInject = %u\n", currentTime - injectTime);
  
  printf("Obsidian 版本：%d.%d.%d in Synthesizer V Studio %s\n",
         VER_MAJOR, VER_MINOR, VER_PATCH, synthVDetected ? synthVDetected : "(unknown)");
  
  exit(42);
}

static uint8_t hatoi8(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  die();
}

size_t parseHex(uint8_t *dst, const char *src) {
  size_t i = 0;
  while (*src) {
    *dst++ = hatoi8(*(src + 1)) | (hatoi8(*src) << 4);
    src += 2;
    i++;
  }
  return i;
}

int memCmpWithMask(const void *a, const void *b, size_t n) {
  const unsigned char *x = a;
  const unsigned char *y = b;

  for (size_t i = 0; i < n; i++) {
    if (x[i] != y[i] && x[i] != 255) {
      return x[i] - y[i];
    }
  }
  return 0;
}

void *memMem(const void *haystack, size_t n, const void *needle, size_t m) {
  if (m > n || !m || !n)
    return NULL;
  if (__builtin_expect((m > 1), 1)) {
    const unsigned char *y = haystack;
    const unsigned char *x = needle;
    size_t j = 0;
    size_t k = 1, l = 2;
    if (x[0] == x[1]) {
      k = 2;
      l = 1;
    }
    while (j <= n-m) {
      if (x[1] != y[j+1]) {
        j += k;
      } else {
        if (!memCmpWithMask(x+2, y+j+2, m-2) && (x[0] == y[j] || x[0] == 255))
          return (void*) &y[j];
        j += l;
      }
    }
  } else {
    /* degenerate case */
    return memchr(haystack, ((unsigned char*)needle)[0], n);
  }
  return NULL;
}

void *safeMemMem(void *pvHaystack, size_t szHaystackSize, void *pvNeedle, size_t szNeedleSize) {
  return memMem(pvHaystack, szHaystackSize, pvNeedle, szNeedleSize);
}

typedef void (*GetActivationStatusFn)(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure);
static GetActivationStatusFn orig_getActivationStatus = NULL;

void hook_getActivationStatus(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure) {
  thisPtr->isActivated = true;
  thisPtr->isFree = true;
  thisPtr->hasNoProductKey = true;
  orig_getActivationStatus(thisPtr, success, success);
}

typedef void (*VoiceDBVerifyProductKeyFn)(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure);
static VoiceDBVerifyProductKeyFn orig_voiceDBVerifyProductKey = NULL;

static void hook_voiceDBVerifyProductKey(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure) {
  thisPtr->isActivated = true;
  thisPtr->isFree = true;
  thisPtr->hasNoProductKey = true;
  orig_voiceDBVerifyProductKey(thisPtr, success, success);
}

typedef void (*AppVerifyProductKeyFn)(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure);
static AppVerifyProductKeyFn orig_appVerifyProductKey = NULL;

static void hook_appVerifyProductKey(struct ProductBase *thisPtr, struct FunctionBase *success, struct FunctionBase *failure) {
  thisPtr->isActivated = true;
  thisPtr->isFree = true;
  thisPtr->hasNoProductKey = true;
  printf("产品名称：%s, 产品厂商：%s\n", thisPtr->name, thisPtr->vendor);
  orig_appVerifyProductKey(thisPtr, success, success);
}

// Linux 下拦截网络访问的钩子
typedef void *(*InternetOpenWFn)(const void *, DWORD, const void *, const void *, DWORD);
static InternetOpenWFn orig_InternetOpenW = NULL;

static void *hook_InternetOpenW(const void *lpszAgent, DWORD dwAccessType, const void *lpszProxy, const void *lpszProxyBypass, DWORD dwFlags) {
  if (blockInternet) {
    debug_printf("[Obsidian] 阻止网络访问\n");
    return NULL;
  }
  return orig_InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
}

// Linux 下拦截 socket 连接
typedef int (*SocketFn)(int domain, int type, int protocol);
static SocketFn orig_socket = NULL;

static int hook_socket(int domain, int type, int protocol) {
  if (blockInternet && (domain == AF_INET || domain == AF_INET6)) {
    debug_printf("[Obsidian] 阻止 socket 创建\n");
    errno = EACCES;
    return -1;
  }
  return orig_socket(domain, type, protocol);
}

typedef int (*ConnectFn)(int sockfd, const void *addr, socklen_t addrlen);
static ConnectFn orig_connect = NULL;

static int hook_connect(int sockfd, const void *addr, socklen_t addrlen) {
  if (blockInternet) {
    debug_printf("[Obsidian] 阻止 connect 调用\n");
    errno = EACCES;
    return -1;
  }
  return orig_connect(sockfd, addr, addrlen);
}

#include <sys/socket.h>
#include <errno.h>

static bool initConfig() {
  printf("[Obsidian] 正在从环境变量初始化配置...\n");

  if (getenv("OBSIDIAN_BASE_ADDR")) {
    fptrAddBase = strtoull(getenv("OBSIDIAN_BASE_ADDR"), NULL, 0);
  }

#ifdef DEBUG
  if (getenv("OBSIDIAN_GETACTIVATIONSTATUS_ADDR")) {
    getActivationStatusFPtr = (void *)strtoull(getenv("OBSIDIAN_GETACTIVATIONSTATUS_ADDR"), NULL, 0);
  }

  if (getenv("OBSIDIAN_VOICEDB_VERIFYPRODUCTKEY_ADDR")) {
    voiceDBVerifyProductKeyFPtr = (void *)strtoull(getenv("OBSIDIAN_VOICEDB_VERIFYPRODUCTKEY_ADDR"), NULL, 0);
  }

  if (getenv("OBSIDIAN_APP_VERIFYPRODUCTKEY_ADDR")) {
    appVerifyProductKeyFPtr = (void *)strtoull(getenv("OBSIDIAN_APP_VERIFYPRODUCTKEY_ADDR"), NULL, 0);
  }

  if (getenv("OBSIDIAN_SETOKFLAG_ADDR")) {
    setOkFlagFPtr = (void *)strtoull(getenv("OBSIDIAN_SETOKFLAG_ADDR"), NULL, 0);
  }
#else
  if (getenv("OBSIDIAN_GAFKALO")) {
    getActivationStatusFPtr = (void *)strtoull(getenv("OBSIDIAN_GAFKALO"), NULL, 0);
  }

  if (getenv("OBSIDIAN_VODPKO")) {
    voiceDBVerifyProductKeyFPtr = (void *)strtoull(getenv("OBSIDIAN_VODPKO"), NULL, 0);
  }

  if (getenv("OBSIDIAN_AOVEKAO")) {
    appVerifyProductKeyFPtr = (void *)strtoull(getenv("OBSIDIAN_AOVEKAO"), NULL, 0);
  }

  if (getenv("OBSIDIAN_SAOSKFP")) {
    setOkFlagFPtr = (void *)strtoull(getenv("OBSIDIAN_SAOSKFP"), NULL, 0);
  }
#endif

  if (getenv("OBSIDIAN_BLOCK_INTERNET")) {
    blockInternet = strtoull(getenv("OBSIDIAN_BLOCK_INTERNET"), NULL, 0) != 0;
  }

  if (getenv("OBSIDIAN_DEBUG_MODE")) {
    debugMode = atoi(getenv("OBSIDIAN_DEBUG_MODE"));
  }

  if (getenv("OBSIDIAN_CONSOLE_FILE")) {
    consoleFile = getenv("OBSIDIAN_CONSOLE_FILE");
  }

  if (getenv("OBSIDIAN_CHECK_SYNTHV_VERSION")) {
    checkSynthVVersion = strtoull(getenv("OBSIDIAN_CHECK_SYNTHV_VERSION"), NULL, 0) != 0;
  }

  if (debugMode) {
    allocConsoleOnce();
  }

  printf("[Obsidian] 配置已加载。\n");

  return true;
}

static bool initHooks() {
  GumInterceptor *interceptor = gum_interceptor_obtain();
  
  printf("[Obsidian] 正在初始化钩子...\n");
  
  // 拦截网络相关函数
  if (blockInternet) {
    gum_interceptor_begin_transaction(interceptor);
    
    void *socket_addr = dlsym(RTLD_NEXT, "socket");
    if (socket_addr) {
      gum_interceptor_attach(interceptor, socket_addr, 
                            GUM_INVOCATION_LISTENER(hook_socket), 
                            (gpointer *)&orig_socket,
                            GUM_ATTACH_FLAGS_NONE);
    }
    
    void *connect_addr = dlsym(RTLD_NEXT, "connect");
    if (connect_addr) {
      gum_interceptor_attach(interceptor, connect_addr,
                            GUM_INVOCATION_LISTENER(hook_connect),
                            (gpointer *)&orig_connect,
                            GUM_ATTACH_FLAGS_NONE);
    }
    
    gum_interceptor_end_transaction(interceptor);
  }
  
  return true;
}

__attribute__((constructor))
static void on_library_load(void) {
  gum_init_embedded();
  
  inDllMain = true;
  
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  injectTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  
  LIFECYCLE("INIT_CONFIG");
  if (!initConfig()) die();
  
  LIFECYCLE("INIT_HOOKS");
  if (!initHooks()) die();
  
  LIFECYCLE("RUNNING");
  printf("[Obsidian] 已就绪。\n");
  inDllMain = false;
}

__attribute__((destructor))
static void on_library_unload(void) {
  printf("[Obsidian] 正在卸载...\n");
  gum_deinit_embedded();
}
