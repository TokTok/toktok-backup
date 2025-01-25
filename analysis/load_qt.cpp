#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QTextStream>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QMap<QString, QMap<QString, QJsonObject>> repos;

    QDir dir("repositories");
    for (const QString& repo : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QMap<QString, QJsonObject> issues;
        QDir issuesDir(dir.filePath(repo) + "/issues");
        for (const QString& issue : issuesDir.entryList(QDir::Files)) {
            QFile file(issuesDir.filePath(issue));
            file.open(QIODevice::ReadOnly);
            issues[issue] = QJsonDocument::fromJson(file.readAll()).object();
        }
        QDir pullsDir(dir.filePath(repo) + "/pulls");
        for (const QString& pull : pullsDir.entryList(QDir::Files)) {
            QFile file(pullsDir.filePath(pull));
            file.open(QIODevice::ReadOnly);
            issues[pull] = QJsonDocument::fromJson(file.readAll()).object();
        }
        repos[repo] = issues;
    }

    QTextStream out(stdout);
    for (auto it = repos.begin(); it != repos.end(); ++it) {
        out << it.key() << ": " << it.value().size() << "\n";
    }

    return 0;
}
