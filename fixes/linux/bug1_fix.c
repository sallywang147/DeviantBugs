// kernel 5.15 bug link: https://github.com/CachyOS/kernel-patches/blob/f7b6796cbaed1f7bd50c08715d77b12d8f306036/5.15/0001-PGO.patch#L1586

 #include <linux/module.h>
 #include <linux/slab.h>
 #include <linux/vmalloc.h>
 #include <linux/mm.h>
 #include "pgo.h"

 static struct dentry *directory;

 struct prf_private_data {
 	void *buffer;	size_t size;
 };

 /*
@@ -213,6 +214,7 @@ static inline unsigned long prf_get_padding(unsigned long size)
 	return 7 & (sizeof(u64) - size % sizeof(u64));
 }

+/* Note: caller *must* hold pgo_lock */
 static unsigned long prf_buffer_size(void)
 {
 	return sizeof(struct llvm_prf_header) 

 }

 /*
  * Serialize the profiling data into a format LLVM's tools can understand.
+ * Returns actual buffer size in p->size.
+ * Note: p->buffer must point into vzalloc()'d
+ * area of at least prf_buffer_size() in size.
  * Note: caller *must* hold pgo_lock.
  */
static int prf_serialize(struct prf_private_data *p, size_t buf_size)
 {
 	int err = 0;
 	void *buffer;

 	p->size = prf_buffer_size();
	if (p->size > buf_size) {
		err = -EAGAIN;
 		goto out;
 	}
 }
static int prf_open(struct inode *inode, struct file *file)
 {
 	struct prf_private_data *data;
 	unsigned long flags;
	size_t buf_size;
	int err = 0;

 	data = kzalloc(sizeof(*data), GFP_KERNEL);
 	if (!data) {
 		err = -ENOMEM;
		goto out_free;
 	}

	/* get initial buffer size */
 	flags = prf_lock();
	data->size = prf_buffer_size();
	prf_unlock(flags);
	do {
		if (data->buffer)
			vfree(data->buffer);

		/* allocate, round up to page size. */
		buf_size = PAGE_ALIGN(data->size);
		data->buffer = vzalloc(buf_size);

		if (!data->buffer) {
			err = -ENOMEM;
			goto out_free;
		}

		/*
		 * try serialize and get actual
		 * data length in data->size
		 */
		flags = prf_lock();
		err = prf_serialize(data, buf_size);
		prf_unlock(flags);
		/* in unlikely case, try again. */
	} while (err == -EAGAIN);

	if (err)
		goto out_free;

 	file->private_data = data;
	return 0;

out_free:
	if (data)
		vfree(data->buffer);
	kfree(data);
 	return err;
 }


