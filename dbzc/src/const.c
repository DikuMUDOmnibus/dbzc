/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 *			     Mud constants module			    *
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

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "mud.h"



/* undef these at EOF */
#define AM 95
#define AC 95
#define AT 85
#define AW 85
#define AV 95
#define AD 95
#define AR 90
#define AA 95

char *	const	npc_race	[MAX_NPC_RACE] =
{
"android", "biodroid", "saiyan", "halfbreed", "human", "namek", "icer", "mutant", 
"buu"
/*"elf", "dwarf", 
"halfling", "pixie", "vampire", "half-ogre",
"half-orc", "half-troll", "half-elf", "gith", "drow", "sea-elf",
"lizardman", "gnome", "r5", "r6", "r7", "r8", "troll",
"ant", "ape", "baboon", "bat", "bear", "bee",
"beetle", "boar", "bugbear", "cat", "dog", "dragon", "ferret", "fly",
"gargoyle", "gelatin", "ghoul", "gnoll", "gnome", "goblin", "golem",
"gorgon", "harpy", "hobgoblin", "kobold", "lizardman", "locust",
"lycanthrope", "minotaur", "mold", "mule", "neanderthal", "ooze", "orc",
"rat", "rustmonster", "shadow", "shapeshifter", "shrew", "shrieker",
"skeleton", "slime", "snake", "spider", "stirge", "thoul", "troglodyte",
"undead", "wight", "wolf", "worm", "zombie", "bovine", "canine", "feline",
"porcine", "mammal", "rodent", "avis", "reptile", "amphibian", "fish",
"crustacean", "insect", "spirit", "magical", "horse", "animal", "humanoid",
"monster", "god"*/
};


char *	const	npc_class	[MAX_NPC_CLASS] =
{
"fighter"
/*, "fighter", "fighter", "warrior", "vampire", "druid", "ranger",
"augurer", "paladin", "nephandi", "savage", "pirate", "pc12", "pc13",
"pc14", "pc15", "pc16", "pc17", "pc18", "pc19",
"baker", "butcher", "blacksmith", "mayor", "king", "queen"*/
};



/*
 * Attribute bonus tables.
 */
const   struct  racial_modifiers race_mod_table       [MAX_NPC_RACE]            =
{

    /* Accu / Evsn / Crit / Life / Gain / PDam / PDef / EDam / EDef / Load */
  {   15,    12,    11,     15,    12,    12,    13,    13,     14,  505    },  /* Android    */
  {   14,    13,    12,     16,    11,    13,    13,    14,     12,  510    },  /* Biodroid   */
  {   14,    12,    11,     13,    9,     14,    13,    13,     11,  501    },  /* Saiyan     */
  {   13,    13,    13,     17,    13,    13,    12,    13,     14,  507    },  /* Halfbreed  */
  {   14,    13,    15,     18,    13,    12,    11,    14,     13,  503    },  /* Human      */
  {   13,    13,    14,     18,    14,    12,    12,    14,     12,  508    },  /* Namek      */
  {   13,    14,    12,     16,    12,    14,    12,    13,     13,  502    },  /* Icer       */
  {   14,    12,    13,     15,    10,    13,    14,    13,     15,  504    },  /* Mutant     */
  {   11,    14,    12,     14,    11,    14,    12,    14,     13,  502    }   /* Buu        */

};

