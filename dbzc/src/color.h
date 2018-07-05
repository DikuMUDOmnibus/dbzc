/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2002 Alsherok. Contributors: Samson, Dwip, Whir,   *
 * Cyberfox, Karangi, Rathian, Cam, Raine, and Tarl.                        *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *               Color Module -- Allow user customizable Colors.            *
 *                                   --Matthew                              *
 *                      Enhanced ANSI parser by Samson                      *
 ****************************************************************************/
/******************************************************************
 *                [D]ragon [B]all [Z] [C]hronicles                *
 *----------------------------------------------------------------*
 * DBZB, DBZDL, DBZC versions done by: Seventeen and Vegeta       *
 *----------------------------------------------------------------*
 * All modifications made by Seventeen and Vegeta are sole        *
 * property of them. Credit must be given if any part is ripped   *
 * or used from this version or any other version of DBZC.        *
 *****************************************************************/

#define SAMSONCOLOR /* To interact with other snippets */

#ifndef MSL
   #define MSL MAX_STRING_LENGTH
#endif

#ifndef MIL
   #define MIL MAX_INPUT_LENGTH
#endif

DECLARE_DO_FUN( do_color );

void reset_colors( CHAR_DATA *ch );
void set_char_color( sh_int AType, CHAR_DATA *ch );
void set_pager_color( sh_int AType, CHAR_DATA *ch );
char *color_str( sh_int AType, CHAR_DATA *ch );
const char *const_color_align( const char *argument, int size, int align );

/*
 * Color Alignment Parameters
 */
#define ALIGN_LEFT	1
#define ALIGN_CENTER	2
#define ALIGN_RIGHT	3

/* These are the ANSI codes for foreground text colors */
#define ANSI_BLACK    	"\e[0;30m"
#define ANSI_DRED    	"\e[0;31m"
#define ANSI_DGREEN     "\e[0;32m"
#define ANSI_ORANGE    	"\e[0;33m"
#define ANSI_DBLUE    	"\e[0;34m"
#define ANSI_PURPLE    	"\e[0;35m"
#define ANSI_CYAN	  	"\e[0;36m"
#define ANSI_GREY		"\e[0;37m"
#define ANSI_DGREY	"\e[1;30m"
#define ANSI_RED		"\e[1;31m"
#define ANSI_GREEN	"\e[1;32m"
#define ANSI_YELLOW   	"\e[1;33m"
#define ANSI_BLUE		"\e[1;34m"
#define ANSI_PINK		"\e[1;35m"
#define ANSI_LBLUE   	"\e[1;36m"
#define ANSI_WHITE   	"\e[1;37m"
#define ANSI_RESET	"\e[0m"

/* These are the ANSI codes for blinking foreground text colors */
#define BLINK_BLACK		"\e[0;5;30m"
#define BLINK_DRED		"\e[0;5;31m"
#define BLINK_DGREEN		"\e[0;5;32m"
#define BLINK_ORANGE		"\e[0;5;33m"
#define BLINK_DBLUE		"\e[0;5;34m"
#define BLINK_PURPLE		"\e[0;5;35m"
#define BLINK_CYAN		"\e[0;5;36m"
#define BLINK_GREY		"\e[0;5;37m"
#define BLINK_DGREY		"\e[1;5;30m"
#define BLINK_RED			"\e[1;5;31m"
#define BLINK_GREEN		"\e[1;5;32m"
#define BLINK_YELLOW		"\e[1;5;33m"
#define BLINK_BLUE		"\e[1;5;34m"
#define BLINK_PINK		"\e[1;5;35m"
#define BLINK_LBLUE		"\e[1;5;36m"
#define BLINK_WHITE		"\e[1;5;37m"

/* These are the ANSI codes for background colors */
#define BACK_BLACK 	"\e[40m"
#define BACK_DRED  	"\e[41m"
#define BACK_DGREEN	"\e[42m"
#define BACK_ORANGE     "\e[43m"
#define BACK_DBLUE      "\e[44m"
#define BACK_PURPLE     "\e[45m"
#define BACK_CYAN       "\e[46m"
#define BACK_GREY       "\e[47m"

