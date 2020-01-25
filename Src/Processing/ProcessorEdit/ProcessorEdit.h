// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef PROCESSOR_EDIT_H
#define PROCESSOR_EDIT_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include "Debugging/MemoryLeakCheck.h"

class ProcessorEdit {

public :
	
	ProcessorEdit();
	ProcessorEdit(ProcessorEdit &edit);
	ProcessorEdit(int editType);

	~ProcessorEdit();

	void AddParam(wxString parameterName, double parameterValue);
	void AddFlag(wxString flagName, int flagValue);
	void AddIntArray(wxString arrayName, int * array, int arraySize);
	void AddDoubleArray(wxString arrayName, double * array, int arraySize);

	void ClearParams();
	void ClearFlags();
	void ClearIntArray();
	void ClearDoubleArray();

	size_t GetParamsSize();
	size_t GetFlagsSize();
	size_t GetNumIntArrays();
	size_t GetNumDoubleArrays();
	
	double GetParam(wxString parameterName);
	int GetFlag(wxString flagName);
	int * GetIntArray(wxString arrayName);
	size_t GetIntArraySize(wxString arrayName);
	double * GetDoubleArray(wxString arrayName);
	size_t GetDoubleArraySize(wxString arrayName);

	bool CheckForParameter(wxString parameterName);
	bool CheckForFlag(wxString flagName);
	bool CheckForIntArray(wxString arrayName);
	bool CheckForDoubleArray(wxString arrayName);

	wxVector<wxString> GetParamterNames();
	wxVector<wxString> GetFlagNames();
	wxVector<wxString> GetIntArrayNames();
	wxVector<wxString> GetDoubleArrayNames();

	void SetDisabled(bool disable);
	bool GetDisabled();

	int GetEditType();
	void SetEditType(int editType);
	void SetEditTypeFromTag(wxString inTag);

	wxString GetEditTag();

	static bool CompareProcessorEdits(ProcessorEdit * editOne, ProcessorEdit * editTwo);
	enum EditType {
		UNDEFINED = 0,
		ADJUST_RGB,
		ADJUST_HSL,
		ADJUST_LAB,
		ADJUST_CONTRAST,
		ADJUST_CONTRAST_CURVE,
		CONVERT_GREYSCALE_AVG,
		CONVERT_GREYSCALE_EYE,
		CONVERT_GREYSCALE_CUSTOM,
		CHANNEL_MIXER,
		ROTATE_NONE,
		ROTATE_90_CW,
		ROTATE_180,
		ROTATE_270_CW,
		ROTATE_CUSTOM_NEAREST,
		ROTATE_CUSTOM_BILINEAR,
		ROTATE_CUSTOM_BICUBIC,
		MIRROR_NONE,
		MIRROR_VERTICAL,
		MIRROR_HORIZONTAL,
		RGB_CURVES,
		LAB_CURVES,
		HSL_CURVES,
		RAW,
		ADJUST_BRIGHTNESS,
		SCALE_NEAREST,
		SCALE_BILINEAR,
		SCALE_BICUBIC,
		CROP,
		BLUR,
		HORIZONTAL_BLUR,
		VERTICAL_BLUR
	};
	
protected:
	std::map<wxString, double> paramMap;
	std::map<wxString, int> flagMap;
	std::map<wxString, int*> intArrayMap;
	std::map<wxString, int> intArraySizeMap;
	std::map<wxString, double*> doubleArrayMap;
	std::map<wxString, int> doubleArraySizeMap;

private:
	int editInt;
	wxString tag;

	bool isDisabled;

public:
	struct ParametersFlags {

		// Common Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_RED_SCALE; // RedScale
		static const wxString PHOEDIX_PARAMETER_GREEN_SCALE; // GreenScale
		static const wxString PHOEDIX_PARAMETER_BLUE_SCALE; // BlueScale
		static const wxString PHOEDIX_PARAMETER_LUMINACE; // Luminace
		static const wxString PHOEDIX_FLAG_INTERPOLATION; // Interpolation
		static const wxString PHOEDIX_PARAMETER_R_CURVE; // RCurve
		static const wxString PHOEDIX_PARAMETER_G_CURVE; // GCurve
		static const wxString PHOEDIX_PARAMETER_B_CURVE; // BCurve
		static const wxString PHOEDIX_PARAMETER_L_CURVE; // LCurve
		static const wxString PHOEDIX_PARAMETER_A_CURVE; // ACurve
		static const wxString PHOEDIX_PARAMETER_H_CURVE; // HCurve
		static const wxString PHOEDIX_PARAMETER_S_CURVE; // SCurve

		// Adjust Brightness Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_BRIGHTNESS; // Brightness
		static const wxString PHOEDIX_PARAMETER_PRESERVATION; // Preservation
		static const wxString PHOEDIX_PARAMETER_TONE; // Tone
		static const wxString PHOEDIX_FLAG_PRESERVATION_TYPE; // PreservationType

		// Adjust HSL Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_HUE; // Hue
		static const wxString PHOEDIX_PARAMETER_SATURATION; // Saturation

		// Adjust LAB Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_A; // A
		static const wxString PHOEDIX_PARAMETER_B; // B

		// Adjust RGB Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_ALL; // All
		static const wxString PHOEDIX_PARAMETER_RED; // Red
		static const wxString PHOEDIX_PARAMETER_GREEN; // Green
		static const wxString PHOEDIX_PARAMETER_BLUE; // Blue

		// Blur Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_BLURSIZE; // BlurSize
		static const wxString PHOEDIX_PARAMETER_NUM_PASSES; // NumberPasses

