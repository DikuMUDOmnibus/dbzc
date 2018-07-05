/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.0 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
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
 *		       Online Building and Editing Module		    *
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef CYGWIN
#include <crypt.h>
#endif
#include "mud.h"



char *sprint_reset( RESET_DATA *pReset, short *num );

extern int	top_affect;
extern int	top_reset;
extern int	top_ed;
extern bool	fBootDb;
void fix_exits( void );
REL_DATA *first_relation = NULL;
REL_DATA *last_relation  = NULL;
bool validate_spec_fun( char *name );

/*
 * Exit Pull/push types
 * (water, air, earth, fire)
 */
char *	const	ex_pmisc [] =
{ "undefined", "vortex", "vacuum", "slip", "ice", "mysterious" };

char *	const	ex_pwater [] =
{ "current", "wave", "whirlpool", "geyser" };

char *	const	ex_pair [] =
{ "wind", "storm", "coldwind", "breeze" };

char *	const	ex_pearth [] =
{ "landslide", "sinkhole", "quicksand", "earthquake" };

char *	const	ex_pfire [] =
{ "lava", "hotair" };


char *  const   ex_flags [] = 
{ 
"isdoor", "closed", "locked", "secret", "swim", "pickproof", "fly", "climb",
"dig", "eatkey", "nopassdoor", "hidden", "passage", "portal", "r1", "r2",
"can_climb", "can_enter", "can_leave", "auto", "noflee", "searchable", 
"bashed", "bashproof", "nomob", "window", "can_look", "isbolt", "bolted", "overland",
"keypad"
};

char *  const   sec_flags [] =
{
 "none", "inside", "city", "field", "forest", "hills", "mountain", "underwater", "air",
"desert", "oceanfloor", "underground", "lava", "swamp", "dark", "ice", "outside", 
 "r1", "r2", "r3", "r4", "r5", "r6", "r7",
"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16"
};

char *	const	r_flags	[] =
{
"dark", "death", "nomob", "indoors", "lawful", "neutral", "chaotic",
"nomagic", "tunnel", "private", "safe", "solitary", "petshop", "norecall",
"donation", "nodropall", "silence", "logspeech", "nodrop", "clanstoreroom",
"nosummon", "noastral", "teleport", "teleshowdesc", "nofloor",
"nosupplicate", "arena", "map", "r4", "r5", "prototype", "stadium"
};

char * const	r_flags2 [] =
{
"heal", "spar", "gravity", "control", "bank", "blackjack", "train", "noquit", "nosnipe", "nosense",
"empty_home", "plr_home", "noexit", "koth", "house", "propa", "pkill", "ugravity", "ctf", "library",
"slots", "claimable", "koth2", "freepk", "r12"
};

char *	const	o_flags	[] =
{
"glow", "hum", "spaceship", "loyal", "evil", "invis", "magic", "nodrop", "bless",
"antigood", "antievil", "antineutral", "noremove", "inventory",
"antimage", "antithief", "antiwarrior", "anticleric", "organic", "metal",
"donation", "clanobject", "clancorpse", "antivampire", "antidruid", 
"hidden", "poisoned", "covering", "deathrot", "buried", "prototype", 
"nolocate", "groundrot", "lootable", "nojunk", "cooler", "nosac", "follow", "onmap" };

char *	const	mag_flags	[] =
{
"returning", "backstabber", "bane", "loyal", "haste", "drain", 
"lightning_blade" 
};

char *	const	w_flags	[] =
{
"take", "finger", "neck", "body", "head", "legs", "feet", "hands", "arms",
"shield", "about", "waist", "wrist", "wield", "hold", "_dual_", "ears", "eyes",
"missile", "back", "face", "ankle", "slot1", "slot2", "r4","r5","r6",
"r7","r8","r9","r10","r11","r12","r13"
};

char *	const	item_w_flags	[] =
{
"take", "finger", "neck", "neck", "neck",  "body", "head", "legs", "feet",
"hands", "arms", "shield", "about", "waist", "wrist", "wrist", "wield",
"hold", "dual", "ears", "eyes", "missile", "back", "face", "ankle", "ankle", "slot1", "slot2"
"r4","r5","r6","r7","r8","r9","r10","r11","r12","r13"
};

char *	const	area_flags	[] =
{
"nopkill", "freekill", "noteleport", "spelllimit", "noarena", "dragon","planet",
"pkill", "prototype", "r5", "r6", 
"r7", "r8",
"r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17",
"r18", "r19","r20","r21","r22","r23","r24",
"r25","r26","r27","r28","r29","r30","r31"
};

char * const	raceskills_name [] =
{
"android", "biodroid", "saiyan", "halfbreed", "human",
"namek", "icer", "mutant", "buu"

};


char *	const	o_types	[] =
{
"none", "light", "scroll", "wand", "staff", "weapon", "_fireweapon", "_missile",
"treasure", "armor", "potion", "_worn", "furniture", "trash", "_oldtrap",
"container", "_note", "drinkcon", "key", "food", "money", "pen", "boat",
"corpse", "corpse_pc", "fountain", "pill", "blood", "bloodstain",
"scraps", "pipe", "herbcon", "herb", "incense", "fire", "book", "switch",
"lever", "pullchain", "button", "dial", "rune", "runepouch", "match", "trap", "map",
"portal", "paper", "tinder", "lockpick", "spike", "disease", "oil",
"fuel", "piece", "_empty2", "missileweapon", "projectile", "quiver", "shovel",
"salve", "cook", "keyring", "odor", "sensu", "scouter", "chip", "spaceship", "dragonball",
"building", "debris", "capsule", "dragonball2", "radar", "jewelry","chance"
};

char *  const   exit_symbols [] =
{
  "none", "shop", "spaceport", "gravity", "heal", "spar", "transporter", "house", "building", "battle",
  "bank", "train"
};


char *	const	a_types	[] =
{
"none", "strength", "dexterity", "intelligence", "wisdom", "constitution",
"sex", "class", "level", "age", "height", "weight", "mana", "hit", "move",
"gold", "experience", "armor", "hitroll", "damroll", "save_poison", "save_rod",
"save_para", "save_breath", "save_spell", "charisma", "affected", "resistant",
"immune", "susceptible", "weaponspell", "luck", "backstab", "pick", "track",
"steal", "sneak", "hide", "palm", "detrap", "dodge", "peek", "scan", "gouge",
"search", "mount", "disarm", "kick", "parry", "bash", "stun", "punch", "climb",
"grip", "scribe", "brew", "wearspell", "removespell", "emotion", "mentalstate",
"stripsn", "remove", "dig", "full", "thirst", "drunk", "blood", "cook",
"recurringspell", "contagious", "xaffected", "odor", "roomflag", "sectortype",
"roomlight", "televnum", "teledelay"
};

char *	const	a_flags [] =
{
"blind", "invisible", "detect_evil", "detect_invis", "detect_magic",
"detect_hidden", "hold", "sanctuary", "faerie_fire", "infrared", "curse",
"_flaming", "poison", "protect", "_paralysis", "sneak", "hide", "sleep",
"charm", "flying", "pass_door", "floating", "truesight", "detect_traps",
"scrying", "fireshield", "shockshield", "r1", "iceshield", "possess", 
"berserk", "aqua_breath", "recurringspell", "contagious", "acidmist",
"venomshield"
};

char *	const	act_flags [] =
{
"npc", "sentinel", "r1", "r2", "scavenger",  "aggressive", "stayarea",
"wimpy", "onmap", "train", "practice", "immortal", "deadly", "polyself",
"meta_aggr", "guardian", "running", "nowander", "mountable", "mounted",
"chance", "secretive", "citizen", "mobinvis", "noassist", "autonomous",
"pacifist", "noattack", "annoying", "statshield", "prototype", "toggled",
"ability", "r14", "r15", "r16"
};

char *	const	pc_flags [] =
{
"r1", "deadly", "unauthed", "norecall", "nointro", "gag", "retired", "guest",
"nosummon", "pager", "notitled", "groupwho", "diagnose", "highgag", "",
"nstart", "dnd", "flags", "sector",  "aname", "idle", "privacy", "super", "stat",
"combat", "info", "pkill", "clan", "noline", "r16", 
"r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25"
};

char *	const	plr_flags [] =
{
"npc", "boughtpet", "shovedrag", "autoexits", "autoloot", "autosac", "blank", 
"outcast", "brief", "combine", "prompt", "telnet_ga", "holylight", 
"wizinvis", "roomvnum", "silence", "noemote", "attacker", "notell", "log", 
"deny", "freeze", "thief","killer", "litterbug", "ansi", "rip", "nice", 
"flee" ,"autogold", "automap", "afk", "invisprompt", "chat", "toggle", "tail",
"large", "small", "mega", "stone", "high", "semi", "spar", "masenko", "tribeam",
"mental", "scatter", "spectator", "flying", "gambler", "outlaw", "fused", "sound",
"strength", "speed", "metallic", "knowledge", "ki", "ssj4", "invis2", "attack", "exempt", 
"ssj1", "ssj2", "ssj3", "onmap", "mapedit", "unlock1", "fat", "skinny", "ultimate", "wasonmap",
"unlock2"
};

char * const   skill_flags [] =
{

"accuracy", "zanzoken", "vstance", "relax", "enrage", "berserk", "overdrive", "bbarrier", "pbarrier",
"regeneration", "kyodaika", "timefreeze", "soulcry", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
"r9", "r10", "r11", "r12", "r13", "r14", "r15"

};

char * const   pu_flags [] =
{

"turbo","ssj1", "ssj2", "ssj3", "ssj4", "mystic", "kaioken", "transform1", "transform2",
"transform3", "transform4", "transform5", "mutate1", "mutate2", "mutate3",
"mutate4", "snamek", "oozaru", "powerup1", "powerup2", "powerup3", "powerup4",
"powerup5", "powerup6", "powerup7", "larvae", "adult", "semip", "perfect", 
"suppress", "ddd", "disk", "bar", "ball", "sphere", "donut", "bomb", "shield", "barricade",
"flyup", "ussj", "golden", "monster", "metal", 
"superhuman", "ultra", "boost", "steam", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13",
"r14", "r15"

};


char *	const	trap_flags [] =
{
"room", "obj", "enter", "leave", "open", "close", "get", "put", "pick",
"unlock", "north", "south", "east", "r1", "west", "up", "down", "examine",
"r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13",
"r14", "r15" 
};

char *const cmd_flags[] =
{
 "possessed", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", 
 "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", 
 "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30"
};

char *	const	wear_locs [] =
{
"light", "finger1", "finger2", "neck1", "neck2", "body", "head", "legs",
"feet", "hands", "arms", "shield", "about", "waist", "wrist1", "wrist2",
"wield", "hold", "dual_wield", "ears", "eyes", "missile_wield", "back",
"face", "ankle1", "ankle2"
};


char *	const	trig_flags [] =
{
"up", "unlock", "lock", "d_north", "d_south", "d_east", "d_west", "d_up",
"d_down", "door", "container", "open", "close", "passage", "oload", "mload",
"teleport", "teleportall", "teleportplus", "death", "cast", "fakeblade",
"rand4", "rand6", "trapdoor", "anotherroom", "usedial", "absolutevnum",
"showroomdesc", "autoreturn", "r2", "r3"
};

char *	const	part_flags [] =
{
"head", "arms", "legs", "heart", "brains", "guts", "hands", "feet", "fingers",
"ear", "eye", "long_tongue", "eyestalks", "tentacles", "fins", "wings",
"tail", "scales", "claws", "fangs", "horns", "tusks", "tailattack",
"sharpscales", "beak", "haunches", "hooves", "paws", "forelegs", "feathers",
"r1", "r2"
};

char *	const	attack_flags [] =
{
"bite", "claws", "tail", "sting", "punch", "kick", "trip", "bash", "stun",
"gouge", "backstab", "feed", "drain", "firebreath", "frostbreath",
"acidbreath", "lightnbreath", "gasbreath", "poison", "nastypoison", "gaze",
"blindness", "causeserious", "earthquake", "causecritical", "curse",
"flamestrike", "harm", "fireball", "colorspray", "weaken", "r1"
};

char *	const	defense_flags [] =
{
"parry", "dodge", "heal", "curelight", "cureserious", "curecritical",
"dispelmagic", "dispelevil", "sanctuary", "fireshield", "shockshield",
"shield", "bless", "stoneskin", "teleport", "monsum1", "monsum2", "monsum3",
"monsum4", "disarm", "iceshield", "grip", "truesight", "r4", "r5", "r6", "r7",
"r8", "r9", "r10", "r11", "r12", "acidmist", "venomshield"
};

/*
 * Note: I put them all in one big set of flags since almost all of these
 * can be shared between mobs, objs and rooms for the exception of
 * bribe and hitprcnt, which will probably only be used on mobs.
 * ie: drop -- for an object, it would be triggered when that object is
 * dropped; -- for a room, it would be triggered when anything is dropped
 *          -- for a mob, it would be triggered when anything is dropped
 *
 * Something to consider: some of these triggers can be grouped together,
 * and differentiated by different arguments... for example:
 *  hour and time, rand and randiw, speech and speechiw
 * 
 */
char *	const	mprog_flags [] =
{
"act", "speech", "rand", "fight", "death", "hitprcnt", "entry", "greet",
"allgreet", "give", "bribe", "hour", "time", "emote", "wear", "remove", "sac",
"look", "exa", "zap", "get", "drop", "damage", "repair", "randiw",
"speechiw", "pull", "push", "sleep", "rest", "leave", "script", "use", "custom"
 };


char *flag_string( int bitvector, char * const flagarray[] )
{
    static char buf[MAX_STRING_LENGTH];
    int x;

    buf[0] = '\0';
    for ( x = 0; x < 32 ; x++ )
      if ( IS_SET( bitvector, 1 << x ) )
      {
	strcat( buf, flagarray[x] );
        /* don't catenate a blank if the last char is blank  --Gorog */
        if ( buf[0]!='\0' && ' '!=buf[strlen(buf)-1] )
           strcat( buf, " " );
      }
    if ( (x=strlen( buf )) > 0 )
      buf[--x] = '\0';
    
    return buf;
}

char *ext_flag_string( EXT_BV *bitvector, char * const flagarray[] )
{
    static char buf[MAX_STRING_LENGTH];
    int x;

    buf[0] = '\0';
    for ( x = 0; x < MAX_BITS ; x++ )
	if ( xIS_SET( *bitvector, x ) )
	{
	    strcat( buf, flagarray[x] );
	    strcat( buf, " " );
	}
    if ( (x=strlen(buf)) > 0 )
	buf[--x] = '\0';
    
    return buf;
}

char *ext_flag_string_capitalize( EXT_BV *bitvector, char * const flagarray[] )
{
    static char buf[MAX_STRING_LENGTH];
    int x;

    buf[0] = '\0';
    for ( x = 0; x < MAX_BITS ; x++ )
        if ( xIS_SET( *bitvector, x ) )
        {
            strcat( buf, capitalize(flagarray[x]) );
            strcat( buf, " " );
        }
    if ( (x=strlen(buf)) > 0 )
        buf[--x] = '\0';

    return buf;
}



bool can_rmodify( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
//	sh_int vnum = room->vnum;
	int vnum = room->vnum;
	AREA_DATA *pArea;

	if ( IS_NPC( ch ) )
	    return FALSE;
	if( IS_PLR_FLAG( ch, PLR_ONMAP ) )
	{
	    send_to_char( "You cannot use redit from the overland maps.\n\r", ch );
	    return FALSE;
	}
	if ( get_trust( ch ) >= 52 || ch->plr_home)
	    return TRUE;
	if ( !IS_SET( room->room_flags, ROOM_PROTOTYPE))
	{
	    send_to_char( "You cannot modify this room.\n\r", ch );
	    return FALSE;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	    send_to_char( "You must have an assigned area to modify this room.\n\r", ch );
	    return FALSE;
	}
	if ( vnum >= pArea->low_r_vnum
	&&   vnum <= pArea->hi_r_vnum )
	    return TRUE;

	send_to_char( "That room is not in your allocated range.\n\r", ch );
	return FALSE;
}

bool can_omodify( CHAR_DATA *ch, OBJ_DATA *obj )
{
//	sh_int vnum = obj->pIndexData->vnum;
	int vnum = obj->pIndexData->vnum;
	AREA_DATA *pArea;
	
	if ( IS_NPC( ch ) )
	    return FALSE;
	if ( get_trust( ch ) >= 58 )
	    return TRUE;
	if ( !IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	{
	    send_to_char( "You cannot modify this object.\n\r", ch );
	    return FALSE;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	    send_to_char( "You must have an assigned area to modify this object.\n\r", ch );
	    return FALSE;
	}
	if ( vnum >= pArea->low_o_vnum
	&&   vnum <= pArea->hi_o_vnum )
	    return TRUE;

	send_to_char( "That object is not in your allocated range.\n\r", ch );
	return FALSE;
}

bool can_oedit( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
//	sh_int vnum = obj->vnum;
	int vnum;
	AREA_DATA *pArea;
vnum = 0;

	if ( IS_NPC( ch ) )
	    return FALSE;
	if ( get_trust( ch ) >= 58 )
	    return TRUE;
	if ( !IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	{
	    send_to_char( "You cannot modify this object.\n\r", ch );
	    return FALSE;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	    send_to_char( "You must have an assigned area to modify this object.\n\r", ch );
	    return FALSE;
	}
	if ( vnum >= pArea->low_o_vnum
	&&   vnum <= pArea->hi_o_vnum )
	    return TRUE;

	send_to_char( "That object is not in your allocated range.\n\r", ch );
	return FALSE;
}


bool can_mmodify( CHAR_DATA *ch, CHAR_DATA *mob )
{
//	sh_int vnum;
	int vnum;
	AREA_DATA *pArea;
	
	if ( mob == ch )
	  return TRUE;

	if ( !IS_NPC(mob) )
	{
	     if ( get_trust(ch) >= 58
	     &&   get_trust(ch) > get_trust(mob) )
		return TRUE;
	     else
		send_to_char( "You can't do that.\n\r", ch );
	     return FALSE;
	}

	vnum = mob->pIndexData->vnum;

	if ( IS_NPC( ch ) )
	    return FALSE;
	if ( get_trust( ch ) >= 58 )
	    return TRUE;
	if ( !xIS_SET(mob->act, ACT_PROTOTYPE) )
	{
	    send_to_char( "You cannot modify this mobile.\n\r", ch );
	    return FALSE;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	    send_to_char( "You must have an assigned area to modify this mobile.\n\r", ch );
	    return FALSE;
	}
	if ( vnum >= pArea->low_m_vnum
	&&   vnum <= pArea->hi_m_vnum )
	    return TRUE;

	send_to_char( "That mobile is not in your allocated range.\n\r", ch );
	return FALSE;
}

bool can_medit( CHAR_DATA *ch, MOB_INDEX_DATA *mob )
{
//	sh_int vnum = mob->vnum;
     int vnum;
	AREA_DATA *pArea;
vnum = 0;

	if ( IS_NPC(ch) )
	    return FALSE;
	if ( get_trust(ch) >= LEVEL_GOD )
	    return TRUE;
	if ( !xIS_SET(mob->act, ACT_PROTOTYPE) )
	{
	    send_to_char( "You cannot modify this mobile.\n\r", ch );
	    return FALSE;
	}
	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	    send_to_char( "You must have an assigned area to modify this mobile.\n\r", ch );
	    return FALSE;
	}
	if ( vnum >= pArea->low_m_vnum
	&&   vnum <= pArea->hi_m_vnum )
	    return TRUE;

	send_to_char( "That mobile is not in your allocated range.\n\r", ch );
	return FALSE;
}

int get_racetype( char *type )
{
    int x;

    for ( x = 0; x < (sizeof(raceskills_name) / sizeof(raceskills_name[0])); x++ )
      if ( !str_cmp(type, raceskills_name[x]) )
        return x;
    return -1;
}


int get_otype( char *type )
{
    int x;

    for ( x = 0; x < (sizeof(o_types) / sizeof(o_types[0])); x++ )
      if ( !str_cmp(type, o_types[x]) )
        return x;
    return -1;
}

int get_exitsymbol( char *type )
{
    int x;

    for ( x = 0; x < (sizeof(exit_symbols) / sizeof(exit_symbols[0])); x++ )
      if ( !str_cmp(type, exit_symbols[x]) )
        return x;
    return -1;
}

int get_aflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(a_flags) / sizeof(a_flags[0])); x++ )
      if ( !str_cmp(flag, a_flags[x]) )
        return x;
    return -1;
}

int get_trapflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(trap_flags) / sizeof(trap_flags[0])); x++ )
      if ( !str_cmp(flag, trap_flags[x]) )
        return x;
    return -1;
}

int get_atype( char *type )
{
    int x;

    for ( x = 0; x < (sizeof(a_types) / sizeof(a_types[0])); x++ )
      if ( !str_cmp(type, a_types[x]) )
        return x;
    return -1;
}

int get_npc_race( char *type )
{
    int x;

    for ( x = 0; x < MAX_NPC_RACE; x++ )
      if ( !str_cmp(type, npc_race[x]) )
        return x;
    return -1;
}

int get_pc_race( char *type )
{
    int i;

    for ( i = 0; i < MAX_PC_RACE; i++ )
      if ( !str_cmp(type, race_table[i]->race_name) )
        return i;
    return -1;
}

int get_wearloc( char *type )
{
    int x;
    
    for ( x = 0; x < (sizeof(wear_locs) / sizeof(wear_locs[0])); x++ )
      if ( !str_cmp(type, wear_locs[x]) )
        return x;
    return -1;
}

int get_secflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(sec_flags) / sizeof(sec_flags[0])); x++ )
      if ( !str_cmp(flag, sec_flags[x]) )
        return x;     
    return -1;            
}                      

int get_exflag( char *flag )
{
    int x;
    
    for ( x = 0; x < (sizeof(ex_flags) / sizeof(ex_flags[0])); x++ )
      if ( !str_cmp(flag, ex_flags[x]) )
        return x;
    return -1;
}

int get_pulltype( char *type )
{
    int x;

    if ( !str_cmp(type, "none") || !str_cmp(type, "clear") )
	return 0;

    for ( x = 0; x < (sizeof(ex_pmisc) / sizeof(ex_pmisc[0])); x++ )
	if ( !str_cmp(type, ex_pmisc[x]) )
	    return x;

    for ( x = 0; x < (sizeof(ex_pwater) / sizeof(ex_pwater[0])); x++ )
	if ( !str_cmp(type, ex_pwater[x]) )
	    return x + PT_WATER;
    for ( x = 0; x < (sizeof(ex_pair) / sizeof(ex_pair[0])); x++ )
	if ( !str_cmp(type, ex_pair[x]) )
	    return x + PT_AIR;
    for ( x = 0; x < (sizeof(ex_pearth) / sizeof(ex_pearth[0])); x++ )
	if ( !str_cmp(type, ex_pearth[x]) )
	    return x + PT_EARTH;
    for ( x = 0; x < (sizeof(ex_pfire) / sizeof(ex_pfire[0])); x++ )
	if ( !str_cmp(type, ex_pfire[x]) )
	    return x + PT_FIRE;
    return -1;
}

int get_rflag( char *flag )
{
    int x;
    
    for ( x = 0; x < (sizeof(r_flags) / sizeof(r_flags[0])); x++ )
      if ( !str_cmp(flag, r_flags[x]) )
        return x;
    return -1;
}

int get_rflag2( char *flag )
{
    int x;
    
    for ( x = 0; x < (sizeof(r_flags2) / sizeof(r_flags2[0])); x++ )
      if ( !str_cmp( flag, r_flags2[x] ) )
        return x;
    return -1;
}

int get_mpflag( char *flag )
{
    int x;
    
    for ( x = 0; x < (sizeof(mprog_flags) / sizeof(mprog_flags[0])); x++ )
      if ( !str_cmp(flag, mprog_flags[x]) )
        return x;
    return -1;
}

int get_oflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(o_flags) / sizeof(o_flags[0])); x++ )
      if ( !str_cmp(flag, o_flags[x]) )
        return x;
    return -1;
}

int get_areaflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(area_flags) / sizeof(area_flags[0])); x++ )
      if ( !str_cmp(flag, area_flags[x]) )
        return x;
    return -1;
}

int get_wflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(w_flags) / sizeof(w_flags[0])); x++ )
      if ( !str_cmp(flag, w_flags[x]) )
        return x;
    return -1;
}

int get_actflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(act_flags) / sizeof(act_flags[0])); x++ )
      if ( !str_cmp(flag, act_flags[x]) )
        return x;
    return -1;
}

int get_pcflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(pc_flags) / sizeof(pc_flags[0])); x++ )
      if ( !str_cmp(flag, pc_flags[x]) )
        return x;
    return -1;
}

int get_plrflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(plr_flags) / sizeof(plr_flags[0])); x++ )
      if ( !str_cmp(flag, plr_flags[x]) )
        return x;
    return -1;
}
int get_puflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(pu_flags) / sizeof(pu_flags[0])); x++ )
      if ( !str_cmp(flag, pu_flags[x]) )
        return x;
    return -1;
}

int get_skillflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(skill_flags) / sizeof(skill_flags[0])); x++ )
      if ( !str_cmp(flag, skill_flags[x]) )
        return x;
    return -1;
}


/*
 * For use with cedit --Shaddai
 */

int
get_cmdflag ( char *flag )
{
  int x;

  for ( x = 0; x < (sizeof(cmd_flags) / sizeof(cmd_flags[0])); x++)
    if ( !str_cmp(flag, cmd_flags[x]))
        return x;
  return -1;
}

int get_trigflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(trig_flags) / sizeof(trig_flags[0])); x++ )
      if ( !str_cmp(flag, trig_flags[x]) )
        return x;
    return -1;
}

int get_partflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(part_flags) / sizeof(part_flags[0])); x++ )
      if ( !str_cmp(flag, part_flags[x]) )
        return x;
    return -1;
}

int get_attackflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(attack_flags) / sizeof(attack_flags[0])); x++ )
      if ( !str_cmp(flag, attack_flags[x]) )
        return x;
    return -1;
}

int get_defenseflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof(defense_flags) / sizeof(defense_flags[0])); x++ )
      if ( !str_cmp(flag, defense_flags[x]) )
        return x;
    return -1;
}


/*
 * Remove carriage returns from a line
 */
char *strip_cr( char *str )
{
    static char newstr[MAX_STRING_LENGTH];
    int i, j;

    for ( i=j=0; str[i] != '\0'; i++ )
	if ( str[i] != '\r' )
	{
	  newstr[j++] = str[i];	
	}
    newstr[j] = '\0';
    return newstr;
}


/*
 * Removes the tildes from a line, except if it's the last character.
 */
/*
void smush_tilde( char *str )
{
    int len;
    char last;
    char *strptr;
    
    strptr = str;
    
    len  = strlen( str );
    if ( len )
      last = strptr[len-1];
    else
      last = '\0';

    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }
    if ( len )
      strptr[len-1] = last;

    return;
}
*/

