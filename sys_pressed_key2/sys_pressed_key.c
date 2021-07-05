#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

volatile int key_pressed_count = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev key_pressed_cdev;
struct kobject *kobj_ref;

void increment(void);

static ssize_t sysfs_show(struct kobject *kobj,
						  struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj,
						   struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute key_pressed_attr = __ATTR(key_pressed_count, 0440, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj,
						  struct kobj_attribute *attr, char *buf)
{
	pr_info("Sysfs - Read!!!\n");
	return sprintf(buf, "%d", key_pressed_count);
}

void increment()
{
	key_pressed_count++;
	return;
}

/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj,
						   struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("Você não pode escrever aqui cara!!!\n");
	return;
}

/*
** This function will be called when we open the Device file
*/
static int key_pressed_open(struct inode *inode, struct file *file)
{
	pr_info("Device File Opened...!!!\n");
	return 0;
}

/*
** This function will be called when we close the Device file
*/
static int key_pressed_release(struct inode *inode, struct file *file)
{
	pr_info("Device File Closed...!!!\n");
	return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t key_pressed_read(struct file *filp,
								char __user *buf, size_t len, loff_t *off)
{
	pr_info("Read function\n");
	return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t key_pressed_write(struct file *filp,
								 const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Write Function\n");
	return len;
}

static struct file_operations fops =
	{
		.owner = THIS_MODULE,
		.read = key_pressed_read,
		.write = key_pressed_write,
		.open = key_pressed_open,
		.release = key_pressed_release,
};

static int __init pressed_key_init(void)
{
	if ((alloc_chrdev_region(&dev, 0, 1, "key_pressed_Dev")) < 0)
	{
		pr_info("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	cdev_init(&key_pressed_cdev, &fops);

	if ((cdev_add(&key_pressed_cdev, dev, 1)) < 0)
	{
		pr_info("Cannot add the device to the system\n");
		goto r_class;
	}

	if ((dev_class = class_create(THIS_MODULE, "key_pressed_class")) == NULL)
	{
		pr_info("Cannot create the struct class\n");
		goto r_class;
	}

	if ((device_create(dev_class, NULL, dev, NULL, "key_pressed_device")) == NULL)
	{
		pr_info("Cannot create the Device 1\n");
		goto r_device;
	}

	kobj_ref = kobject_create_and_add("key_pressed", NULL);

	if (sysfs_create_file(kobj_ref, &key_pressed_attr.attr))
	{
		pr_err("Cannot create sysfs file......\n");
		goto r_sysfs;
	}

	pr_info("Device Driver Insert...Done!!!\n");
	increment();
	increment();
	return 0;

r_sysfs:
	kobject_put(kobj_ref);
	sysfs_remove_file(NULL, &key_pressed_attr.attr);

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	cdev_del(&key_pressed_cdev);
	return -1;
}

static void __exit pressed_key_exit(void)
{
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &key_pressed_attr.attr);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&key_pressed_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device Driver Remove...Done!!!\n");
}

module_init(pressed_key_init);
module_exit(pressed_key_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jose Lucas De Melo Costa <melocosta.joselucas@gmail.com>");
MODULE_DESCRIPTION("Counting keyboard presses");
