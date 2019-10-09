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

int Ymax = 0, Ymin = 255;

int main() {
    BMP Monochrome;
    Monochrome.Read("../Image/Input2.bmp");
    Monochrome.ForAllPixels([&](int x, int y) {
        /* Get the maximum and minimum of Y channel (in YUV encoding system) */
        int Y = Monochrome.GetColor(x, y).GetYUV().R;
        Ymax = max(Y, Ymax);
        Ymin = min(Y, Ymin);
    });
    Monochrome.ForAllPixels([&](int x, int y) {
        Color c = Monochrome.GetColor(x, y);
        /* Grayscale rescaled to fill range [0,255] */
        c.R = c.G = c.B = 255 * (c.GetYUV().R - Ymin) /
                          (Ymax - Ymin); /* R = G = B = Scaled Y */
        Monochrome.SetColor(x, y, c);
    });
    Monochrome.Save("../Image/Monochrome2.bmp");

    BMP Darkened;
    Darkened.Read("../Image/Input2.bmp");
    Darkened.ForAllPixels([&](int x, int y) {
        /* Luminance of the image is reduced by 30% */
        Color c = Darkened.GetColor(x, y);
        c = c.GetYUV();
        c.R = c.R * 0.7; /* Y *= 70% */
        Darkened.SetColor(x, y, c);
    });
    Darkened.Save("../Image/Darkened2.bmp");

    return 0;
}