void goto_char( CHAR_DATA *ch, CHAR_DATA *wch, char *argument )
{
    ROOM_INDEX_DATA *location, *in_room;

    set_char_color( AT_IMMORT, ch );
    location = wch->in_room;

    if( is_ignoring( wch, ch ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( ch, location ) )
    {
      if ( get_trust( ch ) < LEVEL_GREATER )
      {
	   send_to_char( "That room is private right now.\n\r", ch );
	   return;
      }
      else
      {
	   send_to_char( "Overriding private flag!\n\r", ch );
      }
    }

    in_room = ch->in_room;
    if ( ch->fighting )
	stop_fighting( ch, TRUE );

    /* Modified bamfout processing by Altrag, installed by Samson 12-10-97 */
       if (ch->pcdata && ch->pcdata->bamfout[0])
         act(AT_IMMORT, "$T", ch, NULL,
         bamf_print(ch->pcdata->bamfout, ch), TO_CANSEE);
       else
         act(AT_IMMORT, "$n vanishes suddenly into thin air.", ch, NULL, NULL, TO_CANSEE);


    ch->regoto = ch->in_room->vnum;
    leave_map( ch, wch, location );
    /* Modified bamfin processing by Altrag, installed by Samson 12-10-97 */
       if (ch->pcdata && ch->pcdata->bamfin[0])
          act(AT_IMMORT, "$T", ch, NULL,
          bamf_print(ch->pcdata->bamfin, ch), TO_CANSEE);
       else
          act(AT_IMMORT, "$n appears suddenly out of thin air.", ch, NULL, NULL, TO_CANSEE);

    return;
}

void goto_obj( CHAR_DATA *ch, OBJ_DATA *obj, char *argument )
{
    ROOM_INDEX_DATA *location;

    set_char_color( AT_IMMORT, ch );
    location = obj->in_room;

    if ( room_is_private( ch, location ) )
    {
      if ( get_trust( ch ) < LEVEL_GREATER )
      {
	   send_to_char( "That room is private right now.\n\r", ch );
	   return;
      }
      else
      {
	   send_to_char( "Overriding private flag!\n\r", ch );
      }
    }

    if ( ch->fighting )
	stop_fighting( ch, TRUE );

    /* Modified bamfout processing by Altrag, installed by Samson 12-10-97 */
       if (ch->pcdata && ch->pcdata->bamfout[0])
         act(AT_IMMORT, "$T", ch, NULL,
         bamf_print(ch->pcdata->bamfout, ch), TO_CANSEE);
       else
         act(AT_IMMORT, "$n vanishes suddenly into thin air.", ch, NULL, NULL, TO_CANSEE);

    ch->regoto = ch->in_room->vnum;
    leave_map( ch, NULL, location );

    /* Modified bamfin processing by Altrag, installed by Samson 12-10-97 */
       if (ch->pcdata && ch->pcdata->bamfin[0])
          act(AT_IMMORT, "$T", ch, NULL,
          bamf_print(ch->pcdata->bamfin, ch), TO_CANSEE);
       else
          act(AT_IMMORT, "$n appears suddenly out of thin air.", ch, NULL, NULL, TO_CANSEE);

    return;
}


void do_goto( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg1[MIL];
    char arg2[MIL];
    ROOM_INDEX_DATA *location, *in_room;
    CHAR_DATA *fch, *fch_next, *victim, *wch;
    AREA_DATA *pArea;
    OBJ_DATA *obj;
    sh_int vnum;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    /* Begin Overland Map additions */
    if ( !str_cmp( arg, "map" ) )
    {
//	char arg1[MAX_INPUT_LENGTH];
//	char arg2[MAX_INPUT_LENGTH];
	int x, y;
	int map = -1;

//      argument = one_argument( argument, arg1 );
//      argument = one_argument( argument, arg2 );

	if( arg1[0] == '\0' )
      {
	   send_to_char( "Goto which map??\n\r", ch );
	   return;
	}
      
      if( !str_cmp( arg1, "earth" ) )
	   map = ACON_EARTH;

      if( !str_cmp( arg1, "aboveearth" ) )
	   map = ACON_AEARTH;

	if( !str_cmp( arg1, "belowearth" ) )
	   map = ACON_BEARTH;
      if( !str_cmp( arg1, "namek" ) )
           map = ACON_NAMEK;

      if( !str_cmp( arg1, "abovenamek" ) )
           map = ACON_ANAMEK;

        if( !str_cmp( arg1, "belownamek" ) )
           map = ACON_BNAMEK;
        if( !str_cmp( arg1, "battlestar" ) )
           map = ACON_BATTLESTAR;
        if( !str_cmp( arg1, "legendaria" ) )
	   map = ACON_LEGENDARIA;
      if( map == -1 )
	{
        ch_printf( ch, "There isn't a map for '%s'.\n\r", arg1 );
        return;
	}

	if( arg2[0] == '\0' && argument[0] == '\0' )
	{
	   enter_map( ch, 250, 250, map );
	   return;
	}

	if( arg2[0] == '\0' || argument[0] == '\0' )
	{
	   send_to_char( "Usage: goto map <mapname> <X> <Y>\n\r", ch );
	   return;
	}

      x = atoi( arg2 );
	y = atoi( argument );

      if( x < 0 || x >= MAX_X )
      {
	   ch_printf( ch, "Valid x coordinates are 0 to %d.\n\r", MAX_X - 1 );
	   return;
      }

      if( y < 0 || y >= MAX_Y )
      {
	   ch_printf( ch, "Valid y coordinates are 0 to %d.\n\r", MAX_Y - 1 );
	   return;
      }
	enter_map( ch, x, y, map );
	return;
    }

    if( !is_number( arg ) )
    {
      if ( ( wch = get_char_world( ch, arg ) ) != NULL && wch->in_room != NULL )
      {
	  goto_char( ch, wch, arg );
	  return;
      }

      if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
      {
	  goto_obj( ch, obj, arg );
	  return;
      }
    }
    /* End of Overland Map additions */

   if ( !is_number(arg) && (fch=get_char_world(ch, arg)) )
   {
      if ( !IS_NPC(fch) && get_trust(ch) < get_trust(fch)
      &&   xIS_SET(fch->pcdata->flags, PCFLAG_DND) ) 
      {
         ch_printf( ch,  
         "&RSorry, %s does not wish to be disturbed.\n\r", fch->name);
         ch_printf( fch,
            "&RYour DND flag just foiled %s's goto command.\n\r", ch->name);
         return;
      }
   }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	vnum = atoi( arg );
	if ( vnum < 0 || get_room_index( vnum ) )
	{
		send_to_char( "You cannot find that...\n\r", ch );
		return;
	}

//	if ( get_trust( ch ) < LEVEL_CREATOR
	if ( vnum < 1 || IS_NPC(ch) || !ch->pcdata->area )
	{
	  send_to_char( "No such location.\n\r", ch );
	  return;
	}
	if ( get_trust( ch ) < sysdata.level_modify_proto && !ch->plr_home )
	{
	    if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	    {
		send_to_char( "You must have an assigned area to create rooms.\n\r", ch );
		return;
	    }
	    if ( vnum < pArea->low_r_vnum
	    ||   vnum > pArea->hi_r_vnum )
	    {
		send_to_char( "That room is not within your assigned range.\n\r", ch );
		return;
	    }
	}
        location = make_room( vnum, ch->pcdata->area );
	if ( !location )
	{
	  bug( "Goto: make_room failed", 0 );
	  return;
	}
	location->area = ch->pcdata->area;
	set_char_color( AT_WHITE, ch );
	send_to_char( "Waving your hand, you form order from swirling chaos,\n\rand step into a new reality...\n\r", ch );
    }

    if ( (victim = room_is_dnd( ch, location)) )
    {
       ch_printf( ch, "That room is \"do not disturb\" right now.\n\r");
       ch_printf( victim,
         "Your DND flag just foiled %s's goto command.\n\r", ch->name );
       return;
    }
    if ( room_is_private( ch, location ) && !IN_HOUSE(ch))
    {
	if ( get_trust( ch ) < sysdata.level_override_private )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
	else
	    send_to_char( "Overriding private flag!\n\r", ch );
    }

    in_room = ch->in_room;
    if ( ch->fighting )
	stop_fighting( ch, TRUE );

    if ( !xIS_SET(ch->act, PLR_WIZINVIS) )
	act( AT_IMMORT, "$n $T", ch, NULL,
	    (ch->pcdata && ch->pcdata->bamfout[0] != '\0')
	    ? ch->pcdata->bamfout : "leaves in a swirling mist.",  TO_ROOM );


    /* It's assumed that if you've come this far, it's a room vnum you entered */
    ch->regoto = ch->in_room->vnum;
    leave_map( ch, NULL, location );

    if ( !xIS_SET(ch->act, PLR_WIZINVIS) )
        act( AT_IMMORT, "$n $T", ch, NULL,
            (ch->pcdata && ch->pcdata->bamfin[0] != '\0')
            ? ch->pcdata->bamfin : "appears in a swirling mist.", TO_ROOM );
//    if ( ch->level > 48 )
//    do_look( ch, "auto" );

    if ( ch->in_room == in_room )
      return;
    for ( fch = in_room->first_person; fch; fch = fch_next )
    {
	fch_next = fch->next_in_room;
	if ( fch->master == ch && IS_IMMORTAL(fch) )
	{
	    act( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR );
	    do_goto( fch, argument );
	}
/* Experimental change by Gorog so imm's personal mobs follow them */
        else if ( IS_NPC(fch) && fch->master == ch )
        {
           char_from_room (fch);
           char_to_room( fch, location );
        }
    }
    return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf  [MAX_STRING_LENGTH];
    char outbuf[MSL];
    int  num,size,plus;
    char char1,char2;
    CHAR_DATA *victim;
    int value;
    int minattr, maxattr;
    bool lockvictim;
    char *origarg = argument;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't mset\n\r", ch );
	return;    
    }
    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_MOB_DESC:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_mset: sub_mob_desc: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  victim = ch->dest_buf;
	  if ( char_died(victim) )
	  {
		send_to_char( "Your victim died!\n\r", ch );
		stop_editing( ch );
		return;
	  }
	  STRFREE( victim->description );
	  victim->description = copy_buffer( ch );
	  if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  {
		STRFREE( victim->pIndexData->description );
		victim->pIndexData->description = QUICKLINK( victim->description );
	  }
	  stop_editing( ch );
	  ch->substate = ch->tempnum;
	  return;
    }

    victim = NULL;
    lockvictim = FALSE;
    smash_tilde( argument );

    if ( ch->substate == SUB_REPEATCMD )
    {
	victim = ch->dest_buf;
	
	
    if ( !victim )
    {
            send_to_char( "Your victim died!\n\r", ch );
            argument = "done";
    }

	if ( argument[0] == '\0' || !str_cmp( argument, " " )
	||   !str_cmp( argument, "stat" ) )
	{
	    if ( victim )
		do_mstat( ch, victim->name );
	    else
	        send_to_char( "No victim selected.  Type '?' for help.\n\r", ch );
	    return;
	}
	if ( !str_cmp( argument, "done" ) || !str_cmp( argument, "off" ) )
	{
        if ( ch->dest_buf )
            RelDestroy( relMSET_ON, ch, ch->dest_buf );
	    send_to_char( "Mset mode off.\n\r", ch );
	    ch->substate = SUB_NONE;
	    ch->dest_buf = NULL;
	    if ( ch->pcdata && ch->pcdata->subprompt )
	    {
		STRFREE( ch->pcdata->subprompt );
		ch->pcdata->subprompt = NULL;
	    }
	    return;
	}
    }
    if ( victim )
    {
	lockvictim = TRUE;
	strcpy( arg1, victim->name );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
    }
    else
    {
	lockvictim = FALSE;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
    }

    if ( !str_cmp( arg1, "on" ) )
    {
	send_to_char( "Syntax: mset <victim|vnum> on.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || (arg2[0] == '\0' && ch->substate != SUB_REPEATCMD)
    ||   !str_cmp( arg1, "?" ) )
    {
	if ( ch->substate == SUB_REPEATCMD )
	{
	    if ( victim )
		send_to_char( "Syntax: <field>  <value>\n\r",		ch );
	    else
		send_to_char( "Syntax: <victim> <field>  <value>\n\r",	ch );
	}
	else
	    send_to_char( "Syntax: mset <victim> <field>  <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  str int wis dex con cha lck stat sex race\n\r",ch );
	send_to_char( "  powerlevel life energy practice align\n\r", ch );
	send_to_char( "  level gold drunk flags biomass rage kili\n\r", ch );
        send_to_char( "  height weight                             \n\r", ch);
	send_to_char( "  pos defpos part (see BODYPARTS)\n\r",		ch );
	send_to_char( "  attack defense numattacks\n\r",		ch );
	send_to_char( "  name short long description title spec clan\n\r", ch );
	send_to_char( "For editing index/prototype mobiles:\n\r",	ch );
	send_to_char( "  hitnumdie hitsizedie hitplus (hit points)\n\r",ch );
	send_to_char( "  damnumdie damsizedie damplus (damage roll)\n\r",ch );
	send_to_char( "To toggle area flag: aloaded\n\r",ch);
	send_to_char( "To toggle pkill flag: pkill\n\r",ch);
	return;
    }

    if ( !victim && get_trust( ch ) < LEVEL_GOD )
    {
	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }
    else
    if ( !victim )
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "No one like that in all the realms.\n\r", ch );
	    return;
	}
    }

    if ( get_trust( ch ) < get_trust( victim ) && !IS_NPC( victim ) )
    {
	send_to_char( "You can't do that!\n\r", ch );
	ch->dest_buf = NULL;
	return;
    }
    if (IS_NPC(victim))
    {
      if ( ch->level < 65 && (victim->pIndexData->vnum == 298 || victim->pIndexData->vnum == 297))
      {
         send_to_char( "You can't mset Shenlong\n\r", ch);
         ch->dest_buf = NULL;
         return;
      }
    }      
    if ( get_trust( ch ) < LEVEL_GREATER
    &&   IS_NPC( victim )
    &&  xIS_SET( victim->act, ACT_STATSHIELD ) )
    {
	send_to_char( "You can't do that!\n\r", ch );
	ch->dest_buf = NULL;
	return;
    }
    if ( lockvictim )
      ch->dest_buf = victim;

    if ( IS_NPC(victim) )
    {
	minattr = 5;
	maxattr = 40;
    }
    else
    {
	minattr = 5;
	maxattr = 25;
    }

    if ( !str_cmp( arg2, "on" ) )
    {
	CHECK_SUBRESTRICTED( ch );
	ch_printf( ch, "Mset mode on. (Editing %s).\n\r",
		victim->name );
	ch->substate = SUB_REPEATCMD;
	ch->dest_buf = victim;
	if ( ch->pcdata )
	{
	   if ( ch->pcdata->subprompt )
		STRFREE( ch->pcdata->subprompt );
	   if ( IS_NPC(victim) )
		sprintf( buf, "<&CMset &W#%d&w> %%i", victim->pIndexData->vnum );
	   else
		sprintf( buf, "<&CMset &W%s&w> %%i", victim->name );
	   ch->pcdata->subprompt = STRALLOC( buf );
	}
    RelCreate( relMSET_ON, ch, victim );
	return;
    }
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    if ( atoi(arg3) < -1 && value == -1 )
      value = atoi(arg3);

    if ( !str_cmp( arg2, "str" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Strength range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_str = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_str = value;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Intelligence range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_int = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_int = value;
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Wisdom range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_wis = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_wis = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Dexterity range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_dex = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_dex = value;
	return;
    }
    if ( !str_cmp( arg2, "stat" ) )
    {
        if ( !can_mmodify( ch, victim ) )
          return;
        if ( value < minattr || value > maxattr )
        {
            ch_printf( ch, "Stat range is %d to %d.\n\r", minattr, maxattr );
            return;
        }
        victim->perm_str = value;
        victim->perm_int = value;
        victim->perm_dex = value;
        victim->perm_wis = value;
        victim->perm_con = value;
        victim->perm_cha = value;
        victim->perm_lck = value;

        if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
        {
          victim->pIndexData->perm_str = value;
          victim->pIndexData->perm_int = value;
          victim->pIndexData->perm_wis = value;
          victim->pIndexData->perm_dex = value;
          victim->pIndexData->perm_con = value;
          victim->pIndexData->perm_cha = value;
          victim->pIndexData->perm_lck = value;
        }
        return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Constitution range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_con = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_con = value;
	return;
    }

    if ( !str_cmp( arg2, "cha" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Charisma range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_cha = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_cha = value;
	return;
    }

    if ( !str_cmp( arg2, "lck" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < minattr || value > maxattr )
	{
	    ch_printf( ch, "Luck range is %d to %d.\n\r", minattr, maxattr );
	    return;
	}
	victim->perm_lck = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->perm_lck = value;
	return;
    }
    if ( !str_cmp( arg2, "kili"))
    {
	if (ch->level < 63)
		return;
	if ( !can_mmodify( ch, victim ) )
          return;
	if(IS_NPC(victim))
	{
		send_to_char("This can only be set for PCs!\r\n", ch);
		return;
	}
        if ( value < 0 || value > 100000000 )
        {
            send_to_char( "Kili range is 0 to 100,000,000.\n\r", ch );
            return;
        }
        victim->pcdata->kili = value;
        return;
    }
    if ( !str_cmp( arg2, "bless"))
    {
        if (ch->level < 65)
                return;
        if ( !can_mmodify( ch, victim ) )
          return;
        if(IS_NPC(victim))
        {
                send_to_char("This can only be set for PCs!\r\n", ch);
                return;
        }
        if ( value < 0 || value > 100000000 )
        {
            send_to_char( "Bless range is 0 to 100,000,000.\n\r", ch );
            return;
        }
        victim->pcdata->bless = value;
        return;
    }


    if ( !str_cmp( arg2, "biomass" ) )
    {
        if (ch->level < 63)
        return;
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 25000 )
	{
	    send_to_char( "Biomass range is 0 to 25000.\n\r", ch );
	    return;
	}
	victim->biomass = value;
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->sex = value;
	return;
    }
/*
    if ( !str_cmp( arg2, "class" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;


        if( IS_NPC(victim) )
	{
          if ( value >= MAX_NPC_CLASS || value < 0 )
	  {            
		ch_printf( ch, "NPC Class range is 0 to %d.\n", MAX_NPC_CLASS-1 );
	        return;
	  }
	  victim->class = value;
	  if ( xIS_SET(victim->act, ACT_PROTOTYPE) )
	     victim->pIndexData->class = value;
	  return;
	}


	if ( value < 0 || value >= MAX_CLASS )
	{
	    ch_printf( ch, "Class range is 0 to %d.\n", MAX_CLASS );
	    return;
	}
	victim->class = value;
	return;
    }
*/
    if ( !str_cmp( arg2, "race" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( IS_NPC(victim) )
		value = get_npc_race( arg3 );
	else
		value = get_pc_race( arg3 );

	if ( value < 0 )
	  value = atoi( arg3 );
	if ( !IS_NPC(victim) && (value < 0 || value >= MAX_RACE) )
	{
	    ch_printf( ch, "Race range is 0 to %d.\n", MAX_RACE-1 );
	    return;
	}
	if ( IS_NPC(victim) && (value < 0 || value >= MAX_NPC_RACE) )
	{
	    ch_printf( ch, "Race range is 0 to %d.\n", MAX_NPC_RACE-1 );
	    return;
	}
	victim->race = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->race = value;
      if (!IS_NPC(victim))
      victim->pcdata->rank = str_dup( "" );        
	return;
    }
/*
    if ( !str_cmp( arg2, "armor" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
        if (!IS_NPC(victim) && ch->level < 65 );
        return;
	if ( value < -300 || value > 300 )
	{
	    send_to_char( "AC range is -300 to 300.\n\r", ch );
	    return;
	}
	victim->armor = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->ac = value;
	return;
    }
*/
    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
       if ( !IS_NPC(victim) )
	{

            send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > LEVEL_AVATAR + 5 )
	{
            ch_printf( ch, "Level range is 0 to %d.\n\r", LEVEL_AVATAR + 5 );
	    return;
	}
	victim->level = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->level = value;
	return;
    }

    if ( !str_cmp( arg2, "numattacks" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 20 )
	{
	    send_to_char( "Attacks range is 0 to 20.\n\r", ch );
	    return;
	}
	victim->numattacks = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->numattacks = value;
	return;
    }

    if ( !str_cmp( arg2, "gold" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
        if ( !IS_NPC(victim) && ch->level < 65 )
          return;
	victim->gold = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->gold = value;
	return;
    }
    if ( !str_cmp( arg2, "bank" ) )
    {
        if ( !IS_NPC(victim) && ch->level < 65 )
          return;
        victim->balance = value;
        return;
    }

/*
    if ( !str_cmp( arg2, "hitroll" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
        if (!IS_NPC(victim) && ch->level < 65 );
        return;
	victim->hitroll = URANGE(0, value, 85);
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->hitroll = victim->hitroll;
	return;
    }

    if ( !str_cmp( arg2, "damroll" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
        if (!IS_NPC(victim) && ch->level < 65 );
        return;
	victim->damroll = URANGE(0, value, 65);
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->damroll = victim->damroll;
	return;
    }
*/
    if ( !str_cmp( arg2, "life" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
        if ( ch->level < 65 && !IS_NPC(victim) )
        return;
	if ( value < 1 || value > 1000000000 )
	{
	    send_to_char( "Life range is 1 to 1000000000 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

    if ( !str_cmp( arg2, "energy" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 100000 )
	{
	    send_to_char( "Ki energy range is 0 to 100,000 ki points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }
/*
    if ( !str_cmp( arg2, "move" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
	return;
    }
*/

    if ( !str_cmp( arg2, "rage" ) )
    {
     if (ch->level < 65)
     return;
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( value < 0 || value > 2000 )
     {
         send_to_char( "Rage value is from 0 to 1600", ch );
         return;
     }
      victim->rage = value;
      return;
}
if ( !str_cmp( arg2, "height" ) )
{
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( value < 30 || value > 100 )
     {
         send_to_char( "Height value is from 30 to 100", ch );
         return;
     }
      victim->height = value;
      return;
}
if ( !str_cmp( arg2, "weight" ) )
{
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( value < 70 || value > 400 )
     {
         send_to_char( "Weight value is from 70 to 400", ch );
         return;
     }
      victim->weight = value;
      return;
}

if ( !str_cmp( arg2, "armor" ) )
{
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( !IS_IMMORTAL(ch))
     return;
     if ( value < 0 || value > 700 )
     {
         send_to_char( "Armor range is 0 to 700 stat training sessions", ch );
         return;
     }
      victim->armory = value;
      return;
}

if ( !str_cmp( arg2, "train" ) )
{
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( value < 0 || value > 400 )
     {
         send_to_char( "Train range is 0 to 400 stat training sessions", ch );
         return;
     }
      victim->stat = value;
      return;
}
 if ( !str_cmp( arg2, "slevel" ) )
{
     if ( !can_mmodify( ch, victim ) )
       return;
     if ( value < 0 || value > 400 )
     {
         send_to_char( "Slevel range is 0 to 400 level training sessions", ch );
         return;
     }
      victim->skill_lvl = value;
      return;
}

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->alignment = value;
	return;
    }

    if ( !str_cmp( arg2, "password" ) )
    {
      char *pwdnew;
      char *p;

      if ( get_trust( ch ) < LEVEL_SUB_IMPLEM )
      {
	send_to_char( "You can't do that.\n\r", ch );
	return;
      }
      if ( IS_NPC( victim ) )
      {
	send_to_char( "Mobs don't have passwords.\n\r", ch );
	return;
      }

      if ( strlen(arg3) < 5 )
      {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
      }

	if (arg3[0] == '!')
	 {
	  send_to_char( "New password cannot begin with the '!' character.",ch);
	  return;
	 }

      /*
       * No tilde allowed because of player file format.
       */
      pwdnew = crypt( arg3, ch->name );
      for ( p = pwdnew; *p != '\0'; p++ )
      {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
      }

      DISPOSE( victim->pcdata->pwd );
      victim->pcdata->pwd = str_dup( pwdnew );
      if ( IS_SET(sysdata.save_flags, SV_PASSCHG) )
 	save_char_obj( victim );
      send_to_char( "Ok.\n\r", ch );
      ch_printf( victim, "Your password has been changed by %s.\n\r", ch->name );
      return;
    }

    if ( !str_cmp( arg2, "rank" ) )
    {
      if ( get_trust( ch ) < LEVEL_GOD )
      {
        send_to_char( "You can't do that.\n\r", ch );
        return;
      }
      if ( IS_NPC(victim) )
      {
        send_to_char( "Not on NPC's.\n\r", ch );
	return;
      }
      smash_tilde( argument );
//      DISPOSE( victim->pcdata->rank );
      if ( !argument || argument[0] == '\0' || !str_cmp( argument, "none" ) )
      victim->pcdata->rank = str_dup( "" );
   //   do_rank(victim, "none");
      else
        victim->pcdata->rank = str_dup( argument );
      send_to_char( "Ok.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "powerlevel" ) )
    {
        if ( !can_mmodify( ch, victim ) )
          return;
        if (!IS_NPC(victim))
        {
         if ( value < MIN_PL || value > MAX_PL )
         {
             ch_printf( ch, "PL range is %d to %d.\n\r", MIN_PL, MAX_PL );
             return;
         }
        }
        if (IS_NPC(victim))
        {
         if ( value < MIN_MOBPL || value > MAX_MOBPL )
         {
             ch_printf( ch, "PL range is %d to %d.\n\r", MIN_MOBPL, MAX_MOBPL );
             return;
         }
        }
      
        victim->exp = value;
        if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
          victim->pIndexData->exp = value;
        return;
    }
    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 40 )
	{
	    send_to_char( "Drunk range is 0 to 40.\n\r", ch );
	    return;
	}

	victim->drunk = value;
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( !arg3 || arg3[0] == '\0' )
	{
	   send_to_char( "Names can not be set to an empty string.\n\r", ch );
	   return;
	}

	STRFREE( victim->name );
	victim->name = STRALLOC( arg3 );
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	{
	   STRFREE( victim->pIndexData->player_name );
	   victim->pIndexData->player_name = QUICKLINK( victim->name );
	}
	return;
    }

    if ( !str_cmp( arg2, "minsnoop" ) )
    {
	if ( get_trust( ch ) < LEVEL_SUB_IMPLEM )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if ( victim->pcdata )
	{
	    victim->pcdata->min_snoop = value;
	    return;
	}
    }

    if ( !str_cmp( arg2, "clan" ) )
    {
	CLAN_DATA *clan;

	if ( get_trust( ch ) < 65 )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	if ( IS_NPC(victim) )
	{
          if ( !arg3 || arg3[0] == '\0' )
          {
            send_to_char( "Done.\n\r", ch);
            STRFREE(victim->pIndexData->clan);
            return;
          }
          clan = get_clan( arg3 );
          if ( !clan )
          {
             send_to_char( "No such clan.\n\r", ch );
             return;
          }
          STRFREE( victim->pIndexData->clan );
          victim->pIndexData->clan = STRALLOC( clan->shortname );
          send_to_char( "Done.\n\r", ch);
          return;
	}

	if ( !arg3 || arg3[0] == '\0' )
	{
	    /* Crash bug fix, oops guess I should have caught this one :)
	     * But it was early in the morning :P --Shaddai 
	     */
	    if ( victim->pcdata->clan == NULL )
		return;
	    /* Added a check on immortals so immortals don't take up
	     * any membership space. --Shaddai
	     */
        /* Added a check on immortals so immortals don't take up
         * any membership space. --Shaddai
         */
             if( !IS_IMMORTAL( victim ) ) 
             {
                   --victim->pcdata->clan->members; 
                  if( victim->pcdata->clan->members < 0 )
                   victim->pcdata->clan->members = 0;
                   save_clan( victim->pcdata->clan );
             }
	    STRFREE( victim->pcdata->clan_name );
	    victim->pcdata->clan_name	= STRALLOC( "" );
	    victim->pcdata->clan	= NULL;
	    return;
	}
	clan = get_clan( arg3 );
	if ( !clan )
	{
	   send_to_char( "No such clan.\n\r", ch );
	   return;
	}
        if( victim->pcdata->clan != NULL && !IS_IMMORTAL( victim ) )
        {
            --victim->pcdata->clan->members; 
                    if( victim->pcdata->clan->members < 0 )
                       victim->pcdata->clan->members = 0;
                save_clan( victim->pcdata->clan );
        }
	STRFREE( victim->pcdata->clan_name );
	victim->pcdata->clan_name = QUICKLINK( clan->name );
	victim->pcdata->clan = clan;
	if ( !IS_IMMORTAL( victim ) ) {
	   ++victim->pcdata->clan->members; 
	   save_clan( victim->pcdata->clan );
	}
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	STRFREE( victim->short_descr );
	victim->short_descr = STRALLOC( arg3 );
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	{
	   STRFREE( victim->pIndexData->short_descr );
	   victim->pIndexData->short_descr = QUICKLINK( victim->short_descr );
	}
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	STRFREE( victim->long_descr );
	strcpy( buf, arg3 );
	strcat( buf, "\n\r" );
	victim->long_descr = STRALLOC( buf );
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	{
	   STRFREE( victim->pIndexData->long_descr );
	   victim->pIndexData->long_descr = QUICKLINK( victim->long_descr );
	}
	return;
    }

    if ( !str_cmp( arg2, "description" ) )
    {
	if ( arg3[0] )
	{
	   STRFREE( victim->description );
	   victim->description = STRALLOC( arg3 );
	   if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	   {
	      STRFREE(victim->pIndexData->description );
	      victim->pIndexData->description = QUICKLINK( victim->description );
	   }
	   return;
	}
	CHECK_SUBRESTRICTED( ch );
	if ( ch->substate == SUB_REPEATCMD )
	  ch->tempnum = SUB_REPEATCMD;
	else
	  ch->tempnum = SUB_NONE;
	ch->substate = SUB_MOB_DESC;
	ch->dest_buf = victim;
	start_editing( ch, victim->description );
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }
   if ( !str_cmp( arg2, "spec" ) || !str_cmp( arg2, "spec_fun" ) )
   {
	SPEC_FUN *specfun;

	if( !can_mmodify( ch, victim ) )
	   return;

	if( !IS_NPC(victim) )
	{
	   send_to_char( "Not on PC's.\n\r", ch );
	   return;
	}

      if( !str_cmp( arg3, "none" ) )
      {
         victim->spec_fun = NULL;
	   STRFREE( victim->spec_funname );
	   send_to_char( "Special function removed.\n\r", ch );
	   if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_PROTOTYPE ) )
	   {
	      victim->pIndexData->spec_fun = NULL;
		STRFREE( victim->pIndexData->spec_funname );
	   }
	   return;
      }

	if( ( specfun = spec_lookup( arg3 ) ) == NULL )
	{
	   send_to_char( "No such function.\n\r", ch );
	   return;
	}

	if( !validate_spec_fun( arg3 ) )
	{
	   ch_printf( ch, "%s is not a valid spec_fun for mobiles.\n\r", arg3 );
	   return;
	}

	victim->spec_fun = specfun;
	STRFREE( victim->spec_funname );
	victim->spec_funname = STRALLOC( arg3 );
      if( IS_NPC( victim ) && IS_ACT_FLAG( victim, ACT_PROTOTYPE ) )
	{
         victim->pIndexData->spec_fun = victim->spec_fun;
	   STRFREE( victim->pIndexData->spec_funname );
	   victim->pIndexData->spec_funname = STRALLOC( arg3 );
	}
	send_to_char( "Victim special function set.\n\r", ch );
	return;
   }

    if ( !str_cmp( arg2, "flags" ) )
    {
        bool pcflag;
	if ( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_SUPREME )
	{
	    send_to_char( "You can only modify a mobile's flags.\n\r", ch );
	    return;
	}

	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: mset <victim> flags <flag> [flag]...\n\r", ch );
	   return;
	}
	while ( argument[0] != '\0' )
	{
           pcflag = FALSE;
	   argument = one_argument( argument, arg3 );
	   value = IS_NPC( victim) ? get_actflag( arg3 ) : get_plrflag( arg3 );

	   if ( !IS_NPC( victim ) && ( value < 0 || value > MAX_BITS ) )
           {
             pcflag = TRUE;
	     value = get_pcflag( arg3 );
           }
	   if ( value < 0 || value > MAX_BITS )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	   else
	   {
	     if ( IS_NPC(victim)
	     &&   value == ACT_PROTOTYPE
	     &&   get_trust( ch ) < 58 
             &&  !is_name("protoflag", ch->pcdata->bestowments) )
		send_to_char( "You cannot change the prototype flag.\n\r", ch );
	     else
	     if ( IS_NPC(victim) && value == ACT_IS_NPC )
		send_to_char( "If that could be changed, it would cause many problems.\n\r", ch );
	     else
	     {
		if ( pcflag )
		    xTOGGLE_BIT( victim->pcdata->flags, 1 << value );
		else
		{ 
		    xTOGGLE_BIT( victim->act, value );
		    /* NPC check added by Gorog */
		    if ( IS_NPC(victim) && value == ACT_PROTOTYPE )
			victim->pIndexData->act = victim->act;
		}
	     }
	   }
	}
	if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->act = victim->act; 
	return;
    }
    if ( !str_cmp( arg2, "powerup" ) )
    {
        if ( IS_NPC( victim ))
        {
            send_to_char( "Mobiles don't have powerups.\n\r", ch );
            return;
        }
        if ( ch->level < 65)
        {
            send_to_char( "Your not supposed to change this.\n\r", ch );
            return;
        }
        if ( !argument || argument[0] == '\0' )
        {
           send_to_char( "Usage: mset <victim> powerup <flag> [flag]...\n\r", ch );
           return;
        }
        while ( argument[0] != '\0' )
        {
           argument = one_argument( argument, arg3 );
           value = get_puflag( arg3 );

           if ( !IS_NPC( victim ) && ( value < 0 || value > MAX_BITS ) )
             value = get_puflag( arg3 );
  
           if ( value < 0 || value > MAX_BITS )
             ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
           else
             xTOGGLE_BIT( victim->powerup, value );
        }
        return;
    }
    if ( !str_cmp( arg2, "skill" ) )
    {
        if ( IS_NPC( victim ))
        {
            send_to_char( "Mobiles don't have ki maintained.\n\r", ch );
            return;
        }
        if ( ch->level < 65)
        {
            send_to_char( "Your not supposed to change this.\n\r", ch );
            return;
        }
        if ( !argument || argument[0] == '\0' )
        {
           send_to_char( "Usage: mset <victim> skill <flag> [flag]...\n\r", ch );
           return;
        }
        while ( argument[0] != '\0' )
        {
           argument = one_argument( argument, arg3 );
           value = get_skillflag( arg3 );

           if ( !IS_NPC( victim ) && ( value < 0 || value > MAX_BITS ) )
             value = get_skillflag( arg3 );
  
           if ( value < 0 || value > MAX_BITS )
             ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
           else
             xTOGGLE_BIT( victim->skill, value );
        }
        return;
    }
    if ( !str_cmp( arg2, "part" ) )
    {
	if ( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
	{
	    send_to_char( "You can only modify a mobile's parts.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Usage: mset <victim> part <flag> [flag]...\n\r", ch );
	    return;
	}
	while ( argument[0] != '\0' )
	{
	    argument = one_argument( argument, arg3 );
	    value = get_partflag( arg3 );
	    if ( value < 0 || value > 31 )
		ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	    else
		TOGGLE_BIT( victim->xflags, 1 << value );
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->xflags = victim->xflags; 
	return;
    }

    if ( !str_cmp( arg2, "attack" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "You can only modify a mobile's attacks.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Usage: mset <victim> attack <flag> [flag]...\n\r", ch );
	    return;
	}
	while ( argument[0] != '\0' )
	{
	    argument = one_argument( argument, arg3 );
	    value = get_attackflag( arg3 );
	    if ( value < 0 || value > MAX_BITS )
		ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	    else
		xTOGGLE_BIT( victim->attacks, value );
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->attacks = victim->attacks; 
	return;
    }

    if ( !str_cmp( arg2, "defense" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "You can only modify a mobile's defenses.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Usage: mset <victim> defense <flag> [flag]...\n\r", ch );
	    return;
	}
	while ( argument[0] != '\0' )
	{
	    argument = one_argument( argument, arg3 );
	    value = get_defenseflag( arg3 );
	    if ( value < 0 || value > MAX_BITS )
		ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	    else
		xTOGGLE_BIT( victim->defenses,  value );
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->defenses = victim->defenses; 
	return;
    }

    if ( !str_cmp( arg2, "pos" ) )
    {
/*	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
*/	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( value < 0 || value > POS_STANDING )
	{
	    ch_printf( ch, "Position range is 0 to %d.\n\r", POS_STANDING );
	    return;
	}
	victim->position = value;
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->position = victim->position; 
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "defpos" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( value < 0 || value > POS_STANDING )
	{
	    ch_printf( ch, "Position range is 0 to %d.\n\r", POS_STANDING );
	    return;
	}
	victim->defposition = value;
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	    victim->pIndexData->defposition = victim->defposition; 
	send_to_char( "Done.\n\r", ch );
	return;
    }

    /*
     * save some finger-leather
     */
    if ( !str_cmp( arg2, "hitdie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;

        sscanf(arg3,"%d %c %d %c %d",&num,&char1,&size,&char2,&plus);
	sprintf(outbuf,"%s hitnumdie %d",arg1, num);
        do_mset( ch, outbuf );

	sprintf(outbuf,"%s hitsizedie %d",arg1, size);
        do_mset( ch, outbuf );

	sprintf(outbuf,"%s hitplus %d",arg1, plus);
        do_mset( ch, outbuf );
        return;
    }
    /*
     * save some more finger-leather
     */
    if ( !str_cmp( arg2, "damdie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;

        sscanf(arg3,"%d %c %d %c %d",&num,&char1,&size,&char2,&plus);
	sprintf(outbuf,"%s damnumdie %d",arg1, num);
        do_mset( ch, outbuf );
	sprintf(outbuf,"%s damsizedie %d",arg1, size);
        do_mset( ch, outbuf );
	sprintf(outbuf,"%s damplus %d",arg1, plus);
        do_mset( ch, outbuf );
        return;
    }

    if ( !str_cmp( arg2, "hitnumdie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Number of hitpoint dice range is 0 to 30000.\n\r", ch );
	    return;
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->hitnodice = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hitsizedie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Hitpoint dice size range is 0 to 30000.\n\r", ch );
	    return;
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->hitsizedice = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hitplus" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Hitpoint bonus range is 0 to 30000.\n\r", ch );
	    return;
	}
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->hitplus = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "damnumdie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Number of damage dice range is 0 to 100.\n\r", ch );
	    return;
	}
	victim->barenumdie = value;
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->damnodice = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "damsizedie" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	    return;
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Damage dice size range is 0 to 100.\n\r", ch );
	    return;
	}
	victim->baresizedie = value;
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->damsizedice = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "damplus" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Mobiles only.\n\r", ch );
	    return;
	}
	if ( !can_mmodify( ch, victim ) )
	  return;
	if ( value < 0 || value > 1000 )
	{
	    send_to_char( "Damage bonus range is 0 to 1000.\n\r", ch );
	    return;
	}
	victim->damplus = value;
	if ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
	  victim->pIndexData->damplus = value;
	send_to_char( "Done.\n\r", ch );
	return;

    }


    if ( !str_cmp( arg2, "aloaded" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Player Characters only.\n\r", ch );
	    return;
	}

	/* Make sure they have an area assigned -Druid */
	if(!victim->pcdata->area)
	{
	  send_to_char( "Player does not have an area assigned to them.\n\r",ch);
	  return;
	}

	if ( !can_mmodify( ch, victim ) )
	  return;

	if ( !IS_SET(victim->pcdata->area->status, AREA_LOADED ) )
	{
	    SET_BIT( victim->pcdata->area->status, AREA_LOADED );
	    send_to_char( "Your area set to LOADED!\n\r", victim );
            if ( ch != victim )
	      send_to_char( "Area set to LOADED!\n\r", ch );
	    return;
	}
	else
	{
	    REMOVE_BIT( victim->pcdata->area->status, AREA_LOADED );
	    send_to_char( "Your area set to NOT-LOADED!\n\r", victim );
	    if ( ch != victim )
              send_to_char( "Area set to NON-LOADED!\n\r", ch );
	    return;
	}
    }
    

    /*
     * Generate usage message.
     */
    if ( ch->substate == SUB_REPEATCMD )
    {
	ch->substate = SUB_RESTRICTED;
	interpret( ch, origarg );
	ch->substate = SUB_REPEATCMD;
	ch->last_cmd = do_mset;
    }
    else
	do_mset( ch, "" );
    return;
}


void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *obj, *tmpobj;
    EXTRA_DESCR_DATA *ed;
    bool lockobj;
    char *origarg = argument;

    int value, tmp;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't oset\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;

	case SUB_OBJ_EXTRA:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_oset: sub_obj_extra: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  /*
	   * hopefully the object didn't get extracted...
	   * if you're REALLY paranoid, you could always go through
	   * the object and index-object lists, searching through the
	   * extra_descr lists for a matching pointer...
	   */
	  ed  = ch->dest_buf;
	  STRFREE( ed->description );
	  ed->description = copy_buffer( ch );
	  tmpobj = ch->spare_ptr;
	  stop_editing( ch );
	  ch->dest_buf = tmpobj;
	  ch->substate = ch->tempnum;
	  return;
	
	case SUB_OBJ_LONG:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_oset: sub_obj_long: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  obj = ch->dest_buf;
	  if ( obj && obj_extracted(obj) )
	  {
		send_to_char( "Your object was extracted!\n\r", ch );
		stop_editing( ch );
		return;
	  }
	  STRFREE( obj->description );
	  obj->description = copy_buffer( ch );
	  if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  {
		if ( can_omodify(ch, obj) )
		{
		  STRFREE( obj->pIndexData->description );
		  obj->pIndexData->description = QUICKLINK( obj->description );
		}
	  }
	  tmpobj = ch->spare_ptr;
	  stop_editing( ch );
	  ch->substate = ch->tempnum;
	  ch->dest_buf = tmpobj;
	  return;
    }

    obj = NULL;
    smash_tilde( argument );

  
    if ( ch->substate == SUB_REPEATCMD )
    {
        obj = ch->dest_buf;
        if ( !obj )
        {
            send_to_char( "Your object was extracted!\n\r", ch );
            argument = "done";
        }

	if ( argument[0] == '\0' || !str_cmp( argument, " " )
	||   !str_cmp( argument, "stat" ) )
	{
	    if ( obj )
		do_ostat( ch, obj->name );
	    else
	        send_to_char( "No object selected.  Type '?' for help.\n\r", ch );
	    return;
	}
	if ( !str_cmp( argument, "done" ) || !str_cmp( argument, "off" ) )
	{

        if ( ch->dest_buf )
               RelDestroy( relOSET_ON, ch, ch->dest_buf );
	    send_to_char( "Oset mode off.\n\r", ch );
	    ch->substate = SUB_NONE;
	    ch->dest_buf = NULL;
	    if ( ch->pcdata && ch->pcdata->subprompt )
	    {
		STRFREE( ch->pcdata->subprompt );
		ch->pcdata->subprompt = NULL;
	    }
	    return;
	}
    }
    if ( obj )
    {
	lockobj = TRUE;
	strcpy( arg1, obj->name );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
    }
    else
    {
	lockobj = FALSE;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );
    }

    if ( !str_cmp( arg1, "on" ) )
    {
	send_to_char( "Syntax: oset <object|vnum> on.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !str_cmp( arg1, "?" ) )
    {
	if ( ch->substate == SUB_REPEATCMD )
	{
	    if ( obj )
		send_to_char( "Syntax: <field>  <value>\n\r",		ch );
	    else
		send_to_char( "Syntax: <object> <field>  <value>\n\r",	ch );
	}
	else
	    send_to_char( "Syntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags wear level weight cost rent timer\n\r",	ch );
	send_to_char( "  name short long ed rmed actiondesc\n\r",	ch );
	send_to_char( "  type value0 value1 value2 value3 value4 value5\n\r",	ch );
	send_to_char( "  layers\n\r",				ch );
	send_to_char( "For weapons:             For armor:\n\r",	ch );
	send_to_char( "  weapontype condition     ac condition\n\r",	ch );
	send_to_char( "For containers:          For levers and switches:\n\r", ch );
	send_to_char( "  cflags key capacity      tflags\n\r",		ch );
	return;
    }

    if ( !obj && get_trust(ch) < LEVEL_GOD )
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	   send_to_char( "You can't find that here.\n\r", ch );
	   return;
	}
    }
    else
    if ( !obj )
    {
	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
	{
	   send_to_char( "There is nothing like that in all the realms.\n\r", ch );
	   return;
 	}
    }
    if ( lockobj )
	ch->dest_buf = obj;

    separate_obj( obj );
    value = atoi( arg3 );

    if ( !str_cmp( arg2, "on" ) )
    {
	CHECK_SUBRESTRICTED( ch );
	ch_printf( ch, "Oset mode on. (Editing '%s' vnum %d).\n\r",	obj->name, obj->pIndexData->vnum );
	ch->substate = SUB_REPEATCMD;
	ch->dest_buf = obj;
	if ( ch->pcdata )
	{
	   if ( ch->pcdata->subprompt )
		STRFREE( ch->pcdata->subprompt );
	   sprintf( buf, "<&COset &W#%d&w> %%i", obj->pIndexData->vnum );
	   ch->pcdata->subprompt = STRALLOC( buf );
	}
    RelCreate( relOSET_ON, ch, obj );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	bool proto = FALSE;

 	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
		proto = TRUE;
	if ( proto && !can_omodify( ch, obj ) )
	  return;
	STRFREE( obj->name );
	obj->name = STRALLOC( arg3 );
	if ( proto )
	{
	   STRFREE(obj->pIndexData->name );
	   obj->pIndexData->name = QUICKLINK( obj->name );
	}
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	{
           if ( !can_omodify( ch, obj ) )
              return;
	   STRFREE( obj->short_descr );
	   obj->short_descr = STRALLOC( arg3 );
	   STRFREE(obj->pIndexData->short_descr );
	   obj->pIndexData->short_descr = QUICKLINK( obj->short_descr );
	}
        else
        /* Feature added by Narn, Apr/96 
	 * If the item is not proto, add the word 'rename' to the keywords
	 * if it is not already there.
	 */
        {
	  STRFREE( obj->short_descr );
	  obj->short_descr = STRALLOC( arg3 );
          if ( str_infix( "rename", obj->name ) )
          {
            sprintf( buf, "%s %s", obj->name, "rename" );
	    STRFREE( obj->name );
	    obj->name = STRALLOC( buf );
          }
        }
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	if ( arg3[0] )
	{
	   if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	   {
              if ( !can_omodify( ch, obj ) )
                return;
	      STRFREE( obj->description );
	      obj->description = STRALLOC( arg3 );
	      STRFREE(obj->pIndexData->description );
	      obj->pIndexData->description = QUICKLINK( obj->description );
	      return;
	   }
	   STRFREE( obj->description );
	   obj->description = STRALLOC( arg3 );
	   return;
	}
	CHECK_SUBRESTRICTED( ch );
	if ( ch->substate == SUB_REPEATCMD )
	  ch->tempnum = SUB_REPEATCMD;
	else
	  ch->tempnum = SUB_NONE;
	if ( lockobj )
	  ch->spare_ptr = obj;
	else
	  ch->spare_ptr = NULL;
	ch->substate = SUB_OBJ_LONG;
	ch->dest_buf = obj;
	start_editing( ch, obj->description );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	if ( !arg3 || arg3[0] == '\0' )
	{
	    send_to_char( "Syntax: oset <object> ed <keywords>\n\r",
		ch );
	    return;
	}
	CHECK_SUBRESTRICTED( ch );
	if ( obj->timer )
	{
	   send_to_char("It's not safe to edit an extra description on an object with a timer.\n\rTurn it off first.\n\r", ch );
	   return;
	}
	if ( obj->item_type == ITEM_PAPER && get_trust(ch) < LEVEL_IMPLEMENTOR )
	{
	   send_to_char("You can not add an extra description to a note paper at the moment.\n\r", ch);
	   return;
	}
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  ed = SetOExtraProto( obj->pIndexData, arg3 );
	else
	  ed = SetOExtra( obj, arg3 );
	if ( ch->substate == SUB_REPEATCMD )
	  ch->tempnum = SUB_REPEATCMD;
	else
	  ch->tempnum = SUB_NONE;
	if ( lockobj )
	  ch->spare_ptr = obj;
	else
	  ch->spare_ptr = NULL;
	ch->substate = SUB_OBJ_EXTRA;
	ch->dest_buf = ed;
	start_editing( ch, ed->description );
	return;
    }

    if ( !str_cmp( arg2, "rmed" ) )
    {
	if ( !arg3 || arg3[0] == '\0' )
	{
	   send_to_char( "Syntax: oset <object> rmed <keywords>\n\r", ch );
	   return;
	}
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	{
	    if ( DelOExtraProto( obj->pIndexData, arg3 ) )
		send_to_char( "Deleted.\n\r", ch );
	    else
		send_to_char( "Not found.\n\r", ch );
	    return;
	}
	if ( DelOExtra( obj, arg3 ) )
	  send_to_char( "Deleted.\n\r", ch );
	else
	  send_to_char( "Not found.\n\r", ch );
	return;
    }

    if ( get_trust(ch) < LEVEL_DEMI )
    {
	send_to_char("You can only oset the name, short and long right now.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[0] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[0] = value;
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[1] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[2] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[3] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[3] = value;
	return;
    }
    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[4] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[4] = value;
	return;
    }

    if ( !str_cmp( arg2, "value5" ) || !str_cmp( arg2, "v5" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[5] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[5] = value;
	return;
    }

    if ( !str_cmp( arg2, "type" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: oset <object> type <type>\n\r", ch );
	   return;
	}
	value = get_otype( argument );
	if ( value < 1 )
	{
	     ch_printf( ch, "Unknown type: %s\n\r", arg3 );
	     return;	
	}
	obj->item_type = (sh_int) value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->item_type = obj->item_type; 
	return;	
    }

    if ( !str_cmp( arg2, "flags" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: oset <object> flags <flag> [flag]...\n\r", ch );
	   return;
	}
	while ( argument[0] != '\0' )
	{
	   argument = one_argument( argument, arg3 );
	   value = get_oflag( arg3 );
	   if ( value < 0 || value > MAX_BITS )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	   else
	   {
	     if ( value == ITEM_PROTOTYPE
	     &&   get_trust( ch ) < 58 
             &&   !is_name ("protoflag", ch->pcdata->bestowments) )
	       send_to_char( "You cannot change the prototype flag.\n\r", ch );
	     else
	     {
	       xTOGGLE_BIT(obj->extra_flags, value);
	       if ( value == ITEM_PROTOTYPE )
	         obj->pIndexData->extra_flags = obj->extra_flags;
	     }
	   }
	}
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->extra_flags = obj->extra_flags; 
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: oset <object> wear <flag> [flag]...\n\r", ch );
	   return;
	}
	while ( argument[0] != '\0' )
	{
	   argument = one_argument( argument, arg3 );
	   value = get_wflag( arg3 );
	   if ( value < 0 || value > 31 )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	   else
	     TOGGLE_BIT( obj->wear_flags, 1 << value );
	}

	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->wear_flags = obj->wear_flags;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->level = value;
	return;
    }

    if ( !str_cmp( arg2, "weight" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->weight = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->cost = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->cost = value;
	return;
    }

    if ( !str_cmp( arg2, "rent" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->rent = value;
	else
	  send_to_char( "Item must have prototype flag to set this value.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "layers" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->layers = value;
	else
	  send_to_char( "Item must have prototype flag to set this value.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->timer = value;
	return;
    }
	
    if ( !str_cmp( arg2, "actiondesc" ) )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	if ( strstr( arg3, "%n" )
	||   strstr( arg3, "%d" )
	||   strstr( arg3, "%l" ) )
	{
	   send_to_char( "Illegal characters!\n\r", ch );
	   return;
	}
	STRFREE( obj->action_desc );
	obj->action_desc = STRALLOC( arg3 );
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	{
	   STRFREE(obj->pIndexData->action_desc );
	   obj->pIndexData->action_desc = QUICKLINK( obj->action_desc );
	}
	return;
    }


    /*
     * Make it easier to set special object values by name than number
     * 						-Thoric
     */
    tmp = -1;
    switch( obj->item_type )
    {
	case ITEM_WEAPON:
	    if ( !str_cmp( arg2, "weapontype" ) )
	    {
		int x;

		value = -1;
		for ( x = 0; x < sizeof( attack_table ) / sizeof( attack_table[0] ); x++ )
		  if ( !str_cmp( arg3, attack_table[x] ) )
		    value = x;
		if ( value < 0 )
		{
		    send_to_char( "Unknown weapon type.\n\r", ch );
		    return;
		}
		tmp = 3;
		break;
	    }
            if ( !str_cmp( arg2, "wtype" ) )             tmp = 0;
            if ( !str_cmp( arg2, "pdef" )       )       tmp = 1;
            if ( !str_cmp( arg2, "mdef" )       )       tmp = 2;
            if ( !str_cmp( arg2, "grade" )      )       tmp = 3;
            if ( !str_cmp( arg2, "status" )     )       tmp = 4;
            if ( !str_cmp( arg2, "speed" )     )        tmp = 5;
	    break;
	case ITEM_ARMOR:
	case ITEM_JEWELRY:
	    if ( !str_cmp( arg2, "atype" ) )		tmp = 0;
	    if ( !str_cmp( arg2, "pdef" )	)	tmp = 1;
            if ( !str_cmp( arg2, "mdef" )       )       tmp = 2;
            if ( !str_cmp( arg2, "grade" )      )       tmp = 3;
            if ( !str_cmp( arg2, "status" )     )       tmp = 4;

	    break;
	case ITEM_SALVE:
	    if ( !str_cmp( arg2, "slevel"   ) )		tmp = 0;
	    if ( !str_cmp( arg2, "maxdoses" ) )		tmp = 1;
	    if ( !str_cmp( arg2, "doses"    ) )		tmp = 2;
	    if ( !str_cmp( arg2, "delay"    ) )		tmp = 3;
	    break;
	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
	    if ( !str_cmp( arg2, "slevel" ) )		tmp = 0;
	    break;
	case ITEM_STAFF:
	case ITEM_WAND:
	    if ( !str_cmp( arg2, "slevel" ) )		tmp = 0;
	    if ( !str_cmp( arg2, "maxcharges" )	)	tmp = 1;
	    if ( !str_cmp( arg2, "charges" ) )		tmp = 2;
	    break;
	case ITEM_CONTAINER:
	    if ( !str_cmp( arg2, "capacity" ) )		tmp = 0;
	    if ( !str_cmp( arg2, "cflags" ) )		tmp = 1;
	    if ( !str_cmp( arg2, "key" ) )		tmp = 2;
	    break;
	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
	case ITEM_BUTTON:
	    if ( !str_cmp( arg2, "tflags" ) )
	    {
	    	tmp = 0;
		value = get_trigflag(arg3);
	    }
	    break;
    }
    if ( tmp >= 0 && tmp <= 5 )
    {
	if ( !can_omodify( ch, obj ) )
	  return;
	obj->value[tmp] = value;
	if ( IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
	  obj->pIndexData->value[tmp] = value;
	return;
    }

    /*
     * Generate usage message.
     */
    if ( ch->substate == SUB_REPEATCMD )
    {
	ch->substate = SUB_RESTRICTED;
	interpret( ch, origarg );
	ch->substate = SUB_REPEATCMD;
	ch->last_cmd = do_oset;
    }
    else
	do_oset( ch, "" );
    return;
}


/*
 * Obsolete Merc room editing routine
 */
void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;
    bool proto;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags sector\n\r",				ch );
	return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( !can_rmodify( ch, location ) )
      return;

    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	/*
	 * Protect from messing up prototype flag
	 */
	if ( IS_SET( location->room_flags, ROOM_PROTOTYPE ) )
	  proto = TRUE;
	else
	  proto = FALSE;
	location->room_flags	= value;
	if ( proto )
	  SET_BIT( location->room_flags, ROOM_PROTOTYPE );
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}

/*
 * Returns value 0 - 9 based on directional text.
 */
int get_dir( char *txt )
{
    int edir;
    char c1,c2;

    if ( !str_cmp( txt, "northeast" ) )
      return DIR_NORTHEAST;
    if ( !str_cmp( txt, "northwest" ) )
      return DIR_NORTHWEST;
    if ( !str_cmp( txt, "southeast" ) )
      return DIR_SOUTHEAST;
    if ( !str_cmp( txt, "southwest" ) )
      return DIR_SOUTHWEST;
    if ( !str_cmp( txt, "somewhere" ) )
      return 10;

    c1 = txt[0];
    if ( c1 == '\0' )
      return -1;
    c2 = txt[1];
    edir = 0;
    switch ( c1 )
    {
	  case 'n':
	  switch ( c2 )
	  {
		    default:   edir = 0; break;	/* north */
		    case 'e':  edir = 6; break; /* ne	 */
		    case 'w':  edir = 7; break; /* nw	 */
	  }
	  break;    case '0':  edir = 0; break; /* north */
	  case 'e': case '1':  edir = 1; break; /* east  */
	  case 's':
	  switch ( c2 )
	  {
		    default:   edir = 2; break; /* south */
		    case 'e':  edir = 8; break; /* se	 */
		    case 'w':  edir = 9; break; /* sw	 */
	  }
	  break;    case '2':  edir = 2; break; /* south */
	  case 'w': case '3':  edir = 3; break; /* west	 */
	  case 'u': case '4':  edir = 4; break; /* up	 */
	  case 'd': case '5':  edir = 5; break; /* down	 */
		    case '6':  edir = 6; break; /* ne	 */
		    case '7':  edir = 7; break; /* nw	 */
		    case '8':  edir = 8; break; /* se	 */
		    case '9':  edir = 9; break; /* sw	 */
		    case '?':  edir = 10;break; /* somewhere */
    }
    return edir;
}


void do_redit( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MIL];
    char buf [MAX_STRING_LENGTH];
    ROOM_INDEX_DATA	*location, *tmp;
    EXTRA_DESCR_DATA	*ed;
    char		dir;
    EXIT_DATA		*xit, *texit;
    int			edir, ekey, evnum;
    int			value;
    char		*origarg = argument;
    value =0;
    set_char_color( AT_PLAIN, ch );
    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor.\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_ROOM_DESC:
	  location = ch->dest_buf;
	  if ( !location )
	  {
		bug( "redit: sub_room_desc: NULL ch->dest_buf", 0 );
		location = ch->in_room;
	  }
	  STRFREE( location->description );
	  location->description = copy_buffer( ch );
	  stop_editing( ch );
	  ch->substate = ch->tempnum;
	  return;
	case SUB_ROOM_EXTRA:
	  ed = ch->dest_buf;
	  if ( !ed )
	  {
		bug( "redit: sub_room_extra: NULL ch->dest_buf", 0 );
		stop_editing( ch );
		return;
	  }
	  STRFREE( ed->description );
	  ed->description = copy_buffer( ch );
	  stop_editing( ch );
	  ch->substate = ch->tempnum;
	  return;
    }

    location = ch->in_room;

    smash_tilde( argument );
    argument = one_argument( argument, arg );
    if ( ch->substate == SUB_REPEATCMD )
    {
	if ( arg[0] == '\0' )
	{
	    do_rstat( ch, "" );
	    return;
	}
	if ( !str_cmp( arg, "done" ) || !str_cmp( arg, "off" ) )
	{
	    send_to_char( "Redit mode off.\n\r", ch );
	    if ( ch->pcdata && ch->pcdata->subprompt )
	    {
		STRFREE( ch->pcdata->subprompt );
		ch->pcdata->subprompt = NULL;
	    }
	    ch->substate = SUB_NONE;
	    return;
	}
    }
    if ( arg[0] == '\0' || !str_cmp( arg, "?" ) )
    {
	if ( ch->substate == SUB_REPEATCMD )
	    send_to_char( "Syntax: <field> value\n\r",			ch );
	else
	    send_to_char( "Syntax: redit <field> value\n\r",		ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  name desc ed rmed\n\r",			ch );
	send_to_char( "  exit bexit exdesc exflags exname exkey\n\r",	ch );
	send_to_char( "  flags sector teledelay televnum tunnel\n\r",	ch );
	send_to_char( "  rlist exdistance pulltype pull push clan\n\r",		ch );
	send_to_char( "  exit bexit exdesc exflags exname exkey excoord\n\r",	ch );
	return;
    }

    if ( !can_rmodify( ch, location ) )
	return;

    if ( !str_cmp( arg, "on" ) )
    {
	CHECK_SUBRESTRICTED( ch );
	send_to_char( "Redit mode on.\n\r", ch );
	ch->substate = SUB_REPEATCMD;
	if ( ch->pcdata )
	{
	   if ( ch->pcdata->subprompt )
		STRFREE( ch->pcdata->subprompt );
	   ch->pcdata->subprompt = STRALLOC( "<&CRedit &W#%r&w> %i" );
	}
	return;
    }
    if ( !str_cmp( arg, "substate" ) )
    {
	  argument = one_argument( argument, arg2);
          if( !str_cmp( arg2, "north" )  )
	  {
               ch->inter_substate = SUB_NORTH; 
	       return;
	  }
          if( !str_cmp( arg2, "east" )  )
	  {
               ch->inter_substate = SUB_EAST; 
	       return;
	  }
          if( !str_cmp( arg2, "south" )  )
	  {
               ch->inter_substate = SUB_SOUTH; 
	       return;
	  }
          if( !str_cmp( arg2, "west" )  )
	  {
               ch->inter_substate = SUB_WEST; 
	       return;
	  }
          if( !str_cmp( arg2, "up" )  )
	  {
               ch->inter_substate = SUB_UP; 
	       return;
	  }
          if( !str_cmp( arg2, "down" )  )
	  {
               ch->inter_substate = SUB_DOWN; 
	       return;
	  }
          send_to_char( " unrecognized substate in redit\n\r", ch);
	  return;
    }


    if ( !str_cmp( arg, "name" ) )
    {
	if ( argument[0] == '\0' )
	{
	   send_to_char( "Set the room name.  A very brief single line room description.\n\r", ch );
	   send_to_char( "Usage: redit name <Room summary>\n\r", ch );
	   return;
	}
	STRFREE( location->name );
	location->name = STRALLOC( argument );
	return;
    }

    if ( !str_cmp( arg, "desc" ) )
    {
	if ( ch->substate == SUB_REPEATCMD )
	    ch->tempnum = SUB_REPEATCMD;
	else
	    ch->tempnum = SUB_NONE;
	ch->substate = SUB_ROOM_DESC;
	ch->dest_buf = location;
	start_editing( ch, location->description );
	return;
    }

    if ( !str_cmp( arg, "tunnel" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Set the maximum characters allowed in the room at one time. (0 = unlimited).\n\r", ch );
	    send_to_char( "Usage: redit tunnel <value>\n\r", ch );
	    return;
	}
	location->tunnel = URANGE( 0, atoi(argument), 1000 );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "ed" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Create an extra description.\n\r", ch );
	    send_to_char( "You must supply keyword(s).\n\r", ch );
	    return;
	}
	CHECK_SUBRESTRICTED( ch );
	ed = SetRExtra( location, argument );
	if ( ch->substate == SUB_REPEATCMD )
	    ch->tempnum = SUB_REPEATCMD;
	else
	    ch->tempnum = SUB_NONE;
	ch->substate = SUB_ROOM_EXTRA;
	ch->dest_buf = ed;
	start_editing( ch, ed->description );
	return;
    }

    if ( !str_cmp( arg, "rmed" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Remove an extra description.\n\r", ch );
	    send_to_char( "You must supply keyword(s).\n\r", ch );
	    return;
	}
	if ( DelRExtra( location, argument ) )
	    send_to_char( "Deleted.\n\r", ch );
	else
	    send_to_char( "Not found.\n\r", ch );
	return;
    }
   if( !str_cmp( arg, "clan" ) )
   {
      CLAN_DATA *clan;
      
      clan = get_clan( argument );
      if ( !clan )
      {
         send_to_char( "Invalid Clan.\n\r", ch);
         return;
      }
      STRFREE( location->clan );
      location->clan = STRALLOC( clan->shortname );
      send_to_char( "Done.\n\r", ch);      
      return;
   }
   if( !str_cmp( arg, "rlist" ) )
   {
      RESET_DATA *pReset;
      char *rbuf;
      short num;

      if( !location->first_reset )
      {
         send_to_char( "This room has no resets to list.\n\r", ch );
         return;
      }
      num = 0;
      for( pReset = location->first_reset; pReset; pReset = pReset->next )
      {
         num++;
         if( !( rbuf = sprint_reset( pReset, &num ) ) )
            continue;
         send_to_char( rbuf, ch );
      }
      return;
   }
    if ( !str_cmp( arg, "flags" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Toggle the room flags.\n\r", ch );
	    send_to_char( "Usage: redit flags <flag> [flag]...\n\r", ch );
	    return;
	}
	while ( argument[0] != '\0' )
	{
	    argument = one_argument( argument, arg2 );
	    value = get_rflag( arg2 );
	    if ( value < 0 || value > 31 )
		ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
	    else
	    {
		if ( 1 << value == ROOM_PROTOTYPE
		&&   get_trust( ch ) < 58 )
		    send_to_char( "You cannot change the prototype flag.\n\r", ch );
		else
		    TOGGLE_BIT( location->room_flags, 1 << value );
	    }
	}
	return;
    }
    if ( !str_cmp( arg, "remflag" ) )
    {
        if ( !argument || argument[0] == '\0' )
        {
            send_to_char( "Toggle the room flags.\n\r", ch );
            send_to_char( "Usage: redit remflag <flag> [flag]...\n\r", ch );
            return;
        }
        while ( argument[0] != '\0' )
        {
            argument = one_argument( argument, arg2 );
            value = get_rflag( arg2 );
            if ( value < 0 || value > 31 )
                ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
            else
            {
                if ( 1 << value == ROOM_PROTOTYPE
                &&   get_trust( ch ) < 58 )
                    send_to_char( "You cannot change the prototype flag.\n\r", ch );
                else
                    REMOVE_BIT( location->room_flags, 1 << value );
            }
        }
        return;
    }
    if ( !str_cmp( arg, "setflag" ) )
    {
        if ( !argument || argument[0] == '\0' )
        {
            send_to_char( "Toggle the room flags.\n\r", ch );
            send_to_char( "Usage: redit setflag <flag> [flag]...\n\r", ch );
            return;
        }
        while ( argument[0] != '\0' )
        {
            argument = one_argument( argument, arg2 );
            value = get_rflag( arg2 );
            if ( value < 0 || value > 31 )
                ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
            else
            {
                if ( 1 << value == ROOM_PROTOTYPE
                &&   get_trust( ch ) < 58 )
                    send_to_char( "You cannot change the prototype flag.\n\r", ch );
                else
                    SET_BIT( location->room_flags, 1 << value );
            }
        }
        return;
    }
    if ( !str_cmp( arg, "remflag2" ) )
    {
        if ( !argument || argument[0] == '\0' )
        {
            send_to_char( "Toggle the room flags.\n\r", ch );
            send_to_char( "Usage: redit remflag2 <flag> [flag]...\n\r", ch );
            return;
        }
        while ( argument[0] != '\0' )
        {
            argument = one_argument( argument, arg2 );
            value = get_rflag2( arg2 );
            if ( value < 0 || value > 31 )
                ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
            else
            {
                if ( 1 << value == ROOM_PROTOTYPE
                &&   get_trust( ch ) < 58 )
                    send_to_char( "You cannot change the prototype flag.\n\r", ch );
                else
                    REMOVE_BIT( location->room_flags2, 1 << value );
            }
        }
        return;
    }
    if ( !str_cmp( arg, "setflag2" ) )
    {
        if ( !argument || argument[0] == '\0' )
        {
            send_to_char( "Toggle the room flags.\n\r", ch );
            send_to_char( "Usage: redit setflag2 <flag> [flag]...\n\r", ch );
            return;
        }
        while ( argument[0] != '\0' )
        {
            argument = one_argument( argument, arg2 );
            value = get_rflag2( arg2 );
            if ( value < 0 || value > 31 )
                ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
            else
            {
                if ( 1 << value == ROOM_PROTOTYPE
                &&   get_trust( ch ) < 58 )
                    send_to_char( "You cannot change the prototype flag.\n\r", ch );
                else
                    SET_BIT( location->room_flags2, 1 << value );
            }
        }
        return;
    }

   if ( !str_cmp( arg, "flags2" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Toggle the room flags.\n\r", ch );
	   send_to_char( "Usage: redit flags <flag> [flag]...\n\r", ch );
           send_to_char( "\n\rPossible Flags: \n\r", ch );
           send_to_char( "arena nosense nosnipe control empty_home plr_home spar gravity heal noexit\n\r", ch );     
           return;
	}
	while ( argument[0] != '\0' )
	{
	   argument = one_argument( argument, arg2 );
	   value = get_rflag2( arg2 );
	   if ( value < 0 || value > 31 )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
           else if ( 1 << value == ROOM_PLR_HOME && get_trust(ch) < 65 )
               send_to_char( "If you want to build a player home use the 'empty_home' flag instead.\n\r", ch );
           else
	       TOGGLE_BIT( location->room_flags2, 1 << value );
        }
	return;
    }
    if ( !str_cmp( arg, "teledelay" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Set the delay of the teleport. (0 = off).\n\r", ch );
	    send_to_char( "Usage: redit teledelay <value>\n\r", ch );
	    return;
	}
	location->tele_delay = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "televnum" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Set the vnum of the room to teleport to.\n\r", ch );
	    send_to_char( "Usage: redit televnum <vnum>\n\r", ch );
	    return;
	}
	location->tele_vnum = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "sector" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Set the sector type.\n\r", ch );
	    send_to_char( "Usage: redit sector <value>\n\r", ch );
	    return;
	}
	location->sector_type = atoi( argument );
	if ( location->sector_type < 0 || location->sector_type >= SECT_MAX )
	{
	    location->sector_type = 1;
	    send_to_char( "Out of range.\n\r", ch ); 
	}
	else
	    send_to_char( "Done.\n\r", ch );
	return;
    }
