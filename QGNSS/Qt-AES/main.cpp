#include <QCoreApplication>

#include <QCryptographicHash>
#include <QDebug>
#include "qaesencryption.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    {
        QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB , QAESEncryption::ISO);
        QByteArray input("HELLO AES");
        QByteArray key = QString("your-string-key").toLocal8Bit();

        QByteArray encodedText = encryption.encode(input, key);
        QByteArray decodedText = encryption.decode(encodedText, key);

        QString decodedString = QString(QAESEncryption::RemovePadding(decodedText,QAESEncryption::ISO));
        qDebug() << decodedString;
    }

    {
        QString inputStr("The Advanced Encryption Standard (AES), also known by its original name Rijndael "
                         "is a specification for the encryption of electronic data established by the U.S. "
                         "National Institute of Standards and Technology (NIST) in 2001");
        QString key("your-string-key");
        QString iv("your-IV-vector");

        QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
        QByteArray hashIV = QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);

        //Static invocation
        QByteArray encodeText = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                                     inputStr.toLocal8Bit(), hashKey, hashIV);
        QByteArray decodeText = QAESEncryption::Decrypt(QAESEncryption::AES_256,QAESEncryption::CBC,encodeText, hashKey, hashIV);

        // Removal of Padding via Static function
        QString decodedString = QString(QAESEncryption::RemovePadding(decodeText,QAESEncryption::ISO));
        qDebug() << decodedString;
    }
    /****************************************************************************************************/
    {
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

        QString inputStr("The Advanced Encryption Standard (AES), also known by its original name Rijndael "
                         "is a specification for the encryption of electronic data established by the U.S. "
                         "National Institute of Standards and Technology (NIST) in 2001");
        QString key("your-string-key");
        QString iv("your-IV-vector");

        QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
        QByteArray hashIV = QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);

        QByteArray encodeText = encryption.encode(inputStr.toLocal8Bit(), hashKey, hashIV);
        QByteArray decodeText = encryption.decode(encodeText, hashKey, hashIV);

        QString decodedString = QString(encryption.removePadding(decodeText));

        qDebug() << decodedString;
    }

    return 0;
}
