#include "jsonreader.h"
#include <QFile>
#include <QTextStream>

JsonReader::JsonReader()
{
}

bool JsonReader::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    QString jsonString = stream.readAll();
    file.close();
    
    return loadFromString(jsonString);
}

bool JsonReader::loadFromString(const QString &jsonString)
{
    QJsonParseError error;
    m_document = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        return false;
    }
    
    if (!m_document.isObject()) {
        return false;
    }
    
    m_rootObject = m_document.object();
    return true;
}

QJsonArray JsonReader::getGroups() const
{
    return m_rootObject["Groups"].toArray();
}