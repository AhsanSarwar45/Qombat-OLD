#pragma once

#include <QMBTPCH.hpp>

#include "AllocatorData.hpp"

namespace QMBT
{
	class AllocatorData;

	class MemoryManager
	{
	  public:
		explicit MemoryManager(Size applicationBudget);
		~MemoryManager();

		static MemoryManager& GetInstance();

		void Register(std::shared_ptr<AllocatorData> allocatorData);
		void UnRegister(std::shared_ptr<AllocatorData> allocatorData);

		inline void UpdateTotalSize(Size size) { m_TotalAllocatedSize += size; }

		Size GetUsedAllocatedSize() const;
		inline Size GetTotalAllocatedSize() const { return m_TotalAllocatedSize; }
		inline Size GetApplicationMemoryBudget() const { return m_ApplicationBudget; }
		inline const AllocatorVector& GetAllocators() const { return m_Allocators; }

	  private:
		AllocatorVector m_Allocators;

		Size m_ApplicationBudget;
		Size m_TotalAllocatedSize;
	};
} // namespace QMBT
