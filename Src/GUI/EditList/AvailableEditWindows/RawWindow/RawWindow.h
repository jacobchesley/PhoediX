// Copyright 2016 Jacob Chesley

#ifndef RAW_WINDOW_H
#define RAW_WINDOW_H

#include "wx/listctrl.h"

#include "GUI/Controls/CollapsiblePane/CollapsiblePane.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class RawWindow : public EditWindow {
	public:
		RawWindow(wxWindow * parent, wxString editName, Processor * processor);
		void Process();
		void ProcessEvt(wxCommandEvent& WXUNUSED(event));
		void SetParamsAndFlags(ProcessorEdit * edit);
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

		wxWindow * parWindow;
		Processor * proc;

		wxBoxSizer * mainSizer;
		wxStaticText * editLabel;

		// Settings
		CollapsiblePane * settingsPanelCollapse;
		wxPanel * settingsPanel;
		wxFlexGridSizer * settingsSizer;

		wxStaticText * halfSizeLabel;
		wxCheckBox * halfSizeControl;
		bool lastHalfSizeVal;

		wxStaticText * greenMatchingLabel;
		wxCheckBox * greenMatchingControl;
	
		wxStaticText * flipLabel;
		wxComboBox * flipControl;

		wxStaticText * interpolationLabel;
		wxComboBox * interpolationControl;
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
		wxComboBox * highlightControl;

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
		wxComboBox * gammaPresetsControl;

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
		wxComboBox * whiteBalancePresetsControl;

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

		wxStaticText * cfaCleanLabel;
		wxCheckBox * cfaCleanControl;

		wxStaticText * cfaCleanLLabel;
		DoubleSlider * cfaCleanLControl;

		wxStaticText * cfaCleanCLabel;
		DoubleSlider * cfaCleanCControl;

		wxStaticText * cfaCleanLineEnableLabel;
		wxCheckBox * cfaCleanLineEnableControl;

		wxStaticText * cfaCleanLineLabel;
		DoubleSlider * cfaCleanLineControl;

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