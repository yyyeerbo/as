#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>

#include "as.h"

#define __NR_as_create  (333)
#define __NR_as_mmap    (334)
#define __NR_as_munmap  (335)
#define __NR_as_mprotect    (336)
#define __NR_as_switch_mm   (337)
#define __NR_as_destroy     (338)
#define __NR_as_copy        (339)

#define SYS_as_create   __NR_as_create
#define SYS_as_mmap     __NR_as_mmap
#define SYS_as_munmap   __NR_as_munmap
#define SYS_as_mprotect __NR_as_mprotect
#define SYS_as_switch_mm    __NR_as_switch_mm
#define SYS_as_destroy      __NR_as_destroy
#define SYS_as_copy         __NR_as_copy

int main(int argc, char *argv[])
{
    int fd, bfd, ret;
    struct mmap_info info;
    void *heap;
    long sz;
    char buf[20];

    memset(buf, 0, 20);
    fprintf(stderr, "as_copy\n");
    fd = syscall(SYS_as_copy);
    if (fd < 0) {
        perror("as_copy\n");
        exit(-1);
    }

    /* Get some linear address region to mmap */
    heap = sbrk(0);
    fprintf(stderr, "heap start at: [%lx]\n", heap);
    sz = sysconf(_SC_PAGESIZE);

//    pause();

    info.addr = ((long)heap & ~(sz -1)) + 100 * sz;
    info.len  = sz;
    info.prot = PROT_READ | PROT_WRITE;
    info.flags = MAP_SHARED | MAP_FIXED;
    info.offset = 0;

    bfd = open("./test.txt", O_RDWR | O_CREAT);
    if (bfd < 0) {
        perror(strerror(errno));
        syscall(SYS_as_destroy, fd);
        exit(-1);
    }
    info.fd = bfd;

    /* try to do mmap on new as */
    fprintf(stderr, "as_mmap\n");
    heap = (void *)syscall(SYS_as_mmap, fd, &info);
    if (heap == MAP_FAILED) {
        perror("as_mmap\n");
        close(bfd);
        syscall(SYS_as_destroy, fd);
        exit(-1);
    }
    fprintf(stderr, "maped address: %lx\n", (long)heap);

    fprintf(stderr, "as_munmap\n");
    ret = syscall(SYS_as_munmap, fd, (long)heap, sz);
    if (ret) {
        perror("as_munmap\n");
        close(bfd);
        syscall(SYS_as_destroy, fd);
        exit(-1);
    }

    fprintf(stderr, "as_mmap again\n");
    /* mmap onto a different address */
    info.addr += 100 * sz;
    /* do mmap again.. */
    heap = (void *)syscall(SYS_as_mmap, fd, &info);
    if (heap == MAP_FAILED) {
        perror("as_mmap again\n");
        close(bfd);
        syscall(SYS_as_destroy, fd);
        exit(-1);
    }
    fprintf(stderr, "maped address again: %lx\n", heap);

    /* try to switch mm */
    fprintf(stderr, "as_switch_mm\n");
    ret = syscall(SYS_as_switch_mm, fd);
    if (ret) {
        perror("as_switch_mm\n");
        close(bfd);
        syscall(SYS_as_destroy, fd);
        exit(-1);
    }

    /* try to access some data */
    memcpy(buf, heap, 19);
    fprintf(stderr, "file content: %s\n", buf);
    close(bfd);
//    syscall(SYS_as_destroy, fd);
    close(fd);
    return 0;
}
