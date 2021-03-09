#include "tgaimage.h"

// 定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

// C(x0,y0),D(x1,y1)
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	// 这里表示从Xc开始，每次移动1个像素，计算对应的Yc
	for (int x = x0; x <= x1; x++)
	{
		// 这两步看着很迷，其实就是上面两点式转化了一下而已，也就是在求对应的y，可以带入自己验证一下
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1. - t) + y1 * t;
		image.set(x, y, color);
	}
}

int main()
{
	TGAImage image(500, 500, TGAImage::RGB);

	line(0, 0, 500, 500, image, red);

	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}



