/* test_mck_drv-writecombine.c COPYRIGHT FUJITSU LIMITED 2017 */
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/version.h>
#include "test_mck_drv.h"

#define TEST_MCK_TS_NAME "writecombine"

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
static int test_mck_writecombine_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
#else
#if defined(RHEL_RELEASE_CODE) && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(8, 2)
static vm_fault_t test_mck_writecombine_vma_fault(struct vm_fault *vmf)
#else
static int test_mck_writecombine_vma_fault(struct vm_fault *vmf)
#endif
{
	struct vm_area_struct *vma = vmf->vma;
#endif
	struct test_mck_dev *dev;
	unsigned long buf;

	dev = vma->vm_private_data;
	buf = (unsigned long)dev->private_data;
	if (!buf) {
		return -ENOMEM;
	}
	vmf->page = vmalloc_to_page((void*)(buf + (vmf->pgoff << PAGE_SHIFT)));
	get_page(vmf->page);

	return 0;
}

static struct vm_operations_struct test_mck_writecombine_vm_ops = {
	.fault = test_mck_writecombine_vma_fault,
};

/* file ops */
static int test_mck_writecombine_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int test_mck_writecombine_mmap(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_private_data = file->private_data;
	vma->vm_ops = &test_mck_writecombine_vm_ops;
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
	return 0;
}

static int test_mck_writecombine_open(struct inode *inode, struct file *file)
{
	struct test_mck_dev *dev = NULL;

	dev = container_of(inode->i_cdev, struct test_mck_dev, cdev);
	file->private_data = dev;
	return 0;
}

struct file_operations test_mck_writecombine_fops = {
	.owner   = THIS_MODULE,
	.release = test_mck_writecombine_release,
	.mmap    = test_mck_writecombine_mmap,
	.open    = test_mck_writecombine_open,
};

int test_mck_writecombine_init(struct test_mck_dev* dev)
{
	int ret = 0;

	dev->private_data = vmalloc_user(PAGE_SIZE);
	mck_printk("dev(%p) buffer(%p).\n", dev, dev->private_data);

	if (!dev->private_data) {
		ret = -ENOMEM;
		goto out;
	}
	memset(dev->private_data, 0x80, PAGE_SIZE);

out:
	return ret;
}

void test_mck_writecombine_fin(struct test_mck_dev* dev)
{
	if (dev->private_data) {
		vfree(dev->private_data);
		dev->private_data = NULL;
	}
}
