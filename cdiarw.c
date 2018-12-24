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
#include <linux/proc_fs.h>
#include "cdiarw.h"

#define DRIVERNAME "CDI-ARW"
#define PROC_NAME "cdi-arw"

static unsigned int cdiarw_major;
static struct cdev cdiarw_cdev;
const unsigned int MINOR_NUM = 2;
struct arw_param p;


static long cdiarw_ioctl(struct file *filp, unsigned int cmd, unsigned long param)
{
	if(copy_from_user(&p, (void __user *)param, sizeof(struct arw_param))){
		pr_info("cdiarw: copy_from_user fail" );
		return -EFAULT;
	}

	pr_info("ioctl called: copy from 0x%lx to 0x%lx length %ld", (unsigned long)p.src, (unsigned long)p.dest, (unsigned long)p.len);
	switch(cmd){
		case CDIARW_WRITE:
		if(copy_from_user(p.dest, p.src, p.len))
			return -EFAULT;
		break;
		case CDIARW_READ:
		if(copy_to_user(p.dest, p.src, p.len))
			return -EFAULT;
		break;

		default:
		pr_info("unknown cmd: %x", cmd);
		return -EFAULT;
	}
	return 0L;
}


static ssize_t cdiarw_proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	if(copy_from_user(&p, (void __user *)buf, sizeof(struct arw_param))){
		pr_info("cdiarw: copy_from_user fail" );
		return -EFAULT;
	}
	pr_info("proc_write called: copy from 0x%lx to 0x%lx length %ld", (unsigned long)p.src, (unsigned long)p.dest, (unsigned long)p.len);
	if(copy_from_user(p.dest, p.src, p.len))
		return -EFAULT;

	return 0L;
}


struct file_operations s_cdiarw_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = cdiarw_ioctl,
    .compat_ioctl   = cdiarw_ioctl,
};

struct file_operations s_cdiarw_proc_fops = {
	.owner = THIS_MODULE,
    .write = cdiarw_proc_write
};


static int __init cdiarw_init(void) {
	dev_t dev;
	struct proc_dir_entry *entry;

	pr_info("cdiarw: initializong CDI arw module... ");
	
	if(0!=alloc_chrdev_region(&dev, 0, MINOR_NUM, DRIVERNAME))
	{
		pr_err("cdiarw: alloc_chrdev_region fail\n");
		return -1;
	}

	cdiarw_major = MAJOR(dev);
	cdev_init(&cdiarw_cdev, &s_cdiarw_fops);

	if(0 != cdev_add(&cdiarw_cdev, dev, MINOR_NUM)){
		pr_err("cdiarw: cdev_add fail\n");
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

    entry = proc_create(PROC_NAME, S_IRUGO | S_IWUGO, NULL, &s_cdiarw_proc_fops);
	if (entry == NULL) {
        pr_err("cdiarw: proc creation fail\n");
		unregister_chrdev_region(dev, MINOR_NUM);
        return -ENOMEM;
    }

	return 0;
}

static void __exit cdiarw_exit(void) {
	dev_t dev;
	pr_info("cdiarw: removing CDI arw device.");
	dev = MKDEV(cdiarw_major, 0);
	cdev_del(&cdiarw_cdev);
	unregister_chrdev_region(dev, MINOR_NUM);
	remove_proc_entry(PROC_NAME, NULL);
}

module_init(cdiarw_init);
module_exit(cdiarw_exit);
MODULE_AUTHOR("nk <nk@vul.jp>");
MODULE_LICENSE("GPL");
