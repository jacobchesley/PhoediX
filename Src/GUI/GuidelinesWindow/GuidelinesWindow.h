// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef GUIDELINES_WINDOW_H
#define GUIDELINES_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colordlg.h"

#include "GUI/Colors/Colors.h"
#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Controls/PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/ExportWindow/ExportWindow.h"
#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"

class GuidelinesWindow : public wxScrolledWindow {

public:
	GuidelinesWindow(wxWindow * parent, ZoomImagePanel * mainImagePanel);

	wxColour GetGuideColor();
	void SetGuideColor(wxColour color);

	wxColour GetGridColor1();
	void SetGridColor1(wxColour color);

	wxColour GetGridColor2();
	void SetGridColor2(wxColour color);

	int GetGuidelineType();
	void SetGuidelineType(int type);

private:

	enum Buttons {
		GUIDE_COLOR,
		GRID_COLOR1,
		GRID_COLOR2
	};

	void OnCombo(wxCommandEvent& WXUNUSED(event));
	void OnGuideColor(wxCommandEvent& WXUNUSED(event));
	void OnGridColor1(wxCommandEvent& WXUNUSED(event));
	void OnGridColor2(wxCommandEvent& WXUNUSED(event));
	void Save();

	wxWindow * parWindow;
	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizerGuidelines;

	wxStaticText * guidelinesLabel;

	wxStaticText * guidelinesTypeLabel;
	PhoediXComboBox * guidelinesType;

	wxStaticText * guidelinesColorLabel;
	PhoediXButton * guidelinesColor;

	wxStaticText * cropgridColorLabel;

	wxFlexGridSizer * gridSizerCrop;

	wxStaticText * gridColor1Label;
	PhoediXButton * gridColor1;

	wxStaticText * gridColor2Label;
	PhoediXButton * gridColor2;

	ZoomImagePanel * panel;

	wxColour guideColor;
	wxColour grid1Color;
	wxColour grid2Color;
};

#endif
