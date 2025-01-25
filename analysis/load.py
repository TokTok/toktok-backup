import json
import os
from typing import Any

repos: dict[str, dict[str, Any]] = {}

for repo in os.listdir("repositories"):
    issues = repos[repo] = {}
    for issue in os.listdir(os.path.join("repositories", repo, "issues")):
        with open(os.path.join("repositories", repo, "issues", issue), "rb") as file:
            issues[issue] = json.load(file)
    for pull in os.listdir(os.path.join("repositories", repo, "pulls")):
        with open(os.path.join("repositories", repo, "pulls", pull), "rb") as file:
            issues[pull] = json.load(file)

for repo, issues in sorted(repos.items(), key=lambda x: x[0]):
    print(f"{repo}: {len(issues)}")
