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

int main()
{
    TGAImage image(width, height, TGAImage::RGB);
    return 0;
}