#ifndef _DLFCN_H
#define _DLFCN_H 1

extern void *dlopen (const char *__file, int __mode);

extern int dlclose (void *__handle);

extern void *dlsym (void *__handle, const char *__name);

extern char *dlerror (void);

#endif