/*    if ( !str_cmp( arg, "keypad" ) )
    {
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Usage: redit keypad <dir> #####\n\r", ch );
            return;
        }

        if ( arg2[0] == '#' )
        {
            edir = atoi( arg2+1 );
            xit = get_exit_num( location, edir );
        }
        else
        {
            edir = get_dir( arg2 );
            xit = get_exit( location, edir );
        }
        value = atoi( arg3 );
        if ( strlen(arg3) == 0 )
        {
            send_to_char( "Removing Keypad Number.\n\r", ch );
            xit->keypad = 0;
            return;
        }
        if ( strlen(arg3) != 5 )
        {
            send_to_char( "Usage: redit keypad <dir> #####\n\r", ch );
            return;
        }
        if (!is_number(arg3))
        {
            send_to_char( "Usage: redit keypad <dir> #####\n\r", ch );
            return;
        }
        if ( !xit )
        {
            send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
            return;
        }
        xit->keypad = value;
        send_to_char( "Done.\n\r", ch );
        return;
    }
*/
    if ( !str_cmp( arg, "exkey" ) )
    {
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	if ( arg2[0] == '\0' || arg3[0] == '\0' )
	{
	    send_to_char( "Usage: redit exkey <dir> <key vnum>\n\r", ch );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	value = atoi( arg3 );
	if ( !xit )
	{
	    send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	    return;
	}
	xit->key = value;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "excoord" ) )
    {
	int x, y, map;

	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );
	if ( arg2[0] == '\0' || arg3[0] == '\0' || argument[0] == '\0' )
	{
	    send_to_char( "Usage: redit excoord <dir> <X> <Y> <map>\n\r", ch );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}

	x = atoi( arg3 );
        y = atoi( arg4 );
        map = atoi( argument );
        if( x < 0 || x >= MAX_X )
	{
	    ch_printf( ch, "Valid X coordinates are 0 to %d.\n\r", MAX_X - 1 );
	    return;
	}

	if( y < 0 || y >= MAX_Y )
	{
	    ch_printf( ch, "Valid Y coordinates are 0 to %d.\n\r", MAX_Y - 1 );
	    return;
	}
        if( map < 0 || map >= MAP_MAX )
        {
            ch_printf( ch, "Valid Map coordinates are 0 to %d.\n\r", MAP_MAX );
            return;
        }

	if ( !xit )
	{
	    send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	    return;
	}
	xit->x = x;
	xit->y = y;
        xit->map = map;
	send_to_char( "Exit coordinates set.\n\r", ch );
	return;
    }


    if ( !str_cmp( arg, "exname" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Change or clear exit keywords.\n\r", ch );
	    send_to_char( "Usage: redit exname <dir> [keywords]\n\r", ch );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	if ( !xit )
	{
	    send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	    return;
	}
	STRFREE( xit->keyword );
	xit->keyword = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "exflags" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Toggle or display exit flags.\n\r", ch );
	    send_to_char( "Usage: redit exflags <dir> <flag> [flag]...\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	if ( !xit )
	{
	    send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	    return;
	}
	if ( argument[0] == '\0' )
	{
            ch_printf( ch, "\n\rMap Stats:  X: %d    Y: %d   Map: %d\n\r", xit->x, xit->y, xit->map);
	    sprintf( buf, "Flags for exit direction: %d  Keywords: %s  Key: %d KeyPad: %d\n\r[ ",
		xit->vdir, xit->keyword, xit->key, xit->keypad );
	    for ( value = 0; value <= MAX_EXFLAG; value++ )
	    {
		if ( IS_SET( xit->exit_info, 1 << value ) )
		{
		    strcat( buf, ex_flags[value] );
		    strcat( buf, " " );
		}
	    }
	    strcat( buf, "]\n\r" );
	    send_to_char( buf, ch );
	    return;
	}
	while ( argument[0] != '\0' )
	{
	    argument = one_argument( argument, arg2 );
	    value = get_exflag( arg2 );
	    if ( value < 0 || value > MAX_EXFLAG )
		ch_printf( ch, "Unknown flag: %s\n\r", arg2 );
	    else
		TOGGLE_BIT( xit->exit_info, 1 << value );
	}
	return;
    }



    if ( !str_cmp( arg, "ex_flags" ) )
    {
	argument = one_argument( argument, arg2 );
        switch(ch->inter_substate)
	{
	    case SUB_EAST : dir = 'e'; edir = 1; break;
	    case SUB_WEST : dir = 'w'; edir = 3; break;
	    case SUB_SOUTH: dir = 's'; edir = 2; break;
	    case SUB_UP   : dir = 'u'; edir = 4; break;
	    case SUB_DOWN : dir = 'd'; edir = 5; break;
	    default:
	    case SUB_NORTH: dir = 'n'; edir = 0; break;
	}

	value = get_exflag(arg2);
        if ( value < 0 )
	{
	    send_to_char("Bad exit flag. \n\r", ch);
	    return;
	}
	if ( (xit = get_exit(location,edir)) == NULL )
	{ 
	    sprintf(buf,"exit %c 1",dir);
	    do_redit(ch,buf);
	    xit = get_exit(location,edir);
	}     
	TOGGLE_BIT( xit->exit_info, 1 << value );
	return;
    }


    if ( !str_cmp( arg, "ex_to_room" ) )
    {
	argument = one_argument( argument, arg2 );
        switch(ch->inter_substate)
	{
	    case SUB_EAST : dir = 'e'; edir = 1; break;
	    case SUB_WEST : dir = 'w'; edir = 3; break;
	    case SUB_SOUTH: dir = 's'; edir = 2; break;
	    case SUB_UP   : dir = 'u'; edir = 4; break;
	    case SUB_DOWN : dir = 'd'; edir = 5; break;
	    default:
	    case SUB_NORTH: dir = 'n'; edir = 0; break;
	}
	evnum = atoi(arg2);
//	if ( evnum < 1 || evnum > 32766 )
	if ( evnum < 1 || evnum > 1048575999 )
	{
	    send_to_char( "Invalid room number.\n\r", ch );
	    return;
	}
	if ( (tmp = get_room_index( evnum )) == NULL )
	{
	    send_to_char( "Non-existant room.\n\r", ch );
	    return;
	}
	if ( (xit = get_exit(location,edir)) == NULL )
	{ 
	    sprintf(buf,"exit %c 1",dir);
	    do_redit(ch,buf);
	    xit = get_exit(location,edir);
	}     
	xit->vnum = evnum;
	return;
    }

    if ( !str_cmp( arg, "ex_key" ) )
    {
	argument = one_argument( argument, arg2 );
        switch(ch->inter_substate)
	{
	    case SUB_EAST : dir = 'e'; edir = 1; break;
	    case SUB_WEST : dir = 'w'; edir = 3; break;
	    case SUB_SOUTH: dir = 's'; edir = 2; break;
	    case SUB_UP   : dir = 'u'; edir = 4; break;
	    case SUB_DOWN : dir = 'd'; edir = 5; break;
	    default:
	    case SUB_NORTH: dir = 'n'; edir = 0; break;
	}
	if ( (xit = get_exit(location,edir)) == NULL )
	{ 
	    sprintf(buf,"exit %c 1",dir);
	    do_redit(ch,buf);
	    xit = get_exit(location,edir);
	}     
	xit->key = atoi( arg2 );
	return;
    }

    if ( !str_cmp( arg, "ex_exdesc" ) )  
    {
        switch(ch->inter_substate)
	{
	    case SUB_EAST : dir = 'e'; edir = 1; break;
	    case SUB_WEST : dir = 'w'; edir = 3; break;
	    case SUB_SOUTH: dir = 's'; edir = 2; break;
	    case SUB_UP   : dir = 'u'; edir = 4; break;
	    case SUB_DOWN : dir = 'd'; edir = 5; break;
	    default:
	    case SUB_NORTH: dir = 'n'; edir = 0; break;
	}
	if ( (xit = get_exit(location, edir)) == NULL )
	{ 
	    sprintf(buf,"exit %c 1",dir);
	    do_redit(ch,buf);
	}     
	sprintf(buf,"exdesc %c %s",dir,argument);
	do_redit(ch,buf);
	return;
    }

    if ( !str_cmp( arg, "ex_keywords" ) )  /* not called yet */
    {
        switch(ch->inter_substate)
	{
	    case SUB_EAST : dir = 'e'; edir = 1; break;
	    case SUB_WEST : dir = 'w'; edir = 3; break;
	    case SUB_SOUTH: dir = 's'; edir = 2; break;
	    case SUB_UP   : dir = 'u'; edir = 4; break;
	    case SUB_DOWN : dir = 'd'; edir = 5; break;
	    default:
	    case SUB_NORTH: dir = 'n'; edir = 0; break;
	}
	if ( (xit = get_exit(location, edir)) == NULL )
	{ 
	    sprintf(buf, "exit %c 1", dir);
	    do_redit(ch,buf);
	    if ( (xit = get_exit(location, edir)) == NULL )
		return;
	}     
	sprintf( buf, "%s %s", xit->keyword, argument );
	STRFREE( xit->keyword );
	xit->keyword = STRALLOC( buf );
	return;
    }

    if ( !str_cmp( arg, "exit" ) )
    {
	bool addexit, numnotdir;

	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	    send_to_char( "Create, change or remove an exit.\n\r", ch );
	    send_to_char( "Usage: redit exit <dir> [room] [flags] [key] [keywords]\n\r", ch );
	    return;
	}
	addexit = numnotdir = FALSE;
	switch( arg2[0] )
	{
	    default:	edir = get_dir(arg2);			  break;
	    case '+':	edir = get_dir(arg2+1);	addexit = TRUE;	  break;
	    case '#':	edir = atoi(arg2+1);	numnotdir = TRUE; break;  
	}
	if ( !arg3 || arg3[0] == '\0' )
	    evnum = 0;
	else
	    evnum = atoi( arg3 );
	if ( numnotdir )
	{
	    if ( (xit = get_exit_num(location, edir)) != NULL )
	      edir = xit->vdir;
	}
	else
	    xit = get_exit(location, edir);
	if ( !evnum )
	{
	    if ( xit )
	    {
		extract_exit(location, xit);
		send_to_char( "Exit removed.\n\r", ch );
		return;
	    }
	    send_to_char( "No exit in that direction.\n\r", ch );
	    return;
	}
//	if ( evnum < 1 || evnum > 32766 )
	if ( evnum < 1 || evnum > 1048575999 )
	{
	    send_to_char( "Invalid room number.\n\r", ch );
	    return;
	}
	if ( (tmp = get_room_index( evnum )) == NULL )
	{
	    send_to_char( "Non-existant room.\n\r", ch );
	    return;
	}
	if ( addexit || !xit )
	{
	    if ( numnotdir )
	    {
		send_to_char( "Cannot add an exit by number, sorry.\n\r", ch );
		return;
	    }
	    if ( addexit && xit && get_exit_to(location, edir, tmp->vnum) )
	    {
		send_to_char( "There is already an exit in that direction leading to that location.\n\r", ch );
		return;
	    }
	    xit = make_exit( location, tmp, edir );
	    xit->keyword		= STRALLOC( "" );
	    xit->description		= STRALLOC( "" );
	    xit->key			= -1;
	    xit->exit_info		= 0;
	    act( AT_IMMORT, "$n reveals a hidden passage!", ch, NULL, NULL, TO_ROOM );
	}
	else
	    act( AT_IMMORT, "Something is different...", ch, NULL, NULL, TO_ROOM );
	if ( xit->to_room != tmp )
	{
	    xit->to_room = tmp;
	    xit->vnum = evnum;
	    texit = get_exit_to( xit->to_room, rev_dir[edir], location->vnum );
	    if ( texit )
	    {
		texit->rexit = xit;
		xit->rexit = texit;
	    }
	}
	argument = one_argument( argument, arg3 );
	if ( arg3 && arg3[0] != '\0' )
	    xit->exit_info = atoi( arg3 );
	if ( argument && argument[0] != '\0' )
	{
	    one_argument( argument, arg3 );
	    ekey = atoi( arg3 );
	    if ( ekey != 0 || arg3[0] == '0' )
	    {
		argument = one_argument( argument, arg3 );
		xit->key = ekey;
	    }
	    if ( argument && argument[0] != '\0' )
	    {
		STRFREE( xit->keyword );
		xit->keyword = STRALLOC( argument );
	    }
	}
	send_to_char( "Done.\n\r", ch );
	return;
    }

    /*
     * Twisted and evil, but works				-Thoric
     * Makes an exit, and the reverse in one shot.
     */
    if ( !str_cmp( arg, "bexit" ) )
    {
	EXIT_DATA *xit, *rxit;
	ROOM_INDEX_DATA *tmploc;
	int vnum, exnum;
	char rvnum[MAX_INPUT_LENGTH];
	bool numnotdir;

	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	    send_to_char( "Create, change or remove a two-way exit.\n\r", ch );
	    send_to_char( "Usage: redit bexit <dir> [room] [flags] [key] [keywords]\n\r", ch );
	    return;
	}
	numnotdir = FALSE;
	switch( arg2[0] )
	{
	    default:
		edir = get_dir( arg2 );
		break;
	    case '#':
		numnotdir = TRUE;
		edir = atoi( arg2+1 );
		break;
	    case '+':
		edir = get_dir( arg2+1 );
		break;
	}
	tmploc = location;
	exnum = edir;
	if ( numnotdir )
	{
	    if ( (xit = get_exit_num(tmploc, edir)) != NULL )
		edir = xit->vdir;
	}
	else
	    xit = get_exit(tmploc, edir);
	rxit = NULL;
	vnum = 0;
	rvnum[0] = '\0';
	if ( xit )
	{
	    vnum = xit->vnum;
	    if ( arg3[0] != '\0' )
		sprintf( rvnum, "%d", tmploc->vnum );
	    if ( xit->to_room )
		rxit = get_exit(xit->to_room, rev_dir[edir]);
	    else
		rxit = NULL;
	}
/*	sprintf( tmpcmd, "exit %s %s %s", arg2, arg3, argument );
	do_redit( ch, tmpcmd );*/
        command( ch, do_redit, "exit %s %s %s", arg2, arg3, argument );
	if ( numnotdir )
	    xit = get_exit_num(tmploc, exnum);
	else
	    xit = get_exit(tmploc, edir);
	if ( !rxit && xit )
	{
	    vnum = xit->vnum;
	    if ( arg3[0] != '\0' )
		sprintf( rvnum, "%d", tmploc->vnum );
	    if ( xit->to_room )
		rxit = get_exit(xit->to_room, rev_dir[edir]);
	    else
		rxit = NULL;
	}
	if ( vnum )
	{
/*	    sprintf( tmpcmd, "%d redit exit %d %s %s",
				vnum,
				rev_dir[edir],
				rvnum,
				argument );
	    do_at( ch, tmpcmd );*/
          command( ch, do_at, "%d redit exit %d %s %s", vnum, rev_dir[edir], rvnum, argument);
	}
	return;
    }

    if ( !str_cmp(arg, "pulltype") || !str_cmp(arg, "pushtype") )
    {
	int pt;

	argument = one_argument( argument, arg2 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	    ch_printf( ch, "Set the %s between this room, and the destination room.\n\r", arg );
	    ch_printf( ch, "Usage: redit %s <dir> <type>\n\r", arg );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	if ( xit )
	{
	    if ( (pt=get_pulltype(argument)) == -1 )
		ch_printf( ch, "Unknown pulltype: %s.  (See help PULLTYPES)\n\r", argument );
	    else
	    {
		xit->pulltype = pt;
		send_to_char( "Done.\n\r", ch );
		return;
	    }
	}
	send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "pull" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	    send_to_char( "Set the 'pull' between this room, and the destination room.\n\r", ch );
	    send_to_char( "Usage: redit pull <dir> <force (0 to 100)>\n\r", ch );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	if ( xit )
	{
	    xit->pull = URANGE(-100, atoi(argument), 100);
	    send_to_char( "Done.\n\r", ch );
	    return;
	}
	send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "push" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	    send_to_char( "Set the 'push' away from the destination room in the opposite direction.\n\r", ch );
	    send_to_char( "Usage: redit push <dir> <force (0 to 100)>\n\r", ch );
	    return;
	}
	if ( arg2[0] == '#' )
	{
	    edir = atoi( arg2+1 );
	    xit = get_exit_num( location, edir );
	}
	else
	{
	    edir = get_dir( arg2 );
	    xit = get_exit( location, edir );
	}
	if ( xit )
	{
	    xit->pull = URANGE(-100, -(atoi(argument)), 100);
	    send_to_char( "Done.\n\r", ch );
	    return;
	}
	send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "exdistance" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	   send_to_char( "Set the distance (in rooms) between this room, and the destination room.\n\r", ch );
	   send_to_char( "Usage: redit exdistance <dir> [distance]\n\r", ch );
	   return;
	}
	if ( arg2[0] == '#' )
	{
	   edir = atoi( arg2+1 );
	   xit = get_exit_num( location, edir );
	}
	else
	{
	   edir = get_dir( arg2 );
	   xit = get_exit( location, edir );
	}
	if ( xit )
	{
	   xit->distance = URANGE( 1, atoi(argument), 50 );
	   send_to_char( "Done.\n\r", ch );
	   return;
	}
	send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "exdesc" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( !arg2 || arg2[0] == '\0' )
	{
	   send_to_char( "Create or clear a description for an exit.\n\r", ch );
	   send_to_char( "Usage: redit exdesc <dir> [description]\n\r", ch );
	   return;
	}
	if ( arg2[0] == '#' )
	{
	   edir = atoi( arg2+1 );
	   xit = get_exit_num( location, edir );
	}
	else
	{
	   edir = get_dir( arg2 );
	   xit = get_exit( location, edir );
	}
	if ( xit )
	{
	   STRFREE( xit->description );
	   if ( !argument || argument[0] == '\0' )
	     xit->description = STRALLOC( "" );
	   else
	   {
	     sprintf( buf, "%s\n\r", argument );
	     xit->description = STRALLOC( buf );
	   }
	   send_to_char( "Done.\n\r", ch );
	   return;
	}
	send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\n\r", ch );
	return;
    }

    /*
     * Generate usage message.
     */
    if ( ch->substate == SUB_REPEATCMD )
    {
	ch->substate = SUB_RESTRICTED;
	interpret( ch, origarg );
	ch->substate = SUB_REPEATCMD;
	ch->last_cmd = do_redit;
    }
    else
	do_redit( ch, "" );
    return;
}

