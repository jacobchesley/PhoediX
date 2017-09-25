// Copyright 2016 Jacob Chesley

#include "ZoomImagePanel.h"

wxDEFINE_EVENT(GRID_MOVE_EVENT, wxCommandEvent);

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

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif

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

	if(scroller->GetFitImage()){ this->SetFitImage(false); }
	else{ this->SetFitImage(true); }
}

void ZoomImagePanel::OnScrollRightDown(wxMouseEvent& evt) {
	wxMouseEvent newEvent(evt);
	wxPostEvent(this->GetParent(), newEvent);
}

void ZoomImagePanel::FitImage() {

	scroller->FitImage();
	zoomSlider->SetValue(scroller->GetZoom() * 100.0);
}

bool ZoomImagePanel::GetFitImage(){
	return scroller->GetFitImage();
}

void ZoomImagePanel::SetFitImage(bool fitImage){

	if(fitImage){
		// Enable fitting image and hide controls (can't zoom if image is always fit)
		lastDragX = this->GetDragX();
		lastDragY = this->GetDragY();
		scroller->EnableFitImage();
		fullImageView->SetBackgroundColour(Colors::BackGrey);
		scroller->FitImage();
		zoomSlider->Hide();
		viewImage100->Hide();
		this->Layout();
	}
	else{
		// Disable fitting image and reshow all controls
		scroller->DisableFitImage();
		fullImageView->SetBackgroundColour(Colors::BackDarkDarkGrey);
		zoomSlider->Show();
		viewImage100->Show();
		scroller->SetZoom(zoomSlider->GetValue()/100.0);
		this->SetDrag(lastDragX, lastDragY);
		this->Layout();
	}
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

void ZoomImagePanel::ActivateGrid() {
	scroller->ActivateGrid();
}

void ZoomImagePanel::DeactivateGrid() {
	scroller->DeactivateGrid();
}

bool ZoomImagePanel::GetGridActive() {
	return scroller->GetGridActive();
}

Grid ZoomImagePanel::GetGrid() {
	return scroller->GetGrid();
}

void ZoomImagePanel::SetGrid(Grid newGrid) {
	scroller->SetGrid(newGrid);
}

void ZoomImagePanel::SetGridOwner(int newOwner) {
	scroller->SetGridOwner(newOwner);
}

int ZoomImagePanel::GetGridOwner() {
	return scroller->GetGridOwner();
}

void ZoomImagePanel::SetGridAspect(double aspect) {
	scroller->SetGridAspect(aspect);
}

double ZoomImagePanel::GetGridAspect() {
	return scroller->GetGridAspect();
}

void ZoomImagePanel::SetEnforceGridAspect(bool enforceGridAspect) {
	scroller->SetEnforceGridAspect(enforceGridAspect);
}

bool ZoomImagePanel::GetEnforceGridAspect() {
	return scroller->GetEnforceGridAspect();
}

void ZoomImagePanel::NoImage() {
	scroller->NoImage();
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	currentlyDrawing = false;
	disreguardScroll = false;
	zoom = 1.0;
	keepAspect = true;
	bitmapDraw = wxBitmap(wxImage(1, 1));
	noImage = true;
	doFit = false;

	gridActive = false;
	gridOwner = -1;
	grid.startX = 100;
	grid.startY = 100;
	grid.width = 1000;
	grid.height = 1000;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 100;
	drawGrid.startY = 100;
	drawGrid.width = 1000;
	drawGrid.height = 1000;
	gridMoving = false;

	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, Image * image) : wxScrolledWindow(parent) {
	
	currentlyDrawing = false;
	disreguardScroll = false;
	this->ChangeImage(image);
	zoom = 1.0;
	keepAspect = true;
	noImage = false;
	doFit = false;

	SetScrollbars(1, 1, image->GetWidth(), image->GetHeight());
	
	gridActive = false;
	gridOwner = -1;
	grid.startX = 100;
	grid.startY = 100;
	grid.width = 1000;
	grid.height = 1000;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 100;
	drawGrid.startY = 100;
	drawGrid.width = 1000;
	drawGrid.height = 1000;
	gridMoving = false;

	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

ZoomImagePanel::ImageScroll::ImageScroll(wxWindow * parent, wxImage * image) : wxScrolledWindow(parent) {
	
	currentlyDrawing = false;
	disreguardScroll = false;
	this->ChangeImage(image);
	zoom = 1.0;
	keepAspect = true;
	noImage = false;
	doFit = false;

	gridActive = false;
	gridOwner = -1;
	grid.startX = 100;
	grid.startY = 100;
	grid.width = 1000;
	grid.height = 1000;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 100;
	drawGrid.startY = 100;
	drawGrid.width = 1000;
	drawGrid.height = 1000;
	gridMoving = false;

	SetScrollbars(1, 1, image->GetWidth(), image->GetHeight());
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	
	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

void ZoomImagePanel::ImageScroll::NoImage() {
	noImage = true;
}

void ZoomImagePanel::ImageScroll::Render(wxDC& dc) {
    
	// Prepare the DC, zoom the correct amount and draw the bitmap
	this->PrepareDC(dc);
    dc.SetUserScale(zoom, zoom);
    
	// Clear and set background color
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));

	if (noImage) { return; }
	// Do not attempt to draw bitmap if it is not okay
	if (!bitmapDraw.IsOk() || bitmapDraw.GetWidth() < 1 || bitmapDraw.GetHeight() < 1) {
		return;
	}

	// Calculate new width and height based on zoom
	int imgWidth = bitmapDraw.GetWidth() * zoom;
	int imgHeight = bitmapDraw.GetHeight() * zoom;
	this->SetVirtualSize(imgWidth, imgHeight);

	if (imgWidth < 1 || imgHeight < 1) { return; }

	int thisWidth = this->GetClientSize().GetWidth();
	int thisHeight = this->GetClientSize().GetHeight();

	int xShift = 0;
	int yShift = 0;
	if (thisWidth > imgWidth) { xShift = ((thisWidth - imgWidth)/ 2) / zoom; }
	if (thisHeight > imgHeight) { yShift = ((thisHeight - imgHeight) / 2) / zoom; }
   
	dc.DrawBitmap(bitmapDraw, wxPoint(xShift, yShift));
    
	if (gridActive){
		
		int lineWidth = (int) (4.0 / this->GetZoom());
		if(this->GetZoom() < 1.0){ lineWidth = 2;}

		wxColour white(255, 255, 255);
		wxDash dashPattern[2];
		dashPattern[0] = 15;
		dashPattern[1] = 15;
		wxPen dashPen(white, lineWidth, wxPENSTYLE_USER_DASH);
		dashPen.SetDashes(2, dashPattern);

		wxColour black(0, 0, 0);
		wxPen blackPen(black, lineWidth, wxPENSTYLE_SOLID);

		int bottomLineRaise = -1.0 * lineWidth;

		// Draw solid black line
		dc.SetPen(blackPen);

		// Top Line and Bottom Line (solid black)
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + yShift, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + yShift);
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + drawGrid.height + yShift + bottomLineRaise, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY  + drawGrid.height + yShift + bottomLineRaise);

		// Left Line and Right Line (solid black)
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + yShift, drawGrid.startX + xShift, drawGrid.startY + drawGrid.height + yShift);
		dc.DrawLine(drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + yShift, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + drawGrid.height + yShift);

		// Draw dashed white line on top of solid black line
		dc.SetPen(dashPen);

		// Top Line and Bottom Line (dashed white)
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + yShift, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + yShift);
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + drawGrid.height + yShift + bottomLineRaise, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY  + drawGrid.height + yShift + + bottomLineRaise);

		// Left Line and Right Line (dashed white)
		dc.DrawLine(drawGrid.startX + xShift, drawGrid.startY + yShift, drawGrid.startX + xShift, drawGrid.startY + drawGrid.height + yShift);
		dc.DrawLine(drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + yShift, drawGrid.startX + drawGrid.width + xShift, drawGrid.startY + drawGrid.height + yShift);
	}
}

