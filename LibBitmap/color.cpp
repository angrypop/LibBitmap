#include "color.h"

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
	int _G = 1.000 * R - 0.394 * (G - 128) - 0.581 * (B - 128);
	_G = max(min(_G, 255), 0);
	int _B = 1.000 * R + 2.028 * (G - 128) + 0.002 * (B - 128);
	_B = max(min(_B, 255), 0);
	return Color(_R, _G, _B, RGB);
}

double Color::GetLuminance() {
	return GetYUV().R;
}

void Color::SetLuminance(int inL) {
	if (Type == RGB) {
		Color c = GetYUV();
		c.R = inL;
		c = c.GetRGB();
		R = c.R; G = c.G; B = c.B;
	}
	else if (Type == YUV) {
		R = inL;
	}
}