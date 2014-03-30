#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>

#include "common.h"
#include "cmd.h"
#include "key.h"
#include "config.h"

static uint8_t busy = 0;

static void help(char *argv[])
{
	printf_P(PSTR("\n\
          == H A L P ==\n\
\n\
show_keys\n\
   Show currently plugged keys\n\
show_config\n\
   Print configuration (keyboard ID, expected keys) in a format that can be\n\
   directly fed back into the CLI\n\
set_keyboard <ID> <Name...>\n\
   Set keyboard ID and name. Set this up before programming keys!\n\
add_key <ID> <dfl timeout> <max timeout> <flags> <Name...>\n\
   Add key to list of expected keys. Syntax like \"program key\".\n\
   If a key with the same ID is already in the list, that key is replaced.\n\
del_key <ID>\n\
   Remove key from list of expected keys\n\
clear_keys\n\
   Clear list of expected keys\n\
capture_keys\n\
   Clear list of expected keys and replace with all currently plugged keys\n\
program_key <position> <ID> <dfl timeout> <max timeout> <flags> <Name...>\n\
   Program key in position <position>, indexed 1.." __STRINGIFY__(MAX_KEYS) " from left to right.\n\
   This does not add the key to the list of expected keys!\n\
   dfl timeout - default timeout when key is removed, in minutes (1..255)\n\
                 Specify 0 here to disable timeout.\n\
   max timeout - maximum timeout that can be set, in minutes (1..255)\n\
   flags - a combination of any of these letters:\n\
     B - Missing key causes keyboard to beep after timeout\n\
     R - Missing key causes rotating light to turn on occasionally\n\
beeper on|off\n\
   Enable or disable the beeper, so it doesn't annoy you while you program keys\n\
boot\n\
   Jump into DFU bootloader for firmware update\n\
\n\
Common parameter types:\n\
   ID   - decimal 1-255, zero is reserved\n\
   Name - ASCII, max 16 chars, may contain spaces\n\
          No quotes necessary, will just take rest of line\n"));
}

static void ok(void)
{
	printf_P(PSTR("OK\n"));
}

static uint8_t check_kb_setup(void)
{
	if (!config.kb.id)
		printf_P(PSTR("Keyboard ID not set. Use set_keyboard first.\n"));
	return config.kb.id;
}

static void boot(char *argv[])
{
	call_bootloader();
}

static void beeper(char *argv[])
{
	if (!strcmp_P(argv[1], PSTR("off"))) {
		beeper_enable(0);
		ok();
	} else if (!strcmp_P(argv[1], PSTR("on"))) {
		beeper_enable(1);
		ok();
	} else {
		printf_P(PSTR("What?\n"));
	}
}

static void show_keys(char *argv[])
{
	uint8_t i;
	for (i = 0; i < MAX_KEYS; i++) {
		printf_P(PSTR("Position %d: "), i + 1);
		if (keys[i].state == KS_UNKNOWN) {
			printf_P(PSTR("ZOMG BUG!!!1one\n"));
		} else if (keys[i].state == KS_EMPTY) {
			printf_P(PSTR("No key plugged\n"));
		} else if (keys[i].state == KS_READ_ERROR) {
			printf_P(PSTR("Read error\n"));
		} else if (keys[i].state == KS_CRC_ERROR) {
			printf_P(PSTR("Bad checksum\n"));
		} else {
			printf_P(PSTR("ID %d (%s), timeout %d (max %d)%S%S"),
					keys[i].eep.key.id, keys[i].eep.key.name,
					keys[i].eep.key.dfl_timeout, keys[i].eep.key.max_timeout,
					(keys[i].eep.key.flags & KF_BEEP) ? PSTR(", beep when gone") : PSTR(""),
					(keys[i].eep.key.flags & KF_ROTLIGHT) ? PSTR(", rotate light when gone") : PSTR(""));
			if (keys[i].eep.kb.id == config.kb.id)
				printf_P(PSTR("\n"));
			else
				printf_P(PSTR(", belongs to keyboard ID %d (%s)\n"),
						keys[i].eep.kb.id, keys[i].eep.kb.name);
		}
	}
}

static uint8_t parse_key_args(char *argv[], uint8_t argi, struct key_info *data)
{
	data->id = atoi(argv[argi++]);
	data->dfl_timeout = atoi(argv[argi++]);
	data->max_timeout = atoi(argv[argi++]);
	data->flags = (strchr(argv[argi], 'B') ? KF_BEEP : 0) | (strchr(argv[argi], 'R') ? KF_ROTLIGHT : 0);
	argi++;
	strncpy(data->name, argv[argi], NAME_LENGTH);

	return (data->id != 0);
}

static void program_key_cb(uint8_t status)
{
	busy = 0;

	switch (status) {
	case KS_VALID:
		ok();
		break;
	case KS_EMPTY:
		printf_P(PSTR("Could not program: No key plugged\n"));
		break;
	case KS_READ_ERROR:
		printf_P(PSTR("Could not program: Transmission failed\n"));
		break;
	}
}

static void program_key(char *argv[])
{
	struct key_eeprom_data data;
	uint8_t slot = atoi(argv[1]);

	if (!check_kb_setup())
		return;

	if (!slot || slot > MAX_KEYS) {
		printf_P(PSTR("Invalid key position\n"));
		return;
	}

	memset(&data, 0, sizeof(data));
	if (!parse_key_args(argv, 2, &data.key)) {
		printf_P(PSTR("Bad key data specified\n"));
		return;
	}

	data.kb = config.kb;
	busy = 1;
	key_program(slot - 1, &data, program_key_cb);

	/* Success/fail message will come from callback */
}

