#ifndef UI_H_
#define UI_H_

void ui_init(void);
void ui_poll(void);

void ui_select_time(uint8_t timer_id, int16_t default_time, int16_t max_time);
void ui_to_idle(void);

#endif /* MENU_H_ */
