#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include "Debugging\MemoryLeakCheck.h"

class Image {
public:
	Image();
	Image(const Image& imageToCopy);
	~Image();

	void Destroy();

	void Enable16Bit();
	void Disable16Bit();

	int GetColorDepth();

	void SetDataFrom8(uint8_t * inData, int inWidth, int inHeight);
	void SetDataFrom16(uint16_t * inData, int inWidth, int inHeight);

	int GetWidth();
	int GetHeight();

	void SetWidth(int newWidth);
	void SetHeight(int newHeight);
	void InitImage();

	uint8_t * Get8BitDataRed();
	uint8_t * Get8BitDataGreen();
	uint8_t * Get8BitDataBlue();

	uint16_t * Get16BitDataRed();
	uint16_t * Get16BitDataGreen();
	uint16_t * Get16BitDataBlue();

private:
	
	bool bit16Enabled;

	int width;
	int height;

	uint8_t * imageDataRed8;
	uint8_t * imageDataGreen8;
	uint8_t * imageDataBlue8;

	uint16_t * imageDataRed16;
	uint16_t * imageDataGreen16;
	uint16_t * imageDataBlue16;

};

#endif
