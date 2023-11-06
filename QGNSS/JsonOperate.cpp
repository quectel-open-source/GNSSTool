#include "JsonOperate.h"
#include <QDebug>


JsonOperate::JsonOperate(QString jsonFileName)
{
    m_jsonFileName = jsonFileName;

    if (!m_jsonFileName.isNull())
    {
        ReadJsonFile();
    }
}

JsonOperate::~JsonOperate()
{

}

void JsonOperate::ReadJsonFile()
{
    QFile *jsonFile = new QFile(QDir::homePath() + m_jsonFileName);
    jsonFile->setFileName(m_jsonFileName);
    //
    if (!jsonFile->open(QIODevice::ReadWrite))
    {
        return;
    }

    //read json file
    QByteArray jsonFileData = jsonFile->readAll();
    jsonFile->close();

    // convert to JSON file
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFileData, &jsonError);

    if (!jsonDoc.isNull())
    {
        qDebug()<<"found json doc";
        if (jsonError.error == QJsonParseError::NoError)
        {
            qDebug()<<"json no Error";
            // parse ok
            if (jsonDoc.isObject())
            {
                qDebug()<<"json object";
                // JSON document object
                m_jsonObject = jsonDoc.object();
            }
        }
    }
    else
    {
        //json file empty
        JsonFileInit();
    }
}

bool JsonOperate::WriteJsonFile()
{
    //json
    QFile *jsonFile = new QFile(QDir::currentPath() + m_jsonFileName);

    jsonFile->setFileName(m_jsonFileName);

    qDebug()<<"WriteJsonFile():"<<m_jsonFileName;

    if (!jsonFile->open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
        return false;
    }

    // QJsonDocument json
    QJsonDocument jsonDoc;
    jsonDoc.setObject(m_jsonObject);

    // json
    jsonFile->write(jsonDoc.toJson());
    jsonFile->close();
    return true;
}

void JsonOperate::JsonRemoveItem(QJsonObject &jsonObject, QString key)
{
    jsonObject.remove(key);
}

void JsonOperate::JsonFileInit()
{

    //GLOBAL_CONFIG
    QJsonObject globalObject;

    /* set command dialog command init value */
    globalObject.insert("COM_CMD_01", "$PQCFGNMEAMSG");
    globalObject.insert("COM_CMD_02", "$PSTMWARM");
    globalObject.insert("COM_CMD_03", "$PSTMHOT");
    globalObject.insert("COM_CMD_04", "$PSTMCOLD,F");
    globalObject.insert("COM_CMD_05", "");
    globalObject.insert("COM_CMD_06", "");
    globalObject.insert("COM_CMD_07", "");
    globalObject.insert("COM_CMD_08", "");
    globalObject.insert("COM_CMD_09", "");
    globalObject.insert("COM_CMD_10", "");
    globalObject.insert("COM_CMD_11", "");
    globalObject.insert("COM_CMD_12", "");
    globalObject.insert("COM_CMD_13", "");
    globalObject.insert("COM_CMD_14", "");
    globalObject.insert("COM_CMD_15", "");
    globalObject.insert("COM_CMD_16", "");
    globalObject.insert("COM_CMD_17", "");
    globalObject.insert("COM_CMD_18", "");
    globalObject.insert("COM_CMD_19", "");
    globalObject.insert("COM_CMD_20", "");
    globalObject.insert("COM_CMD_21", "");

    m_jsonObject.insert("GLOBAL_CONFIG", QJsonValue(globalObject));

    WriteJsonFile();
}

QJsonObject JsonOperate::JsonObjectGet(void)
{
    return m_jsonObject;
}

void JsonOperate::WriteJson(QString key, QJsonValue value)
{
    // QJsonObject
    JsonAddItem(m_jsonObject, key, value);
    WriteJsonFile();
}

