#ifndef USEFULFUNCS_H
#define	USEFULFUNCS_H

char * strUTF8(const char * str)
{
		int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* result = new char[len + 1];
		memset(result, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, result, len, NULL, NULL);
		if (wstr)delete[] wstr;
		return result;
}

#endif // !USEFULFUNCS_H
