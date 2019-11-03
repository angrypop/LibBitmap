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
	std::string inpath[4] = { "RESERVED", "../Image/Input1", "../Image/Input2", "../Image/Input3" };

	/* Logarithmic Enhancement */
	BMP enhance[4];
	for (int i = 1; i <= 3; i++) {
		enhance[i].Read(inpath[i] + ".bmp");
		enhance[i].Enhance();
		enhance[i].Save(inpath[i] + "_enhanced.bmp");
	}

	/* Histogram Equalization */
	BMP hist[4];
	for (int i = 1; i <= 3; i++) {
		hist[i].Read(inpath[i] + ".bmp");
		hist[i].MakeGrayscale();
		hist[i].Save(inpath[i] + "_gray.bmp");
		hist[i].HistEqualize();
		hist[i].Save(inpath[i] + "_gray_equalized.bmp");
	}
	for (int i = 1; i <= 3; i++) {
		hist[i].Read(inpath[i] + ".bmp");
		hist[i].HistPrintCSV(inpath[i] + ".csv");
		hist[i].HistEqualize();
		hist[i].HistPrintCSV(inpath[i] + "_equalized.csv");
		hist[i].Save(inpath[i] + "_equalized.bmp");
	}
	system("pause");
    return 0;
}