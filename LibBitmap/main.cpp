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
	std::string inpath[6] = { "RESERVED", "../Image/Input1", "../Image/Input2", "../Image/Input3", "../Image/Input4", "../Image/Input5"};
	BMP translate[5];
	int dx[5] = { 0,100,100,-200,-200 }, dy[5] = { 0,200,-200,100,-100 };
	for (int i = 1; i <= 4; i++) {
		translate[i].Read(inpath[1] + ".bmp");
		translate[i].Translate(dx[i], dy[i]);
		translate[i].Save(inpath[1] + "_translate_" + char('0' + i) + ".bmp");
	}

	BMP mirror;
	mirror.Read(inpath[2] + ".bmp");
	mirror.Mirror(BMP::Horizontal);
	mirror.Save(inpath[2] + "_mir_horizontal.bmp");
	mirror.Mirror(BMP::Horizontal);
	mirror.Mirror(BMP::Vertical);
	mirror.Save(inpath[2] + "_mir_vertical.bmp");

	BMP rotate;
	rotate.Read(inpath[3] + ".bmp");
	rotate.Rotate(30);
	rotate.Save(inpath[3] + "_rotate.bmp");
	
	BMP scale[3];
	double kx[3] = {1,2,0.5}, ky[3] = {1,1.5,0.8};
	for (int i = 1; i <= 2; i++) {
		scale[i].Read(inpath[4] + ".bmp");
		scale[i].Scale(kx[i],ky[i]);
		scale[i].Save(inpath[4] + "_scale_" + char('0' + i) + ".bmp");
	}

	BMP shear[3];
	for (int i = 1; i <= 2; i++) {
		shear[i].Read(inpath[5] + ".bmp");
		shear[i].Shear(0.25, (i == 1 ? BMP::Horizontal : BMP::Vertical));
		shear[i].Save(inpath[5] + "_shear_" + char('0' + i) + ".bmp");
	}
	system("pause");
    return 0;
}