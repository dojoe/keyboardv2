#include <string.h>
#include <avr/eeprom.h>
#include "config.h"

struct config config;

struct config EEMEM config_eep = {{ 0 }};

void save_config(void)
{
	eeprom_update_block(&config, &config_eep, sizeof(config));
}

void load_config(void)
{
	eeprom_read_block(&config, &config_eep, sizeof(config));

	/* On a freshly erased and programmed device, clear the config */
	if (config.kb.name[0] == 0xFF)
		memset(&config, 0, sizeof(config));
}

int8_t find_key(uint8_t id)
{
	uint8_t i;

	for (i = 0; i < MAX_KEYS; i++)
		if (config.keys[i].id == id)
			return i;

	return -1;
}

