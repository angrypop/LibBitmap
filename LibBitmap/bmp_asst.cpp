#include "bmp.h"

void BMP::PrintInfo() {
	std::cout << "Offset         " << FileHeader.bfOffBits << std::endl;
	std::cout << "Width          " << InfoHeader.biWidth << std::endl;
	std::cout << "Height         " << InfoHeader.biHeight << std::endl;
	std::cout << "Bits per pixel " << InfoHeader.biBitCount << std::endl;
	std::cout << "Bytes per line " << ByteLine << std::endl;
	std::cout << "Size           " << InfoHeader.biSize << std::endl;
	std::cout << "SizeImg        " << InfoHeader.biSizeImage << std::endl;
}

void BMP::ResizeFrame(int newW, int newH, std::string cmd) {
	/*
		cmd == "stay" (default):
			color at (x, y) stays unchanged
			Warning: Reducing the frame leads to INFORMATION LOST!
		cmd == "clear":
			Erase the whole image to make it white.
	*/
	int oldByteLine = ByteLine, oldW = InfoHeader.biWidth, oldH = InfoHeader.biHeight;
	BYTE* buf = new BYTE[oldByteLine * oldH];
	for (int i = 0; i < oldByteLine * oldH; i++) buf[i] = PixelData[i];

	InfoHeader.biWidth = newW;
	InfoHeader.biHeight = newH;
	InfoHeader.biSizeImage = newW * newH * InfoHeader.biBitCount;
	ByteLine = (InfoHeader.biWidth * InfoHeader.biBitCount / 8 + 3) / 4 * 4;
	free(PixelData);
	PixelData = new BYTE[ByteLine * InfoHeader.biHeight];
	if (cmd == "clear") {
		for (int i = 0; i < ByteLine * InfoHeader.biHeight; i++) PixelData[i] = 255; /* Default color: white */
	}
	else {
		for (int i = 0; i < ByteLine * InfoHeader.biHeight; i++) PixelData[i] = 255; /* Default color: white */
		for (int y = 0; y < min(oldH, newH); y++)
			for (int x = 0; x < min(oldW, newW); x++) {
				SetColor(x, y, GetColor(x, y, buf, oldByteLine, oldW, oldH));
			}
	}
	free(buf);
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

Color BMP::GetColor(int x, int y, BYTE* buf, int bpl, int W, int H) {
	if (y < 0 || x < 0 || y >= H || x >= W) {
		return Color(0, 0, 0);
	}
	return Color(*(buf + y * bpl + 3 * x + 2),
		*(buf + y * bpl + 3 * x + 1),
		*(buf + y * bpl + 3 * x + 0));
}

void BMP::SetColor(int x, int y, Color color) {
	if (color.Type != Color::RGB) color = color.GetRGB();
	*(PixelData + y * ByteLine + 3 * x + 2) = color.R;
	*(PixelData + y * ByteLine + 3 * x + 1) = color.G;
	*(PixelData + y * ByteLine + 3 * x + 0) = color.B;
}

BMP::BMP() {
	PixelData = NULL;
}

BMP::~BMP() {
	free(PixelData);
	PixelData = NULL;
}