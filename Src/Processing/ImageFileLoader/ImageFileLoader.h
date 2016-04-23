#ifndef IMAGE_FILE_LOADER_H
#define IMAGE_FILE_LOADER_H
	
// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Image\Image.h"

class ImageFileLoader {
public:
	static void LoadImageFromFile(wxString fileName, Image * image);
};
#endif
