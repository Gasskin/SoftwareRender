#include "tgaimage.h"

// ������ɫ
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main()
{
	// ����һ��ͼƬ��������Ϊ��λ�����Ͻ���(0,0)�����½���(100,100)
	TGAImage image(100, 100, TGAImage::RGB);
	// ��������(52,41)��������Ϊ��ɫ
	image.set(52, 41, red);
	// ��ֱ��ת���������ܹ������½Ǳ�Ϊ(0,0)���꣬������������ϵϰ��
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}