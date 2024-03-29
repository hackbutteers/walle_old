#include <walle/wsl/ascii.h>


namespace wsl {


const int ascii::CHARACTER_PROPERTIES[128] = 
{
	/* 00 . */ eAcpControl,
	/* 01 . */ eAcpControl,
	/* 02 . */ eAcpControl,
	/* 03 . */ eAcpControl,
	/* 04 . */ eAcpControl,
	/* 05 . */ eAcpControl,
	/* 06 . */ eAcpControl,
	/* 07 . */ eAcpControl,
	/* 08 . */ eAcpControl,
	/* 09 . */ eAcpControl | eAcpSpace,
	/* 0a . */ eAcpControl | eAcpSpace,
	/* 0b . */ eAcpControl | eAcpSpace,
	/* 0c . */ eAcpControl | eAcpSpace,
	/* 0d . */ eAcpControl | eAcpSpace,
	/* 0e . */ eAcpControl,
	/* 0f . */ eAcpControl,
	/* 10 . */ eAcpControl,
	/* 11 . */ eAcpControl,
	/* 12 . */ eAcpControl,
	/* 13 . */ eAcpControl,
	/* 14 . */ eAcpControl,
	/* 15 . */ eAcpControl,
	/* 16 . */ eAcpControl,
	/* 17 . */ eAcpControl,
	/* 18 . */ eAcpControl,
	/* 19 . */ eAcpControl,
	/* 1a . */ eAcpControl,
	/* 1b . */ eAcpControl,
	/* 1c . */ eAcpControl,
	/* 1d . */ eAcpControl,
	/* 1e . */ eAcpControl,
	/* 1f . */ eAcpControl,
	/* 20   */ eAcpSpace | eAcpPrint,
	/* 21 ! */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 22 " */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 23 # */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 24 $ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 25 % */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 26 & */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 27 ' */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 28 ( */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 29 ) */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2a * */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2b + */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2c , */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2d - */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2e . */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 2f / */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 30 0 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 31 1 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 32 2 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 33 3 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 34 4 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 35 5 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 36 6 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 37 7 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 38 8 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 39 9 */ eAcpDigit | eAcpHexDigit | eAcpGraph | eAcpPrint,
	/* 3a : */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 3b ; */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 3c < */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 3d = */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 3e > */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 3f ? */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 40 @ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 41 A */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 42 B */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 43 C */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 44 D */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 45 E */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 46 F */ eAcpHexDigit | eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 47 G */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 48 H */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 49 I */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4a J */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4b K */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4c L */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4d M */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4e N */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 4f O */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 50 P */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 51 Q */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 52 R */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 53 S */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 54 T */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 55 U */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 56 V */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 57 W */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 58 X */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 59 Y */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 5a Z */ eAcpAlpha | eAcpUpper | eAcpGraph | eAcpPrint,
	/* 5b [ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 5c \ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 5d ] */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 5e ^ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 5f _ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 60 ` */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 61 a */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 62 b */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 63 c */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 64 d */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 65 e */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 66 f */ eAcpHexDigit | eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 67 g */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 68 h */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 69 i */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6a j */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6b k */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6c l */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6d m */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6e n */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 6f o */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 70 p */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 71 q */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 72 r */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 73 s */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 74 t */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 75 u */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 76 v */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 77 w */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 78 x */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 79 y */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 7a z */ eAcpAlpha | eAcpLower | eAcpGraph | eAcpPrint,
	/* 7b { */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 7c | */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 7d } */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 7e ~ */ eAcpPunct | eAcpGraph | eAcpPrint,
	/* 7f . */ eAcpControl
};


}  //namespace walle
