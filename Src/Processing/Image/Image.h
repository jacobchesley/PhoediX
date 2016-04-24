#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

#ifdef CHECK_MEMORY_LEAK
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#endif

class Image {
public:
	Image();
	~Image();

	void Destroy();

	void Enable16Bit();
	void Disable16Bit();

	int GetColorDepth();

	void SetDataFrom8(unsigned char * inData, int inWidth, int inHeight);
	void SetDataFrom16(int16_t * inData, int inWidth, int inHeight);

	int GetWidth();
	int GetHeight();

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
