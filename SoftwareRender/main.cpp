#include "tgaimage.h"
#include "model.h"

#include <windows.h>
#include <iostream>


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 1000;
const int height = 1000;

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

int main(int argc, char** argv) {

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

