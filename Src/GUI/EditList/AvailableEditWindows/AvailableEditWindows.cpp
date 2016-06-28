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

		case AvailableEditIDS::EDIT_ID_RGB_CURVES:
			newEditWindow = new RGBCurvesWindow(parent, "RGB Curves", processor);
			break;

		case AvailableEditIDS::EDIT_ID_LAB_CURVES:
			newEditWindow = new LABCurvesWindow(parent, "LAB Curves", processor);
			break;
	}

	return newEditWindow;
}

EditWindow* AvailableEditWindows::GetEditWindow(ProcessorEdit * edit, wxWindow * parent, Processor * processor) {

	EditWindow * newEditWindow = NULL;

	// Create new edit window based on type of edit
	switch (edit->GetEditType()) {

		case ProcessorEdit::EditType::SHIFT_BRIGHTNESS:
			newEditWindow = new ShiftBrightnessWindow(parent, "Shift Brightness", processor);
			break;

		case ProcessorEdit::EditType::SCALE_BRIGHTNESS:
			newEditWindow = new ScaleBrightnessWindow(parent, "Scale Brightness", processor);
			break;

		case ProcessorEdit::EditType::ADJUST_CONTRAST:
			newEditWindow = new ContrastWindow(parent, "Adjust Contrast", processor);
			break;

		case ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG:
			newEditWindow = new ConvertGreyscaleWindow(parent, "Convert to Greyscale", processor);
			break;

		case ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM:
			newEditWindow = new ConvertGreyscaleWindow(parent, "Convert to Greyscale", processor);
			break;

		case ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE:
			newEditWindow = new ConvertGreyscaleWindow(parent, "Convert to Greyscale", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_NONE:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_90_CW:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_180:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_270_CW:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case ProcessorEdit::EditType::CHANNEL_TRANSFORM:
			newEditWindow = new ChannelTransformWindow(parent, "Channel Transform", processor);
			break;
		case ProcessorEdit::EditType::MIRROR_NONE:
			newEditWindow = new MirrorWindow(parent, "Mirror", processor);
			break;

		case ProcessorEdit::EditType::MIRROR_HORIZONTAL:
			newEditWindow = new MirrorWindow(parent, "Mirror", processor);
			break;

		case ProcessorEdit::EditType::MIRROR_VERTICAL:
			newEditWindow = new MirrorWindow(parent, "Mirror", processor);
			break;

		case ProcessorEdit::EditType::RGB_CURVES:
			newEditWindow = new RGBCurvesWindow(parent, "RGB Curves", processor);
			break;

		case ProcessorEdit::EditType::LAB_CURVES:
			newEditWindow = new LABCurvesWindow(parent, "LAB Curves", processor);
			break;
	}

	// Set loaded parameters
	if (newEditWindow != NULL) {
		newEditWindow->SetParamsAndFlags(edit);
	}
	return newEditWindow;
}

int AvailableEditWindows::GetEditIDFromEdit(ProcessorEdit * edit) {

	EditWindow * newEditWindow = NULL;

	// Create new edit window based on type of edit
	switch (edit->GetEditType()) {

	case ProcessorEdit::EditType::SHIFT_BRIGHTNESS:
		return AvailableEditIDS::EDIT_ID_SHIFT_BRIGHTNESS;

	case ProcessorEdit::EditType::SCALE_BRIGHTNESS:
		return AvailableEditIDS::EDIT_ID_SCALE_BRIGHTNESS;

	case ProcessorEdit::EditType::ADJUST_CONTRAST:
		return AvailableEditIDS::EDIT_ID_CONTRAST;

	case ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG:
		return AvailableEditIDS::EDIT_ID_GREYSCALE;

	case ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM:
		return AvailableEditIDS::EDIT_ID_GREYSCALE;

	case ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE:
		return AvailableEditIDS::EDIT_ID_GREYSCALE;

	case ProcessorEdit::EditType::ROTATE_NONE:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_90_CW:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_180:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_270_CW:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST:
		return AvailableEditIDS::EDIT_ID_ROTATE;

	case ProcessorEdit::EditType::CHANNEL_TRANSFORM:
		return AvailableEditIDS::EDIT_ID_CHANNEL_TRANSFORM;

	case ProcessorEdit::EditType::MIRROR_NONE:
		return AvailableEditIDS::EDIT_ID_MIRROR;

	case ProcessorEdit::EditType::MIRROR_HORIZONTAL:
		return AvailableEditIDS::EDIT_ID_MIRROR;

	case ProcessorEdit::EditType::MIRROR_VERTICAL:
		return AvailableEditIDS::EDIT_ID_MIRROR;

	case ProcessorEdit::EditType::RGB_CURVES:
		return AvailableEditIDS::EDIT_ID_RGB_CURVES;

	case ProcessorEdit::EditType::LAB_CURVES:
		return AvailableEditIDS::EDIT_ID_LAB_CURVES;
	}

	return -1;
}