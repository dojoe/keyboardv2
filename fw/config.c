#include <avr/eeprom.h>
#include "config.h"

struct config config;

struct config EEMEM config_eep = {
		.kb = { .id = 1, .name = "Lounge" },
		.keys = {
				{ 1, "Matelager", 10, 60, KF_BEEP }
		}
};

void save_config(void)
{
	eeprom_update_block(&config, &config_eep, sizeof(config));
}

void load_config(void)
{
	eeprom_read_block(&config, &config_eep, sizeof(config));
}
