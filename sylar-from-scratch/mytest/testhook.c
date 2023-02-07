#define _GNU_SOURCE
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
typedef void* (*malloc_func_t)(size_t size);
typedef void (*free_func_t)(void *ptr);
 
// 这两个指针用于保存libc中的malloc和free的地址
malloc_func_t sys_malloc = NULL;
free_func_t sys_free = NULL;
 
// 重定义malloc和free，在这里重定义会导致libc中的同名符号被覆盖
// 这里不能调用带缓冲的printf接口，否则会出段错误
void *malloc(size_t size) {
    // 先调用标准库里的malloc申请内存，再记录内存分配信息，这里只是简单地将内存地址和长度打印出来
    void *ptr = sys_malloc(size);
    fprintf(stderr, "malloc: ptr=%p, length=%ld\n", ptr, size);
    return ptr;
}
void free(void *ptr) {
    // 打印内存释放信息，再调用标准库里的free释放内存
    fprintf(stderr, "free: ptr=%p\n", ptr);
    sys_free(ptr);
}
 
int main() {
    // 通过dlsym找到标准库中的malloc和free的符号地址
    sys_malloc = dlsym(RTLD_NEXT, "malloc");
    assert(dlerror() == NULL);
    sys_free = dlsym(RTLD_NEXT, "free");
    assert(dlerror() == NULL);
 
    char *ptrs[5];
 
    for(int i = 0; i < 5; i++) {
        ptrs[i] = malloc(100 + i);
        memset(ptrs[i], 0, 100 + i);
    }
     
    for(int i = 0; i < 5; i++) {
        free(ptrs[i]);
    }
    return 0;
}