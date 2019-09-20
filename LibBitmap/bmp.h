#pragma once
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
class Color {
   public:
    enum EncodeType { RGB, YUV } Type;
    int R, G, B;
    Color(int inR, int inG, int inB, EncodeType inType = RGB);
    Color operator+(Color color) const;
    Color operator-(const Color color) const;
    Color operator*(const double multiplier) const;
    Color operator/(const double divider) const;
    Color GetYUV();
    Color GetRGB();
};

class BMP {
   private:
    BYTE* PixelData; /* RGB value of each pixel stored here */
                     /* In the order of G[0][0] B[0][0] R[0][0] G[0][1]...*/
                     /* Memory space will be assigned in read() */
   public:
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    int ByteLine; /* Bytes per line, which is multiplication of 4 */
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
    ~BMP();
};