// Copyright 2016 Jacob Chesley

#ifndef PROCESSOR_EDIT_H
#define PROCESSOR_EDIT_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging\MemoryLeakCheck.h"


class ProcessorEdit {
public :
	
	ProcessorEdit();
	ProcessorEdit(ProcessorEdit &edit);
	ProcessorEdit(int editType);

	~ProcessorEdit();


	void AddParam(double param);
	void AddFlag(int flag);
	void AddIntArray(int * addArray, int arraySize);
	void AddDoubleArray(double * addArray, int arraySize);

	void ClearParams();
	void ClearFlags();
	void ClearIntArray();
	void ClearDoubleArray();

	int GetParamsSize();
	int GetFlagsSize();
	int GetNumIntArrays();
	int GetNumDoubleArrays();
	
	double GetParam(size_t index);
	int GetFlag(size_t index);
	int * GetIntArray(size_t index);
	int GetIntArraySize(size_t index);
	double * GetDoubleArray(size_t index);
	int GetDoubleArraySize(size_t index);

	void SetDisabled(bool disable);
	bool GetDisabled();


	int GetEditType();
	void SetEditType(int editType);
	void SetEditTypeFromTag(wxString inTag);

	wxString GetEditTag();

	enum EditType {
		UNDEFINED,
		SHIFT_RGB,
		ADJUST_HSL,
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
		CROP
	};
	
private:
	int edit;
	wxString tag;
	wxVector<double> params;
	wxVector<int> flags;

	wxVector<int*> intArrays;
	wxVector<int> intArraySizes;

	wxVector<double*> doubleArrays;
	wxVector<int> doubleArraySizes;

	bool isDisabled;

};

#endif