#include "GECommonIncludes.h"

// 차후 할 일
#include <locale>
#include <codecvt>
#include <string>

// deprecated c++17
//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//std::string narrow = converter.to_bytes(wide_utf16_source_string);
//std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

namespace SMGE
{
	CWString ToTCHAR(const CString& astr)
	{
		return CWString(astr.begin(), astr.end());	// 여기 - 제대로 된 처리 필요
	}

	CString ToASCII(const CWString& wstr)
	{
		return CString(wstr.begin(), wstr.end());	// 여기 - 제대로 된 처리 필요
	}

	namespace Path
	{
		CWString GetNormalizedPath(const CWString& path)
		{
			CWString ret = Replace(path, L"\\", L"/");
			return Replace(ret, L"//", L"/");	// 차후 - 없을 때까지 제대로 처리하자
		}

		bool IsValidPath(const CWString& path)
		{
			return path.length() > 0 && (path.find(L'/') != CWString::npos || path.find(L'\\') != CWString::npos);
		}

		CWString GetDirectoryFullPath(const CWString& dir)
		{
			wchar full[_MAX_PATH];
			if (_wfullpath(full, dir.c_str(), _MAX_PATH) != nullptr)
				return GetNormalizedPath(full);

			return wtext("error_path");
		}

		CWString GetDirectoryCurrent()
		{
			return GetDirectoryFullPath(wtext("./"));
		}
	}
}
