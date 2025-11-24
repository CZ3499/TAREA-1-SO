#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    char *addr = sbrk(0); // Current heap pointer
    sbrk(4096);           // Allocate one page
    
    addr[0] = 'Z';        // Write initial value

    printf("1. Protecting memory (removing Read permission)...\n");
    
    // Protect against read access
    if (mrdprotect(addr, 1) < 0) {
        printf("mrdprotect failed\n");
        exit(1);
    }

    printf("2. Attempting to WRITE 'A' (Should succeed in Write-Only mode)...\n");
    
    // Writes should still be allowed (write-only)
    // If the program crashes here with scause 0xf, it means hardware doesn't support W=1/R=0
    addr[0] = 'A';

    printf("3. Write successful! Now attempting to READ (Should TRAP)...\n");

    // Attempting to read should cause a fault (trap)
    // The program should die here if the protection works correctly.
    // Expected error: scause 0xd (Load Page Fault)
    char c = addr[0]; 

    printf("ALERT: Read value: %c (This should NOT print)\n", c);

    // If we reach here, the protection failed

    // Revert protection (only if it didn't die earlier)
    if (munrdprotect(addr, 1) < 0) {
        printf("munrdprotect failed\n");
        exit(1);
    }

    printf("Protection reverted successfully.\n");
    exit(0);
}