const	struct	str_app_type	str_app		[36]		=
{
    {  0,  0,   0,  0 },  /* 0  */
    {  1,  1,   3,  100 },  /* 1  */
    {  1,  1,   3,  200 },
    {  1,  1,  10,  300 },  /* 3  */
    {  1,  1,  25,  400 },
    {  1,  1,  55,  500 },  /* 5  */
    {  1,  1,  80,  2000 },
    {  1,  1,  90,  3000 },
    {  2,  2, 100,  4000 },
    {  2,  2, 100,  5000 },
    {  2,  2, 115,  6000 }, /* 10  */
    {  2,  2, 115, 7000 },
    {  2,  2, 140, 8000 },
    {  2,  2, 140, 8000 }, /* 13  */
    {  2,  2, 170, 8000 },
    {  3,  3, 170, 8000  }, /* 15  */
    {  3,  3, 195, 8000  },
    {  3,  3, 220, 8000  },
    {  3,  3, 250, 8000  }, /* 18  */
    {  3,  3, 400, 8000  },
    {  4,  4, 500, 8000  }, /* 20  */
    {  4,  4, 600, 8000  },
    {  4,  4, 700, 8000  },
    {  4,  4, 800, 8000  },
    {  4,  4, 900, 8000  },
    {  5,  5, 999, 8000  },  /* 25   */
    {  5,  5, 999, 8000  },  /* 26   */
    {  5,  5, 999, 8000  },  /* 27   */
    {  5,  5, 999, 8000  },  /* 28   */
    {  5,  5, 999, 8000  },  /* 29   */
    {  5,  5, 999, 8000  },  /* 30   */
    {  6,  6, 999, 8000  },  /* 31   */
    {  6,  6, 999, 8000  },  /* 32   */
    {  6,  6, 999, 8000  },  /* 33   */
    {  6,  6, 999, 8000  },  /* 34   */
    {  7,  7, 999, 8000  }  /* 35   */
};


const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 5 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 7 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};


const	struct	cha_app_type	cha_app		[26]		=
{
    { - 60 },   /* 0 */
    { - 50 },   /* 1 */
    { - 50 },
    { - 40 },
    { - 30 },
    { - 20 },   /* 5 */
    { - 10 },
    { -  5 },
    { -  1 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    1 },
    {    5 },   /* 15 */
    {   10 },
    {   20 },
    {   30 },
    {   40 },
    {   50 },   /* 20 */
    {   60 },
    {   70 },
    {   80 },
    {   90 },
    {   99 }    /* 25 */
};

/* Have to fix this up - not exactly sure how it works (Scryn) */
const	struct	lck_app_type	lck_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};

/*
const   struct  coord_struct    coords          [MAX_COORDINATES] =
{
 { "gate1",	    "spaceport_gate1",	   7, 12,  5,  1     },
 { "gate2",         "spaceport_gate2",     7, 12,  5,  2     },
 { "gate3",         "spaceport_gate3",     7, 12,  5,  3     },
 { "gate4",         "spaceport_gate4",     7, 12,  5,  4     },
 { "gate5",         "spaceport_gate5",     7, 12,  5,  5     },
 { "gate6",         "spaceport_gate6",     7, 12,  5,  6     },
 { "gate7",         "spaceport_gate7",     7, 12,  5,  7     },
 { "gate8",         "spaceport_gate8",     7, 12,  5,  8     },
 { "gate9",         "spaceport_gate9",     7, 12,  5,  9     },
 { "gate10",        "spaceport_gate10",    7, 12,  5, 10     },
 { "gate11",        "spaceport_gate11",    7, 12,  5, 11     },
 { "gate12",        "personal_docks",      7, 12,  5, 12     },
 { "Evil Chikara",  "evil_chikara",       22, 20, 10,  0     },
 { "Good Chikara",  "good_chikara",       32, 19, 15,  0     },
 { "Planet Vegeta", "planet_vegeta",      30, 10,  2,  0     },
 { "Planet Mutant", "planet_mutant",       5, 30, 20,  0     },
 { "Planet Namek",  "planet_namek",       20, 20, 40,  0     },
 { "Planet Earth",  "planet_earth",       15, 25, 40,  0     },
 { "Planet Freezar","planet_freezar",     40, 15,  5,  0     },
 { "Ghetti",        "planet_ghetti",      10, 40,  5,  0     },
 { "Droid Station", "android_station",    15, 40, 10,  0     },
 { "PK Asteroid",   "asteroid",            5, 25, 10,  0     },
 { "Cobra Sect HQ", "cs_hq",              12, 15,  9,  0     },
 { "Fox Hunters SS","fh_ss",              33, 29, 18,  0     },
 { "Fox Hunters SD","fh_sd",               7, 25, 25,  0     },
 { "Majin HQ",      "mhq",		   6,  6,  6,  6     },
 { "Turk HQ",       "turk_hq",             6, 15,  3,  0     },
 { "Chaos Knights HQ",   "ck_hq",         13,  8, 33,  0     },
 { "The Council HQ",     "tc_hq",          8, 13, 33,  0     },
 { "Faded Dragons HQ",   "fd_hq",          4,  9, 14,  0     },
 { "Z Fighters HQ",      "zf_hq",         18,  9,  9,  5     },
 { "Majin Dynasty HQ",   "md_hq",          0,  4,  2,  0     }

};
*/

