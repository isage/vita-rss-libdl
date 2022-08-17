#ifndef TAIUTILS_H
#define TAIUTILS_H

#include <psp2/kernel/modulemgr.h>
#include <taihen.h>

int module_get_export(const char *module_name, int32_t library_nid, uint32_t func_nid, void *func_dst);

#endif