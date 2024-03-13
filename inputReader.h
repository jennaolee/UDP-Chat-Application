#ifndef _INPUT_READER_H
#define _INPUT_READER_H

#include "list.h"

void* readKeyboardInput();
void initInputReader(List* list);
void cancelInputReader();
void closeInputReader();

#endif