// FileZilla Server - a Windows ftp server

// Copyright (C) 2002-2016 - Tim Kosse <tim.kosse@filezilla-project.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "stdafx.h"
#include "version.h"

#include <libfilezilla/format.hpp>

std::wstring GetVersionString(bool include_suffix)
{
	std::wstring version;

	//Fill the version info
	TCHAR fullpath[MAX_PATH + 10];
	if (GetModuleFileName(0, fullpath, MAX_PATH + 10)) {
		TCHAR *str = new TCHAR[_tcslen(fullpath) + 1];
		_tcscpy(str, fullpath);
		DWORD tmp = 0;
		DWORD len = GetFileVersionInfoSize(str, &tmp);
		char* pBlock = new char[len];
		GetFileVersionInfo(str, 0, len, pBlock);
		LPVOID ptr;
		UINT ptrlen;

		//Format the versionstring
		if (VerQueryValue(pBlock, _T("\\"), &ptr, &ptrlen)) {
			VS_FIXEDFILEINFO *fi = (VS_FIXEDFILEINFO*)ptr;

			std::wstring suffix;
			if (fi->dwFileVersionMS >> 16) {
				//v1.00+
				if (include_suffix) {
					suffix = _T(" final");
				}

				if (fi->dwFileVersionLS >> 16) {
					TCHAR ch = 'a';
					ch += static_cast<TCHAR>(fi->dwFileVersionLS >> 16) - 1;
					version = fz::sprintf(L"%d.%d%c" + suffix, fi->dwFileVersionMS >> 16, fi->dwFileVersionMS & 0xFFFF, ch);
				}
				else {
					version = fz::sprintf(L"%d.%d" + suffix, fi->dwFileVersionMS >> 16, fi->dwFileVersionMS & 0xFFFF);
				}
			}
			else {
				if (include_suffix) {
					suffix = L" beta";
				}
				//beta versions
				if ((fi->dwFileVersionLS & 0xFFFF) / 100) {
					//final version
					version = fz::sprintf(L"0.%d.%d%c" + suffix, fi->dwFileVersionMS & 0xFFFF, fi->dwFileVersionLS >> 16, (fi->dwFileVersionLS & 0xFFFF) / 100 + 'a' - 1);
				}
				else {
					//final version
					version = fz::sprintf(L"0.%d.%d" + suffix, fi->dwFileVersionMS & 0xFFFF, fi->dwFileVersionLS >> 16);
				}
			}

		}
		delete [] str;
		delete [] pBlock;
	}
	return version;
}

std::wstring GetProductVersionString()
{
	std::wstring ProductName = L"Unknown";

	//Fill the version info
	TCHAR fullpath[MAX_PATH + 10];
	if (GetModuleFileName(0, fullpath, MAX_PATH + 10)) {
		TCHAR *str = new TCHAR[_tcslen(fullpath) + 1];
		_tcscpy(str, fullpath);
		DWORD tmp = 0;
		DWORD len = GetFileVersionInfoSize(str, &tmp);
		char* pBlock = new char[len];
		GetFileVersionInfo(str, 0, len, pBlock);
		LPVOID ptr;
		UINT ptrlen;

		//Retreive the product name

		TCHAR SubBlock[50];

		// Structure used to store enumerated languages and code pages.
		struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;

		UINT cbTranslate;

		// Read the list of languages and code pages.
		if (VerQueryValue(pBlock,
					_T("\\VarFileInfo\\Translation"),
					(LPVOID*)&lpTranslate,
					&cbTranslate))
		{
			// Read the file description for each language and code page.

			_stprintf( SubBlock,
				   _T("\\StringFileInfo\\%04x%04x\\ProductName"),
				   lpTranslate[0].wLanguage,
				   lpTranslate[0].wCodePage);
			// Retrieve file description for language and code page "0".
			if (VerQueryValue(pBlock,
					SubBlock,
					&ptr,
						&ptrlen))
			{
				ProductName = (TCHAR*)ptr;
			}
		}

		delete [] str;
		delete [] pBlock;
	}
	return ProductName + L" " + GetVersionString(true);
}
