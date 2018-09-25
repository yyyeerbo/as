#ifndef ___AS__H
#define ___AS__H

#if defined(__KERNEL__)
#include <linux/types.h>
#include <linux/kernel.h>

extern struct file *get_empty_filp(void);
#endif

struct mmap_info {
    unsigned long addr;
    unsigned long len;
    unsigned long prot;
    unsigned long flags;
    unsigned long fd;
    unsigned long offset;
};

#endif
