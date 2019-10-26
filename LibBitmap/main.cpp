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
	std::string InputPath = "../Image/Input1.bmp";
	BMP orig;
	orig.Read(InputPath);
	orig.MakeBinary();
	orig.Save("../Image/Binary.bmp");
	BMP erosion1;
	erosion1.Read(InputPath);
	erosion1.Erode(1);
	erosion1.Save("../Image/Erosion1.bmp");
	BMP erosion2;
	erosion2.Read(InputPath);
	erosion2.Erode(2);
	erosion2.Save("../Image/Erosion2.bmp");
	BMP dilation1;
	dilation1.Read(InputPath);
	dilation1.Dilate(1);
	dilation1.Save("../Image/Dilation1.bmp");
	BMP dilation2;
	dilation2.Read(InputPath);
	dilation2.Dilate(2);
	dilation2.Save("../Image/Dilation2.bmp");
	BMP opening;
	opening.Read(InputPath);
	opening.Erode();
	opening.Dilate();
	opening.Save("../Image/Opening.bmp");
	BMP closing;
	closing.Read(InputPath);
	closing.Dilate();
	closing.Erode();
	closing.Save("../Image/Closing.bmp");
	system("pause");
    return 0;
}