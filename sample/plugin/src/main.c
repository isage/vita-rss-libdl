#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <string.h>
#include <math.h>

int testplugTest(int val)
{
    sceClibPrintf("WOW %d!\n", val);
    return val;
}


int module_start() {
    return SCE_KERNEL_START_SUCCESS;
}

int module_stop() {
    return SCE_KERNEL_STOP_SUCCESS;
}

void _start() __attribute__ ((weak, alias ("module_start")));
