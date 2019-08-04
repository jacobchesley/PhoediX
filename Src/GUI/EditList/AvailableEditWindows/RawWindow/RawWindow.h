// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef RAW_WINDOW_H
#define RAW_WINDOW_H

#include "wx/listctrl.h"

#include "GUI/Controls/CollapsiblePane/CollapsiblePane.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class RawWindow : public EditWindow {
	public:
		RawWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * imgPanel);
		void Process();
		void ProcessEvt(wxCommandEvent& WXUNUSED(event));
		void SetParamsAndFlags(ProcessorEdit * edit, bool reprocess = true);
		ProcessorEdit * GetParamsAndFlags();
		bool CheckCopiedParamsAndFlags();

	private:

		void PopulateRawInfo();
		bool AddRawInfo(wxString tag, wxString value, bool doNotAddZero = false);
		wxString GetShutterSpeedStr(float shutterSpeed);

		void FormatSlider(DoubleSlider * slider);

		void OnCheck(wxCommandEvent& checkEvt);
		void OnCombo(wxCommandEvent& comboEvt);
		void OnSlide(wxCommandEvent& slideEvt);
		void OnCollapseOpenClose(wxCommandEvent& WXUNUSED(evt));

		wxWindow * parWindow;
		Processor * proc;

		wxBoxSizer * mainSizer;
		wxStaticText * editLabel;

		// Settings
		CollapsiblePane * settingsPanelCollapse;
		wxPanel * settingsPanel;
		wxFlexGridSizer * settingsSizer;

		wxStaticText * greenMatchingLabel;
		wxCheckBox * greenMatchingControl;
	
		wxStaticText * flipLabel;
		PhoediXComboBox * flipControl;

		wxStaticText * interpolationLabel;
		PhoediXComboBox * interpolationControl;
		int lastInterpolationVal;

		// Exposure
		CollapsiblePane * exposurePanelCollapse;
		wxPanel * exposurePanel;
		wxFlexGridSizer * exposureSizer;

		wxStaticText * exposureLabel;
		DoubleSlider * exposureControl;

		wxStaticText * exposurePreserveLabel;
		DoubleSlider * exposurePreserveControl;

		wxStaticText * brightnessLabel;
		DoubleSlider * brightnessControl;

		wxStaticText * highlightLabel;
		PhoediXComboBox * highlightControl;

		wxStaticText * blackLevelLabel;
		DoubleSlider * blackLevelControl;
		int defaultBlack;

		wxStaticText * satLevelLabel;
		DoubleSlider * satLevelControl;
		int defaultSat;

		wxStaticText * autoBrightLabel;
		wxCheckBox * autoBrightControl;

		wxStaticText * autoBrightThrLabel;
		DoubleSlider * autoBrightThrControl;

		wxStaticText * maxThrLabel;
		DoubleSlider * maxThrControl;

		wxStaticText * gammaPresetsLabel;
		PhoediXComboBox * gammaPresetsControl;

		wxStaticText * gammaLevelLabel;
		DoubleSlider * gammaLevelControl;
		double lastGammaLevel;

		wxStaticText * gammaSlopeLabel;
		DoubleSlider * gammaSlopeControl;	
		double lastGammaSlope;
		
		bool dontProcessNexrGammaEvt;

		// Color
		CollapsiblePane * colorPanelCollapse;
		wxPanel * colorPanel;
		wxFlexGridSizer * colorSizer;

		wxStaticText * whiteBalancePresetsLabel;
		PhoediXComboBox * whiteBalancePresetsControl;

		wxStaticText * redMultiplierLabel;
		DoubleSlider * redMultiplierControl;
		
		wxStaticText * greenMultiplierLabel;
		DoubleSlider * greenMultiplierControl;

		wxStaticText * blueMultiplierLabel;
		DoubleSlider * blueMultiplierControl;

		wxStaticText * redBlackLabel;
		DoubleSlider * redBlackControl;

		wxStaticText * greenBlackLabel;
		DoubleSlider * greenBlackControl;

		wxStaticText * blueBlackLabel;
		DoubleSlider * blueBlackControl;

		// Noise
		CollapsiblePane * noisePanelCollapse;
		wxPanel * noisePanel;
		wxFlexGridSizer * noiseSizer;

		wxStaticText * waveletNoiseLabel;
		DoubleSlider * waveletNoiseControl;

		// Info
		CollapsiblePane * infoPanelCollapse;
		wxPanel * infoPanel;
		wxBoxSizer * infoSizer;

		wxListCtrl * infoList;

		enum {
			ID_AUTO_BRIGHT_CHECK,
			ID_GAMMA_PRESET_COMBO,
			ID_GAMMA_LEVEL_SLIDER,
			ID_GAMMA_SLOPE_SLIDER,
			ID_WHITE_BALANCE_COMBO,
			ID_RED_MULTIPLIER_SLIDER,
			ID_GREEN_MULTIPLIER_SLIDER,
			ID_BLUE_MULTIPLIER_SLIDER,
			ID_CFA_CLEAN_CHECK,
			ID_CFA_CLEAN_LINE_CHECK
		};
};
#endif