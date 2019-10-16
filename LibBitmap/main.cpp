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
	BMP test;
	test.Read("../Image/Input2.bmp");
	test.MakeGrayscale();
	test.Save("../Image/Gray.bmp");
	test.MakeBinary();
	test.Save("../Image/Binary.bmp");
	system("pause");
    return 0;
}