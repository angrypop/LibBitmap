#define _CRT_SECURE_NO_WARNINGS
#include "bmp.h"

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

void BMP::Enhance() {
	/* Logarithmic Enhancement */
	double Lmax = 0;
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++)
			Lmax = max(Lmax, GetColor(x, y).GetLuminance());
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			Color c = GetColor(x, y);
			c.SetLuminance(255 * log(c.GetLuminance() / 255.0 + 1) / log(Lmax / 255.0 + 1));
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

void BMP::Filter(double *F, int size) {
	/*
		Note that the filter array is
			00 01 02 y
			10 11 12
			20 21 22
			x
		while the coordinate we use is
			y
			20 21 22
			10 11 12
			00 01 02 x
		Rule: Assure the filter array LOOKS as you expect when initializing
	*/
	if (size % 2 != 1) {
		printf("ERROR  @Filter() Size of kernel is even");
		return;
	}
	const int W = InfoHeader.biWidth, H = InfoHeader.biHeight;
	BYTE* buf = new BYTE[H * ByteLine];
	for (int i = 0; i < H * ByteLine; i++) buf[i] = PixelData[i];
	for (int y = 0; y < H; y++)
		for (int x = 0; x < W; x++) {
			double sum_w = 0, l = 0;
			for (int dy = -(size / 2); dy <= size / 2; dy++)
				for (int dx = -(size / 2); dx <= size / 2; dx++) {
					double w = *(F + (size / 2 - dy) * size + size / 2 + dx);
					//w = F[size/2-dy][size/2+dx]
					l += GetColor(x + dx, y + dy, buf, ByteLine, W, H).GetLuminance() * w;
					sum_w += w;
				}
			if(sum_w != 0) l /= sum_w;
			Color c = GetColor(x, y, buf, ByteLine, W, H);
			c.SetLuminance(l);
			SetColor(x, y, c);
		}
	free(buf);
}

void BMP::Filter(std::string cmd, int size) {
	/* commands supported:
		-extend (for laplacian filtering only)
		-fuse (for laplacian filtering only)
		-gray
		-binary
	*/
	if (cmd.find("mean") != -1) {
		double *F = new double[size * size];
		for (int x = 0; x < size; x++)
			for (int y = 0; y < size; y++)
				*(F + y * size + x) = 1;
		Filter((double*)F, size);
	}
	else if (cmd.find("median") != -1) {
		const int W = InfoHeader.biWidth, H = InfoHeader.biHeight;
		BYTE* buf = new BYTE[H * ByteLine];
		for (int i = 0; i < H * ByteLine; i++) buf[i] = PixelData[i];
		for (int y = 0; y < H; y++)
			for (int x = 0; x < W; x++) {
				Color c = GetColor(x, y, buf, ByteLine, W, H);
				std::vector<double> L;
				for (int dy = - size / 2; dy <= size / 2; dy++)
					for (int dx = - size / 2; dx <= size / 2; dx++) {
						L.push_back(GetColor(x + dx, y + dy, buf, ByteLine, W, H).GetLuminance());
					}
				std::sort(L.begin(), L.end());
				c.SetLuminance(L[size * size / 2]);
				SetColor(x, y, c);
			}
		free(buf);
	}
	else if (cmd.find("laplacian") != -1) {
		double F[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0} };
		if (cmd.find("extend") != -1) {
			for (int x = 0; x < 3; x++)
				for (int y = 0; y < 3; y++)
					F[x][y] = -1;
			F[1][1] = 8;
		}
		if(cmd.find("fuse") != -1) F[1][1] += 1;
		Filter((double*)F, 3);
	}
	else {
		std::cout << "ERROR Unresolved command @ Filter()" << std::endl;
	}
	if (cmd.find("gray") != -1) MakeGrayscale();
	if (cmd.find("binary") != -1) MakeBinary();
}

