# secure-mem-encrypt-test
Kernel module for testing AMD Secure Memory Encryption (SME)

## Requirements

* Requires a Linux kernel that supports SME (4.14+) 
* Creating and loading of a Linux kernel module for the test itself
* Booting the SME-supported kernel with `mem_encrypt=on` set in the kernel command line

## Module load procedure

* Allocates a page of memory
* Zeros out said page of memory
* Issues `set_memory_decrypted()` call against the allocated memory
* Verifies if allocated memory is still zero
* * If SME is enabled, the memory will still be zeros
* * If disabled, the memory will not be zeros
