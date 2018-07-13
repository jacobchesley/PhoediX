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

	fullImageView = new PhoediXButton(this, ZoomImagePanel::Buttons::ZOOM_FIT, "Fit Image");
	fullImageView->SetBackgroundColour(Colors::BackDarkDarkGrey);
	fullImageView->SetForegroundColour(Colors::TextLightGrey);

	viewImage100 = new PhoediXButton(this, ZoomImagePanel::Buttons::ZOOM_100, "100% Zoom");
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
	newEvent.SetId(this->GetId());
	wxPostEvent(this->GetParent(), newEvent);
}

void ZoomImagePanel::FitImage() {

	scroller->FitImage(true);
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

void ZoomImagePanel::EnableHalfSize(){
	scroller->EnableHalfSize();
}

void ZoomImagePanel::DisableHalfSize(){
	scroller->DisableHalfSize();
}

void ZoomImagePanel::NoImage() {
	scroller->NoImage();
}

double ZoomImagePanel::GetImageAspect() {
	return scroller->GetImageAspect();
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
	scaleGrid.startX = 0.0;
	scaleGrid.startY = 0.0;
	scaleGrid.endX = 1.0;
	scaleGrid.endY = 1.0;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 0.0;
	drawGrid.startY = 0.0;
	drawGrid.endX = 0.0;
	drawGrid.endY= 0.0;
	gridMoving = false;

	scalar = 1.0;
	quality = wxINTERPOLATION_FAST;

	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	this->Bind(wxEVT_SCROLLBAR, (wxObjectEventFunction)&ImageScroll::OnScroll, this);

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
	scaleGrid.startX = 0.0;
	scaleGrid.startY = 0.0;
	scaleGrid.endX = 1.0;
	scaleGrid.endY = 1.0;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 0.0;
	drawGrid.startY = 0.0;
	drawGrid.endX = image->GetWidth();
	drawGrid.endY = image->GetHeight();
	gridMoving = false;

	scalar = 1.0;
	quality = wxINTERPOLATION_FAST;

	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	this->Bind(wxEVT_SCROLLBAR, (wxObjectEventFunction)&ImageScroll::OnScroll, this);

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
	scaleGrid.startX = 0.0;
	scaleGrid.startY = 0.0;
	scaleGrid.endX = 1.0;
	scaleGrid.endY = 1.0;
	gridAspect = 1.0;
	enforceGridAspect = false;

	drawGrid.startX = 0.0;
	drawGrid.startY = 0.0;
	drawGrid.endX = image->GetWidth();
	drawGrid.endY = image->GetHeight();
	gridMoving = false;

	scalar = 1.0;
	quality = wxINTERPOLATION_FAST;

	SetScrollbars(1, 1, image->GetWidth(), image->GetHeight());
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&ImageScroll::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ImageScroll::OnDragStart, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&ImageScroll::OnRightDown, this);
	this->Bind(wxEVT_SCROLLBAR, (wxObjectEventFunction)&ImageScroll::OnScroll, this);

	#if defined(__WXMSW__) || defined(__WXGTK__)
		this->SetDoubleBuffered(true);
	#endif
}

void ZoomImagePanel::ImageScroll::NoImage() {
	noImage = true;
}

