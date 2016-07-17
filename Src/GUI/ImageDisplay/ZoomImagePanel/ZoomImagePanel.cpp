#include "ZoomImagePanel.h"

ZoomImagePanel::ZoomImagePanel(wxWindow * parent) : wxPanel(parent) {

	this->SetBackgroundColour(Colors::BackDarkGrey);
	this->SetDoubleBuffered(true);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	controlSizer = new wxBoxSizer(wxHORIZONTAL);

	this->SetSizer(mainSizer);

	scroller = new ImageScroll(this, NULL);
	
	zoomSlider = new DoubleSlider(this, 100.0, 1.0, 400.0, 100000, 0);
	zoomSlider->SetForegroundColour(Colors::TextLightGrey);
	zoomSlider->SetBackgroundColour(parent->GetBackgroundColour());
	zoomSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	fullImageView = new wxButton(this, ZoomImagePanel::Buttons::ZOOM_FIT, "Fit Image", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	fullImageView->SetBackgroundColour(Colors::BackDarkGrey);
	fullImageView->SetForegroundColour(Colors::TextLightGrey);

	viewImage100 = new wxButton(this, ZoomImagePanel::Buttons::ZOOM_100, "100% Zoom", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	viewImage100->SetBackgroundColour(Colors::BackDarkGrey);
	viewImage100->SetForegroundColour(Colors::TextLightGrey);

	controlSizer->Add(zoomSlider);
	controlSizer->Add(fullImageView);
	controlSizer->Add(viewImage100);
	
	this->GetSizer()->Add(scroller, 1, wxGROW);
	this->GetSizer()->Add(controlSizer);

	this->Bind(wxEVT_SLIDER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ZoomImagePanel::OnFitImage, this, ZoomImagePanel::Buttons::ZOOM_FIT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ZoomImagePanel::OnZoom100, this, ZoomImagePanel::Buttons::ZOOM_100);
}

ZoomImagePanel::ZoomImagePanel(wxWindow * parent, Image * img) : wxPanel(parent) {

	this->SetBackgroundColour(Colors::BackDarkGrey);
	this->SetDoubleBuffered(true);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	controlSizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(mainSizer);
	
	scroller = new ImageScroll(this, img);

	zoomSlider = new DoubleSlider(this, 100.0, 1.0, 400.0, 100000, 0);
	zoomSlider->SetForegroundColour(Colors::TextLightGrey);
	zoomSlider->SetBackgroundColour(parent->GetBackgroundColour());
	zoomSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	fullImageView = new wxButton(this, ZoomImagePanel::Buttons::ZOOM_FIT, "Fit Image", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	fullImageView->SetBackgroundColour(Colors::BackDarkDarkGrey);
	fullImageView->SetForegroundColour(Colors::TextLightGrey);

	viewImage100 = new wxButton(this, ZoomImagePanel::Buttons::ZOOM_100, "100% Zoom", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	viewImage100->SetBackgroundColour(Colors::BackDarkDarkGrey);
	viewImage100->SetForegroundColour(Colors::TextLightGrey);

	controlSizer->AddSpacer(10);
	controlSizer->Add(zoomSlider);
	controlSizer->AddSpacer(15);
	controlSizer->Add(fullImageView);
	controlSizer->AddSpacer(15);
	controlSizer->Add(viewImage100);

	this->GetSizer()->Add(scroller, 1, wxGROW);
	this->GetSizer()->Add(controlSizer);

	this->Bind(wxEVT_SLIDER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ZoomImagePanel::OnZoom, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ZoomImagePanel::OnFitImage, this, ZoomImagePanel::Buttons::ZOOM_FIT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ZoomImagePanel::OnZoom100, this, ZoomImagePanel::Buttons::ZOOM_100);
}

void ZoomImagePanel::ChangeImage(Image * newImage) {
	scroller->ChangeImage(newImage);
}

void ZoomImagePanel::OnZoom(wxCommandEvent& slideEvent) {
	scroller->SetZoom(zoomSlider->GetValue()/100.0);
	slideEvent.Skip(false);
}

void ZoomImagePanel::OnZoom100(wxCommandEvent& WXUNUSED(event)) {
	scroller->SetZoom(1.0);
	zoomSlider->SetValue(100.0);
}

void ZoomImagePanel::OnFitImage(wxCommandEvent& WXUNUSED(event)) {
	scroller->FitImage();
	scroller->FitImage();
	zoomSlider->SetValue(scroller->GetZoom()*100.0);
}

void ZoomImagePanel::SetZoom(float zoom) {
	scroller->SetZoom(zoom / 100.0);
	zoomSlider->SetValue(zoom);
}

float ZoomImagePanel::GetZoom() {
	return scroller->GetZoom() * 100.0f;
}

int ZoomImagePanel::GetDragX() {
	int x = 0;
	int y = 0;
	scroller->GetViewStart(&x, &y);
	return x;
}

int ZoomImagePanel::GetDragY() {
	int x = 0;
	int y = 0;
	scroller->GetViewStart(&x, &y);
	return y;
}

void ZoomImagePanel::SetDrag(int x, int y) {

	scroller->DisreguardScroll();
	scroller->Scroll(x, y);
	scroller->ReguardScroll();
}

void ZoomImagePanel::Redraw() {
	scroller->Redraw();
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent) : wxScrolledWindow(parent) {

}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, Image * image) : wxScrolledWindow(parent) {
	
	disreguardScroll = false;
	this->ChangeImage(image);
	zoom = 1.0;
	keepAspect = true;
	resize = false;

	SetScrollbars(1, 1, image->GetWidth(), image->GetHeight());
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_MOTION, (wxObjectEventFunction)&ImageScroll::OnDragContinue, this);
	this->SetDoubleBuffered(true);
}

void ZoomImagePanel::ImageScroll::Render(wxDC& dc) {
	
	// Clear and set background color
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));

	// Do not attempt to draw bitmap if it is not okay
	if (!bitmapDraw.IsOk()) {
		return; 
	}

	// Calculate new width and height based on zoom
	int imgWidth = bitmapDraw.GetWidth() * zoom;
	int imgHeight = bitmapDraw.GetHeight() * zoom;
	this->SetVirtualSize(imgWidth, imgHeight);

	// Prepare the DC, zoom the correct amount and draw the bitmap
	this->PrepareDC(dc);
	dc.SetUserScale(zoom, zoom);
	dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
}

