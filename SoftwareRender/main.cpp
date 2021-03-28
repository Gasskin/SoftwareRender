#include "tgaimage.h"
#include "geometry.h"
#include "geometry.cpp"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

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

void line(Vec2i t0, Vec2i t1,TGAImage& image, TGAColor color)
{
	line(t0.x, t0.y, t1.x, t1.y, image, color);
}

int barycentric(Vec2i* pts, Vec2i P)
{
	int pre = -1;
	for (int i = 0; i < 3; i++)
	{
		// AB = B - A
		int x1 = pts[(i + 1) % 3][0] - pts[i][0];
		int y1 = pts[(i + 1) % 3][1] - pts[i][1];
		// AP = P - A
		int x2 = P[0] - pts[i][0];
		int y2 = P[1] - pts[i][1];
		// x2y1-x1y2
		int res = (x2 * y1 - x1 * y2);
		if (pre == -1)
		{
			pre = res > 0 ? 1 : 0;
		}
		res = res > 0 ? 1 : 0;
		if (res != pre)
		{
			return 0;
		}
	}
	return 1;
}

void triangle(Vec2i* pts, TGAImage& image, TGAColor color)
{
	// 这里是包围盒的最大最小范围
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	// 这个是用来限制我们的范围，总不能比图片本身大小还大吧？
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	// 分别对三个顶点遍历
	for (int i = 0; i < 3; i++)
	{
		// 分别对xy遍历
		for (int j = 0; j < 2; j++)
		{
			// 这里嵌套了两层，先比较当前记录的最小和当前顶点，再比较0和最小点
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec2i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			int res = barycentric(pts, P);
			if (res == 0)
				continue;
			image.set(P.x, P.y, color);
		}
	}
}



int main()
{
	model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir(0, 0, -1);

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0)
		{
			triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}

	image.flip_vertically(); 
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}



