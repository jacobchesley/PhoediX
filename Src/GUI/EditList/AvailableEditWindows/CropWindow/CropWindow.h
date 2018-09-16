// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef CROP_WINDOW_H
#define CROP_WINDOW_H

#include "wx/thread.h"
#include "wx/tokenzr.h"
#include "wx/arrstr.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class CropWindow : public EditWindow {
public:
	CropWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * imgPanel);
	void DestroyEditWindow();

	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	void OnCombo(wxCommandEvent& WXUNUSED(evt));
	void OnEnableBox(wxCommandEvent& WXUNUSED(evt));
	void EnableBox(bool enable);
	void OnEnableCrop(wxCommandEvent& WXUNUSED(evt));
	void EnableCrop(bool enable);
	void OnResetCrop(wxCommandEvent& WXUNUSED(evt));
	void OnText(wxCommandEvent& evt);
	void FlipAspect();
	void OnFlipAspect(wxCommandEvent& WXUNUSED(evt));
	void ResetCropValues();
	void ChangeAspect(bool gridUpdate = true);
	void OnGridMove(wxCommandEvent& WXUNUSED(evt));
	Grid limitGrid(Grid inGrid);
	Grid GetNewAspectGrid(double aspectRatio);

	wxWindow * parWindow;
	Processor * proc;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;
	wxBoxSizer * buttonSizer;
	wxBoxSizer * aspectsSizer;

	wxStaticText * editLabel;

	PhoediXComboBox * defaultAspects;
	PhoediXButton * flipAspects;
	wxTextCtrl * customAspectWidth;
	wxStaticText * customAspectX;
	wxTextCtrl * customAspectHeight;
	wxBoxSizer * customAspectSizer;

	PhoediXButton * enableCropBox;
	PhoediXButton * enableDisableCrop;
	PhoediXButton * resetCrop;

	bool boxEnabled;
	bool cropEnabled;
	bool forceAspect;
	double aspect;
	bool aspectFlipped;

	Grid cropGrid;
	
	enum {
		ID_ENABLE_CROP_BOX,
		ID_ENABLE_CROP,
		ID_RESET_CROP,
		ID_FLIP_ASPECTS
	};

};
#endif