#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <psp2common/types.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include "taiutils.h"
#include "moduleref.h"

typedef struct SceSblDmac5HashTransformParam {
   void* src;
   void* dst;
   SceSize size;
   uint32_t unk_C;
   uint32_t unk_10;
   void* iv;
} SceSblDmac5HashTransformParam;

int sceSblDmac5HashTransform(SceSblDmac5HashTransformParam *pParam, SceUInt32 command, SceUInt32 flags);

module_ref_t *first = NULL;
module_ref_t *last = NULL;

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
    // TODO: empty path = main module
    int module_id;
    char fullpath[256];

    if (__file == NULL) // main module
    {
        // since we don't load/unload it, we don't care about refcount
        module_ref_t *module = (module_ref_t*) malloc(sizeof(module_ref_t));
        module->ref_count = 1;
        module->module_id = MAIN_MODULE_ID;
        sceClibMemset(module->path, 0, 256);
        return (void*)module;
    }
    else
    {
        char* pathret = realpath(__file, fullpath);

        if (pathret == NULL)
        {
            set_dl_error("[libdl] dlopen(%s, %d) realpath() error: %x\n", __file, __mode, errno);
            return NULL;
        }

        module_ref_t* current = first;
        while(current != NULL && strcmp(current->path, fullpath) != 0)
        {
            current = current->next;
        }
        if (current != NULL)
        {
            current->ref_count++;
            return (void*)current;
        }

        module_id = sceKernelLoadStartModule(fullpath, 0, NULL, 0, NULL, NULL);
        if (module_id < 0)
        {
            set_dl_error("[libdl] sceKernelLoadStartModule(%s, 0, NULL, 0, NULL, NULL) error: 0x%08x\n", __file, module_id);
            return NULL;
        }
    }

    module_ref_t *module = (module_ref_t*) malloc(sizeof(module_ref_t));
    module->ref_count = 1;
    module->module_id = module_id;
    strncpy(module->path, fullpath, 256);

    if (last)
        last->next = module;
    else
        first = module;

    module->prev = last;
    module->next = NULL;
    last = module;

    return (void*)module;
}

int dlclose(void *__handle)
{
    module_ref_t* module = (struct module_ref *)__handle;
    if (module)
    {
        if (module->module_id != MAIN_MODULE_ID)
        {
            module->ref_count--;
            if (module->ref_count == 0)
            {
                sceKernelStopUnloadModule(module->module_id, 0, NULL, 0, NULL, NULL);

                if(first == module)
                   first = module->next;
                else if (module->prev)
                    module->prev->next = module->next;

                if(last == module)
                   last = module->prev;
                else if (module->next)
                    module->next->prev = module->prev;

                free(module);
            }
        }
        else
        {
            free(module);
        }
        return 0;
    }
    set_dl_error("[libdl] dlclose(0x%08x) error: invalid handle\n", __handle);
    return -1;
}

void *dlsym(void *__handle, const char *__name)
{
    module_ref_t* module = (struct module_ref *)__handle;
    char* module_name;
    int ret;

    SceKernelModuleInfo info = {0};
    info.size = sizeof(SceKernelModuleInfo);

    if (__handle !=NULL && module->module_id != MAIN_MODULE_ID)
    {
        ret = sceKernelGetModuleInfo(module->module_id, &info);
        if (ret < 0)
        {
            set_dl_error("[libdl] dlsym(0x%08x, %s): sceKernelGetModuleInfo() error: 0x%08x\n", __handle, __name, ret);
            return NULL;
        }
        module_name = info.module_name;
    }
    else
    {
        module_name = TAI_MAIN_MODULE;
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

    ret = module_get_export(module_name, TAI_ANY_LIBRARY, nid, &func);

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

        ret = module_get_export(module_name, TAI_ANY_LIBRARY, nid, &func);
        if (ret == 0)
        {
            return (void*)func;
        }
    }
    set_dl_error("[libdl] module_get_export(%s, TAI_ANY_LIBRARY, 0x%08x, &func) error: 0x%08x\n", module_name, nid, ret);
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
