#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

// Define the IRQ line number and the device number
static int irq = 28, dev = 0xaa;

// Define the global variable to count the keyboard presses
volatile int key_pressed_count = 0;

// Define the kobject pointer
struct kobject *kobj_ref;

// Functions prototypes
void increment(void);
static ssize_t sysfs_show(struct kobject *kobj,
                          struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj,
                           struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute key_pressed_attr = __ATTR(key_pressed_count, 0440, sysfs_show, sysfs_store);

// Increment the key_pressed_count variable
void increment()
{
    key_pressed_count++;
    return;
}

// Sysfs function when the file is read
static ssize_t sysfs_show(struct kobject *kobj,
                          struct kobj_attribute *attr, char *buf)
{
    pr_info("\n\n[KERNEL]  Leitura do arquivo sysfs\n\n");
    return sprintf(buf, "%d\n", key_pressed_count);
}

// When we try to write the sysfs file
static ssize_t sysfs_store(struct kobject *kobj,
                           struct kobj_attribute *attr, const char *buf, size_t count)
{
    pr_info("Você não pode escrever aqui cara!!!\n");
    return;
}

// The interrupt handler
static irq_handler_t keyboard_handler(int irq, void *dev)
{
    increment();
    return IRQ_NONE;
}

// Driver init function
static int keyboard_pressed_init(void)
{

    pr_info("[KERNEL] %s: Inicializacao do driver\n\n", __func__);
    int res = request_irq(irq, (irq_handler_t)keyboard_handler, IRQF_SHARED, "my_keyboard_handler", &dev);

    kobj_ref = kobject_create_and_add("key_pressed", NULL);

    if (sysfs_create_file(kobj_ref, &key_pressed_attr.attr))
    {
        pr_err("Nao foi possivel criar o arquivo sysfs......\n");
        goto r_sysfs;
    }

    pr_info("\n\n[KERNEL]  Driver do dispositivo inserido!!!\n\n");

    return 0;

r_sysfs:
    kobject_put(kobj_ref);
    sysfs_remove_file(NULL, &key_pressed_attr.attr);
}

// Driver exit function
static void keyboard_pressed_exit(void)
{
    pr_info("\n\n[KERNEL] %s: Finalizacao do driver \n\n", __func__);
    synchronize_irq(irq);
    free_irq(irq, &dev);
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &key_pressed_attr.attr);
    pr_info("\n\n [KERNEL] Driver do dispositivo removido com sucesso!!!\n\n");
}

module_init(keyboard_pressed_init);
module_exit(keyboard_pressed_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanefuji, G.; Costa, J.; Fuentes, S. <jmcosta@usp.br>");
MODULE_DESCRIPTION("Counting keyboard presses");