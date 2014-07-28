
#include <rnd.h>

#include <linux/random.h>
#include <linux/slab.h>

u64 *ln_rnd = NULL;
static int is_seeded = 0;
static size_t bytes;

/* We need this for later when we need to figure out how many items we can pull
 * out of the ln_rnd pool of randomness, because we use this to mask off any key
 * and ensure the key doesn't walk off the end of the randomness.
 */
static int item_mask;

// sz is in megabytes
void ln_rnd_seed(size_t sz)
{
	bytes = sz * 1024 * 1024;

	if (sz > 4 || sz < 0)
		sz = 4;
	
	ln_rnd = (u64 *) krealloc(ln_rnd, bytes, GFP_KERNEL);

	if (unlikely(ksize(ln_rnd) < bytes))
		bytes = ksize(ln_rnd);
	get_random_bytes_arch((char *) ln_rnd, bytes);

	is_seeded = 1;

	/* The item mask will be one less than the theoretical maximum.
	 * This is because the mask is used to AND with any supplied key to mask off
	 * the lower bits only, and hence we want to take ALL the bits which could
	 * feasibly produce something from the pool of randomness, but nothing
	 * outside. */
	item_mask = (bytes / sizeof(u64)) - 1;

#ifdef DEBUG
	printk(KERN_ALERT "Allocated %zu bytes (%zu internally).\n", 
		ksize(ln_rnd), bytes);
#endif
}

u64 ln_rnd_key_get_val(u64 key)
{
	/* Mask off the upper bits of the key -- we only want the lower bits which
	 * are going to give us an index into our array of randomness -- variable
	 * depending on how much randomness we allocated.
	 */
	return ln_rnd[(key & item_mask)];
}

u64 ln_rnd_key_mask(u64 key)
{
	return key & item_mask;
}
