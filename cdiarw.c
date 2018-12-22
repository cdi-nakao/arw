/*
 * Copyright (C) 2018 Cyber Defense Institute, Inc.	
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 
 * as published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>


struct arw_param {
	void *src;
	void *dest;
	ssize_t len;
};

#define CDIARW_MAGIC 0xCE
#define CDIARW_WRITE _IOW(CDIARW_MAGIC, 0, struct arw_param)
#define CDIARW_READ _IOR(CDIARW_MAGIC, 1, struct arw_param)

static unsigned int cdiarw_major;
static struct cdev cdiarw_cdev;
const unsigned int MINOR_NUM = 2;
struct arw_param val;

static long cdiarw_ioctl(struct file *filp, unsigned int cmd, unsigned long param)
{
	pr_info("ioctl called\n");
	switch(cmd){
		case CDIARW_WRITE:
		if(copy_from_user(&val, (void*)param, sizeof(struct arw_param)))
			return -EFAULT;
		break;

		case CDIARW_READ:
		if(copy_to_user((void*)param, &val, sizeof(struct arw_param)))
			return -EFAULT;
		break;

		default:
		return -EFAULT;
	}
	return 0L;
}


struct file_operations s_cdiarw_fops = {
	.unlocked_ioctl = cdiarw_ioctl,
    .compat_ioctl   = cdiarw_ioctl
};


static int __init cdiarw_init(void) {
	dev_t dev;
	pr_info("initializong CDI arw module... ");
	
	if(0!=alloc_chrdev_region(&dev, 0, MINOR_NUM, "CDI-ARW"))
	{
		pr_err("alloc_chrdev_region fail\n");
		return -1;
	}

	cdiarw_major = MAJOR(dev);
	cdev_init(&cdiarw_cdev, &s_cdiarw_fops);
	cdiarw_cdev.owner = THIS_MODULE;

	if(0 != cdev_add(&cdiarw_cdev, dev, MINOR_NUM)){
		pr_err("cdev_add fail\n");
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}
	return 0;
}

static void __exit cdiarw_exit(void) {
	dev_t dev;
	pr_info("removing CDI arw device");
	dev = MKDEV(cdiarw_major, 0);
	cdev_del(&cdiarw_cdev);
	unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(cdiarw_init);
module_exit(cdiarw_exit);
MODULE_AUTHOR("nk <nk@vul.jp>");
MODULE_LICENSE("GPL");
