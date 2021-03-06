// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "AvailableEditWindows.h"

EditWindow* AvailableEditWindows::GetEditWindow(int editID, wxWindow * parent, Processor * processor, ZoomImagePanel * imgPanel) {

	EditWindow * newEditWindow = NULL;

	// Create new edit window based on type of edit
	switch (editID) {

		case AvailableEditIDS::EDIT_ID_ADJUST_BRIGHTNESS:
			newEditWindow = new AdjustBrightnessWindow(parent, "Adjust Brightness", processor);
			break;

		case AvailableEditIDS::EDIT_ID_CONTRAST:
			newEditWindow = new ContrastWindow(parent, "Adjust Contrast", processor);
			break;

		case AvailableEditIDS::EDIT_ID_ADJUST_HSL:
			newEditWindow = new AdjustHSLWindow(parent, "Adjust HSL", processor);
			break;

		case AvailableEditIDS::EDIT_ID_ADJUST_LAB:
			newEditWindow = new AdjustLABWindow(parent, "Adjust LAB", processor);
			break;

		case AvailableEditIDS::EDIT_ID_ADJUST_RGB:
			newEditWindow = new AdjustRGBWindow(parent, "Adjust RGB", processor);
			break;

		case AvailableEditIDS::EDIT_ID_BLUR:
			newEditWindow = new BlurWindow(parent, "Blur", processor);
			break;

		case AvailableEditIDS::EDIT_ID_CHANNEL_MIXER:
			newEditWindow = new ChannelMixerWindow(parent, "Channel Mixer", processor);
			break;

		case AvailableEditIDS::EDIT_ID_CROP:
			newEditWindow = new CropWindow(parent, "Crop", processor, imgPanel);
			break;

		case AvailableEditIDS::EDIT_ID_HSL_CURVES:
			newEditWindow = new HSLCurvesWindow(parent, "Curves - HSL", processor);
			break;

		case AvailableEditIDS::EDIT_ID_LAB_CURVES:
			newEditWindow = new LABCurvesWindow(parent, "Curves - LAB", processor);
			break;

		case AvailableEditIDS::EDIT_ID_RGB_CURVES:
			newEditWindow = new RGBCurvesWindow(parent, "Curves - RGB", processor);
			break;

		case AvailableEditIDS::EDIT_ID_GREYSCALE:
			newEditWindow = new ConvertGreyscaleWindow(parent, "Greyscale", processor);
			break;

		case AvailableEditIDS::EDIT_ID_MIRROR:
			newEditWindow = new MirrorWindow(parent, "Mirror", processor);
			break;

		case AvailableEditIDS::EDIT_ID_ROTATE:
			newEditWindow = new RotationWindow(parent, "Rotate", processor);
			break;

		case AvailableEditIDS::EDIT_ID_SCALE:
			newEditWindow = new ScaleWindow(parent, "Scale", processor);
			break;

		case AvailableEditIDS::EDIT_ID_RAW:
			newEditWindow = new RawWindow(parent, "Raw Processor", processor, imgPanel);
			break;
	}

	return newEditWindow;
}

EditWindow* AvailableEditWindows::GetEditWindow(ProcessorEdit * edit, wxWindow * parent, Processor * processor, ZoomImagePanel * imgPanel) {

	return AvailableEditWindows::GetEditWindow(AvailableEditWindows::GetEditIDFromEdit(edit), parent, processor, imgPanel);

}

int AvailableEditWindows::GetEditIDFromEdit(ProcessorEdit * edit) {

	// Create new edit window based on type of edit
	switch (edit->GetEditType()) {

	case ProcessorEdit::EditType::ADJUST_BRIGHTNESS:
		return AvailableEditIDS::EDIT_ID_ADJUST_BRIGHTNESS;

	case ProcessorEdit::EditType::ADJUST_RGB:
		return AvailableEditIDS::EDIT_ID_ADJUST_RGB;

	case ProcessorEdit::EditType::ADJUST_HSL:
		return AvailableEditIDS::EDIT_ID_ADJUST_HSL;

	case ProcessorEdit::EditType::ADJUST_LAB:
		return AvailableEditIDS::EDIT_ID_ADJUST_LAB;

	case ProcessorEdit::EditType::ADJUST_CONTRAST:
		return AvailableEditIDS::EDIT_ID_CONTRAST;

	case ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE:
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

	case ProcessorEdit::EditType::CHANNEL_MIXER:
		return AvailableEditIDS::EDIT_ID_CHANNEL_MIXER;

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

	case ProcessorEdit::EditType::HSL_CURVES:
		return AvailableEditIDS::EDIT_ID_HSL_CURVES;

	case ProcessorEdit::EditType::SCALE_BICUBIC:
		return AvailableEditIDS::EDIT_ID_SCALE;

	case ProcessorEdit::EditType::SCALE_BILINEAR:
		return AvailableEditIDS::EDIT_ID_SCALE;

	case ProcessorEdit::EditType::SCALE_NEAREST:
		return AvailableEditIDS::EDIT_ID_SCALE;

	case ProcessorEdit::EditType::CROP:
		return AvailableEditIDS::EDIT_ID_CROP;

	case ProcessorEdit::EditType::BLUR:
		return AvailableEditIDS::EDIT_ID_BLUR;

	case ProcessorEdit::EditType::HORIZONTAL_BLUR:
		return AvailableEditIDS::EDIT_ID_BLUR;

	case ProcessorEdit::EditType::VERTICAL_BLUR:
		return AvailableEditIDS::EDIT_ID_BLUR;

	case ProcessorEdit::EditType::RAW:
		return AvailableEditIDS::EDIT_ID_RAW;
	}

	return -1;
}