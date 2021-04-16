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

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--; )
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) 
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); 
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}

Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main()
{
	model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir(0, 0, -1);

	float* zbuffer = new float[width * height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = world2screen(v);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0)
		{
			triangle(screen_coords, zbuffer,image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}

	image.flip_vertically(); 
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}



