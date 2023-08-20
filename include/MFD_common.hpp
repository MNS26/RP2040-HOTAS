
#define MFD_BRIGHTNESS 0xb1

#define MFD_CLOCK 0xC0
#define MFD_OFFSET1 0xC1
#define MFD_OFFSET2 0xC2
#define MFD_DATE 0xC4

#define MFD_LINE_1 0xD9
#define MFD_LINE_2 0xDA
#define MFD_LINE_3 0xDC

#define MFD_APPEND_LINE_1 0xD1
#define MFD_APPEND_LINE_2 0xD2
#define MFD_APPEND_LINE_3 0xD4

#define MFD_BASE_ADDR 0xd9
#define MFD_LINE_LEN 16


// These define the character map for the MFD display
#define MFD_CHAR_SPACE 0x20
#define MFD_CHAR_EXCLAMATION 0x21
#define MFD_CHAR_DOUBLE_QUOTE 0x22
#define MFD_CHAR_POUND 0x23
#define MFD_CHAR_DOLLAR 0x24
#define MFD_CHAR_PERCENT 0x25
#define MFD_CHAR_AMPERSAND 0x26
#define MFD_CHAR_SINGLE_QUOTE 0x27
#define MFD_CHAR_LEFT_PAREN 0x28
#define MFD_CHAR_RIGHT_PAREN 0x29
#define MFD_CHAR_ASTERISK 0x2A
#define MFD_CHAR_PLUS 0x2B
#define MFD_CHAR_COMMA 0x2C
#define MFD_CHAR_MINUS 0x2D
#define MFD_CHAR_PERIOD 0x2E
#define MFD_CHAR_SLASH 0x2F
#define MFD_CHAR_0 0x30
#define MFD_CHAR_1 0x31
#define MFD_CHAR_2 0x32

// This struct will be used to store the current state of the MFD
struct mfd_t {
  char line1[17];
  char line2[17];
  char line3[17];
  bool shift;
}mfd;

/*
	command layout
	MFD brightness
	  	   2                   1
	 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|X X X X X X X X|1 0 1 1 0 0 0 1|1 0 0 1 0 0 0 1|
	|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|			DATA 			| 		0xb1 			| 		0x91

	CLOCK
	 3   3                   2       1           1
	 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	|-|-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|M|    Hour     |    Minute     |1 1 0 0 0 0 0 0|1 0 0 1 0 0 0 1|
	|-|-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
								DATA 							| 		0xc0 			| 			0x91

	OFFSET 1
	 3   3                   2       1           1
	 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	|-+-+-+-+-+-|-+-+-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|M 0 0 0 0 I|       Offset      |1 1 0 0 0 0 0 1|1 0 0 1 0 0 0 1|
	|-+-+-+-+-+-|-+-+-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
						DATA		 							| 		0xc1 			| 			0x91
	OFFSET 2
	 3   3                   2       1           1
	 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	|-+-+-+-+-+-|-+-+-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|M 0 0 0 0 I|       Offset      |1 1 0 0 0 0 1 0|1 0 0 1 0 0 0 1|
	|-+-+-+-+-+-|-+-+-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
						DATA		 							| 		0xc2 			| 			0x91

	DATE
	 3   3                   2       1           1
	 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
	|    Month      |     Date      |1 1 0 0 0 1 0 0|1 0 0 1 0 0 0 1|
	|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
								DATA 							| 		0xc4		 	| 			0x91
*/