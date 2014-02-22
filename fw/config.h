#ifndef CONFIG_H_
#define CONFIG_H_

#include "common.h"
#include "key.h"

struct config {
	struct kb_info  kb;
	struct key_info keys[MAX_KEYS];
};

extern struct config config;

void save_config(void);
void load_config(void);

#endif /* CONFIG_H_ */