void do_ocreate( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA	*pObjIndex;
    OBJ_DATA		*obj;
    int			 vnum, cvnum;

    if ( IS_NPC(ch) )
    {
	send_to_char( "Mobiles cannot create.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    vnum = is_number( arg ) ? atoi( arg ) : -1;

    if ( vnum == -1 || !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage:  ocreate <vnum> [copy vnum] <item name>\n\r", ch );
	return;
    }

//    if ( vnum < 1 || vnum > 32767 )
    if ( vnum < 1 || vnum > 1048576000 )
    {
	send_to_char( "Vnum out of range.\n\r", ch );
	return;
    }

    one_argument( argument, arg2 );
    cvnum = atoi( arg2 );
    if ( cvnum != 0 )
	argument = one_argument( argument, arg2 );
    if ( cvnum < 1 )
	cvnum = 0;

    if ( get_obj_index( vnum ) )
    {
	send_to_char( "An object with that number already exists.\n\r", ch );
	return;
    }    

    if ( IS_NPC( ch ) )
	return;
    if ( get_trust( ch ) < LEVEL_LESSER )
    {
	AREA_DATA *pArea;

	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	  send_to_char( "You must have an assigned area to create objects.\n\r", ch );
	  return;
	}
	if ( vnum < pArea->low_o_vnum
	||   vnum > pArea->hi_o_vnum )
	{
	  send_to_char( "That number is not in your allocated range.\n\r", ch );
	  return;
	}
    }

    pObjIndex = make_object( vnum, cvnum, argument );
    if ( !pObjIndex )
    {
	send_to_char( "Error.\n\r", ch );
	log_string( "do_ocreate: make_object failed." );
	return;
    }
    obj = create_object( pObjIndex, get_trust(ch) );
    obj_to_char( obj, ch );
    act( AT_IMMORT, "$n makes arcane gestures, and opens $s hands to reveal $p!", ch, obj, NULL, TO_ROOM );
    ch_printf_color( ch, "&YYou make arcane gestures, and open your hands to reveal %s!\n\rObjVnum:  &W%d   &YKeywords:  &W%s\n\r",
	pObjIndex->short_descr,pObjIndex->vnum, pObjIndex->name );
}

void do_mcreate( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA	*pMobIndex;
    CHAR_DATA		*mob;
    int			 vnum, cvnum;

    if ( IS_NPC(ch) )
    {
	send_to_char( "Mobiles cannot create.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );
    
    vnum = is_number( arg ) ? atoi( arg ) : -1;

    if ( vnum == -1 || !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage:  mcreate <vnum> [cvnum] <mobile name>\n\r", ch );
	return;
    }

//    if ( vnum < 1 || vnum > 32767 )
    if ( vnum < 1 || vnum > 1048576000 )
    {
	send_to_char( "Vnum out of range.\n\r", ch );
	return;
    }

    one_argument( argument, arg2 );
    cvnum = atoi( arg2 );
    if ( cvnum != 0 )
	argument = one_argument( argument, arg2 );
    if ( cvnum < 1 )
	cvnum = 0;

    if ( get_mob_index( vnum ) )
    {
	send_to_char( "A mobile with that number already exists.\n\r", ch );
	return;
    }    

    if ( IS_NPC( ch ) )
	return;
    if ( get_trust( ch ) < LEVEL_LESSER )
    {
	AREA_DATA *pArea;

	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	  send_to_char( "You must have an assigned area to create mobiles.\n\r", ch );
	  return;
	}
	if ( vnum < pArea->low_m_vnum
	||   vnum > pArea->hi_m_vnum )
	{
	  send_to_char( "That number is not in your allocated range.\n\r", ch );
	  return;
	}
    }

    pMobIndex = make_mobile( vnum, cvnum, argument );
    if ( !pMobIndex )
    {
	send_to_char( "Error.\n\r", ch );
	log_string( "do_mcreate: make_mobile failed." );
	return;
    }

    mob = create_mobile( pMobIndex );
    mob->max_hit = 150;
    mob->hit = mob->max_hit;

    char_to_room( mob, ch->in_room );
    /* If you create one on the map, make sure it gets placed properly - Samson 8-21-99 */
    fix_maps( ch, mob );
    act( AT_IMMORT, "$n waves $s arms about, and $N appears at $s command!", ch, NULL, mob, TO_ROOM );
    ch_printf_color( ch, "&YYou wave your arms about, and %s appears at your command!\n\rMobVnum:  &W%d   &YKeywords:  &W%s\n\r",
	pMobIndex->short_descr, pMobIndex->vnum, pMobIndex->player_name );
}

/*
 * Simple but nice and handy line editor.			-Thoric
 */
/*
void edit_buffer( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    EDITOR_DATA *edit;
    char cmd[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    sh_int x, line, max_buf_lines;
    bool save;

    if ( (d = ch->desc) == NULL )
    {
	send_to_char( "You have no descriptor.\n\r", ch );
	return;
    }

    if ( d->connected != CON_EDITING )
    {
	send_to_char( "You can't do that!\n\r", ch );
	bug( "Edit_buffer: d->connected != CON_EDITING", 0 );
	return;
    }
    
    if ( ch->substate <= SUB_PAUSE )
    {
	send_to_char( "You can't do that!\n\r", ch );
	bug( "Edit_buffer: illegal ch->substate (%d)", ch->substate );
	d->connected = CON_PLAYING;
	return;
    }
   
    if ( !ch->editor )
    {
	send_to_char( "You can't do that!\n\r", ch );
	bug( "Edit_buffer: null editor", 0 );
	d->connected = CON_PLAYING;
	return;
    }
   
    edit = ch->editor;
    save = FALSE;
    max_buf_lines = 80;

    if ( ch->substate == SUB_MPROG_EDIT || ch->substate == SUB_HELP_EDIT )
	max_buf_lines = 80;

    if ( argument[0] == '/' || argument[0] == '\\' )
    {
	one_argument( argument, cmd );
	if ( !str_cmp( cmd+1, "?" ) )
	{
	    send_to_char( "Editing commands\n\r---------------------------------\n\r", ch );
	    send_to_char( "/l              list buffer\n\r",	    ch );
	    send_to_char( "/c              clear buffer\n\r",	    ch );
	    send_to_char( "/d [line]       delete line\n\r",	    ch );
	    send_to_char( "/f              formats the buffer\n\r", ch );
            send_to_char( "/g <line>       goto line\n\r",	    ch );
	    send_to_char( "/i <line>       insert line\n\r",	    ch );
	    send_to_char( "/r <old> <new>  global replace\n\r",	    ch );
	    send_to_char( "/a              abort editing\n\r",	    ch );
	    if ( get_trust(ch) > LEVEL_IMMORTAL )
		send_to_char( "/! <command>    execute command (do not use another editing command)\n\r",  ch );
	    send_to_char( "/s              save buffer\n\r\n\r> ",ch );
	    return;
	}
	if ( !str_cmp( cmd+1, "c" ) )
	{
	    memset( edit, '\0', sizeof(EDITOR_DATA) );
	    edit->numlines = 0;
	    edit->on_line   = 0;
	    send_to_char( "Buffer cleared.\n\r> ", ch );
	    return;
	}
	if ( !str_cmp( cmd+1, "r" ) )
	{
	    char word1[MAX_INPUT_LENGTH];
	    char word2[MAX_INPUT_LENGTH];
	    char *sptr, *wptr, *lwptr;
	    int x, count, wordln, word2ln, lineln;

	    sptr = one_argument( argument, word1 );
	    sptr = one_argument( sptr, word1 );
	    sptr = one_argument( sptr, word2 );
	    if ( word1[0] == '\0' || word2[0] == '\0' )
	    {
		send_to_char( "Need word to replace, and replacement.\n\r> ", ch );
		return;
	    }
	    if ( strcmp( word1, word2 ) == 0 )
	    {
		send_to_char( "Done.\n\r> ", ch );
		return;
	    }
	    count = 0;  wordln = strlen(word1);  word2ln = strlen(word2);
	    ch_printf( ch, "Replacing all occurrences of %s with %s...\n\r", word1, word2 );
	    for ( x = edit->on_line; x < edit->numlines; x++ )
	    {
		lwptr = edit->line[x];
		while ( (wptr = strstr( lwptr, word1 )) != NULL )
		{
		    ++count;
		    lineln = sprintf( buf, "%s%s", word2, wptr + wordln );
		    if (lineln + wptr - edit->line[x] > 79)
			buf[lineln] = '\0';
		    strcpy( wptr, buf );
		    lwptr = wptr + word2ln;
		}
	    }
	    ch_printf( ch, "Found and replaced %d occurrence(s).\n\r> ", count );
	    return;
	}

 if ( !str_cmp( cmd+1, "f" ) )
	{
	    char   temp_buf[MAX_STRING_LENGTH+max_buf_lines];
	    int    x, ep, old_p, end_mark; 
	    int    p = 0;
	    
          ch_printf( ch, "Reformating...\n\r");

	    for ( x = 0; x < edit->numlines; x++ )
	    {
	       strcpy ( temp_buf+p , edit->line[x] );
	       p += strlen( edit->line[x] );
	       temp_buf[p] = ' ';
	       p++;
	    }
	    
	    temp_buf[p] = '\0';
	    end_mark = p;
	    p = 75;
	    old_p =0;
	    edit->on_line =0;
	    edit->numlines =0;
	    
	    while ( old_p < end_mark )
	    {
	        while ( temp_buf[p] != ' ' && p > old_p )
	          p--;
	          
	        if ( p == old_p )
	          p +=75;
	        
	        if ( p > end_mark )
	          p = end_mark;
	        
	        ep =0;
	        for ( x= old_p ; x < p ; x++ )  
	        {
	           edit->line[edit->on_line][ep] = temp_buf[x];
                   ep++;
                }
                edit->line[edit->on_line][ep] = '\0';

	        edit->on_line++;
	        edit->numlines++;
	        
	        old_p = p+1 ;
	        p += 75;
	        
	    }
          ch_printf( ch, "Reformating done.\n\r> ");
	    return;
	}
	
          if ( !str_cmp( cmd+1, "i" ) )
	{
	    if ( edit->numlines >= max_buf_lines )
		send_to_char( "Buffer is full.\n\r> ", ch );
	    else
	    {
		if ( argument[2] == ' ' )
		  line = atoi( argument + 2 ) - 1;
		else
		  line = edit->on_line;
		if ( line < 0 )
		  line = edit->on_line;
		if ( line < 0 || line > edit->numlines )
		  send_to_char( "Out of range.\n\r> ", ch );
		else
		{
		  for ( x = ++edit->numlines; x > line; x-- )
			strcpy( edit->line[x], edit->line[x-1] );
		  strcpy( edit->line[line], "" );
		  send_to_char( "Line inserted.\n\r> ", ch );
		}
 	    }
	    return;
	}
	if ( !str_cmp( cmd+1, "d" ) )
	{
	    if ( edit->numlines == 0 )
		send_to_char( "Buffer is empty.\n\r> ", ch );
	    else
	    {
		if ( argument[2] == ' ' )
		    line = atoi( argument + 2 ) - 1;
		else
		    line = edit->on_line;
		if ( line < 0 )
		    line = edit->on_line;
		if ( line < 0 || line > edit->numlines )
		    send_to_char( "Out of range.\n\r> ", ch );
		else
		{
		    if ( line == 0 && edit->numlines == 1 )
		    {
			memset( edit, '\0', sizeof(EDITOR_DATA) );
			edit->numlines = 0;
			edit->on_line   = 0;
			send_to_char( "Line deleted.\n\r> ", ch );
			return;
		    }
		    for ( x = line; x < (edit->numlines - 1); x++ )
			strcpy( edit->line[x], edit->line[x+1] );
		    strcpy( edit->line[edit->numlines--], "" );
		    if ( edit->on_line > edit->numlines )
			edit->on_line = edit->numlines;
		    send_to_char( "Line deleted.\n\r> ", ch );
		}
 	    }
	    return;
	}
	if ( !str_cmp( cmd+1, "g" ) )
	{
	    if ( edit->numlines == 0 )
		send_to_char( "Buffer is empty.\n\r> ", ch );
	    else
	    {
		if ( argument[2] == ' ' )
		    line = atoi( argument + 2 ) - 1;
		else
		{
		    send_to_char( "Goto what line?\n\r> ", ch );
		    return;
		}
		if ( line < 0 )
		    line = edit->on_line;
		if ( line < 0 || line > edit->numlines )
		    send_to_char( "Out of range.\n\r> ", ch );
		else
		{
		    edit->on_line = line;
		    ch_printf( ch, "(On line %d)\n\r> ", line+1 );
		}
 	    }
	    return;
	}
	if ( !str_cmp( cmd+1, "l" ) )
	{
	    if ( edit->numlines == 0 )
		send_to_char( "Buffer is empty.\n\r> ", ch );
	    else
	    {
		send_to_char( "------------------\n\r", ch );
		for ( x = 0; x < edit->numlines; x++ )
		    ch_printf( ch, "%2d> %s\n\r", x+1, edit->line[x] );
		send_to_char( "------------------\n\r> ", ch );
	    }
	    return;
	}
	if ( !str_cmp( cmd+1, "a" ) )
	{
	    send_to_char( "\n\rAborting... ", ch );
	    stop_editing( ch );
	    return;
	}
	if ( get_trust(ch) > LEVEL_IMMORTAL && !str_cmp( cmd+1, "!" ) )
	{
	    DO_FUN *last_cmd;
	    int substate = ch->substate;

	    last_cmd = ch->last_cmd;
	    ch->substate = SUB_RESTRICTED;
	    interpret(ch, argument+3);
	    ch->substate = substate;
	    ch->last_cmd = last_cmd;
	    set_char_color( AT_GREEN, ch );
	    send_to_char( "\n\r> ", ch );
	    return;
	}
	if ( !str_cmp( cmd+1, "s" ) )
	{
	    d->connected = CON_PLAYING;
	    if ( !ch->last_cmd )
	      return;
	    (*ch->last_cmd) ( ch, "" );
	    return;
	}
    }

    if ( edit->size + strlen(argument) + 1 >= MAX_STRING_LENGTH - 1 )
	send_to_char( "You buffer is full.\n\r", ch );
    else
    {
	if ( strlen(argument) > 79 )
	{
	  strncpy( buf, argument, 79 );
	  buf[79] = 0; 
	  send_to_char( "(Long line trimmed)\n\r> ", ch );
	}
	else
	  strcpy( buf, argument );
	strcpy( edit->line[edit->on_line++], buf );
	if ( edit->on_line > edit->numlines )
	  edit->numlines++;
	if ( edit->numlines > max_buf_lines )
	{
	  edit->numlines = max_buf_lines;
	  send_to_char( "Buffer full.\n\r", ch );
	  save = TRUE;
	}
    }

    if ( save )
    {
	d->connected = CON_PLAYING;
	if ( !ch->last_cmd )
	    return;
	(*ch->last_cmd) ( ch, "" );
	return;
    }
    send_to_char( "> ", ch );
}*/


void assign_area( CHAR_DATA *ch )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char taf[1024];
	AREA_DATA *tarea, *tmp;
	bool created = FALSE;

	if ( IS_NPC( ch ) )
	  return;
	if ( get_trust( ch ) > LEVEL_IMMORTAL
	&&   ch->pcdata->r_range_lo
	&&   ch->pcdata->r_range_hi )
	{
	  tarea = ch->pcdata->area;
	  sprintf( taf, "%s.are", capitalize( ch->name ) );
	  if ( !tarea )
	  {
		for ( tmp = first_build; tmp; tmp = tmp->next )
			if ( !str_cmp( taf, tmp->filename ) )
			{  
			  tarea = tmp;
			  break;
			}
	  }
	  if ( !tarea )
	  {
	    sprintf( buf, "Creating area entry for %s", ch->name );
	    log_string_plus( buf, LOG_NORMAL, ch->level );
	    CREATE( tarea, AREA_DATA, 1 );
	    LINK( tarea, first_build, last_build, next, prev );
            tarea->first_room = tarea->last_room = NULL;
	    sprintf( buf, "{PROTO} %s's area in progress", ch->name );
	    tarea->name		= str_dup( buf );
	    tarea->filename	= str_dup( taf );
	    sprintf( buf2, "%s", ch->name );
	    tarea->author 	= STRALLOC( buf2 );
	    tarea->age		= 0;
	    tarea->nplayer	= 0;
	    
	    CREATE(tarea->weather, WEATHER_DATA, 1); /* FB */
	    tarea->weather->temp = 0;
	    tarea->weather->precip = 0;
	    tarea->weather->wind = 0;
	    tarea->weather->temp_vector = 0;
	    tarea->weather->precip_vector = 0;
	    tarea->weather->wind_vector = 0;
	    tarea->weather->climate_temp = 2;
	    tarea->weather->climate_precip = 2;
	    tarea->weather->climate_wind = 2;
	    tarea->weather->first_neighbor = NULL;
	    tarea->weather->last_neighbor = NULL;
	    tarea->weather->echo = NULL;
	    tarea->weather->echo_color = AT_GREY;
	    
	    created = TRUE;
	  }
	  else
	  {
	    sprintf( buf, "Updating area entry for %s", ch->name );
	    log_string_plus( buf, LOG_NORMAL, ch->level );
	  }
	  tarea->low_r_vnum	= ch->pcdata->r_range_lo;
	  tarea->low_o_vnum	= ch->pcdata->o_range_lo;
	  tarea->low_m_vnum	= ch->pcdata->m_range_lo;
	  tarea->hi_r_vnum	= ch->pcdata->r_range_hi;
	  tarea->hi_o_vnum	= ch->pcdata->o_range_hi;
	  tarea->hi_m_vnum	= ch->pcdata->m_range_hi;
	  ch->pcdata->area	= tarea;
	  if ( created )
	  	sort_area( tarea, TRUE );
	}
}

void do_aassign( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	AREA_DATA *tarea, *tmp;

        set_char_color( AT_IMMORT, ch );

	if ( IS_NPC( ch ) )
	  return;

	if ( argument[0] == '\0' )
	{
	    send_to_char( "Syntax: aassign <filename.are>\n\r", ch );
	    return;
	}

	if ( !str_cmp( "none", argument )
	||   !str_cmp( "null", argument )
	||   !str_cmp( "clear", argument ) )
	{
	    ch->pcdata->area = NULL;
	    assign_area( ch );
	    if ( !ch->pcdata->area )
	      send_to_char( "Area pointer cleared.\n\r", ch );
	    else
	      send_to_char( "Originally assigned area restored.\n\r", ch );
	    return;
	} 

	sprintf( buf, "%s", argument );
        tarea = NULL;

/*	if ( get_trust(ch) >= sysdata.level_modify_proto )   */

	if ( get_trust(ch) >= LEVEL_SUB_IMPLEM
        ||  (is_name( buf, ch->pcdata->bestowments )
        &&   get_trust(ch) >= sysdata.level_modify_proto) )
	  for ( tmp = first_area; tmp; tmp = tmp->next )
	    if ( !str_cmp( buf, tmp->filename ) )
	    {  
		tarea = tmp;
		break;
	    }

	if ( !tarea )
	  for ( tmp = first_build; tmp; tmp = tmp->next )
	    if ( !str_cmp( buf, tmp->filename ) )
	    {
/*		if ( get_trust(ch) >= sysdata.level_modify_proto  */
		if ( get_trust(ch) >= LEVEL_GREATER
	        ||   is_name( tmp->filename, ch->pcdata->bestowments ))
		{  
		    tarea = tmp;
		    break;
		}
		else
		{
		    send_to_char( "You do not have permission to use that area.\n\r", ch );
		    return;
		}
	    }
	 
	if ( !tarea )
	{
	    if ( get_trust(ch) >= sysdata.level_modify_proto )
		send_to_char( "No such area.  Use 'zones'.\n\r", ch );
	    else
		send_to_char( "No such area.  Use 'newzones'.\n\r", ch );
	    return;
	}
	ch->pcdata->area = tarea;
	ch_printf( ch, "Assigning you: %s\n\r", tarea->name );
	return;
}


EXTRA_DESCR_DATA *SetRExtra( ROOM_INDEX_DATA *room, char *keywords )
{
    EXTRA_DESCR_DATA *ed;
    
    for ( ed = room->first_extradesc; ed; ed = ed->next )
    {
	  if ( is_name( keywords, ed->keyword ) )
	    break;
    }
    if ( !ed )
    {
	CREATE( ed, EXTRA_DESCR_DATA, 1 );
	LINK( ed, room->first_extradesc, room->last_extradesc, next, prev );
	ed->keyword	= STRALLOC( keywords );
	ed->description	= STRALLOC( "" );
	top_ed++;
    }
    return ed;
}

bool DelRExtra( ROOM_INDEX_DATA *room, char *keywords )
{
    EXTRA_DESCR_DATA *rmed;
    
    for ( rmed = room->first_extradesc; rmed; rmed = rmed->next )
    {
	  if ( is_name( keywords, rmed->keyword ) )
	    break;
    }
    if ( !rmed )
      return FALSE;
    UNLINK( rmed, room->first_extradesc, room->last_extradesc, next, prev );
    STRFREE( rmed->keyword );
    STRFREE( rmed->description );
    DISPOSE( rmed );
    top_ed--;
    return TRUE;
}

EXTRA_DESCR_DATA *SetOExtra( OBJ_DATA *obj, char *keywords )
{
    EXTRA_DESCR_DATA *ed;
    
    for ( ed = obj->first_extradesc; ed; ed = ed->next )
    {
	  if ( is_name( keywords, ed->keyword ) )
	    break;
    }
    if ( !ed )
    {
	CREATE( ed, EXTRA_DESCR_DATA, 1 );
	LINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
	ed->keyword	= STRALLOC( keywords );
	ed->description	= STRALLOC( "" );
	top_ed++;
    }
    return ed;
}

bool DelOExtra( OBJ_DATA *obj, char *keywords )
{
    EXTRA_DESCR_DATA *rmed;
    
    for ( rmed = obj->first_extradesc; rmed; rmed = rmed->next )
    {
	  if ( is_name( keywords, rmed->keyword ) )
	    break;
    }
    if ( !rmed )
      return FALSE;
    UNLINK( rmed, obj->first_extradesc, obj->last_extradesc, next, prev );
    STRFREE( rmed->keyword );
    STRFREE( rmed->description );
    DISPOSE( rmed );
    top_ed--;
    return TRUE;
}

EXTRA_DESCR_DATA *SetOExtraProto( OBJ_INDEX_DATA *obj, char *keywords )
{
    EXTRA_DESCR_DATA *ed;
    
    for ( ed = obj->first_extradesc; ed; ed = ed->next )
    {
	  if ( is_name( keywords, ed->keyword ) )
	    break;
    }
    if ( !ed )
    {
	CREATE( ed, EXTRA_DESCR_DATA, 1 );
	LINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
	ed->keyword	= STRALLOC( keywords );
	ed->description	= STRALLOC( "" );
	top_ed++;
    }
    return ed;
}

bool DelOExtraProto( OBJ_INDEX_DATA *obj, char *keywords )
{
    EXTRA_DESCR_DATA *rmed;
    
    for ( rmed = obj->first_extradesc; rmed; rmed = rmed->next )
    {
	  if ( is_name( keywords, rmed->keyword ) )
	    break;
    }
    if ( !rmed )
      return FALSE;
    UNLINK( rmed, obj->first_extradesc, obj->last_extradesc, next, prev );
    STRFREE( rmed->keyword );
    STRFREE( rmed->description );
    DISPOSE( rmed );
    top_ed--;
    return TRUE;
}

void fold_area( AREA_DATA *tarea, char *filename, bool install, bool msg )
{
    RESET_DATA *pReset, *tReset, *gReset;
    ROOM_INDEX_DATA	*room;
    MOB_INDEX_DATA	*pMobIndex;
    OBJ_INDEX_DATA	*pObjIndex;
    MPROG_DATA		*mprog;
    EXIT_DATA		*xit;
    EXTRA_DESCR_DATA	*ed;
    SHOP_DATA		*pShop;
    SHIP_DATA		*pShip;
    REPAIR_DATA		*pRepair;
    NEIGHBOR_DATA	*neigh;
    char		 buf[MAX_STRING_LENGTH];
    FILE		*fpout;
    int			 vnum;
    int			 val0, val1, val2, val3, val4, val5;
    bool		 complexmob;

    sprintf( buf, "None");
    sprintf( buf, "Saving %s...", tarea->filename );
    if ( msg )
    log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

    sprintf( buf, "%s.bak", filename );
    rename( filename, buf );
    fclose( fpReserve );
    if ( ( fpout = fopen( filename, "w" ) ) == NULL )
    {
	bug( "fold_area: fopen", 0 );
	perror( filename );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }
   if( install )
      REMOVE_BIT( tarea->flags, AFLAG_PROTOTYPE );
    
    fprintf( fpout, "#AREA   %s~\n\n\n\n", tarea->name );
    fprintf( fpout, "#VERSION %d\n", AREA_VERSION_WRITE );
    fprintf( fpout, "#AUTHOR %s~\n\n", tarea->author );
    fprintf( fpout, "#RANGES\n");
    fprintf( fpout, "%d %d %d %d\n", tarea->low_soft_range,
				     tarea->hi_soft_range,
		 		     tarea->low_hard_range,
			   	     tarea->hi_hard_range );
    fprintf( fpout, "$\n\n");
    if ( AREA_VERSION_WRITE > 1 )
    	fprintf( fpout, "#SPELLLIMIT %d\n", tarea->spelllimit );
    if ( tarea->resetmsg )	/* Rennard */
	fprintf( fpout, "#RESETMSG %s~\n\n", tarea->resetmsg );
    if ( tarea->reset_frequency )
	fprintf( fpout, "#FLAGS\n%d %d\n\n",
		tarea->flags, tarea->reset_frequency );
    else
	fprintf( fpout, "#FLAGS\n%d\n\n", tarea->flags );
    fprintf( fpout, "#CONTINENT %s~\n\n", continents[tarea->continent] );
    if ( IS_SET(tarea->flags, AFLAG_PLANET))
    {
      fprintf( fpout, "#INCOME %d\n", tarea->income);
      fprintf( fpout, "#POPSUPPORT %d\n", tarea->popsupport );
      fprintf( fpout, "#GOVERNMENT %s~\n",  tarea->governed_by ? tarea->governed_by : "None" );
      fprintf( fpout, "#GUARDS %d %d %d %d\n\n", tarea->guard1, tarea->guard2, tarea->guard3, tarea->guard4 );
    }  
    fprintf( fpout, "#ECONOMY %d %d\n\n", tarea->high_economy, tarea->low_economy );

    /* Climate info - FB */
    fprintf( fpout, "#CLIMATE %d %d %d\n\n", tarea->weather->climate_temp,
    					     tarea->weather->climate_precip,
    					     tarea->weather->climate_wind);
    
    /* neighboring weather systems - FB */
    for(neigh = tarea->weather->first_neighbor; neigh; neigh = neigh->next)
    	fprintf( fpout, "#NEIGHBOR %s~\n\n", neigh->name);

    fprintf( fpout, "#MOBILES\n" );
    for ( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
    {
	if ( (pMobIndex = get_mob_index( vnum )) == NULL )
	  continue;
	if ( install )
	  xREMOVE_BIT(pMobIndex->act, ACT_PROTOTYPE);
	if ( pMobIndex->perm_str != 13	||   pMobIndex->perm_int   != 13
	||   pMobIndex->perm_wis != 13	||   pMobIndex->perm_dex   != 13
	||   pMobIndex->perm_con != 13	||   pMobIndex->perm_cha   != 13
	||   pMobIndex->perm_lck != 13
	||   pMobIndex->hitroll  != 0	||   pMobIndex->damroll    != 0
	||   pMobIndex->race	 != 0	||   pMobIndex->class	   != 3
	||  !xIS_EMPTY(pMobIndex->attacks)
	||  !xIS_EMPTY(pMobIndex->defenses)
	||   pMobIndex->height	 != 0	||   pMobIndex->weight	   != 0
	||   pMobIndex->xflags	 != 0   ||   pMobIndex->numattacks != 0 )
	  complexmob = TRUE;
	else
	  complexmob = FALSE;
	fprintf( fpout, "#%d\n",	vnum				);
	fprintf( fpout, "%s~\n",	pMobIndex->player_name		);
	fprintf( fpout,	"%s~\n",	pMobIndex->short_descr		);
	fprintf( fpout,	"%s~\n",	strip_cr(pMobIndex->long_descr)	);
	fprintf( fpout, "%s~\n",	strip_cr(pMobIndex->description));
	fprintf( fpout, "%s ",		print_bitvector(&pMobIndex->act));
	fprintf( fpout, "%d %c\n",	pMobIndex->alignment,
					complexmob ? 'C' : 'S'		);
					
	fprintf( fpout, "%d %d %d ",	pMobIndex->level,
					pMobIndex->mobthac0,
					pMobIndex->ac			);
	fprintf( fpout, "%dd%d+%d ",	pMobIndex->hitnodice,
					pMobIndex->hitsizedice,
					pMobIndex->hitplus		);
	fprintf( fpout, "%dd%d+%d\n",	pMobIndex->damnodice,
					pMobIndex->damsizedice,
					pMobIndex->damplus		);
	fprintf( fpout, "%lld %ld\n",	pMobIndex->gold,
					pMobIndex->exp			);
/* Need to convert to new positions correctly on loadup sigh -Shaddai */
	fprintf( fpout, "%d %d %d\n",	pMobIndex->position+100,
					pMobIndex->defposition+100,
					pMobIndex->sex			);
	if ( complexmob )
	{
	  fprintf( fpout, "%d %d %d %d %d %d %d\n",
					pMobIndex->perm_str,
					pMobIndex->perm_int,
					pMobIndex->perm_wis,
					pMobIndex->perm_dex,
					pMobIndex->perm_con,
					pMobIndex->perm_cha,
					pMobIndex->perm_lck );
	  fprintf( fpout, "%d %d %d %d %d\n",
					pMobIndex->saving_poison_death,
					pMobIndex->saving_wand,
					pMobIndex->saving_para_petri,
					pMobIndex->saving_breath,
					pMobIndex->saving_spell_staff );
	  fprintf( fpout, "%d %d %d %d %d %d %d\n",
					pMobIndex->race,
					pMobIndex->class,
					pMobIndex->height,
					pMobIndex->weight,
					pMobIndex->race,
					pMobIndex->race,
					pMobIndex->numattacks );
	  fprintf( fpout, "%d %d %d %d %d %d %s ",
					pMobIndex->hitroll,
					pMobIndex->damroll,
					pMobIndex->xflags,
					pMobIndex->resistant,
					pMobIndex->immune,
					pMobIndex->susceptible,
			print_bitvector(&pMobIndex->attacks) );
	  fprintf( fpout, "%s\n",
	  		print_bitvector(&pMobIndex->defenses) );
	}
        fprintf( fpout,"Z\n%s~\n", pMobIndex->clan ? pMobIndex->clan : "none" );
	if ( pMobIndex->mudprogs )
	{
	  for ( mprog = pMobIndex->mudprogs; mprog; mprog = mprog->next )
		fprintf( fpout, "> %s %s~\n%s~\n",
				mprog_type_to_name( mprog->type ),
				mprog->arglist, strip_cr(mprog->comlist) );
	  fprintf( fpout, "|\n" );	  
	}
    }
    fprintf( fpout, "#0\n\n\n" );
    if ( install && vnum < tarea->hi_m_vnum )
      tarea->hi_m_vnum = vnum - 1;

    /* save objects */
    fprintf( fpout, "#OBJECTS\n" );
    for ( vnum = tarea->low_o_vnum; vnum <= tarea->hi_o_vnum; vnum++ )
    {
	if ( (pObjIndex = get_obj_index( vnum )) == NULL )
	  continue;
	if ( install )
	  xREMOVE_BIT( pObjIndex->extra_flags, ITEM_PROTOTYPE );
	fprintf( fpout, "#%d\n",	vnum				);
	fprintf( fpout, "%s~\n",	pObjIndex->name			);
	fprintf( fpout, "%s~\n",	pObjIndex->short_descr		);
	fprintf( fpout, "%s~\n",	pObjIndex->description		);
	fprintf( fpout, "%s~\n",	pObjIndex->action_desc		);
	if ( pObjIndex->layers )
	  fprintf( fpout, "%d %s %d %d\n",	pObjIndex->item_type,
				print_bitvector(&pObjIndex->extra_flags),
						pObjIndex->wear_flags,
						pObjIndex->layers	);
	else
	  fprintf( fpout, "%d %s %d\n",	pObjIndex->item_type,
			print_bitvector(&pObjIndex->extra_flags),
					pObjIndex->wear_flags		);

	val0 = pObjIndex->value[0];
	val1 = pObjIndex->value[1];
	val2 = pObjIndex->value[2];
	val3 = pObjIndex->value[3];
	val4 = pObjIndex->value[4];
	val5 = pObjIndex->value[5];
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_STAFF:
	case ITEM_SALVE:
	    break;
	}
	if ( val4 || val5 )
	  fprintf( fpout, "%d %d %d %d %d %d\n",val0, 
						val1,
						val2,
						val3,
						val4,
						val5 );
	else
	  fprintf( fpout, "%d %d %d %d\n",	val0, 
						val1,
						val2,
						val3 );

	fprintf( fpout, "%d %d %d\n",	pObjIndex->weight,
					pObjIndex->cost,
					pObjIndex->rent ? pObjIndex->rent :
				 (int) (pObjIndex->cost / 10)		);

        if ( AREA_VERSION_WRITE > 0 )
    	switch ( pObjIndex->item_type )
    	{
    	case ITEM_PILL: 
    	case ITEM_POTION:
        break;
    	}

	for ( ed = pObjIndex->first_extradesc; ed; ed = ed->next )
	   fprintf( fpout, "E\n%s~\n%s~\n",
			ed->keyword, strip_cr( ed->description )	);

	if ( pObjIndex->mudprogs )
	{
	  for ( mprog = pObjIndex->mudprogs; mprog; mprog = mprog->next )
		fprintf( fpout, "> %s %s~\n%s~\n",
				mprog_type_to_name( mprog->type ),
				mprog->arglist, strip_cr(mprog->comlist) );
	  fprintf( fpout, "|\n" );	  
	}
    }
    fprintf( fpout, "#0\n\n\n" );
    if ( install && vnum < tarea->hi_o_vnum )
      tarea->hi_o_vnum = vnum - 1;

    /* save rooms   */
    fprintf( fpout, "#ROOMS\n" );
    for ( vnum = tarea->low_r_vnum; vnum <= tarea->hi_r_vnum; vnum++ )
    {
	if ( (room = get_room_index( vnum )) == NULL )
	  continue;
	if ( install )
	{
	    CHAR_DATA *victim, *vnext;
	    OBJ_DATA  *obj, *obj_next;

	    /* remove prototype flag from room */
	    REMOVE_BIT( room->room_flags, ROOM_PROTOTYPE );
	    /* purge room of (prototyped) mobiles */
	    for ( victim = room->first_person; victim; victim = vnext )
	    {
		vnext = victim->next_in_room;
		if ( IS_NPC(victim) )
		  extract_char( victim, TRUE );
	    }
	    /* purge room of (prototyped) objects */
	    for ( obj = room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		extract_obj( obj );
	    }
	}
	fprintf( fpout, "#%d\n",	vnum				);
	fprintf( fpout, "%s~\n",	room->name			);
	fprintf( fpout, "%s~\n",	strip_cr( room->description )	);
	if ( (room->tele_delay > 0 && room->tele_vnum > 0) || room->tunnel > 0 )
	  fprintf( fpout, "0 %d %d %d %d %d %d\n",	room->room_flags,
						room->sector_type,
                                                room->room_flags2,
						room->tele_delay,
						room->tele_vnum,
						room->tunnel		);
	else
	  fprintf( fpout, "0 %d %d %d\n",	room->room_flags,
					room->sector_type,
                                                room->room_flags2);
        if ( room->clan )
           fprintf( fpout, "C\n%s~\n", 		room->clan );
        for ( xit = room->first_exit; xit; xit = xit->next )
	{
	   if ( IS_SET(xit->exit_info, EX_PORTAL) ) /* don't fold portals */
		continue;
	   fprintf( fpout, "D%d\n",		xit->vdir );
	   fprintf( fpout, "%s~\n",		strip_cr( xit->description ) );
	   fprintf( fpout, "%s~\n",		strip_cr( xit->keyword ) );



           if ( xit->distance > 1 || xit->pull )
             fprintf( fpout, "%d %d %d %d %d %d %d %d %d\n",
                                                xit->exit_info & ~EX_BASHED,
                                                xit->key,
                                                xit->vnum,
                                                xit->x, xit->y, xit->map,
                                                xit->distance,
                                                xit->pulltype,
                                                xit->pull );

           else
             fprintf( fpout, "%d %d %d %d %d %d\n",
                                                xit->exit_info & ~EX_BASHED,
                                                xit->key,
                                                xit->vnum,
                                                xit->x, xit->y, xit->map );
	}	
      for( pReset = room->first_reset; pReset; pReset = pReset->next )
      {
	   switch( pReset->command ) /* extra arg1 arg2 arg3 */
	   {
	      default:  case '*': break;
	      case 'm': case 'M':
	      case 'o': case 'O':
		   fprintf( fpout, "R %c %d %d %d %d\n", UPPER( pReset->command ),
		      pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3 );
            
               for( tReset = pReset->first_reset; tReset; tReset = tReset->next_reset )
               {
                     switch( tReset->command )
                     {
                        case 'p':
                        case 'P':
                        case 'e':
                        case 'E':
                           fprintf( fpout, "  R %c %d %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2, tReset->arg3 );
                           if( tReset->first_reset )
                           {
                              for( gReset = tReset->first_reset; gReset; gReset = gReset->next_reset )
                              {
                                 if( gReset->command != 'p' && gReset->command != 'P' )
                                    continue;
                                 fprintf( fpout, "    R %c %d %d %d %d\n", UPPER( gReset->command ),
                                    gReset->extra, gReset->arg1, gReset->arg2, gReset->arg3 );
                              }
                           }
                           break;

                        case 'g':
                        case 'G':
                           fprintf( fpout, "  R %c %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2 );
                           if( tReset->first_reset )
                           {
                              for( gReset = tReset->first_reset; gReset; gReset = gReset->next_reset )
                              {
                                 if( gReset->command != 'p' && gReset->command != 'P' )
                                    continue;
                                 fprintf( fpout, "    R %c %d %d %d %d\n", UPPER( gReset->command ),
                                    gReset->extra, gReset->arg1, gReset->arg2, gReset->arg3 );
                              }
                           }
                        break;

                        case 't':
                        case 'T':
                        case 'h':
                        case 'H':
                           fprintf( fpout, "  R %c %d %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2, tReset->arg3 );
                           break;
                     }
               }
		   break;

	      case 'd': case 'D':
	      case 't': case 'T':
	  	case 'h': case 'H':
		   fprintf( fpout, "R %c %d %d %d %d\n", UPPER( pReset->command ),
		      pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3 );
		   break;

	      case 'r': case 'R':
		   fprintf( fpout, "R %c %d %d %d\n", UPPER( pReset->command ), pReset->extra, pReset->arg1, pReset->arg2 );
		   break;
	   }
      }

	for ( ed = room->first_extradesc; ed; ed = ed->next )
	   fprintf( fpout, "E\n%s~\n%s~\n",
			ed->keyword, strip_cr( ed->description ));

	if ( room->mudprogs )
	{
	  for ( mprog = room->mudprogs; mprog; mprog = mprog->next )
		fprintf( fpout, "> %s %s~\n%s~\n",
				mprog_type_to_name( mprog->type ),
				mprog->arglist, strip_cr(mprog->comlist) );
	  fprintf( fpout, "|\n" );	  
	}
	fprintf( fpout, "S\n" );
    }
    fprintf( fpout, "#0\n\n\n" );
    if ( install && vnum < tarea->hi_r_vnum )
	tarea->hi_r_vnum = vnum - 1;

    /* save shops */
    fprintf( fpout, "#SHOPS\n" );
    for ( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
    {
	if ( (pMobIndex = get_mob_index( vnum )) == NULL )
	  continue;
	if ( (pShop = pMobIndex->pShop) == NULL )
	  continue;
	fprintf( fpout, " %d   %2d %2d %2d %2d %2d   %3d %3d",
		 pShop->keeper,
		 pShop->buy_type[0],
		 pShop->buy_type[1],
		 pShop->buy_type[2],
		 pShop->buy_type[3],
		 pShop->buy_type[4],
		 pShop->profit_buy,
		 pShop->profit_sell );
	fprintf( fpout, "        %2d %2d    ; %s\n",
		 pShop->open_hour,
		 pShop->close_hour,
		 pMobIndex->short_descr );
    }
    fprintf( fpout, "0\n\n\n" );

    fprintf( fpout, "#SHIPS\n" );
    for ( vnum = tarea->low_o_vnum; vnum <= tarea->hi_o_vnum; vnum++ )
    {
        if ( (pObjIndex = get_obj_index( vnum )) == NULL )
          continue;
        if ( (pShip = pObjIndex->pShip) == NULL )
          continue;
      

        fprintf( fpout, " %d %d %d %d %d %d %d %d %d\n",
                 pShip->obj_vnum,
		 pShip->cockpit,
                 pShip->home,
                 pShip->speed,
                 pShip->current_loc,
                 pShip->keyvnum,
                 pShip->entrance,
                 pShip->destination,
                 pShip->lock);
        fprintf( fpout, " %d %d %d %d %d %d %d %d %d\n",
                 pShip->x1,
                 pShip->y1,
                 pShip->z1,
                 pShip->d1,
                 pShip->x2,
                 pShip->y2,
                 pShip->z2,
                 pShip->d2,
 		 pShip->ship_state);
        fprintf( fpout, "%s~\n",
                 pShip->name);
        if ( pShip->lock == SHIP_SHUTTLE )
        {
           fprintf( fpout, "%s~\n",
                    pShip->first_stop ? pShip->first_stop->coordname : "none");
           fprintf( fpout, "%s~\n",
                    pShip->second_stop ? pShip->second_stop->coordname : "none");
           fprintf( fpout, "  %d %d\n",
		 pShip->launch1,
		 pShip->launch2 );
        }
     }
    fprintf( fpout, "0\n\n\n" );


    /* save repair shops */
    fprintf( fpout, "#REPAIRS\n" );
    for ( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
    {
	if ( (pMobIndex = get_mob_index( vnum )) == NULL )
	  continue;
	if ( (pRepair = pMobIndex->rShop) == NULL )
	  continue;
	fprintf( fpout, " %d   %2d %2d %2d         %3d %3d",
		 pRepair->keeper,
		 pRepair->fix_type[0],
		 pRepair->fix_type[1],
		 pRepair->fix_type[2],
		 pRepair->profit_fix,
		 pRepair->shop_type );
	fprintf( fpout, "        %2d %2d    ; %s\n",
		 pRepair->open_hour,
		 pRepair->close_hour,
		 pMobIndex->short_descr );
    }
    fprintf( fpout, "0\n\n\n" );
   /* save specials */
   fprintf( fpout, "%s", "#SPECIALS\n" );
   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
   {
      if( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
         if( pMobIndex->spec_fun )
	      fprintf( fpout, "M  %d %s\n", pMobIndex->vnum, pMobIndex->spec_funname );
   }
    fprintf( fpout, "S\n\n\n" );

    /* END */
    fprintf( fpout, "#$\n" );
    fclose( fpout );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_savearea( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*tarea;
    char	 filename[256];

    set_char_color( AT_IMMORT, ch );

    if ( IS_NPC(ch) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
    ||  ( argument[0] == '\0' && !ch->pcdata->area) )
    {
	send_to_char( "You don't have an assigned area to save.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
	tarea = ch->pcdata->area;
    else
    {
	bool found;

	if ( get_trust( ch ) < LEVEL_GOD )
	{
	    send_to_char( "You can only save your own area.\n\r", ch );
	    return;
	}
	for ( found = FALSE, tarea = first_build; tarea; tarea = tarea->next )
	    if ( !str_cmp( tarea->filename, argument ) )
	    {
		found = TRUE;
		break;
	    }
	if ( !found )
	{
	    send_to_char( "Area not found.\n\r", ch );
	    return;
	}
    }

    if ( !tarea )
    {
	send_to_char( "No area to save.\n\r", ch );
	return;
    }

/* Ensure not wiping out their area with save before load - Scryn 8/11 */
    if ( !IS_SET(tarea->status, AREA_LOADED ) )
    {
	send_to_char( "Your area is not loaded!\n\r", ch );
	return;
    }

    sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
    send_to_char( "Saving area...\n\r", ch );
    fold_area( tarea, filename, FALSE, TRUE );
    set_char_color( AT_IMMORT, ch );
    send_to_char( "Done.\n\r", ch );
}

void do_loadarea( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*tarea;
    char	 filename[256];
    int		tmp;

    set_char_color( AT_IMMORT, ch );

    if ( IS_NPC(ch) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
    ||  ( argument[0] == '\0' && !ch->pcdata->area) )
    {
	send_to_char( "You don't have an assigned area to load.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
	tarea = ch->pcdata->area;
    else
    {
	bool found;

	if ( get_trust( ch ) < LEVEL_GOD )
	{
	    send_to_char( "You can only load your own area.\n\r", ch );
	    return;
	}
	for ( found = FALSE, tarea = first_build; tarea; tarea = tarea->next )
	    if ( !str_cmp( tarea->filename, argument ) )
	    {
		found = TRUE;
		break;
	    }
	if ( !found )
	{
	    send_to_char( "Area not found.\n\r", ch );
	    return;
	}
    }

    if ( !tarea )
    {
	send_to_char( "No area to load.\n\r", ch );
	return;
    }

/* Stops char from loading when already loaded - Scryn 8/11 */
    if ( IS_SET ( tarea->status, AREA_LOADED) )
    { 
	send_to_char( "Your area is already loaded.\n\r", ch );
	return;
    }
    sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
    send_to_char( "Loading...\n\r", ch );
    load_area_file( tarea, filename );
    send_to_char("Planing...\n\r", ch);
    send_to_char( "Linking exits...\n\r", ch );
    fix_area_exits( tarea );

    if( tarea->first_room )
    {
	tmp = tarea->nplayer;
	tarea->nplayer = 0;
	send_to_char( "Resetting area...\n\r", ch );
	reset_area( tarea );
	tarea->nplayer = tmp;
    }
    send_to_char( "Done.\n\r", ch );
}

/*
 * Dangerous command.  Can be used to install an area that was either:
 *   (a) already installed but removed from area.lst
 *   (b) designed offline
 * The mud will likely crash if:
 *   (a) this area is already loaded
 *   (b) it contains vnums that exist
 *   (c) the area has errors
 *
 * NOTE: Use of this command is not recommended.		-Thoric
 */
void do_unfoldarea( CHAR_DATA *ch, char *argument )
{

    set_char_color( AT_IMMORT, ch );

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Unfold what?\n\r", ch );
	return;
    }

    fBootDb = TRUE;
    load_area_file( last_area, argument );
    fBootDb = FALSE;
    return;
}


void do_foldarea( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*tarea;
    char  arg1[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
 
   set_char_color( AT_IMMORT, ch );

    if ( arg1[0] == '\0' )
    {
      if (!ch->pcdata->area)
      {
	send_to_char( "Fold what?\n\r", ch );
	return;
      }
      else
      {
       fold_area( ch->pcdata->area, ch->pcdata->area->filename, FALSE, TRUE );
       send_to_char( "Done.\n\r", ch);
       return;
      } 
    }
    if (!str_cmp(arg1,"all")){
      for ( tarea = first_area; tarea; tarea = tarea->next )
    	fold_area( tarea, tarea->filename, FALSE, TRUE );
      send_to_char("Done.\n\r", ch);
      return;
    } 
    if (!str_cmp(arg1,"allnomsg")){
      for ( tarea = first_area; tarea; tarea = tarea->next )
        fold_area( tarea, tarea->filename, FALSE, FALSE );
      send_to_char("Done.\n\r", ch);
      return;
    }

    for ( tarea = first_area; tarea; tarea = tarea->next )
    {
	if ( !str_cmp( tarea->filename, arg1 ) )
	{
	  send_to_char( "Folding area...\n\r", ch );
	  fold_area( tarea, tarea->filename, FALSE, TRUE );
	  set_char_color( AT_IMMORT, ch );
	  send_to_char( "Done.\n\r", ch );
	  return;
	}
    }
    send_to_char( "No such area exists.\n\r", ch );
    return;
}

extern int top_area;

void write_area_list( )
{
    AREA_DATA *tarea;
    FILE *fpout;

    fpout = fopen( AREA_LIST, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open area.lst for writing!\n\r", 0 );
 	return;
    }	  
    fprintf( fpout, "help.are\n" );
    for ( tarea = first_area; tarea; tarea = tarea->next )
	fprintf( fpout, "%s\n", tarea->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}

/*
 * A complicated to use command as it currently exists.		-Thoric
 * Once area->author and area->name are cleaned up... it will be easier
 */
void do_installarea( CHAR_DATA *ch, char *argument )
{
    AREA_DATA	*tarea;
    char	arg[MAX_INPUT_LENGTH];
    char	buf[MAX_STRING_LENGTH];
    int		num;
    DESCRIPTOR_DATA *d;

    set_char_color( AT_IMMORT, ch );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: installarea <filename> [Area title]\n\r", ch );
	return;
    }

    for ( tarea = first_build; tarea; tarea = tarea->next )
    {
	if ( !str_cmp( tarea->filename, arg ) )
	{
	  if ( argument && argument[0] != '\0' )
	  {
		DISPOSE( tarea->name );
		tarea->name = str_dup( argument );
	  }

	  /* Fold area with install flag -- auto-removes prototype flags */
	  send_to_char( "Saving and installing file...\n\r", ch );
	  fold_area( tarea, tarea->filename, TRUE, TRUE );

	  /* Remove from prototype area list */
	  UNLINK( tarea, first_build, last_build, next, prev );

	  /* Add to real area list */
	  LINK( tarea, first_area, last_area, next, prev );

	  /* Remove it from the prototype sort list. BUGFIX: Samson 4-15-03 */
	  UNLINK( tarea, first_bsort, last_bsort, next_sort, prev_sort );

	  /* Sort the area into it's proper sort list. BUGFIX: Samson 4-15-03 */
	  sort_area( tarea, FALSE );

	  /* Fix up author if online */
	  for ( d = first_descriptor; d; d = d->next )
	    if ( d->character
	    &&   d->character->pcdata
	    &&   d->character->pcdata->area == tarea )
	    {
		/* remove area from author */
		d->character->pcdata->area = NULL;
		/* clear out author vnums  */
		d->character->pcdata->r_range_lo = 0;
		d->character->pcdata->r_range_hi = 0;
		d->character->pcdata->o_range_lo = 0;
		d->character->pcdata->o_range_hi = 0;
		d->character->pcdata->m_range_lo = 0;
		d->character->pcdata->m_range_hi = 0;
	    }

	  top_area++;
	  send_to_char( "Writing area.lst...\n\r", ch );
	  write_area_list( );
	  send_to_char( "Resetting new area.\n\r", ch );
	  num = tarea->nplayer;
	  tarea->nplayer = 0;
	  reset_area( tarea );
	  tarea->nplayer = num;
	  send_to_char( "Renaming author's building file.\n\r", ch );
	  sprintf( buf, "%s%s.installed", BUILD_DIR, tarea->filename );
	  sprintf( arg, "%s%s", BUILD_DIR, tarea->filename );
	  rename( arg, buf );
	  send_to_char( "Done.\n\r", ch );
	  return;
	}
    }
    send_to_char( "No such area exists.\n\r", ch );
    return;
}


void do_astat( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *tarea;
    bool proto, found;
    int pdeaths = 0, pkills = 0, mdeaths = 0, mkills = 0; 
    found = FALSE; proto = FALSE;

    set_char_color( AT_PLAIN, ch );

    if ( !str_cmp( "summary", argument ) )
    {
	for ( tarea = first_area; tarea; tarea = tarea->next )
	{
	    pdeaths += tarea->pdeaths;
	    mdeaths += tarea->mdeaths;
	    pkills  += tarea->pkills;
	    mkills  += tarea->mkills;
	}
        ch_printf_color( ch, "&WTotal pdeaths:      &w%d\n\r", pdeaths );
        ch_printf_color( ch, "&WTotal pkills:       &w%d\n\r", pkills );
        ch_printf_color( ch, "&WTotal mdeaths:      &w%d\n\r", mdeaths );
        ch_printf_color( ch, "&WTotal mkills:       &w%d\n\r", mkills );
   	return;
    }

    for ( tarea = first_area; tarea; tarea = tarea->next )
	if ( !str_cmp( tarea->filename, argument ) )
	{
	  found = TRUE;
	  break;
	}

    if ( !found )
      for ( tarea = first_build; tarea; tarea = tarea->next )
	if ( !str_cmp( tarea->filename, argument ) )
	{
	  found = TRUE;
	  proto = TRUE;
	  break;
	}

    if ( !found )
    {
      if ( argument && argument[0] != '\0' ) 
      {
 	send_to_char( "Area not found.  Check 'zones'.\n\r", ch );
	return;
      }
      else
      {
        tarea = ch->in_room->area;
      }
    }

    ch_printf_color( ch, "\n\r&wName:     &W%s\n\r&wFilename: &W%-20s  &wPrototype: &W%s\n\r&wAuthor:   &W%s\n\r",
			tarea->name,
			tarea->filename,
			proto ? "yes" : "no",
			tarea->author );
    ch_printf_color( ch, "&wAge: &W%-3d  &wCurrent number of players: &W%-3d  &wMax players: &W%d\n\r",
			tarea->age,
			tarea->nplayer,
			tarea->max_players );
    if ( !proto )
    {
	if ( tarea->high_economy )
	   ch_printf_color( ch,  "&wArea economy: &W%d &wbillion and &W%d gold coins.\n\r",
			tarea->high_economy,
			tarea->low_economy );
	else
	   ch_printf_color( ch, "&wArea economy: &W%d &wgold coins.\n\r",
			tarea->low_economy );
	ch_printf_color( ch, "&wGold Looted:  &W%d\n\r",
			tarea->gold_looted );
	ch_printf_color( ch, "&wMdeaths: &W%d   &wMkills: &W%d   &wPdeaths: &W%d   &wPkills: &W%d   &wIllegalPK: &W%d\n\r",
			tarea->mdeaths,
			tarea->mkills,
			tarea->pdeaths,
			tarea->pkills,
			tarea->illegal_pk );
    }
    ch_printf_color( ch, "&wlow_room: &W%5d    &whi_room: &W%5d\n\r",
			tarea->low_r_vnum,
			tarea->hi_r_vnum );
    ch_printf_color( ch, "&wlow_obj : &W%5d    &whi_obj : &W%5d\n\r",
			tarea->low_o_vnum,
			tarea->hi_o_vnum );
    ch_printf_color( ch, "&wlow_mob : &W%5d    &whi_mob : &W%5d\n\r",
			tarea->low_m_vnum,
			tarea->hi_m_vnum );
    ch_printf_color( ch, "&wsoft range: &W%d - %d    &whard range: &W%d - %d\n\r",
			tarea->low_soft_range, 
			tarea->hi_soft_range,
			tarea->low_hard_range, 
			tarea->hi_hard_range );
    ch_printf_color( ch, "&wArea flags: &W%s\n\r", flag_string(tarea->flags, area_flags) );
    ch_printf_color( ch, "&wResetmsg: &W%s\n\r", tarea->resetmsg ? tarea->resetmsg
						: "(default)" ); /* Rennard */
    ch_printf_color( ch, "&wReset frequency: &W%d &wminutes.\n\r",
		tarea->reset_frequency ? tarea->reset_frequency : 15 );

    ch_printf_color( ch, "&wContinent or Plane: &W%s\n\r", continents[tarea->continent] );
}


void do_aset( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *tarea;
    CLAN_DATA *clan=NULL;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MSL];
    bool proto, found;
    int vnum, value;

    set_char_color( AT_IMMORT, ch );
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    vnum = atoi( argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Usage: aset <area filename> <field> <value>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "  low_room hi_room low_obj hi_obj low_mob hi_mob\n\r", ch );
	send_to_char( "  name filename low_soft hi_soft low_hard hi_hard\n\r", ch );
	send_to_char( "  author resetmsg resetfreq flags income popsupport governed_by\n\r", ch );
        send_to_char( "Capture Code:\n\r", ch);
        send_to_char( "  income popsupport governed_by guard1 guard2 guard3 guard4\n\r", ch );
	return;
    }

    found = FALSE; proto = FALSE;
    for ( tarea = first_area; tarea; tarea = tarea->next )
	if ( !str_cmp( tarea->filename, arg1 ) )
	{
	  found = TRUE;
	  break;
	}

    if ( !found )
      for ( tarea = first_build; tarea; tarea = tarea->next )
	if ( !str_cmp( tarea->filename, arg1 ) )
	{
	  found = TRUE;
	  proto = TRUE;
	  break;
	}

    if ( !found )
    {
	send_to_char( "Area not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	DISPOSE( tarea->name );
	tarea->name = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( tarea->filename );
	tarea->filename = str_dup( argument );
	write_area_list( );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "continent" ) )
    {
      /* Area continent editing - Samson 8-8-98 */
    	if ( !argument || argument[0] == '\0' )
	{
	    send_to_char( "Set the area's continent.\n\r", ch );
	    send_to_char( "Usage: aset continent <name>\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg2 );
	value = get_continent( arg2 );
	if ( value < 0 || value > ACON_MAX )
	{
	   tarea->continent = 0;
	   send_to_char( "Invalid area continent, set to 'alsherok' by default.\n\r", ch );
	}
	else
	{
	   tarea->continent = value;
	   ch_printf ( ch, "Area continent set to %s.\n\r", arg2 );
	}
	return;
    }
    if ( !str_cmp( arg2, "low_economy" ) )
    {
	tarea->low_economy = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "popsupport" ) )
    {
        tarea->popsupport = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "guard1" ) )
    {
        tarea->guard1 = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "guard2" ) )
    {
        tarea->guard2 = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "guard3" ) )
    {
        tarea->guard3 = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "guard4" ) )
    {
        tarea->guard4 = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "planetname" ) )
    {
        DISPOSE( tarea->planetname );
        sprintf( buf, "%s", argument); 
       tarea->planetname = str_dup( buf );
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "governed_by" ) )
    {
        clan = get_clan_short( argument );
        if ( !clan )
        {
          send_to_char( "Invalid clan.\n\r", ch);
          return;
        }
        tarea->governed_by = str_dup(clan->shortname);
        send_to_char( "Done.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "income" ) )
    {
        tarea->income = vnum;
        send_to_char( "Done.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "high_economy" ) )
    {
	tarea->high_economy = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "low_room" ) )
    {
	tarea->low_r_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hi_room" ) )
    {
	tarea->hi_r_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "low_obj" ) )
    {
	tarea->low_o_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hi_obj" ) )
    {
	tarea->hi_o_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "low_mob" ) )
    {
	tarea->low_m_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hi_mob" ) )
    {
	tarea->hi_m_vnum = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "low_soft" ) )
    {
	if ( vnum < 0 || vnum > MAX_LEVEL )
        {
	    send_to_char( "That is not an acceptable value.\n\r", ch);
	    return;
	}

	tarea->low_soft_range = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hi_soft" ) )
    {
	if ( vnum < 0 || vnum > MAX_LEVEL )
        {
	    send_to_char( "That is not an acceptable value.\n\r", ch);
	    return;
	}

	tarea->hi_soft_range = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "low_hard" ) )
    {
	if ( vnum < 0 || vnum > MAX_LEVEL )
        {
	    send_to_char( "That is not an acceptable value.\n\r", ch);
	    return;
	}

	tarea->low_hard_range = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hi_hard" ) )
    {
	if ( vnum < 0 || vnum > MAX_LEVEL )
        {
	    send_to_char( "That is not an acceptable value.\n\r", ch);
	    return;
	}

	tarea->hi_hard_range = vnum;
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "author" ) )
    {
	STRFREE( tarea->author );
	tarea->author = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "resetmsg" ) )
    {
        if ( tarea->resetmsg )
          DISPOSE( tarea->resetmsg );
        if ( str_cmp( argument, "clear" ) )
          tarea->resetmsg = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        return;
    } /* Rennard */

    if ( !str_cmp( arg2, "resetfreq" ) )
    {
	tarea->reset_frequency = vnum;
        send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "flags" ) )
    {
	if ( !argument || argument[0] == '\0' )
	{
	   send_to_char( "Usage: aset <filename> flags <flag> [flag]...\n\r", ch );
	   return;
	}
	while ( argument[0] != '\0' )
	{
	   argument = one_argument( argument, arg3 );
	   value = get_areaflag( arg3 );
	   if ( value < 0 || value > 31 )
	     ch_printf( ch, "Unknown flag: %s\n\r", arg3 );
	   else
	   {
		if ( IS_SET( tarea->flags, 1 << value ) )
		    REMOVE_BIT( tarea->flags, 1 << value );
		else
		    SET_BIT( tarea->flags, 1 << value );
	   }
        }
        return;
    }

    do_aset( ch, "" );
    return;
}


void do_rlist( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*room;
    int			 vnum;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    AREA_DATA		*tarea;
    int lrange;
    int trange;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC(ch) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
    || ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
    {
	send_to_char_color( "&YYou don't have an assigned area.\n\r", ch );
	return;
    }

    tarea = ch->pcdata->area;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0]!='\0' && !is_number(arg1) )
       return;
    if ( arg2[0]!='\0' && !is_number(arg2) )
       return;

/* I know the above code could be combined into 3 lines, but we write code
   once and then read it many times. It is so much easier to read without
   trying to figure out what opening parenthesis belongs with what closing
   one.  -- Gorog
*/

/* Bah! The following code uses atoi(arg1) and atoi(arg2) without even
   checking they are numeric or even exist. I put a fix for this above.
   It also uses is_number(arg1) without checking if arg1 may be null.
   Caused a crash when the command was uses with a single alpha arg.
   -- Gorog
*/

    if ( tarea )
    {
      if ( arg1[0] == '\0' )		/* cleaned a big scary mess */
        lrange = tarea->low_r_vnum;	/* here.	    -Thoric */
      else
        lrange = atoi( arg1 );
      if ( arg2[0] == '\0' )
        trange = tarea->hi_r_vnum;
      else
        trange = atoi(arg2);

      if ( ( lrange < tarea->low_r_vnum || trange > tarea->hi_r_vnum )
  	&& get_trust( ch ) < LEVEL_GREATER )
      {
	send_to_char_color("&YThat is out of your vnum range.\n\r", ch);
	return;
      }
    }
   else
    {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 1 );
    }

    for ( vnum = lrange; vnum <= trange; vnum++ )
    {
	if ( (room = get_room_index( vnum )) == NULL )
	  continue;
	ch_printf( ch, "&Y%6d&W) %s&w\n\r", vnum, room->name );
    }
    return;
}

void do_olist( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA	*obj;
    int			 vnum;
    AREA_DATA		*tarea;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int lrange;
    int trange;

    /*
     * Greater+ can list out of assigned range - Tri (mlist/rlist as well)
     */

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC(ch) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
    || ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
    {
	send_to_char_color( "&YYou don't have an assigned area.\n\r", ch );
	return;
    }
    tarea = ch->pcdata->area;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( tarea )
    {
      if ( arg1[0] == '\0' )		/* cleaned a big scary mess */
        lrange = tarea->low_o_vnum;	/* here.	    -Thoric */
      else
        lrange = atoi( arg1 );
      if ( arg2[0] == '\0' )
        trange = tarea->hi_o_vnum;
      else
        trange = atoi(arg2);

      if ((lrange < tarea->low_o_vnum || trange > tarea->hi_o_vnum)
      &&   get_trust( ch ) < LEVEL_GREATER )
      {
	send_to_char_color("&YThat is out of your vnum range.\n\r", ch);
	return;
      }
    }
   else
    {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 3 );
    }

    for ( vnum = lrange; vnum <= trange; vnum++ )
    {
	if ( (obj = get_obj_index( vnum )) == NULL )
	  continue;
	ch_printf( ch, "%5d) %-20s (%s)\n\r", vnum,
					     obj->name,
					     obj->short_descr );
    }
    return;
}

void do_mlist( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA	*mob;
    int			 vnum;
    AREA_DATA		*tarea;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int lrange;
    int trange;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC(ch) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
    ||  ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
    {
	send_to_char_color( "&YYou don't have an assigned area.\n\r", ch );
	return;
    }

    tarea = ch->pcdata->area;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( tarea )
    {
      if ( arg1[0] == '\0' )		/* cleaned a big scary mess */
        lrange = tarea->low_m_vnum;	/* here.	    -Thoric */
      else
        lrange = atoi( arg1 );
      if ( arg2[0] == '\0' )
        trange = tarea->hi_m_vnum;
      else
        trange = atoi( arg2 );

      if ( ( lrange < tarea->low_m_vnum || trange > tarea->hi_m_vnum )
	&& get_trust( ch ) < LEVEL_GREATER )
      {
  	send_to_char_color("&YThat is out of your vnum range.\n\r", ch);
	return;
      }
    }
    else
    {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 1 );
    }
    
    for ( vnum = lrange; vnum <= trange; vnum++ )
    {
	  if ( (mob = get_mob_index( vnum )) == NULL )
	    continue;
	  pager_printf( ch, "%5d) %-20s '%s'\n\r", vnum,
					 mob->player_name,
					 mob->short_descr );
    }
}

void mpedit( CHAR_DATA *ch, MPROG_DATA *mprg, int mptype, char *argument )
{
    if ( mptype != -1 )
    {
	mprg->type = mptype;
	if ( mprg->arglist )
	    STRFREE( mprg->arglist );
	mprg->arglist = STRALLOC( argument );
    }
    ch->substate = SUB_MPROG_EDIT;
    ch->dest_buf = mprg;
    if ( !mprg->comlist )
	mprg->comlist = STRALLOC( "" );
    start_editing( ch, mprg->comlist );
    return;
}

/*
 * Mobprogram editing - cumbersome				-Thoric
 */
void do_mpedit( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    CHAR_DATA  *victim;
    MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
    int value, mptype = -1, cnt;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't mpedit\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_MPROG_EDIT:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_mpedit: sub_mprog_edit: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  mprog	 = ch->dest_buf;
	  if ( mprog->comlist )
	    STRFREE( mprog->comlist );
	  mprog->comlist = copy_buffer( ch );
	  stop_editing( ch );
	  return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    value = atoi( arg3 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: mpedit <victim> <command> [number] <program> <value>\n\r", ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Command being one of:\n\r",			ch );
	send_to_char( "  add delete insert edit list\n\r",		ch );
	send_to_char( "Program being one of:\n\r",			ch );
	send_to_char( "  act speech rand fight hitprcnt greet allgreet\n\r", ch );
	send_to_char( "  entry give bribe death time hour script emote custom\n\r",ch );
	return;
    }

    if ( get_trust( ch ) < LEVEL_GOD )
    {
      if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
      {
	send_to_char( "They aren't here.\n\r", ch );
	return;
      }
    }
    else
    {
      if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
      {
	send_to_char( "No one like that in all the realms.\n\r", ch );
	return;
      }
    }

    if ( get_trust( ch ) < victim->level || !IS_NPC(victim) )
    {
	send_to_char( "You can't do that!\n\r", ch );
	return;
    }
    if ( get_trust( ch ) < LEVEL_GREATER
    &&   IS_NPC( victim )
    &&  xIS_SET( victim->act, ACT_STATSHIELD ) )
    {
        set_pager_color( AT_IMMORT, ch );
        send_to_pager( "Their godly glow prevents you from getting close enough.\n\r", ch );
        return;
    }         
    if ( !can_mmodify( ch, victim ) )
	return;

    if ( !xIS_SET(victim->act, ACT_PROTOTYPE) )
    {
	send_to_char( "A mobile must have a prototype flag to be mpset.\n\r", ch );
	return;
    }

    mprog = victim->pIndexData->mudprogs;

    set_char_color( AT_GREEN, ch );

    if ( !str_cmp( arg2, "list" ) )
    {
	cnt = 0;
	if ( !mprog )
	{
	   send_to_char( "That mobile has no mob programs.\n\r", ch );
	   return;
	}
	
	if(value < 1)
	{
		if(strcmp("full", arg3))
		{
			for(mprg = mprog; mprg; mprg = mprg->next)
			{
				ch_printf(ch, "%d>%s %s\n\r",
					++cnt,
					mprog_type_to_name(mprg->type),
					mprg->arglist);
			}
			
			return;
		}
		else
		{
			for(mprg = mprog; mprg; mprg = mprg->next)
			{
				ch_printf(ch, "%d>%s %s\n\r%s\n\r",
					++cnt,
					mprog_type_to_name(mprg->type),
					mprg->arglist,
					mprg->comlist);
			}
			
			return;
		}
	}
	
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
		if(++cnt == value)
		{
			ch_printf( ch, "%d>%s %s\n\r%s\n\r",
	      			cnt,
	      			mprog_type_to_name( mprg->type ),
	      			mprg->arglist,
	      			mprg->comlist );
	      		break;
	      	}	
	}
	
	if(!mprg)
		send_to_char("Program not found.\n\r", ch);
	
	return;
    }

    if ( !str_cmp( arg2, "edit" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "That mobile has no mob programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	if ( arg4[0] != '\0' )
	{
	  mptype = get_mpflag( arg4 );
	  if ( mptype == -1 )
	  {
	    send_to_char( "Unknown program type.\n\r", ch );
	    return;
	  }
	}
	else
	  mptype = -1;
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mpedit( ch, mprg, mptype, argument );
		xCLEAR_BITS(victim->pIndexData->progtypes);
		for ( mprg = mprog; mprg; mprg = mprg->next )
		   xSET_BIT(victim->pIndexData->progtypes, mprg->type);
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "delete" ) )
    {
	int num;
	bool found;

	if ( !mprog )
	{
	   send_to_char( "That mobile has no mob programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0; found = FALSE;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mptype = mprg->type;
		found = TRUE;
		break;
	   }
	}
	if ( !found )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = num = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	   if ( mprg->type == mptype )
	     num++;
	if ( value == 1 )
	{
	   mprg_next = victim->pIndexData->mudprogs;
	   victim->pIndexData->mudprogs = mprg_next->next;
	}
	else
	for ( mprg = mprog; mprg; mprg = mprg_next )
	{
	   mprg_next = mprg->next;
	   if ( ++cnt == (value - 1) )
	   {
		mprg->next = mprg_next->next;
		break;
	   }
	}
	if ( mprg_next )
	{
	    STRFREE( mprg_next->arglist );
	    STRFREE( mprg_next->comlist );
	    DISPOSE( mprg_next );
	    if ( num <= 1 )
		xREMOVE_BIT( victim->pIndexData->progtypes, mptype );
	    send_to_char( "Program removed.\n\r", ch );
	}
	return;
    }

    if ( !str_cmp( arg2, "insert" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "That mobile has no mob programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	mptype = get_mpflag( arg4 );
	if ( mptype == -1 )
	{
	   send_to_char( "Unknown program type.\n\r", ch );
	   return;
	}
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	if ( value == 1 )
	{
	   CREATE( mprg, MPROG_DATA, 1 );
	   xSET_BIT(victim->pIndexData->progtypes, mptype);
	   mpedit( ch, mprg, mptype, argument );
	   mprg->next = mprog;
	   victim->pIndexData->mudprogs = mprg;
	   return;
	}
	cnt = 1;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value && mprg->next )
	   {
		CREATE( mprg_next, MPROG_DATA, 1 );
		xSET_BIT(victim->pIndexData->progtypes, mptype);
		mpedit( ch, mprg_next, mptype, argument );
		mprg_next->next = mprg->next;
		mprg->next	= mprg_next;
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "add" ) )
    {
	mptype = get_mpflag( arg3 );
	if ( mptype == -1 )
	{
	   send_to_char( "Unknown program type.\n\r", ch );
	   return;
	}
	if ( mprog != NULL )
	  for ( ; mprog->next; mprog = mprog->next );
	CREATE( mprg, MPROG_DATA, 1 );
	if ( mprog )
	  mprog->next			= mprg;
	else
	  victim->pIndexData->mudprogs	= mprg;
	xSET_BIT(victim->pIndexData->progtypes,	mptype);
	mpedit( ch, mprg, mptype, argument );
	mprg->next = NULL;
	return;
    }

    do_mpedit( ch, "" );
}

