#include "FastIPC.h"
#include "common/util/StrUtil.h"

namespace fastipc{
	std::wstring genServerName(){
		return jw::GenerateGuidW();
	}
	std::wstring genServerName(const std::wstring servername){
		return servername + L"_c";
	}
	std::wstring genWritedEventName(const std::wstring servername){
		return servername + L"_w";
	}
	std::wstring genReadedEventName(const std::wstring servername){
		return servername + L"_r";
	}
	std::wstring genMappingFileName(const std::wstring servername){
		return servername + L"_m";
	}
}
