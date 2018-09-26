# as
address space operation support on host

***patch based on 4.17.19***

**as\_create**: create empty address space. almost useless map everything
                by yourself(eg. execv implement in userspace)

**as\_copy**: copy current address space, no cow.

**as\_mmap**: mmap on specific mm\_struct instead of current-\>{mm, active\_mm}

**as\_munmap**: like mmap, but is unmap

**as\_mprotect**: mprotect on specific mm\_struct

**as\_switch\_mm**: switch to address space prepared by previous steps.

**as\_destroy**: destroy address space. It is equivalent to close. should be 
                 deleted sooner or later
