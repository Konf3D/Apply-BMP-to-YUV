#pragma once
#include <fstream>
#include <vector>
#include <thread>
struct BITMAPFILEHEADER{
	uint16_t Signature;
	uint32_t Size;
	uint16_t Reserved1;
	uint16_t Reserved2;
	uint32_t BitsOffset;
};

struct BITMAPHEADER{
	uint32_t HeaderSize;
	int32_t Width;
	int32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t SizeImage;
	int32_t PelsPerMeterX;
	int32_t PelsPerMeterY;
	uint32_t ClrUsed;
	uint32_t ClrImportant;
} ;

struct PIXELBITMAP24
{
	std::vector<uint8_t> r;
	std::vector<uint8_t> g;
	std::vector<uint8_t> b;
	uint32_t width = 0;
	uint32_t height = 0;
};

struct YUVMAP
{
	std::vector<uint8_t> y;
	std::vector<uint8_t> u;
	std::vector<uint8_t> v;
	uint32_t width = 0;
	uint32_t height = 0;
};

void VMirrorFrame(std::vector<uint8_t>& res, uint32_t width, uint32_t height);
void HVMirrorFrame(std::vector<uint8_t>& res);
static void CorrectBitMap(PIXELBITMAP24& bmp);
bool FetchBitMap(std::fstream& file, PIXELBITMAP24& bmp);