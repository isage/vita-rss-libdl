#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psp2common/types.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include "taiutils.h"

typedef struct SceSblDmac5HashTransformParam {
   void* src;
   void* dst;
   SceSize size;
   uint32_t unk_C;
   uint32_t unk_10;
   void* iv;
} SceSblDmac5HashTransformParam;

int sceSblDmac5HashTransform(SceSblDmac5HashTransformParam *pParam, SceUInt32 command, SceUInt32 flags);


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
        set_dl_error("[libdl] dlsym(0x%08x, %s): sceKernelGetModuleInfo() error: 0x%08x\n", __handle, __name, ret);
        return NULL;
    }

    size_t len = strlen(__name);

    uint8_t src[0x40 + 0x3F];
    uint8_t dst[0x20];

    void *p = (void*)((uintptr_t)(src + 0x3F) & ~0x3F);

    snprintf((char*)p, 64, __name);

    SceSblDmac5HashTransformParam param = {0};
    param.src = p;
    param.dst = dst;
    param.size = strlen(__name);

    ret = sceSblDmac5HashTransform(&param, 0x13, 0x000);

    if (ret < 0)
    {
        set_dl_error("[libdl] sceSblDmac5HashTransform(sha256) error: 0x%08x\n", ret);
        return NULL;
    }

    uint32_t nid = (dst[0] << 24) | (dst[1] << 16) | (dst[2] << 8) | dst[3];

    uintptr_t func;

    ret = module_get_export(info.module_name, TAI_ANY_LIBRARY, nid, &func);

    if (ret == 0)
    {
        return (void*)func;
    }

    if (ret == 0x90010002) // try sha1 instead
    {
        ret = sceSblDmac5HashTransform(&param, 0x03, 0x000);
        if (ret < 0)
        {
            set_dl_error("[libdl] sceSblDmac5HashTransform(sha1) error: 0x%08x\n", ret);
            return NULL;
        }
        nid = (dst[0] << 24) | (dst[1] << 16) | (dst[2] << 8) | dst[3];

        ret = module_get_export(info.module_name, TAI_ANY_LIBRARY, nid, &func);
        if (ret == 0)
        {
            return (void*)func;
        }
    }
    set_dl_error("[libdl] module_get_export(%s, TAI_ANY_LIBRARY, 0x%08x, &func) error: 0x%08x\n", info.module_name, nid, ret);
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
