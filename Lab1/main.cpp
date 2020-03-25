#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

//block type
struct mem_block {
    void* pointer;
    size_t size;
};

vector<mem_block> taken_memory;

void* mem_alloc(size_t);// allocates a block of memory
void* mem_realloc(void*, size_t);//resize the block
void mem_free(void*, bool with_output=true);//makes block of memory free
size_t round_up_size(size_t);//makes a block a word size //word=4bytes
int* get_address(size_t, int* start_pointer=nullptr);//func get address of memory blockgi
void mem_dump(); //func shows status of memory block

int main() {
    cout << "Try allocate memory. Size:5" << endl;
    void* old_pointer = mem_alloc(5);

    cout << "Try allocate memory. Size:12" << endl;
    void* another_pointer = mem_alloc(12);

    cout << "Try resize memory blok Size:5 to Size:8" << endl;
    void* new_pointer = mem_realloc(old_pointer, 8);

    cout << "Try allocate memory. Size:4" << endl;
    void* last_pointer = mem_alloc(4);

    cout << "Try to free up memory block. Size:8" << endl;
    mem_free(new_pointer);
}

// allocates a block of memory
void* mem_alloc(size_t size) {
    size_t size_to_allocate = round_up_size(size);
    void* ptr = static_cast<void*>(get_address(size_to_allocate));
    int* values = new (ptr) int[size_to_allocate/4];
    if (ptr == nullptr) {
        return nullptr;
    }
    mem_block fragment{ptr, size_to_allocate};
    cout << "The memory was allocated at " << ptr << endl;
    taken_memory.push_back(fragment);
    mem_dump();
    return static_cast<void*>(ptr);
}


//resize the block
void* mem_realloc(void* addr, size_t size) {
    size_t size_to_allocate = round_up_size(size);
	for (auto element : taken_memory) {
		if (element.pointer == addr) {
			if (element.size >= size_to_allocate) {
				element.size = size_to_allocate;
				cout << "The memory was reallocated at " << element.pointer << " (the same address)" << endl;
				mem_dump();
				return element.pointer;
			}
			void* ptr = static_cast<void*>(get_address(size_to_allocate, static_cast<int*>(addr)));
			int* values = new (ptr) int[size_to_allocate / 4];
			int* previous_values = static_cast<int*>(addr);
			for (int i = 0; i < element.size / 4; i++) {
				values[i] = static_cast<int>(previous_values[i]);
			}
			for (int i = element.size / 4; i < size_to_allocate / 4; i++) {
				values[i] = 0;
			}
			if (ptr == nullptr) {
				return nullptr;
			}
			mem_free(addr, false);
			mem_block fragment{ ptr, size_to_allocate };
			cout << "The memory was reallocated at " << ptr << endl;
			taken_memory.push_back(fragment);
			mem_dump();
			return static_cast<void*>(ptr);
		}
	}
}


//makes block of memory free
void mem_free(void* addr, bool with_output) {
    int *a = static_cast<int*>(addr);
    for (int i = 0; i < taken_memory.size(); i++) {
        if (taken_memory[i].pointer == a) {
            taken_memory.erase(taken_memory.begin()+i);
        }
    }
    if (with_output) {
        cout << "The memory at " << addr << " was freed" << endl;
        mem_dump();
    }
}

//makes a block a word size //word=4bytes
size_t round_up_size(size_t original_size) {
    size_t new_size = original_size;
    if (original_size % 4 != 0) {
        new_size += (4 - (original_size % 4));
    }
    return new_size;
}

//func get address of memory block
int* get_address(size_t size, int* start_pointer) {
    int * possible_pointer;
    if (start_pointer == nullptr) {
        int prime_variable = 0;
        possible_pointer = &prime_variable + 4;
    }
    else {
        possible_pointer = start_pointer;
        for (auto & mem_block : taken_memory) {
            if (mem_block.pointer == start_pointer) {
                possible_pointer += mem_block.size;
                break;
            }
        }
    }
    while (true) {
        bool nothing_overlaps = true;
        for (auto & mem_block : taken_memory) {
            if (mem_block.pointer < possible_pointer + size
            && mem_block.pointer > possible_pointer - mem_block.size) {
                nothing_overlaps = false;
                break;
            }
        }
        if (!nothing_overlaps) {
            possible_pointer += 4;
        }
        else {
            return possible_pointer;
        }
    }
}


//func shows status of memory block
void mem_dump() {
    cout << "Currently taken memory:";
    if (taken_memory.empty()) {
        cout << " (none)";
    }
    cout << endl;
    for (auto element: taken_memory) {
        cout << "-> " << element.pointer << ", " << element.size << " bytes" << endl;
    }
}
