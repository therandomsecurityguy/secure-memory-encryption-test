/*
Secure Memory Encryption Tests
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/smp.h>
#include <linux/mm.h>
#include <linux/io.h>

#include <asm/set_memory.h>

static unsigned int debug;
module_param(debug, uint, 0444);
MODULE_PARM_DESC(debug, " print extra debug information - any non-zero value");

#undef pr_fmt
#define pr_fmt(fmt)	"Memory Encryption: " fmt

static int __init secure_mem_encrypt_test_init(void)
{
	struct page *page, *page_reference;
	void *buffer, *buffer_reference;

	if (!mem_encrypt_active()) {
		pr_err("SME is not active, cannot conduct memory encryption tests\n");
		return -EAGAIN;
	}
	pr_notice("SME is active\n");

	page = alloc_page(GFP_KERNEL);
	buffer = page_address(page);
	memset(buffer, 0x00, PAGE_SIZE);

	page_reference = alloc_page(GFP_KERNEL);
	buffer_reference = page_address(page_reference);
	memset(buffer_reference, 0x00, PAGE_SIZE);

	if (debug)
		print_hex_dump(KERN_DEBUG, "Memory Encryption: Buffer (first 64 bytes -    C-bit): ", DUMP_PREFIX_OFFSET, 16, 1, buffer, 64, 1);

	/*
 	 * Call set_memory_decrypted() to remove the encryption bit associated
 	 * with the buffer under test. This will not actually decrypt the contents
 	 * of the memory, just mark it as not encrypted. This fact can then be
 	 * used to compare against the reference buffer and determine the state
 	 * of SME.
 	 */
	set_memory_decrypted((unsigned long)buffer, 1);
	if (debug)
		print_hex_dump(KERN_DEBUG, "Memory Encryption: Buffer (first 64 bytes - no C-bit): ", DUMP_PREFIX_OFFSET, 16, 1, buffer, 64, 1);

	if (memcmp(buffer, buffer_reference, PAGE_SIZE) == 0) {
		/* Buffers match - SME is active */
		pr_notice("SME is active\n");
	} else {
		/* Buffers don't match - SME is not active */
		pr_notice("SME is not active\n");
	}

	/*
 	 * Free buffers and reset encryption attributes as appropriate.
 	 */
	free_page((unsigned long)buffer_reference);

	set_memory_encrypted((unsigned long)buffer, 1);
	free_page((unsigned long)buffer);

	/*
 	 * Return an error so the test can be re-run without have to unload the
 	 * module first.
 	 */
	return -EAGAIN;
}

static void __exit secure_mem_encrypt_test_exit(void)
{
}

module_init(secure_mem_encrypt_test_init);
module_exit(secure_mem_encrypt_test_exit);
