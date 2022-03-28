#pragma once
#include "../bmpK3D/bmpK3D.h"

bool RGB2YUV(const PIXELBITMAP24& bmp, YUVMAP& yuv);

bool YUV_444_to_420(YUVMAP& yuvFrame);

bool ImprintBMP2YUV(std::fstream& video, uint32_t vWidth, uint32_t vHeight, YUVMAP& yuvFrame, uint32_t widthOffset, uint32_t heightOffset, uint32_t startFrameNumber, uint32_t maxFramesNumber);