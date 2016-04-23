#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

class Image {
public:
	Image();

	void Enable16Bit();
	void Disable16Bit();

	int GetColorDepth();

	void SetDataFrom8(int8_t * inData, int inWidth, int inHeight);
	void SetDataFrom16(int16_t * inData, int inWidth, int inHeight);

	int GetWidth();
	int GetHeight();

	int8_t * Get8BitDataRed();
	int8_t * Get8BitDataGreen();
	int8_t * Get8BitDataBlue();

	int16_t * Get16BitDataRed();
	int16_t * Get16BitDataGreen();
	int16_t * Get16BitDataBlue();

private:
	
	bool bit16Enabled;

	int width;
	int height;

	int8_t * imageDataRed8;
	int8_t * imageDataGreen8;
	int8_t * imageDataBlue8;

	int16_t * imageDataRed16;
	int16_t * imageDataGreen16;
	int16_t * imageDataBlue16;

};

#endif