void do_opedit( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    OBJ_DATA   *obj;
    MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
    int value, mptype = -1, cnt;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't opedit\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_MPROG_EDIT:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_opedit: sub_oprog_edit: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  mprog	 = ch->dest_buf;
	  if ( mprog->comlist )
	    STRFREE( mprog->comlist );
	  mprog->comlist = copy_buffer( ch );
	  stop_editing( ch );
	  return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    value = atoi( arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: opedit <object> <command> [number] <program> <value>\n\r", ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Command being one of:\n\r",			ch );
	send_to_char( "  add delete insert edit list\n\r",		ch );
	send_to_char( "Program being one of:\n\r",			ch );
	send_to_char( "  act speech rand wear remove sac zap get\n\r",  ch );
	send_to_char( "  drop damage repair greet exa use emote custom\n\r",ch );
	send_to_char( "  pull push (for levers,pullchains,buttons)\n\r",ch );
	send_to_char( "\n\r", ch);
	send_to_char( "Object should be in your inventory to edit.\n\r",ch);
	return;
    }

    if ( get_trust( ch ) < LEVEL_GOD )
    {
      if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
      {
	send_to_char( "You aren't carrying that.\n\r", ch );
	return;
      }
    }
    else
    {
      if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
      {
	send_to_char( "Nothing like that in all the realms.\n\r", ch );
	return;
      }
    }

    if ( !can_omodify( ch, obj ) )
	return;

    if ( !IS_OBJ_STAT(obj, ITEM_PROTOTYPE) )
    {
	send_to_char( "An object must have a prototype flag to be opset.\n\r", ch );
	return;
    }

    mprog = obj->pIndexData->mudprogs;

    set_char_color( AT_GREEN, ch );

    if ( !str_cmp( arg2, "list" ) )
    {
	cnt = 0;
	if ( !mprog )
	{
	   send_to_char( "That object has no obj programs.\n\r", ch );
	   return;
	}
	for ( mprg = mprog; mprg; mprg = mprg->next )
	  ch_printf( ch, "%d>%s %s\n\r%s\n\r",
	      ++cnt,
	      mprog_type_to_name( mprg->type ),
	      mprg->arglist,
	      mprg->comlist );
	return;
    }

    if ( !str_cmp( arg2, "edit" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "That object has no obj programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	if ( arg4[0] != '\0' )
	{
	  mptype = get_mpflag( arg4 );    
	  if ( mptype == -1 )
	  {
	    send_to_char( "Unknown program type.\n\r", ch );
	    return;
	  }
	}
	else
	  mptype = -1;
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mpedit( ch, mprg, mptype, argument );
		xCLEAR_BITS(obj->pIndexData->progtypes);
		for ( mprg = mprog; mprg; mprg = mprg->next )
		   xSET_BIT(obj->pIndexData->progtypes, mprg->type);
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "delete" ) )
    {
	int num;
	bool found;

	if ( !mprog )
	{
	   send_to_char( "That object has no obj programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0; found = FALSE;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mptype = mprg->type;
		found = TRUE;
		break;
	   }
	}
	if ( !found )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = num = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	   if ( mprg->type == mptype )
	     num++;
	if ( value == 1 )
	{
	   mprg_next = obj->pIndexData->mudprogs;
	   obj->pIndexData->mudprogs = mprg_next->next;
	}
	else
	for ( mprg = mprog; mprg; mprg = mprg_next )
	{
	   mprg_next = mprg->next;
	   if ( ++cnt == (value - 1) )
	   {
		mprg->next = mprg_next->next;
		break;
	   }
	}
	if ( mprg_next )
	{
	    STRFREE( mprg_next->arglist );
	    STRFREE( mprg_next->comlist );
	    DISPOSE( mprg_next );
	    if ( num <= 1 )
		xREMOVE_BIT( obj->pIndexData->progtypes, mptype );
	    send_to_char( "Program removed.\n\r", ch );
	}
	return;
    }

    if ( !str_cmp( arg2, "insert" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "That object has no obj programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg4 );
	mptype = get_mpflag( arg4 );
	if ( mptype == -1 )
	{
	    send_to_char( "Unknown program type.\n\r", ch );
	    return;
	}
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	if ( value == 1 )
	{
	   CREATE( mprg, MPROG_DATA, 1 );
	   xSET_BIT(obj->pIndexData->progtypes, mptype);
	   mpedit( ch, mprg, mptype, argument );
	   mprg->next = mprog;
	   obj->pIndexData->mudprogs = mprg;
	   return;
	}
	cnt = 1;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value && mprg->next )
	   {
		CREATE( mprg_next, MPROG_DATA, 1 );
		xSET_BIT(obj->pIndexData->progtypes, mptype);
		mpedit( ch, mprg_next, mptype, argument );
		mprg_next->next = mprg->next;
		mprg->next	= mprg_next;
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "add" ) )
    {
	mptype = get_mpflag( arg3 );
	if ( mptype == -1 )
	{
	   send_to_char( "Unknown program type.\n\r", ch );
	   return;
	}
	if ( mprog != NULL )
	for ( ; mprog->next; mprog = mprog->next );
	CREATE( mprg, MPROG_DATA, 1 );
	if ( mprog )
	  mprog->next			 = mprg;
	else
	  obj->pIndexData->mudprogs	 = mprg;
	xSET_BIT(obj->pIndexData->progtypes, mptype);
	mpedit( ch, mprg, mptype, argument );
	mprg->next = NULL;
	return;
    }

    do_opedit( ch, "" );
}



