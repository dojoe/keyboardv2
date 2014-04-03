#ifndef CONFIG_H_
#define CONFIG_H_

#include "common.h"
#include "key.h"

struct config {
	struct kb_info  kb;
	struct key_info keys[MAX_KEYS];
};

extern struct config config;
extern uint8_t config_changed;

void save_config(void);
void load_config(void);

/**
 * Search for a key with ID id in the configuration
 * Return index if found, negative if not found.
 * You can search for ID 0 to look for empty slots in the config.
 */
int8_t find_key(uint8_t id);

#endif /* CONFIG_H_ */
