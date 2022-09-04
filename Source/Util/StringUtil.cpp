#include "StringUtil.h"

namespace acc3d
{
	namespace Util
	{
		std::string WCharArrayToStdString(wchar_t* ptr, size_t size)
		{
			size_t const strSize = size;
			char* const convertedData = new char[strSize];
			size_t charsConverted = 0;
			wcstombs_s(&charsConverted, convertedData, strSize, ptr, 128);
			return { convertedData };
		}
	}
}