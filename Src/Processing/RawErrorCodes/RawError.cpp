// Copyright 2016 Jacob Chesley

#include "RawError.h"

wxString RawError::GetStringFromError(int librawErrorCode){
	switch(librawErrorCode){
		case LIBRAW_UNSUFFICIENT_MEMORY:
			return "Libraw Error: Attempt to get memory from the system has failed.";

		case LIBRAW_DATA_ERROR:
			return "Libraw Error: A fatal error emerged during data unpacking.";

		case LIBRAW_IO_ERROR:
			return "Libraw Error: A fatal error emerged during file reading (premature end-of-file encountered or file is corrupt).";

		case LIBRAW_CANCELLED_BY_CALLBACK:
			return "Libraw Error: Processing cancelled due to calling application demand.";

		case LIBRAW_BAD_CROP:
			return "Libraw Error: Incorrect cropping coordinates were set.";

	}
}