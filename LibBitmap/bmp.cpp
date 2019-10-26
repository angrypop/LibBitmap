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

bool Color::operator==(const Color color) const {
	return R == color.R && G == color.G && B == color.B;
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
	/* Otsu Binarization */
	/* Optimized from O(256*N) to O(N) */
	if (Type == Binary) return;
	if (Type != Grayscale) MakeGrayscale();
	Type = Binary;
	int Threshold, N = InfoHeader.biHeight * InfoHeader.biWidth, bestThreshold = 0;
	double maxvar = 0, var_between, n_foreground = 0, n_background = N, sumY_foreground = 0, sumY_background = 0;
	int cnt[256];/*cnt[n]: number of pixels of Y value n*/
	memset(cnt, 0, sizeof(cnt));
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			cnt[GetColor(x, y).R]++;
			sumY_background += GetColor(x, y).R;
		}

	for (Threshold = 1; Threshold < 255; Threshold++) {
		n_foreground += cnt[Threshold];
		n_background -= cnt[Threshold];
		sumY_foreground += cnt[Threshold] * Threshold;
		sumY_background -= cnt[Threshold] * Threshold;
		var_between = n_foreground / N * n_background / N * (sumY_foreground / n_foreground - sumY_background / n_background)*(sumY_foreground / n_foreground - sumY_background / n_background);
		if (var_between > maxvar) {
			maxvar = var_between;
			bestThreshold = Threshold;
		}
	}
	Threshold = bestThreshold;
	/*
	------------ THE UNOPTIMIZED VERSION ------------
	double w_foreground, w_background, u_foreground, u_background;
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
	}*/
	std::cout << "Best threshold: " << bestThreshold << std::endl;
	Color black = Color(0, 0, 0), white = Color(255,255,255);
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			if (GetColor(x, y).R < Threshold) SetColor(x, y, black);
			else SetColor(x, y, white);
		}
}

void BMP::MakeGrayscale() {
	Type = Grayscale;
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
			if (Ymax - Ymin != 0) c.R = c.G = c.B = 255 * (c.GetYUV().R - Ymin) /
				(Ymax - Ymin); /* R = G = B = Scaled Y */
			else c.R = c.G = c.B = c.GetYUV().R;/* Pure color security check */
			SetColor(x, y, c);
		}
}

void BMP::Erode(int size) {
	if (Type != Binary) MakeBinary();
	Color  black = Color(0, 0, 0), white = Color(255, 255, 255);
	std::vector<std::pair<int, int>> ErodeList;
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			if (GetColor(x, y) == black) {
				bool added = false;
				for (int ty = y - size; ty <= y + size; ty++) {
					for (int tx = x - size; tx <= x + size; tx++) {
						if (ty < 0 || ty >= InfoHeader.biHeight || tx < 0 || tx >= InfoHeader.biWidth) continue;
						if (GetColor(tx, ty) == white) {
							ErodeList.push_back(std::make_pair(x, y));
							added = true;
							break;
						}
					}
					if (added) break;
				}
			}
		}
	for (std::vector<std::pair<int, int>>::iterator it = ErodeList.begin(); it != ErodeList.end(); it++) {
		SetColor(it->first, it->second, white);
	}
}

void BMP::Dilate(int size) {
	if (Type != Binary) MakeBinary();
	Color  black = Color(0, 0, 0), white = Color(255, 255, 255);
	std::vector<std::pair<int, int>> DilateList;
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			if (GetColor(x, y) == white) {
				bool deleted = false;
				for (int ty = y - size; ty <= y + size; ty++) {
					for (int tx = x - size; tx <= x + size; tx++) {
						if (ty < 0 || ty >= InfoHeader.biHeight || tx < 0 || tx >= InfoHeader.biWidth) continue;
						if (GetColor(tx, ty) == black) {
							DilateList.push_back(std::make_pair(x, y));
							deleted = true;
							break;
						}
					}
					if (deleted) break;
				}
			}
		}
	for (std::vector<std::pair<int, int>>::iterator it = DilateList.begin(); it != DilateList.end(); it++) {
		SetColor(it->first, it->second, black);
	}
}

bool BMP::Read(std::string FileName) {
	Type = Original;
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
	//if (InfoHeader.biSizeImage <= 0) return 0;
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
	std::cout << "Bytes per line " << ByteLine << std::endl;
	std::cout << "Size           " << InfoHeader.biSize << std::endl;
	std::cout << "SizeImg        " << InfoHeader.biSizeImage << std::endl;
}

Color BMP::GetColor(int x, int y) {
	if (y < 0 || x < 0 || y >= InfoHeader.biHeight || x >= InfoHeader.biWidth) {
		std::cout << "WARNING invalid color query" << std::endl;
		return Color(0, 0, 0);
	}
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