/*
 * RoomProg Support
 */
void rpedit( CHAR_DATA *ch, MPROG_DATA *mprg, int mptype, char *argument )
{
	if ( mptype != -1 )
	{
	  mprg->type = mptype;
	  if ( mprg->arglist )
	    STRFREE( mprg->arglist );
	  mprg->arglist = STRALLOC( argument );
	}
	ch->substate = SUB_MPROG_EDIT;
	ch->dest_buf = mprg;
	if(!mprg->comlist)
          mprg->comlist = STRALLOC("");
	start_editing( ch, mprg->comlist );
	return;
}

void do_rpedit( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
    int value, mptype = -1, cnt;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't rpedit\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_MPROG_EDIT:
	  if ( !ch->dest_buf )
	  {
		send_to_char( "Fatal error: report to Thoric.\n\r", ch );
		bug( "do_opedit: sub_oprog_edit: NULL ch->dest_buf", 0 );
		ch->substate = SUB_NONE;
		return;
	  }
	  mprog	 = ch->dest_buf;
	  if ( mprog->comlist )
	    STRFREE( mprog->comlist );
	  mprog->comlist = copy_buffer( ch );
	  stop_editing( ch );
	  return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    value = atoi( arg2 );
    /* argument = one_argument( argument, arg3 ); */

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: rpedit <command> [number] <program> <value>\n\r", ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Command being one of:\n\r",			ch );
	send_to_char( "  add delete insert edit list\n\r",		ch );
	send_to_char( "Program being one of:\n\r",			ch );
	send_to_char( "  act speech rand sleep rest rfight enter\n\r",  ch );
	send_to_char( "  leave death emote custom\n\r",                 ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "You should be standing in room you wish to edit.\n\r",ch);
	return;
    }

    if ( !can_rmodify( ch, ch->in_room ) )
	return;

    mprog = ch->in_room->mudprogs;

    set_char_color( AT_GREEN, ch );

    if ( !str_cmp( arg1, "list" ) )
    {
	cnt = 0;
	if ( !mprog )
	{
	   send_to_char( "This room has no room programs.\n\r", ch );
	   return;
	}
	for ( mprg = mprog; mprg; mprg = mprg->next )
	  ch_printf( ch, "%d>%s %s\n\r%s\n\r",
	      ++cnt,
	      mprog_type_to_name( mprg->type ),
	      mprg->arglist,
	      mprg->comlist );
	return;
    }

    if ( !str_cmp( arg1, "edit" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "This room has no room programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg3 );
	if ( arg3[0] != '\0' )
	{
	  mptype = get_mpflag( arg3 );    
	  if ( mptype == -1 )
	  {
	    send_to_char( "Unknown program type.\n\r", ch );
	    return;
	  }
	}
	else
	  mptype = -1;
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mpedit( ch, mprg, mptype, argument );
		xCLEAR_BITS(ch->in_room->progtypes);
		for ( mprg = mprog; mprg; mprg = mprg->next )
		   xSET_BIT(ch->in_room->progtypes, mprg->type);
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "delete" ) )
    {
	int num;
	bool found;

	if ( !mprog )
	{
	   send_to_char( "That room has no room programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg3 );
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = 0; found = FALSE;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value )
	   {
		mptype = mprg->type;
		found = TRUE;
		break;
	   }
	}
	if ( !found )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	cnt = num = 0;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	   if ( mprg->type == mptype )
	     num++;
	if ( value == 1 )
	{
	   mprg_next = ch->in_room->mudprogs;
	   ch->in_room->mudprogs = mprg_next->next;
	}
	else
	for ( mprg = mprog; mprg; mprg = mprg_next )
	{
	   mprg_next = mprg->next;
	   if ( ++cnt == (value - 1) )
	   {
		mprg->next = mprg_next->next;
		break;
	   }
	}
	if ( mprg_next )
	{
	    STRFREE( mprg_next->arglist );
	    STRFREE( mprg_next->comlist );
	    DISPOSE( mprg_next );
	    if ( num <= 1 )
		xREMOVE_BIT( ch->in_room->progtypes, mptype );
	    send_to_char( "Program removed.\n\r", ch );
	}
	return;
    }

    if ( !str_cmp( arg2, "insert" ) )
    {
	if ( !mprog )
	{
	   send_to_char( "That room has no room programs.\n\r", ch );
	   return;
	}
	argument = one_argument( argument, arg3 );
	mptype = get_mpflag( arg2 );
	if ( mptype == -1 )
	{
	   send_to_char( "Unknown program type.\n\r", ch );
	   return;
	}
	if ( value < 1 )
	{
	   send_to_char( "Program not found.\n\r", ch );
	   return;
	}
	if ( value == 1 )
	{
	   CREATE( mprg, MPROG_DATA, 1 );
	   xSET_BIT(ch->in_room->progtypes, mptype);
	   mpedit( ch, mprg, mptype, argument );
	   mprg->next = mprog;
	   ch->in_room->mudprogs = mprg;
	   return;
	}
	cnt = 1;
	for ( mprg = mprog; mprg; mprg = mprg->next )
	{
	   if ( ++cnt == value && mprg->next )
	   {
		CREATE( mprg_next, MPROG_DATA, 1 );
		xSET_BIT(ch->in_room->progtypes, mptype);
		mpedit( ch, mprg_next, mptype, argument );
		mprg_next->next = mprg->next;
		mprg->next	= mprg_next;
		return;
	   }
	}
	send_to_char( "Program not found.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "add" ) )
    {
	mptype = get_mpflag( arg2 );
	if ( mptype == -1 )
	{
	   send_to_char( "Unknown program type.\n\r", ch );
	   return;
	}
	if ( mprog )
	  for ( ; mprog->next; mprog = mprog->next );
	CREATE( mprg, MPROG_DATA, 1 );
	if ( mprog )
	  mprog->next		= mprg;
	else
	  ch->in_room->mudprogs	= mprg;
	xSET_BIT(ch->in_room->progtypes, mptype);
	mpedit( ch, mprg, mptype, argument );
	mprg->next = NULL;
	return;
    }

    do_rpedit( ch, "" );
}

void do_rdelete( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg1[MIL];
    char arg2[MIL];
    int firstvnum, lastvnum;
    ROOM_INDEX_DATA *location;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: rdelete <vnum>\n\r        rdelete <first vnum> <last vnum>\n\r", ch );
	return;
    }
    if (!str_cmp( arg, "loop" ) )
    {
      if ( arg1[0] == '\0' || arg2[0] == '\0' )
      {
          send_to_char( "Syntax: rdelete <vnum>\n\r        rdelete <first vnum> <last vnum>\n\r", ch );
          return;
      }
      if (!is_number(arg1) || !is_number(arg2) )
      {
          send_to_char( "The vnums must be numbers.\n\r", ch );
          return;
      }
      firstvnum = atoi(arg1);
      lastvnum = atoi(arg2);
      if ( firstvnum >= lastvnum)
      {
          send_to_char( "Firstvnum MUST be lower than the last.\n\r", ch );
          return;
      }
      for ( firstvnum = atoi(arg1); firstvnum <= lastvnum; firstvnum++ )
      {
         if ( ( location = get_room_index( firstvnum ) ) == NULL )
         {
             ch_printf( ch, "No such room.\n\rRoom: %d", firstvnum );
             continue;
         }
         /* Does the player have the right to delete this room? */
         if ( get_trust( ch ) < sysdata.level_modify_proto
         && ( location->vnum < ch->pcdata->r_range_lo ||
              location->vnum > ch->pcdata->r_range_hi ) )
         {
           if ( !ch->plr_home)
           {
             send_to_char( "That room is not in your assigned range.\n\r", ch );
             continue;
           }
         }

         /* We could go to the trouble of clearing out the room, but why? */
         /* Delete_room does that anyway, but this is probably safer:) */
        if ( location->first_person || location->first_content )
        {
           send_to_char( "The room must be empty first.\n\r", ch );
           return;
         }

         /* Wipe out resets too */
        {
         AREA_DATA *pArea;

         pArea = location->area;
        }

         /* Ok, we've determined that the room exists, it is empty and the
            player has the authority to delete it, so let's dump the thing.
            The function to do it is in db.c so it can access the top-room
            variable. */
         delete_room( location );
	 fix_exits(); /* Need to call this to solve a crash */
         send_to_char( "Room deleted.\n\r", ch );
         continue;

      }
    }
    /* Find the room. */
    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    /* Does the player have the right to delete this room? */
    if ( get_trust( ch ) < sysdata.level_modify_proto
    && ( location->vnum < ch->pcdata->r_range_lo || 
         location->vnum > ch->pcdata->r_range_hi ) )
    {
      if ( !ch->plr_home)
      {
        send_to_char( "That room is not in your assigned range.\n\r", ch );
        return;
      }
    }

    /* We could go to the trouble of clearing out the room, but why? */
    /* Delete_room does that anyway, but this is probably safer:) */
    if ( location->first_person || location->first_content )
    {
      send_to_char( "The room must be empty first.\n\r", ch );
      return;
    }


    /* Ok, we've determined that the room exists, it is empty and the 
       player has the authority to delete it, so let's dump the thing. 
       The function to do it is in db.c so it can access the top-room 
       variable. */
    delete_room( location );
    fix_exits(); /* Need to call this to solve a crash */
    send_to_char( "Room deleted.\n\r", ch );
    return;
}

