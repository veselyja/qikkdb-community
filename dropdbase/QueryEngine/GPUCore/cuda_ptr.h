#pragma once

#include <memory>

#include "GPUMemory.cuh"

/// A class wrapper for a RAII CUDA pointer for buffer allocationand automatic deallocation
template <typename T>
class cuda_ptr
{
private:
    /// The raw pointer
    std::unique_ptr<T, void (*)(void*)> pointer_;

public:
    cuda_ptr(const cuda_ptr&) = delete;
    cuda_ptr& operator=(const cuda_ptr&) = delete;

    cuda_ptr(cuda_ptr&& other) : pointer_(std::move(other.pointer_))
    {
    }

    cuda_ptr& operator=(cuda_ptr&& other)
    {
        pointer_ = std::move(other.pointer_);
    }

    /// Constructor
    /// <param name="dataElementCount"> Chunk of memory of size sizeof(T) * dataElementCount </param>
    explicit cuda_ptr(size_t dataElementCount)
    : pointer_(nullptr, &GPUMemory::free) // TODO bind CudaMemAllocator for correct graphic card
    {
        T* rawPointer;
        GPUMemory::alloc(&rawPointer, dataElementCount);
        pointer_.reset(rawPointer);
    }

    /// Constructor
    /// <param name="dataElementCount"> Chunk of memory of size sizeof(T) * dataElementCount
    /// </param> <param name="value"> The value to pre-fill the memory with </param>
    cuda_ptr(size_t dataElementCount, int value) : pointer_(nullptr, &GPUMemory::free)
    {
        T* rawPointer;
        GPUMemory::allocAndSet(&rawPointer, value, dataElementCount);
        pointer_.reset(rawPointer);
    }

    /// This method returns the raw pointer
    /// <returns> Raw CUDA pointer embedded in the class</returns>
    T* get() const
    {
        return pointer_.get();
    }
};
