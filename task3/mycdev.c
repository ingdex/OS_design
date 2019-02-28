#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define BUFSIZE 1024 //max buf size

static char buf[BUFSIZE];

unsigned int mydev_major = 0;

static int my_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);//判断module模块是否处于活动状态，将该模块的引用计数加1
    printk("mycdev open\n");

    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);//使指定的模块使用量减一
    printk("cdev release\n");

    return 0;
}

static ssize_t my_read(struct file *file, char __user *ubuf, size_t count, loff_t *p_cfo)
{
    if (copy_to_user(ubuf, (void *)(buf + *p_cfo), count))
    {
        return -EFAULT;
    }

    return count;
}

static ssize_t my_write(struct file *file, const char __user *ubuf, size_t count, loff_t *p_cfo)
{
    if (copy_from_user(buf, ubuf, count))
    {
        return -EFAULT;
    }

    return count;
}

static loff_t my_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t cfo = 0;//current file offset
    switch (whence)
    {
        case 0://SEEK_SET
            cfo = offset;break;
        case 1://SEEK_CUR
            cfo = file->f_pos + offset;break;
        case 2://SEEK_END
            cfo = BUFSIZE - 1 + offset;break;
    }
    if (cfo < 0 || cfo > BUFSIZE)//the cfo is out of bounds
    {
        return -EINVAL;
    }
    file->f_pos = cfo;
    return cfo;
}

struct file_operations fops = {
    //.owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .llseek = my_llseek,
    //open:my_open,
    //read:my_read,
    //write:my_write,
    //release:my_release,
};

int init_module(void)
{
    //printk("mycdev: init\n");
    int dev_num;
    printk("mycdev: init\n");
    dev_num = register_chrdev(0, "mycdev", &fops);
    if (dev_num < 0)
    {
        printk("mycdev: failed to get major number\n");
        return dev_num;
    }
    if (mydev_major == 0)
        mydev_major = dev_num;
    printk("mycdev: success %d\n", dev_num);

    return 0;
}

void cleanup_module(void)	//bug: cleanup_moudule()
{
    unregister_chrdev(mydev_major, "mycdev");
    printk("unregister mycdev!\n");
}

MODULE_LICENSE("GPL");
