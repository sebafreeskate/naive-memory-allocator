#include <cstddef>
#include <iostream>

struct MemoryNode {

    bool hasEnoughMemory(size_t size) {
        return this->size >=  size + sizeof(MemoryNode);
    }

    void * memory;     
    std::size_t size;  
    MemoryNode * prev; 
    MemoryNode * next; 
    bool isFree;       
};

MemoryNode* head;

void *increment_pointer(void* p, size_t value) 
{
    return static_cast<void* >(static_cast<char*>(p) + value);
}

void *decrement_pointer(void* p, size_t value) 
{
    return static_cast<void* >(static_cast<char*>(p) - value);
}

MemoryNode* build_node(void *memory, size_t size, MemoryNode* prev, bool isFree)
{
    MemoryNode* newNode = static_cast<MemoryNode*>(memory);
    newNode->size = size - sizeof(newNode);
    newNode->memory = increment_pointer(memory, sizeof(newNode));
    newNode->prev = prev;
    newNode->next = NULL;
    newNode->isFree = isFree;

    return newNode;
}

void mysetup(void *buf, std::size_t size)
{
    MemoryNode* root = build_node(buf, size, NULL, true);
    head = root;
}

void *allocate(size_t size)
{
    MemoryNode* newNode = build_node(head->memory, size, head, false);  
    head->next = newNode;
    return newNode->memory;
}

void *myalloc(std::size_t size)
{
    if(head->hasEnoughMemory(size)) {
        return allocate(size);
    }
    while(head->next && head->next != head) { //its a looped linked list
        head = head->next;
        if(head->hasEnoughMemory(size)) {
            return allocate(size);
        }
    }
    return NULL;
}

void myfree(void *p)
{
    MemoryNode *currentNode= static_cast<MemoryNode *>((decrement_pointer(p, sizeof(MemoryNode))));
    MemoryNode *prevNode = currentNode->prev;
    if(prevNode && prevNode->isFree) {
        prevNode->size = prevNode->size + sizeof(currentNode) + currentNode->size;
        prevNode->next = currentNode->next;
    } else {
        currentNode->isFree = true;
    }
}

int main() 
{
    return 0;
}