/*
 * Liquid properties.
 * Used in #OBJECT section of area file.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			"clear",	{  0, 1, 10 }	},  /*  0 */
    { "beer",			"amber",	{  3, 2,  5 }	},
    { "wine",			"rose",		{  5, 2,  5 }	},
    { "ale",			"brown",	{  2, 2,  5 }	},
    { "dark ale",		"dark",		{  1, 2,  5 }	},

    { "whisky",			"golden",	{  6, 1,  4 }	},  /*  5 */
    { "lemonade",		"pink",		{  0, 1,  8 }	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }	},
    { "local specialty",	"everclear",	{  3, 3,  3 }	},
    { "slime mold juice",	"green",	{  0, 4, -8 }	},

    { "milk",			"white",	{  0, 3,  6 }	},  /* 10 */
    { "tea",			"tan",		{  0, 1,  6 }	},
    { "coffee",			"black",	{  0, 1,  6 }	},
    { "blood",			"red",		{  0, 2, -1 }	},
    { "salt water",		"clear",	{  0, 1, -2 }	},

    { "cola",			"cherry",	{  0, 1,  5 }	},  /* 15 */
    { "mead",			"honey color",	{  4, 2,  5 }	},  /* 16 */
    { "grog",			"thick brown",	{  3, 2,  5 }	}   /* 17 */
};


char *	const	attack_table	[18] =
{
    "hit",
    "slice",  "slash",   "slice", "slash",  "crush",
    "blast",  "pound",   "crush", "grep",  "bite",
    "pierce", "suction", "bolt",  "arrow", "dart",
    "stone",  "pea"
};

char * s_blade_messages[24] =
{
	"miss", "barely scratch", "scratch", "nick", "cut", "hit", "tear",
	"rip", "gash", "lacerate", "hack", "maul", "rend", "decimate",
	"_mangle_", "_devastate_", "_cleave_", "_butcher_", "DISEMBOWEL",
	"DISFIGURE", "GUT", "EVISCERATE", "* SLAUGHTER *", "*** ANNIHILATE ***"
};

char * p_blade_messages[24] =
{
	"misses", "barely scratches", "scratches", "nicks", "cuts", "hits",
	"tears", "rips", "gashes", "lacerates", "hacks", "mauls", "rends",
	"decimates", "_mangles_", "_devastates_", "_cleaves_", "_butchers_",
	"DISEMBOWELS", "DISFIGURES", "GUTS", "EVISCERATES", "* SLAUGHTERS *",
	"*** ANNIHILATES ***"
};

char * s_blunt_messages[24] =
{
	"miss", "barely scuff", "scuff", "pelt", "bruise", "strike", "thrash",
	"batter", "flog", "pummel", "smash", "maul", "bludgeon", "decimate",
	"_shatter_", "_devastate_", "_maim_", "_cripple_", "MUTILATE", "DISFIGURE",
	"MASSACRE", "PULVERIZE", "* OBLITERATE *", "*** ANNIHILATE ***"
};

