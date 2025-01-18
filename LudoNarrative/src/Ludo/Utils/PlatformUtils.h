#pragma once

#include <filesystem>

namespace Ludo {

	class FileDialogs
	{
	public:
		// Returns empty string if cancelled
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
	};

}