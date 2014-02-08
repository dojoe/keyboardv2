#ifndef KEY_H_
#define KEY_H_

#include "hw.h"

enum key_flags {
	KF_ROTLIGHT = 1, /* Use rotating light if key missing */
	KF_BEEP     = 2, /* Beep if key times out */
};

#define NAME_LENGTH 16+1

struct key_eeprom_data {
	uint8_t  kb_id;
	uint8_t  key_id;
	uint8_t  dfl_timeout;
	uint8_t  max_timeout;
	uint8_t  flags;
	char     key_name[NAME_LENGTH];
	char     kb_name[NAME_LENGTH];
	uint16_t crc16;
} __attribute__((packed));

enum key_state {
	KS_EMPTY,
	KS_INVALID1,
	KS_INVALID2,
	KS_VALID
};

struct key_info {
	uint8_t state;
	struct key_eeprom_data eep;
};

extern struct key_info keys[MAX_KEYS];

void key_init(void);
void key_poll(void);

#endif /* KEY_H_ */