void do_odelete( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *obj;
    OBJ_DATA *temp;

    argument = one_argument( argument, arg );

    /* Temporarily disable this command. */
/*    return;*/

    if ( arg[0] == '\0' )
    {
	send_to_char( "Delete which object?\n\r", ch );
	return;
    }

    /* Find the object. */
    if (!(obj = get_obj_index(atoi(arg))))
    {
      if (!(temp = get_obj_here(ch, arg)))
      {
        send_to_char( "No such object.\n\r", ch );
        return;
      }
      obj = temp->pIndexData;
    }

    /* Does the player have the right to delete this room? */
    if ( get_trust( ch ) < sysdata.level_modify_proto
    && ( obj->vnum < ch->pcdata->o_range_lo || 
         obj->vnum > ch->pcdata->o_range_hi ) )
    {
      send_to_char( "That object is not in your assigned range.\n\r", ch );
      return;
    }

    /* Ok, we've determined that the room exists, it is empty and the 
       player has the authority to delete it, so let's dump the thing. 
       The function to do it is in db.c so it can access the top-room 
       variable. */
    delete_obj( obj );

    send_to_char( "Object deleted.\n\r", ch );
    return;
}

void do_mdelete( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *mob;
    CHAR_DATA *temp;

    argument = one_argument( argument, arg );

    /* Temporarily disable this command. */
/*    return;*/

    if ( arg[0] == '\0' )
    {
	send_to_char( "Delete which mob?\n\r", ch );
	return;
    }

    /* Find the mob. */
    if (!(mob = get_mob_index(atoi(arg))))
    {
      if (!(temp = get_char_room(ch, arg)) || !IS_NPC(temp))
      {
        send_to_char( "No such mob.\n\r", ch );
        return;
      }
      mob = temp->pIndexData;
    }

    /* Does the player have the right to delete this room? */
    if ( get_trust( ch ) < sysdata.level_modify_proto
    && ( mob->vnum < ch->pcdata->m_range_lo || 
         mob->vnum > ch->pcdata->m_range_hi ) )
    {
      send_to_char( "That mob is not in your assigned range.\n\r", ch );
      return;
    }

    /* Ok, we've determined that the mob exists and the player has the
       authority to delete it, so let's dump the thing.
       The function to do it is in db.c so it can access the top_mob_index
       variable. */
    delete_mob( mob );

    send_to_char( "Mob deleted.\n\r", ch );
    return;
}

/*  
 *  Mobile and Object Program Copying 
 *  Last modified Feb. 24 1999
 *  Mystaric
 */
 
void mpcopy( MPROG_DATA *source, MPROG_DATA *destination)
{
   destination->type=source->type;
   destination->triggered=source->triggered;
   destination->resetdelay=source->resetdelay;
   destination->arglist = STRALLOC( source->arglist);
   destination->comlist = STRALLOC( source->comlist);               
   destination->next = NULL;
}   

