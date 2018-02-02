// Copyright 2016 Jacob Chesley

#ifndef CROP_WINDOW_H
#define CROP_WINDOW_H

#include "wx/thread.h"
#include "wx/tokenzr.h"
#include "wx/arrstr.h"

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
	void SetCropBoxUI(double startX, double startY, double width, double height);
private:

	void OnCombo(wxCommandEvent& WXUNUSED(evt));
	void OnEnableBox(wxCommandEvent& WXUNUSED(evt));
	void OnEnableCrop(wxCommandEvent& WXUNUSED(evt));
	void EnableCrop(bool enable);
	void OnResetCrop(wxCommandEvent& WXUNUSED(evt));
	void OnText(wxCommandEvent& evt);
	void FlipAspect(wxCommandEvent& WXUNUSED(evt));
	void ResetCropValues();
	void ChangeAspect(bool gridUpdate = true);
	Grid limitGrid(Grid inGrid);
	Grid GetNewAspectGrid(double aspect);

	wxWindow * parWindow;
	Processor * proc;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;
	wxBoxSizer * buttonSizer;
	wxBoxSizer * aspectsSizer;

	wxStaticText * editLabel;

	wxComboBox * defaultAspects;
	wxButton * flipAspects;
	wxTextCtrl * customAspectWidth;
	wxStaticText * customAspectX;
	wxTextCtrl * customAspectHeight;
	wxBoxSizer * customAspectSizer;

	wxButton * enableCropBox;
	wxButton * enableDisableCrop;
	wxButton * resetCrop;

	bool boxEnabled;
	bool cropEnabled;
	bool forceAspect;
	double aspect;

	Grid cropGrid;
	
	enum {
		ID_ENABLE_CROP_BOX,
		ID_ENABLE_CROP,
		ID_RESET_CROP,
		ID_FLIP_ASPECTS
	};

};
#endif