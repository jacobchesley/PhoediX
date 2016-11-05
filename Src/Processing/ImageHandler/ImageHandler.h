// Copyright 2016 Jacob Chesley

#ifndef IMAGE_FILE_LOADER_H
#define IMAGE_FILE_LOADER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/filename.h"

#include "Processing\Image\Image.h"
#include "Debugging\MemoryLeakCheck.h"
#include "libraw.h"
#include "tiffconf.vc.h"
#include "tiffio.h"

class ImageHandler {

public:

	enum SaveType{
		JPEG,
		PNG,
		BMP,
		TIFF8,
		TIFF16
	};

	static void LoadImageFromFile(wxString fileName, Image * image);
	static void SaveImageToFile(wxString fileName, Image * image, int type, int quality);
	static void LoadImageFromwxImage(wxImage* inImage, Image * image);
	static void CopyImageData8(Image * image, uint8_t * outArray);
	static void CopyImageData16(Image * image, uint16_t * outArray);
	static void CopyImageFromRaw(libraw_processed_image_t * rawImage, Image * outImage);
	static void CopyImageFromRaw(libraw_processed_image_t * rawImage, wxImage * outImage);

	static bool CheckRaw(wxString fileName);
	static bool CheckImage(wxString fileName);

	static wxString imageOpenDialogList;
private:

	static wxString jpegFileDialogList;
	static wxString pngFileDialogList;
	static wxString bmpFileDialogList;
	static wxString tiffFileDialogList;

	static wxString allFileDialogList;

	static int GetTiffBitDepth(wxString fileName);
	static void Read16BitTiff(wxString fileName, Image * image);
	static void Write16BitTiff(wxString fileName, Image * image);
};
#endif
