#include <stdint.h>
#include <stdlib.h>
#include <string.h>

union RGBW { 
	struct { 
		uint8_t R,G,B,W;
	}; 
	uint32_t raw; 
};
