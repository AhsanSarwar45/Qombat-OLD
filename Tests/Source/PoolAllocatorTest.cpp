#include <vector>

#include <Qombat/Tests.hpp>
#include <catch2/catch_test_macros.hpp>

#include "MemoryTestObjects.hpp"

using namespace QMBT;

TEST_CASE("PoolAllocator Initialisation Test", "[Memory]")
{
	PoolAllocator<TestObject> poolAllocator = PoolAllocator<TestObject>("Allocator", 50);

	REQUIRE(poolAllocator.GetUsedSize() == 0);
}

TEST_CASE("PoolAllocator Allocation Test", "[Memory]")
{
	PoolAllocator<TestObject> poolAllocator = PoolAllocator<TestObject>("Allocator", 50);
	std::vector<TestObject*> objectPtrs = std::vector<TestObject*>();

	int numAllocations = 0;

		SECTION("Single Allocation")
	{
		numAllocations = 1;
	}

	SECTION("Multiple Allocations")
	{
		numAllocations = 8;
	}

	for (int i = 0; i < numAllocations; i++)
	{
		objectPtrs.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
	}

	REQUIRE(poolAllocator.GetUsedSize() == numAllocations * sizeof(TestObject));

	for (int i = 0; i < numAllocations; i++)
	{

		REQUIRE(objectPtrs[i]->a == i);
		REQUIRE(objectPtrs[i]->b == 2.1f + i);
		REQUIRE(objectPtrs[i]->c == 'a' + i);
		REQUIRE(objectPtrs[i]->d == numAllocations % 2);
		REQUIRE(objectPtrs[i]->e == 10.6f + (2 * i));
	}
}

TEST_CASE("PoolAllocator Deallocation Test", "[Memory]")
{
	PoolAllocator<TestObject> poolAllocator = PoolAllocator<TestObject>("Allocator", 50);
	std::vector<TestObject*> objectPtrs = std::vector<TestObject*>();

	int numAllocations = 0;

	SECTION("Normal Order")
	{

		SECTION("Single Allocation")
	{
		numAllocations = 1;
	}

	SECTION("Multiple Allocations")
	{
		numAllocations = 8;
	}

		for (int i = 0; i < numAllocations; i++)
		{
			objectPtrs.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
		}

		for (int i = 0; i < numAllocations; i++)
		{
			poolAllocator.Delete(objectPtrs[i]);
		}
	}

	SECTION("Reverse Order")
	{

		SECTION("Single Allocation")
	{
		numAllocations = 1;
	}

	SECTION("Multiple Allocations")
	{
		numAllocations = 8;
	}

		for (int i = 0; i < numAllocations; i++)
		{
			objectPtrs.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
		}

		for (int i = numAllocations - 1; i >= 0; i--)
		{
			poolAllocator.Delete(objectPtrs[i]);
		}
	}

	REQUIRE(poolAllocator.GetUsedSize() == 0);
}

TEST_CASE("PoolAllocator Reallocation Test", "[Memory]")
{
	PoolAllocator<TestObject> poolAllocator = PoolAllocator<TestObject>("Allocator", 50);
	std::vector<TestObject*> objectPtrs = std::vector<TestObject*>();
	int numAllocations = 0;

	SECTION("Normal Order")
	{

		SECTION("Single Allocation")
	{
		numAllocations = 1;
	}

	SECTION("Multiple Allocations")
	{
		numAllocations = 8;
	}

		for (int i = 0; i < numAllocations; i++)
		{
			objectPtrs.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
		}

		for (int i = 0; i < numAllocations; i++)
		{
			poolAllocator.Delete(objectPtrs[i]);
		}
	}

	SECTION("Reverse Order")
	{
		SECTION("Single Allocation")
	{
		numAllocations = 1;
	}

	SECTION("Multiple Allocations")
	{
		numAllocations = 8;
	}

		for (int i = 0; i < numAllocations; i++)
		{
			objectPtrs.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
		}

		for (int i = numAllocations - 1; i >= 0; i--)
		{
			poolAllocator.Delete(objectPtrs[i]);
		}
	}

	std::vector<TestObject*> objectPtrs2 = std::vector<TestObject*>();

	for (int i = 0; i < numAllocations; i++)
	{
		objectPtrs2.push_back(poolAllocator.New(i, 2.1f + i, 'a' + i, numAllocations % 2, 10.6f + (2 * i)));
	}

	REQUIRE(poolAllocator.GetUsedSize() == numAllocations * sizeof(TestObject));

	for (int i = 0; i < numAllocations; i++)
	{

		REQUIRE(objectPtrs2[i]->a == i);
		REQUIRE(objectPtrs2[i]->b == 2.1f + i);
		REQUIRE(objectPtrs2[i]->c == 'a' + i);
		REQUIRE(objectPtrs2[i]->d == numAllocations % 2);
		REQUIRE(objectPtrs2[i]->e == 10.6f + (2 * i));
	}
}
