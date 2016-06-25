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
	ProcessorEdit(int editType, bool multiTheads = false, int numThreads = 0);

	void AddParam(double param);
	void AddIntArray(int * addArray, int arraySize);
	void ClearParams();
	void ClearIntArray();

	int GetParamsSize();
	int GetIntArraySize();

	double GetParam(size_t index);
	int * GetIntArray(size_t index);
	int GetIntArraySize(size_t index);

	void AddFlag(int flag);
	void ClearFlags();
	int GetFlagsSize();
	double GetFlag(size_t index);

	int GetEditType();
	void SetEditType(int editType);

	void SetMultithread(bool multiThread);
	bool GetMultithread();

	int GetNumThreads();
	void SetNumThreads(int numThreads);

	enum EditType {
		SHIFT_BRIGHTNESS,
		SCALE_BRIGHTNESS,
		ADJUST_CONTRAST,
		CONVERT_GREYSCALE_AVG,
		CONVERT_GREYSCALE_EYE,
		CONVERT_GREYSCALE_CUSTOM,
		CHANNEL_TRANSFORM,
		ROTATE_90_CW,
		ROTATE_180,
		ROTATE_270_CW,
		ROTATE_CUSTOM_NEAREST,
		ROTATE_CUSTOM_BILINEAR,
		ROTATE_CUSTOM_BICUBIC,
		MIRROR_VERTICAL,
		MIRROR_HORIZONTAL,
		RGB_CURVES,
		LAB_CURVES
	};
	
private:
	int edit;
	wxVector<double> params;
	wxVector<int> flags;

	wxVector<int*> intArrays;
	wxVector<int> intArraySizes;

	bool doMultithread;
	int numThread;

};

#endif