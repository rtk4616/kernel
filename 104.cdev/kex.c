#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>

/* module information */
MODULE_AUTHOR("Troy D. Hanson");
MODULE_DESCRIPTION("Example of character device");
MODULE_LICENSE("Dual BSD/GPL");

#define NUM_MINORS 1

/* a global in the driver to keep state */
struct kex_t {
  dev_t dev;        /* has major and minor bits */
  struct cdev cdev; /* has our ops, owner, etc */
} c;

struct file_operations ops = {
  .read = NULL,
  .write = NULL,
  .unlocked_ioctl = NULL,
  .open = NULL,
  .release = NULL
};

int __init kex_init(void) {
  int rc;

  /* ask for a dynamic major */
  rc = alloc_chrdev_region(&c.dev, 0, NUM_MINORS, "kex");
  if (rc) {
    printk(KERN_WARNING "alloc_chrdev_region: failed to get major\n");
    goto done;
  }

  /* init the struct cdev */
  cdev_init(&c.cdev, &ops);
  c.cdev.owner = THIS_MODULE;

  /* make device live */
  rc = cdev_add(&c.cdev, c.dev, NUM_MINORS);
  if (rc) {
    printk(KERN_WARNING "cdev_add: can't add device\n");
    unregister_chrdev_region(c.dev, NUM_MINORS);
    cdev_del(&c.cdev);
    goto done;
  }

  rc = 0;

 done:
  return rc ? -ENODEV : 0;
}

void __exit kex_cleanup(void) {
  cdev_del(&c.cdev);
  unregister_chrdev_region(c.dev, NUM_MINORS);
}

module_init(kex_init);
module_exit(kex_cleanup);
