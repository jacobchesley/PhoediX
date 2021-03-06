// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "ImagePanel.h"

ImagePanel::ImagePanel(wxWindow * parent, bool doKeepAspect) : wxPanel(parent) {

	zoom = 1.0;
	keepAspect = doKeepAspect;
	resize = false;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&ImagePanel::OnSize, this);

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

ImagePanel::ImagePanel(wxWindow * parent, Image * image, bool doKeepAspect) : wxPanel(parent) {

	img = image;
	zoom = 1.0;
	keepAspect = doKeepAspect;
	resize = false;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&ImagePanel::OnSize, this);

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

void ImagePanel::SetKeepAspect(bool doKeepAspect) {
	keepAspect = doKeepAspect;
}

void ImagePanel::ChangeImage(Image * newImage) {
	img = newImage;
}

void ImagePanel::Render(wxDC & dc) {

	// Get current width and height
	int newWidth = this->GetSize().GetWidth();
	int newHeight = this->GetSize().GetHeight();

	if (oldHeight != newHeight || oldWidth != newWidth) {

		if (keepAspect) {
			// Get the aspect ratio of the image, and the current panel
			double imgAspectRatio = (double)img->GetWidth() / (double)img->GetHeight();
			double thisAspecRatio = (double)newWidth / (double)newHeight;

			// Resize width based on height
			if (thisAspecRatio > imgAspectRatio) {
				newWidth = newHeight  * imgAspectRatio;
			}

			// Resize height based on width
			else if (thisAspecRatio < imgAspectRatio) {
				newHeight = newWidth / imgAspectRatio;
			}
		}

		// Create wxImage from Image
		unsigned char * imageData = (unsigned char *)malloc(sizeof(unsigned char) * img->GetWidth() * img->GetHeight() * 3);
		ImageHandler::CopyImageData8(img, imageData);
		wxImage tempImage = wxImage(img->GetWidth(), img->GetHeight(), imageData);
		
		// Convert wxImage to wxBitmap, with scaled width and height
		if (tempImage.IsOk()) {
			if (resize) {
				bitmapDraw = wxBitmap(tempImage.Scale(newWidth * zoom, newHeight * zoom, wxIMAGE_QUALITY_HIGH));
			}
			else {
				bitmapDraw = wxBitmap(tempImage);
			}
		}

		oldWidth = newWidth;
		oldHeight = newHeight;
	}

	dc.Clear();
	dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
}

void ImagePanel::Redraw(){
	wxClientDC dc(this);
	wxBufferedDC dcBuffer(&dc);
	this->Render(dcBuffer);
}

void ImagePanel::OnPaint(wxPaintEvent& event) {
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	event.Skip();
}

void ImagePanel::OnSize(wxSizeEvent& event) {
	this->SetSize(img->GetWidth(), img->GetHeight());
	Refresh();
	event.Skip();
}

void ImagePanel::SetZoom(double zoomFactor) {
	zoom = zoomFactor;
}