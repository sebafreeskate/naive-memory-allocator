
#include <iostream>
#include <string>

//memory structure is implemented by double circular linked list 

struct MemoryNode;

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


struct MemoryNode {

    bool hasEnoughMemory(size_t size) 
    {
        return (this->size) >=  (size + sizeof(MemoryNode)) 
        && this->isFree;
    }

    void * memory;     
    std::size_t size;  
    MemoryNode * prev; 
    MemoryNode * next; 
    bool isFree;       

};

//this project assumes no use of c/c++ dynamyc allocation, its me who should realize this stuff
//so there are some kind of constructor for MemoryNode 

MemoryNode* build_node(void *memory, size_t size_to_allocate, MemoryNode* prev, bool isFree)
{
    MemoryNode* newNode = static_cast<MemoryNode*>(memory);
    newNode->size = size_to_allocate;
    newNode->memory = increment_pointer(memory, sizeof(MemoryNode));
    newNode->prev = prev;
    newNode->next = root;
    newNode->isFree = isFree;

    return newNode;
}

void *allocate(size_t size_to_allocate)
{
    MemoryNode* newNode = build_node(head->memory, size_to_allocate, head, false);  
    root->prev = newNode;
    head->next = newNode;
	head->memory = increment_pointer(head->memory, size_to_allocate + sizeof(MemoryNode));
	head->size = head->size - (size_to_allocate + sizeof(MemoryNode));
	head = newNode;
    return head->memory;
}

void setup(void *buf, std::size_t size)
{
    root = build_node(buf, size - sizeof(MemoryNode), static_cast<MemoryNode *>(buf), true); //yep, point to itself, loop
	root->next = root;
	root->prev = root;
    head = root;
}

void *myalloc(std::size_t size_to_allocate)
{
    if(head->hasEnoughMemory(size_to_allocate)) {
        return allocate(size_to_allocate);
    }

	MemoryNode *end = head;
    while( head->next != end) { //its a looped linked list
        head = head->next;
        if(head->hasEnoughMemory(size_to_allocate)) {
            return allocate(size_to_allocate);
        }
    }
    return NULL;
}

void myfree(void *p)
{
    MemoryNode *currentNode= static_cast<MemoryNode *>((decrement_pointer(p, sizeof(MemoryNode))));
    MemoryNode *prevNode = currentNode->prev;
    if(prevNode->isFree) {
        prevNode->size = prevNode->size + sizeof(MemoryNode) + currentNode->size;
        prevNode->memory = decrement_pointer(prevNode->memory, sizeof(MemoryNode) + currentNode->size);
        prevNode->next = currentNode->next;
        head = prevNode;
    } else {
        currentNode->isFree = true;
    }
}

void print_available_bytes() 
{
    int ret = 0;

    MemoryNode *end = head;
    MemoryNode *i = head;
    do {
        if(i->isFree) {
            ret += i->size; 
        }
        i = i->next;
    } while(i != end);
    std::cout << "available bytes = " << ret << std::endl;
}

void print_available_range(void *buf, size_t bytes)
{
    std::cout << "range between " << (uint64_t)buf << " and " << (uint64_t)(increment_pointer(buf, bytes));
}

bool is_it_in_range(void *addr, void *buf, size_t bytes) 
{
    uint64_t lowBound = (uint64_t)buf;
    uint64_t highBound = (uint64_t)(increment_pointer(buf, bytes));
    return ((uint64_t)addr >= lowBound) 
        && ((uint64_t)addr <= highBound);
}

int main() 
{
    std::cout << "enter total bytes count to manage" << std::endl;
	size_t bytes;
    std::cin >> bytes;
     
    void *buf = malloc(bytes);
	setup(buf, bytes);
    print_available_bytes();

	std::string action;
	size_t size;
	while (true) {
		std::cout << "enter action" << std::endl;
		std::cin >> action;
		if (action == "malloc") {
			std::cout << "enter size" << std::endl;
			std::cin >> size;
            void *addr = (myalloc(size));
			std::cout << "ur address : " << (uint64_t)addr << std::endl;
            std::cout << "is it in range = " << is_it_in_range(addr, buf, bytes) << std::endl; 
            print_available_bytes();
		} else if (action == "free") {
			std::cout << "enter address" << std::endl;
			uint64_t addr;
			std::cin >> addr;
			myfree(static_cast<void*>((int*)(addr)));
            print_available_bytes();
		} else if (action == "quit" || action == "exit") {
            return 0;
        }
	}
    return 0;
}
