const uint32_t temp_icon_width = 50;
const uint32_t temp_icon_height = 50;
const uint8_t temp_icon_data[(50*50)/2] = {
0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xA4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x4A, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAF, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFE, 0xFF, 0xFF, 0xFF, 0x5C, 0x12, 0xC5, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFE, 0xFF, 0xFF, 0xAF, 0x00, 0x44, 0x00, 0xFA, 0xFF, 0xFF, 0xFF, 0x4A, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xE4, 0xFF, 0xFF, 0xFF, 0x0E, 0xE2, 0xFF, 0x2C, 0xE0, 0xFF, 0xFF, 0xFF, 0x4A, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xC4, 0xFF, 0xFF, 0xFF, 0x08, 0xFC, 0xFF, 0xCF, 0x80, 0xFF, 0xFF, 0xFF, 0x4E, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x06, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xE4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x51, 0xFF, 0xFF, 0xFF, 0x4E, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xFF, 0xFF, 0xFF, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xFF, 0xFF, 0xFF, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0xBF, 0x00, 0xFC, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x60, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x16, 0x8F, 0x00, 0xF8, 0x51, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xFF, 0x15, 0x8F, 0x00, 0xF8, 0x61, 0xFF, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0xEF, 0x12, 0x8F, 0x00, 0xF8, 0x21, 0xFE, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0x1F, 0xB1, 0x8F, 0x00, 0xF8, 0x1B, 0xF2, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xFF, 0x05, 0xFC, 0x18, 0x00, 0x81, 0xCF, 0x60, 0xFF, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xEF, 0x70, 0x5F, 0x00, 0x00, 0x00, 0xF5, 0x07, 0xFE, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0x9F, 0xC0, 0x0C, 0x00, 0x00, 0x00, 0xB0, 0x0C, 0xFA, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0x8F, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xF7, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0x6F, 0xF1, 0x06, 0x00, 0x00, 0x00, 0x60, 0x1F, 0xF6, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0x8F, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x90, 0x0F, 0xF8, 0xFF, 0x4F, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xF4, 0xFF, 0xBF, 0xA0, 0x0E, 0x00, 0x00, 0x00, 0xE0, 0x0A, 0xFB, 0xFF, 0x4E, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xE4, 0xFF, 0xFF, 0x31, 0xAF, 0x00, 0x00, 0x00, 0xFA, 0x13, 0xFF, 0xFF, 0x4E, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xC4, 0xFF, 0xFF, 0x0A, 0xF6, 0x5C, 0x22, 0xC5, 0x6F, 0xA0, 0xFF, 0xFF, 0x4E, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xA4, 0xFF, 0xFF, 0x7F, 0x40, 0xFC, 0xFF, 0xCF, 0x04, 0xF6, 0xFF, 0xFF, 0x4A, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFF, 0xFF, 0xFF, 0x09, 0x20, 0x55, 0x03, 0x80, 0xFF, 0xFF, 0xFF, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFC, 0xFF, 0xFF, 0xEF, 0x38, 0x12, 0x83, 0xFE, 0xFF, 0xFF, 0xEF, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x4C, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x4C, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	};
