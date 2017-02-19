// Copyright 2016 Jacob Chesley

#ifndef CROP_WINDOW_H
#define CROP_WINDOW_H

#include "wx/thread.h"
#include "wx/tokenzr.h"

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class CropWindow : public EditWindow {
public:
	CropWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * imgPanel);
	
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

protected:
	void SetCropBoxUI(int startX, int startY, int width, int height);
private:

	void OnCombo(wxCommandEvent& WXUNUSED(evt));
	void OnEnableBox(wxCommandEvent& WXUNUSED(evt));
	void OnEnableCrop(wxCommandEvent& WXUNUSED(evt));
	void OnResetCrop(wxCommandEvent& WXUNUSED(evt));
	void OnGridMove(wxCommandEvent& WXUNUSED(evt));
	void OnText(wxCommandEvent& evt);
	void ResetCropValues();
	Grid limitGrid(Grid inGrid);

	wxWindow * parWindow;
	Processor * proc;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;
	wxBoxSizer * buttonSizer;

	wxStaticText * editLabel;

	wxComboBox * defaultAspects;
	wxTextCtrl * customAspectWidth;
	wxStaticText * customAspectX;
	wxTextCtrl * customAspectHeight;
	wxBoxSizer * customAspectSizer;

	wxStaticText * startXLabel;
	wxStaticText * startYLabel;
	wxStaticText * widthLabel;
	wxStaticText * heightLabel;

	wxTextCtrl * startXCtrl;
	wxTextCtrl * startYCtrl;
	wxTextCtrl * widthCtrl;
	wxTextCtrl * heightCtrl;

	wxButton * enableCropBox;
	wxButton * enableDisableCrop;
	wxButton * resetCrop;

	Grid lastCropGrid;

	bool boxEnabled;
	bool cropEnabled;
	bool forceAspect;
	double aspect;
	
	enum {
		ID_ENABLE_CROP_BOX,
		ID_ENABLE_CROP,
		ID_RESET_CROP
	};

};
#endif