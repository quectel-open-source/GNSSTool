#include "keyvalue.h"

KeyValue::KeyValue(QObject *parent)
    : QObject{parent}
{

}

int KeyValue::value() const
{
    return value_;
}

void KeyValue::setValue(int newValue)
{
    if (value_ == newValue)
        return;
    value_ = newValue;
    emit valueChanged();
}

const QString &KeyValue::key() const
{
    return key_;
}

void KeyValue::setKey(const QString &newKey)
{
    if (key_ == newKey)
        return;
    key_ = newKey;
    emit keyChanged();
}
