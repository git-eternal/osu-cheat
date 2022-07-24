#include <cstdint>
#include <cstddef>
#include <Windows.h>

namespace sed
{
	class protect_guard
	{
		protect_guard(protect_guard &&) = delete;
		protect_guard(const protect_guard &&) = delete;
		protect_guard(protect_guard &) = delete;
		protect_guard(const protect_guard &) = delete;

	public:
		protect_guard(void * address, std::size_t size, DWORD prot_flags = PAGE_EXECUTE_READWRITE);
		~protect_guard();

		operator bool() const noexcept;

	private:
		void *      address;
		std::size_t size;
		DWORD       oprot;
	};
}