// Copyright 2016 Jacob Chesley

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
};

// Common Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_RED_SCALE "RedScale"
#define PHOEDIX_PARAMETER_GREEN_SCALE "GreenScale"
#define PHOEDIX_PARAMETER_BLUE_SCALE "BlueScale"
#define PHOEDIX_PARAMETER_LUMINACE "Luminace"
#define PHOEDIX_FLAG_INTERPOLATION "Interpolation"
#define PHOEDIX_PARAMETER_R_CURVE "RCurve"
#define PHOEDIX_PARAMETER_G_CURVE "GCurve"
#define PHOEDIX_PARAMETER_B_CURVE "BCurve"
#define PHOEDIX_PARAMETER_L_CURVE "LCurve"
#define PHOEDIX_PARAMETER_A_CURVE "ACurve"
#define PHOEDIX_PARAMETER_H_CURVE "HCurve"
#define PHOEDIX_PARAMETER_S_CURVE "SCurve"

// Adjust Brightness Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_BRIGHTNESS "Brightness"
#define PHOEDIX_PARAMETER_PRESERVATION "Preservation"
#define PHOEDIX_PARAMETER_TONE "Tone"
#define PHOEDIX_FLAG_PRESERVATION_TYPE "PreservationType"

// Adjust HSL Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_HUE "Hue"
#define PHOEDIX_PARAMETER_SATURATION "Saturation"

// Adjust LAB Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_A "A"
#define PHOEDIX_PARAMETER_B "B"

// Adjust RGB Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_ALL "All"
#define PHOEDIX_PARAMETER_RED "Red"
#define PHOEDIX_PARAMETER_GREEN "Green"
#define PHOEDIX_PARAMETER_BLUE "Blue"

// Blur Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_BLURSIZE "BlurSize"
#define PHOEDIX_PARAMETER_NUM_PASSES "NumberPasses"

// Channel Mixer Parameter/Flag/Array names
#define PHOEDIX_FLAG_PRESET "Preset"
#define PHOEDIX_PARAMETER_RED_RED "RedRed"
#define PHOEDIX_PARAMETER_RED_GREEN "RedGreen"
#define PHOEDIX_PARAMETER_RED_BLUE "RedBlue"
#define PHOEDIX_PARAMETER_GREEN_RED "GreenRed"
#define PHOEDIX_PARAMETER_GREEN_GREEN "GreenGreen"
#define PHOEDIX_PARAMETER_GREEN_BLUE "GreenBlue"
#define PHOEDIX_PARAMETER_BLUE_RED "BlueRed"
#define PHOEDIX_PARAMETER_BLUE_GREEN "BlueGreen"
#define PHOEDIX_PARAMETER_BLUE_BLUE "BlueBlue"

// Contrast Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_ALL_CONTRAST "AllContrast"
#define PHOEDIX_PARAMETER_RED_CONTRAST "RedContrast"
#define PHOEDIX_PARAMETER_GREEN_CONTRAST "greenContrast"
#define PHOEDIX_PARAMETER_BLUE_CONTRAST "BlueContrast"
#define PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER "AllContrastCenter"
#define PHOEDIX_PARAMETER_RED_CONTRAST_CENTER "RedContrastCenter"
#define PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER "greenContrastCenter"
#define PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER "BlueContrastCenter"

// Crop Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_STARTX "StartX"
#define PHOEDIX_PARAMETER_STARTY "StartY"
#define PHOEDIX_PARAMETER_CROP_WIDTH "CropWidth"
#define PHOEDIX_PARAMETER_CROP_HEIGHT "CropHeight"
#define PHOEDIX_PARAMETER_NUMERATOR "Numerator"
#define PHOEDIX_PARAMETER_DENOMINATOR "Denominator"
#define PHOEDIX_FLAG_ASPECT_SELECTION "AspectSelection"
#define PHOEDIX_FLAG_CROP_ENABLED "CropEnabled"
#define PHOEDIX_FLAG_FLIP_ASPECT "FlipAspect"
#define PHOEDIX_FLAG_BOX_ENABLED "BoxEnabled"

// Greyscale Parameter/Flag/Array names
#define PHOEDIX_FLAG_GREYTYPE "GreyType"

// Mirror Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_MIRROR_SELECT "MirrorSelection"

// RGB Curve Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_BRIGHT_CURVE "BrightCurve"

// RGB 16 Curve Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_BRIGHT_CURVE_16 "BrightCurve16"
#define PHOEDIX_PARAMETER_R_CURVE_16 "RCurve16"
#define PHOEDIX_PARAMETER_G_CURVE_16 "GCurve16"
#define PHOEDIX_PARAMETER_B_CURVE_16 "BCurve16"

// Rotation Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_ROTATE_ANGLE "RotationAngle"
#define PHOEDIX_FLAG_ROTATE_TYPE "RotationType"
#define PHOEDIX_FLAG_ROTATE_CROP "RotationCrop"

// Scale Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_SCALE_WIDTH "ScaleWidth"
#define PHOEDIX_PARAMETER_SCALE_HEIGHT "ScaleHeight"
#define PHOEDIX_FLAG_SCALE_TYPE "ScaleType"
#define PHOEDIX_FLAG_SCALE_KEEP_ASPECT "ScaleKeepAspect"

// Raw Parameter/Flag/Array names
#define PHOEDIX_PARAMETER_RAW_BRIGHTNESS "Brightness"
#define PHOEDIX_PARAMETER_RAW_HIGHLIGHTS "Highlights"
#define PHOEDIX_PARAMETER_RAW_SATURATION "Saturation"
#define PHOEDIX_PARAMETER_RAW_AUTOBRIGHT "AutoBright"
#define PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH "AutoBrightThreshold"
#define PHOEDIX_PARAMETER_RAW_EXPOSURE "Exposure"
#define PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE "ExposurePreserve"
#define PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH "MaxThreshold"
#define PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL "GammaLevel"
#define PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE "GammaSlope"
#define PHOEDIX_PARAMETER_RAW_INTERPOLATION "Interpolation"
#define PHOEDIX_PARAMETER_RAW_GREEN_MATCH "GreenMatch"
#define PHOEDIX_PARAMETER_RAW_WB_PRESET "WhiteBalancePreset"
#define PHOEDIX_PARAMETER_RAW_WB_RED "WhiteBalanceRed"
#define PHOEDIX_PARAMETER_RAW_WB_GREEN "WhiteBalanceGreen"
#define PHOEDIX_PARAMETER_RAW_WB_BLUE "WhiteBalanceBlue"
#define PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD "WaveletNoiseRD"
#define PHOEDIX_PARAMETER_FLIP "Flip"
#define PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS "SettingsTabOpen"
#define PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE "ExposureTabOpen"
#define PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR "ColorTabOpen"
#define PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE "NoiseTabOpen"
#define PHOEDIX_FLAG_RAW_TAB_OPEN_INFO "InfoTabOpen"

#endif