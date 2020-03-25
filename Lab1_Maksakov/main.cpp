#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

struct memory_fragment {
    void* pointer;
    size_t size;
};

vector<memory_fragment> taken_memory; // lists the taken pointers

void* mem_alloc(size_t);
void* mem_realloc(void*, size_t);
void mem_free(void*, bool with_output=true);
size_t round_up_size(size_t);
int* get_address(size_t, int* start_pointer=nullptr);
void mem_dump();


int main() {
    void* old_pointer = mem_alloc(5);
    void* another_pointer = mem_alloc(12);
    void* new_pointer = mem_realloc(old_pointer, 8);
    void* last_pointer = mem_alloc(4);
    mem_free(new_pointer);
}

// Allocating memory
void* mem_alloc(size_t size) {
    size_t size_to_allocate = round_up_size(size);
    void* ptr = static_cast<void*>(get_address(size_to_allocate));
    int* values = new (ptr) int[size_to_allocate/4];
    if (ptr == nullptr) {
        return nullptr;
    }
    memory_fragment fragment{ptr, size_to_allocate};
    cout << "The memory was allocated at " << ptr << endl;
    taken_memory.push_back(fragment);
    mem_dump();
    return static_cast<void*>(ptr);
}

// Reallocating memory
void* mem_realloc(void* addr, size_t size) {
    size_t size_to_allocate = round_up_size(size);
	for (auto element : taken_memory) {
		if (element.pointer == addr) {
			// if the size is the same or less, there's no need in reallocating
			if (element.size >= size_to_allocate) {
				element.size = size_to_allocate;
				cout << "The memory was reallocated at " << element.pointer << " (the same address)" << endl;
				mem_dump();
				return element.pointer;
			}
			// otherwise, it searches for other memory, while clearing the existing memory
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
			memory_fragment fragment{ ptr, size_to_allocate };
			cout << "The memory was reallocated at " << ptr << endl;
			taken_memory.push_back(fragment);
			mem_dump();
			return static_cast<void*>(ptr);
		}
	}
}

// Freeing memory
// (The second param is there because we don't need output
// when this function is part of reallocating the memory)
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

// Rounding up the number of bytes (as it should divide by 4 without residual)
size_t round_up_size(size_t original_size) {
    size_t new_size = original_size;
    if (original_size % 4 != 0) {
        new_size += (4 - (original_size % 4));
    }
    return new_size;
}

// Getting the address that follows the requirements
// For example, the number of bytes reserved
// should be equal to the value of size variable
int* get_address(size_t size, int* start_pointer) {
    // the program will start searching the address to allocate memory
    // from the address of prime_variable, if it's not mentioned in params
    int * possible_pointer;
    if (start_pointer == nullptr) {
        int prime_variable = 0;
        possible_pointer = &prime_variable + 4;
    }
    else {
        possible_pointer = start_pointer;
        for (auto & memory_fragment : taken_memory) {
            if (memory_fragment.pointer == start_pointer) {
                possible_pointer += memory_fragment.size;
                break;
            }
        }
    }
    // searching for an address that doesn't overlap with anything else
    while (true) {
        bool nothing_overlaps = true;
        for (auto & memory_fragment : taken_memory) {
            if (memory_fragment.pointer < possible_pointer + size
            && memory_fragment.pointer > possible_pointer - memory_fragment.size) {
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

// Outputting the memory statistics
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