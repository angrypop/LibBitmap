#pragma once
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "color.h"

class BMP {
   private:
    BYTE* PixelData; /* RGB value of each pixel stored here */
                     /* In the order of G[0][0] B[0][0] R[0][0] G[0][1]...*/
                     /* Memory space will be assigned in read() */
   public:
	enum ImgType {Colored, Grayscale, Binary} Type;
	enum Direction { Horizontal, Vertical };
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    int ByteLine; /* Bytes per line, which is multiplication of 4 */

	/* --Transformations-- */
	/* bmp_trans.cpp */
	void Dilate(int size = 1);
	void Enhance();
	void Erode(int size = 1);
	void Filter(double *F, int size);
	void Filter(std::string cmd, int size = 3);
	void HistEqualize();
	void MakeBinary();
	void MakeGrayscale();
	void Mirror(Direction MirrorDir = Horizontal);
	void Rotate(int degree);
	void Scale(double factorx, double factory);
	void Shear(double factor, Direction ShearDir = Horizontal);
	void Translate(int dx, int dy);
	void LinearTransform(int newW, int newH, double F[][3], std::string cmd = "empty");

	/* --I/O Functions--*/
	/* bmp_io.cpp */
    bool Read(std::string FileName);
    bool Save(std::string FileName);
	void HistPrintCSV(std::string FileName);


	/* --Asst Functions-- */
	/* bmp_asst.cpp */
	void PrintInfo();
	void ResizeFrame(int newW, int newH, std::string cmd = "stay");
	Color GetColor(int x, int y);
	static Color GetColor(int x, int y, BYTE* buf, int bpl, int W, int H);
	/*	Generalized version of GetColor(). 
		Allows read from buffer buf instead of PixelDate. */
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