char * p_blunt_messages[24] =
{
	"misses", "barely scuffs", "scuffs", "pelts", "bruises", "strikes",
	"thrashes", "batters", "flogs", "pummels", "smashes", "mauls",
	"bludgeons", "decimates", "_shatters_", "_devastates_", "_maims_",
	"_cripples_", "MUTILATES", "DISFIGURES", "MASSACRES", "PULVERIZES",
	"* OBLITERATES *", "*** ANNIHILATES ***"
};

char * s_generic_messages[24] =
{
	"miss", "scratch", "scratch", "graze", "nick", "jolt", "wound",
	"injure", "hit", "jar", "thrash", "maul", "decimate", "_traumatize_",
	"_devastate_", "_maim_", "_demolish_", "MUTILATE", "MASSACRE",
	"PULVERIZE", "DESTROY", "* OBLITERATE *", "*** ANNIHILATE ***",
	"**** SMITE ****"
};

char * p_generic_messages[24] =
{
	"misses", "scratches", "scratches", "grazes", "nicks", "jolts", "wounds",
	"injures", "hits", "jars", "thrashes", "mauls", "decimates", "_traumatizes_",
	"_devastates_", "_maims_", "_demolishes_", "MUTILATES", "MASSACRES",
	"PULVERIZES", "DESTROYS", "* OBLITERATES *", "*** ANNIHILATES ***",
	"**** SMITES ****"
};

char ** const s_message_table[18] =
{
	s_generic_messages,	/* hit */
	s_blade_messages,	/* slice */
	s_blade_messages,	/* stab */
	s_blade_messages,	/* slash */
	s_blunt_messages,	/* whip */
	s_blade_messages,	/* claw */
	s_generic_messages,	/* blast */
	s_blunt_messages,	/* pound */
	s_blunt_messages,	/* crush */
	s_generic_messages,	/* grep */
	s_blade_messages,	/* bite */
	s_blade_messages,	/* pierce */
	s_blunt_messages,	/* suction */
	s_generic_messages,	/* bolt */
	s_generic_messages,	/* arrow */
	s_generic_messages,	/* dart */
	s_generic_messages,	/* stone */
	s_generic_messages,     /* pea */
};

char ** const p_message_table[18] =
{
	p_generic_messages,	/* hit */
	p_blade_messages,	/* slice */
	p_blade_messages,	/* stab */
	p_blade_messages,	/* slash */
	p_blunt_messages,	/* whip */
	p_blade_messages,	/* claw */
	p_generic_messages,	/* blast */
	p_blunt_messages,	/* pound */
	p_blunt_messages,	/* crush */
	p_generic_messages,	/* grep */
	p_blade_messages,	/* bite */
	p_blade_messages,	/* pierce */
	p_blunt_messages,	/* suction */
	p_generic_messages,	/* bolt */
	p_generic_messages,	/* arrow */
	p_generic_messages,	/* dart */
	p_generic_messages,	/* stone */
	p_generic_messages,	/* pea */
};

/* Weather constants - FB */
char * const temp_settings[MAX_CLIMATE] =
{
	"cold",
	"cool",
	"normal",
	"warm",
	"hot",
};

char * const precip_settings[MAX_CLIMATE] =
{
	"arid",
	"dry",
	"normal",
	"damp",
	"wet",
};

char * const wind_settings[MAX_CLIMATE] =
{
	"still",
	"calm",
	"normal",
	"breezy",
	"windy",
};

