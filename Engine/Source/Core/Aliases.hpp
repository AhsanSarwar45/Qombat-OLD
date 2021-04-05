#pragma once

#include <QMBTPCH.hpp>

namespace QMBT
{
	using Byte = uint8_t;
	using Size = size_t;

	using UInt8 = uint8_t;
	using UInt16 = uint16_t;
	using UInt32 = uint32_t;
	using UInt64 = uint64_t;

	using Int8 = int8_t;
	using Int16 = int16_t;
	using Int32 = int32_t;
	using Int64 = int64_t;

	using ULLInt = unsigned long long int;

	inline Size operator"" _KB(ULLInt x) { return 1024 * x; }
	inline Size operator"" _MB(ULLInt x) { return 1024 * 1024 * x; }
	inline Size operator"" _GB(ULLInt x) { return 1024 * 1024 * 1024 * x; }

	constexpr Size Bits(Size x)
	{
		return 8 * x;
	}
	constexpr Size Kb(Size x)
	{
		return 8 * 1024 * x;
	}
	constexpr Size Mb(Size x)
	{
		return 8 * 1024 * 1024 * x;
	}
	// ToGb gives integer overflow with type "Size"

	constexpr Size KB(Size x)
	{
		return 1024 * x;
	}
	constexpr Size MB(Size x)
	{
		return 1024 * 1024 * x;
	}
	constexpr Size GB(Size x)
	{
		return 1024 * 1024 * 1024 * x;
	}

	constexpr Size ToKB(Size x)
	{
		return x / 1024;
	}

	constexpr Size ToMB(Size x)
	{
		return ToKB(x) / 1024;
	}

	constexpr Size ToGB(Size x)
	{
		return ToMB(x) / 1024;
	}

	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using WeakRef = std::weak_ptr<T>;
	template <typename T, typename... Args>
	constexpr WeakRef<T> CreateWeakRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	constexpr bool IsUninitialized(WeakRef<T> const& weak)
	{
		return !weak.owner_before(WeakRef<T>{}) && !WeakRef<T>{}.owner_before(weak);
	}
} // namespace QMBT