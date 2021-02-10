#include "tgaimage.h"
#include <windows.h>
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);


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
    TGAImage image(100, 100, TGAImage::RGB);

    __int64 frequency;
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

    __int64 time1,time2;
    QueryPerformanceCounter((LARGE_INTEGER*)&time1);

    //================== 主代码区 ======================

    for (int i = 0; i < 1000000; i++)
    {
        line(13, 20, 80, 40, image, white);
        //line(20, 13, 40, 80, image, red);
        //line(80, 40, 13, 20, image, red);
    }

    //=================================================
    QueryPerformanceCounter((LARGE_INTEGER*)&time2);
    std::cout << (double)(time2 - time1) / frequency << std::endl;

    image.write_tga_file("output.tga");
    return 0;
}

