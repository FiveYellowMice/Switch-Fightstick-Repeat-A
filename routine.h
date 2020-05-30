enum _Routine_Flag_Indicator_PlayPause {
	NONE,
	PAUSE,
	PLAY,
};

typedef struct {
	bool returnable : 1;
	bool indicator_return : 1;
	enum _Routine_Flag_Indicator_PlayPause indicator_play_pause : 2;
	bool indicator_left : 1;
	bool indicator_right : 1;
	struct _Routine *next_routine;
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

#define DEFINE_ROUTINE_MENU(handle, name, upper_level, menu_members_length) \
	const char PROGMEM routine_##handle##_name[] = name; \
	Routine routine_##handle = { \
		routine_##handle##_name, \
		&routine_##upper_level, \
		NULL, \
		routine_##handle##_members, \
		menu_members_length, \
		0, \
		false, \
	};

#define DEFINE_ROUTINE_FUNCTION(handle, name, upper_level) \
	const char PROGMEM routine_##handle##_name[] = name; \
	Routine routine_##handle = { \
		routine_##handle##_name, \
		&routine_##upper_level, \
		routine_##handle##_function, \
		NULL, \
		0, \
		0, \
		false, \
	};
