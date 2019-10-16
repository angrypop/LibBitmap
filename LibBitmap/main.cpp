/* Copyright Yonghuan "angrypop" Hu 2019 */
/* Contact wanngrypop@gmail.com */
/* NO UNAUTHORIZED REDISTRIBUTION ALLOWED */
/* Environment:
    Windows SDK     10.0.17763.0
    Visual Studio   2017(v141)
    Visual C++      2017
*/
#include <Windows.h>
#include <iostream>
#include "bmp.h"
using namespace std;

int main() {
	BMP orig;
	orig.Read("../Image/Input1.bmp");
	orig.MakeBinary();
	orig.Save("../Image/Binary.bmp");
	BMP erosion;
	erosion.Read("../Image/Input1.bmp");
	erosion.Erode();
	erosion.Save("../Image/Erosion.bmp");
	BMP dilation;
	dilation.Read("../Image/Input1.bmp");
	dilation.Dilate();
	dilation.Save("../Image/Dilation.bmp");

	system("pause");
    return 0;
}