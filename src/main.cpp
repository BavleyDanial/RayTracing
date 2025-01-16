#include <iostream>
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi/stb_image_write.h>

void save_frame(std::string_view file, uint8_t* data) {
	std::string fileName(file);
	std::ofstream out_file(fileName);
	out_file.close();
	stbi_write_png(file.data(), 1920, 1080, 3, data, 1920*3);
}

int main() {
	uint8_t* image_data = new uint8_t[1920 * 1080 * 3];

	for (int y = 0; y < 1080; y++) {
		for (int x = 0; x < 1920; x++) {
			image_data[3 * (y * 1920 + x)] = 0;
			image_data[3 * (y * 1920 + x) + 1] = 0;
			image_data[3 * (y * 1920 + x) + 2] = 255;
		}
	}

	save_frame("test.png", image_data);

	return 0;
}
