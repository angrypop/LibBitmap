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
	std::string inpath[10] = { "RESERVED", "../Image/Input1", "../Image/Input2", "../Image/Input3", "../Image/Input4", "../Image/Input5", "../Image/Input6" };
	
	BMP filtering[7];
	std::string cmd[7] = { "",
		"mean",
		"median",
		"laplacian -binary",
		"laplacian -fuse",
		"laplacian -extend -binary",
		"laplacian -extend -fuse" };
	for (int i = 1; i <= 6; i++) {
		filtering[i].Read(inpath[1] + ".bmp");
		filtering[i].Filter(cmd[i]);
		filtering[i].Save(inpath[1] + "_" + cmd[i] + "_.bmp");
	}

	system("pause");
	return 0;
}