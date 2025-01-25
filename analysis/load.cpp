#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

int main()
{
    std::map<std::string, std::map<std::string, json>> repos;

    for (const auto& repo : std::filesystem::directory_iterator("repositories")) {
        std::map<std::string, json> issues;
        for (const auto& issue : std::filesystem::directory_iterator(repo.path() / "issues")) {
            std::ifstream file(issue.path());
            issues[issue.path().filename()] = json::parse(file);
        }
        for (const auto& pull : std::filesystem::directory_iterator(repo.path() / "pulls")) {
            std::ifstream file(pull.path());
            issues[pull.path().filename()] = json::parse(file);
        }
        repos[repo.path().filename()] = issues;
    }

    for (const auto& [repo, issues] : repos) {
        std::cout << repo << ": " << issues.size() << "\n";
    }

    return 0;
}
