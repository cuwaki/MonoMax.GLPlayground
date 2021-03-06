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
		return CWString(astr.begin(), astr.end());
	}

	CString ToASCII(const CWString& wstr)
	{
		return CString(wstr.begin(), wstr.end());
	}

	void Quat2Euler(glm::quat& q, float& pitch, float& yaw, float& roll)
	{
		float test = q.x * q.y + q.z * q.w;
		if (test > 0.499) { // singularity at north pole
			yaw = 2 * atan2f(q.x, q.w);
			roll = 3.141592f / 2;
			pitch = 0;
			return;
		}
		if (test < -0.499) { // singularity at south pole
			yaw = -2 * atan2f(q.x, q.w);
			roll = -3.141592f / 2;
			pitch = 0;
			return;
		}
		float sqx = q.x * q.x;
		float sqy = q.y * q.y;
		float sqz = q.z * q.z;
		yaw = atan2f(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz);
		roll = asinf(2 * test);
		pitch = atan2f(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz);
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
