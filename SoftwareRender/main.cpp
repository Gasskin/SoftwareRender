#include "tgaimage.h"

// 定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	// ==== 以上内容无改动 ====
	// 在这里，我们把所有derror相关的变量全部*2dx，然后化简
	// (dy/dx)*2dx=2dy
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		// 这里换成derror2
		error2 += derror2;
		// 0.5*2dx=dx
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			// 1*2dx=2dx
			error2 -= dx * 2;
		}
	}
}

int main()
{
	TGAImage image(500, 500, TGAImage::RGB);

	line(500, 500, 0, 0, image, red);

	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}



