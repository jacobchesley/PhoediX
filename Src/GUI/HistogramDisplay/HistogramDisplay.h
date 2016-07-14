#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\dc.h"
#include "wx\dcbuffer.h"

#include "GUI\Colors\Colors.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\ImageDisplay\wxImagePanel\wxImagePanel.h"
#include "Processing\Processor\Processor.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "Debugging\MemoryLeakCheck.h"

class HistogramDisplay : public wxPanel {
public:
	HistogramDisplay(wxWindow * parent, Processor * processor);
	void UpdateHistograms();
	void SetHistogramDisplay(int selection);
	int GetHistogramDisplay();
private:

	void OnZoom(wxCommandEvent& WXUNUSED(event));
	void OnCombo(wxCommandEvent& WXUNUSED(event));
	void ProcessComboChange();

	wxBoxSizer * mainLayout;
	wxFlexGridSizer * controlsLayout;

	wxStaticText * selectionLabel;
	wxComboBox * histogramSelector;

	wxStaticText * zoomLabel;
	DoubleSlider * histogramZoom;
	
	class HistogramScrolled : public wxScrolledWindow {

	public:
		HistogramScrolled(wxWindow * parent, Processor * processor);
		void UpdateHistograms();
		
		void HideRed();
		void ShowRed();
		void HideGreen();
		void ShowGreen();
		void HideBlue();
		void ShowBlue();
		void HideGrey();
		void ShowGrey();
		void HideAll();
		void ShowAll();

		void RedrawHistograms();
	private:

		void GenerateHistograms();

		Processor * proc;

		wxBoxSizer * histogramLayout;

		wxImage * redHistogram;
		wxImage * greenHistogram;
		wxImage * blueHistogram;
		wxImage * greyHistogram;
		wxImage * allHistogram;

		WXImagePanel * redHistogramPanel;
		WXImagePanel * greenHistogramPanel;
		WXImagePanel * blueHistogramPanel;
		WXImagePanel * greyHistogramPanel;
		WXImagePanel * allHistogramPanel;

		bool showRed;
		bool showGreen;
		bool showBlue;
		bool showGrey;
		bool showAll;
	};

	HistogramScrolled * histograms;
};

#endif