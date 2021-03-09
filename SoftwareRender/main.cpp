#include "tgaimage.h"

// 定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main()
{
	// 定义一张图片，以像素为单位，左上角是(0,0)，右下角是(100,100)
	TGAImage image(100, 100, TGAImage::RGB);
	// 设置坐标(52,41)处的像素为红色
	image.set(52, 41, red);
	// 垂直翻转，那样就能够让左下角变为(0,0)坐标，符合正常坐标系习惯
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}