void ZoomImagePanel::ImageScroll::Redraw() {

	currentlyDrawing = true;
	// Render a buffered DC from the client DC
	wxClientDC dc(this);
	if(!dc.IsOk()){ return; }
	wxBufferedDC dcBuffer(&dc);
	this->Render(dc);
	currentlyDrawing = false;
}

void ZoomImagePanel::ImageScroll::ChangeImage(Image * newImage) {
	
	noImage = false;
	if (newImage == NULL) { 
		bitmapDraw = wxBitmap(wxImage(1, 1));
		return; 
	}
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
	else{
			bitmapDraw = wxBitmap(wxImage(1, 1));
	}

	if(doFit){
		// No need to refresh, this will be taken care of during rendering.
		// Just need to set zoom factor
		this->FitImage(false); 
	}
}

void ZoomImagePanel::ImageScroll::ChangeImage(wxImage * newImage) {

	noImage = false;
	if (newImage == NULL) { 
		bitmapDraw = wxBitmap(wxImage(1, 1));
		return; 
	}

	if (newImage->IsOk()){
		if(newImage->GetWidth() > 0 && newImage->GetHeight() > 0) {
			bitmapDraw = wxBitmap(*newImage);
		}
		else {
			bitmapDraw = wxBitmap(1, 1);
		}
	}
	else {
		bitmapDraw = wxBitmap(1, 1);
	}

	if(doFit){
		// No need to refresh, this will be taken care of during rendering.
		// Just need to set zoom factor
		this->FitImage(false); 
	}
}

