#ifndef _OUTPUT_WRITER_H
#define _OUTPUT_WRITER_H

#include "list.h"

void* writeMessages();
void initOutputWriter(List* l);
void cancelOutputWriter();
void closeOutputWriter();

#endif