#ifndef QAESENCRYPTION_H
#define QAESENCRYPTION_H

#include <QObject>
#include <QByteArray>

class QAESEncryption : public QObject
{
    Q_OBJECT
public:
    enum Aes {
        AES_128,
        AES_192,
        AES_256
    };

    enum Mode {
        ECB,
        CBC,
        CFB,
        OFB
    };

    enum Padding {
      ZERO,
      PKCS7,
      ISO
    };

    static QByteArray Crypt(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &rawText, const QByteArray &key,
                            const QByteArray &iv = NULL, QAESEncryption::Padding padding = QAESEncryption::ISO);
    static QByteArray Decrypt(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &rawText, const QByteArray &key,
                              const QByteArray &iv = NULL, QAESEncryption::Padding padding = QAESEncryption::ISO);
    static QByteArray ExpandKey(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &key);
    static QByteArray RemovePadding(const QByteArray &rawText, QAESEncryption::Padding padding);

    QAESEncryption(QAESEncryption::Aes level, QAESEncryption::Mode mode,
                   QAESEncryption::Padding padding = QAESEncryption::ISO);

    QByteArray encode(const QByteArray &rawText, const QByteArray &key, const QByteArray &iv = NULL);
    QByteArray decode(const QByteArray &rawText, const QByteArray &key, const QByteArray &iv = NULL);
    QByteArray removePadding(const QByteArray &rawText);
    QByteArray expandKey(const QByteArray &key);

signals:

public slots:

private:
    int m_nb;
    int m_blocklen;
    int m_level;
    int m_mode;
    int m_nk;
    int m_keyLen;
    int m_nr;
    int m_expandedKey;
    int m_padding;
    QByteArray* m_state;

    struct AES256{
        int nk = 8;
        int keylen = 32;
        int nr = 14;
        int expandedKey = 240;
    };

    struct AES192{
        int nk = 6;
        int keylen = 24;
        int nr = 12;
        int expandedKey = 209;
    };

    struct AES128{
        int nk = 4;
        int keylen = 16;
        int nr = 10;
        int expandedKey = 176;
    };

    quint8 getSBoxValue(quint8 num){return sbox[num];}
    quint8 getSBoxInvert(quint8 num){return rsbox[num];}

    void addRoundKey(const quint8 round, const QByteArray expKey);
    void subBytes();
    void shiftRows();
    void mixColumns();
    void invMixColumns();
    void invSubBytes();
    void invShiftRows();
    QByteArray getPadding(int currSize, int alignment);
    QByteArray cipher(const QByteArray &expKey, const QByteArray &plainText);
    QByteArray invCipher(const QByteArray &expKey, const QByteArray &plainText);
    QByteArray byteXor(const QByteArray &in, const QByteArray &iv);

    static const quint8 sbox[256];

    static const quint8 rsbox[256];

    // The round constant word array, Rcon[i], contains the values given by
    // x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
    // Only the first 14 elements are needed
    static const quint8 Rcon[256];
};

#endif // QAESENCRYPTION_H
