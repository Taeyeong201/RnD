#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern "C" {
#include <turbojpeg.h>
}
#include <iostream>
#include <chrono>

#ifdef _WIN32
#ifdef _DEBUG
#define strcasecmp  _stricmp
#define strncasecmp  _strnicmp
#else
#define strcasecmp  stricmp
#define strncasecmp  strnicmp
#endif
#endif

#define THROW(action, message) { \
  printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
  retval = -1;  goto bailout; \
}

#define THROW_TJ(action)  THROW(action, tjGetErrorStr2(tjInstance))

#define THROW_UNIX(action)  THROW(action, strerror(errno))

#define DEFAULT_SUBSAMP  TJSAMP_444
#define DEFAULT_QUALITY  95


const char* subsampName[TJ_NUMSAMP] = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

const char* colorspaceName[TJ_NUMCS] = {
  "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"
};

tjscalingfactor* scalingFactors = NULL;
int numScalingFactors = 0;


int encode() {

	std::cout << "Encode~~~~" << std::endl;

	int outSubsamp = -1, outQual = -1;
	tjtransform xform;
	int flags = 0;
	int width, height;
	char* inFormat, * outFormat;
	FILE* jpegFile = NULL;
	unsigned char* imgBuf = NULL, * jpegBuf = NULL;
	int retval = 0, i, pixelFormat = TJPF_UNKNOWN;
	tjhandle tjInstance = NULL;

	/* Input image is not a JPEG image.  Load it into memory. */
	if ((imgBuf = tjLoadImage("1920x1080.bgr", &width, 1, &height, &pixelFormat, 0)) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	outSubsamp = TJSAMP_444;

	printf("Output Image (jpg):  %d x %d pixels", width, height);

	/* Output image format is JPEG.  Compress the uncompressed image. */
	unsigned long jpegSize = 0;

	jpegBuf = NULL;  /* Dynamically allocate the JPEG buffer */

	if (outQual < 0)
		outQual = DEFAULT_QUALITY;
	printf(", %s subsampling, quality = %d\n", subsampName[outSubsamp],
		outQual);

	if ((tjInstance = tjInitCompress()) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	std::chrono::system_clock::time_point compress_start = std::chrono::system_clock::now();
	for (int i = 0; i < 100; i++) {
		if (tjCompress2(tjInstance, imgBuf, width, 0, height, pixelFormat,
			&jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0) {
			std::cout << "encode fail" << std::endl;

			break;
		}
	}
	std::chrono::system_clock::time_point compress_end = std::chrono::system_clock::now();

	std::cout << "time : "
		<< (std::chrono::duration_cast<std::chrono::milliseconds>(compress_end - compress_start).count() / 100)
		<< " ms" << std::endl;

	tjDestroy(tjInstance);  tjInstance = NULL;
	//fclose(jpegFile);  jpegFile = NULL;
	tjFree(jpegBuf);  jpegBuf = NULL;

	tjFree(imgBuf);
	if (tjInstance) tjDestroy(tjInstance);
	tjFree(jpegBuf);
	if (jpegFile) fclose(jpegFile);
	return retval;
}

int decode() {

	std::cout << std::endl;
	std::cout << "decode~~~~" << std::endl;

	int outSubsamp = -1;
	int flags = 0;
	int width, height;
	FILE* jpegFile = NULL;
	unsigned char* imgBuf = NULL, * jpegBuf = NULL;
	int retval = 0, i, pixelFormat = TJPF_UNKNOWN;
	tjhandle tjInstance = NULL;

	if ((scalingFactors = tjGetScalingFactors(&numScalingFactors)) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	outSubsamp = TJSAMP_444;

	/* Determine input and output image formats based on file extensions. */


	/* Input image is a JPEG image.  Decompress and/or transform it. */
	long size;
	int inSubsamp, inColorspace;
	unsigned long jpegSize;

	/* Read the JPEG file into memory. */
	if ((jpegFile = fopen("1920test.jpg", "rb")) == NULL)
	{
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) ||
		fseek(jpegFile, 0, SEEK_SET) < 0) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}
	if (size == 0) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}
	jpegSize = (unsigned long)size;
	if ((jpegBuf = (unsigned char*)tjAlloc(jpegSize)) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}
	if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}
	fclose(jpegFile);  jpegFile = NULL;

	if ((tjInstance = tjInitDecompress()) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height,
		&inSubsamp, &inColorspace) < 0) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	printf("Input Image:  %d x %d pixels, %s subsampling, %s colorspace\n", width, height,
		subsampName[inSubsamp], colorspaceName[inColorspace]);

	pixelFormat = TJPF_BGRX;
	if ((imgBuf = (unsigned char*)tjAlloc(width * height * tjPixelSize[pixelFormat])) == NULL) {
		tjFree(imgBuf);
		if (tjInstance) tjDestroy(tjInstance);
		tjFree(jpegBuf);
		if (jpegFile) fclose(jpegFile);
		return retval;
	}

	std::chrono::system_clock::time_point compress_start = std::chrono::system_clock::now();

	for (int i = 0; i < 100; i++) {
		if (tjDecompress2(tjInstance, jpegBuf, jpegSize, imgBuf, width, 0, height, pixelFormat, flags) < 0) {
			std::cout << "decode fail" << std::endl;
			break;
		}
	}

	std::chrono::system_clock::time_point compress_end = std::chrono::system_clock::now();
	std::cout << "time : "
		<< (std::chrono::duration_cast<std::chrono::milliseconds>(compress_end - compress_start).count() / 100)
		<< " ms" << std::endl;

	tjFree(jpegBuf);  jpegBuf = NULL;
	tjDestroy(tjInstance);  tjInstance = NULL;

	printf("Output Image (jpg):  %d x %d pixels", width, height);

	tjFree(imgBuf);
	if (tjInstance) tjDestroy(tjInstance);
	tjFree(jpegBuf);
	if (jpegFile) fclose(jpegFile);
	return retval;
}

int main(int argc, char** argv)
{

	encode();
	decode();

	return 0;
}