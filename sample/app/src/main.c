#include <stdio.h>
#include <stdlib.h>
#include <psp2/kernel/clib.h>

#include <dlfcn.h>

// function, exported from app itself
void maintestfunc(int val)
{
    sceClibPrintf("Main func called with: %d\n", val);
}


int main(int argc, char *argv[]) 
{
  maintestfunc(1);

  sceClibPrintf("loading lib\n");
  void* h = dlopen("app0:/module/testplug.suprx",0);
  sceClibPrintf("loaded lib: 0x%08x\n", h);

  sceClibPrintf("loading lib again\n");
  void* h2 = dlopen("app0:/module/testplug.suprx",0); // instead of loading again should increase refcount
  sceClibPrintf("loaded lib again: 0x%08x\n", h);

  sceClibPrintf("unloading lib once\n");
  dlclose(h2);  // decreases refcount, doesn't unload, since refcount still > 0 after that.

  int     (*fptr)(int val);
  sceClibPrintf("resolving testplugTest from lib\n");
  fptr = dlsym(h, "testplugTest");

  if (fptr!=NULL)
  {
    sceClibPrintf("got 0x%08x\n", fptr);
    fptr(1488);
  }

  sceClibPrintf("loading main\n");
  void* h3 = dlopen(NULL,0);
  sceClibPrintf("loaded main\n");

  void     (*f2ptr)(int val);
  sceClibPrintf("resolving maintestfunc from app\n");
  f2ptr = dlsym(h3, "maintestfunc");

  if (f2ptr!=NULL)
  {
    sceClibPrintf("got 0x%08x\n", f2ptr);
    f2ptr(2);
  }

  return 0;
}
