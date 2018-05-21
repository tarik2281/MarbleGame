
#include <stdint.h>
#include <string>
#include <algorithm>

#include <png.h>

#include "Image.h"

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

int LoadPNGFromFile(FILE*, Image*, bool);
int LoadBMPFromFile(FILE*, Image*, bool);

int LoadImageFromFile(const char* pPath, Image** outImage, bool addAlpha) {
	std::string path(pPath);
	unsigned pos = path.find_last_of('.');
	std::string extension = path.substr(pos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

	FILE* file = fopen(pPath, "rb");

	if (!file) {
		return 1;
	}

	Image* image = DBG_NEW Image();
	
	int res = 0;
	if (extension.compare("png") == 0)
		res = LoadPNGFromFile(file, image, addAlpha);
	else if (extension.compare("bmp") == 0)
		res = LoadBMPFromFile(file, image, addAlpha);

	fclose(file);

	*outImage = image;

	return 0;
}

int LoadPNGFromFile(FILE* stream, Image* outImage, bool addAlpha) {
	png_byte header[8];

	fread(header, 1, 8, stream);

	if (png_sig_cmp(header, 0, 8))
	{
		//fprintf(stderr, "error: %s is not a PNG.\n", path);
		return 1;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		//fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		return 1;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		//fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 1;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		//fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return 1;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		//fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return 1;
	}

	// init png reading
	png_init_io(png_ptr, stream);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	if (addAlpha && color_type == PNG_COLOR_TYPE_RGB)
		png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

	outImage->colorType = ColorType::RGBA;
	
	if (!addAlpha && color_type == PNG_COLOR_TYPE_RGB)
		outImage->colorType = ColorType::RGB;

	outImage->width = temp_width;
	outImage->height = temp_height;

	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes - 1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	outImage->imageData = DBG_NEW u8[rowbytes * temp_height * sizeof(png_byte)];

	if (outImage->imageData == NULL)
	{
		//fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return 1;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep * row_pointers = (png_bytepp)malloc(temp_height * sizeof(png_bytep));
	if (row_pointers == NULL)
	{
		//fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		// free(image_data);
		return 1;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	unsigned int i;
	for (i = 0; i < temp_height; i++)
	{
		row_pointers[temp_height - 1 - i] = outImage->imageData + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_pointers);

	return 0;
}


#pragma pack(2)
struct BITMAPFILEHEADER {
	u16 bfType;
	u32 bfSize;
	u32 bfReserved;
	u32 btOffBits;
};
#pragma pack()

struct BITMAPINFOHEADER {
	u32 biSize;
	s32 biWidth;
	s32 biHeight;
	u16 biPlanes;
	u16 biBitCount;
	u32 biCompression;
	u32 biSizeImage;
	s32 biXPelsPerMeter;
	s32 biYPelsPerMeter;
	u32 biClrUsed;
	u32 biClrImportant;
};



int LoadBMPFromFile(FILE* stream, Image* outImage, bool addAlpha) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;

	fread(&header, 1, sizeof(BITMAPFILEHEADER), stream);
	fread(&info, 1, sizeof(BITMAPINFOHEADER), stream);

	outImage->imageData = DBG_NEW u8[info.biSizeImage];
	fread(outImage->imageData, 1, info.biSizeImage, stream);

	outImage->colorType = ColorType::BGR;
	outImage->width = info.biWidth;
	outImage->height = info.biHeight;

	if (addAlpha) {
		outImage->colorType = ColorType::BGRA;
		
		u8* temp = DBG_NEW u8[info.biSizeImage * 4 / 3];
		for (unsigned int i = 0; i < info.biSizeImage / 3; i++) {
			memcpy(temp + (i * 4), outImage->imageData + (i * 3), 3);
			temp[i * 4] = 0xFF;
		}
		delete [] outImage->imageData;
		outImage->imageData = temp;
	}

	return  0;
}