void do_opcopy( CHAR_DATA *ch, char *argument )
{
    char sobj [MAX_INPUT_LENGTH];
    char prog [MAX_INPUT_LENGTH];
    char num  [MAX_INPUT_LENGTH];
    char dobj [MAX_INPUT_LENGTH];
    OBJ_DATA  *source=NULL, *destination=NULL;
    MPROG_DATA *source_oprog=NULL, *dest_oprog=NULL, *source_oprg=NULL, *dest_oprg=NULL;
    int value = -1, optype = -1, cnt=0;
    bool COPY = FALSE;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't opcopy\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, sobj );
    argument = one_argument( argument, prog );

    if ( sobj[0] == '\0' || prog[0] == '\0' )
    {
        send_to_char( "Syntax: opcopy <source object> <program> [number] <destination object>\n\r", ch );
        send_to_char( "        opcopy <source object> all <destination object>\n\r", ch );
        send_to_char( "        opcopy <source object> all <destination object> <program>\n\r", ch );    
	send_to_char( "\n\r",						ch );
	send_to_char( "Program being one of:\n\r",			ch );
	send_to_char( "  act speech rand wear remove sac zap get\n\r",  ch );
	send_to_char( "  drop damage repair greet exa use\n\r",ch );
	send_to_char( "  pull push (for levers,pullchains,buttons)\n\r",ch );
	send_to_char( "\n\r", ch);
	send_to_char( "Object should be in your inventory to edit.\n\r",ch);
	return;
    }

    if (!strcmp( prog, "all") )
    { 
        argument = one_argument( argument, dobj );
        argument = one_argument( argument, prog ); 
        optype = get_mpflag(prog);
        COPY = TRUE;
    }    
    else
    {
       argument = one_argument( argument, num );
       argument = one_argument( argument, dobj );
       value = atoi( num );
    }

    if ( get_trust( ch ) < LEVEL_GOD )
    {
        if ( ( source = get_obj_carry( ch, sobj ) ) == NULL )
        {
   	   send_to_char( "You aren't carrying source object.\n\r", ch );
	   return;
        }
      
        if ( ( destination = get_obj_carry( ch, dobj ) ) == NULL )
        {
   	   send_to_char( "You aren't carrying destination object.\n\r", ch );
	   return;
        }
    }
    else
    {
        if ( ( source = get_obj_world( ch, sobj ) ) == NULL )
        {
	    send_to_char( "Can't find source object in all the realms.\n\r", ch );
	    return;
        }

        if ( ( destination = get_obj_world( ch, dobj ) ) == NULL )
        {
	    send_to_char( "Can't find destination object in all the realms.\n\r", ch );
	    return;
        }
   }

    if ( source == destination )
    {
        send_to_char( "Source and destination objects cannot be the same\n\r", ch);
        return;
    }


    if ( !can_omodify( ch, destination ) )
    {
        send_to_char( "You cannot modify destination object.\n\r", ch); 
	return;
    }

    if ( !IS_OBJ_STAT(destination, ITEM_PROTOTYPE) )
    {
	send_to_char( "Destination object must have prototype flag.\n\r", ch );
	return;
    }

    set_char_color( AT_PLAIN, ch );
    
    source_oprog = source->pIndexData->mudprogs;
    dest_oprog = destination->pIndexData->mudprogs;



    set_char_color( AT_GREEN, ch );

    if ( !source_oprog )
    {
        send_to_char( "Source object has no mob programs.\n\r", ch );
	return;
    }

    if(COPY)
    {
        for ( source_oprg = source_oprog; source_oprg; source_oprg = source_oprg->next )
        {
	    if ( optype==source_oprg->type || optype == -1 )
	    {
	        if ( dest_oprog != NULL )
	        for ( ; dest_oprog->next; dest_oprog = dest_oprog->next );
  	        CREATE( dest_oprg, MPROG_DATA, 1 );
  	        if (dest_oprog ) 
	           dest_oprog->next= dest_oprg;
   	        else
                { 
                   destination->pIndexData->mudprogs = dest_oprg;
    	           dest_oprog = dest_oprg;
    	        }    
    	        mpcopy( source_oprg, dest_oprg );
    	        xSET_BIT(destination->pIndexData->progtypes, dest_oprg->type);
    	        cnt++;
    	    }
        }
    
    if (cnt == 0)
    {
       ch_printf(ch, "No such program in source object\n\r");
       return;
    }         
    ch_printf(ch, "%d programs successfully copied from %s to %s.\n\r", cnt, sobj, dobj); 
    return;
    }    
     
     if(value < 1 )
     {
         send_to_char( "No such program in source object.\n\r", ch );
         return;
     }

     optype = get_mpflag(prog);
	
     for ( source_oprg = source_oprog; source_oprg; source_oprg = source_oprg->next )
     {
         if(++cnt == value && source_oprg->type == optype )
	 {
	     if ( dest_oprog != NULL )
	         for ( ; dest_oprog->next; dest_oprog = dest_oprog->next );
  	     CREATE( dest_oprg, MPROG_DATA, 1 );
  	     if (dest_oprog ) 
	         dest_oprog->next= dest_oprg;
   	     else
                 destination->pIndexData->mudprogs = dest_oprg;
	     mpcopy( source_oprg, dest_oprg );
             xSET_BIT(destination->pIndexData->progtypes, dest_oprg->type);
             ch_printf(ch, "%s program %d from %s successfully copied to %s.\n\r", 
             prog, value, sobj, dobj); 
             return;
         }   
     }
     if (!source_oprg)
     {
         send_to_char( "No such program in source object.\n\r", ch );
         return;
      }
      do_opcopy( ch, "" );
}

void do_mpcopy( CHAR_DATA *ch, char *argument )
{
    char smob [MAX_INPUT_LENGTH];
    char prog [MAX_INPUT_LENGTH];
    char num  [MAX_INPUT_LENGTH];
    char dmob [MAX_INPUT_LENGTH];
    CHAR_DATA  *source=NULL, *destination=NULL;
    MPROG_DATA *source_mprog=NULL, *dest_mprog=NULL, *source_mprg=NULL, *dest_mprg=NULL;
    int value = -1, mptype = -1, cnt = 0;
    bool COPY = FALSE;
    
    set_char_color( AT_PLAIN, ch );
 
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mob's can't opcop\n\r", ch );
	return;    
    }

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor\n\r", ch );
	return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, smob );
    argument = one_argument( argument, prog );

    if ( smob[0] == '\0' || prog[0] == '\0' )
    {
        send_to_char( "Syntax: mpcopy <source mobile> <program> [number] <destination mobile>\n\r", ch );
        send_to_char( "        mpcopy <source mobile> all <destination mobile>\n\r", ch );
        send_to_char( "        mpcopy <source mobile> all <destination mobile> <program>\n\r", ch );    
	send_to_char( "\n\r",						ch );
	send_to_char( "Program being one of:\n\r",			ch );
	send_to_char( "  act speech rand fight hitprcnt greet allgreet\n\r", ch );
	send_to_char( "  entry give bribe death time hour script\n\r",	ch );
	return;
    }

    if (!strcmp( prog, "all") )
    {
        argument = one_argument( argument, dmob );
        argument = one_argument( argument, prog ); 
        mptype = get_mpflag(prog);
        COPY = TRUE;
    }    
    else
    {
       argument = one_argument( argument, num );
       argument = one_argument( argument, dmob );
       value = atoi( num );
    }
    
    if ( get_trust( ch ) < LEVEL_GOD )
    {
        if ( ( source = get_char_room( ch, smob ) ) == NULL )
        {
            send_to_char( "Source mobile is not present.\n\r", ch );
            return;
        }
        
        if ( ( destination = get_char_room( ch, dmob ) ) == NULL )
        {
            send_to_char( "Destination mobile is not present.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( ( source = get_char_world( ch, smob ) ) == NULL )
        {
            send_to_char( "Can't find source mobile\n\r", ch );
            return;
        }
       
        if ( ( destination = get_char_world( ch, dmob ) ) == NULL )
        {
            send_to_char( "Can't find destination mobile\n\r", ch );
            return;
        }
    }
    if ( source == destination )
    {
        send_to_char( "Source and destination mobiles cannot be the same\n\r", ch);
        return;
    }

    if ( get_trust( ch ) < source->level || !IS_NPC(source) || 
         get_trust( ch) < destination->level || !IS_NPC(destination) ) 
    {
	send_to_char( "You can't do that!\n\r", ch );
	return;
    }

    if ( !can_mmodify( ch, destination ) )
    {
        send_to_char( "You cannot modify destination mobile.\n\r", ch); 
	return;
    }
    
    if ( !xIS_SET(destination->act, ACT_PROTOTYPE) )
    {
	send_to_char( "Destination mobile must have a prototype flag to mpcopy.\n\r", ch );
	return;
    }

    source_mprog = source->pIndexData->mudprogs;
    dest_mprog = destination->pIndexData->mudprogs;

    set_char_color( AT_GREEN, ch );

    if ( !source_mprog )
    {
        send_to_char( "Source mobile has no mob programs.\n\r", ch );
	return;
    }

    
    if(COPY)
    {
        for ( source_mprg = source_mprog; source_mprg; source_mprg = source_mprg->next )
        {
	    if ( mptype==source_mprg->type || mptype == -1 )
	    {
	        if ( dest_mprog != NULL )
	            for ( ; dest_mprog->next; dest_mprog = dest_mprog->next );
  	        CREATE( dest_mprg, MPROG_DATA, 1 );

  	        if (dest_mprog ) 
	            dest_mprog->next= dest_mprg;
   	        else
                {
                    destination->pIndexData->mudprogs = dest_mprg;
                    dest_mprog = dest_mprg;
                }
                mpcopy( source_mprg, dest_mprg );
                xSET_BIT(destination->pIndexData->progtypes, dest_mprg->type);               
    	        cnt++;
    	    }
        }
    
    if (cnt == 0)
    {
       ch_printf(ch, "No such program in source mobile\n\r");
       return;
    }         
    ch_printf(ch, "%d programs successfully copied from %s to %s.\n\r", cnt, smob, dmob); 
    return;
    }    
     
     if(value < 1 )
     {
         send_to_char( "No such program in source mobile.\n\r", ch );
         return;
     }

     mptype = get_mpflag(prog);
	
     for ( source_mprg = source_mprog; source_mprg; source_mprg = source_mprg->next )
     {
         if(++cnt == value && source_mprg->type == mptype )
	 {
	     if ( dest_mprog != NULL )
	         for ( ; dest_mprog->next; dest_mprog = dest_mprog->next );
  	     CREATE( dest_mprg, MPROG_DATA, 1 );
  	     if (dest_mprog ) 
	         dest_mprog->next= dest_mprg;
   	     else
                 destination->pIndexData->mudprogs = dest_mprg;
	     mpcopy( source_mprg, dest_mprg );
             xSET_BIT(destination->pIndexData->progtypes, dest_mprg->type);
             ch_printf(ch, "%s program %d from %s successfully copied to %s.\n\r", 
             prog, value, smob, dmob); 
             return;
         }    
     }

     if (!source_mprg)
     {
         send_to_char( "No such program in source mobile.\n\r", ch );
         return;
      }
      do_mpcopy( ch, "" );
}
 


/*
 * function to allow modification of an area's climate
 * Last modified: July 15, 1997
 * Fireblade
 */
void do_climate(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	AREA_DATA *area;
	
	/* Little error checking */
	if(!ch)
	{
		bug("do_climate: NULL character.");
		return;
	}
	else if(!ch->in_room)
	{
		bug("do_climate: character not in a room.");
		return;
	}
	else if(!ch->in_room->area)
	{
		bug("do_climate: character not in an area.");
		return;
	}
	else if(!ch->in_room->area->weather)
	{
		bug("do_climate: area with NULL weather data.");
		return;
	}
	
	set_char_color(AT_BLUE, ch);
	
	area = ch->in_room->area;
	
	argument = strlower(argument);
	argument = one_argument(argument, arg);
	
	/* Display current climate settings */
	if(arg[0] == '\0')
	{
		NEIGHBOR_DATA *neigh;
		
		ch_printf(ch, "%s:\n\r", area->name);
		ch_printf(ch, "\tTemperature:\t%s\n\r",
			temp_settings[area->weather->climate_temp]);
		ch_printf(ch, "\tPrecipitation:\t%s\n\r",
			precip_settings[area->weather->climate_precip]);
		ch_printf(ch, "\tWind:\t\t%s\n\r",
			wind_settings[area->weather->climate_wind]);
		
		if(area->weather->first_neighbor)
			ch_printf(ch, "\n\rNeighboring weather systems:\n\r");
		
		for(neigh = area->weather->first_neighbor; neigh;
				neigh = neigh->next)
		{
			ch_printf(ch, "\t%s\n\r", neigh->name);
		}
		
		return;
	}
	/* set climate temperature */
	else if(!str_cmp(arg, "temp"))
	{
		int i;
		argument = one_argument(argument, arg);
		
		for(i = 0; i < MAX_CLIMATE; i++)
		{
			if(str_cmp(arg, temp_settings[i]))
				continue;
			
			area->weather->climate_temp = i;
			ch_printf(ch, "The climate temperature "
				"for %s is now %s.\n\r", area->name,
				temp_settings[i]);
			break;
		}
		
		if(i == MAX_CLIMATE)
		{
			ch_printf(ch, "Possible temperature "
				"settings:\n\r");
			for(i = 0; i < MAX_CLIMATE; i++)
			{
				ch_printf(ch,"\t%s\n\r",
					temp_settings[i]);
			}
		}
		
		return;
	}
	/* set climate precipitation */
	else if(!str_cmp(arg, "precip"))
	{
		int i;
		argument = one_argument(argument, arg);
		
		for(i = 0; i < MAX_CLIMATE; i++)
		{
			if(str_cmp(arg, precip_settings[i]))
				continue;
			
			area->weather->climate_precip = i;
			ch_printf(ch, "The climate precipitation "
				"for %s is now %s.\n\r", area->name,
				precip_settings[i]);
			break;
		}
		
		if(i == MAX_CLIMATE)
		{
			ch_printf(ch, "Possible precipitation "
				"settings:\n\r");
			for(i = 0; i < MAX_CLIMATE; i++)
			{
				ch_printf(ch, "\t%s\n\r",
					precip_settings[i]);
			}
		}
		
		return;
	}
	/* set climate wind */
	else if(!str_cmp(arg, "wind"))
	{
		int i;
		argument = one_argument(argument, arg);
		
		for(i = 0; i < MAX_CLIMATE; i++)
		{
			if(str_cmp(arg, wind_settings[i]))
				continue;
			
			area->weather->climate_wind = i;
			ch_printf(ch, "The climate wind for %s "
				"is now %s.\n\r", area->name,
				wind_settings[i]);
			break;
		}
		
		if(i == MAX_CLIMATE)
		{
			ch_printf(ch, "Possible wind settings:\n\r");
			for(i = 0; i < MAX_CLIMATE; i++)
			{
				ch_printf(ch, "\t%s\n\r",
					wind_settings[i]);
			}
		}
		
		return;
	}
	/* add or remove neighboring weather systems */
	else if(!str_cmp(arg, "neighbor"))
	{
		NEIGHBOR_DATA *neigh;
		AREA_DATA *tarea;
		
		if(argument[0] == '\0')
		{
			ch_printf(ch, "Add or remove which area?\n\r");
			return;
		}

		/* look for a matching list item */
		for(neigh = area->weather->first_neighbor; neigh;
				neigh = neigh->next)
		{
			if(nifty_is_name(argument, neigh->name))
				break;
		}
		
		/* if the a matching list entry is found, remove it */
		if(neigh)
		{
			/* look for the neighbor area in question */
			if(! (tarea = neigh->address) )
				tarea = get_area(neigh->name);

			/* if there is an actual neighbor area */
			/* remove its entry to this area */
			if(tarea)
			{
				NEIGHBOR_DATA *tneigh;
				
				tarea = neigh->address;
				for(tneigh = tarea->weather->first_neighbor;
						tneigh; tneigh = tneigh->next)
				{
					if(!strcmp(area->name, tneigh->name))
						break;
				}
				
				UNLINK(tneigh,
				       tarea->weather->first_neighbor,
				       tarea->weather->last_neighbor,
				       next,
				       prev);
				STRFREE(tneigh->name);
				DISPOSE(tneigh);
			}
			
			UNLINK(neigh,
			       area->weather->first_neighbor,
			       area->weather->last_neighbor,
			       next,
			       prev);
			ch_printf(ch,"The weather in %s and %s "
				"no longer affect each other.\n\r",
				neigh->name, area->name);
			STRFREE(neigh->name);
			DISPOSE(neigh);
		}
		/* otherwise add an entry */		
		else
		{
			tarea = get_area(argument);
		
			if(!tarea)
			{
				ch_printf(ch, "No such area exists.\n\r");
				return;
			}
			else if(tarea == area)
			{
				ch_printf(ch, "%s already affects its "
					"own weather.\n\r", area->name);
				return;
			}
			
			/* add the entry */
			CREATE(neigh, NEIGHBOR_DATA, 1);
			neigh->name = STRALLOC(tarea->name);
			neigh->address = tarea;
			LINK(neigh,
			     area->weather->first_neighbor,
			     area->weather->last_neighbor,
			     next,
			     prev);
			
			/* add an entry to the neighbor's list */
			CREATE(neigh, NEIGHBOR_DATA, 1);
			neigh->name = STRALLOC(area->name);
			neigh->address = area;
			LINK(neigh,
			     tarea->weather->first_neighbor,
			     tarea->weather->last_neighbor,
			     next,
			     prev);
			
			ch_printf(ch, "The weather in %s and %s now "
				"affect one another.\n\r", tarea->name,
				area->name);
		}
		
		return;
	}
	else
	{
		ch_printf(ch, "Climate may only be followed by one "
			"of the following fields:\n\r");
		ch_printf(ch, "\ttemp\n\r");
		ch_printf(ch, "\tprecip\n\r");
		ch_printf(ch, "\twind\n\r");
		ch_printf(ch, "\tneighbor\n\r");
		
		return;
	}
}


/*
 * Relations created to fix a crash bug with oset on and rset on
 * code by: gfinello@mail.karmanet.it
 */
void RelCreate( relation_type tp, void *actor, void *subject ) {
   REL_DATA *tmp;
  
   if ( tp < relMSET_ON || tp > relOSET_ON ) {
      bug( "RelCreate: invalid type (%d)", tp );
      return;
      }
   if ( !actor ) {
      bug( "RelCreate: NULL actor" );
      return;
      }
   if ( !subject ) {
      bug( "RelCreate: NULL subject" );
      return;
      }
   for ( tmp = first_relation; tmp; tmp = tmp->next )
      if ( tmp->Type == tp && tmp->Actor == actor && tmp->Subject ==
           subject ) {
         bug( "RelCreate: duplicated relation" );
         return;
         }
   CREATE( tmp, REL_DATA, 1 );
   tmp->Type = tp;
   tmp->Actor = actor;
   tmp->Subject = subject;
   LINK( tmp, first_relation, last_relation, next, prev );
}


/*
 * Relations created to fix a crash bug with oset on and rset on
 * code by: gfinello@mail.karmanet.it
 */
void RelDestroy( relation_type tp, void *actor, void *subject ) {
   REL_DATA * rq;
  
   if ( tp < relMSET_ON || tp > relOSET_ON ) {
      bug( "RelDestroy: invalid type (%d)", tp );
      return;
      }
   if ( !actor ) {
      bug( "RelDestroy: NULL actor" );
      return;
      }
   if ( !subject ) {
      bug( "RelDestroy: NULL subject" );
      return;
      }
   for ( rq = first_relation; rq; rq = rq->next )
      if ( rq->Type == tp && rq->Actor == actor && rq->Subject == subject ) {
         UNLINK(  rq, first_relation, last_relation, next, prev );
         /* Dispose will also set to NULL the passed parameter */
         DISPOSE( rq );
         break;
         }
}


void do_cleanroom( CHAR_DATA *ch, char *argument )
{
 ROOM_INDEX_DATA *location;
/* CHAR_DATA *vnext;
 CHAR_DATA *victim;
 OBJ_DATA *obj;
 OBJ_DATA *obj_next;*/
 EXIT_DATA *xit;
 int exitnum;

 location = ch->in_room;

    if ( !ch->desc )
    {
        send_to_char( "You have no descriptor.\n\r", ch );
        return;
    }

    if ( !can_rmodify( ch, location ) )
      return;

/*        for ( victim = ch->in_room->first_person; victim; victim = vnext )
        {
            vnext = victim->next_in_room;
            if ( IS_NPC(victim) && victim != ch && !IS_SET(victim->act, ACT_POLYMORPHED))
                extract_char( victim, TRUE );
        }*/

	for ( exitnum = 0 ; exitnum < 11 ; exitnum++ ) 
	{
          xit = get_exit(location, exitnum);
	  if(xit != NULL)
	    extract_exit(location, xit);
	}
	location->first_extradesc	= NULL;
	location->last_extradesc	= NULL;
	location->name		= STRALLOC("Cleaned Room");
	location->description		= STRALLOC("");
	location->room_flags		= ROOM_PROTOTYPE;
	location->sector_type		= 1;
	location->light		= 0;

      ch_printf(ch, "&WRoom Cleared.\n\r");
      return;
}
/*
void do_makefree( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	AREA_DATA *pArea;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *wasinroom;
    int Start, End, vnum;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    Start = atoi( arg1 );
    End = atoi( arg2 );

    wasinroom = ch->in_room;

    if ( arg1 == '\0' || arg2 == '\0')
    {
	send_to_char( "Syntax: makefree <starting vnum> <ending vnum>\n\r", ch );
	return;
    }

    if ( Start < 1 || End < Start || Start > End || Start == End || End > 1276700000 )
    {
	send_to_char( "Invalid range.\n\r", ch );
	return;
    }

	if ( !ch->pcdata || !(pArea=ch->pcdata->area) )
	{
	  send_to_char( "You must have an assigned area to create rooms.\n\r", ch );
	  return;
	}
	if ( Start < pArea->low_r_vnum
	||   End > pArea->hi_r_vnum )
	{
	  send_to_char( "You cannot create that room because it is not within your assigned vnum range.\n\r", ch);
	  return;
	}

    for ( vnum = Start; vnum <= End; vnum++ )
    {
	 if ( (location = get_room_index(vnum)) != NULL )
	  continue;

	 make_room( vnum );
         char_from_room( ch );
         char_to_room( ch, location );
    }
    char_from_room( ch );
    char_to_room( ch, wasinroom );

    send_to_char( "Done.\n\r", ch );
    return;
}*/

void do_makefree( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*location;
    AREA_DATA           *pArea;
    int			vnum, x;
    int                 room_count;
    int                 room_hold[MAX_RGRID_ROOMS];

    pArea = ch->pcdata->area;

    if (!pArea)
    {
	send_to_char( "You must have an area assigned to do this.\n\r", ch );
	return;
    }

    if ( !argument || argument[0] == '\0' )
      {
	send_to_char( "Create a block of rooms.\n\r", ch );
	send_to_char( "Usage: makerooms <# of rooms>\n\r", ch );
	return;
      }
    x = atoi(argument);


    ch_printf(ch, "Attempting to create a block of %d rooms.\r\n", x);

    if(x > 300) 
    {
      ch_printf(ch, "The maximum number of rooms this mud can create at once is 300.\r\n");
      return;
    }

    room_count = 0;

    if(pArea->low_r_vnum + x > pArea->hi_r_vnum) 
    {

      send_to_char( "You don't even have that many rooms assigned to you.\r\n", ch);
      return;
    }

    for(vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++) 
    {

      if(get_room_index(vnum) == NULL)
	room_count++;

      if(room_count >= x)
	break;
    }

    if(room_count < x) 
    {
      send_to_char( "There aren't enough free rooms in your assigned range!\r\n", ch);
      return;
    }

    send_to_char( "Creating the rooms...\r\n", ch);

    room_count = 0;
    vnum = pArea->low_r_vnum;

    while(room_count < x) 
    {

      if(get_room_index(vnum) == NULL) 
      {
	room_hold[room_count++] = vnum;
	location = make_room( vnum, ch->pcdata->area );
	if ( !location ) 
        {
	  bug( "%s: make_room failed", __FUNCTION__ );
	  return;
	}
	location->area = ch->pcdata->area;
      }
      vnum++;
    }

    ch_printf( ch, "%d rooms created.\r\n", room_count);

    return;

}

bool check_guardian ( AREA_DATA *pArea )
{
   CHAR_DATA *victim;
   bool guard1, guard2, guard3, guard4; 

   guard1 = guard2 = guard3 = guard4 =FALSE;

   if (!pArea)
   return FALSE;
   if ( !pArea->guard1 || pArea->guard1 == 0 )
   guard1 = FALSE;
   if ( !pArea->guard2 || pArea->guard2 == 0 )
   guard2 = FALSE;
   if ( !pArea->guard3 || pArea->guard3 == 0 )
   guard3 = FALSE;
   if ( !pArea->guard4 || pArea->guard4 == 0 )
   guard4 = FALSE;

   for ( victim = first_char; victim; victim = victim->next )
   {
        if (!victim)
        break;
        if (!IS_NPC(victim))
        continue;
        if (IS_NPC(victim))
        {
          if (guard1)
          {
             if (pArea->guard1 == victim->pIndexData->vnum)
             return TRUE;
          }
          if (guard2)
          {
             if (pArea->guard2 == victim->pIndexData->vnum)
             return TRUE;
          }
          if (guard3)
          {
             if (pArea->guard3 == victim->pIndexData->vnum)
             return TRUE;
          }
          if (guard4)
          {
             if (pArea->guard4 == victim->pIndexData->vnum)
             return TRUE;
          }
        }          
   }


  return FALSE;
}

void do_propaganda ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AREA_DATA *planet;
    CLAN_DATA   *clan;
    CLAN_DATA   *isclan;

    argument = one_argument( argument, arg1 );

    if (!ch->pcdata->clan || !ch->pcdata)
    return;

    clan = ch->pcdata->clan;
    planet = ch->in_room->area;
    isclan = get_clan_short(planet->governed_by);    
 switch( ch->substate )
 { 
  default:
   if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area )
   {
       send_to_char( "What would be the point of that.\n\r", ch );
       return;
   }
    

    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Spread propeganda to who?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !IS_NPC(victim))
    {
	send_to_char( "What would be the point of that?\n\r", ch );
	return;
    }
    if ( victim->pIndexData->propangandized > 5 )
    {
	send_to_char( "They are already set of mind.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }
    if ( !IS_SET(ch->in_room->area->flags, AFLAG_PLANET ))
    {
        send_to_char( "You can do this only on planets.\n\r", ch);
        return;
    }
    if ( !xIS_SET(victim->act, ACT_CITIZEN))
    {
        send_to_char( "They don't care about politics.\n\r", ch);
        return;
    }
    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Interesting combat technique.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "They're a little busy right now.\n\r" , ch );
        return;
    }
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "In your dreams or what?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "You might want to wake them first...\n\r" , ch );
        return;
    }


    sprintf( buf, ", and the evils of their current leaders" );
    ch_printf( ch, "\n\rYou speak to them about the benifits of the %s%s.\n\r", ch->pcdata->clan->name, isclan == clan ? "" : buf );
    act( AT_ACTION, "$n speaks about his organization.", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n tells $N about their organization.",  ch, NULL, victim, TO_NOTVICT );
    add_timer ( ch , TIMER_DO_FUN , 2, do_propaganda , 1 );
    ch->dest_buf = str_dup(arg1);
    victim->pIndexData->propangandized += 1;

    return;
    case 1: 
    	if ( !ch->dest_buf )
    	     return;
    	strcpy(arg1, ch->dest_buf);
    	DISPOSE( ch->dest_buf);
    	break;
    	
    case SUB_TIMER_DO_ABORT:
    	DISPOSE( ch->dest_buf );
    	ch->substate = SUB_NONE;    		                                   
            send_to_char("\n\r&RYou are interupted and fail to finish advertising your organization.\n\r", ch);
            return;

   }
    WAIT_STATE( ch, 1 );
    if ( isclan == clan )
    { 
       planet->popsupport += number_range( 2, get_curr_cha(ch)/4 );
       send_to_char( "Popular support for your organization increases.\n\r", ch );
    }     
    else
    {
       planet->popsupport -=  number_range( 2, get_curr_cha(ch)/4 );
       send_to_char( "Popular support for the current organization decreases.\n\r", ch );
    }
        
    if ( planet->popsupport > 100 )
        planet->popsupport = 100;
    if ( planet->popsupport < -100 )
        planet->popsupport = -100;
     fold_area( ch->in_room->area, ch->in_room->area->filename, FALSE, FALSE );
     add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
}


void  do_planets( CHAR_DATA *ch, char *argument )
{
   AREA_DATA *pArea;
   char buf[MSL];
   CLAN_DATA *clan;
   FILE *www = NULL;
   if ( !ch && !str_cmp( argument, "www") )
   {
       unlink( WEBPLANETS_FILE );
       www = fopen( WEBPLANETS_FILE, "w" );
       if ( !www )
       {
            bug( "do_clans: cannot open clans web file!" );
            return;
       }
       fprintf( www, htmlcolor("\n&cPlanet                 PopSupport    Income          Organization") );
       fprintf( www, htmlcolor("\n&z==========================================================================\n" ));
   }
   else
   {
     send_to_char( "\n\r&cPlanet                 PopSupport    Income          Organization", ch);
     send_to_char( "\n\r&z==========================================================================\n\r", ch);
   }
   for ( pArea = first_area; pArea; pArea = pArea->next )
   {
       if (!IS_SET(pArea->flags, AFLAG_PLANET ))
       continue;
       clan = get_clan_short( pArea->governed_by );


       if (!ch && !str_cmp( argument, "www" ))
       {
        sprintf(buf,"&G%-17s      &W%-3d&z          &O$&Y%-10d&C     %s\n", pArea->name, pArea->popsupport,pArea->income, clan ? clan->name : "None");
         fprintf( www, htmlcolor(buf) );
       }
       else
       {
         sprintf(buf,"&G%-17s      &W%-3d&z%%          &O$&Y%-10d&C     %s\n", pArea->name, pArea->popsupport,pArea->income, clan ? clan->name : "None");
         send_to_char( buf, ch );
       }
       if (pArea == last_area)
       break;
   }
   if ( !ch && !str_cmp( argument, "www" ))
   {
     fprintf( www, htmlcolor("&z==========================================================================\n"));
     fclose( www );
   }
   else
     send_to_char( "&z==========================================================================\n\r", ch);
   return; 
}

void do_capture( CHAR_DATA *ch, char *argument )
{
  AREA_DATA *planet;
  AREA_DATA *pArea;
  CLAN_DATA *clan, *clan2;
  
  int pCount = 0;
  int support = 0;
  char buf[MSL];
   if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area )
   {
       send_to_char( "What would be the point of that.\n\r", ch );
       return;
   }
   clan = ch->pcdata->clan;
   if ( !IS_SET(ch->in_room->area->flags, AFLAG_PLANET ))
   {
        send_to_char( "You can do this only on planets.\n\r", ch);
        return;
   }
   planet = ch->in_room->area;
   clan2 = get_clan_short( planet->governed_by );    
   if ( clan2 == clan )
   {
        send_to_char( "Your organization already owns this planet.\n\r", ch);
        return;
   }
   if ( planet->popsupport > -75 )
   {
       send_to_char ( "The population is not in favour of changing leaders right now.\n\r" , ch );
       return;
   }
   for ( pArea = first_area; pArea; pArea = pArea->next)
      if (IS_SET(ch->in_room->area->flags, AFLAG_PLANET))
        if ( clan == clan2 )
        {
            pCount++;
            support += pArea->popsupport;
 	    if ( support < 0 )
   	    {
        	send_to_char ( "There is not enough popular support for your organization!\n\rTry improving loyalty on the planets that you already control.\n\r" , ch );
	        return;
   	    }
        }
 
 
   if ( support < 0 )
   {
        send_to_char ( "There is not enough popular support for your organization!\n\rTry improving loyalty on the planets that you already control.\n\r" , ch );
       return;
   }
   planet->governed_by = str_dup( clan->shortname );
   planet->popsupport = 50 + (get_curr_cha(ch)*2);
   sprintf( buf , "&R%s has been captured by %s!", planet->name, clan->name );   
   infochan(buf);
   fold_area( ch->in_room->area, ch->in_room->area->filename, FALSE, FALSE );
}
