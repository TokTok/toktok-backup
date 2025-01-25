#include <filesystem>
#include <iostream>
#include <map>
#include <simdjson.h>
#include <string>

int main()
{
    std::map<std::string, std::map<std::string, simdjson::simdjson_result<simdjson::dom::element>>> repos;

    for (const auto& repo : std::filesystem::directory_iterator("repositories")) {
        std::map<std::string, simdjson::simdjson_result<simdjson::dom::element>> issues;
        for (const auto& issue : std::filesystem::directory_iterator(repo.path() / "issues")) {
            simdjson::dom::parser parser;
            auto doc = parser.load(issue.path());
            issues[issue.path().filename()] = doc;
        }
        for (const auto& pull : std::filesystem::directory_iterator(repo.path() / "pulls")) {
            simdjson::dom::parser parser;
            auto doc = parser.load(pull.path());
            issues[pull.path().filename()] = doc;
        }
        repos[repo.path().filename()] = issues;
    }

    for (const auto& [repo, issues] : repos) {
        std::cout << repo << ": " << issues.size() << "\n";
    }

    return 0;
}
