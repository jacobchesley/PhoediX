// Copyright 2016 Jacob Chesley

#ifndef PIXEL_PEEP_WINDOW_H
#define PIXEL_PEEP_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Image\Image.h"

class PixelPeepWindow : public wxScrolledWindow{

public:
	 PixelPeepWindow (wxWindow * parent);
	 void UpdateImage(Image * newImage);
	 void UpdatePosition(int x, int y);
	 void UpdateGridSize(int width, int height);

private:
	void GeneratePixelGrid();

};

#endif
