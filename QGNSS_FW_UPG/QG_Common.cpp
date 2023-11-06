#include "QG_Common.h"
 bool isLittleEndian()
{
    union U
    {
        int  i;
        char c;
    };
    U u;
    u.i = 1;
    return u.c == 1;
}

 char QG_XOR_CheckSum(char* data, unsigned int size)
{
    char data_xor = 0;
    if (!data || !size) { return data_xor; }
    data_xor = data[0];

    //complement of byte 8
    if (size == 1)
    {
        data_xor ^= 0xFF;
        return data_xor;
    }
    for (int i = 1; i < size; i++)
    {
        data_xor ^= data[i];
    }
    return data_xor;
}

 bool str2hex(const std::string& str, std::string& hexstr)
{
    if (str.empty())
    {
        return false;
    }
    for (auto chr = str.cbegin(); chr != str.cend(); ++chr)
    {
        char tempc[100]{ 0 };
        sprintf(tempc, "%02X ", *chr);
        hexstr.append(tempc);
    }
    return true;
}

//true :not over time false :over time
 bool QG_overtime(std::chrono::steady_clock::time_point start,/*milliseconds*/ unsigned int totaltime)
{
    return  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < totaltime;
}

 bool QG_Str_Compare(std::string_view str1, std::string_view str2, bool casesenstive)
{
    std::string s1, s2;
    std::transform(str1.cbegin(), str1.cend(), s1.begin(), [](char c) {return std::tolower(c); });
    std::transform(str2.begin(), str2.end(), s2.begin(), [](char c) {return std::tolower(c); });
    return s1 == s2;
}

 std::size_t QG_file_size(const char* filename, const char* mode)
 {
     std::size_t filesize = 0;
     FILE* fp = fopen(filename, /*"rb"*/mode);
     if (fp == INVALID_HANDLE_VALUE)
     {
         return filesize;
     }
     if (!fp) { filesize = 0; return filesize; }
     fseek(fp, 0L, SEEK_END);
     filesize = ftell(fp);
     fclose(fp);
     return filesize;
 }

 std::size_t QG_Read_File(const char* filename
     , const char* mode
     , std::size_t size
     , std::string& data)
 {
     std::size_t readsize = 0;
     FILE* fp = fopen(filename, /*"rb"*/mode);
     if (fp == INVALID_HANDLE_VALUE )
     {
         return readsize;
     }
     data.resize(size);
     readsize= fread(data.data(), 1, size, fp);
     data= data.substr(0, readsize);
     return readsize;
 }

 QG_EXTERN QG_PORTEXTERN int QG_get_progress(uint32_t totalsize, uint32_t now, uint32_t progress_max)
 {
     return  ((double)now / totalsize)*progress_max;
 }

 std::string wstringToString(const std::wstring& wstr)
 {
   // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
   if (wstr.empty())
   {
     return std::string();
   }

   int size = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
   std::string ret = std::string(size, 0);
   WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &ret[0], size, NULL, NULL); // CP_UTF8
   return ret;
 }

 std::wstring StringToWString(const std::string& str)
 {
   int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
   wchar_t* wide = new wchar_t[len + 1];
   memset(wide, '\0', sizeof(wchar_t) * (len + 1));
   MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, len);
   std::wstring w_str(wide);
   delete[] wide;
   return w_str;
 }
