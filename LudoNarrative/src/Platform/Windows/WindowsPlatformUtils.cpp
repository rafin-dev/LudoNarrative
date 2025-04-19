#include "ldpch.h"

#include "ludo/Core/Application.h"
#include "ludo/Utils/PlatformUtils.h"

#include "Platform/Windows/WindowsWindow.h"

#include <Windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <ShlObj_core.h>
#include <shlwapi.h>

namespace Ludo {

	std::filesystem::path FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn = {};
		CHAR szFile[260] = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = static_cast<WindowsWindow&>(Application::Get().GetWindow()).GetHandle();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::filesystem::path();
	}

	std::filesystem::path FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn = {};
		CHAR szFile[260] = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = static_cast<WindowsWindow&>(Application::Get().GetWindow()).GetHandle();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::filesystem::path();
	}

	std::filesystem::path FileDialogs::GetFolder()
	{
		char pathBuffer[MAX_PATH] = {};

		BROWSEINFOA info = {};
		info.hwndOwner = ((WindowsWindow&)Application::Get().GetWindow()).GetHandle();
		info.pidlRoot = NULL;
		info.pszDisplayName = pathBuffer;
		info.lpszTitle = "Select Project Folder";
		info.lpfn = NULL;
		info.ulFlags = BIF_USENEWUI;

		if (LPITEMIDLIST idList = SHBrowseForFolderA(&info))
		{
			memset(pathBuffer, 0, sizeof(pathBuffer));
			if (SHGetPathFromIDListA(idList, pathBuffer))
			{
				return info.pszDisplayName;
			}
			
			CoTaskMemFree(idList);
		}

		return std::filesystem::path();
	}

	std::filesystem::path FileDialogs::GetDocumentsFolder()
	{
		std::string path;
		PWSTR pPath = NULL;
		if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pPath) == S_OK)
		{
			int wlen = lstrlenW(pPath);
			int len = WideCharToMultiByte(CP_ACP, 0, pPath, wlen, NULL, 0, NULL, NULL);
			if (len > 0)
			{
				path.resize(len + 1);
				WideCharToMultiByte(CP_ACP, 0, pPath, wlen, &path[0], len, NULL, NULL);
				path[len] = '\\';
			}
			CoTaskMemFree(pPath);
		}

		return path;
	}

	void FileDialogs::CopyFile_(const std::filesystem::path& existingFile, const std::filesystem::path& newFile)
	{
		CopyFileW(existingFile.c_str(), newFile.c_str(), FALSE);
	}

}