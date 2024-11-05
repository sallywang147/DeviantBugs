 b/fs/btrfs/block-group.c
 int btrfs_remove_block_group(struct btrfs_trans_handle *trans,
 	spin_unlock(&cluster->refill_lock);
 
 	btrfs_clear_treelog_bg(block_group);
	btrfs_clear_data_reloc_bg(block_group);
 
 	path = btrfs_alloc_path();
 	if (!path) {

 b/fs/btrfs/ctree.h
struct btrfs_fs_info {
 	spinlock_t treelog_bg_lock;
 	u64 treelog_bg;
 
	/*
	 * Start of the dedicated data relocation block group, protected by
	 * relocation_bg_lock.
	 */
	spinlock_t relocation_bg_lock;
	u64 data_reloc_bg;

 #ifdef CONFIG_BTRFS_FS_REF_VERIFY
 	spinlock_t ref_verify_lock;
 	struct rb_root block_tree;
 static inline bool btrfs_is_zoned(const struct btrfs_fs_info *fs_info)
 	return fs_info->zoned != 0;
 }
 
static inline bool btrfs_is_data_reloc_root(const struct btrfs_root *root)
{
	return root->root_key.objectid == BTRFS_DATA_RELOC_TREE_OBJECTID;
}

 b/fs/btrfs/disk-io.c
 static int btrfs_init_fs_root(struct btrfs_root *root, dev_t anon_dev)
 		goto fail;
 
 	if (root->root_key.objectid != BTRFS_TREE_LOG_OBJECTID &&
	    !btrfs_is_data_reloc_root(root)) {
 		set_bit(BTRFS_ROOT_SHAREABLE, &root->state);
 		btrfs_check_and_init_root_item(&root->root_item);
 	}
void btrfs_init_fs_info(struct btrfs_fs_info *fs_info)
 	spin_lock_init(&fs_info->buffer_lock);
 	spin_lock_init(&fs_info->unused_bgs_lock);
 	spin_lock_init(&fs_info->treelog_bg_lock);
	spin_lock_init(&fs_info->relocation_bg_lock);
 	rwlock_init(&fs_info->tree_mod_log_lock);
 	mutex_init(&fs_info->unused_bg_unpin_mutex);
 	mutex_init(&fs_info->reclaim_bgs_lock);

 b/fs/btrfs/extent-tree.c
int btrfs_cross_ref_exist(struct btrfs_root *root, u64 objectid, u64 offset,
 
 out:
 	btrfs_free_path(path);
	if (btrfs_is_data_reloc_root(root))
 		WARN_ON(ret > 0);
 	return ret;
 }