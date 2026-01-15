#include "stdafx.h"
#include "base64.h"

namespace Base64
{

	static const std::string alphabet64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	static const char pad = '=';
	static const char np  = (char)std::string::npos;
	static char table64vals[] =
	{
		62, np, np, np, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, np, np, np, np, np,
		np, np,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23, 24, 25, np, np, np, np, np, np, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};

	inline char table64( unsigned char c )
	{
		return ( c < 43 || c > 122 ) ? np : table64vals[c-43];
	}

	const std::string encode64( const std::string& input )
	{
		std::string encoded;
		char c;
		const std::string::size_type length = input.length();

		encoded.reserve( length * 2 );

		for( std::string::size_type i = 0; i < length; ++i )
		{
			c = static_cast<char>( ( input[i] >> 2 ) & 0x3f );
			encoded += alphabet64[c];

			c = static_cast<char>( ( input[i] << 4 ) & 0x3f );
			if( ++i < length )
				c = static_cast<char>( c | static_cast<char>( ( input[i] >> 4 ) & 0x0f ) );
			encoded += alphabet64[c];

			if( i < length )
			{
				c = static_cast<char>( ( input[i] << 2 ) & 0x3c );
				if( ++i < length )
					c = static_cast<char>( c | static_cast<char>( ( input[i] >> 6 ) & 0x03 ) );
				encoded += alphabet64[c];
			}
			else
			{
				++i;
				encoded += pad;
			}

			if( i < length )
			{
				c = static_cast<char>( input[i] & 0x3f );
				encoded += alphabet64[c];
			}
			else
			{
				encoded += pad;
			}
		}

		return encoded;
	}

	const std::string decode64( const std::string& input )
	{
		char c, d;
		const std::string::size_type length = input.length();
		std::string decoded;

		decoded.reserve( length );

		for( std::string::size_type i = 0; i < length; ++i )
		{
			c = table64(input[i]);
			++i;
			d = table64(input[i]);
			c = static_cast<char>( ( c << 2 ) | ( ( d >> 4 ) & 0x3 ) );
			decoded += c;
			if( ++i < length )
			{
				c = input[i];
				if( pad == c )
					break;

				c = table64(input[i]);
				d = static_cast<char>( ( ( d << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0xf ) );
				decoded += d;
			}

			if( ++i < length )
			{
				d = input[i];
				if( pad == d )
					break;

				d = table64(input[i]);
				c = static_cast<char>( ( ( c << 6 ) & 0xc0 ) | d );
				decoded += c;
			}
		}

		return decoded;
	}
}

string ZBase64::Encode(const unsigned char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	//返回值
	string strEncode;
	unsigned char Tmp[4]={0};
	int LineLength=0;
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
		//        if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}

	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}

string ZBase64::Decode(const char* Data,int DataByte,int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};

	//返回值
	string strDecode;
	int nValue;
	int i= 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data!='\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode+=(nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode+=(nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode+=nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}