void ZoomImagePanel::ImageScroll::Render(wxGCDC& dc) {
	
	double tempScalar = 1.0;
	if(!this->GetFitImage()){
		tempScalar = scalar;
	}
	// Prepare the DC, zoom the correct amount and draw the bitmap
	this->PrepareDC(dc);
    dc.SetUserScale(zoom * tempScalar, zoom * tempScalar);
    
	// Clear and set background color
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));

	if (noImage) { return; }
	// Do not attempt to draw bitmap if it is not okay
	if (!bitmapDraw.IsOk() || bitmapDraw.GetWidth() < 1 || bitmapDraw.GetHeight() < 1) {
		return;
	}

	// Calculate new width and height based on zoom
	int imgWidth = bitmapDraw.GetWidth() * zoom * tempScalar;
	int imgHeight = bitmapDraw.GetHeight() * zoom * tempScalar;
	this->SetVirtualSize(imgWidth, imgHeight);

	if (imgWidth < 1 || imgHeight < 1) { return; }

	int thisWidth = this->GetClientSize().GetWidth();
	int thisHeight = this->GetClientSize().GetHeight();

	// Shift image to middle of panel
	int xShift = 0;
	int yShift = 0;
	if (thisWidth > imgWidth) { xShift = ((thisWidth - imgWidth)/ 2) / zoom; }
	if (thisHeight > imgHeight) { yShift = ((thisHeight - imgHeight) / 2) / zoom; }

	dc.GetGraphicsContext()->SetInterpolationQuality(quality);
	dc.DrawBitmap(bitmapDraw, wxPoint(xShift/tempScalar, yShift/tempScalar));

	if (gridActive){
		
		int lineWidth = (int) (4.0 / this->GetZoom());
		if(this->GetZoom() < 1.0){ lineWidth = 2;}

		xShift /= scalar;
		yShift /= scalar;
		lineWidth /= scalar;

		wxColour white(255, 255, 255);
		wxDash dashPattern[2];
		dashPattern[0] = 15;
		dashPattern[1] = 15;
		wxPen dashPen(white, lineWidth, wxPENSTYLE_USER_DASH);
		dashPen.SetDashes(2, dashPattern);

		wxColour black(0, 0, 0);
		wxPen blackPen(black, lineWidth, wxPENSTYLE_SOLID);

		// Draw solid black line
		dc.SetPen(blackPen);

		int startXShift = (int)drawGrid.startX + xShift;
		int startYShift = (int)drawGrid.startY + yShift;
		int endXShift = (int)drawGrid.endX + xShift;
		int endYShift = (int)drawGrid.endY + yShift;

		// Top Line and Bottom Line (solid black)
		dc.DrawLine(startXShift, startYShift, endXShift, startYShift);
		dc.DrawLine(startXShift, endYShift, endXShift, endYShift);

		// Left Line and Right Line (solid black)
		dc.DrawLine(startXShift, startYShift, startXShift, endYShift);
		dc.DrawLine(endXShift, startYShift, endXShift, endYShift);

		// Draw dashed white line on top of solid black line
		dc.SetPen(dashPen);

		// Top Line and Bottom Line (dashed white)
		dc.DrawLine(startXShift, startYShift, endXShift, startYShift);
		dc.DrawLine(startXShift, endYShift, endXShift, endYShift);

		// Left Line and Right Line (dashed white)
		dc.DrawLine(startXShift, startYShift, startXShift, endYShift);
		dc.DrawLine(endXShift, startYShift, endXShift, endYShift);
	}
}

