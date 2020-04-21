#pragma once

#include <vector>
#include <string>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
//#include <filesystem>

// With Visual Studio compiler, filesystem is still "experimental"
namespace fs = std::experimental::filesystem;

namespace imgui_ext {

    const struct file {
        std::string alias;
        fs::path path;
    };

    class file_browser_modal final {

        static const int modal_flags;
        const char* m_title;
        bool m_oldVisibility;
        int m_selection;
        fs::path m_currentPath;
        bool m_currentPathIsDir;
        std::vector<file> m_filesInScope;
        fs::path m_initialPath;
        const char* m_filter;

    public:
        file_browser_modal(const char* title, const char* initial_path, const char* filter = "");
        const bool render(const bool isVisible, std::string& outPath);
    };

};