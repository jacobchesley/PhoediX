// Copyright 2016 Jacob Chesley

#include "wxImagePanel.h"

WXImagePanel::WXImagePanel(wxWindow * parent, bool doKeepAspect) : wxPanel(parent) {

	currentlyDrawing = false;
	zoom = 1.0;
	keepAspect = doKeepAspect;
	resize = false;
	staticImage = false;
	bitmapDraw = wxBitmap(1, 1); 

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&WXImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&WXImagePanel::OnSize, this);

	this->SetDoubleBuffered(true);
	doDraw = true;
}

WXImagePanel::WXImagePanel(wxWindow * parent, wxImage * image, bool doKeepAspect, bool staticImg) : wxPanel(parent) {

	currentlyDrawing = false;
	img = image;
	zoom = 1.0;
	keepAspect = doKeepAspect;
	staticImage = staticImg;
	resize = false;

	this->ChangeImage(image);

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&WXImagePanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&WXImagePanel::OnSize, this);

	this->SetDoubleBuffered(true);
	this->SetSize(img->GetSize());
	this->SetMinSize(img->GetSize());
	doDraw = true;
}

void WXImagePanel::SetKeepAspect(bool doKeepAspect) {
	keepAspect = doKeepAspect;
}

void WXImagePanel::SetResize(bool doResize) {
	resize = doResize;
}

void WXImagePanel::ChangeImage(wxImage * newImage) {
	if(newImage == NULL){ 

		bitmapDraw = wxBitmap(1, 1); 
		return;
	}
	if (staticImage) {

		if(newImage->GetWidth() > 0 || newImage->GetHeight() > 0){
			bitmapDraw = wxBitmap(*newImage);
			this->SetSize(newImage->GetSize());
			this->SetMinSize(newImage->GetSize());
		}
		else{
			bitmapDraw = wxBitmap(1, 1);
		}
	}
	img = newImage;
	this->SetSize(img->GetSize());
	this->SetMinSize(img->GetSize());
}

void WXImagePanel::Render(wxDC & dc) {

	this->PrepareDC(dc);

	if (!doDraw) { return;  }
	if (staticImage) {
		dc.Clear();
		if(bitmapDraw.IsOk()){
			dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
		}
		return;
	}

	if(img == NULL){ return; }
	if(img->GetWidth() < 1 || img->GetHeight() < 1){ return; }

	// Get current width and height
	int newWidth = this->GetSize().GetWidth();
	int newHeight = this->GetSize().GetHeight();


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


	// Convert wxImage to wxBitmap, with scaled width and height
	if (img->IsOk()) {
		if (resize) {
			bitmapDraw = wxBitmap(img->Scale(newWidth * zoom, newHeight * zoom, wxIMAGE_QUALITY_HIGH));
		}
		else {
			bitmapDraw = wxBitmap(*img);
		}
	}
	
	dc.Clear();
	dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
}


void WXImagePanel::Redraw() {

	if (currentlyDrawing) {
		return;
	}
	currentlyDrawing = true;
	wxClientDC dc(this);
	wxBufferedDC dcBuffer(&dc);
	this->Render(dcBuffer);
	currentlyDrawing = false;
}

void WXImagePanel::StopDrawing() {
	doDraw = false;
}

void WXImagePanel::OnPaint(wxPaintEvent& event) {

	if (currentlyDrawing) {
		return;
	}
	currentlyDrawing = true;
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	event.Skip();
	currentlyDrawing = false;
}

void WXImagePanel::OnSize(wxSizeEvent& event) {
	if (staticImage) {
		this->SetSize(bitmapDraw.GetWidth(), bitmapDraw.GetHeight());
		return;
	}
	this->SetSize(img->GetWidth(), img->GetHeight());
	this->Redraw();
	event.Skip();
}

void WXImagePanel::SetZoom(double zoomFactor) {
	zoom = zoomFactor;
}
