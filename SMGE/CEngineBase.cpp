#include "CEngineBase.h"

namespace SMGE
{
	namespace nsGE
	{
		CEngineBase::CEngineBase()
		{
		}

		CEngineBase::~CEngineBase()
		{
		}

		void CEngineBase::Tick(float)
		{
		}

		void CEngineBase::Render(float)
		{
		}
	}
}

// ���� �� ��
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
	{	// �߰� �� �� - ���� ����� �ȵ� �� ����
		//CWString ret = converter.from_bytes(astr);
		CWString ret(astr.begin(), astr.end());
		//ret.assign(astr.begin(), astr.end());
		return ret;
	}

	CString ToASCII(const CWString& wstr)
	{	// �߰� �� �� - ���� ����� �ȵ� �� ����
		//CString ret = converter.to_bytes(wstr);
		CString ret(wstr.begin(), wstr.end());
		//ret.assign;
		return ret;
	}

	namespace SMGEGlobal
	{
		CWString GetNormalizedPath(const CWString& path)
		{
			return CuwakiDevUtils::Replace(path, L"\\", L"/");
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