static void add_key(char *argv[])
{
	struct key_info data;
	int8_t config_slot;

	memset(&data, 0, sizeof(data));
	if (!parse_key_args(argv, 1, &data)) {
		printf_P(PSTR("Bad key data specified\n"));
		return;
	}

	config_slot = find_key(data.id);
	if (config_slot < 0) {
		config_slot = find_key(0);
		if (config_slot < 0) {
			printf_P(PSTR("No free slot in key database, delete another key first\n"));
			return;
		}
	}

	config.keys[config_slot] = data;
	save_config();

	ok();
}

static void del_key(char *argv[])
{
	uint8_t id = atoi(argv[1]);
	int8_t config_slot;

	if (!id) {
		printf_P(PSTR("Invalid key ID\n"));
		return;
	}

	config_slot = find_key(id);
	if (config_slot < 0) {
		printf_P(PSTR("Key not found in database\n"));
		return;
	}

	memset(&config.keys[config_slot], 0, sizeof(*config.keys));
	save_config();

	ok();
}

static void clear_keys(char *argv[])
{
	memset(config.keys, 0, sizeof(config.keys));
	save_config();
	ok();
}

static void capture_keys(char *argv[])
{
	uint8_t slot, i;

	if (!check_kb_setup())
		return;

	for (slot = 0; slot < MAX_KEYS; slot++) {
		if (!(
				(keys[slot].state == KS_EMPTY) ||
				((keys[slot].state == KS_VALID) && (keys[slot].eep.kb.id == config.kb.id))
			)) {
			printf_P(PSTR("Key in position %d is not valid\n"), slot + 1);
			return;
		}
		for (i = 0; i < slot; i++) {
			if (keys[i].state == KS_VALID && keys[slot].state == KS_VALID &&
					keys[i].eep.key.id == keys[slot].eep.key.id) {
				printf_P(PSTR("Duplicate key ID in positions %d and %d\n"), i + 1, slot + 1);
				return;
			}
		}
	}

	memset(config.keys, 0, sizeof(config.keys));
	for (slot = 0; slot < MAX_KEYS; slot++) {
		if (keys[slot].state == KS_VALID)
			config.keys[slot] = keys[slot].eep.key;
	}
	save_config();

	ok();
}

static void set_keyboard(char *argv[])
{
	config.kb.id = atoi(argv[1]);
	memset(config.kb.name, 0, sizeof(config.kb.name));
	strncpy(config.kb.name, argv[2], NAME_LENGTH);
	save_config();
	ok();
}

static void show_config(char *argv[])
{
	int i;
	struct key_info *k;

	if (!check_kb_setup())
		return;

	printf_P(PSTR("# Keyboard v2 config dump\n"
		   "set_keyboard %d %s\n"
		   "clear_keys\n"),
		   config.kb.id, config.kb.name);

	for (i = 0, k = config.keys; i < MAX_KEYS; i++, k++) {
		if (!k->id)
			continue;
		printf_P(PSTR("add_key %d %d %d %s%s %s\n"),
			   k->id, k->dfl_timeout, k->max_timeout, (k->flags & KF_BEEP) ? "B" : "",
			   (k->flags & KF_ROTLIGHT) ? "R" : "", k->name);
	}

	printf_P(PSTR("# END Keyboard v2 config dump\n"));
}

#define CMD_MAX 12+1
#define MAX_ARGC 10

typedef void (*handler_t)(char *argv[]);

struct cmd_def {
	char cmd[CMD_MAX];
	handler_t handler;
	uint8_t argc;
};

static const PROGMEM struct cmd_def commands[] = {
		{ "help",         help, 0 },
		{ "?",            help, 0 },
		{ "boot",         boot, 0 },
		{ "beeper",       beeper, 1 },
		{ "show_keys",    show_keys, 0 },
		{ "show_config",  show_config, 0 },
		{ "set_keyboard", set_keyboard, 2 },
		{ "add_key",      add_key, 5 },
		{ "del_key",      del_key, 1 },
		{ "clear_keys",   clear_keys, 0 },
		{ "capture_keys", capture_keys, 0 },
		{ "program_key",  program_key, 6 },
};

void handle_command(char *cmd)
{
	char *tmp, *argv[MAX_ARGC];
	uint8_t i, j, argc = 0;
	handler_t handler;

	if (busy) {
		printf_P(PSTR("Busy, try again.\n"));
		return;
	}

	/* Strip comments */
	tmp = strchr(cmd, '#');
	if (tmp)
		*tmp = 0;

	/* split off command itself */
	argv[0] = strtok_r(cmd, " ", &tmp);

	/* Identify command */
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp_P(argv[0], commands[i].cmd))
			continue;

		/* Split off arguments, use remainder of line for last argument */
		argc = pgm_read_byte(&commands[i].argc) + 1;
		for (j = 1; j < argc; j++) {
			argv[j] = strtok_r(NULL, (j == argc - 1) ? "" : " ", &tmp);
			if (!argv[j])
				goto error;
		}

		/* Matched command, run handler and return */
		handler = (handler_t)pgm_read_word(&commands[i].handler);
		if (!handler)
			goto error;
		handler(argv);

		return;
	}

error:
	printf_P(PSTR("What?\n"));
}
