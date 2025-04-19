#pragma once

#include <filesystem>

namespace Ludo {

	class FileDialogs
	{
	public:
		// Returns empty string if cancelled
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
		static std::filesystem::path GetFolder();

		static std::filesystem::path GetDocumentsFolder();

		// I hae win32's lack of something to not mess up naming
		static void CopyFile_(const std::filesystem::path& existingFile, const std::filesystem::path& newFile);
	};

}