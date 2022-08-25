#ifndef MODULEREF_H
#define MODULEREF_H

#include <stdint.h>
#include <psp2common/types.h>

#define MAIN_MODULE_ID -1

typedef struct module_ref {
   uint32_t ref_count;
   SceUID module_id;
   char path[256];

   struct module_ref *prev;
   struct module_ref *next;
} module_ref_t;

#endif

