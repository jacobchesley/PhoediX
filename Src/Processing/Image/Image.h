// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef IMAGE_H
#define IMAGE_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <stdint.h>
#include "Debugging/MemoryLeakCheck.h"
#include <math.h>
 #include <iostream>  
#include <map>

struct UnsignedRational {
	unsigned long numerator;
	unsigned long denominator;
};

struct SignedRational {
	long numerator;
	long denominator;
};

class Image {

public:

	Image();
	Image(const Image& imageToCopy);
	~Image();

	static void InitExif();
	static std::map<size_t, wxString> exifTags;
	static std::map<size_t, int> exifFormats;
	static bool exifIsGPSCoordinate(size_t tag);

	enum ExifType{
		U_BYTE = 1,
		ASCII = 2,
		U_SHORT = 3,
		U_LONG = 4,
		U_RATIONAL = 5,
		S_BYTE = 6,
		UNDEFINED = 7,
		S_SHORT = 8,
		S_LONG = 9,
		S_RATIONAL = 10,
		FLOAT = 11,
		DOUBLE = 12
	};

	void Destroy();

	void Enable16Bit();
	void Disable16Bit();

	int GetColorDepth();

	void SetDataFrom8(uint8_t * inData, int inWidth, int inHeight);
	void SetDataFrom16(uint16_t * inData, int inWidth, int inHeight);
	void SetDataFrom16(uint8_t * inData, int inWidth, int inHeight);

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

	wxString GetErrorStr();
	std::map<size_t, void*> * GetExifMap();

private:

	bool bit16Enabled;

	uint32_t width;
	uint32_t height;

	uint8_t * imageDataRed8;
	uint8_t * imageDataGreen8;
	uint8_t * imageDataBlue8;

	uint16_t * imageDataRed16;
	uint16_t * imageDataGreen16;
	uint16_t * imageDataBlue16;

	wxString errorMessage;

	std::map<size_t, void*> exifMap;
};

#endif
