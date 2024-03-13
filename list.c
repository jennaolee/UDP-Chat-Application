#include "list.h"
#include <stdio.h>
#include <stdlib.h>

// statically-allocated arrays for nodes and listHeads
static Node nodesArr[LIST_MAX_NUM_NODES];
static List listsArr[LIST_MAX_NUM_HEADS];

// flag to make sure nodesArr and listsArr are initialized once in List_create()
static bool hasFirstListBeenCreated = false;

// pointers that point to the first available node is nodesArr and list in listsArr
static Node *availableNode;
static List *availableList;

// int value to represent non-OOB state
int LIST_IN_BOUNDS = 2;

// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create() {

    // set up the stacks using linked list to keep track of free nodes/lists and be able to add removed nodes/lists to front
    if (!hasFirstListBeenCreated) {
        
        // create nodes stack/LL - set up pointers
        for (int i = 0; i < LIST_MAX_NUM_NODES - 1; i++) {
            nodesArr[i].next = &nodesArr[i + 1];
            nodesArr[i + 1].prev = &nodesArr[i];
        }

        // init first node prev pointer
        nodesArr[0].prev = NULL;

        // init last node next pointer
        nodesArr[LIST_MAX_NUM_NODES - 1].next = NULL;

        // create lists stack/LL - set up pointers
        for (int j = 0; j < LIST_MAX_NUM_HEADS - 1; j++) {
            listsArr[j].next = &listsArr[j + 1];
            listsArr[j + 1].prev = &listsArr[j];
            listsArr[j].currentState = LIST_OOB_START;
        }

        // init first list prev pointer
        listsArr[0].prev = NULL;

        // init last list next pointer and currentState
        listsArr[LIST_MAX_NUM_HEADS - 1].next = NULL;
        listsArr[LIST_MAX_NUM_HEADS - 1].currentState = LIST_OOB_START;

        // set the pointers to the first available node and list
        availableList = &listsArr[0];
        availableNode = &nodesArr[0];

        // set hasFirstListBeenCreated to true - stack has been created
        hasFirstListBeenCreated = true;
    }

    // case: all lists are exhausted, then fails
    if (availableList == NULL) {
        return NULL;
    }

    // case: there is an available list
    // get the first available list and set the default values
    List *newList = availableList;
    newList->size = 0;
    newList->head = NULL;
    newList->tail = NULL;
    newList->curr = NULL;
    newList->currentState = LIST_OOB_START;

    // update the availableList pointer to the next availableList
    availableList->prev = NULL;
    availableList = availableList->next;

    // newList is no longer pointing to the next availableList, point to NULL
    newList->next = NULL;
    return newList;
}

