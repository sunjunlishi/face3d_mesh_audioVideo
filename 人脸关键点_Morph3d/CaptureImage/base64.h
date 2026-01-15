#ifndef BASE64_H__
#define BASE64_H__

#include <string>
using namespace std;

namespace Base64
{

	/**
	* Base64-encodes the input according to RFC 3548.
	* @param input The data to encode.
	* @return The encoded string.
	*/
	const std::string encode64(const std::string& input);

	/**
	* Base64-decodes the input according to RFC 3548.
	* @param input The encoded data.
	* @return The decoded data.
	*/
	const std::string decode64(const std::string& input);
}

class ZBase64
{
public:
    /*编码
    DataByte
        [in]输入的数据长度,以字节为单位
    */
    string Encode(const unsigned char* Data,int DataByte);

    /*解码
    DataByte
        [in]输入的数据长度,以字节为单位
    OutByte
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算
        输出数据的长度
    */
    string Decode(const char* Data,int DataByte,int& OutByte);
};

#endif // BASE64_H__
