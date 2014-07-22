#ifndef LN_RND_H
#define LN_RND_H

#include <linux/random.h>
#include <linux/types.h>

/* Pseudo-randomness interface */

#define LN_RND_SIZE_DEFAULT	1	// Amount of randomness to generate, in MB.

extern u64 *ln_rnd;

void ln_rnd_seed(size_t);
u64 ln_rnd_key_get_val(u64 key);
u64 ln_rnd_key_mask(u64 key);

#endif // LN_RND_H
