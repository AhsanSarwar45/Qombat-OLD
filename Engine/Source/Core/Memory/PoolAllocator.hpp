#pragma once

#include <QMBTPCH.hpp>

#include "Core/Memory/MemoryManager.hpp"

namespace QMBT
{

	struct Chunk
	{
		/*
		When a chunk is free, the `next` contains the
		address of the next chunk in a list.
		
		When it's allocated, this space is used by
		the user.
		*/
		Chunk* next;
	};

	/**
	 * @brief A templated allocator that can only be used to allocate memory for a 
	 * collection of the same type. 
	 * 
	 * @tparam Object 
	 */
	template <typename Object>
	class PoolAllocator
	{
	  public:
		//Prohibit default construction, moving and assignment
		// ! For some reason, the default constructor does not get deleted
		
		PoolAllocator(const PoolAllocator&) = delete;
		PoolAllocator(PoolAllocator&&) = delete;
		PoolAllocator& operator=(const PoolAllocator&) = delete;
		PoolAllocator& operator=(PoolAllocator&&) = delete;

		/**
		 * @brief Construct a new Pool Allocator object.
		 * 
		 * @param debugName The name that will appear in logs and any editor.
		 * @param chunksPerBlock After this many items have been allocated, the allocator allocates
		 * a new block of size equal to chunksPerBlock * sizeof(Object). 
		 */
		PoolAllocator(const std::string& debugName = "Allocator", Size chunksPerBlock = 8);
		~PoolAllocator();

		/**
		 * @brief Gets an address in the pool, constructs the object at the address and returns the address
		 * 
		 * @return Object* The pointer to the newly allocated memory
		 */
		void* Allocate();

		template <typename... Args>
		Object* New(Args... argList)
		{
			void* address = Allocate();				 // Allocate the raw memory and get a pointer to it
			return new (address) Object(argList...); //Call the placement new operator, which constructs the Object
		}

		void Deallocate(Object* ptr);
		void Delete(Object* ptr);

		inline Size GetUsedSpace() const { return m_Data->UsedSize; }

	  private:
	  PoolAllocator();
		PoolAllocator(PoolAllocator&);
		Chunk* AllocateBlock(Size chunkSize);

	  private:
		Ref<AllocatorData> m_Data;

		Size m_ChunksPerBlock;
		Size m_ObjectSize;

		Chunk* m_HeadPtr = nullptr;
	};

	template <typename Object>
	PoolAllocator<Object>::PoolAllocator(const std::string& debugName, Size chunksPerBlock)
		: m_ChunksPerBlock(chunksPerBlock)
	{
		QMBT_CORE_ASSERT(m_ChunksPerBlock > 0, "Chunks per block have to be more than 0!");

		m_ObjectSize = sizeof(Object);

		m_Data = std::make_shared<AllocatorData>(debugName, 0);
		m_HeadPtr = AllocateBlock(m_ObjectSize);

		MemoryManager::GetInstance()
			.Register(m_Data);
	}

	template <typename Object>
	PoolAllocator<Object>::~PoolAllocator()
	{
		MemoryManager::GetInstance().UnRegister(m_Data);
		free(m_HeadPtr);
	}

	template <typename Object>
	void* PoolAllocator<Object>::Allocate()
	{
		// No chunks left in the current block, or no any block
		// exists yet. Allocate a new one, passing the chunk size:

		if (m_HeadPtr == nullptr)
		{
			m_HeadPtr = AllocateBlock(m_ObjectSize);
		}

		// The return value is the current position of
		// the allocation pointer:

		Chunk* freeChunk = m_HeadPtr;

		// Advance (bump) the allocation pointer to the next chunk.
		//
		// When no chunks left, the `mAlloc` will be set to `nullptr`, and
		// this will cause allocation of a new block on the next request:

		m_HeadPtr = m_HeadPtr->next;

		m_Data->UsedSize += m_ObjectSize;
		LOG_CORE_INFO("{0} Allocated {1} bytes", m_Data->DebugName, m_ObjectSize);

		return freeChunk;
	}
	template <typename Object>
	void PoolAllocator<Object>::Deallocate(Object* ptr)
	{
		// The freed chunk's next pointer points to the
		// current allocation pointer:
		reinterpret_cast<Chunk*>(ptr)->next = m_HeadPtr;

		// And the allocation pointer is now set
		// to the returned (free) chunk:

		m_HeadPtr = reinterpret_cast<Chunk*>(ptr);

		m_Data->UsedSize -= m_ObjectSize;
		LOG_CORE_INFO("{0} Deallocated {1} bytes", m_Data->DebugName, m_ObjectSize);
	}

	template <typename Object>
	void PoolAllocator<Object>::Delete(Object* ptr)
	{
		ptr->~Object();	 // Call the destructor on the object
		Deallocate(ptr); // Deallocate the pointer
	}

	template <typename Object>
	Chunk* PoolAllocator<Object>::AllocateBlock(Size chunkSize)
	{
		QMBT_CORE_ASSERT(chunkSize > sizeof(Chunk), "Object size must be larger than pointer size");
		Size blockSize = m_ChunksPerBlock * chunkSize;

		// The first chunk of the new block.
		Chunk* blockBegin = reinterpret_cast<Chunk*>(malloc(blockSize));

		m_Data->TotalSize += blockSize;
		MemoryManager::GetInstance().UpdateTotalSize(blockSize);

		// Once the block is allocated, we need to chain all
		// the chunks in this block:

		Chunk* chunk = blockBegin;

		for (int i = 0; i < m_ChunksPerBlock - 1; ++i)
		{
			chunk->next =
				reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
			chunk = chunk->next;
		}

		chunk->next = nullptr;

		LOG_CORE_INFO("{0} Allocated block ({1} chunks)", m_Data->DebugName, m_ChunksPerBlock);

		return blockBegin;
	}

} // namespace QMBT