void BMP::HistEqualize() {
	int n[256], cumulative_n[256], n_after[256], setto[256], N = InfoHeader.biHeight*InfoHeader.biWidth;
	memset(n, 0, sizeof(n));
	memset(cumulative_n, 0, sizeof(cumulative_n));
	memset(n_after, 0, sizeof(n_after));
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			n[(int)GetColor(x, y).GetLuminance()]++;
		}
	cumulative_n[0] = n[0];
	for (int k = 1; k <= 255; k++) {
		cumulative_n[k] = cumulative_n[k - 1] + n[k];
		setto[k] = 255 * cumulative_n[k] / N;
		n_after[setto[k]] += n[k];
	}
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			Color c = GetColor(x, y);
			c.SetLuminance(setto[(int)c.GetLuminance()]);
			SetColor(x, y, c);
		}
}

void BMP::MakeBinary() {
	/* Otsu Binarization */
	/* Optimized from O(256*N) to O(N) */
	if (Type == Binary) return;
	if (Type != Grayscale) MakeGrayscale();
	Type = Binary;
	int Threshold, N = InfoHeader.biHeight * InfoHeader.biWidth, bestThreshold = 127;
	double maxvar = 0, var_between, n_foreground = N, n_background = 0, sumY_foreground = 0, sumY_background = 0;
	int cnt[256];/*cnt[n]: number of pixels of Y value n*/
	memset(cnt, 0, sizeof(cnt));
	for (int y = 0; y < InfoHeader.biHeight; y++)
		for (int x = 0; x < InfoHeader.biWidth; x++) {
			cnt[GetColor(x, y).R]++;
			sumY_foreground += GetColor(x, y).R;
		}
	/* foreground: white (l > threshold) */
	for (Threshold = 0; Threshold < 255; Threshold++) {
		n_foreground -= cnt[Threshold];
		n_background += cnt[Threshold];
		sumY_foreground -= cnt[Threshold] * Threshold;
		sumY_background += cnt[Threshold] * Threshold;
		var_between = n_foreground / N * n_background / N * (sumY_foreground / n_foreground - sumY_background / n_background)*(sumY_foreground / n_foreground - sumY_background / n_background);
		//printf("[%d] cnt:%d n_f:%d n_b:%d sumY_f:%d sumY_b:%d var:%lf\n", Threshold, cnt[Threshold], (int)n_foreground, (int)n_background, (int)sumY_foreground, (int)sumY_background, var_between);
		if (var_between >= maxvar) {
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
	std::cout << "       @MakeBinary() Best threshold: " << bestThreshold << std::endl;
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
			int Y = GetColor(x, y).GetLuminance();
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

void BMP::Mirror(Direction MirrorDir) {
	const double W = InfoHeader.biWidth, H = InfoHeader.biHeight;
	if (MirrorDir == Horizontal) {
		double F[2][3] = {
			{-1,0,W},
			{0,1,0}
		};
		LinearTransform(W, H, F);
	}
	else if (MirrorDir == Vertical) {
		double F[2][3] = {
			{1,0,0},
			{0,-1,H}
		};
		LinearTransform(W, H, F);
	}
}

void BMP::Rotate(int degree) {
	/* degree:[0,360) , anti-clockwise */
	degree %= 360;
	const double theta=degree/180.0*3.14159265,
		oldW = InfoHeader.biWidth,
		oldH = InfoHeader.biHeight, diagonal_L = sqrt(oldW*oldW + oldH * oldH);
	const int oldByteLine = ByteLine,
		newW = max(fabs(diagonal_L*cos(atan(oldH / oldW) + theta)), fabs(diagonal_L*cos(-atan(oldH / oldW) + theta))),
		newH = max(fabs(diagonal_L*sin(atan(oldH / oldW) + theta)), fabs(diagonal_L*sin(-atan(oldH / oldW) + theta)));
	double F[2][3] = {
		{cos(theta),-sin(theta),0},
		{sin(theta),cos(theta),0}
	};
	LinearTransform(newW, newH, F, "autobias");
}

void BMP::Scale(double factorx, double factory) {
	double F[2][3] = { 
		{fabs(factorx),0,0},
		{0,fabs(factory),0}
	};
	LinearTransform(
		InfoHeader.biWidth*factorx,
		InfoHeader.biHeight*factory,
		F);
}

void BMP::Shear(double factor, Direction ShearDir) {
	factor = fabs(factor);
	if (ShearDir == BMP::Horizontal) {
		double F[2][3] = {
			{1,factor,0},
			{0,1,0}
		};
		LinearTransform(InfoHeader.biWidth + factor * InfoHeader.biHeight, InfoHeader.biHeight, F);
	}
	else if (ShearDir == BMP::Vertical) {
		double F[2][3] = {
			{1,0,0},
			{factor,1,0}
		};
		LinearTransform(InfoHeader.biWidth, InfoHeader.biHeight + factor * InfoHeader.biWidth, F);
	}
}

void BMP::Translate(int dx, int dy) {
	const int oldW = InfoHeader.biWidth, oldH = InfoHeader.biHeight,
		newW = InfoHeader.biWidth + abs(dx), newH = InfoHeader.biHeight + abs(dy);
	/* In LinearTransform() the image is at bottom-left by default */
	if (dx < 0) dx = 0;
	if (dy < 0) dy = 0;
	double F[2][3] = {
			{1,0,dx},
			{0,1,dy}
	};
	LinearTransform(newW, newH, F);
}

void BMP::LinearTransform(int newW, int newH, double F[][3], std::string cmd){
	/* (newx,newy,1) = F * (oldx,oldy,1) + c */
	const double oldW = InfoHeader.biWidth, oldH = InfoHeader.biHeight;
	const int oldByteLine = ByteLine;
	double biasx = 0, biasy = 0;
	BYTE* buf = new BYTE[oldH*oldByteLine];
	for (int i = 0; i < oldH*oldByteLine; i++) buf[i] = PixelData[i];
	ResizeFrame(newW, newH, "clear");

	/* Interpreting commands */
	if (cmd.find("autobias") != std::string::npos) {
		double oldx[4] = { 0,0,oldW,oldW }, oldy[4] = { 0,oldH,0,oldH };
		for (int i = 0; i < 4; i++) {
			double newx = F[0][0] * oldx[i] + F[0][1] * oldy[i] + F[0][2],
				newy = F[1][0] * oldx[i] + F[1][1] * oldy[i] + F[1][2];
			biasx = min(biasx, newx);
			biasy = min(biasy, newy);
		}
		biasx = fabs(biasx); biasy = fabs(biasy);
		printf("WARNING autobias enabled. biasx:%d biasy:%d\n", (int)biasx, (int)biasy);
	}

	/* Bilinear Interpolation */
	for (int y = 0; y < newH; y++)
		for (int x = 0; x < newW; x++) {
			double oldx = (F[1][1] * (x - biasx) - F[1][1] * F[0][2] - F[0][1] * (y - biasy) + F[0][1] * F[1][2]) / (F[0][0] * F[1][1] - F[0][1] * F[1][0]),
				oldy = (F[1][0] * (x - biasx) - F[0][2] * F[1][0] - F[0][0] * (y - biasy) + F[0][0] * F[1][2]) / (F[0][1] * F[1][0] - F[0][0] * F[1][1]);
			if (oldx < 0 || oldx >= oldW || oldy < 0 || oldy >= oldH) continue;
			double x1 = (int)oldx, x2 = (int)oldx + 1, y1 = (int)oldy, y2 = (int)oldy + 1;
			Color c[4] = { GetColor(x1,y1,buf,oldByteLine,oldW,oldH),
				GetColor(x1,y2,buf,oldByteLine,oldW,oldH),
				GetColor(x2,y1,buf,oldByteLine,oldW,oldH),
				GetColor(x2,y2,buf,oldByteLine,oldW,oldH)
			};
			Color newcolor = (c[0] * 1000.0 * fabs((x2 - oldx)*(y2 - oldy))
				+ c[1] * 1000.0 * fabs((x2 - oldx) * (y1 - oldy))
				+ c[2] * 1000.0 * fabs((x1 - oldx) * (y2 - oldy))
				+ c[3] * 1000.0 * fabs((x1 - oldx) * (y1 - oldy)))
				/ (1000.0 * (x2 - x1) * (y2 - y1));
			/* Factor 1000.0: avoid integer traction in intermediate steps */
			SetColor(x, y, newcolor);
		}
}