/* Other miscelaneous ANSI tags that can be used */
#define ANSI_BOLD		"\e[1m" /* For bright color stuff */
#define ANSI_ITALIC	"\e[3m" /* Italic text */
#define ANSI_UNDERLINE  "\e[4m" /* Underline text */
#define ANSI_BLINK	"\e[5m" /* Blinking text */
#define ANSI_REVERSE    "\e[7m" /* Reverse colors */
#define ANSI_STRIKEOUT  "\e[9m" /* Overstrike line */

#define AT_BLACK    	0
#define AT_BLOOD    	1
#define AT_DGREEN       2
#define AT_ORANGE    	3
#define AT_DBLUE    	4
#define AT_PURPLE    	5
#define AT_CYAN	  	6
#define AT_GREY		7
#define AT_DGREY		8
#define AT_RED		9
#define AT_GREEN		10
#define AT_YELLOW   	11
#define AT_BLUE		12
#define AT_PINK		13
#define AT_LBLUE   	14
#define AT_WHITE   	15
#define AT_BLINK   	16

#define AT_WHITE_BLINK	   AT_WHITE + AT_BLINK
#define AT_RED_BLINK	   AT_RED + AT_BLINK

#define AT_PLAIN		17
#define AT_ACTION		18
#define AT_SAY    		19
#define AT_GOSSIP		20
#define AT_YELL		21
#define AT_TELL		22
#define AT_HIT		23
#define AT_HITME		24
#define AT_IMMORT		25
#define AT_HURT		26
#define AT_FALLING	27
#define AT_DANGER		28
#define AT_MAGIC		29
#define AT_CONSIDER	30
#define AT_REPORT		31
#define AT_POISON		32
#define AT_SOCIAL		33
#define AT_DYING		34
#define AT_DEAD		35
#define AT_SKILL		36
#define AT_CARNAGE	37
#define AT_DAMAGE		38
#define AT_FLEE		39
#define AT_RMNAME		40
#define AT_RMDESC		41
#define AT_OBJECT		42
#define AT_PERSON		43
#define AT_LIST		44
#define AT_BYE		45
#define AT_GOLD		46
#define AT_GTELL		47
#define AT_NOTE		48
#define AT_HUNGRY		49
#define AT_THIRSTY	50
#define AT_FIRE		51
#define AT_SOBER		52
#define AT_WEAROFF	53
#define AT_EXITS		54
#define AT_SCORE		55
#define AT_RESET		56
#define AT_LOG		57
#define AT_DIEMSG		58
#define AT_WARTALK      59
#define AT_ARENA        60
#define AT_MUSE         61
#define AT_THINK        62
#define AT_AFLAGS      	63 /* Added by Samson 9-29-98 for area flag display line */
#define AT_WHO	    	64 /* Added by Samson 9-29-98 for wholist */
#define AT_RACETALK   	65 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_IGNORE     	66 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_WHISPER    	67 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_DIVIDER    	68 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_MORPH      	69 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_SHOUT		70 /* Added by Samson 9-29-98 for shout channel */
#define AT_RFLAGS		71 /* Added by Samson 12-20-98 for room flag display line */
#define AT_STYPE		72 /* Added by Samson 12-20-98 for sector display line */
#define AT_ANAME		73 /* Added by Samson 12-20-98 for filename display line */
#define AT_AUCTION      74 /* Added by Samson 12-25-98 for auction channel */
#define AT_SCORE2		75 /* Added by Samson 2-3-99 for DOTD code */
#define AT_SCORE3		76 /* Added by Samson 2-3-99 for DOTD code */
#define AT_SCORE4		77 /* Added by Samson 2-3-99 for DOTD code */
#define AT_WHO2		78 /* Added by Samson 2-3-99 for DOTD code */
#define AT_WHO3		79 /* Added by Samson 2-3-99 for DOTD code */
#define AT_WHO4		80 /* Added by Samson 2-3-99 for DOTD code */
#define AT_INTERMUD     81 /* Added by Samson 1-15-01 for Intermud3 Channels */
#define AT_HELP		82 /* Added by Samson 1-15-01 for helpfiles */
#define AT_WHO5		83 /* Added by Samson 2-7-01 for guild names on who */
#define AT_CORPSE       84
#define MAX_COLORS    85

extern const sh_int default_set [MAX_COLORS];
