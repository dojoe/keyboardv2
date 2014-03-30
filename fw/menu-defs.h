#ifndef MENU_DEFS_H_
#define MENU_DEFS_H_

#define MENU_TIMEOUT_SECONDS		          30
#define MENU_TIMEOUT_SELECT_SECONDS	      15
#define PIZZA_TIMER_DEFAULT_TIME          5  /* minutes */
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
};

#endif /* MENU_DEFS_H_ */
