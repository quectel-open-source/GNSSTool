#include "qaesencryption.h"


const quint8 QAESEncryption::sbox[] =   {
     //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
     0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
     0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
     0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
     0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
     0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
     0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
     0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
     0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
     0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
     0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
     0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
     0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
     0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
     0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
     0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
     0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

const quint8 QAESEncryption::rsbox[256] =
    { 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
      0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
      0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
      0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
      0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
      0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
      0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
      0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
      0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
      0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
      0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
      0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
      0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
      0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
      0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
      0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

const quint8 QAESEncryption::Rcon[256] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab/*, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
        0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d */};


/*
 * Static Functions
 * */
QByteArray QAESEncryption::Crypt(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &rawText,
                                 const QByteArray &key, const QByteArray &iv, QAESEncryption::Padding padding)
{
    return QAESEncryption(level, mode, padding).encode(rawText, key, iv);
}

QByteArray QAESEncryption::Decrypt(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &rawText,
                                   const QByteArray &key, const QByteArray &iv, QAESEncryption::Padding padding)
{
     return QAESEncryption(level, mode, padding).decode(rawText, key, iv);
}

QByteArray QAESEncryption::ExpandKey(QAESEncryption::Aes level, QAESEncryption::Mode mode, const QByteArray &key)
{
     return QAESEncryption(level, mode).expandKey(key);
}

QByteArray QAESEncryption::RemovePadding(const QByteArray &rawText, QAESEncryption::Padding padding)
{
    QByteArray ret(rawText);
    switch (padding)
    {
    case Padding::ZERO:
        //Works only if the last byte of the decoded array is not zero
        while (ret.at(ret.length()-1) == 0x00)
            ret.remove(ret.length()-1, 1);
        break;
    case Padding::PKCS7:
        ret.remove(ret.length() - ret.at(ret.length()-1), ret.at(ret.length()-1));
        break;
    case Padding::ISO:
        ret.truncate(ret.lastIndexOf(0x80));
        break;
    default:
        //do nothing
        break;
    }
    return ret;
}
/*
 * End Static function declarations
 * */

/*
 * Inline Functions
 * */

inline quint8 xTime(quint8 x){
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

inline quint8 multiply(quint8 x, quint8 y){
  return (((y & 1) * x) ^ ((y>>1 & 1) * xTime(x)) ^ ((y>>2 & 1) * xTime(xTime(x))) ^ ((y>>3 & 1)
            * xTime(xTime(xTime(x)))) ^ ((y>>4 & 1) * xTime(xTime(xTime(xTime(x))))));
}

/*
 * End Inline functions
 * */


QAESEncryption::QAESEncryption(Aes level, Mode mode,
                               Padding padding)
    : m_nb(4), m_blocklen(16), m_level(level), m_mode(mode), m_padding(padding)
{
    m_state = NULL;

    switch (level)
    {
    case AES_128: {
        AES128 aes;
        m_nk = aes.nk;
        m_keyLen = aes.keylen;
        m_nr = aes.nr;
        m_expandedKey = aes.expandedKey;
        }
        break;
    case AES_192: {
        AES192 aes;
        m_nk = aes.nk;
        m_keyLen = aes.keylen;
        m_nr = aes.nr;
        m_expandedKey = aes.expandedKey;
        }
        break;
    case AES_256: {
        AES256 aes;
        m_nk = aes.nk;
        m_keyLen = aes.keylen;
        m_nr = aes.nr;
        m_expandedKey = aes.expandedKey;
        }
        break;
    default: {
        AES128 aes;
        m_nk = aes.nk;
        m_keyLen = aes.keylen;
        m_nr = aes.nr;
        m_expandedKey = aes.expandedKey;
        }
        break;
    }

}
QByteArray QAESEncryption::getPadding(int currSize, int alignment)
{
    int size = (alignment - currSize % alignment) % alignment;
    if (size == 0) return QByteArray();
    switch(m_padding)
    {
    case Padding::ZERO:
        return QByteArray(size, 0x00);
        break;
    case Padding::PKCS7:
        return QByteArray(size,size);
        break;
    case Padding::ISO:
        return QByteArray (size-1, 0x00).prepend(0x80);
        break;
    default:
        return QByteArray(size, 0x00);
        break;
    }
    return QByteArray(size, 0x00);
}

QByteArray QAESEncryption::expandKey(const QByteArray &key)
{
  int i, k;
  quint8 tempa[4]; // Used for the column/row operations
  QByteArray roundKey(key);

  // The first round key is the key itself.
  // ...

  // All other round keys are found from the previous round keys.
  //i == Nk
  for(i = m_nk; i < m_nb * (m_nr + 1); i++)
  {
    tempa[0] = (quint8) roundKey.at((i-1) * 4 + 0);
    tempa[1] = (quint8) roundKey.at((i-1) * 4 + 1);
    tempa[2] = (quint8) roundKey.at((i-1) * 4 + 2);
    tempa[3] = (quint8) roundKey.at((i-1) * 4 + 3);

    if (i % m_nk == 0)
    {
        // This function shifts the 4 bytes in a word to the left once.
        // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

        // Function RotWord()
        k = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = k;

        // Function Subword()
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);

        tempa[0] =  tempa[0] ^ Rcon[i/m_nk];
    }
    if (m_level == AES_256 && i % m_nk == 4)
    {
        // Function Subword()
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
    }
    roundKey.insert(i * 4 + 0, (quint8) roundKey.at((i - m_nk) * 4 + 0) ^ tempa[0]);
    roundKey.insert(i * 4 + 1, (quint8) roundKey.at((i - m_nk) * 4 + 1) ^ tempa[1]);
    roundKey.insert(i * 4 + 2, (quint8) roundKey.at((i - m_nk) * 4 + 2) ^ tempa[2]);
    roundKey.insert(i * 4 + 3, (quint8) roundKey.at((i - m_nk) * 4 + 3) ^ tempa[3]);
  }
  return roundKey;
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void QAESEncryption::addRoundKey(const quint8 round, const QByteArray expKey)
{
  QByteArray::iterator it = m_state->begin();
  for(int i=0; i < 16; ++i)
      it[i] = (quint8) it[i] ^ (quint8) expKey.at(round * m_nb * 4 + (i/4) * m_nb + (i%4));
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void QAESEncryption::subBytes()
{
  QByteArray::iterator it = m_state->begin();
  for(int i = 0; i < 16; i++)
    it[i] = getSBoxValue((quint8) it[i]);
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void QAESEncryption::shiftRows()
{
    QByteArray::iterator it = m_state->begin();
    quint8 temp;
    //Keep in mind that QByteArray is column-driven!!

     //Shift 1 to left
    temp   = (quint8)it[1];
    it[1]  = (quint8)it[5];
    it[5]  = (quint8)it[9];
    it[9]  = (quint8)it[13];
    it[13] = (quint8)temp;

    //Shift 2 to left
    temp   = (quint8)it[2];
    it[2]  = (quint8)it[10];
    it[10] = (quint8)temp;
    temp   = (quint8)it[6];
    it[6]  = (quint8)it[14];
    it[14] = (quint8)temp;

    //Shift 3 to left
    temp   = (quint8)it[3];
    it[3]  = (quint8)it[15];
    it[15] = (quint8)it[11];
    it[11] = (quint8)it[7];
    it[7]  = (quint8)temp;
}

// MixColumns function mixes the columns of the state matrix
//optimized!!
void QAESEncryption::mixColumns()
{
  QByteArray::iterator it = m_state->begin();
  quint8 tmp, tm, t;

  for(int i = 0; i < 16; i += 4){
    t       = (quint8)it[i];
    tmp     =  (quint8)it[i] ^ (quint8)it[i+1] ^ (quint8)it[i+2] ^ (quint8)it[i+3] ;

    tm      = xTime( (quint8)it[i] ^ (quint8)it[i+1] );
    it[i]   = (quint8)it[i] ^ (quint8)tm ^ (quint8)tmp;

    tm      = xTime( (quint8)it[i+1] ^ (quint8)it[i+2]);
    it[i+1] = (quint8)it[i+1] ^ (quint8)tm ^ (quint8)tmp;

    tm      = xTime( (quint8)it[i+2] ^ (quint8)it[i+3]);
    it[i+2] =(quint8)it[i+2] ^ (quint8)tm ^ (quint8)tmp;

    tm      = xTime((quint8)it[i+3] ^ (quint8)t);
    it[i+3] =(quint8)it[i+3] ^ (quint8)tm ^ (quint8)tmp;
  }
}

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
void QAESEncryption::invMixColumns()
{
  QByteArray::iterator it = m_state->begin();
  quint8 a,b,c,d;
  for(int i = 0; i < 16; i+=4){
    a = (quint8) it[i];
    b = (quint8) it[i+1];
    c = (quint8) it[i+2];
    d = (quint8) it[i+3];

    it[i]   = (quint8) (multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09));
    it[i+1] = (quint8) (multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d));
    it[i+2] = (quint8) (multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b));
    it[i+3] = (quint8) (multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e));
  }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void QAESEncryption::invSubBytes()
{
    QByteArray::iterator it = m_state->begin();
    for(int i = 0; i < 16; ++i)
        it[i] = getSBoxInvert((quint8) it[i]);
}

void QAESEncryption::invShiftRows()
{
    QByteArray::iterator it = m_state->begin();
    quint8 temp;

    //Keep in mind that QByteArray is column-driven!!

    //Shift 1 to right
    temp   = (quint8)it[13];
    it[13] = (quint8)it[9];
    it[9]  = (quint8)it[5];
    it[5]  = (quint8)it[1];
    it[1]  = (quint8)temp;

    //Shift 2
    temp   = (quint8)it[10];
    it[10] = (quint8)it[2];
    it[2]  = (quint8)temp;
    temp   = (quint8)it[14];
    it[14] = (quint8)it[6];
    it[6]  = (quint8)temp;

    //Shift 3
    temp   = (quint8)it[15];
    it[15] = (quint8)it[3];
    it[3]  = (quint8)it[7];
    it[7]  = (quint8)it[11];
    it[11] = (quint8)temp;
}

QByteArray QAESEncryption::byteXor(const QByteArray &a, const QByteArray &b)
{
  QByteArray::const_iterator it_a = a.begin();
  QByteArray::const_iterator it_b = b.begin();
  QByteArray ret;

  //for(int i = 0; i < m_blocklen; i++)
  for(int i = 0; i < std::min(a.size(), b.size()); i++)
      ret.insert(i,it_a[i] ^ it_b[i]);

  return ret;
}

// Cipher is the main function that encrypts the PlainText.
QByteArray QAESEncryption::cipher(const QByteArray &expKey, const QByteArray &in)
{

  //m_state is the input buffer...
  QByteArray output(in);
  m_state = &output;

  // Add the First round key to the state before starting the rounds.
  addRoundKey(0, expKey);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for(quint8 round = 1; round < m_nr; ++round){
    subBytes();
    shiftRows();
    mixColumns();
    addRoundKey(round, expKey);
  }

  // The last round is given below.
  // The MixColumns function is not here in the last round.
  subBytes();
  shiftRows();
  addRoundKey(m_nr, expKey);

  return output;
}

QByteArray QAESEncryption::invCipher(const QByteArray &expKey, const QByteArray &in)
{
    //m_state is the input buffer.... handle it!
    QByteArray output(in);
    m_state = &output;

    // Add the First round key to the state before starting the rounds.
    addRoundKey(m_nr, expKey);

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for(quint8 round=m_nr-1; round>0 ; round--){
        invShiftRows();
        invSubBytes();
        addRoundKey(round, expKey);
        invMixColumns();
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    invShiftRows();
    invSubBytes();
    addRoundKey(0, expKey);

    return output;
}

QByteArray QAESEncryption::encode(const QByteArray &rawText, const QByteArray &key, const QByteArray &iv)
{
    if (m_mode >= CBC && (iv.isNull() || iv.size() != m_blocklen))
       return QByteArray();

    QByteArray ret;
    QByteArray expandedKey = expandKey(key);
    QByteArray alignedText(rawText);

    //Fill array with padding
    alignedText.append(getPadding(rawText.size(), m_blocklen));

    switch(m_mode)
    {
    case ECB:
        for(int i=0; i < alignedText.size(); i+= m_blocklen)
            ret.append(cipher(expandedKey, alignedText.mid(i, m_blocklen)));
        break;
    case CBC: {
            QByteArray ivTemp(iv);
            for(int i=0; i < alignedText.size(); i+= m_blocklen) {
                alignedText.replace(i, m_blocklen, byteXor(alignedText.mid(i, m_blocklen),ivTemp));
                ret.append(cipher(expandedKey, alignedText.mid(i, m_blocklen)));
                ivTemp = ret.mid(i, m_blocklen);
            }
        }
        break;
    case CFB: {
            ret.append(byteXor(alignedText.left(m_blocklen), cipher(expandedKey, iv)));
            for(int i=0; i < alignedText.size(); i+= m_blocklen) {
                if (i+m_blocklen < alignedText.size())
                    ret.append(byteXor(alignedText.mid(i+m_blocklen, m_blocklen),
                                       cipher(expandedKey, ret.mid(i, m_blocklen))));
            }
        }
        break;
    case OFB: {
            QByteArray ofbTemp;
            ofbTemp.append(cipher(expandedKey, iv));
            for (int i=m_blocklen; i < alignedText.size(); i += m_blocklen){
                ofbTemp.append(cipher(expandedKey, ofbTemp.right(m_blocklen)));
            }
            ret.append(byteXor(alignedText, ofbTemp));
        }
        break;
    default: break;
    }
    return ret;
}

QByteArray QAESEncryption::decode(const QByteArray &rawText, const QByteArray &key, const QByteArray &iv)
{
    if (m_mode >= CBC && (iv.isNull() || iv.size() != m_blocklen))
       return QByteArray();

    QByteArray ret;
    QByteArray expandedKey = expandKey(key);

    switch(m_mode)
    {
    case ECB:
        for(int i=0; i < rawText.size(); i+= m_blocklen)
            ret.append(invCipher(expandedKey, rawText.mid(i, m_blocklen)));
        break;
    case CBC: {
            QByteArray ivTemp(iv);
            for(int i=0; i < rawText.size(); i+= m_blocklen){
                ret.append(invCipher(expandedKey, rawText.mid(i, m_blocklen)));
                ret.replace(i, m_blocklen, byteXor(ret.mid(i, m_blocklen),ivTemp));
                ivTemp = rawText.mid(i, m_blocklen);
            }
        }
        break;
    case CFB: {
            ret.append(byteXor(rawText.mid(0, m_blocklen), cipher(expandedKey, iv)));
            for(int i=0; i < rawText.size(); i+= m_blocklen){
                if (i+m_blocklen < rawText.size()) {
                    ret.append(byteXor(rawText.mid(i+m_blocklen, m_blocklen),
                                       cipher(expandedKey, rawText.mid(i, m_blocklen))));
                }
            }
        }
        break;
    case OFB: {
        QByteArray ofbTemp;
        ofbTemp.append(cipher(expandedKey, iv));
        for (int i=m_blocklen; i < rawText.size(); i += m_blocklen){
            ofbTemp.append(cipher(expandedKey, ofbTemp.right(m_blocklen)));
        }
        ret.append(byteXor(rawText, ofbTemp));
    }
        break;
    default:
        //do nothing
        break;
    }
    return ret;
}

QByteArray QAESEncryption::removePadding(const QByteArray &rawText)
{
    QByteArray ret(rawText);
    switch (m_padding)
    {
    case Padding::ZERO:
        //Works only if the last byte of the decoded array is not zero
        while (ret.at(ret.length()-1) == 0x00)
            ret.remove(ret.length()-1, 1);
        break;
    case Padding::PKCS7:
        ret.remove(ret.length() - ret.at(ret.length()-1), ret.at(ret.length()-1));
        break;
    case Padding::ISO:
        ret.truncate(ret.lastIndexOf(0x80));
        break;
    default:
        //do nothing
        break;
    }
    return ret;
}