void ZoomImagePanel::ImageScroll::Redraw() {

	quality = wxINTERPOLATION_FAST;
	currentlyDrawing = true;
	// Render a buffered DC from the client DC
	wxClientDC dc(this);
	if(!dc.IsOk()){ return; }
	wxBufferedDC dcBuffer(&dc);
	dcBuffer.Clear();
	dcBuffer.SetBackground(wxBrush(this->GetBackgroundColour()));
	wxGCDC gc(dcBuffer);
	this->Render(gc);
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

void ZoomImagePanel::ImageScroll::OnScroll(wxCommandEvent & WXUNUSED(evt)){
	quality = wxINTERPOLATION_NONE;

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
	int dragStartXScale = unscrollX / (this->GetZoom() * scalar);
	int dragStartYScale = unscrollY / (this->GetZoom() * scalar);

	int pixelToleranceGridCorner = 10 / (this->GetZoom() * scalar);
	int hitTarget = -1;  // target = 0 i top left.  target = 1 if top right.  target = 2 if bottom left.  target = 3 if bottom right.
	int gridMoveDirection = 1;

	// Calculate new width and height based on zoom
	int imgWidth = bitmapDraw.GetWidth() * (this->GetZoom() * scalar);
	int imgHeight = bitmapDraw.GetHeight() * (this->GetZoom() * scalar);
	if (imgWidth < 1 || imgHeight < 1) { return; }

	// Get shift values (because image is centered in panel)
	int thisWidth = this->GetClientSize().GetWidth();
	int thisHeight = this->GetClientSize().GetHeight();

	// Calcualte shift values to center image if it does not take up whole panel
	int xShift = 0;
	int yShift = 0;
	if (thisWidth > imgWidth) { xShift = ((thisWidth - imgWidth) / 2) / zoom; }
	if (thisHeight > imgHeight) { yShift = ((thisHeight - imgHeight) / 2) / zoom; }
	xShift /= scalar;
	yShift /= scalar;

	int gridStartXShift = (int)drawGrid.startX + xShift;
	int gridStartYShift = (int)drawGrid.startY + yShift;
	int gridEndXShift = (int)drawGrid.endX + xShift;
	int gridEndYShift = (int)drawGrid.endY + yShift;

	Grid lastGoodDrawGrid;
	lastGoodDrawGrid.startX = drawGrid.startX;
	lastGoodDrawGrid.endX = drawGrid.endX;
	lastGoodDrawGrid.startY = drawGrid.startY;
	lastGoodDrawGrid.endY = drawGrid.endY;

	for (int xCorner = -pixelToleranceGridCorner; xCorner < pixelToleranceGridCorner; xCorner++) {
		for (int yCorner = -pixelToleranceGridCorner; yCorner < pixelToleranceGridCorner; yCorner++) {

			int checkX = dragStartXScale + xCorner;
			int checkY = dragStartYScale + yCorner;

			// Check top left for hit
			if (gridStartXShift == checkX && gridStartYShift  == checkY) {
				hitTarget = ZoomImagePanel::GridHitTaget::TOP_LEFT_CORNER;
				break;
			}

			// Check top right for hit
			if (gridEndXShift == checkX && gridStartYShift == checkY) {
				hitTarget = ZoomImagePanel::GridHitTaget::TOP_RIGHT_CORNER;
				break;
			}

			// Check bottom left for hit
			if (gridStartXShift == checkX && gridEndYShift == checkY) {
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM_LEFT_CORNER;
				break;
			}

			// Check bottom right for hit
			if (gridEndXShift == checkX && gridEndYShift == checkY) {
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM_RIGHT_CORNER;
				break;
			}

			// Check top line for hit
			if(gridStartYShift == checkY){
				hitTarget = ZoomImagePanel::GridHitTaget::TOP;
				gridMoveDirection = ZoomImagePanel::GridMoveDirection::GRID_MOVE_HEIGHT;
				break;
			}

			// Check bottom line for hit
			if(gridEndYShift == checkY){
				hitTarget = ZoomImagePanel::GridHitTaget::BOTTOM;
				gridMoveDirection = ZoomImagePanel::GridMoveDirection::GRID_MOVE_HEIGHT;
				break;
			}

			// Check left line for hit
			if(gridStartXShift == checkX){
				hitTarget = ZoomImagePanel::GridHitTaget::LEFT;
				gridMoveDirection = ZoomImagePanel::GridMoveDirection::GRID_MOVE_WIDTH;
				break;
			}
					
			// Check right line for hit
			if(gridEndXShift == checkX){
				hitTarget = ZoomImagePanel::GridHitTaget::RIGHT;
				gridMoveDirection = ZoomImagePanel::GridMoveDirection::GRID_MOVE_WIDTH;
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

			dxScale /= scalar;
			dyScale /= scalar;

			int dragScaleShiftX = dragStartXScale + dxScale - xShift;
			int dragScaleShiftY = dragStartYScale + dyScale - yShift;

			// Move top left corner
			if (hitTarget == ZoomImagePanel::GridHitTaget::TOP_LEFT_CORNER) {
				drawGrid.startX = dragScaleShiftX;
				drawGrid.startY = dragScaleShiftY;
			}

			// Move top right corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::TOP_RIGHT_CORNER) {
				drawGrid.startY = dragScaleShiftY;
				drawGrid.endX = dragScaleShiftX;
			}
			
			// Move bottom left corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_LEFT_CORNER) {
				drawGrid.startX = dragScaleShiftX;
				drawGrid.endY = dragScaleShiftY;
			}

			// Move bottom right corner
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM_RIGHT_CORNER) {
				drawGrid.endX = dragScaleShiftX;
				drawGrid.endY = dragScaleShiftY;
			}

			// Move top
			else if (hitTarget == ZoomImagePanel::GridHitTaget::TOP) {
				drawGrid.startY = dragScaleShiftY;
			}

			// Move bottom
			else if (hitTarget == ZoomImagePanel::GridHitTaget::BOTTOM) {
				drawGrid.endY = dragScaleShiftY;
			}

			// Move left
			else if (hitTarget == ZoomImagePanel::GridHitTaget::LEFT) {
				drawGrid.startX = dragScaleShiftX;
			}

			// Move right
			else if (hitTarget == ZoomImagePanel::GridHitTaget::RIGHT) {
				drawGrid.endX = dragScaleShiftX;
			}

			if (enforceGridAspect) {

				// Flip start and end x if needed
				if (drawGrid.startX > drawGrid.endX) {
					int tempStart = drawGrid.startX;
					drawGrid.startX = drawGrid.endX;
					drawGrid.endX = tempStart;
				}

				// Flip start and end y if needed
				if (drawGrid.startY > drawGrid.endY) {
					int tempStart = drawGrid.startY;
					drawGrid.startY = drawGrid.endY;
					drawGrid.endY = tempStart;
				}

				// Get current grid aspect ratio
				double gridWidth = drawGrid.endX - drawGrid.startX;
				double gridHeight = drawGrid.endY - drawGrid.startY;
	
				// Draging width, adjust height
				if(gridMoveDirection == ZoomImagePanel::GridMoveDirection::GRID_MOVE_WIDTH){

					int newGridHeight = gridWidth / gridAspect;
					int dGridHeight = newGridHeight - gridHeight;
					if (dGridHeight % 2 == 0) {
						drawGrid.startY -= dGridHeight / 2;
						drawGrid.endY += dGridHeight / 2;
					}
					else {
						drawGrid.startY -= dGridHeight / 2;
						drawGrid.endY += (dGridHeight / 2) + 1;
					}
				}

				// Draging height, adjust width
				if(gridMoveDirection == ZoomImagePanel::GridMoveDirection::GRID_MOVE_HEIGHT){
					
					int newGridWidth = gridHeight * gridAspect;
					int dGridWidth = newGridWidth - gridWidth;
					if (dGridWidth % 2 == 0) {
						drawGrid.startX -= dGridWidth / 2;
						drawGrid.endX += dGridWidth / 2;
					}
					else {
						drawGrid.startX -= dGridWidth / 2;
						drawGrid.endX += (dGridWidth / 2) + 1;
					}
				}			

				// Check if new grid is invalid
				bool newGridInvalid = false;
				if (drawGrid.startX < 0.0 || drawGrid.startX > drawWidth) { newGridInvalid = true; }
				if (drawGrid.startY < 0.0 || drawGrid.startY > drawHeight) { newGridInvalid = true; }
				if (drawGrid.endX < 0.0 || drawGrid.endX > drawWidth) { newGridInvalid = true; }
				if (drawGrid.endY < 0.0 || drawGrid.endY > drawHeight) { newGridInvalid = true; }

				// Grid is invalid, use last valid grid
				if(newGridInvalid){
					drawGrid.startX = lastGoodDrawGrid.startX;
					drawGrid.startY = lastGoodDrawGrid.startY;
					drawGrid.endX = lastGoodDrawGrid.endX;
					drawGrid.endY = lastGoodDrawGrid.endY;
				}

				// Grid is valid, set last valid grid to current grid
				else{
					lastGoodDrawGrid.startX = drawGrid.startX;
					lastGoodDrawGrid.startY = drawGrid.startY;
					lastGoodDrawGrid.endX = drawGrid.endX;
					lastGoodDrawGrid.endY = drawGrid.endY;
				}
			}
			
			// Set grid to image bounds
			if (drawGrid.startX < 0.0) { drawGrid.startX = 0.0; }
			if (drawGrid.startX > drawWidth) { drawGrid.startX = drawWidth; }
			if (drawGrid.startY < 0.0) { drawGrid.startY = 0.0; }
			if (drawGrid.startY > drawHeight) { drawGrid.startY = drawHeight; }
			if (drawGrid.endX < 0.0) { drawGrid.endX = 0.0; }
			if (drawGrid.endX > drawWidth) { drawGrid.endX = drawWidth; }
			if (drawGrid.endY < 0.0) { drawGrid.endY = 0.0; }
			if (drawGrid.endY > drawHeight) { drawGrid.endY = drawHeight; }

			// Refresh and Update are CRITICAL!  This forces the screen to redraw NOW
			// so the freshly drawn DC with lines gets drawn to screen immediately.
			// Provides real time feedback!
			this->Refresh();
			this->Update();
		
			mouse.SetState(wxGetMouseState());
            wxSafeYield();
		}

		if(drawGrid.startX > drawGrid.endX){
			int tempStart = drawGrid.startX;
			drawGrid.startX = drawGrid.endX;
			drawGrid.endX = tempStart;
		}

		if(drawGrid.startY > drawGrid.endY){
			int tempStart = drawGrid.startY;
			drawGrid.startY = drawGrid.endY;
			drawGrid.endY = tempStart;
		}

		scaleGrid.startX = drawGrid.startX / drawWidth;
		scaleGrid.endX = drawGrid.endX / drawWidth;
		scaleGrid.startY = drawGrid.startY / drawHeight;
		scaleGrid.endY = drawGrid.endY / drawHeight;

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
			quality = wxINTERPOLATION_NONE;
			this->Scroll(newScrollPosX, newScrollPosY);
            this->Refresh();
            this->Update();
			mouse.SetState(wxGetMouseState());
            wxSafeYield();
		}
		this->Redraw();
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
		paintDC.Clear();
		paintDC.SetBackground(wxBrush(this->GetBackgroundColour()));
		wxGCDC gc(paintDC);
        this->Render(gc);
    }
    else{
        wxPaintDC paintDC(this);
        if(!paintDC.IsOk()){ return; }
		paintDC.Clear();
		paintDC.SetBackground(wxBrush(this->GetBackgroundColour()));
		wxGCDC gc(paintDC);
        this->Render(gc);
    }
	evt.Skip(false);
	currentlyDrawing = false;
	quality = wxINTERPOLATION_FAST;
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

