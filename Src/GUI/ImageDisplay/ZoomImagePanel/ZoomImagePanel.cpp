#include "ZoomImagePanel.h"

ZoomImagePanel::ZoomImagePanel(wxWindow * parent) : wxPanel(parent) {

	this->SetBackgroundColour(Colors::BackDarkGrey);
	this->SetDoubleBuffered(true);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	scroller = new ImageScroll(this, NULL);

	this->GetSizer()->Add(scroller, 1, wxGROW);

	zoomSlider = new DoubleSlider(this, 100.0, 1.0, 400.0, 100000, 0);
	zoomSlider->SetForegroundColour(Colors::TextLightGrey);
	zoomSlider->SetBackgroundColour(parent->GetBackgroundColour());
	zoomSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	this->GetSizer()->Add(zoomSlider);

	this->Bind(wxEVT_SLIDER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);

}

ZoomImagePanel::ZoomImagePanel(wxWindow * parent, Image * img) : wxPanel(parent) {

	this->SetBackgroundColour(Colors::BackDarkGrey);
	this->SetDoubleBuffered(true);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);
	
	scroller = new ImageScroll(this, img);

	this->GetSizer()->Add(scroller, 1, wxGROW);

	zoomSlider = new DoubleSlider(this, 100.0, 1.0, 400.0, 100000, 0);
	zoomSlider->SetForegroundColour(Colors::TextLightGrey);
	zoomSlider->SetBackgroundColour(parent->GetBackgroundColour());
	zoomSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	this->GetSizer()->Add(zoomSlider);

	this->Bind(wxEVT_SLIDER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
}

void ZoomImagePanel::ChangeImage(Image * newImage) {
	scroller->ChangeImage(newImage);
}

void ZoomImagePanel::OnZoom(wxCommandEvent& slideEvent) {
	scroller->SetZoom(zoomSlider->GetValue()/100.0);
	slideEvent.Skip(false);
}

void ZoomImagePanel::Redraw() {
	scroller->Redraw();
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent) : wxScrolledWindow(parent) {

}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, Image * image) : wxScrolledWindow(parent) {
	
	this->ChangeImage(image);
	zoom = 1.0;
	keepAspect = true;
	resize = false;

	SetScrollbars(1, 1, image->GetWidth(), image->GetHeight());
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->SetDoubleBuffered(true);
}

void ZoomImagePanel::ImageScroll::Render(wxDC& dc) {
	
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));

	if (!bitmapDraw.IsOk()) {
		return; 
	}

	int imgWidth = bitmapDraw.GetWidth() * zoom;
	int imgHeight = bitmapDraw.GetHeight() * zoom;
	this->SetVirtualSize(imgWidth, imgHeight);

	this->PrepareDC(dc);
	dc.SetUserScale(zoom, zoom);
	dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
}

void ZoomImagePanel::ImageScroll::Redraw() {
	wxClientDC dc(this);
	wxBufferedDC dcBuffer(&dc);
	this->Render(dcBuffer);
}

void ZoomImagePanel::ImageScroll::ChangeImage(Image * newImage) {

	if (newImage->GetWidth() > 0 && newImage->GetHeight() > 0) {
		unsigned char * imageData = (unsigned char *)malloc(sizeof(unsigned char) * newImage->GetWidth() * newImage->GetHeight() * 3);
		ImageHandler::CopyImageData8(newImage, imageData);
		wxImage tempImage = wxImage(newImage->GetWidth(), newImage->GetHeight(), imageData);
		bitmapDraw = wxBitmap(tempImage);
	}
}
void ZoomImagePanel::ImageScroll::OnPaint(wxPaintEvent& event) {
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	event.Skip();
}

void ZoomImagePanel::ImageScroll::SetZoom(double zoomFactor) {
	zoom = zoomFactor;
	this->Redraw();
}