#define _CRT_SECURE_NO_WARNINGS
#include "bmp.h"

Color::Color(int inR, int inG, int inB, EncodeType inType) {
    R = inR;
    G = inG;
    B = inB;
    Type = inType;
}

Color Color::operator+(const Color color) const {
    return Color(R + color.R, G + color.G, B + color.B);
}

Color Color::operator-(const Color color) const {
    return Color(R - color.R, G - color.G, B - color.B);
}

Color Color::operator*(const double multiplier) const {
    return Color(R * multiplier, G * multiplier, B * multiplier);
}

Color Color::operator/(const double divider) const {
    return Color(R / divider, G / divider, B / divider);
}

Color Color::GetYUV() {
    if (Type == YUV) return *this;
    int Y = 0.299 * R + 0.587 * G + 0.114 * B;
    int U = -0.148 * R - 0.289 * G + 0.437 * B + 128;
    int V = 0.615 * R - 0.515 * G - 0.100 * B + 128;
    return Color(Y, U, V, YUV);
}

Color Color::GetRGB() {
    if (Type == RGB) return *this;
    /* R, G and B below indicate Y, U and V */
    /* Due to the precision, value of some channel may exceed [0,255] */
    /* Hence security check turns to be necessary */
    int _R = 1.000 * R + 0.000 * (G - 128) + 1.140 * (B - 128);
    _R = max(min(_R, 255), 0);
    int _G = 1.000 * R - 0.395 * (G - 128) - 0.581 * (B - 128);
    _G = max(min(_G, 255), 0);
    int _B = 1.000 * R + 2.032 * (G - 128) + 0.000 * (B - 128);
    _B = max(min(_B, 255), 0);
    return Color(_R, _G, _B, RGB);
}

void BMP::MakeBinary() {
	MakeGrayscale();
	int Threshold, N = InfoHeader.biHeight * InfoHeader.biWidth, bestThreshold;
	double maxvar = 0, var_between, w_foreground, w_background, u_foreground, u_background;
	for (Threshold = 1; Threshold < 255; Threshold++) {
		w_foreground = 0; w_background = 0;
		u_foreground = 0; u_background = 0;
		for (int y = 0; y < InfoHeader.biHeight; y++)
			for (int x = 0; x < InfoHeader.biWidth; x++) {
				if (GetColor(x, y).R < Threshold) {
					w_foreground++;
					u_foreground += GetColor(x, y).R;
				}
				else {
					w_background++;
					u_background += GetColor(x, y).R;
				}
			}
		w_foreground /= N;
		w_background /= N;
		u_foreground /= N*w_foreground;
		u_background /= N*w_background;
		var_between = w_foreground * w_background*(u_foreground - u_background)*(u_foreground - u_background);
		if (var_between > maxvar) {
			maxvar = var_between;
			bestThreshold = Threshold;
		}
	}
	Threshold = bestThreshold;

	Color white = Color(0, 0, 0), black = Color(255,255,255);
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			if (GetColor(x, y).R < Threshold) SetColor(x, y, white);
			else SetColor(x, y, black);
		}
}

void BMP::MakeGrayscale() {
	/* Get the maximum and minimum of Y channel (in YUV encoding system) */
	int Ymax = 0, Ymin = 255;
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			int Y = GetColor(x, y).GetYUV().R;
			Ymax = max(Y, Ymax);
			Ymin = min(Y, Ymin);
		}
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			Color c = GetColor(x, y);
			c.R = c.G = c.B = 255 * (c.GetYUV().R - Ymin) /
				(Ymax - Ymin); /* R = G = B = Scaled Y */
			SetColor(x, y, c);
		}
}

bool BMP::Read(std::string FileName) {
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

void BMP::PrintInfo() {
    std::cout << "Offset         " << FileHeader.bfOffBits << std::endl;
    std::cout << "Width          " << InfoHeader.biWidth << std::endl;
	std::cout << "Height         " << InfoHeader.biHeight << std::endl;
	std::cout << "Bits per pixel " << InfoHeader.biBitCount << std::endl;
}

Color BMP::GetColor(int x, int y) {
    return Color(*(PixelData + y * ByteLine + 3 * x + 2),
                 *(PixelData + y * ByteLine + 3 * x + 1),
                 *(PixelData + y * ByteLine + 3 * x + 0));
}

void BMP::SetColor(int x, int y, Color color) {
    if (color.Type != Color::RGB) color = color.GetRGB();
    *(PixelData + y * ByteLine + 3 * x + 2) = color.R;
    *(PixelData + y * ByteLine + 3 * x + 1) = color.G;
    *(PixelData + y * ByteLine + 3 * x + 0) = color.B;
}

BMP::~BMP() { free(PixelData); }