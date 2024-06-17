#ifndef ESPNOW_MSG
#define ESPNOW_MSG

typedef struct {
	uint32_t count;
} struct_espnow_msg;

//  Use custom MAC so there's less to screw up with the testcase
uint8_t masterDeviceMac[] = {0xB4, 0xE6, 0x2D, 0xE9, 0xFE, 0x6E};

#endif
