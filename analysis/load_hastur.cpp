#include "json/json.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <string_view>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <windows.h>
#endif

using namespace json;

namespace {
template <typename T>
using opt_or_void = std::conditional_t<std::is_void_v<T>, void, std::optional<T>>;

template <typename F, typename... Args>
using result_or_void = opt_or_void<std::invoke_result_t<F, Args...>>;

template <typename T>
constexpr T default_value()
{
    return {};
}

template <>
constexpr void default_value<void>()
{
}

#ifndef _WIN32
struct unique_fd
{
    int fd = -1;
    ~unique_fd()
    {
        close(fd);
    }
};

struct unique_mapped_file
{
    void* data = nullptr;
    std::size_t size = 0;
    ~unique_mapped_file()
    {
        if (data != nullptr) {
            munmap(data, size);
        }
    }
};

template <typename F>
auto with_mapped_file(const std::filesystem::directory_entry& path, F&& f)
{
    const unique_fd fd{open(path.path().c_str(), O_RDONLY)};
    if (fd.fd == -1) {
        std::cerr << "Failed to open " << path << "\n";
        return default_value<result_or_void<F, std::string_view>>();
    }
    const unique_mapped_file mapped_file{
        mmap(nullptr, path.file_size(), PROT_READ, MAP_PRIVATE, fd.fd, 0),
        path.file_size(),
    };
    if (mapped_file.data == MAP_FAILED || mapped_file.data == nullptr) {
        std::cerr << "Failed to map " << path << "\n";
        return default_value<result_or_void<F, std::string_view>>();
    }
    std::string_view file_contents(static_cast<char*>(mapped_file.data), mapped_file.size);
    return f(file_contents);
}
#else
struct unique_handle
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    ~unique_handle()
    {
        if (handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }
};

struct unique_mapped_file
{
    void* data = nullptr;
    ~unique_mapped_file()
    {
        if (data != nullptr) {
            UnmapViewOfFile(data);
        }
    }
};

template <typename F>
auto with_mapped_file(const std::filesystem::directory_entry& path, F&& f)
{
    unique_handle hFile{
        CreateFile(path.path().string().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr),
    };
    if (hFile.handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open " << path << "\n";
        return default_value<result_or_void<F, std::string_view>>();
    }
    unique_handle hMap{CreateFileMapping(hFile.handle, nullptr, PAGE_READONLY, 0, 0, nullptr)};
    if (hMap.handle == nullptr) {
        std::cerr << "Failed to map " << path << "\n";
        return default_value<result_or_void<F, std::string_view>>();
    }
    unique_mapped_file mapped_file{MapViewOfFile(hMap.handle, FILE_MAP_READ, 0, 0, 0)};
    std::string_view file_contents(static_cast<char*>(mapped_file.data), path.file_size());
    return f(file_contents);
}
#endif
} // namespace

int main()
{
    std::map<std::string, std::map<std::string, Value>> repos;

    for (const auto& repo : std::filesystem::directory_iterator("repositories")) {
        std::map<std::string, Value> issues;
        if (!std::filesystem::is_directory(repo.path() / "issues")) {
            continue;
        }

        for (const auto& issue : std::filesystem::directory_iterator(repo.path() / "issues")) {
            with_mapped_file(issue, [&](std::string_view file_contents) {
                issues[issue.path().filename().string()] = parse(file_contents).value();
            });
        }
        for (const auto& pull : std::filesystem::directory_iterator(repo.path() / "pulls")) {
            with_mapped_file(pull, [&](std::string_view file_contents) {
                issues[pull.path().filename().string()] = parse(file_contents).value();
            });
        }
        repos[repo.path().filename().string()] = issues;
    }

    for (const auto& [repo, issues] : repos) {
        std::cout << repo << ": " << issues.size() << "\n";
    }

    return 0;
}