void ZoomImagePanel::ImageScroll::OnDragStart(wxMouseEvent& evt) {

	// First left click before drag
	dragStartX = evt.GetPosition().x;
	dragStartY = evt.GetPosition().y;
	
	// Get scroll bar original position
	this->GetViewStart(&scrollStartX, &scrollStartY);

	// Calculate unscrolled position
	int unscrollX = 0;
	int unscrollY = 0;
	this->CalcUnscrolledPosition(dragStartX, dragStartY, &unscrollX, &unscrollY);

	// Scale dragging coordinates
	int dragStartXScale = unscrollX / this->GetZoom();
	int dragStartYScale = unscrollY / this->GetZoom();

	int pixelToleranceGridCorner = 10 / this->GetZoom();
	int hitTarget = -1;  // target = 0 i top left.  target = 1 if top right.  target = 2 if bottom left.  target = 3 if bottom right.

	// Calculate new width and height based on zoom
	int imgWidth = bitmapDraw.GetWidth() * zoom;
	int imgHeight = bitmapDraw.GetHeight() * zoom;
	if (imgWidth < 1 || imgHeight < 1) { return; }

	// Get shift values
	int thisWidth = this->GetClientSize().GetWidth();
	int thisHeight = this->GetClientSize().GetHeight();

	int xShift = 0;
	int yShift = 0;
	if (thisWidth > imgWidth) { xShift = ((thisWidth - imgWidth) / 2) / zoom; }
	if (thisHeight > imgHeight) { yShift = ((thisHeight - imgHeight) / 2) / zoom; }

	for (int i = -1.0 * pixelToleranceGridCorner; i < pixelToleranceGridCorner; i++) {
		for (int j = -1.0 * pixelToleranceGridCorner; j < pixelToleranceGridCorner; j++) {

			// Check top left for hit
			if ((grid.startX + xShift) == (dragStartXScale + i) && (grid.startY + yShift) == (dragStartYScale + j)) {
				hitTarget = ZoomImagePanel::GridHitTaget::TOP_LEFT_CORNER;
				break;
			}

			// Check top right for hit
			if (((grid.startX + xShift) + grid.width) == (dragStartXScale + i) && (grid.startY + yShift) == (dragStartYScale + j)) {
				hitTarget = ZoomImagePanel::GridHitTaget::TOP_RIGHT_CORNER;
				break;
			}

			// Check bottom left for hit
			if ((grid.startX + xShift) == (dragStartXScale + i) && ((grid.startY + yShift) + grid.height) == (dragStartYScale + j)) {
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM_LEFT_CORNER;
				break;
			}

			// Check bottom right for hit
			if (((grid.startX + xShift) + grid.width) == (dragStartXScale + i) && ((grid.startY + yShift) + grid.height) == (dragStartYScale + j)) {
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM_RIGHT_CORNER;
				break;
			}

			// Check top line for hit
			if ((grid.startY) == (dragStartYScale + j) && (dragStartXScale > (grid.startX + xShift)) && (dragStartXScale < ((grid.startX + xShift) + grid.width))) {
				hitTarget = ZoomImagePanel::GridHitTaget::TOP;
				break;
			}

			// Check bottom line for hit
			if ((grid.startY + grid.height) == (dragStartYScale + j) && (dragStartXScale > (grid.startX + xShift)) && (dragStartXScale < ((grid.startX + xShift) + grid.width))) {
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM;
				break;
			}

			// Check left line for hit
			if (((grid.startX + xShift)) == (dragStartXScale + j) && (dragStartYScale >(grid.startY + yShift)) && (dragStartYScale < ((grid.startY + yShift) + grid.height))) {
				hitTarget = ZoomImagePanel::GridHitTaget::LEFT;
				break;
			}
					
			// Check right line for hit
			if (((grid.startX + xShift) + grid.width) == (dragStartXScale + j) && (dragStartYScale >(grid.startY + yShift)) && (dragStartYScale < ((grid.startY + yShift) + grid.height))) {
				hitTarget = ZoomImagePanel::GridHitTaget::RIGHT;
				break;
			}
		}
		if (hitTarget > -1) { break; }
	}
	
	wxMouseState mouse(wxGetMouseState());

	// Handle moving grid lines
	if (hitTarget > -1) {

		int drawWidth = bitmapDraw.GetWidth();
		int drawHeight = bitmapDraw.GetHeight();
		gridMoving = true;

		int x = 0;
		int y = 0;
		while (mouse.LeftIsDown()) {
			
			// get the local mouse position (relative to the panel)
			wxPoint Local = this->ScreenToClient(mouse.GetPosition());
			x = Local.x;
			y = Local.y;

			// Get mouse dx and dy in pixels
			int dx = x - dragStartX;
			int dy = y - dragStartY;

			int dxScale = dx / this->GetZoom();
			int dyScale = dy / this->GetZoom();

			// Move top left corner
			if (hitTarget == ZoomImagePanel::GridHitTaget::TOP_LEFT_CORNER) {

				drawGrid.startX = dragStartXScale + dxScale;
				drawGrid.startY = dragStartYScale + dyScale;
				drawGrid.width = (grid.startX - drawGrid.startX) + grid.width;
				drawGrid.height = (grid.startY - drawGrid.startY) + grid.height;
			}

			// Move top right corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::TOP_RIGHT_CORNER) {

				drawGrid.startY = dragStartYScale + dyScale;
				drawGrid.width = grid.width + dxScale;
				drawGrid.height = (grid.startY - drawGrid.startY) + grid.height;
			}

			// Move bottom left corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_LEFT_CORNER) {

				drawGrid.startX = dragStartXScale + dxScale;
				drawGrid.width = (grid.startX - drawGrid.startX) + grid.width;
				drawGrid.height = grid.height + dyScale;
			}

			// Move bottom right corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_RIGHT_CORNER) {

				drawGrid.width = grid.width + dxScale;
				drawGrid.height = grid.height + dyScale;
			}

			//  Move top line
			else if (hitTarget == ZoomImagePanel::GridHitTaget::TOP) {

				// Do not move left line if enforce aspect is active and height is max
				if (enforceGridAspect) {

					// Only move if width is not maxed out, and its moving in bottom ways direction
					if (drawGrid.width < (drawWidth - 1) || (dragStartYScale + dyScale) > drawGrid.startY) {
						drawGrid.startY = dragStartYScale + dyScale;
						drawGrid.height = (grid.startY - drawGrid.startY) + grid.height;
					}
				}

				drawGrid.startY = dragStartYScale + dyScale;
				drawGrid.height = (grid.startY - drawGrid.startY) + grid.height;
			}

			//  Move bottom line
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM) {
				drawGrid.height = grid.height + dyScale;
			}

			//  Move left line
			else if (hitTarget == ZoomImagePanel::GridHitTaget::LEFT) {

				// Do not move left line if enforce aspect is active and height is max
				if(enforceGridAspect){

					// Only move if height is not maxed out, and its moving in right ways direction
					if (drawGrid.height < (drawHeight-1) || (dragStartXScale + dxScale - xShift) > drawGrid.startX) {
						drawGrid.startX = dragStartXScale + dxScale - xShift;
						drawGrid.width = (grid.startX - drawGrid.startX) + grid.width;
					}
				}

				// Freely draw line, no aspect restrictions
				else {
					drawGrid.startX = dragStartXScale + dxScale - xShift;
					drawGrid.width = (grid.startX - drawGrid.startX) + grid.width;
				}
			}

			//  Move right line
			else if (hitTarget == ZoomImagePanel::GridHitTaget::RIGHT) {
				drawGrid.width = grid.width + dxScale;
			}

			else {	}
			// Fix left line crossing over right
			if (drawGrid.width < 0 && drawGrid.startX > (grid.startX + grid.width)) {
				drawGrid.startX = grid.startX + grid.width;
				drawGrid.width *= -1;
			}

			// Fix right line crossing over left
			if (drawGrid.width < 0 && drawGrid.startX < (grid.startX + grid.width)) {
				drawGrid.startX = grid.startX + drawGrid.width;
				drawGrid.width *= -1;
			}

			// Fix top line crossing over bottom
			if (drawGrid.height < 0 && drawGrid.startY > (grid.startY + grid.height)) {
				drawGrid.startY = grid.startY + grid.height;
				drawGrid.height *= -1;
			}

			// Fix bottom line crossing over top
			if (drawGrid.height < 0 && drawGrid.startY < (grid.startY + grid.height)) {
				drawGrid.startY = grid.startY + drawGrid.height;
				drawGrid.height *= -1;
			}

			// Set starting points in image bounds
			if (drawGrid.startX < 0) { drawGrid.startX = 0; }
			if (drawGrid.startY < 0) { drawGrid.startY = 0; }

			// Set width and height in image bounds
			if (drawGrid.startX + drawGrid.width > drawWidth) {
				int subAmnt = drawGrid.startX + drawGrid.width - drawWidth;
				drawGrid.width -= subAmnt;
			}
			if (drawGrid.startY + drawGrid.height > drawHeight) {
				int subAmnt = drawGrid.startY + drawGrid.height - drawHeight;
				drawGrid.height -= subAmnt;
			}

			if (enforceGridAspect) {
				double newAspecRatio = (double)drawGrid.width / (double)drawGrid.height;

				if (gridAspect == 0.0) { gridAspect = 0.000001; }

				// Resize box according to aspect for corners
				if (hitTarget == ZoomImagePanel::GridHitTaget::TOP_LEFT_CORNER ||
					hitTarget == ZoomImagePanel::GridHitTaget::TOP_RIGHT_CORNER ||
					hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_LEFT_CORNER ||
					hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_RIGHT_CORNER) {
					
					// Resize width based on height
					if (newAspecRatio > gridAspect) {
						drawGrid.width = drawGrid.height  * gridAspect;
					}

					// Resize height based on width
					else if (newAspecRatio < gridAspect) {
						drawGrid.height = drawGrid.width / gridAspect;
					}
				}

				// Resize box according to aspect for top and bottom lines
				if (hitTarget == ZoomImagePanel::GridHitTaget::TOP ||
					hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM){

					// Resize width based on height (since we are moving top / bottom)
					drawGrid.width = drawGrid.height * gridAspect;

					// Check to verify grid is in bounds
					if (drawGrid.startX + drawGrid.width > drawWidth) {

						// Rescale height based on final width
						int subAmnt = drawGrid.startX + drawGrid.width - drawWidth;
						drawGrid.width -= subAmnt;
						drawGrid.height = drawGrid.width / gridAspect;

						if (drawGrid.startX > 0) {
							drawGrid.startX -= subAmnt;
						}
					}
				}
				
				// Resize box according to aspect for left and right lines
				if (hitTarget == ZoomImagePanel::GridHitTaget::LEFT ||
					hitTarget == ZoomImagePanel::GridHitTaget::RIGHT) {

					// Resize height based on width (since we are moving left / right)
					drawGrid.height = drawGrid.width  / gridAspect;	

					// Check to verify grid is in bounds
					if (drawGrid.startY + drawGrid.height > drawHeight) {

						// Rescale width based on final height
						int subAmnt = drawGrid.startY + drawGrid.height - drawHeight;
						drawGrid.height -= subAmnt;
						drawGrid.width = drawGrid.height * gridAspect;

						if (drawGrid.startY > 0) {
							drawGrid.startY -= subAmnt;
						}	
					}
				}
			}
			
			// Refresh and Update are CRITICAL!  This forces the screen to redraw NOW
			// so the freshly drawn DC with lines gets drawn to screen immediately.
			// Provides real time feedback!
			this->Refresh();
			this->Update();
		
			mouse.SetState(wxGetMouseState());
            wxSafeYield();
		}

		grid.startX = drawGrid.startX;
		grid.startY = drawGrid.startY;
		grid.width = drawGrid.width;
		grid.height = drawGrid.height;
		gridMoving = false;
		wxCommandEvent moveEvt(GRID_MOVE_EVENT, ID_GRID_MOVE_EVENT);
		wxPostEvent(wxWindow::FindWindowById(this->GetGridOwner()), moveEvt);
	}

	// Handle dragging image
	else {

		int newX = 0;
		int newY = 0;
		while (mouse.LeftIsDown()) {
			
			// get the local mouse position (relative to the panel)
			wxPoint Local = this->ScreenToClient(mouse.GetPosition());
			newX = Local.x;
			newY = Local.y;

			// Get mouse dx and dy in pixels
			int dx = newX - dragStartX;
			int dy = newY - dragStartY;

			// Get scroll rate to convert pixels to scroll units
			int scrollScaleX = 1;
			int scrollScaleY = 1;
			this->GetScrollPixelsPerUnit(&scrollScaleX, &scrollScaleY);

			// Convert pixels to scroll units and add to start position.  Negate dx and dy to scroll in direction of mouse drag
			int newScrollPosX = (-dx / scrollScaleX) + (scrollStartX / scrollScaleX);
			int newScrollPosY = (-dy / scrollScaleY) + (scrollStartY / scrollScaleY);

			// Scroll to new calculated position
			this->Scroll(newScrollPosX, newScrollPosY);
            this->Refresh();
            this->Update();
			mouse.SetState(wxGetMouseState());
            wxSafeYield();
		}
	}
}

