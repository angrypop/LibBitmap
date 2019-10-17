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
	std::string InputPath = "../Image/Input2.bmp";
	BMP orig;
	orig.Read(InputPath);
	orig.MakeBinary();
	orig.Save("../Image/Binary.bmp");
	BMP erosion;
	erosion.Read(InputPath);
	erosion.Erode();
	erosion.Save("../Image/Erosion.bmp");
	BMP dilation;
	dilation.Read(InputPath);
	dilation.Dilate();
	dilation.Save("../Image/Dilation.bmp");
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