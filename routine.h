typedef struct _Routine {
	const char *name;
	struct _Routine *upper_level;
	void (*function)(void);
	struct _Routine **menu_members;
	size_t menu_members_length;
	uint8_t menu_current_index;
	bool menu_lower_selected;
} Routine;
