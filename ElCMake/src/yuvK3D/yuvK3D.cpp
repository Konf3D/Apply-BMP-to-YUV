#include "yuvK3D.h"
#include <thread>
static void RGB2YUV_Y(const PIXELBITMAP24& bmp, std::vector<uint8_t>& dst)
{
	for (uint32_t i = 0; i < dst.size(); i++)
	{
		dst[i] = ((66 * bmp.r[i] + 129 * bmp.g[i] + 25 * bmp.b[i] + 128) >> 8) + 16;
	}
}
static void RGB2YUV_U(const PIXELBITMAP24& bmp, std::vector<uint8_t>& dst)
{
	for (uint32_t i = 0; i < dst.size(); i++)
	{
		dst[i] = ((-38 * bmp.r[i] - 74 * bmp.g[i] + 112 * bmp.b[i] + 128) >> 8) + 128;
	}
}
static void RGB2YUV_V(const PIXELBITMAP24& bmp, std::vector<uint8_t>& dst)
{
	for (uint32_t i = 0; i < dst.size(); i++)
	{
		dst[i] = ((112 * bmp.r[i] - 94 * bmp.g[i] - 18 * bmp.b[i] + 128) >> 8) + 128;
	}
}
bool RGB2YUV(const PIXELBITMAP24& bmp, YUVMAP& yuv)
{
	if (bmp.r.size() != bmp.g.size() || bmp.g.size() != bmp.b.size())
		return false;

	yuv.width = bmp.height;
	yuv.height = bmp.height;

	yuv.y.resize(bmp.r.size());
	yuv.u.resize(bmp.g.size());
	yuv.v.resize(bmp.b.size());

	//BT.601-07 rec. RGB888 to YUV444 https://www.itu.int/rec/R-REC-BT.601-7-201103-I/en
	//https://docs.microsoft.com/ru-ru/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#converting-rgb888-to-yuv-444
	std::thread u(RGB2YUV_U, std::ref(bmp), std::ref(yuv.u));
	std::thread v(RGB2YUV_V, std::ref(bmp), std::ref(yuv.v));
	std::thread y(RGB2YUV_Y, std::ref(bmp), std::ref(yuv.y));

	u.join();
	v.join();
	y.join();

	return true;
}

bool YUV_444_to_420(YUVMAP& yuvFrame)
{
	const uint32_t origin = yuvFrame.y.size();
	if (yuvFrame.u.size() != yuvFrame.v.size() || origin != yuvFrame.u.size())
		return false;

	std::thread comp_u(
		[&yuvFrame]()
		{
			std::vector<uint8_t> u;
			for (uint32_t i = 0; i < yuvFrame.height; i += 2)
			{
				for (uint32_t j = 0; j < yuvFrame.width; j += 2)
				{
					u.push_back(yuvFrame.u[i * yuvFrame.width + j]);
				}
			}
			u.resize(yuvFrame.height * yuvFrame.width / 4);
			std::swap(u, yuvFrame.u);
		}
	);

	std::thread comp_v(
		[&yuvFrame]()
		{
			std::vector<uint8_t> v;
			for (uint32_t i = 0; i < yuvFrame.height; i += 2)
			{
				for (uint32_t j = 0; j < yuvFrame.width; j += 2)
				{
					v.push_back(yuvFrame.v[i * yuvFrame.width + j]);
				}
			}
			v.resize(yuvFrame.height * yuvFrame.width / 4);
			std::swap(v, yuvFrame.v);
		}
	);

	comp_u.join();
	comp_v.join();
	return true;
}

bool ImprintBMP2YUV(std::fstream& video, uint32_t vWidth, uint32_t vHeight, YUVMAP& yuvFrame, uint32_t widthOffset, uint32_t heightOffset, uint32_t startFrameNumber, uint32_t maxFramesNumber)
{
	if (widthOffset + yuvFrame.width > vWidth)
		return false;
	if (heightOffset + yuvFrame.height > vHeight)
		return false;

	uint32_t frameNumber = 0;
	video.seekp(heightOffset * vWidth + widthOffset, std::fstream::beg);
	while (frameNumber < maxFramesNumber)
	{
		for (uint32_t i = 0; i < yuvFrame.height; i++)
		{
			for (uint32_t j = 0; j < yuvFrame.width; j++)
			{
				video.put(yuvFrame.y[i * yuvFrame.width + j]);
			}
			video.seekp(vWidth - yuvFrame.width, std::fstream::cur);
		}
		video.seekp(vWidth * vHeight * frameNumber / 2 * 3 + vWidth * vHeight + (heightOffset * vWidth / 2 + widthOffset) / 2, std::fstream::beg);
		for (uint32_t i = 0; i < yuvFrame.height / 2; i++)
		{
			for (uint32_t j = 0; j < yuvFrame.width / 2; j++)
			{
				video.put(yuvFrame.u[i * yuvFrame.width / 2 + j]);
			}
			video.seekp((vWidth - yuvFrame.width) / 2, std::fstream::cur);
		}
		video.seekp(vWidth * vHeight * frameNumber / 2 * 3 + vWidth * vHeight * 5 / 4 + (heightOffset * vWidth / 2 + widthOffset) / 2, std::fstream::beg);
		for (uint32_t i = 0; i < yuvFrame.height / 2; i++)
		{
			for (uint32_t j = 0; j < yuvFrame.width / 2; j++)
			{
				video.put(yuvFrame.v[i * yuvFrame.width / 2 + j]);
			}
			video.seekp((vWidth - yuvFrame.width) / 2, std::fstream::cur);
		}
		frameNumber++;
		video.seekp(vWidth * vHeight * frameNumber / 2 * 3 + heightOffset * vWidth + widthOffset, std::fstream::beg);
	}
	return true;
}