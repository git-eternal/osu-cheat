#include <sed/macro.hpp>
#include <sed/memory.hpp>

namespace sdk
{
	union raw_mode_info_t
	{
		OC_UNS_PAD(0xC, bool, is_raw)
	};

	class pp_raw_mode_info_t : public sed::basic_ptrptr<raw_mode_info_t> {};
}