void ZoomImagePanel::ImageScroll::OnPaint(wxPaintEvent& evt) {

	if(doFit){
		// No need to refresh, this will be taken care of during rendering.
		// Just need to set zoom factor
		this->FitImage(false); 
	}
	currentlyDrawing = true;
	// Create buffered and dc and render
    if(this->IsDoubleBuffered()){
        wxBufferedPaintDC paintDC(this);
        if(!paintDC.IsOk()){ return; }
        this->Render(paintDC);
    }
    else{
        wxPaintDC paintDC(this);
        if(!paintDC.IsOk()){ return; }
        this->Render(paintDC);
    }
	evt.Skip(false);
	currentlyDrawing = false;
}

void ZoomImagePanel::ImageScroll::SetZoom(double zoomFactor, bool refresh) {
	zoom = zoomFactor;
	if(refresh){ this->Redraw(); }
}

double ZoomImagePanel::ImageScroll::GetZoom() {
	return zoom;
}

void ZoomImagePanel::ImageScroll::EnableFitImage(){
	doFit = true;
}

void ZoomImagePanel::ImageScroll::DisableFitImage(){
	doFit = false;
}

bool ZoomImagePanel::ImageScroll::GetFitImage(){
	return doFit;
}

void ZoomImagePanel::ImageScroll::FitImage(bool refresh) {

	int width = this->GetClientSize().GetWidth();
	int height = this->GetClientSize().GetHeight();
	
	int imageWidth = bitmapDraw.GetWidth();
	int imageHeight = bitmapDraw.GetHeight();

	double zoomWidth = (double)width / (double)imageWidth;
	double zoomHeight = (double)height / (double)imageHeight;

	if (zoomWidth <= zoomHeight) {
		this->SetZoom(zoomWidth, refresh);
	}
	else {
		this->SetZoom(zoomHeight, refresh);
	}
}

