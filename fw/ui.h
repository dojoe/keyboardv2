#ifndef UI_H_
#define UI_H_

#define MENU_TIMEOUT_SECONDS            30
#define MENU_TIMEOUT_SELECT_SECONDS	    10
#define UI_MESSAGE_TIMEOUT_SECONDS      15

#define PIZZA_TIMER_DEFAULT_TIME           5 /* minutes */
#define PIZZA_TIMER_MAX_TIME              90 /* minutes */

enum menu_state {
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

void ui_init(void);
void ui_poll(void);

void ui_select_time(uint8_t timer_id, uint8_t default_time, uint8_t max_time);
void ui_to_idle(void);
void ui_message(uint8_t dest_state);

static inline void ui_short_message(void)
{
	ui_message(UIS_MESSAGE_TIMEOUT);
}

static inline void ui_error_message(void)
{
	ui_message(UIS_KEY_ERROR);
}

#endif /* MENU_H_ */
