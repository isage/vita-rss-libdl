#ifndef _DLFCN_H
#define _DLFCN_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY 0x00001
#define RTLD_NOW 0x00002
#define RTLD_GLOBAL 0x00100
#define RTLD_LOCAL 0x0

extern void *dlopen (const char *__file, int __mode);

extern int dlclose (void *__handle);

extern void *dlsym (void *__handle, const char *__name);

extern char *dlerror (void);
  
#ifdef __cplusplus
}
#endif

#endif
