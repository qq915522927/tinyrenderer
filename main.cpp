#include <iostream>
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "util.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);

Model *model = NULL;

const int width = 800;
const int height = 800;

// function declaration
void line(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color);
void render_model(const char* obj);

// function definition

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
	// 这里分开讨论了 k >0  和 k < 0的 情况
	// 递推公式， 差不多正好相反
	// 可能可以把一些逻辑合并， 但这样对我更好理解
	if(!decrease){

		while (x <= x1)
		{
			if(steep){
				img.set(y, x, color);
			} else{
				img.set(x, y, color);
			}
			if(d_error <0){
				// get the up point
				x = x +1;
				y = y + 1;
				d_error = d_error - 2*dy + 2 *dx;
			} else {
				x = x + 1;
				d_error = d_error - 2*dy;
			}
		}
	} else {
		while (x <= x1)
		{
			if(steep){
				img.set(y, x, color);
			} else{
				img.set(x, y, color);
			}
			if(d_error <0){
				// get the up point
				x = x +1;
				d_error = d_error - 2*dy;
			} else {
				x = x + 1;
				y = y - 1;
				d_error = d_error - 2*dy - 2*dx;
			}
		}

	}
}

void line(Vec2i v0, Vec2i v1, TGAImage &img, TGAColor color){
	line3(v0.x, v0.y, v1.x, v1.y, img, color);
}
void triangle(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage &img, TGAColor color){
	if(v0.y > v1.y) {swap(v0, v1);}
	if(v1.y > v2.y) {swap(v1, v2);}
	if(v0.y > v1.y) {swap(v0, v1);}

	int total_height  = v2.y - v0.y;
	// 直角三角形， 这个值为0
	int v1_v0_height = v1.y - v0.y;
	// 下半部分
	if(v1_v0_height != 0){
		for(int y=v0.y; y <= v1.y; y ++){
			// A， B 为扫描线与边的两个交点
			// 相似三角形， 求值
			double alpha = (double)(y - v0.y) / total_height;

			Vec2i A = v0 + (v2 - v0) * alpha;

			double theta = (double)(y - v0.y) / (v1_v0_height);
			Vec2i B = v0 + (v1 - v0) * theta;
			if (A.x>B.x) std::swap(A, B);
			for (int j=A.x; j<=B.x; ++j) {
				img.set(j, y, color);
			}
		}

	}
	// 上半部分
	// 直角三角形， 这个值为0
	int v2_v1_height = v2.y - v1.y;
	if(v2_v1_height != 0){
		for(int y=v2.y; y >= v1.y; y--){

			double alpha = (double)(v2.y - y) / total_height;
			Vec2i A = v2 + (v0 - v2) * alpha;


			double theta = (double)(v2.y - y) / v2_v1_height;
			Vec2i B = v2 + (v1 - v2) * theta;
			if(A.x > B.x){swap(A, B);}
			for(int x=A.x; x<=B.x; x++)
				img.set(x, y, color);
		}

	}

}
void test_triangle(){
    TGAImage image(300, 300, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
}

int test_line() {
	TGAImage image(500, 500, TGAImage::RGB);
	int r = 100;
	vector<vector<int> > pixs;
	int ox = 300;
	int oy = 300;
	for(double theta = 0.; theta < 2 * M_PI; theta += 0.2){
		vector<int> position;
		position.push_back(ox);
		position.push_back(oy);
		position.push_back(ox + r * cos(theta));
		position.push_back(oy + r * sin(theta));
		pixs.push_back(position);
	}
	for(vector<vector<int> >::iterator it=pixs.begin(); it != pixs.end(); it++){
		// line2((*it)[0], (*it)[1], (*it)[2], (*it)[3], image, green);
		line3((*it)[0], (*it)[1], (*it)[2], (*it)[3], image, white);
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

void test_line2(){

	TGAImage image(width, height, TGAImage::RGB);
	line3(300, 600, 310, 390, image, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
}
int main(int argc, char const *argv[])
{
	render_model("obj/african_head.obj");
	cout << "Down" << endl;
	return 0;
}

void render_model(const char* obj){

	model = new Model(obj);
	TGAImage img(width, height, TGAImage::RGB);
	cout << "number of face: " << model->nfaces() << endl;
	for(int i=0; i<model->nfaces(); ++i){
		vector<int> face = model->face(i);
		for (int j = 0; j < 3; ++j)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1)%3]);
			int x0 = (v0.x + 1.)* width / 2;
			int y0 = (v0.y + 1.)* height / 2;
			int x1 = (v1.x + 1.)* width / 2;
			int y1 = (v1.y + 1.)* height / 2;
			line3(x0, y0, x1, y1, img, white);
		}
			  Vec2i screen_coords[3];
		  for (int j=0; j<3; j++) { 
			Vec3f world_coords = model->vert(face[j]); 
			screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
			} 
		triangle(screen_coords[0], screen_coords[1], screen_coords[2], img, TGAColor(rand()%255, rand()%255, rand()%255, 255));
	}
	img.flip_vertically();
	img.write_tga_file("output.tga");
	delete model;
}


