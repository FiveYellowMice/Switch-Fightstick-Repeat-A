#define BTN_LEFT 3
#define BTN_RETURN 2
#define BTN_PLAY_PAUSE 1
#define BTN_RIGHT 0

#define BTN_STATE(btn) (buttons_state & (1 << (btn)))

extern uint8_t buttons_state;

void buttons_setup(void);
void buttons_debounce(void);
