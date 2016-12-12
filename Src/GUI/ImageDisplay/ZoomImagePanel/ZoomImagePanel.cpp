// Copyright 2016 Jacob Chesley

#include "ZoomImagePanel.h"

ZoomImagePanel::ZoomImagePanel(wxWindow * parent) : wxPanel(parent) {
	par = parent;
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);
	scroller = new ImageScroll(this, new wxImage(0,0));
	reguardScrollCountdown= new wxTimer(this);
	this->InitControls();
}

ZoomImagePanel::ZoomImagePanel(wxWindow * parent, wxImage * img) : wxPanel(parent) {
	par = parent;
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);
	scroller = new ImageScroll(this, img);
	reguardScrollCountdown= new wxTimer(this);
	this->InitControls();	
}

ZoomImagePanel::ZoomImagePanel(wxWindow * parent, Image * img) : wxPanel(parent) {

	par = parent;
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);
	scroller = new ImageScroll(this, img);
	reguardScrollCountdown= new wxTimer(this);
	this->InitControls();
}

void ZoomImagePanel::InitControls(){

	this->SetDoubleBuffered(true);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	controlSizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(mainSizer);

	zoomSlider = new DoubleSlider(this, 100.0, 1.0, 400.0, 100000, 0);
	zoomSlider->SetForegroundColour(Colors::TextLightGrey);
	zoomSlider->SetBackgroundColour(Colors::BackDarkDarkGrey);
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
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&ZoomImagePanel::OnReguardScrollTimer, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ZoomImagePanel::OnScrollRightDown, this);
}

void ZoomImagePanel::DestroyTimer(){

	reguardScrollCountdown->Stop();
	delete reguardScrollCountdown;
}

void ZoomImagePanel::ChangeImage(Image * newImage) {
	scroller->ChangeImage(newImage);
}

void ZoomImagePanel::ChangeImage(wxImage * newImage) {
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
	this->FitImage();
}

void ZoomImagePanel::OnScrollRightDown(wxMouseEvent& evt) {
	wxMouseEvent newEvent(evt);
	wxPostEvent(this->GetParent(), newEvent);
}

void ZoomImagePanel::FitImage() {
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

	reguardScrollCountdown->Start(500, true);
}

void ZoomImagePanel::OnReguardScrollTimer(wxTimerEvent& WXUNUSED(evt)){
	scroller->ReguardScroll();
}

void ZoomImagePanel::SetTempSize(int tempWidth, int tempHeight){
	scroller->SetVirtualSize(tempWidth, tempHeight);
}

void ZoomImagePanel::Redraw() {
	scroller->Redraw();
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	currentlyDrawing = false;
	disreguardScroll = false;
	zoom = 1.0;
	keepAspect = true;
	resize = false;
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, Image * image) : wxScrolledWindow(parent) {
	
	currentlyDrawing = false;
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
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	this->SetDoubleBuffered(true);

}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, wxImage * image) : wxScrolledWindow(parent) {
	
	currentlyDrawing = false;
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
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	this->SetDoubleBuffered(true);
}

void ZoomImagePanel::ImageScroll::Render(wxDC& dc) {
	
	// Prepare the DC, zoom the correct amount and draw the bitmap
	this->PrepareDC(dc);

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

	dc.SetUserScale(zoom, zoom);
	dc.DrawBitmap(bitmapDraw, wxPoint(0, 0));
}

void ZoomImagePanel::ImageScroll::Redraw() {
	
	if (currentlyDrawing) {
		return;
	}
	currentlyDrawing = true;
	// Render a buffered DC from the client DC
	wxClientDC dc(this);
	wxBufferedDC dcBuffer(&dc);
	this->Render(dcBuffer);
	currentlyDrawing = false;
}

void ZoomImagePanel::ImageScroll::ChangeImage(Image * newImage) {
	
	if (newImage == NULL) { return; }
	// Verify new image is okay
	if (newImage->GetWidth() > 0 && newImage->GetHeight() > 0) {

		int numPixels = newImage->GetWidth() * newImage->GetHeight();

		// Copy image data (8 bit)
		unsigned char * imageData = (unsigned char *)malloc(sizeof(unsigned char) * numPixels * 3);

		if(imageData != NULL){
			ImageHandler::CopyImageData8(newImage, imageData);
			wxImage tempImage = wxImage(newImage->GetWidth(), newImage->GetHeight(), imageData);
			if(!tempImage.IsOk()){
				bitmapDraw = wxBitmap(wxImage(1, 1));
			}
			bitmapDraw = wxBitmap(tempImage);
			if(!bitmapDraw.IsOk()){
				bitmapDraw = wxBitmap(wxImage(1, 1));
			}
		}
		else{
			bitmapDraw = wxBitmap(wxImage(1, 1));
		}
	}
}

void ZoomImagePanel::ImageScroll::ChangeImage(wxImage * newImage) {

	if (newImage->IsOk()) {
		bitmapDraw = wxBitmap(*newImage);
	}
	else {
		bitmapDraw = wxBitmap(1, 1);
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
	
	if(disreguardScroll){
		this->ReguardScroll();
	}
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

	if (currentlyDrawing) {
		return;
	}
	currentlyDrawing = true;
	// Create buffered and dc and render
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	evt.Skip();
	currentlyDrawing = false;
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
	disreguardScroll = true;
	this->Unbind(wxEVT_MOTION, (wxObjectEventFunction)&ImageScroll::OnDragContinue, this);
}

void ZoomImagePanel::ImageScroll::ReguardScroll(){
	this->Bind(wxEVT_MOTION, (wxObjectEventFunction)&ImageScroll::OnDragContinue, this);
	disreguardScroll = false;
}

void ZoomImagePanel::ImageScroll::OnRightDown(wxMouseEvent & evt) {

	wxMouseEvent newEvent(evt);
	wxCoord newX;
	wxCoord newY;

	this->CalcUnscrolledPosition(evt.GetX(), evt.GetY(), &newX, &newY);
	newEvent.SetX(newX / zoom);
	newEvent.SetY(newY / zoom);

	wxPostEvent(this->GetParent(), newEvent);
}