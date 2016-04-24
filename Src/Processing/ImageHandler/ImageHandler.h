#ifndef IMAGE_FILE_LOADER_H
#define IMAGE_FILE_LOADER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Image\Image.h"

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

class ImageHandler {
public:
	static void LoadImageFromFile(wxString fileName, Image * image);
	static void SaveImageToFile(wxString fileName, Image * image);
	static void CopyImageData8(Image * image, unsigned char * outArray);
};
#endif
