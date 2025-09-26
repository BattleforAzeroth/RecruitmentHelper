#ifndef SNIPPETMANAGER_H
#define SNIPPETMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QString>
#include <QVector>

struct Snippet {
    QString title;
    QString content;
    bool wrap;
};

struct SnippetGroup {
    QString name;
    QVector<Snippet> snippets;
};

class SnippetManager : public QObject
{
    Q_OBJECT
public:
    explicit SnippetManager(QObject *parent = nullptr);
    bool loadFromFile(const QString &filename);
    bool loadFromJson(const QJsonDocument &jsonDoc);
    QVector<SnippetGroup> getGroups() const;

private:
    QVector<SnippetGroup> m_groups;
};

#endif // SNIPPETMANAGER_H