#include "tgaimage.h"
#include <iostream>
#include <cmath>

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color){
	for(double t=0.; t<=1;t += 0.01){
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		img.set(x, y, color);
	}
}
/**
 * 微分 斜截式
 */
void line2(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color){


	double t = (y1 - y0) / (double)(x1 - x0);

	if(abs(t) <= 1){
		if(x0 > x1){
			swap(x0 ,x1);
			swap(y0, y1);
		}
	    t = (y1 - y0) / (double)(x1 - x0);
		for(int x=x0; x <= x1; ++x ){
			int y = y0 + (x - x0) * t;
			img.set(x, y, color);
		}
	} else {
		if(y0 > y1){
			swap(x0, x1);
			swap(y0, y1);
		}
		t = (x1 - x0) / (double) (y1 - y0);
		for(int y=y0; y <= y1; ++y){
			int x = (y - y0) * t + x0;
			img.set(x, y, color);
		}
	}
}

/*
* 中点线算法
* 增量运算
* 没有浮点数运算
*/
void line3(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color){


	bool steep = false;
	if(abs(y1-y0) > abs(x1 - x0)){
		// if the line is steep, swap x, y
		steep = true;
		swap(x0, y0);
		swap(x1, y1);
	}
	if(x1 < x0){
		cout << "x1 < x0" << endl;
		swap(x1, x0);
		swap(y1, y0);
	}
	bool decrease(false);
	if(y1 < y0){
		decrease = true;
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int d_error = dx - 2 * dy;
	int x = x0;
	int y = y0;
	while (x <= x1)
	{
		if(d_error <=0){
			// get the up point
			x = x +1;
			if(decrease){
				// keep y

			} else{
				y = y + 1;
			}
			d_error = d_error - 2*dy + 2 *dx;
		} else {
			// get the down point
			x = x + 1;
			if(decrease){
				y = y - 1;
			}
			d_error = d_error - 2*dy;
		}
		if(steep){
			img.set(y, x, color);
		} else{
			img.set(x, y, color);
		}
	}
}

int test_line() {
	TGAImage image(500, 500, TGAImage::RGB);
	line3(200, 200, 300, 300, image, red);
	// steep
	line3(100, 100, 200, 400, image, white);

	for(int y=200; y<= 540; y+=20){
		line3(100, 100, 500, y, image, red);
	}


	// k < 0
	line3(100, 500, 500, 100, image, white);

	// k == 0
	line3(400, 200, 100, 200, image, red);


	// k > 0
	line3(400, 500, 100, 200, image, green);


	// line3(200, 200, 230, 300, image, white);

	// line3(300, 200, 200, 300, image, green);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}


