 // kernel 5.15 bug link: https://github.com/CachyOS/kernel-patches/blob/f7b6796cbaed1f7bd50c08715d77b12d8f306036/5.15/0001-PGO.patch#L1586
 #include <linux/module.h>
 #include <linux/slab.h>
 #include <linux/vmalloc.h>
 #include "pgo.h"

 static struct dentry *directory;

 struct prf_private_data {
 	void *buffer;
	unsigned long size;
 };


 static unsigned long prf_buffer_size(void)
 {
 	return sizeof(struct llvm_prf_header)}

static int prf_serialize(struct prf_private_data *p)
 {
 	int err = 0;
 	void *buffer;


 	p->size = prf_buffer_size();
	p->buffer = vzalloc(p->size);

	if (!p->buffer) {
		err = -ENOMEM;

 	}
 }
 static int prf_open(struct inode *inode, struct file *file)
 {
 	struct prf_private_data *data;
 	unsigned long flags;
	int err;
 	data = kzalloc(sizeof(*data), GFP_KERNEL);
 	if (!data) {
 		err = -ENOMEM;
		goto out;
 	}

	prf_unlock(flags);

	err = prf_serialize(data);
	if (unlikely(err)) {
		kfree(data);
		goto out_unlock;
	}
 	file->private_data = data;

out_unlock:
	prf_unlock(flags);
out:

 	return err;
 }