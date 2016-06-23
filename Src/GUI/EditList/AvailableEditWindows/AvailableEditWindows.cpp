#include "AvailableEditWindows.h"

EditWindow* AvailableEditWindows::GetEditWindow(int editID, wxWindow * parent, Processor * processor) {

	EditWindow * newEditWindow = NULL;

	// Create new edit window based on type of edit
	switch (editID) {

		case AvailableEditIDS::EDIT_ID_SHIFT_BRIGHTNESS:
			newEditWindow = new ShiftBrightnessWindow(parent, "Shift Brightness", processor);
			break;

		case AvailableEditIDS::EDIT_ID_SCALE_BRIGHTNESS:
			newEditWindow = new ScaleBrightnessWindow(parent, "Scale Brightness", processor);
			break;

		case AvailableEditIDS::EDIT_ID_CONTRAST:
			newEditWindow = new ContrastWindow(parent, "Adjust Contrast", processor);
			break;

		case AvailableEditIDS::EDIT_ID_GREYSCALE:
			newEditWindow = new ConvertGreyscaleWindow(parent, "Convert to Greyscale", processor);
			break;

		case AvailableEditIDS::EDIT_ID_ROTATE:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case AvailableEditIDS::EDIT_ID_CHANNEL_TRANSFORM:
			newEditWindow = new ChannelTransformWindow(parent, "Channel Transform", processor);
			break;

		case AvailableEditIDS::EDIT_ID_MIRROR:
			newEditWindow = new MirrorWindow(parent, "Mirror", processor);
			break;
	}

	return newEditWindow;
}