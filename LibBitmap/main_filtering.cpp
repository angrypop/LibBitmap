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
#include <string>
#include "bmp.h"
using namespace std;

int main_filtering() {
	std::string inpath[10] = { "RESERVED", "../Image/Input1", "../Image/Input2", "../Image/Input3", "../Image/Input4", "../Image/Input5", "../Image/Input6" };
	
	std::string cmd[10] = { "",
		"mean",
		"median",
		"laplacian ",
		"laplacian -fuse",
		"laplacian -binary",
		"laplacian -extend -fuse",
		"bilateral" };
	int sigma[10] = {1, 10, 25, 50, 100, 1000};
	for (int size = 5; size <= 25; size += 5) {
		for (int i = 0; i < 6; i++) {
			BMP filtering;
			filtering.Read(inpath[5] + ".bmp");
			filtering.Filter(cmd[7], size, sigma[i]);
			filtering.Save(inpath[5] + "_" + cmd[7] + "_size" + to_string(size) + "_sigma" + to_string(sigma[i]) + ".bmp");
		}
	}
	system("pause");
	return 0;
}