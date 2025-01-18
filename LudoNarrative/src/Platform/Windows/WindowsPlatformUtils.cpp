#include "ldpch.h"

#include "ludo/Core/Application.h"
#include "ludo/Utils/PlatformUtils.h"

#include "Platform/Windows/WindowsWindow.h"

#include <Windows.h>
#include <commdlg.h>

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

}