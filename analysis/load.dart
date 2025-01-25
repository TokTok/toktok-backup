import 'dart:convert';
import 'dart:io';

void main() {
  var repos = <String, Map<String, dynamic>>{};

  for (var repo in Directory("repositories").listSync()) {
    var issues = repos[repo.path] = {};
    for (var issue in Directory("${repo.path}/issues").listSync()) {
      issues[issue.path] = jsonDecode(File(issue.path).readAsStringSync());
    }
    for (var pull in Directory("${repo.path}/pulls").listSync()) {
      issues[pull.path] = jsonDecode(File(pull.path).readAsStringSync());
    }
  }

  for (var repo in repos.keys.toList()..sort()) {
    print("$repo: ${repos[repo]!.length}");
  }
}
