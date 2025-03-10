#pragma once

#include "thirdparty/thread_pool.hpp"

namespace stylizer {
	struct thread_pool {
	protected:
		static ZenSepiol::ThreadPool& get_thread_pool(optional<size_t> initial_pool_size = {})
	#ifdef IS_STYLIZER_CORE_CPP
		{
			static ZenSepiol::ThreadPool pool(initial_pool_size ? *initial_pool_size : std::thread::hardware_concurrency() - 1);
			return pool;
		}
	#else
		;
	#endif

	public:
		template <typename F, typename... Args>
		static auto enqueue(F&& function, optional<size_t> initial_pool_size = {}, Args&&... args) {
			return get_thread_pool(initial_pool_size).AddTask(function, args...);
		}

		struct future: std::future<void> {};
	};
}