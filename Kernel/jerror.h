// $Id: jerror.h 264 2006-01-19 12:28:28Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
/*
Modified for Xara LX for 64-bit portability and for type compatibility with 
the other Xara LX code (use of INT32 instead of INT32, etc.)
*/
#include "jpgermgr.h"
/*
 * jerror.h
 *
 * Copyright (C) 1994-1995, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the error and message codes for the JPEG library.
 * Edit this file to add new codes, or to translate the message strings to
 * some other language.
 * A set of error-reporting macros are defined too.  Some applications using
 * the JPEG library may wish to include this file to get the error codes
 * and/or the macros.
 */

/*
 * To define the enum list of message codes, include this file without
 * defining macro JMESSAGE.  To create a message string table, include it
 * again with a suitable JMESSAGE definition (see jerror.c for an example).
 */

#ifndef JMESSAGE
#ifndef JERROR_H
/* First time through, define the enum list */
#define JMAKE_ENUM_LIST
#else
/* Repeated inclusions of this file are no-ops unless JMESSAGE is defined */
#define JMESSAGE(code,string,errorclass)
#endif /* JERROR_H */
#endif /* JMESSAGE */

#ifdef JMAKE_ENUM_LIST


typedef enum {

#define JMESSAGE(code,string,errorclass)	code ,

#endif /* JMAKE_ENUM_LIST */

JMESSAGE(JMSG_NOMESSAGE, "Bogus message code %d", JPEGErrorManager::ERR_INTERNAL) /* Must be first entry! */

/* For maintenance convenience, list is alphabetical by message code name */
JMESSAGE(JERR_ARITH_NOTIMPL, "Sorry, there are legal restrictions on arithmetic coding", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_BAD_ALIGN_TYPE, "ALIGN_TYPE is wrong, please fix", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_ALLOC_CHUNK, "MAX_ALLOC_CHUNK is wrong, please fix", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_BUFFER_MODE, "Bogus buffer control mode", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_COMPONENT_ID, "Invalid component ID %d in SOS", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_DCTSIZE, "IDCT output block size %d not supported", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_BAD_IN_COLORSPACE, "Bogus input colorspace", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_J_COLORSPACE, "Bogus JPEG colorspace", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_LENGTH, "Bogus marker length", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_LIB_VERSION, "Wrong JPEG library version: library is %d, caller expects %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_MCU_SIZE, "Sampling factors too large for interleaved scan", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_BAD_POOL_ID, "Invalid memory pool code %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_PRECISION, "Unsupported JPEG data precision %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_PROGRESSION, "Invalid progressive parameters Ss=%d Se=%d Ah=%d Al=%d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_PROG_SCRIPT, "Invalid progressive parameters at scan script entry %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_SAMPLING, "Bogus sampling factors", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_BAD_SCAN_SCRIPT, "Invalid scan script at entry %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_STATE, "Improper call to JPEG library in state %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_STRUCT_SIZE, "JPEG parameter struct mismatch: library thinks size is %u, caller expects %u", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BAD_VIRTUAL_ACCESS, "Bogus virtual array access", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_BUFFER_SIZE, "Buffer passed to JPEG library is too small", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_CANT_SUSPEND, "Suspension not allowed here", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_CCIR601_NOTIMPL, "CCIR601 sampling not implemented yet", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_COMPONENT_COUNT, "Too many color components: %d, max %d", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_CONVERSION_NOTIMPL, "Unsupported color conversion request", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_DAC_INDEX, "Bogus DAC index %d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_DAC_VALUE, "Bogus DAC value 0x%x", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_DHT_COUNTS, "Bogus DHT counts", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_DHT_INDEX, "Bogus DHT index %d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_DQT_INDEX, "Bogus DQT index %d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_EMPTY_IMAGE, "Empty JPEG image (DNL not supported)", JPEGErrorManager::ERR_UNSUPPORTED)
//JMESSAGE(JERR_EMS_READ, "Read from EMS failed")
//JMESSAGE(JERR_EMS_WRITE, "Write to EMS failed")
JMESSAGE(JERR_EOI_EXPECTED, "Didn't expect more than one scan", JPEGErrorManager::ERR_INTERNAL)
//JMESSAGE(JERR_FILE_READ, "Input file read error")
//JMESSAGE(JERR_FILE_WRITE, "Output file write error --- out of disk space?")
JMESSAGE(JERR_FRACT_SAMPLE_NOTIMPL, "Fractional sampling not implemented yet", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_HUFF_CLEN_OVERFLOW, "Huffman code size table overflow", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_HUFF_MISSING_CODE, "Missing Huffman code table entry", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_IMAGE_TOO_BIG, "Maximum supported image dimension is %u pixels", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_INPUT_EMPTY, "Empty input file", JPEGErrorManager::ERR_FORMAT)
JMESSAGE(JERR_INPUT_EOF, "Premature end of input file", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_MISMATCHED_QUANT_TABLE, "Cannot transcode due to multiple use of quantization table %d", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_MISSING_DATA, "Scan script does not transmit all data", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_MODE_CHANGE, "Invalid color quantization mode change", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_NOTIMPL, "Not implemented yet", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_NOT_COMPILED, "Requested feature was omitted at compile time", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_NO_BACKING_STORE, "Backing store not supported", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_NO_HUFF_TABLE, "Huffman table 0x%02x was not defined", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_NO_IMAGE, "JPEG datastream contains no image", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_NO_QUANT_TABLE, "Quantization table 0x%02x was not defined", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_NO_SOI, "Not a JPEG file: starts with 0x%02x 0x%02x", JPEGErrorManager::ERR_FORMAT)
JMESSAGE(JERR_OUT_OF_MEMORY, "Insufficient memory (case %d)", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_QUANT_COMPONENTS,	 "Cannot quantize more than %d color components", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_QUANT_FEW_COLORS, "Cannot quantize to fewer than %d colors", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_QUANT_MANY_COLORS, "Cannot quantize to more than %d colors", JPEGErrorManager::ERR_LIMITS)
JMESSAGE(JERR_SOF_DUPLICATE, "Invalid JPEG file structure: two SOF markers", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_SOF_NO_SOS, "Invalid JPEG file structure: missing SOS marker", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_SOF_UNSUPPORTED, "Unsupported JPEG process: SOF type 0x%02x", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_SOI_DUPLICATE, "Invalid JPEG file structure: two SOI markers", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JERR_SOS_NO_SOF, "Invalid JPEG file structure: SOS before SOF", JPEGErrorManager::ERR_STRUCTURE)
//JMESSAGE(JERR_TFILE_CREATE, "Failed to create temporary file %s")
//JMESSAGE(JERR_TFILE_READ, "Read failed on temporary file")
//JMESSAGE(JERR_TFILE_SEEK, "Seek failed on temporary file")
//JMESSAGE(JERR_TFILE_WRITE, "Write failed on temporary file --- out of disk space?")
JMESSAGE(JERR_TOO_LITTLE_DATA, "Application transferred too few scanlines", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_UNKNOWN_MARKER, "Unsupported marker type 0x%02x", JPEGErrorManager::ERR_UNSUPPORTED)
JMESSAGE(JERR_VIRTUAL_BUG, "Virtual array controller messed up", JPEGErrorManager::ERR_INTERNAL)
JMESSAGE(JERR_WIDTH_OVERFLOW, "Image too wide for this implementation", JPEGErrorManager::ERR_LIMITS)
//JMESSAGE(JERR_XMS_READ, "Read from XMS failed")
//JMESSAGE(JERR_XMS_WRITE, "Write to XMS failed")
//JMESSAGE(JMSG_COPYRIGHT, JCOPYRIGHT)
//JMESSAGE(JMSG_VERSION, JVERSION)
JMESSAGE(JTRC_16BIT_TABLES, "Caution: quantization tables are too coarse for baseline JPEG", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_ADOBE, "Adobe APP14 marker: version %d, flags 0x%04x 0x%04x, transform %d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_APP0, "Unknown APP0 marker (not JFIF), length %u", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_APP14, "Unknown APP14 marker (not Adobe), length %u", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_DAC, "Define Arithmetic Table 0x%02x: 0x%02x", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_DHT, "Define Huffman Table 0x%02x", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_DQT, "Define Quantization Table %d  precision %d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_DRI, "Define Restart Interval %u", JPEGErrorManager::ERR_TRACE)
//JMESSAGE(JTRC_EMS_CLOSE, "Freed EMS handle %u")
//JMESSAGE(JTRC_EMS_OPEN, "Obtained EMS handle %u")
JMESSAGE(JTRC_EOI, "End Of Image", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_HUFFBITS, "        %3d %3d %3d %3d %3d %3d %3d %3d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_JFIF, "JFIF APP0 marker, density %dx%d  %d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_JFIF_BADTHUMBNAILSIZE, "Warning: thumbnail image size does not match data length %u", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_JFIF_MINOR, "Unknown JFIF minor revision number %d.%02d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_JFIF_THUMBNAIL, "    with %d x %d thumbnail image", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_MISC_MARKER, "Skipping marker 0x%02x, length %u", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_PARMLESS_MARKER, "Unexpected marker 0x%02x", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_QUANTVALS, "        %4u %4u %4u %4u %4u %4u %4u %4u", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_QUANT_3_NCOLORS, "Quantizing to %d = %d*%d*%d colors", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_QUANT_NCOLORS, "Quantizing to %d colors", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_QUANT_SELECTED, "Selected %d colors for quantization", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_RECOVERY_ACTION, "At marker 0x%02x, recovery action %d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_RST, "RST%d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SMOOTH_NOTIMPL, "Smoothing not supported with nonstandard sampling ratios", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOF, "Start Of Frame 0x%02x: width=%u, height=%u, components=%d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOF_COMPONENT, "    Component %d: %dhx%dv q=%d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOI, "Start of Image", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOS, "Start Of Scan: %d components", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOS_COMPONENT, "    Component %d: dc=%d ac=%d", JPEGErrorManager::ERR_TRACE)
JMESSAGE(JTRC_SOS_PARAMS, "  Ss=%d, Se=%d, Ah=%d, Al=%d", JPEGErrorManager::ERR_TRACE)
//JMESSAGE(JTRC_TFILE_CLOSE, "Closed temporary file %s")
//JMESSAGE(JTRC_TFILE_OPEN, "Opened temporary file %s")
JMESSAGE(JTRC_UNKNOWN_IDS, "Unrecognized component IDs %d %d %d, assuming YCbCr", JPEGErrorManager::ERR_TRACE)
//JMESSAGE(JTRC_XMS_CLOSE, "Freed XMS handle %u")
//JMESSAGE(JTRC_XMS_OPEN, "Obtained XMS handle %u")
JMESSAGE(JWRN_ADOBE_XFORM, "Unknown Adobe color transform code %d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_BOGUS_PROGRESSION, "Inconsistent progression sequence for component %d coefficient %d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_EXTRANEOUS_DATA, "Corrupt JPEG data: %u extraneous bytes before marker 0x%02x", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_HIT_MARKER, "Corrupt JPEG data: premature end of data segment", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_HUFF_BAD_CODE, "Corrupt JPEG data: bad Huffman code", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_JFIF_MAJOR, "Warning: unknown JFIF revision number %d.%02d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_JPEG_EOF, "Premature end of JPEG file", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_MUST_RESYNC, "Corrupt JPEG data: found marker 0x%02x instead of RST%d", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_NOT_SEQUENTIAL, "Invalid SOS parameters for sequential JPEG", JPEGErrorManager::ERR_STRUCTURE)
JMESSAGE(JWRN_TOO_MUCH_DATA, "Application transferred too many scanlines", JPEGErrorManager::ERR_INTERNAL)

#ifdef JMAKE_ENUM_LIST

  JMSG_LASTMSGCODE
} J_MESSAGE_CODE;

#undef JMAKE_ENUM_LIST
#endif /* JMAKE_ENUM_LIST */

/* Zap JMESSAGE macro so that future re-inclusions do nothing by default */
#undef JMESSAGE


#ifndef JERROR_H
#define JERROR_H

/* Macros to simplify using the error and trace message stuff */
/* The first parameter is either type of cinfo pointer */

/* Fatal errors (print message and exit) */
#define ERREXIT(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT3(cinfo,code,p1,p2,p3)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT4(cinfo,code,p1,p2,p3,p4)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (cinfo)->err->msg_parm.i[3] = (p4), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXITS(cinfo,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))

#define MAKESTMT(stuff)		do { stuff } while (0)

#ifdef _DEBUG
/* Nonfatal errors (we can keep going, but the data is probably corrupt) */
#define WARNMS(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))
#define WARNMS1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))
#define WARNMS2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))

/* Informational/debugging messages */
#define TRACEMS(cinfo,lvl,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS1(cinfo,lvl,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS2(cinfo,lvl,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS3(cinfo,lvl,code,p1,p2,p3)  \
  MAKESTMT(INT32 * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS4(cinfo,lvl,code,p1,p2,p3,p4)  \
  MAKESTMT(INT32 * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS8(cinfo,lvl,code,p1,p2,p3,p4,p5,p6,p7,p8)  \
  MAKESTMT(INT32 * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   _mp[4] = (p5); _mp[5] = (p6); _mp[6] = (p7); _mp[7] = (p8); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMSS(cinfo,lvl,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#else
#define WARNMS(cinfo,code)
#define WARNMS1(cinfo,code,p1)
#define WARNMS2(cinfo,code,p1,p2)
#define TRACEMS(cinfo,lvl,code)
#define TRACEMS1(cinfo,lvl,code,p1)
#define TRACEMS2(cinfo,lvl,code,p1,p2)
#define TRACEMS3(cinfo,lvl,code,p1,p2,p3)
#define TRACEMS4(cinfo,lvl,code,p1,p2,p3,p4)
#define TRACEMS8(cinfo,lvl,code,p1,p2,p3,p4,p5,p6,p7,p8)
#define TRACEMSS(cinfo,lvl,code,str)
#endif // _DEBUG
#endif /* JERROR_H */
