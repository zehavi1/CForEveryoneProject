#pragma once
#include <iostream>
#include <memory>
#include <vector>
class AllocationMemory
{
    std::vector<void*> allocatedPointers;
	void* ptr;
public:
	void addAllocation(void* ptr)
	{
        if (ptr) {
            allocatedPointers.push_back(ptr);
        }
	}
    void freeAll() {
        for (void* ptr : allocatedPointers) {
            free(ptr);
        }
        allocatedPointers.clear();
    }
};

