enum _Routine_Flag_Indicator_PlayPause {
	NONE,
	PAUSE,
	PLAY,
};

typedef struct {
	bool returnable : 1;
	bool indicator_return : 1;
	bool indicator_usb : 1;
	enum _Routine_Flag_Indicator_PlayPause indicator_play_pause : 2;
	bool indicator_left : 1;
	bool indicator_right : 1;
} Routine_Flags;

typedef struct _Routine {
	const char *name;
	struct _Routine *upper_level;
	void (*function)(Routine_Flags *);
	struct _Routine **menu_members;
	size_t menu_members_length;
	uint8_t menu_current_index;
	bool menu_lower_selected;
} Routine;
