#include "libxslt/xsltlocale.h"
#include <stdio.h>
#include <libxml/xmlmemory.h>  // For xmlFree declaration
#include <locale.h>
#include <string.h>  // For strxfrm_l declaration

int main() {
    // Create locale directly using POSIX functions
    locale_t locale = newlocale(LC_ALL_MASK, "C.UTF-8", NULL);
    if (!locale) {
        printf("Failed to create locale\n");
        return 1;
    }

    // Use a string that will cause strxfrm_l to fail
    const xmlChar *str = (const xmlChar *)"\xff\xff\xff\xff";  // Invalid UTF-8 sequence
    
    // Force a small buffer allocation to test overflow
    size_t xstrlen = 1;  // Intentionally small buffer
    printf("Forcing small buffer allocation of size: %zu\n", xstrlen);
    xmlChar *xstr = (xmlChar *) xmlMalloc(xstrlen);
    if (xstr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // This should trigger the heap buffer overflow
    printf("Allocating buffer of size: %zu\n", xstrlen);
    size_t r = strxfrm_l((char *)xstr, (const char *)str, xstrlen, locale);
    printf("Second strxfrm_l returned: %zu\n", r);
    
    if (r >= xstrlen) {
        printf("Potential buffer overflow detected! Required size: %zu, allocated: %zu\n", r, xstrlen);
        // Access memory beyond allocated buffer to check for overflow
        for (size_t i = xstrlen; i < r + 1; i++) {
            printf("Accessing byte %zu: 0x%02x\n", i, xstr[i]);
        }
    }

    if (xstr) {
        // Print buffer contents before freeing
        printf("Buffer contents: ");
        for (size_t i = 0; i < r + 1; i++) {
            printf("0x%02x ", xstr[i]);
        }
        printf("\n");
        xmlFree(xstr);
    }
    if (locale) {
        freelocale(locale);
    }
    return 0;
}
