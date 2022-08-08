#include <iostream>
#include <fstream>

#include "FFEncoder.h"

int main(int argc, char** argv)
{
	FFEncoder encoder;
	EncodeParams params{ "h264", 1920, 1080, 1024*1024, 30, 5, 1 };
	encoder.Init(params);

	constexpr unsigned int nv12_frame_size = 1920 * 1080 * 3 / 2;
	uint8_t* buffer = (uint8_t*)malloc(nv12_frame_size);

	std::ofstream output("test.h264", std::ios_base::binary);
	uint8_t* out_buf = (uint8_t*)malloc(nv12_frame_size);


	std::ifstream file("test.yuv", std::ios_base::binary);
	if (file.is_open()) {
		int frame_size = 0;
		while (file) {
			file.read((char*)buffer, nv12_frame_size);

			frame_size = encoder.EncodeFrame(buffer, 1920, 1080, out_buf, nv12_frame_size,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			if (frame_size < 0) break;
			else {
				output.write((char*)out_buf, frame_size);
			}
		}
	}

	encoder.FlushEncoder(out_buf);
	encoder.Destroy();

	output.close();
	file.close();

	free(buffer);
	return 0;
}