char * const preciptemp_msg[6][6] =
{
	/* precip = 0 */
	{
		"Frigid temperatures settle over the land",
		"It is bitterly cold",
		"The weather is crisp and dry",
		"A comfortable warmth sets in",
		"A dry heat warms the land",
		"Seething heat bakes the land"
	 },
	 /* precip = 1 */
	 {
	 	"A few flurries drift from the high clouds",
	 	"Frozen drops of rain fall from the sky",
	 	"An occasional raindrop falls to the ground",
	 	"Mild drops of rain seep from the clouds",
	 	"It is very warm, and the sky is overcast",
	 	"High humidity intensifies the seering heat"
	 },
	 /* precip = 2 */
	 {
	 	"A brief snow squall dusts the earth",
	 	"A light flurry dusts the ground",
	 	"Light snow drifts down from the heavens",
	 	"A light drizzle mars an otherwise perfect day",
	 	"A few drops of rain fall to the warm ground",
	 	"A light rain falls through the sweltering sky"
	 },
	 /* precip = 3 */
	 {
	 	"Snowfall covers the frigid earth",
	 	"Light snow falls to the ground",
	 	"A brief shower moistens the crisp air",
	 	"A pleasant rain falls from the heavens",
	 	"The warm air is heavy with rain",
	 	"A refreshing shower eases the oppresive heat"
	 },
	 /* precip = 4 */
	 {
	 	"Sleet falls in sheets through the frosty air",
	 	"Snow falls quickly, piling upon the cold earth",
	 	"Rain pelts the ground on this crisp day",
	 	"Rain drums the ground rythmically",
	 	"A warm rain drums the ground loudly",
	 	"Tropical rain showers pelt the seering ground"
	 },
	 /* precip = 5 */
	 {
	 	"A downpour of frozen rain covers the land in ice",
	 	"A blizzard blankets everything in pristine white",
	 	"Torrents of rain fall from a cool sky",
	 	"A drenching downpour obscures the temperate day",
	 	"Warm rain pours from the sky",
	 	"A torrent of rain soaks the heated earth"
	 }
};

char * const windtemp_msg[6][6] =
{
	/* wind = 0 */
	{
		"The frigid air is completely still",
		"A cold temperature hangs over the area",
		"The crisp air is eerily calm",
		"The warm air is still",
		"No wind makes the day uncomfortably warm",
		"The stagnant heat is sweltering"
	},
	/* wind = 1 */
	{
		"A light breeze makes the frigid air seem colder",
		"A stirring of the air intensifies the cold",
		"A touch of wind makes the day cool",
		"It is a temperate day, with a slight breeze",
		"It is very warm, the air stirs slightly",
		"A faint breeze stirs the feverish air"
	},
	/* wind = 2 */
	{
		"A breeze gives the frigid air bite",
		"A breeze swirls the cold air",
		"A lively breeze cools the area",
		"It is a temperate day, with a pleasant breeze",
		"Very warm breezes buffet the area",
		"A breeze ciculates the sweltering air"
	},
	/* wind = 3 */
	{
		"Stiff gusts add cold to the frigid air",
		"The cold air is agitated by gusts of wind",
		"Wind blows in from the north, cooling the area",
		"Gusty winds mix the temperate air",
		"Brief gusts of wind punctuate the warm day",
		"Wind attempts to cut the sweltering heat"
	},
	/* wind = 4 */
	{
		"The frigid air whirls in gusts of wind",
		"A strong, cold wind blows in from the north",
		"Strong wind makes the cool air nip",
		"It is a pleasant day, with gusty winds",
		"Warm, gusty winds move through the area",
		"Blustering winds punctuate the seering heat"
	},
	/* wind = 5 */
	{
		"A frigid gale sets bones shivering",
		"Howling gusts of wind cut the cold air",
		"An angry wind whips the air into a frenzy",
		"Fierce winds tear through the tepid air",
		"Gale-like winds whip up the warm air",
		"Monsoon winds tear the feverish air"
	}
};

char * const precip_msg[3] =
{
	"there is not a cloud in the sky",
	"pristine white clouds are in the sky",
	"thick, grey clouds mask the sun"
};

char * const wind_msg[6] =
{
	"there is not a breath of wind in the air",
	"a slight breeze stirs the air",
	"a breeze wafts through the area",
	"brief gusts of wind punctuate the air",
	"angry gusts of wind blow",
	"howling winds whip the air into a frenzy"
};

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n
#define LI LEVEL_IMMORTAL

#undef AM 
#undef AC 
#undef AT 
#undef AW 
#undef AV 
#undef AD 
#undef AR
#undef AA

#undef LI
