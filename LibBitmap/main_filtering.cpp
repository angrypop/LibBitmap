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
	
	std::string cmd[10] = { "",
		"mean",
		"median",
		"laplacian ",
		"laplacian -fuse",
		"laplacian -binary",
		"laplacian -extend -fuse" };
	for (int i = 1; i <= 6; i++) {
		BMP filtering;
		filtering.Read(inpath[1] + ".bmp");
		filtering.Filter(cmd[i]);
		filtering.Save(inpath[1] + "_" + cmd[i] + "_.bmp");
	}
	for (int i = 1; i <= 2; i++) {
		BMP filtering;
		filtering.Read(inpath[1] + ".bmp");
		filtering.Filter(cmd[i], 11);
		filtering.Save(inpath[1] + "_" + cmd[i] + "_size5_.bmp");
	}
	system("pause");
	return 0;
}