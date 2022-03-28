#include "bmpK3D.h"

void VMirrorFrame(std::vector<uint8_t>& res, uint32_t width, uint32_t height)
{
	for (uint32_t i = 0; i < height; i++)
	{
		std::reverse(res.begin() + i * width, res.begin() + (i + 1) * width);
	}
}
void HVMirrorFrame(std::vector<uint8_t>& res)
{
	std::reverse(res.begin(), res.end());
}
static void CorrectBitMap(PIXELBITMAP24& bmp)
{
	std::thread r(HVMirrorFrame, std::ref(bmp.r));
	std::thread g(HVMirrorFrame, std::ref(bmp.g));
	std::thread b(HVMirrorFrame, std::ref(bmp.b));
	r.join();
	g.join();
	b.join();
}
bool FetchBitMap(std::fstream& file, PIXELBITMAP24& bmp)
{

	if (!file.good())
		return false;
	file.seekg(0, std::fstream::beg);
	BITMAPFILEHEADER bmfh;

	file.read((char*)(&bmfh.Signature), 2);

	if (bmfh.Signature != 0x4d42) // 4d42 -> BM file signature
		return false;

	//get RGB bits array ptr offset
	file.seekg(sizeof(bmfh.Size) + sizeof(bmfh.Reserved1) + sizeof(bmfh.Reserved2), std::ios::cur);
	file.read((char*)(&bmfh.BitsOffset), 4);

	BITMAPHEADER bmh;
	file.read((char*)(&bmh), sizeof(bmh));

	if (bmh.Compression > 0) // if compression is other than BI_RGB(0) type
		return false;

	if (bmh.BitCount != 24) // bitmap must be an rgb structure
		return false;

	const int padding = (4 - (bmh.Width * 3) % 4) % 4;
	bmp.width = bmh.Width;
	bmp.height = bmh.Height;

	file.seekg(bmfh.BitsOffset, std::fstream::beg);

	for (uint32_t i = 0; i < bmp.height; i++)
	{
		for (uint32_t j = 0; j < bmp.width; j++)
		{
			bmp.b.push_back(file.get());
			bmp.g.push_back(file.get());
			bmp.r.push_back(file.get());
		}
		file.seekg(padding, std::fstream::cur);
	}

	std::thread r(VMirrorFrame, std::ref(bmp.r), std::ref(bmp.width), std::ref(bmp.height));
	std::thread g(VMirrorFrame, std::ref(bmp.g), std::ref(bmp.width), std::ref(bmp.height));
	std::thread b(VMirrorFrame, std::ref(bmp.b), std::ref(bmp.width), std::ref(bmp.height));

	r.join();
	g.join();
	b.join();
	file.seekg(0, std::fstream::beg);
	CorrectBitMap(bmp);
	return true;
}