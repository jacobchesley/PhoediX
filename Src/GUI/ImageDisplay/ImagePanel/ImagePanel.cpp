#include "ImagePanel.h"

ImagePanel::ImagePanel(wxWindow * parent) : wxPanel(parent) {

	zoom = 1.0;
	keepAspect = true;
	resize = false;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&ImagePanel::OnSize, this);

	this->SetDoubleBuffered(true);
}

ImagePanel::ImagePanel(wxWindow * parent, Image * image) : wxPanel(parent) {

	img = image;
	zoom = 1.0;
	keepAspect = true;
	resize = false;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&ImagePanel::OnSize, this);

	this->SetDoubleBuffered(true);
}


void ImagePanel::Render(wxDC & dc) {

	// Get current width and height
	int newWidth = this->GetSize().GetWidth();
	int newHeight = this->GetSize().GetHeight();

	OutputDebugStringA("Draw Width - ");
	OutputDebugStringA(std::to_string(newWidth).c_str());
	OutputDebugStringA("Draw Height - ");
	OutputDebugStringA(std::to_string(newHeight).c_str());

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