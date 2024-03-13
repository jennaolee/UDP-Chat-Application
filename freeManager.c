#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

// free messages once removed from inputList/outputList
void freeMessage(char *message) {
    free(message);
    message = NULL;
}
