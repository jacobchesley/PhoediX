#include "ImageFileLoader.h"

void ImageFileLoader::LoadImageFromFile(wxString fileName, Image * image) {

	wxImage fileImage(fileName);
	image->SetDataFrom8((int8_t*)fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
}