// Returns the number of items in pList.
int List_count(List* pList) {
    // case: pList is NULL, return -1
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // return the size/number of elements in the list
    return pList->size;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList) {
    // case: pList is NULL, return NULL
    if (pList == NULL) {
        return NULL;
    }

    // case: pList is empty, return NULL and set current to NULL
    if (pList->size == 0) {
        pList->curr = NULL;
        return NULL;
    }

    // else, sets current to head and currentState becomes in-bounds
    pList->curr = pList->head;
    pList->currentState = LIST_IN_BOUNDS;
    return pList->head->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList) {
    // case: pList is NULL, return NULL
    if (pList == NULL) {
        return NULL;
    }

    // case: pList is empty, return NULL and set current to NULL
    if (pList->size == 0) {
        pList->curr = NULL;
        return NULL;
    }

    // else, sets current to tail and currentState becomes in-bounds
    pList->curr = pList->tail;
    pList->currentState = LIST_IN_BOUNDS;
    return pList->tail->item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList) {
    // case: pList is NULL or currentState is LIST_OOB_END, return NULL (no changes)
    if (pList == NULL || pList->currentState == LIST_OOB_END) {
        return NULL;
    }

    // case: pList is empty
    if (pList->size == 0) {
        pList->currentState = LIST_OOB_END;
        return NULL;
    }

    // case: pList is non-empty and currentState is LIST_OOB_START
    if (pList->currentState == LIST_OOB_START) {
        pList->curr = pList->head;
        pList->currentState = LIST_IN_BOUNDS;
        return pList->curr->item;
    }

    // case: operation advances the current item beyond the end of the list, return NULL, set current to NULL, and set current state to LIST_OOB_END
    if (pList->curr->next == NULL) {
        pList->currentState = LIST_OOB_END;
        pList->curr = NULL;
        return NULL;
    }

    // case: current pointer is in the middle of the list
    pList->curr = pList->curr->next;
    pList->currentState = LIST_IN_BOUNDS;
    return pList->curr->item;
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList) {
    // case: pList is NULL or empty, or currentState is LIST_OOB_START, return NULL (no changes)
    if (pList == NULL || pList->size == 0 || pList->currentState == LIST_OOB_START) {
        return NULL;
    }

    // case: pList is non-empty and currentState is LIST_OOB_END
    if (pList->currentState == LIST_OOB_END) {
        pList->curr = pList->tail;
        pList->currentState = LIST_IN_BOUNDS;
        return pList->curr->item;
    }

    // case: operation backs up the current item beyond the start of the list, return NULL, set current to NULL, and set currentState to LIST_OOB_START
    if (pList->curr->prev == NULL) {
        pList->currentState = LIST_OOB_START;
        pList->curr = NULL;
        return NULL;
    }

    // case: current pointer is in the middle of the list
    pList->curr = pList->curr->prev;
    pList->currentState = LIST_IN_BOUNDS;
    return pList->curr->item;
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList) {
    // case: if pList is NULL or empty, or LIST_OOB_START/END, there is no current item - return NULL
    if (pList == NULL || pList->size == 0 || pList->currentState == LIST_OOB_START || pList->currentState == LIST_OOB_END) {
        return NULL;
    }

    return pList->curr->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem) {
    // case: pList is NULL
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // case: all nodes exhausted, operation fails
    if (availableNode == NULL) {
        return LIST_FAIL;
    }

    // create new node
    // get first available node
    Node *newNode = availableNode;

    // move availableNode to next
    availableNode = availableNode->next;

    // assign node item
    newNode->item = pItem;

    // case: pList is empty
    if (pList->size == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        pList->head = newNode;
        pList->curr = newNode;
        pList->tail = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else if (pList->currentState == LIST_OOB_START && pList->size > 0) {
        // case: list is non-empty and current pointer is before the start of the list
        newNode->next = pList->head;
        pList->head->prev = newNode;
        newNode->prev = NULL;
        pList->curr = newNode;
        pList->head = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else if (pList->currentState == LIST_OOB_END) {
        // case: list is non-empty and current pointer is after the end of the list
        newNode->next = NULL;
        newNode->prev = pList->tail;
        pList->tail->next = newNode;
        pList->curr = newNode;
        pList->tail = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else if (pList->curr->next == NULL) {
        newNode->next = NULL;
        newNode->prev = pList->curr;
        pList->curr->next = newNode;
        pList->curr = newNode;
        pList->tail = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else {
        newNode->next = pList->curr->next;
        pList->curr->next->prev = newNode;
        newNode->prev = pList->curr;
        pList->curr->next = newNode;
        pList->curr = newNode;
    }

    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem) {
    // case: pList is NULL
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // case: all nodes are exhausted
    if (availableNode == NULL) {
        return LIST_FAIL;
    }

    // create new node
    // get first available node
    Node *newNode = availableNode;

    // move availableNode to next
    availableNode = availableNode->next;

    // set up node item
    newNode->item = pItem;

    // case: pList is empty
    if (pList->size == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        pList->head = newNode;
        pList->curr = newNode;
        pList->tail = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else if (pList->currentState == LIST_OOB_START && pList->size > 0) {   
        // case: pList is non empty and current is LIST_OOB_START
        newNode->prev = NULL;
        newNode->next = pList->head;
        pList->head->prev = newNode;
        pList->head = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else if (pList->currentState == LIST_OOB_END) {
        // case: pList is non empty and current is OOB_END
        newNode->next = NULL;
        pList->tail->next = newNode;
        newNode->prev = pList->tail;
        pList->tail = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;
    
    } else if (pList->curr->prev == NULL) {
        newNode->prev = NULL;
        newNode->next = pList->head;
        pList->head->prev = newNode;
        pList->head = newNode;
        pList->curr = newNode;

    } else {
        newNode->prev = pList->curr->prev;
        pList->curr->prev->next = newNode;
        newNode->next = pList->curr;
        pList->curr->prev = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;
    }

    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem) {
    // case: pList is NULL
    if (pList == NULL) {
        return LIST_FAIL;
    } 

    // case: all nodes are exhausted
    if (availableNode == NULL) {
        return LIST_FAIL;
    }  

    // assign new node
    Node *newNode = availableNode;

    // move availableNode to the next availableNode
    availableNode = availableNode->next;

    // assign item to the node
    newNode->item = pItem;

    // case: pList is empty
    if (pList->size == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        pList->head = newNode;
        pList->tail = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else {
        newNode->next = NULL;
        newNode->prev = pList->tail;
        pList->tail->next = newNode;
        pList->tail = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;
    }

    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem) {

    // case: pList is NULL
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // case: all nodes are exhausted
    if (availableNode == NULL) {
        return LIST_FAIL;
    }

    // create new node
    Node *newNode = availableNode;

    // move availableNode to next availableNode
    availableNode = availableNode->next;

    // assign item to node
    newNode->item = pItem;

    // case: pList is empty
    if (pList->size == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        pList->head = newNode;
        pList->curr = newNode;
        pList->tail = newNode;
        pList->currentState = LIST_IN_BOUNDS;

    } else {
        newNode->prev = NULL;
        newNode->next = pList->head;
        pList->head->prev = newNode;
        pList->head = newNode;
        pList->curr = newNode;
        pList->currentState = LIST_IN_BOUNDS;
    }

    pList->size++;
    return LIST_SUCCESS;

}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList) {
    // case: pList is NULL or empty, and currentState is LIST_OOB_START/END
    if (pList == NULL || pList->size == 0 || pList->currentState == LIST_OOB_START || pList->currentState == LIST_OOB_END) {
        return NULL;
    }

    // save current item
    void *currentItem = pList->curr->item;
 
    // case: one element in the list
    if (pList->size == 1) {
        // add node back to available nodes
        pList->curr->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->curr;
        }

        availableNode = pList->curr;
        availableNode->prev = NULL;

        // set to default values
        pList->curr = NULL;
        pList->head = NULL;
        pList->tail = NULL;

        // currentState becomes LIST_OOB_END
        pList->currentState = LIST_OOB_END;

    } else if (pList->curr->prev == NULL) {
        // case: current is the head node
        pList->head = pList->head->next;
        pList->curr->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->curr;
        }

        availableNode = pList->curr;
        pList->curr->prev = NULL;
        pList->curr = pList->head;
        pList->curr->prev = NULL;
        pList->currentState = LIST_IN_BOUNDS;
        

    } else if (pList->curr->next == NULL) {
        // case: current is the tail node
        pList->tail = pList->tail->prev;
        pList->curr->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->curr;
        }

        availableNode = pList->curr;
        availableNode->prev = NULL;
        pList->curr = NULL;
        pList->tail->next = NULL;
        pList->currentState = LIST_OOB_END;

    } else {
        // update pointers
        pList->curr->prev->next = pList->curr->next;
        pList->curr->next->prev = pList->curr->prev;

        // // add to available node list
        pList->curr->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->curr;
        }
        availableNode = pList->curr;
        pList->curr = pList->curr->prev->next;
        
        availableNode->prev = NULL;
    }

    pList->size--;
    return currentItem;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList) {
    // case: pList is NULL or empty
    if (pList == NULL || pList->size == 0) {
        return NULL;
    }

    // save last item
    void *lastItem = pList->tail->item;

    if (pList->size == 1) {
        // add node to available nodes
        pList->curr->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->curr;
        }

        availableNode = pList->curr;
        availableNode->prev = NULL;

        // reset to defaul values
        pList->curr = NULL;
        pList->head = NULL;
        pList->tail = NULL;

        // new last item is the previous item - previous item is before start of list
        pList->currentState = LIST_OOB_END; 

    } else {
        pList->curr = pList->tail->prev;
        pList->tail->prev->next = NULL;
        pList->tail->prev = NULL;

        // // add node to available nodes
        pList->tail->next = availableNode;

        if (availableNode != NULL) {
            availableNode->prev = pList->tail;
        }
        availableNode = pList->tail;

        // mode tail to new tail
        pList->tail = pList->curr;
        pList->currentState = LIST_IN_BOUNDS;
    }

    pList->size--;
    return lastItem;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2) {
    // case: pList1 == NULL or pList2 == NULL 
    if (pList1 == NULL || pList2 == NULL) {
        return;
    }

    // case: pList2 is empty
    if (pList2->size == 0) {
        // exit if statement to reset pList2 to default values and add pList2 back to availableList
    } else if (pList1->size == 0) {
        // case: pList1 is empty
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->size = pList2->size;

        pList1->curr = NULL;
        pList1->currentState = LIST_OOB_START;

    } else {
        // connect pointers
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;

        // move tail1 to tail2
        pList1->tail = pList2->tail;

        // update size
        pList1->size += pList2->size;
    }

    // reset pList2 to default values
    pList2->head = NULL;
    pList2->curr = NULL;
    pList2->tail = NULL;
    pList2->currentState = LIST_OOB_START;
    pList2->size = 0;

    // add pList2 back to availableList
    pList2->next = availableList;
    pList2->prev = NULL;
    
    if (availableList != NULL) {
        availableList->prev = pList2;
    }

    availableList = pList2;
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn) {
    // case: pList or function pointer is NULL
    if (pList == NULL || pItemFreeFn == NULL) {
        return;
    }

    // set current to head to start iterating
    pList->curr = pList->head;
    pList->currentState = LIST_IN_BOUNDS;
    while(pList->curr != NULL) {
        // call free routine
        (*pItemFreeFn)(pList->curr->item);

        // remove item from list
        List_remove(pList);
    }

    // set currentState to LIST_OOB_START
    pList->currentState = LIST_OOB_START;

    // add pList back to availableList
    pList->next = availableList;
    if (availableList != NULL) {
        availableList->prev = pList;
    }
    availableList = pList;
    availableList->prev = NULL;
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    // case: pList is NULL or pList is empty or OOB_END, current is NULL and cannot start at beginning of list
    if (pList == NULL || pList->currentState == LIST_OOB_END) {
        return NULL;
    }

    // case: pList is empty
    if (pList->size == 0) {
        pList->currentState = LIST_OOB_END;
        return NULL;
    }

    // case: pList state is OOB_START
    if (pList->currentState == LIST_OOB_START) {
        // start searching from head node
        pList->curr = pList->head;
        pList->currentState = LIST_IN_BOUNDS;
    }

    // store result of comparison
    bool result = false;
    Node *current = pList->curr;

    // continue looping while current is in the list and a match has not been found
    while (pList->curr != NULL && result != true) {
        // call comparator function and update result
        result = (*pComparator)(pList->curr->item, pComparisonArg);

        // save actual current node
        current = pList->curr;

        // increment current to move forward in the list
        pList->curr = pList->curr->next;
    }

    // reassign current to actual current node
    pList->curr = current;

    // if current is the tail node and no match was found, reassign current to next node (NULL) as it caused it to exit the while loop
    // and update currentStatus to LIST_OOB_END
    // no match was found so return NULL
    if (pList->curr->next == NULL && result == false) {
        pList->curr = NULL;
        pList->currentState = LIST_OOB_END;
        return NULL;
    } 
    
    // if match was found, return matchin item
    return pList->curr->item;
}