void ZoomImagePanel::ImageScroll::DisreguardScroll(){
	disreguardScroll = true;
}

void ZoomImagePanel::ImageScroll::ReguardScroll(){
	disreguardScroll = false;
}

bool ZoomImagePanel::ImageScroll::GetReguardScroll() {
	return !disreguardScroll;
}

void ZoomImagePanel::ImageScroll::ActivateGrid() {
	gridActive = true;
	this->Redraw();
}

void ZoomImagePanel::ImageScroll::DeactivateGrid() {
	gridActive = false;
	this->Redraw();
}

bool ZoomImagePanel::ImageScroll::GetGridActive() {
	return gridActive;
}

Grid ZoomImagePanel::ImageScroll::GetGrid() {
	return grid;
}

void ZoomImagePanel::ImageScroll::SetGridOwner(int newOwner) {
	gridOwner = newOwner;
}

int ZoomImagePanel::ImageScroll::GetGridOwner() {
	return gridOwner;
}

void ZoomImagePanel::ImageScroll::SetGridAspect(double aspect) {
	gridAspect = aspect;
}

double ZoomImagePanel::ImageScroll::GetGridAspect() {
	return gridAspect;
}

void ZoomImagePanel::ImageScroll::SetEnforceGridAspect(bool enforceAspect) {
	enforceGridAspect = enforceAspect;
}