void ZoomImagePanel::ImageScroll::EnableHalfSize(){
	scalar = 2.0;
}

void ZoomImagePanel::ImageScroll::DisableHalfSize(){

	scalar = 1.0;
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
	return scaleGrid;
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
	if (enforceAspect) {
		scaleGrid.endY = gridAspect;
		drawGrid.endY *= gridAspect;
	}
}

bool ZoomImagePanel::ImageScroll::GetEnforceGridAspect() {
	return enforceGridAspect;
}

bool ZoomImagePanel::ImageScroll::GetGridMoving() {
	return gridMoving;
}

void ZoomImagePanel::ImageScroll::SetGrid(Grid newGrid) {

	scaleGrid.startX = newGrid.startX;
	scaleGrid.startY = newGrid.startY;
	scaleGrid.endX = newGrid.endX;
	scaleGrid.endY = newGrid.endY;

	drawGrid.startX = newGrid.startX * bitmapDraw.GetWidth();
	drawGrid.startY = newGrid.startY * bitmapDraw.GetHeight();
	drawGrid.endX = newGrid.endX * bitmapDraw.GetWidth();
	drawGrid.endY = newGrid.endY * bitmapDraw.GetHeight();

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

double ZoomImagePanel::ImageScroll::GetImageAspect() {
	return (double)bitmapDraw.GetWidth() / (double)bitmapDraw.GetHeight();
}