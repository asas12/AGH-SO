#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

int main(){
void* handle = dlopen("./libzad3_shared.so", RTLD_LAZY);
if(!handle){printf("Can't find lib\n"); return 0;}


    void (*fun)(void) = dlsym(handle, "createArrayStatic");
    fun();
    char** (*funar)(int, int) = dlsym(handle, "createArrayDynamic");
    char** ar = funar(3,4);
    ar[0][0]='a';
    ar[0][1]='\0';
    printf("%s\n", ar[0]);
    char* SA = (char*) dlsym(handle, "StaticArray");
    *SA = 'A';
    printf("%c",*SA);    
//int* SoB = (int*) dlsym(handle, "SoBlocks");
    //int SoBlocks = *SoB;
   // printf("%d", SoB);
   // printf("%d", SoBlocks);

return 0;
}

