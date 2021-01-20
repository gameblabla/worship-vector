#include "Processes.r"
#include "Retro68.r"

#if 0
/*
 * By default, Application CODE resources are no longer
 * created by Rez, but rather by Elf2Mac.
 * It is still possible to create a single-segment
 * application this way, though.
 */
resource 'CODE' (0) RETRO68_JUMP_TABLE;

resource 'CODE' (1) {
#ifdef BREAK_ON_ENTRY
	breakAtEntry,
#else
	dontBreakAtEntry,
#endif
	$$read(FLT_FILE_NAME);
};
#endif

resource 'SIZE' (-1) {
	reserved,
	ignoreSuspendResumeEvents,
	reserved,
	cannotBackground,
	needsActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	is32BitCompatible,
	notHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	1024 * 4096,
	1024 * 4096
};
