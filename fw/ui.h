#ifndef UI_H_
#define UI_H_

#define MENU_TIMEOUT_SECONDS            15
#define UI_MESSAGE_TIMEOUT_SECONDS      15

#define PIZZA_TIMER_DEFAULT_TIME           5 /* minutes */
#define PIZZA_TIMER_MAX_TIME              90 /* minutes */

enum ui_state {
	UIS_IDLE = 0,
	UIS_MENU_FIND_KEY,
	UIS_MENU_PIZZA1,
	UIS_MENU_PIZZA2,
	UIS_MENU_PIZZA3,
	UIS_MENU_BOOTLOADER,
	UIS_SELECT_TIME,
	UIS_FIND_KEY,
	UIS_KEY_ERROR,
	UIS_MESSAGE_TIMEOUT,
};

enum ui_flags {
	UIF_TIMER_EXPIRED      = 1,
	UIF_KEY_ERROR_OTHER_KB = 2,
	UIF_KEY_ERROR_READ_ERR = 4,
	UIF_KEY_ERROR_UNKNOWN  = 8,
	UIF_KEY_ERROR          = UIF_KEY_ERROR_OTHER_KB | UIF_KEY_ERROR_READ_ERR | UIF_KEY_ERROR_UNKNOWN,
};

extern uint8_t ui_flags;

void ui_init(void);
void ui_poll(void);

void ui_select_time(uint8_t timer_id, uint8_t default_time, uint8_t max_time);
void ui_message(uint8_t dest_state);

void ui_set_timer_expired(uint8_t timer_idx);
void ui_clear_timer_expired(void);

void ui_set_key_error(uint8_t error_type, uint8_t slot_idx);
void ui_clear_key_error(void);

static inline void ui_short_message(void)
{
	ui_message(UIS_MESSAGE_TIMEOUT);
}

static inline void ui_error_message(void)
{
	ui_message(UIS_KEY_ERROR);
}

#endif /* MENU_H_ */
