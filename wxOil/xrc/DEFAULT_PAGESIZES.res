;-----------------------------------------------------------------------------
; Page size definitions
;
; Syntax : PageSize		<PageName> <Hidth> <Height> {"MILLIMETRES" | "INCHES" } 
;
;	PageName 		The textual description of the page.
;	Width			Width of page in specified units
;	Height			Height of page in specified units
;	
;	MILLIMETRES if measurements are in millimetres
;	INCHES if measurements are in inches

PageSize	"A0"					840.0	1188.0	MILLIMETRES
PageSize	"A1"					594.0	840.0	MILLIMETRES
PageSize	"A2"					420.0	594.0	MILLIMETRES
PageSize	"A3"					297.0	420.0	MILLIMETRES
PageSize	"A4"					210.0	297.0	MILLIMETRES
PageSize	"A5"					148.5	210.0	MILLIMETRES
PageSize	"A6"					105.0	148.5	MILLIMETRES
PageSize	"US Legal"				8.5		14		INCHES
PageSize	"US Letter"				8.5		11		INCHES
PageSize	"International Fanfold"	8.25	12		INCHES

; Must be the last token in the file
PageSizeEnd
