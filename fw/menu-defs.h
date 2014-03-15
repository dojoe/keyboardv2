#ifndef MENU_DEFS_H_
#define MENU_DEFS_H_

#define MENU_TIMEOUT_SECONDS		          30
#define MENU_TIMEOUT_SELECT_SECONDS	      15
#define PIZZA_TIMER_DEFAULT_TIME         300

enum menu_state {
	UIS_INACTIVE = 0,
	UIS_PIZZA1,
	UIS_PIZZA2,
	UIS_PIZZA3,
	UIS_SELECT_TIME,
};

#endif /* MENU_DEFS_H_ */
