#ifndef IMAGE_FILE_LOADER_H
#define IMAGE_FILE_LOADER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Image\Image.h"
#include "Debugging\MemoryLeakCheck.h"

class ImageHandler {
public:
	static void LoadImageFromFile(wxString fileName, Image * image);
	static void SaveImageToFile(wxString fileName, Image * image);
	static void LoadImageFromwxImage(wxImage* inImage, Image * image);
	static void CopyImageData8(Image * image, uint8_t * outArray);
	static void CopyImageData16(Image * image, uint16_t * outArray);
};
#endif
