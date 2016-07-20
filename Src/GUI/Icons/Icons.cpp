#include "Icons.h"

Icons::Icons() {

	wxMemoryInputStream upbuttonInStream(up_button_png, up_button_png_size);
	UpButton = wxImage(upbuttonInStream);

	wxMemoryInputStream downbuttonInStream(down_button_png, down_button_png_size);
	DownButton = wxImage(downbuttonInStream);

	wxMemoryInputStream deleteButtonInStream(delete_button_png, delete_button_png_size);
	DeleteButton = wxImage(deleteButtonInStream);

	wxMemoryInputStream disableButtonInStream(disable_button_png, disable_button_png_size);
	DisableButton = wxImage(disableButtonInStream);
}