bool ZoomImagePanel::ImageScroll::GetEnforceGridAspect() {
	return enforceGridAspect;
}

bool ZoomImagePanel::ImageScroll::GetGridMoving() {
	return gridMoving;
}

void ZoomImagePanel::ImageScroll::SetGrid(Grid newGrid) {
	grid.width = newGrid.width;
	grid.height = newGrid.height;
	grid.startX = newGrid.startX;
	grid.startY = newGrid.startY;

	drawGrid.width = newGrid.width;
	drawGrid.height = newGrid.height;
	drawGrid.startX = newGrid.startX;
	drawGrid.startY = newGrid.startY;

	this->Refresh();
	this->Update();
}

void ZoomImagePanel::ImageScroll::OnRightDown(wxMouseEvent & evt) {

	wxMouseEvent newEvent(evt);
	wxCoord newX;
	wxCoord newY;

	this->CalcUnscrolledPosition(evt.GetX(), evt.GetY(), &newX, &newY);

	int imgWidth = bitmapDraw.GetWidth() * zoom;
	int imgHeight = bitmapDraw.GetHeight() * zoom;
	int thisWidth = this->GetClientSize().GetWidth();
	int thisHeight = this->GetClientSize().GetHeight();
	int xShift = 0;
	int yShift = 0;

	if (thisWidth > imgWidth) { xShift = ((thisWidth - imgWidth) / 2); }
	if (thisHeight > imgHeight) { yShift = ((thisHeight - imgHeight) / 2); }
	
	newX -= xShift;
	newY -= yShift;

	newEvent.SetX(newX / zoom);
	newEvent.SetY(newY / zoom);

	wxPostEvent(this->GetParent(), newEvent);
}