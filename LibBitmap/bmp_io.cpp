#define _CRT_SECURE_NO_WARNINGS
#include "bmp.h"

bool BMP::Read(std::string FileName) {
	Type = Colored;
	FILE* fp = fopen(FileName.c_str(), "rb");
	if (fp == NULL) {
		/* Error opening target image */
		std::cout << "ERROR opening file " << FileName.c_str() << std::endl;
		system("pause");
		return 0;
	}
	/* Read file header */
	fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	/* Read info header */
	fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	ByteLine = (InfoHeader.biWidth * InfoHeader.biBitCount / 8 + 3) / 4 * 4;
	PixelData = new BYTE[ByteLine * InfoHeader.biHeight];
	/* Assuming no palette used */
	/* Read pixel data */
	fread(PixelData, ByteLine * InfoHeader.biHeight, 1, fp);
	fclose(fp);
	return 1;
}

bool BMP::Save(std::string FileName) {
	FILE* fp = fopen(FileName.c_str(), "wb");
	if (fp == NULL) {
		/* Error saving target image */
		std::cout << "ERROR saving file " << FileName.c_str() << std::endl;
		system("pause");
		return 0;
	}
	/* Write file header */
	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	/* Write info header */
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	/* Assuming no palette used */
	/* Read pixel data */
	fwrite(PixelData, ByteLine * InfoHeader.biHeight, 1, fp);
	fclose(fp);
	return 1;
}

bool BMP::PrintCOE(std::string FileName) {
	/* .coe files are used in Xilinx ISE to initialize ROM */
	/* The output image is vertically flipped */
	/* color: {B(4bits), G(4bits), R(4bits) */
	FILE* fp = fopen(FileName.c_str(), "w");
	if (fp == NULL) {
		/* Error saving target image */
		std::cout << "ERROR saving file " << FileName.c_str() << std::endl;
		system("pause");
		return 0;
	}
	int cnt = 0;
	std::cout << InfoHeader.biWidth << "*" << InfoHeader.biHeight << std::endl;
	fprintf(fp, "memory_initialization_radix=16;\n");
	fprintf(fp, "memory_initialization_vector=\n");
	for (int y = InfoHeader.biHeight - 1; y >= 0; y--) {
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			cnt++;
			Color c = GetColor(x, y);
			fprintf(fp, "%x%x%x", c.B / 16, c.G / 16, c.R / 16);
			if (y == 0 && x == InfoHeader.biWidth - 1) fprintf(fp, ";");
			else fprintf(fp, ",");
		}
	}
	std::cout << "Items outputed in .coe file: " << cnt << std::endl;
	fclose(fp);
	return 1;
}

void BMP::HistPrintCSV(std::string FileName) {
	FILE* fp = fopen(FileName.c_str(), "w");/* Expected to be a .csv file */
	int n[256], N = InfoHeader.biHeight*InfoHeader.biWidth;
	memset(n, 0, sizeof(n));
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			n[(int)GetColor(x, y).GetLuminance()]++;
		}
	fprintf(fp, "luminance,p\n");
	for (int k = 0; k <= 255; k++) {
		fprintf(fp, "%d,%lf\n", k, (double)n[k] / N);
	}
	fclose(fp);
}