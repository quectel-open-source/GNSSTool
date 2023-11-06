#ifndef JSONOPERATE_H
#define JSONOPERATE_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QFile>
#include <QDir>

class JsonOperate
{
public:
    JsonOperate(QString jsonFileName);
    ~JsonOperate();

    bool WriteJsonFile();
private:

    void ReadJsonFile();


    void JsonRemoveItem(QJsonObject &jsonObject, QString key);

    void JsonFileInit();
public:

    static void JsonAddItem(QJsonObject &jsonObject, QString key, QJsonValue value)
    {
        jsonObject.insert(key, value);
    }

    static bool JsonContains(QJsonObject jsonObject, QString key)
    {
        if (jsonObject.contains(key))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static QJsonValue JsonGetValue(QJsonObject jsonObject, QString key)
    {
        QJsonValue value = QJsonValue::Null;
        if (jsonObject.contains(key))
        {
            value = jsonObject.value(key);
        }
        return value;
    }


    static QJsonObject JsonGetObject(QJsonObject jsonObject, QString key)
    {
        QJsonObject object{};
        if (jsonObject.contains(key))
        {
            QJsonValue value = jsonObject.value(key);
            object = value.toObject();
        }
        return object;
    };
    QJsonObject JsonObjectGet(void);

    void WriteJson(QString key, QJsonValue value);

public:
    QJsonObject m_jsonObject{};
    QString     m_jsonFileName;
private:
};

#endif // JSONOPERATE_H