		// Channel Mixer Parameter/Flag/Array names
		static const wxString PHOEDIX_FLAG_PRESET; // Preset
		static const wxString PHOEDIX_PARAMETER_RED_RED; // RedRed
		static const wxString PHOEDIX_PARAMETER_RED_GREEN; // RedGreen
		static const wxString PHOEDIX_PARAMETER_RED_BLUE; // RedBlue
		static const wxString PHOEDIX_PARAMETER_GREEN_RED; // GreenRed
		static const wxString PHOEDIX_PARAMETER_GREEN_GREEN; // GreenGreen
		static const wxString PHOEDIX_PARAMETER_GREEN_BLUE; // GreenBlue
		static const wxString PHOEDIX_PARAMETER_BLUE_RED; // BlueRed
		static const wxString PHOEDIX_PARAMETER_BLUE_GREEN; // BlueGreen
		static const wxString PHOEDIX_PARAMETER_BLUE_BLUE; // BlueBlue

		// Contrast Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_ALL_CONTRAST; // AllContrast
		static const wxString PHOEDIX_PARAMETER_RED_CONTRAST; // RedContrast
		static const wxString PHOEDIX_PARAMETER_GREEN_CONTRAST; // greenContrast
		static const wxString PHOEDIX_PARAMETER_BLUE_CONTRAST; // BlueContrast
		static const wxString PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER; // AllContrastCenter
		static const wxString PHOEDIX_PARAMETER_RED_CONTRAST_CENTER; // RedContrastCenter
		static const wxString PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER; // greenContrastCenter
		static const wxString PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER; // BlueContrastCenter

		// Crop Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_STARTX; // StartX
		static const wxString PHOEDIX_PARAMETER_STARTY; // StartY
		static const wxString PHOEDIX_PARAMETER_CROP_WIDTH; // CropWidth
		static const wxString PHOEDIX_PARAMETER_CROP_HEIGHT; // CropHeight
		static const wxString PHOEDIX_PARAMETER_NUMERATOR; // Numerator
		static const wxString PHOEDIX_PARAMETER_DENOMINATOR; // Denominator
		static const wxString PHOEDIX_FLAG_ASPECT_SELECTION; // AspectSelection
		static const wxString PHOEDIX_FLAG_CROP_ENABLED; // CropEnabled
		static const wxString PHOEDIX_FLAG_FLIP_ASPECT; // FlipAspect
		static const wxString PHOEDIX_FLAG_BOX_ENABLED; // BoxEnabled

		// Greyscale Parameter/Flag/Array names
		static const wxString PHOEDIX_FLAG_GREYTYPE; // GreyType

		// Mirror Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_MIRROR_SELECT; // MirrorSelection

		// RGB Curve Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_BRIGHT_CURVE; // BrightCurve

		// RGB 16 Curve Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_BRIGHT_CURVE_16; // BrightCurve16
		static const wxString PHOEDIX_PARAMETER_R_CURVE_16; // RCurve16
		static const wxString PHOEDIX_PARAMETER_G_CURVE_16; // GCurve16
		static const wxString PHOEDIX_PARAMETER_B_CURVE_16; // BCurve16

		// Rotation Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_ROTATE_ANGLE; // RotationAngle
		static const wxString PHOEDIX_FLAG_ROTATE_TYPE; // RotationType
		static const wxString PHOEDIX_FLAG_ROTATE_CROP; // RotationCrop

		// Scale Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_SCALE_WIDTH; // ScaleWidth
		static const wxString PHOEDIX_PARAMETER_SCALE_HEIGHT; // ScaleHeight
		static const wxString PHOEDIX_FLAG_SCALE_TYPE; // ScaleType
		static const wxString PHOEDIX_FLAG_SCALE_KEEP_ASPECT; // ScaleKeepAspect

		// Raw Parameter/Flag/Array names
		static const wxString PHOEDIX_PARAMETER_RAW_BRIGHTNESS; // Brightness
		static const wxString PHOEDIX_PARAMETER_RAW_HIGHLIGHTS; // Highlights
		static const wxString PHOEDIX_PARAMETER_RAW_SATURATION; // Saturation
		static const wxString PHOEDIX_PARAMETER_RAW_AUTOBRIGHT; // AutoBright
		static const wxString PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH; // AutoBrightThreshold
		static const wxString PHOEDIX_PARAMETER_RAW_EXPOSURE; // Exposure
		static const wxString PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE; // ExposurePreserve
		static const wxString PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH; // MaxThreshold
		static const wxString PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL; // GammaLevel
		static const wxString PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE; // GammaSlope
		static const wxString PHOEDIX_PARAMETER_RAW_INTERPOLATION; // Interpolation
		static const wxString PHOEDIX_PARAMETER_RAW_GREEN_MATCH; // GreenMatch
		static const wxString PHOEDIX_PARAMETER_RAW_WB_PRESET; // WhiteBalancePreset
		static const wxString PHOEDIX_PARAMETER_RAW_WB_RED; // WhiteBalanceRed
		static const wxString PHOEDIX_PARAMETER_RAW_WB_GREEN; // WhiteBalanceGreen
		static const wxString PHOEDIX_PARAMETER_RAW_WB_BLUE; // WhiteBalanceBlue
		static const wxString PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD; // WaveletNoiseRD
		static const wxString PHOEDIX_PARAMETER_FLIP; // Flip
		static const wxString PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS; // SettingsTabOpen
		static const wxString PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE; // ExposureTabOpen
		static const wxString PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR; // ColorTabOpen
		static const wxString PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE; // NoiseTabOpen
		static const wxString PHOEDIX_FLAG_RAW_TAB_OPEN_INFO; // InfoTabOpen
	};
};

#endif