void ZoomImagePanel::ImageScroll::Redraw() {

	// Render a buffered DC from the client DC
	wxClientDC dc(this);
	wxBufferedDC dcBuffer(&dc);
	this->Render(dcBuffer);
}

void ZoomImagePanel::ImageScroll::ChangeImage(Image * newImage) {

	// Verify new image is okay
	if (newImage->GetWidth() > 0 && newImage->GetHeight() > 0) {

		// Copy image data (8 bit)
		unsigned char * imageData = (unsigned char *)malloc(sizeof(unsigned char) * newImage->GetWidth() * newImage->GetHeight() * 3);
		ImageHandler::CopyImageData8(newImage, imageData);

		// Create a new wxImage and wxBitmap from it for rendering
		wxImage tempImage = wxImage(newImage->GetWidth(), newImage->GetHeight(), imageData);
		bitmapDraw = wxBitmap(tempImage);
	}
}

void ZoomImagePanel::ImageScroll::OnDragStart(wxMouseEvent& evt) {

	// First left click before drag
	dragStartX = evt.GetPosition().x;
	dragStartY = evt.GetPosition().y;

	// Get scroll bar original position
	this->GetViewStart(&scrollStartX, &scrollStartY);
}

void ZoomImagePanel::ImageScroll::OnDragContinue(wxMouseEvent& evt) {
	
	// Verify this is a drag event, with left mouse button down
	if (evt.Dragging()) {

		// Get mouse dx and dy in pixels
		int dx = evt.GetPosition().x - dragStartX;
		int dy = evt.GetPosition().y - dragStartY;

		// Get scroll rate to convert pixels to scroll units
		int scrollScaleX = 1;
		int scrollScaleY = 1;
		this->GetScrollPixelsPerUnit(&scrollScaleX, &scrollScaleY);

		// Convert pixels to scroll units and add to start position.  Negate dx and dy to scroll in direction of mouse drag
		int newScrollPosX = (-dx / scrollScaleX) + scrollStartX;
		int newScrollPosY = (-dy / scrollScaleY) + scrollStartY;

		// Scroll to new calculated position
		if(!disreguardScroll){
			this->Scroll(newScrollPosX, newScrollPosY);
		}
	}
}

void ZoomImagePanel::ImageScroll::OnPaint(wxPaintEvent& evt) {

	// Create buffered and dc and render
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	evt.Skip();
}

void ZoomImagePanel::ImageScroll::SetZoom(double zoomFactor) {
	zoom = zoomFactor;
	this->Redraw();
}

double ZoomImagePanel::ImageScroll::GetZoom() {
	return zoom;
}

void ZoomImagePanel::ImageScroll::FitImage() {

	int width = this->GetClientSize().GetWidth();
	int height = this->GetClientSize().GetHeight();
	
	int imageWidth = bitmapDraw.GetWidth();
	int imageHeight = bitmapDraw.GetHeight();

	double zoomWidth = (double)width / (double)imageWidth;
	double zoomHeight = (double)height / (double)imageHeight;

	if (zoomWidth <= zoomHeight) {
		this->SetZoom(zoomWidth);
	}
	else {
		this->SetZoom(zoomHeight);
	}
}

void ZoomImagePanel::ImageScroll::DisreguardScroll(){
	this->Unbind(wxEVT_MOTION, (wxObjectEventFunction)&ImageScroll::OnDragContinue, this);
	disreguardScroll = true;
}

void ZoomImagePanel::ImageScroll::ReguardScroll(){
	this->Bind(wxEVT_MOTION, (wxObjectEventFunction)&ImageScroll::OnDragContinue, this);
	disreguardScroll = false;
}