#include <cstddef>
#include <iostream>
#include <string>

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

MemoryNode *root;
MemoryNode *head;

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
    newNode->size = size - sizeof(MemoryNode);
    newNode->memory = increment_pointer(memory, sizeof(MemoryNode));
    newNode->prev = prev;
    newNode->next = root;
    newNode->isFree = isFree;

    return newNode;
}


void *allocate(size_t size)
{
    MemoryNode* newNode = build_node(head->memory, size, head, false);  
    head->next = newNode;
	head->memory = increment_pointer(head->memory, size);
	head->size = head->size - size;
	head = newNode;
    return head->memory;
}

void mysetup(void *buf, std::size_t size)
{
    MemoryNode* root = build_node(buf, size, static_cast<MemoryNode *>(buf), true); //yep, point to itself, loop
    head = root;
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
        prevNode->size = prevNode->size + sizeof(MemoryNode) + currentNode->size;
        prevNode->next = currentNode->next;
    } else {
        currentNode->isFree = true;
    }
}

int main() 
{
	size_t bytes = 524288;
	mysetup(malloc(bytes), bytes);
	std::string action;
	size_t size;
	while (true) {
		std::cout << "enter action" << std::endl;
		std::cin >> action;
		if (action == "malloc") {
			std::cout << "enter size" << std::endl;
			std::cin >> size;
			std::cout << "ur address : " << (int)(myalloc(size)) << std::endl;
		} else if (action == "free") {
			std::cout << "enter address" << std::endl;
			int addr;
			std::cin >> addr;
			myfree(static_cast<void*>((int*)(addr)));
		}
	}
    return 0;
}
