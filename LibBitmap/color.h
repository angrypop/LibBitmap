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
	Color();
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

	void Print();
};
