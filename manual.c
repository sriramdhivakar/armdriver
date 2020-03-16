#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/current.h>

#define CHAR_NAME "sriram"
#define MAX_SIZE 2048
#define MAJORNO 300
#define MINORNO 0

static dev_t first;
static int count = 1;
static struct cdev *mydev;
static char *kbuf;
static int inuse = 0;
static int balance = 0;

static int open(struct inode *inode, struct file *filp){
	inuse++;
	printk(KERN_INFO"This device is used by these many users %d\n",inuse);
	printk(KERN_INFO"Process name is %s,Process id is %d,Process state is %ld\n",current->comm,current->pid,current->state);
	printk(KERN_INFO"Majorno is %d,Minorno is %d\n",imajor(inode),iminor(inode));
	printk(KERN_INFO"The Refno is %d\n",module_refcount(THIS_MODULE));
	return 0;
}

static ssize_t write(struct file *filp, const char __user *buf, size_t ln, loff_t *pos) {
	int nbyte,byte_do_to;
	balance = MAX_SIZE - *pos;
	if(ln < balance)
	{
		byte_do_to = ln;
	}
	else
	{
		byte_do_to = balance;
	}

	if(byte_do_to == 0)
	{
		printk(KERN_ERR"This device is reached the end\n");
		return -ENOSPC;
	}

	nbyte = byte_do_to - copy_from_user(kbuf +(*pos),buf,byte_do_to);
	*pos +=	nbyte;
	return nbyte;
}

static ssize_t read(struct file *filp, char __user *buf, size_t ln, loff_t *pos){
	int nbyte,byte_do_to;
	balance = MAX_SIZE - *pos;
	if(ln < balance)
	{
		byte_do_to = ln;
	}
	else
	{
		byte_do_to = balance;
	}

	if(byte_do_to == 0)
	{
		printk(KERN_ERR"This device is reached the end\n");
		return -ENOSPC;
	}

	nbyte = byte_do_to - copy_to_user(buf,kbuf +(*pos),byte_do_to);
	*pos +=	nbyte;
	return nbyte;
}

static int close(struct inode *inode, struct file *filp){
	printk(KERN_INFO"This device is ejected from the kernel\n");
	return 0;
}
static struct file_operations f_ops = {
	.owner	=	THIS_MODULE,
	.open	=	open,
	.write	=	write,
	.read	=	read,
	.release=	close,
};

static int __init start(void){
	first = MKDEV(MAJORNO,MINORNO);
	if(register_chrdev_region(first,count,CHAR_NAME) < 0)
	{
		printk(KERN_ERR"This device is not register in the kernel\n");
		return -EINVAL;
	}
	mydev = cdev_alloc();
	cdev_init(mydev,&f_ops);
	if(cdev_add(mydev,first,count) < 0)
	{
		printk(KERN_ERR"This device is not register in the cdev\n");
		return -EINVAL;
	}
	kbuf = (char*)kzalloc(MAX_SIZE,GFP_KERNEL);
	printk(KERN_INFO"This device is register with the name is %s\n",CHAR_NAME);
	printk(KERN_INFO"Majorno is %d,Minorno is %d\n",MAJOR(first),MINOR(first));
	printk(KERN_INFO"Process name is %s,Process id is %d,Process state is %ld\n",current->comm,current->pid,current->state);
	return 0;
}

static void __exit stop(void)
{
	kfree(kbuf);
	cdev_del(mydev);
	unregister_chrdev_region(first,count);
	printk(KERN_INFO"This device is removed from the kernel\n");
}

module_init(start);
module_exit(stop);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("V0.1");
MODULE_AUTHOR("sriramdhivakar.M");
MODULE_DESCRIPTION("This module is based on the manual adding of the character device driver");
