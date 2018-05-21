#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdint.h>

typedef uint8_t u8;

enum ColorType {
	RGB,
	RGBA,
	BGR,
	BGRA
};

class Image {
public:
	int width;
	int height;
	u8* imageData;
	ColorType colorType;

	void Release();
};

int LoadImageFromFile(const char*, Image**, bool = false);
int LoadPNGFromMemory(void*, Image**);
int LoadBMPFromMemory(void*, Image**);

inline void Image::Release() {
	delete[] imageData;
	delete this;
}

#endif