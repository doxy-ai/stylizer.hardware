#include <filesystem>
#include <expected>

#include "thirdparty/mio.hpp"

namespace stylizer {
	inline namespace filesystem {
		using namespace std::filesystem;
		using namespace mio;
	}

	template<typename F, typename... Targs>
	auto load_from_file(const F& memory_loader, const filesystem::path& path, Targs... args) {
		using return_t = decltype(memory_loader({(std::byte*)nullptr, 0}, std::forward<Targs>(args)...));
		using expected_t = std::expected<return_t, std::error_code>;

		std::error_code error;
		mio::mmap_source mmap = mio::make_mmap_source(path.c_str(), error);
		if constexpr(std::is_same_v<return_t, void>) {
			if(error) return error;
		} else if(error) return std::unexpected<expected_t>(error);

		if constexpr(std::is_same_v<return_t, void>) {
			memory_loader({(std::byte*)mmap.data(), mmap.size()}, std::forward<Targs>(args)...);
			return error;
		} else return expected_t(memory_loader({(std::byte*)mmap.data(), mmap.size()}, std::forward<Targs>(args)...));
	}
}