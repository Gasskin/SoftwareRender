#include "tgaimage.h"
#include "model.h"

#include <windows.h>
#include <iostream>


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 800;
const int height = 800;

/// <summary>
/// 绘制直线
/// </summary>
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    bool steep = false;
    // 保证x轴是更长的那个轴，比如(0,0)到(1,5)，x轴只变动1，y轴变动5，此时如果依旧按照x轴绘制，则只会有2个点（x++)
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    { 
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    // 确保从小端绘制到大端
    if (x0 > x1)
    { 
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    // 这里本身是 dy/dx，二下面的if里是 error>0.5
    // 同时乘以 2dx ，可以优化掉所有的除法
    int derror = std::abs(dy) * 2;
    int error = 0;
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
        error += derror;
        if (error > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color)
{
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x)
    {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++)
    {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
    }
}

Vec3f barycentric(Vec2i* pts, Vec2i P)
{
    // 基于右手坐标系
    // ab x ap , bc x bp , ca x cp
    // x1y2 - x2y1
    int res[3] = { 0 };
    for (int i = 0; i < 3; i++)
    {
        Vec2i l1 = pts[(i + 1)%3] - pts[i];
        Vec2i l2 = P - pts[i];
        res[i] = l1.x * l2.y - l2.x * l1.y;
    }
    return Vec3f(res[0], res[1], res[2]);
}

void triangle(Vec2i* pts, TGAImage& image, TGAColor color)
{
   

    int minX = image.get_width() - 1;
    int maxX = 0;
    int minY = image.get_height() - 1;
    int maxY = 0;

    for (int i = 0; i < 3; i++)
    {
        if (pts[i].x < minX)
        {
            minX = pts[i].x;
        }
        if (pts[i].x > maxX)
        {
            maxX = pts[i].x;
        }
        if (pts[i].y < minY)
        {
            minY = pts[i].y;
        }
        if (pts[i].y > maxY)
        {
            maxY = pts[i].y;
        }
    }

    Vec2i P;
    for (P.x = minX; P.x <= maxX; P.x++)
    {
        for (P.y = minY; P.y <= maxY; P.y++)
        {
            Vec3f bc_screen = barycentric(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
            {
                continue;
            }
            image.set(P.x, P.y, color);
        }
    }
}

void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
    if (t0.y == t1.y && t0.y == t2.y) return; // i dont care about degenerate triangles
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++)
    {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}

int main01(int argc, char** argv) {

    TGAImage image(width, height, TGAImage::RGB);
    Model* model = new Model("obj/cmdr.obj");

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);

        float xscale = model->maxX - model->minX;
        float yscale = model->maxY - model->minY;

        float scale = xscale > yscale ? xscale : yscale;

        float movex = 0;
        float movey = 0;
        if (xscale > yscale)
        {
            movey = height * 0.5 - yscale / scale * height * 0.5;
        }
        else
        {
            movex = width * 0.5 - xscale / scale * width * 0.5;
        }

        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            
            int x0 = (v0.x - model->minX) / scale * width + movex;
            int y0 = (v0.y - model->minY) / scale * height + movey;
            int x1 = (v1.x - model->minX) / scale * width + movex;
            int y1 = (v1.y - model->minY) / scale * height + movey;

            line(x0, y0, x1, y1, image, white);
        }
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

int main02()
{
    TGAImage image(width, height, TGAImage::RGB);
    //Model* model = new Model("obj/african_head.obj");
    Model* model = new Model("obj/UZI.obj");
    //Model* model = new Model("obj/cmdr.obj");

    Vec3f light_dir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::cout << (float)(i + 1) / (float)model->nfaces() * 100. << "%" << std::endl;

        std::vector<int> face = model->face(i);

        float xscale = model->maxX - model->minX;
        float yscale = model->maxY - model->minY;

        float scale = xscale > yscale ? xscale : yscale;

        float movex = 0;
        float movey = 0;
        if (xscale > yscale)
        {
            movey = height * 0.5 - yscale / scale * height * 0.5;
        }
        else
        {
            movex = width * 0.5 - xscale / scale * width * 0.5;
        }

        Vec2i screen_coords[3];
        Vec3f world_coords[3];

        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x - model->minX) / scale * width + movex, (v.y - model->minY) / scale * height + movey);
            world_coords[j] = v;
        }

        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0)
        {
            triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
            //triangle2(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

int main()
{
    TGAImage image(width, height, TGAImage::RGB);
    return 0;
}