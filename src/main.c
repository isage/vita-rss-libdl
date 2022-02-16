#include <stdio.h>
#include <stdlib.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <taihen.h>
#include "sha256.h"

#define DLERR_MAX 256

static char _dl_err[DLERR_MAX];
static int _dl_err_set = 0;

#define set_dl_error(...) \
    do { \
        snprintf(_dl_err, DLERR_MAX, __VA_ARGS__); \
        sceClibPrintf(__VA_ARGS__); \
        _dl_err_set = 1; \
    } while (0)


void *dlopen(const char *__file, int __mode)
{
    int ret = sceKernelLoadStartModule(__file, 0, NULL, 0, NULL, NULL);
    if (ret < 0)
    {
        set_dl_error("[libdl] sceKernelLoadStartModule(%s, 0, NULL, 0, NULL, NULL) error: 0x%08x\n", __file, ret);
        return NULL;
    }
    return (void*)ret;
}

int dlclose(void *__handle)
{
    sceKernelStopUnloadModule((int)__handle, 0, NULL, 0, NULL, NULL);
}

void *dlsym(void *__handle, const char *__name)
{
    SceKernelModuleInfo info = {0};
    info.size = sizeof(SceKernelModuleInfo);
    int ret = sceKernelGetModuleInfo((int)__handle, &info);
    if (ret < 0)
    {
        set_dl_error("[libdl] sceKernelGetModuleInfo(0x%08x, &info) error: 0x%08x\n", __handle, ret);
        return NULL;
    }

    size_t len = strlen(__name);
    uint32_t nid = sha256_32_vector(1, (uint8_t**)&__name, (size_t*)&len);

    uintptr_t func;
    ret = taiGetModuleExportFunc(info.module_name, TAI_ANY_LIBRARY, nid, &func);
    if (ret == 0)
    {
        return (void*)func;
    }
    set_dl_error("[libdl] taiGetModuleExportFunc(%s, TAI_ANY_LIBRARY, 0x%08x, &func) error: 0x%08x\n", info.module_name, nid, ret);
    return NULL;
}

char *dlerror (void)
{
    if (_dl_err_set)
    {
        _dl_err_set = 0;
        return _dl_err;
    }
    return NULL;
}
