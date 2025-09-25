#ifndef JSONREADER_H
#define JSONREADER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class JsonReader
{
public:
    JsonReader();

    bool loadFromFile(const QString &filePath);
    bool loadFromString(const QString &jsonString);

    QJsonArray getGroups() const;

private:
    QJsonDocument m_document;
    QJsonObject m_rootObject;
};

#endif // JSONREADER_H