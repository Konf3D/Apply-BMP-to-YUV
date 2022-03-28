// ElCMake.cpp: определяет точку входа для приложения.
//

#include "ElCMake.h"
#include "src/bmpK3D/bmpK3D.h"
#include "src/yuvK3D/yuvK3D.h"
using namespace std;

int main()
{
	std::fstream myfile;

	myfile.open("test.bmp", std::fstream::in | std::fstream::binary);

	if (!myfile.good())
	{
		myfile.close();
		return 0;
	}

	// BMP read sequence
	PIXELBITMAP24 bmp;
	FetchBitMap(myfile, bmp);
	myfile.close();
	// Convertion sequence
	YUVMAP yuvFromBMP;
	RGB2YUV(std::move(bmp), yuvFromBMP); //convert RGB888 -> YUV444, bmp not needed after
	YUV_444_to_420(yuvFromBMP); // convert YUV444 -> YUV420


	myfile.open("out.yuv", std::fstream::binary | std::fstream::in | std::fstream::out); //864x480 yuv with 332 frames vs 224x224 bmp
	ImprintBMP2YUV(myfile, 864, 480, yuvFromBMP, 124, 124);
	myfile.close();


	return 0;
}
