#pragma once
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <cstdio>
class Color {
   public:
    enum EncodeType { RGB, YUV } Type;
    int R, G, B;
    Color(int inR, int inG, int inB, EncodeType inType = RGB);
    Color operator+(const Color color) const;
    Color operator-(const Color color) const;
    Color operator*(const double multiplier) const;
    Color operator/(const double divider) const;
	bool operator==(const Color color) const;
    Color GetYUV();
    Color GetRGB();
	double GetLuminance();
	void SetLuminance(int inL);
};

class BMP {
   private:
    BYTE* PixelData; /* RGB value of each pixel stored here */
                     /* In the order of G[0][0] B[0][0] R[0][0] G[0][1]...*/
                     /* Memory space will be assigned in read() */
   public:
	enum ImgType {Colored, Grayscale, Binary} Type;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    int ByteLine; /* Bytes per line, which is multiplication of 4 */
	void MakeBinary();
	void MakeGrayscale();
	void Erode(int size = 1);
	void Dilate(int size = 1);
	void Enhance();
	void HistEqualize();
	void HistPrintCSV(std::string FileName);
    bool Read(std::string FileName);
    bool Save(std::string FileName);
    void PrintInfo();
    Color GetColor(int x, int y);
    void SetColor(int x, int y, Color color);
	template <typename Action>
	void ForAllPixels(Action const& action) {
		for (int y = 0; y < InfoHeader.biHeight; y++)
			for (int x = 0; x < InfoHeader.biWidth; x++) 
				action(x, y);
	}
	BMP();
    ~BMP();
};