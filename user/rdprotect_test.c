#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    char *addr = sbrk(0); // Current heap pointer
    sbrk(4096);           // Allocate one page
    
    addr[0] = 'Z';        // Write initial value

    // Protect against read access
    if (mrdprotect(addr, 1) < 0) {
        printf("mrdprotect falló\n");
        exit(1);
    }
    // Writes should still be allowed (write-only)
    addr[0] = 'A';

    // Attempting to read should cause a fault (trap)
    // The program should die here if the protection works correctly.
    char c = addr[0]; 

    printf("Valor leído: %c (esto NO debería imprimirse)\n", c);

    // If we reach here, the protection failed

    // Revert protection (only if it didn't die earlier)
    if (munrdprotect(addr, 1) < 0) {
        printf("munrdprotect falló\n");
        exit(1);
    }

    printf("Protección revertida correctamente.\n");
    exit(0);
}