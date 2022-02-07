#pragma once
#include <cstdint>
#include <cstdlib>
#include <type_traits>

extern "C"  void* _syscaller_stub();

namespace syscall
{
	template <typename... Args>
	static constexpr void* syscall_stub_helper(Args... args) noexcept
	{
		auto fn = reinterpret_cast<void* (*)(Args...)>(&_syscaller_stub);
		return fn(args...);
	}

	template <std::size_t argc, typename>
	struct argument_remapper
	{
		// At least 5 params
		template<typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack>
			static constexpr void* do_call(std::uint32_t idx,
				First first, Second second,
				Third third, Fourth fourth,
				Pack... pack) noexcept
		{
			return syscall_stub_helper(first, second, third, fourth, idx, nullptr, pack...);
		}
	};

	template <std::size_t Argc> struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		// 4 or less params
		template<typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*>
			static constexpr void* do_call(std::uint32_t idx,
				First first = First{},
				Second second = Second{},
				Third third = Third{},
				Fourth fourth = Fourth{}) noexcept
		{
			return syscall_stub_helper(first, second, third, fourth, idx, nullptr);
		}
	};
}

template<typename Return, typename... Args>
constexpr Return _syscall(std::uint32_t idx, Args... args) noexcept
{
	using mapper = syscall::argument_remapper<sizeof...(Args), void>;
	return (Return)mapper::do_call(idx, args...);
}
