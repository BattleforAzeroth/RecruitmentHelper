#include "snippetmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

SnippetManager::SnippetManager(QObject *parent) : QObject(parent)
{
}

bool SnippetManager::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << filename;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    return loadFromJson(jsonDoc);
}

bool SnippetManager::loadFromJson(const QJsonDocument &jsonDoc)
{
    if (!jsonDoc.isObject()) {
        qWarning() << "Invalid JSON structure: root should be an object";
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonArray groupsArray = rootObj["Groups"].toArray();

    m_groups.clear();

    for (const QJsonValue &groupValue : groupsArray) {
        QJsonObject groupObj = groupValue.toObject();
        SnippetGroup group;
        
        group.name = groupObj["Name"].toString();
        QJsonArray snippetsArray = groupObj["Snippets"].toArray();
        
        for (const QJsonValue &snippetValue : snippetsArray) {
            QJsonObject snippetObj = snippetValue.toObject();
            Snippet snippet;
            
            snippet.title = snippetObj["Title"].toString();
            snippet.content = snippetObj["Content"].toString();
            
            // Check if wrap property exists and is set to "true"
            if (snippetObj.contains("wrap")) {
                snippet.wrap = (snippetObj["wrap"].toString().toLower() == "true");
            } else {
                snippet.wrap = false;
            }
            
            group.snippets.append(snippet);
        }
        
        m_groups.append(group);
    }
    
    return true;
}

QVector<SnippetGroup> SnippetManager::getGroups() const
{
    return m_groups;
}