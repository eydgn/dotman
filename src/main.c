#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cfg.h"
#include "core.h"

int main(void)
{
    entry_t* entries;
    entry_new(&entries);

    read_cfg("test.cfg", &entries);
    write_cfg(entries, "test.cfg");

    printf("\nAfter sorting:\n");

    for (size_t i = 0; i < entries->len; i++)
    {
        printf("%s\n", *entries->data[i].entry->str);
    }

    for (size_t i = 0; i < entries->len; i++)
    {
        svec_free(&entries->data[i].entry);
    }

    entry_free(&entries);
    return 0;
}
