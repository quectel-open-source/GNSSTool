/****************************************************************************
* Copyright(C) 2022-2022 *** All Rights Reserved
* Name:         setting_cfg.h
* Description:
* History:
* Version Date                           Author          Description
*         2022-05-09      victor.gong
* ***************************************************************************/
#ifndef SETTING_CFG_H
#define SETTING_CFG_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QApplication>

#include "iodvice.h"
#include "brom_base.h"
namespace CFG {

static QJsonDocument read_json()
{
    QString  DEFAULT_PTH=QApplication::applicationDirPath ()+"/config";
    QDir dir(DEFAULT_PTH);
    if(!dir.exists ())
    {
        dir.mkdir (DEFAULT_PTH);
    }
    QFile file(DEFAULT_PTH+"/config");
    if(!file.exists ())
    {
        QJsonDocument emptyDoc;
        file.write (emptyDoc.toJson ());
        file.flush ();
        file.close ();
    }
    file.open (QIODevice::ReadWrite);
    QByteArray readbits=file.readAll ();
    file.close ();
    QJsonParseError parseError;
    QJsonDocument doc=  QJsonDocument::fromJson (readbits,&parseError);
    if(parseError.error!=QJsonParseError::NoError)
    {
        qDebug()<<parseError.errorString ();
    }
    return doc;
}

static void save_json(const QJsonDocument &jsondoc)
{
    QString  DEFAULT_PTH=QApplication::applicationDirPath ()+"/config";
    QDir dir(DEFAULT_PTH);
    if(!dir.exists ())
    {
        dir.mkpath (DEFAULT_PTH);
    }
    QFile file(DEFAULT_PTH+"/config");
    if(!file.exists ())
    {
        file.open (QIODevice::WriteOnly);
        file.write (jsondoc.toJson ());
        file.flush ();
        file.close ();
    }
    file.open (QIODevice::WriteOnly);
    file.write (jsondoc.toJson ());
    file.flush ();
    file.close ();
}

template<typename T>
class  Setting_cfg{

public:
    static bool writeValue(BROM_BASE::QGNSS_Model model,T t,const QVariant & value){
        QJsonDocument jsondoc=read_json();
        QString modelkey=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>(model);
        QString   key=IODvice::Enum_Value2Key<T>(t);
        QJsonObject keyobj=jsondoc.object ();
        QJsonObject keyvlue=jsondoc.object ().value (modelkey).toObject ();
        keyvlue.insert (key,value.toJsonValue ());
        keyobj.insert (modelkey,keyvlue);
        jsondoc.setObject (keyobj);
        save_json(jsondoc);
        return true;
    }

    static QVariant  readValue(BROM_BASE::QGNSS_Model model,T t,const QVariant &defaultValue){
        if(!std::is_enum<T>())
        {
            return defaultValue;
        }
        QJsonDocument jsondoc=read_json();
        QString modelkey=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>(model);
        QString  key=IODvice::Enum_Value2Key<T>(t);
        QVariant var=jsondoc.object ().value (modelkey).toObject ().value (key).toVariant ();
        if(var.isNull ())
        {
            return defaultValue;
        }
        else
        {
            return var;
        }
    }

};

template<>
class  Setting_cfg<QString>{

public:
 static bool writeValue(BROM_BASE::QGNSS_Model model,QString key,const QVariant & value){
        QJsonDocument jsondoc=read_json();
        QString modelkey=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>(model);
        QJsonObject keyobj=jsondoc.object ();
        QJsonObject keyvlue=jsondoc.object ().value (modelkey).toObject ();
        keyvlue.insert (key,value.toJsonValue ());
        keyobj.insert (modelkey,keyvlue);
        jsondoc.setObject (keyobj);
        save_json(jsondoc);
        return true;
    }

 static   QVariant  readValue(BROM_BASE::QGNSS_Model model,QString key,const QVariant &defaultValue){
     QJsonDocument jsondoc=read_json();
     QString modelkey=IODvice::Enum_Value2Key<BROM_BASE::QGNSS_Model>(model);
     QVariant var=jsondoc.object ().value (modelkey).toObject ().value (key).toVariant ();
     if(var.isNull ())
     {
         return defaultValue;
     }
     else
     {
         return var;
     }
    }

};


}
#endif//SETTING_CFG_H
