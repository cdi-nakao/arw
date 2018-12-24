# CDI-ARW

This is a kernel module to copy any value at any address you want.

## how to use

### 0. build

```bash
$ sudo apt install linux-headers-[your kernel version]
$ make
```

### 1. load module and make node
```bash
$ sudo insmod cdiarw.ko
$ sudo mknod c `cat /proc/devices | grep -i cdi-arw|cut -d" " -f1` 0 /dev/cdiarw0
```

### 2. attack!

sample code
```C
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "cdiarw.h"

int main(void)
{
    char msg[64];
    char msg2[64];
    struct arw_param p;
    int i, fd;

    strncpy(msg, "hello world", sizeof(msg));
    strncpy(msg2, "hello modified world", sizeof(msg));
    
    p.src = msg2; // set destination address
    p.dest= msg;  // set source address
    p.len = strlen(msg2);  // set copy length

    if(-1 == (fd = open("/dev/arw0", O_RDWR))){
	    fprintf(stderr, "%s\n", strerror(errno));
	    return -1;
    }

    for(i=0; i<5; i++){
        printf("%s\n", msg);
        sleep(1);
        if(i==2){
            // call copy function using ioctl syscall
            ioctl(fd, CDIARW_WRITE, &p); 
        }
    }
    return 0;
}
```
result:
```
$ sudo ./a.out 
hello world
hello world
hello world
hello modified world
hello modified world
```