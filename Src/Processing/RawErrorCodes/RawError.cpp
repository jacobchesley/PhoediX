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

		// Non fatal errors
		case LIBRAW_UNSPECIFIED_ERROR:
			return "Libraw Error: An unknown error has been encountered.";

		case LIBRAW_FILE_UNSUPPORTED:
			return "Libraw Error: Unsupported file format.";

		case LIBRAW_REQUEST_FOR_NONEXISTENT_IMAGE:
			return "Libraw Error: Attempt to retrieve a RAW image with a number absent in the data file.";

		case LIBRAW_OUT_OF_ORDER_CALL:
			return "Libraw Error: API functions have been called in wrong order";

		case LIBRAW_NO_THUMBNAIL:
			return "Libraw Error: Returned upon an attempt to retrieve a thumbnail from a file containing no preview";

		case LIBRAW_UNSUPPORTED_THUMBNAIL:
			return "Libraw Error: RAW file contains a preview of unsupported format";

	}

	return "Libraw Error";
}