#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <QObject>
#include "QGNSS_BROM_global.h"
class QGNSS_BROM_EXPORT KeyValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)

public:
    explicit KeyValue(QObject *parent = nullptr);

    int value() const;

    const QString &key() const;

Q_SIGNALS:
    void valueChanged();

    void keyChanged();

public Q_SLOTS:
    void setValue(int newValue);

    void setKey(const QString &newKey);

private:
    int value_;
    QString key_;
};

#endif // KEYVALUE_H
