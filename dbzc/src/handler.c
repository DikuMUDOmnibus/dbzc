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
 *		        Main structure manipulation module		    *
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
#include "mud.h"


extern int		top_exit;
extern int		top_ed;
extern int		top_affect;
extern int		cur_qobjs;
extern int		cur_qchars;
extern CHAR_DATA *	gch_prev;
extern OBJ_DATA  *	gobj_prev;
extern REL_DATA  *      first_relation;
extern REL_DATA  *      last_relation;


CHAR_DATA	*cur_char;
ROOM_INDEX_DATA	*cur_room;
bool		 cur_char_died;
ch_ret		 global_retcode;

int		 cur_obj;
int		 cur_obj_serial;
bool		 cur_obj_extracted;
obj_ret		 global_objcode;

OBJ_DATA *group_object( OBJ_DATA *obj1, OBJ_DATA *obj2 );
bool in_magic_container( OBJ_DATA *obj );
void delete_reset( RESET_DATA *pReset );


/*
 * Return how much exp a char has
 */
int get_exp( CHAR_DATA *ch )
{
    return ch->exp;
}
int clan_name( CLAN_DATA *clan )
{

  if ( !str_cmp( clan->name, "Z Fighters") )
  return 1;
  if ( !str_cmp( clan->name, "Cobra Sect") )
  return 2;
  if ( !str_cmp( clan->name, "Majin Dynasty") )
  return 3;

 return 0;
}
char *clan_get( int clan )
{
  
  switch( clan )
  {
    default:
       return "None";
       break;
    case 1:
       return "Z Fighters";
       break;
    case 2:
       return "Cobra Sect";
       break;
    case 3:
       return "Majin Dynasty";
       break;

  }
  
 return "None";
}
char *get_transformation( CHAR_DATA *ch )
{
    char buf[MSL];
    char *sent;
    sprintf(buf, "  %s", race_table[ch->race]->race_name); 
    if (xIS_SET(ch->powerup, PU_SSJ1) || xIS_SET(ch->powerup, PU_SSJ2) || xIS_SET(ch->powerup, PU_SSJ3) || xIS_SET(ch->powerup, PU_SSJ4) )
    return "  &YS&Ouper &YS&Oaiyan ";
    else if (xIS_SET(ch->powerup, PU_MYSTIC))
    return "  &CM&zy&Wst&zi&Cc       ";
    else if (xIS_SET(ch->powerup, PU_KAIOKEN))
    return "  &WK&ra&Riok&re&Wn      ";
    else if (xIS_SET(ch->powerup, PU_USSJ) )
    return "  &rU&Rlt&Yr&Oa&Y S&RS&rJ    ";
    else if (xIS_SET(ch->powerup, PU_OOZARU) )
    return "  &YO&O&wo&Wz&wa&Or&Yu       ";
    else if (xIS_SET(ch->powerup, PU_SUPERHUMAN))
    return "  &WS&bu&Bpe&Cr H&Bum&ba&Wn  ";
    else if (xIS_SET(ch->powerup, PU_ULTRA) )
    return "  &wU&Gl&gt&cr&za N&ca&gm&Ge&wk  ";
    else if (xIS_SET(ch->powerup, PU_SNAMEK))
    return "  &wS&Wu&Gp&ge&cr N&ga&Gm&We&wk  ";
    else if (xIS_SET(ch->powerup, PU_ADULT))
    return "  &zA&rd&Ru&rl&zt       ";
    else if (xIS_SET(ch->powerup, PU_SEMIP))
    return "  &zS&We&rmi&R-Per&rfe&Wc&zt ";
    else if (xIS_SET(ch->powerup, PU_PERFECT))
    return "  &zP&rer&Rf&rec&zt      ";
    else if (xIS_SET(ch->powerup, PU_MONSTER))
    return "  &zM&po&On&Ys&Ot&pe&zr      ";
    else if (xIS_SET(ch->powerup, PU_TURBO))
    return "  &bT&Bu&Cr&Bb&bo        ";
    else if (xIS_SET(ch->powerup, PU_MUTATE2) || xIS_SET(ch->powerup, PU_MUTATE3) || xIS_SET(ch->powerup, PU_MUTATE4))
    return "  &pM&Ou&Ytat&Oe&pd      ";
    else if (xIS_SET(ch->powerup, PU_TRANSFORM2) || xIS_SET(ch->powerup, PU_TRANSFORM3) || xIS_SET(ch->powerup, PU_TRANSFORM4) || xIS_SET(ch->powerup, PU_TRANSFORM5) || xIS_SET(ch->powerup, PU_METAL))
    return "  &bTr&Ban&Csfo&Brm&bed  ";
    else
    {
      sent = buf;
      return sent;
    }
    sent = buf;
 return sent;
}

bool check_weightabout( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_ABOUT ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 120 )
    return TRUE;
  return FALSE;
}
bool check_weightwristleft( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_WRIST_L ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 121 )
    return TRUE;
  return FALSE;
}
bool check_weightwristright( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_WRIST_R ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 121 )
    return TRUE;
  return FALSE;
}
bool check_weightfeet( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_FEET ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 122 )
    return TRUE;
  return FALSE;
}
bool check_weightlegs( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_LEGS ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 123 )
    return TRUE;
  return FALSE;
}
bool check_weighthead( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, WEAR_HEAD ) ) == NULL )
    return FALSE;
    if ( obj->pIndexData->vnum == 124 )
    return TRUE;
  return FALSE;
}

/*
 * Calculate roughly how much experience a character is worth
 */
int get_exp_worth( CHAR_DATA *ch )
{
    int exp;

    exp = ch->level * ch->level * ch->level * 5;
    exp += ch->max_hit;
    exp -= (ch->armory-50) * 2;
    exp += ( ch->barenumdie * ch->baresizedie + GET_DAMROLL(ch) ) * 50;
    exp += GET_HITROLL(ch) * ch->level * 10;
    exp = URANGE( MIN_EXP_WORTH, exp, MAX_EXP_WORTH );

    return exp;
}

sh_int get_exp_base( CHAR_DATA *ch )
{
    if ( IS_NPC(ch) )
      return 1000;
    return class_table[ch->class]->exp_base;
}

/*								-Thoric
 * Return how much experience is required for ch to get to a certain level
 */
int exp_level( CHAR_DATA *ch, sh_int level )
{
   int lvl;

   lvl = UMAX(0, level - 1);
   return ( lvl * lvl * lvl * get_exp_base(ch) );
}

/*
 * Get what level ch is based on exp
 */
sh_int level_exp( CHAR_DATA *ch, int exp )
{
    int x, lastx, y, tmp;

    x = LEVEL_SUPREME;
    lastx = x;
    y = 0;
    while ( !y )
    {
	tmp = exp_level(ch, x);
	lastx = x;
	if ( tmp > exp )
	  x /= 2;
	else
	if (lastx != x )
	  x += ( x / 2 );
	else
	  y = x;
    }
    if ( y < 1 )
      y = 1;
    if ( y > LEVEL_SUPREME )
      y = LEVEL_SUPREME;
    return y;
}

/*
 * Retrieve a character's trusted level for permission checking.
 */
sh_int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    if ( ch->trust != 0 )
	return ch->trust;

    if ( IS_NPC(ch) && ch->level >= LEVEL_AVATAR )
	return LEVEL_AVATAR;

    if ( ch->level >= LEVEL_NEOPHYTE && IS_RETIRED( ch ) )
      return LEVEL_NEOPHYTE;

    return ch->level;
}


/*
 * Retrieve a character's age.
 */
sh_int get_age( CHAR_DATA *ch )
{
    return 4 + ( ch->played + (current_time - ch->logon) ) / 7200;
}



/*
 * Retrieve character's current strength.
 */
sh_int get_curr_str( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_str + ch->mod_str, max );
}



/*
 * Retrieve character's current intelligence.
 */
sh_int get_curr_int( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_int + ch->mod_int, max );
}



/*
 * Retrieve character's current wisdom.
 */
sh_int get_curr_wis( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_wis + ch->mod_wis, max );
}



/*
 * Retrieve character's current dexterity.
 */
sh_int get_curr_dex( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_dex + ch->mod_dex, max );
}



/*
 * Retrieve character's current constitution.
 */
sh_int get_curr_con( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_con + ch->mod_con, max );
}

/*
 * Retrieve character's current charisma.
 */
sh_int get_curr_cha( CHAR_DATA *ch )
{
    sh_int max;
    sh_int cha;
	max = 40;
    if ( IS_NPC(ch))
    cha = 5;
    else
    cha = ch->perm_cha + ch->mod_cha;
    
    return URANGE( 3, cha, max );
}

sh_int get_curr_armor( CHAR_DATA *ch )
{
    return URANGE( -1000, ch->armory, 1000 );
}
char *get_name_color( CHAR_DATA *ch )
{
    switch( ch->in_room->sector_type )
    {
        case SECT_INSIDE:
              return "&Y";
        case SECT_CITY:
              return "&B";
        case SECT_FIELD:
              return "&Y";
        case SECT_FOREST:
	      return "&Y";
	case SECT_HILLS:
	      return "&G";
        case SECT_MOUNTAIN:
	      return "&z";
	case SECT_UNDERWATER:
	      return "&C";
	case SECT_AIR:
	      return "&W";
	case SECT_DESERT:
	      return "&Y";
	case SECT_OCEANFLOOR:
	      return "&B";
	case SECT_UNDERGROUND:
	      return "&z";
	case SECT_LAVA:
	      return "&R";
	case SECT_SWAMP:
	      return "&O";
        case SECT_ICE:
              return "&C";
       
    	default: return "&G";
    }
}

char *get_desc_color( CHAR_DATA *ch )
{
    switch( ch->in_room->sector_type )
    {
        case SECT_INSIDE:
              return "&c";
        case SECT_CITY:
              return "&b";
        case SECT_FIELD:
              return "&G";
        case SECT_FOREST:
              return "&g";
        case SECT_HILLS:
              return "&O";
        case SECT_MOUNTAIN:
              return "&r";
        case SECT_UNDERWATER:
              return "&B";
        case SECT_AIR:
              return "&w";
        case SECT_DESERT:
              return "&O";
        case SECT_OCEANFLOOR:
              return "&O";
        case SECT_UNDERGROUND:
              return "&O";
        case SECT_LAVA:
              return "&r";
        case SECT_SWAMP:
              return "&z";
        case SECT_ICE:
              return "&B";
        default: return "&g";
    }
}

void get_curr_home( SHIP_DATA *ship )
{
  DOCK_DATA *dock;
    for ( dock = first_dock; dock; dock = dock->next )
    {
       if( dock->room == ship->home )
       {
         ship->x1 = ship->x2 = dock->x;
         ship->y1 = ship->y2 = dock->y;
         ship->z1 = ship->z2 = dock->z;
         ship->d1 = ship->d2 = dock->d;
       }
       else
       continue;
    } 
 return;
}
long long int get_curr_pl2( CHAR_DATA *ch )
{
   long long int pl;
   long long int max;

   pl = 0;

   if (IS_NPC(ch))
   pl = ch->exp;
   else if (!IS_NPC(ch) && !xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->exp * ch->mod);
   else if (!IS_NPC(ch) && xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->suppress);

   max = 2000100000;

   return URANGE( 1, pl, max );
}

long long int get_curr_pl( CHAR_DATA *ch )
{
   long long int pl;
   long long int max;

   pl = 0;

   if (IS_NPC(ch))
   pl = ch->exp + 100000;
   else if (!IS_NPC(ch) && !xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->exp * ch->mod) + 100000;
   else if (!IS_NPC(ch) && xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->suppress) + 100000;

   max = 1850000000;
 
   return URANGE( 1, pl, max );
}

double get_curr_pl3( CHAR_DATA *ch )
{
   long long int pl;
   long long int max;

   pl = 0;

   if (IS_NPC(ch))
   pl = ch->exp;
   else if (!IS_NPC(ch) && xIS_SET(ch->powerup, KI_BAR))
   pl = ch->barrier;
   else if (!IS_NPC(ch) && !xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->exp * ch->mod);
   else if (!IS_NPC(ch) && xIS_SET(ch->powerup, PU_SUPPRESS))
   pl = (ch->suppress);
   
   max = 1850000000;

   return URANGE( 1, pl, max );
}


long long int get_base_pl( CHAR_DATA *ch )
{
   long long int pl;
   long long int max;

   pl = ch->exp + 100000;

   max = 125000000;

   return URANGE( 1, pl, max );
}
long long int get_supp_pl( CHAR_DATA *ch )
{
   long long int pl;
   long long int max;

   pl = ch->suppress + 100000;

   max = 12500000;

   return URANGE( 1, pl, max );
}



/*
 * Retrieve character's current luck.
 */
sh_int get_curr_lck( CHAR_DATA *ch )
{
    sh_int max;

	max = 40;

    return URANGE( 3, ch->perm_lck + ch->mod_lck, max );
}


/*
 * Retrieve a character's carry capacity.
 * Vastly reduced (finally) due to containers		-Thoric
 */
int can_carry_n( CHAR_DATA *ch )
{
  int value;

  value = (sqrt(race_mod_table[ch->race].weight) / 2 ) * (get_curr_str(ch) / 2 );


 return URANGE( 20, value, 300 );
}





/*
 * See if a player/mob can take a piece of prototype eq		-Thoric
 */
bool can_take_proto( CHAR_DATA *ch )
{
    if ( IS_IMMORTAL(ch) )
	return TRUE;
    else
    if ( IS_NPC(ch) && xIS_SET(ch->act, ACT_PROTOTYPE) )
	return TRUE;
    else
	return FALSE;
}


/*
 * See if a string is one of the names of an object.
 */
bool is_name( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}

bool is_name_prefix( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_prefix( str, name ) )
	    return TRUE;
    }
}

/*
 * See if a string is one of the names of an object.		-Thoric
 * Treats a dash as a word delimiter as well as a space
 */
bool is_name2( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument2( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}

bool is_name2_prefix( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument2( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_prefix( str, name ) )
	    return TRUE;
    }
}

/*								-Thoric
 * Checks if str is a name in namelist supporting multiple keywords
 */
bool nifty_is_name( char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];
    
    if ( !str || str[0] == '\0' )
      return FALSE;
 
    for ( ; ; )
    {
	str = one_argument2( str, name );
	if ( name[0] == '\0' )
	    return TRUE;
	if ( !is_name2( name, namelist ) )
	    return FALSE;
    }
}

bool nifty_is_name_prefix( char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];
    
    if ( !str || str[0] == '\0' )
      return FALSE;
 
    for ( ; ; )
    {
	str = one_argument2( str, name );
	if ( name[0] == '\0' )
	    return TRUE;
	if ( !is_name2_prefix( name, namelist ) )
	    return FALSE;
    }
}





/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( !ch->in_room )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if ( !IS_NPC(ch) )
	--ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room->light > 0 )
	--ch->in_room->light;

    if ( !char_died(ch) )
    {
	if ( char_died(ch) )	/* could die from removespell, etc */
	    return;
    }

    UNLINK( ch, ch->in_room->first_person, ch->in_room->last_person,
		next_in_room, prev_in_room );
    ch->was_in_room  = ch->in_room;
    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    ch->prev_in_room = NULL;

    if ( !IS_NPC(ch)
    &&   get_timer( ch, TIMER_SHOVEDRAG ) > 0 )
	remove_timer( ch, TIMER_SHOVEDRAG );

    return;
}


/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;
/*    OBJ_DATA *dragonball;
    OBJ_DATA *obj_next;*/
    if ( !ch )
    {
	bug( "Char_to_room: NULL ch!", 0 );
	return;
    }
    if ( !pRoomIndex )
    {
	bug( "Char_to_room: %s -> NULL room!  Putting char in limbo (%d)",
		ch->name, ROOM_VNUM_LIMBO );
        /*
         * This used to just return, but there was a problem with crashing
         * and I saw no reason not to just put the char in limbo.  -Narn
         */
        pRoomIndex = get_room_index( ROOM_VNUM_LIMBO );
    }
/*    for ( dragonball = ch->first_carrying; dragonball; dragonball = obj_next )
    {
       obj_next = dragonball->next_content;

      if (dragonball->item_type == ITEM_DRAGONBALL && !IS_IMMORTAL(ch) && IS_SET(pRoomIndex->room_flags2, ROOM_CONTROL))
      {
        send_to_char("&RYou can't enter having a dragonball at hand.\n\r", ch);
        return;
      }
    }
*/
    ch->in_room		= pRoomIndex;
    if( ch->home_vnum < 1 )
       ch->home_vnum = ch->in_room->vnum;

    LINK( ch, pRoomIndex->first_person, pRoomIndex->last_person,
    	      next_in_room, prev_in_room );

    if ( !IS_NPC(ch) )
	if ( ++pRoomIndex->area->nplayer > pRoomIndex->area->max_players )
	    pRoomIndex->area->max_players = pRoomIndex->area->nplayer;

    if ( (obj=get_eq_char(ch, WEAR_LIGHT)) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0 )
	++pRoomIndex->light;

    /*
     * Room's effect on the character
     */
    if ( !char_died(ch) )
    {
	if ( char_died(ch) )	/* could die from a wearspell, etc */
	    return;
    }

    if ( !IS_NPC(ch)
    &&    IS_SET(pRoomIndex->room_flags, ROOM_SAFE)
    &&    get_timer(ch, TIMER_SHOVEDRAG) <= 0 )
	add_timer(ch, TIMER_SHOVEDRAG, 10, NULL, 0);  /*-30 Seconds-*/

    /*
     * Delayed Teleport rooms					-Thoric
     * Should be the last thing checked in this function
     */
    if ( IS_SET(pRoomIndex->room_flags, ROOM_TELEPORT)
    &&	 pRoomIndex->tele_delay > 0 )
    {
	TELEPORT_DATA *tele;

	for ( tele = first_teleport; tele; tele = tele->next )
	    if ( tele->room == pRoomIndex )
		return;

	CREATE( tele, TELEPORT_DATA, 1 );
	LINK( tele, first_teleport, last_teleport, next, prev );
	tele->room		= pRoomIndex;
	tele->timer		= pRoomIndex->tele_delay;
    }
    if ( !ch->was_in_room )
    	ch->was_in_room = ch->in_room;
    return;
}

/*
 * Give an obj to a char.
 */
OBJ_DATA *obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    OBJ_DATA *otmp;
    OBJ_DATA *oret = obj;
    bool skipgroup, grouped;
    int oweight = get_obj_weight(obj);
    int onum = get_obj_number(obj);
    int wear_loc = obj->wear_loc;
    EXT_BV extra_flags = obj->extra_flags;

    skipgroup = FALSE;
    grouped = FALSE;

    if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
    {
	if (!IS_IMMORTAL( ch ) && !IS_ACT_FLAG(ch, ACT_PROTOTYPE) )
	  return obj_to_room( obj, ch->in_room, ch );
    }

    /* Should handle all cases of picking stuff up from maps - Samson */
    if ( ch != supermob )
    {
      REMOVE_OBJ_STAT( obj, ITEM_ONMAP );
      obj->x = -1;
      obj->y = -1;
      obj->map = -1;
    }
    if ( loading_char == ch )
    {
	int x,y;
	for ( x = 0; x < MAX_WEAR; x++ )
	    for ( y = 0; y < MAX_LAYERS; y++ )
		if ( save_equipment[x][y] == obj )
		{
		    skipgroup = TRUE;
		    break;
		}
    }
    if( IS_NPC(ch) && ch->pIndexData->pShop )
      skipgroup = TRUE;
    if ( !skipgroup )
	for ( otmp = ch->first_carrying; otmp; otmp = otmp->next_content )
	    if ( (oret=group_object( otmp, obj )) == otmp )
	    {
		grouped = TRUE;
		break;
	    }
    if ( !grouped )
    {
	if (!IS_NPC(ch) || !ch->pIndexData->pShop)
	{
	    LINK( obj, ch->first_carrying, ch->last_carrying,	next_content, prev_content );
	    obj->carried_by                 = ch;
	    obj->in_room                    = NULL;
	    obj->in_obj                     = NULL;
        }
        else
        {
	    /* If ch is a shopkeeper, add the obj using an insert sort */
	    for ( otmp = ch->first_carrying; otmp; otmp = otmp->next_content)
	    {
		if ( obj->level > otmp->level )
		{
		    INSERT(obj, otmp, ch->first_carrying,	next_content, prev_content);
		    break;
		}
		else
		if ( obj->level == otmp->level && strcmp(obj->short_descr,otmp->short_descr) < 0 )
		{
		    INSERT(obj, otmp, ch->first_carrying,	next_content, prev_content);
		    break;
		}
	    }
                
	    if ( !otmp )
	    {
		LINK(obj, ch->first_carrying, ch->last_carrying, next_content, prev_content);
	    }
                
	    obj->carried_by = ch;
	    obj->in_room = NULL;
	    obj->in_obj = NULL;
	}
    }
    if (wear_loc == WEAR_NONE)
    {
	ch->carry_number	+= onum;
	ch->carry_weight	+= oweight;
    }
    else
    if ( !xIS_SET(extra_flags, ITEM_MAGIC) )
	ch->carry_weight	+= oweight;
    return (oret ? oret : obj);
}




/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    /* obj may drop during unequip... */
    if ( !obj->carried_by )
      return;

    UNLINK( obj, ch->first_carrying, ch->last_carrying, next_content, prev_content );

    if ( IS_OBJ_STAT(obj, ITEM_COVERING) && obj->first_content )
	empty_obj( obj, NULL, NULL );

    obj->in_room	 = NULL;
    obj->carried_by	 = NULL;
    ch->carry_number	-= get_obj_number(obj);
    ch->carry_weight	-= get_obj_weight(obj);
    return;
}

int get_extra( sh_int grade, sh_int value )
{
  grade = URANGE( 1, grade, 5 );

  switch( value )
  {
    case 0:
    default:
      return 0;
    case 1:
    case 2:
    case 3:
      return value + (grade/2);
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      return (value * 1.5) + (grade/2);
  }
  return 0;

}


/*
 * Find the ac value of an obj, including position effect.
 */

int apply_ac( OBJ_DATA *obj, int iWear )
{
    if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_JEWELRY)
        return 0;

    switch ( iWear )
    {
    case WEAR_BODY:	return     obj->value[1];
    case WEAR_HEAD:	return     obj->value[1];
    case WEAR_LEGS:	return     obj->value[1];
    case WEAR_FEET:	return     obj->value[1];
    case WEAR_HANDS:	return     obj->value[1];
    case WEAR_ARMS:	return     obj->value[1];
    case WEAR_SHIELD:	return     obj->value[1];
    case WEAR_FINGER_L:	return     obj->value[1];
    case WEAR_FINGER_R: return     obj->value[1];
    case WEAR_NECK_1:	return     obj->value[1];
    case WEAR_NECK_2:	return     obj->value[1];
    case WEAR_ABOUT:	return     obj->value[1];
    case WEAR_WAIST:	return     obj->value[1];
    case WEAR_WRIST_L:	return     obj->value[1];
    case WEAR_WRIST_R:	return     obj->value[1];
    case WEAR_HOLD:	return     obj->value[1];
    case WEAR_EYES:	return	   obj->value[1];
    case WEAR_FACE:	return	   obj->value[1];
    case WEAR_BACK:	return	   obj->value[1];
    case WEAR_ANKLE_L:	return	   obj->value[1];
    case WEAR_ANKLE_R:	return	   obj->value[1];
    }

    return 0;
}

int apply_pdef( OBJ_DATA *obj, int iWear )
{
    sh_int value;
    if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_JEWELRY)
        return 0;
    
    value = obj->value[1] + get_extra( obj->value[3], obj->value[4] );
    switch ( iWear )
    {
        case WEAR_BODY:
        case WEAR_HEAD:
        case WEAR_LEGS:          
        case WEAR_FEET:          
        case WEAR_HANDS:         
        case WEAR_ARMS:          
        case WEAR_SHIELD:        
        case WEAR_FINGER_L:      
        case WEAR_FINGER_R:      
        case WEAR_NECK_1:        
        case WEAR_NECK_2:        
        case WEAR_ABOUT:         
        case WEAR_WAIST:         
        case WEAR_WRIST_L:       
        case WEAR_WRIST_R:       
        case WEAR_HOLD:          
        case WEAR_EYES:          
        case WEAR_FACE:          
        case WEAR_BACK:          
        case WEAR_ANKLE_L:       
        case WEAR_ANKLE_R:       
        return value;
    }
    return 0;
}

int apply_edef( OBJ_DATA *obj, int iWear )
{
    sh_int value;
    if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_JEWELRY)
        return 0;

    value = obj->value[2] + get_extra( obj->value[3], obj->value[4] );
    switch ( iWear )
    {
        case WEAR_BODY:
        case WEAR_HEAD:
        case WEAR_LEGS:
        case WEAR_FEET:
        case WEAR_HANDS:
        case WEAR_ARMS:
        case WEAR_SHIELD:
        case WEAR_FINGER_L:
        case WEAR_FINGER_R:
        case WEAR_NECK_1:
        case WEAR_NECK_2:
        case WEAR_ABOUT:
        case WEAR_WAIST:
        case WEAR_WRIST_L:
        case WEAR_WRIST_R:
        case WEAR_HOLD:
        case WEAR_EYES:
        case WEAR_FACE:
        case WEAR_BACK:
        case WEAR_ANKLE_L:
        case WEAR_ANKLE_R:
        return value;
    }
    return 0;
}


int apply_eatk( OBJ_DATA *obj, int iWear )
{
    sh_int value;
    if ( obj->item_type != ITEM_WEAPON )
        return 0;

    value = obj->value[2] + get_extra( obj->value[3], obj->value[4] );
    switch ( iWear )
    {
        case WEAR_WIELD:
        case WEAR_BODY:
        case WEAR_HEAD:
        case WEAR_LEGS:
        case WEAR_FEET:
        case WEAR_HANDS:
        case WEAR_ARMS:
        case WEAR_SHIELD:
        case WEAR_FINGER_L:
        case WEAR_FINGER_R:
        case WEAR_NECK_1:
        case WEAR_NECK_2:
        case WEAR_ABOUT:
        case WEAR_WAIST:
        case WEAR_WRIST_L:
        case WEAR_WRIST_R:
        case WEAR_HOLD:
        case WEAR_EYES:
        case WEAR_FACE:
        case WEAR_BACK:
        case WEAR_ANKLE_L:
        case WEAR_ANKLE_R:
        return value;
    }
    return 0;
}

int apply_patk( OBJ_DATA *obj, int iWear )
{
    sh_int value;
    if ( obj->item_type != ITEM_WEAPON )
        return 0;

    value = obj->value[1] + get_extra( obj->value[3], obj->value[4] );
    switch ( iWear )
    {
        case WEAR_WIELD:
        case WEAR_BODY:
        case WEAR_HEAD:
        case WEAR_LEGS:
        case WEAR_FEET:
        case WEAR_HANDS:
        case WEAR_ARMS:
        case WEAR_SHIELD:
        case WEAR_FINGER_L:
        case WEAR_FINGER_R:
        case WEAR_NECK_1:
        case WEAR_NECK_2:
        case WEAR_ABOUT:
        case WEAR_WAIST:
        case WEAR_WRIST_L:
        case WEAR_WRIST_R:
        case WEAR_HOLD:
        case WEAR_EYES:
        case WEAR_FACE:
        case WEAR_BACK:
        case WEAR_ANKLE_L:
        case WEAR_ANKLE_R:
        return value;
    }
    return 0;
}


/*
 * Find a piece of eq on a character.
 * Will pick the top layer if clothing is layered.		-Thoric
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj, *maxobj = NULL;

    if ( !ch )
    {
      bug( "Null Ch, Get_eq_char" );
      return NULL;
    }

    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
      if ( obj->wear_loc == iWear )
      {
	    if ( !obj->pIndexData->layers )
		return obj;
	    else
	    if ( !maxobj
	    ||    obj->pIndexData->layers > maxobj->pIndexData->layers )
		maxobj = obj;
      }

    return maxobj;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    OBJ_DATA	*otmp;

    if( obj->carried_by != ch )
    {
        bug( "equip_char: obj not being carried by ch!" );
        return;
    }

    if ( (otmp=get_eq_char( ch, iWear )) != NULL
    &&   ( !otmp->pIndexData->layers || !obj->pIndexData->layers) )
    {
	bug( "Equip_char: already equipped (%d).", iWear );
	return;
    }


    separate_obj(obj);	/* just in case */
    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	if ( loading_char != ch )
	{
	   act( AT_MAGIC, "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	   act( AT_MAGIC, "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	}
	if ( obj->carried_by )
	   obj_from_char( obj );
	obj_to_room( obj, ch->in_room, ch );
	oprog_zap_trigger( ch, obj);
	if ( IS_SET(sysdata.save_flags, SV_ZAPDROP) && !char_died(ch) )
	    save_char_obj( ch );
	return;
    }

    ch->armory      	+= apply_ac( obj, iWear );
    ch->pdef		+= apply_pdef( obj, iWear );
    ch->edef            += apply_edef( obj, iWear );
    ch->patk            += apply_patk( obj, iWear );
    ch->eatk            += apply_eatk( obj, iWear );

    obj->wear_loc	 = iWear;

    ch->carry_number	-= get_obj_number( obj );
    if ( IS_OBJ_STAT(obj, ITEM_MAGIC) )
      ch->carry_weight  -= get_obj_weight( obj );

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room )
	++ch->in_room->light;

    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{

    if ( obj->wear_loc == WEAR_NONE )
    {
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }

    ch->carry_number	+= get_obj_number( obj );
    if ( IS_OBJ_STAT(obj, ITEM_MAGIC) )
	ch->carry_weight  += get_obj_weight( obj );

    ch->armory		-= apply_ac( obj, obj->wear_loc );
    ch->pdef            -= apply_pdef( obj, obj->wear_loc );
    ch->edef            -= apply_edef( obj, obj->wear_loc );
    ch->patk            -= apply_patk( obj, obj->wear_loc );
    ch->eatk            -= apply_eatk( obj, obj->wear_loc );

    obj->wear_loc	 = -1;


    if ( !obj->carried_by )
      return;

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room
    &&   ch->in_room->light > 0 )
	--ch->in_room->light;

    return;
}






/*
 * Move an obj out of a room.
 */
void	write_corpses	args( ( CHAR_DATA *ch, char *name, OBJ_DATA *objrem ) );

int falling;

void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;

    if ( (in_room = obj->in_room) == NULL )
    {
	bug( "obj_from_room: NULL.", 0 );
	return;
    }

    UNLINK( obj, in_room->first_content, in_room->last_content,
	next_content, prev_content );

    /* uncover contents */
    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) && obj->first_content )
	empty_obj( obj, NULL, obj->in_room );

    if ( obj->item_type == ITEM_FIRE )
	  obj->in_room->light -= obj->count;

    obj->carried_by   = NULL;
    obj->in_obj	      = NULL;
    obj->in_room      = NULL;
    if ( obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && falling < 1 )
	write_corpses( NULL, obj->short_descr+14, obj );
    return;
}


/*
 * Move an obj into a room.
 */
OBJ_DATA *obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex, CHAR_DATA *ch )
{
    OBJ_DATA *otmp, *oret;
    short count = obj->count;
    short item_type = obj->item_type;
    
    if (!pRoomIndex)
    {
       bug( "Obj_to_Room returns NULL Room");
       return NULL;
    }
    for ( otmp = pRoomIndex->first_content; otmp; otmp = otmp->next_content )
	if ( (oret=group_object( otmp, obj )) == otmp )
	{
	    if ( item_type == ITEM_FIRE )
		pRoomIndex->light += count;
	    return oret;
	}

    LINK( obj, pRoomIndex->first_content, pRoomIndex->last_content, next_content, prev_content );
    obj->in_room				= pRoomIndex;
    obj->carried_by				= NULL;
    obj->in_obj					= NULL;
    if ( item_type == ITEM_FIRE )
	pRoomIndex->light += count;
    falling++;
    obj_fall( obj, FALSE );
    falling--;

/* Hoping that this will cover all instances of objects from character to room - Samson 8-22-99 */
    if( ch != NULL )
    {
	if( IS_ACT_FLAG( ch, ACT_ONMAP ) || IS_PLR_FLAG( ch, PLR_ONMAP ) )
      {
	   SET_OBJ_STAT( obj, ITEM_ONMAP );
	   obj->map = ch->map;
	   obj->x = ch->x;
	   obj->y = ch->y;
	}
	else
      {
	   REMOVE_OBJ_STAT( obj, ITEM_ONMAP );
	   obj->map = -1;
	   obj->x = -1;
	   obj->y = -1;
	}
    }
 
    if ( obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && falling < 1 )
	write_corpses( NULL, obj->short_descr+14, NULL );

    return obj;
}


/*
 * Who's carrying an item -- recursive for nested objects	-Thoric
 */
CHAR_DATA *carried_by( OBJ_DATA *obj )
{
    if ( obj->in_obj )
	return carried_by(obj->in_obj);

    return obj->carried_by;
}


/*
 * Move an object into an object.
 */
OBJ_DATA *obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    OBJ_DATA *otmp, *oret;
    CHAR_DATA *who;

    if ( obj == obj_to )
    {
	bug( "Obj_to_obj: trying to put object inside itself: vnum %d", obj->pIndexData->vnum );
	return obj;
    }

    if ( !in_magic_container(obj_to) && (who=carried_by(obj_to)) != NULL )
	who->carry_weight += get_obj_weight(obj);

    for ( otmp = obj_to->first_content; otmp; otmp = otmp->next_content )
	if ( (oret=group_object(otmp, obj)) == otmp )
	    return oret;

    LINK( obj, obj_to->first_content, obj_to->last_content,
	next_content, prev_content );

    obj->in_obj				 = obj_to;
    obj->in_room			 = NULL;
    obj->carried_by			 = NULL;

    return obj;
}


/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;
    bool magic;

    if ( (obj_from = obj->in_obj) == NULL )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    magic = in_magic_container(obj_from);

    UNLINK( obj, obj_from->first_content, obj_from->last_content,	next_content, prev_content );

    /* uncover contents */
    if ( IS_OBJ_STAT(obj, ITEM_COVERING) && obj->first_content )
	empty_obj(obj, obj->in_obj, NULL);

    obj->in_obj       = NULL;
    obj->in_room      = NULL;
    obj->carried_by   = NULL;

/* This will hopefully cover all objs coming from containers going to the maps - Samson 8-22-99 */
    if( IS_OBJ_STAT( obj_from, ITEM_ONMAP ) )
    {
	SET_OBJ_STAT( obj, ITEM_ONMAP );
	obj->map = obj_from->map;
	obj->x = obj_from->x;
	obj->y = obj_from->y;
    }

    if ( !magic )
	for ( ; obj_from; obj_from = obj_from->in_obj )
	    if ( obj_from->carried_by )
		obj_from->carried_by->carry_weight -= get_obj_weight( obj );

    return;
}





/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    REL_DATA *RQueue, *rq_next;

    if ( !obj )
    {
       bug( "Extract Obj: NULL" );
       return;
    }
    if ( obj_extracted(obj) )
    {
	bug( "extract_obj: obj %d already extracted!", obj->pIndexData->vnum );
	return;
    }

    if ( obj->carried_by )
	obj_from_char( obj );
    else
    if ( obj->in_room )
	obj_from_room( obj );
    else
    if ( obj->in_obj )
	obj_from_obj( obj );

    while ( ( obj_content = obj->last_content ) != NULL )
	extract_obj( obj_content );


    /* remove extra descriptions */
    {
	EXTRA_DESCR_DATA *ed;
	EXTRA_DESCR_DATA *ed_next;

	for ( ed = obj->first_extradesc; ed; ed = ed_next )
	{
	    ed_next = ed->next;
	    STRFREE( ed->description );
	    STRFREE( ed->keyword     );
	    DISPOSE( ed );
	}
	obj->first_extradesc = obj->last_extradesc = NULL;
    }

    if ( obj == gobj_prev )
	     gobj_prev = obj->prev;

    for ( RQueue = first_relation; RQueue; RQueue = rq_next ) {
       rq_next = RQueue->next;
       if ( RQueue->Type == relOSET_ON ) {
         if ( obj == RQueue->Subject )
           ( ( CHAR_DATA * ) RQueue->Actor )->dest_buf = NULL;
         else
            continue;
         UNLINK( RQueue, first_relation, last_relation, next, prev );
         DISPOSE( RQueue );
       }
    }



    UNLINK( obj, first_object, last_object, next, prev );

    /* shove onto extraction queue */
    queue_extracted_obj( obj );

    obj->pIndexData->count -= obj->count;
    numobjsloaded -= obj->count;
    --physicalobjects;
    if ( obj->serial == cur_obj )
    {
	cur_obj_extracted = TRUE;
	if ( global_objcode == rNONE )
	    global_objcode = rOBJ_EXTRACTED;
    }
    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    REL_DATA *RQueue, *rq_next; 

    if ( !ch )
    {
    	bug( "Extract_char: NULL ch.", 0 );
	return;
    }

    if ( !ch->in_room )
    {
	bug( "Extract_char: %s in NULL room.", ch->name ? ch->name : "???" );
	return;
    }

    if ( ch == supermob )
    {
	bug( "Extract_char: ch == supermob!", 0 );
	return;
    }

    if ( char_died(ch) )
    {
	bug( "extract_char: %s already died!", ch->name );
	return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_STADIUM))
    {
        ch->hit = ch->max_hit;
        ch->mana = ch->max_mana;
        ch->move = ch->max_move;
    }
    if ( ch == cur_char )
	cur_char_died = TRUE;

    /* shove onto extraction queue */
    queue_extracted_char( ch, fPull );

   for ( RQueue = first_relation; RQueue; RQueue = rq_next ) {
       rq_next = RQueue->next;
       if ( fPull && RQueue->Type == relMSET_ON ) {
          if ( ch == RQueue->Subject )
             ( ( CHAR_DATA * ) RQueue->Actor )->dest_buf = NULL;
          else if ( ch != RQueue->Actor )
             continue;
          UNLINK( RQueue, first_relation, last_relation, next, prev );
          DISPOSE( RQueue );
       }
    }


    if ( gch_prev == ch )
	gch_prev = ch->prev;

    if ( fPull)
	die_follower( ch );

    stop_fighting( ch, TRUE );

    if ( ch->mount )
    {
	xREMOVE_BIT( ch->mount->act, ACT_MOUNTED );
	ch->mount = NULL;
	ch->position = POS_STANDING;
    }

    /*
     * check if this NPC was a mount or a pet
     */
    if ( IS_NPC(ch) )
    {
	for ( wch = first_char; wch; wch = wch->next )
	{
	    if ( wch->mount == ch )
	    {
		wch->mount = NULL;
		wch->position = POS_STANDING;
		if ( wch->in_room == ch->in_room )
		{
		    act( AT_SOCIAL, "Your faithful mount, $N collapses beneath you...",
		    	wch, NULL, ch, TO_CHAR );
		    act( AT_SOCIAL, "Sadly you dismount $M for the last time.",
			wch, NULL, ch, TO_CHAR );
		    act( AT_PLAIN, "$n sadly dismounts $N for the last time.",
			wch, NULL, ch, TO_ROOM );
		}
	    }
	    if ( wch->pcdata && wch->pcdata->pet == ch )
	    {
		wch->pcdata->pet = NULL;
		if ( wch->in_room == ch->in_room )
		    act( AT_SOCIAL, "You mourn for the loss of $N.",
			wch, NULL, ch, TO_CHAR );
	    }
	}
    }
    xREMOVE_BIT( ch->act, ACT_MOUNTED );

/*    while ( (obj = ch->last_carrying) != NULL )
	extract_obj(obj);*/
    char_from_room(ch);

    if ( !fPull )
    {
	location = NULL;


	if ( !location )
	    location = get_room_index( ROOM_VNUM_ALTAR );

	if ( !location )
	    location = get_room_index( 1 );

	char_to_room( ch, location );

	if( IS_PLR_FLAG( ch, PLR_ONMAP ) )
	{
    	   REMOVE_PLR_FLAG( ch, PLR_ONMAP );
	   REMOVE_PLR_FLAG( ch, PLR_MAPEDIT ); /* Just in case they were editing */

         ch->x = -1;
    	   ch->y = -1;
    	   ch->map = -1;
	}
	/*
	 * Make things a little fancier				-Thoric
	 */
	if ( ( wch = get_char_room( ch, "healer" ) ) != NULL )
	{
	    act( AT_MAGIC, "$n mutters a few incantations, waves $s hands and points $s finger.",
		wch, NULL, NULL, TO_ROOM );
	    act( AT_MAGIC, "$n appears from some strange swirling mists!", ch, NULL, NULL, TO_ROOM );
	    sprintf(buf, "Welcome back to the land of the living, %s",
		    capitalize( ch->name ) );
	    do_say( wch, buf );
	}
	else
	    act( AT_MAGIC, "$n appears from some strange swirling mists!", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_STANDING;
        ch->mana = ch->max_mana;
        ch->hit = ch->max_hit;
	return;
    }

    if ( IS_NPC(ch) )
    {
	--ch->pIndexData->count;
	--nummobsloaded;
    }

  /* Not sure this should stay or not Shaddai */

    if ( ch->desc && ch->desc->original )
	do_return( ch, "" );

    for ( wch = first_char; wch; wch = wch->next )
    {
    	if ( wch->reply == ch )
	    wch->reply = NULL;
	if( wch->retell == ch )
	    wch->retell = NULL;
    }

    UNLINK( ch, first_char, last_char, next, prev );

    if ( ch->desc )
    {
	if ( ch->desc->character != ch )
	    bug( "Extract_char: char's descriptor points to another char", 0 );
	else
	{
	    ch->desc->character = NULL;
	    close_socket( ch->desc, FALSE );
	    ch->desc = NULL;
	}
    }

    return;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    int number, count, vnum;

    number = number_argument( argument, arg );
    if ( !str_cmp( arg, "self" ) )
	return ch;

    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    count  = 0;

/*
    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
	if ( can_see( ch, rch, FALSE )
	&&  ( ( nifty_is_name( arg, rch->name ) && !xIS_SET(rch->act, PLR_FUSED)) ||  (IS_NPC(rch) && vnum == 
rch->pIndexData->vnum)) )
	{
	    if ( number == 0 && !IS_NPC(rch) )
		return rch;
	    else
	    if ( ++count == number )
		return rch;
	}
*/

    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
        if ( can_see( ch, rch, FALSE ))
        {
          if ( nifty_is_name( arg, rch->name ) && !IS_NPC(rch) && !xIS_SET(rch->act, PLR_FUSED))
          {
            if ( number == 0 && !IS_NPC(rch) )
                return rch;
            else
            if ( ++count == number )
                return rch;
          }
          else if ( nifty_is_name( arg, rch->fusionname ) && !IS_NPC(rch) && xIS_SET(rch->act, PLR_FUSED))
          {
            if ( number == 0 && !IS_NPC(rch) )
                return rch;
            else
            if ( ++count == number )
                return rch;
          }
          else if ( IS_NPC(rch) && vnum == rch->pIndexData->vnum)  
          {
            if ( number == 0 && !IS_NPC(rch) )
                return rch;
            else
            if ( ++count == number )
                return rch;
          }
        }
    }
    if ( vnum != -1 )
	return NULL;

    /*
     * If we didn't find an exact match, run through the list of characters
     * again looking for prefix matching, ie gu == guard.
     * Added by Narn, Sept/96
     */
    count  = 0;

    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch, FALSE ) || !nifty_is_name_prefix( arg, rch->name ) )
	    continue;
	if ( number == 0 && !IS_NPC(rch) )
	    return rch;
	else
	if ( ++count == number )
	    return rch;
    }
    return NULL;
}




/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number, count, vnum;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "self" ) )
	return ch;

    /*
     * Allow reference by vnum for saints+			-Thoric
     */
    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    /* check the room for an exact match */
    for ( wch = ch->in_room->first_person; wch; wch = wch->next_in_room )
	if ( can_see( ch, wch, TRUE )
	&&  (nifty_is_name( arg, wch->name )
	||  (IS_NPC(wch) && vnum == wch->pIndexData->vnum)) )
	{
	    if ( number == 0 && !IS_NPC(wch) )
		return wch;
	    else
	    if ( ++count == number )
		return wch;
	}

    count = 0;

    

    /* check the world for an exact match */
    for ( wch = first_char; wch; wch = wch->next )
	if ( can_see( ch, wch, TRUE )
	&&  (nifty_is_name( arg, wch->name )
	||  (IS_NPC(wch) && vnum == wch->pIndexData->vnum)) )
	{
	    if ( number == 0 && !IS_NPC(wch) )
		return wch;
	    else
	    if ( ++count == number )
		return wch;
	}

    /* bail out if looking for a vnum match */
    if ( vnum != -1 )
	return NULL;

    /*
     * If we didn't find an exact match, check the room for
     * for a prefix match, ie gu == guard.
     * Added by Narn, Sept/96
     */
    count  = 0;
    for ( wch = ch->in_room->first_person; wch; wch = wch->next_in_room )
    {
	if ( !can_see( ch, wch, TRUE ) || !nifty_is_name_prefix( arg, wch->name ) )
	    continue;
	if ( number == 0 && !IS_NPC(wch) )
	    return wch;
	else
	if ( ++count == number )
	    return wch;
    }

    /*
     * If we didn't find a prefix match in the room, run through the full list
     * of characters looking for prefix matching, ie gu == guard.
     * Added by Narn, Sept/96
     */
    count  = 0;
    for ( wch = first_char; wch; wch = wch->next )
    {
	if ( !can_see( ch, wch, TRUE ) || !nifty_is_name_prefix( arg, wch->name ) )
	    continue;
	if ( number == 0 && !IS_NPC(wch) )
	    return wch;
	else
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}




/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) && nifty_is_name( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    /*
     * If we didn't find an exact match, run through the list of objects
     * again looking for prefix matching, ie swo == sword.
     * Added by Narn, Sept/96
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) && nifty_is_name_prefix( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    return NULL;
}

/*
 * Find an obj in a list...going the other way			-Thoric
 */
OBJ_DATA *get_obj_list_rev( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj; obj = obj->prev_content )
	if ( can_see_obj( ch, obj ) && nifty_is_name( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    /*
     * If we didn't find an exact match, run through the list of objects
     * again looking for prefix matching, ie swo == sword.
     * Added by Narn, Sept/96
     */
    count = 0;
    for ( obj = list; obj; obj = obj->prev_content )
	if ( can_see_obj( ch, obj ) && nifty_is_name_prefix( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    return NULL;
}

/*
 * Find an obj in player's inventory or wearing via a vnum -Shaddai
 */

OBJ_DATA *get_obj_vnum( CHAR_DATA *ch, int vnum )
{
    OBJ_DATA *obj;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        if (  can_see_obj( ch, obj ) && obj->pIndexData->vnum == vnum )
                return obj;
    return NULL;
}


/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number, count, vnum;

    number = number_argument( argument, arg );
    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    count  = 0;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
	if ( obj->wear_loc == WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&  (nifty_is_name( arg, obj->name ) || obj->pIndexData->vnum == vnum) )
	    if ( (count += obj->count) >= number )
		return obj;

    if ( vnum != -1 )
	return NULL;

    /*
     * If we didn't find an exact match, run through the list of objects
     * again looking for prefix matching, ie swo == sword.
     * Added by Narn, Sept/96
     */
    count = 0;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
	if ( obj->wear_loc == WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&   nifty_is_name_prefix( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number, count, vnum;

    number = number_argument( argument, arg );

    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    count  = 0;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
	if ( obj->wear_loc != WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&  (nifty_is_name( arg, obj->name ) || obj->pIndexData->vnum == vnum) )
	    if ( ++count == number )
		return obj;

    if ( vnum != -1 )
	return NULL;

    /*
     * If we didn't find an exact match, run through the list of objects
     * again looking for prefix matching, ie swo == sword.
     * Added by Narn, Sept/96
     */
    count = 0;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
	if ( obj->wear_loc != WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&   nifty_is_name_prefix( arg, obj->name ) )
	    if ( ++count == number )
		return obj;

    return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list_rev( ch, argument, ch->in_room->last_content );
    if ( obj && ch->map == obj->map && ch->x == obj->x && ch->y == obj->y )
	return obj;

    if ( ( obj = get_obj_carry( ch, argument ) ) != NULL )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) != NULL )
	return obj;

    return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number, count, vnum;

    if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
	return obj;

    number = number_argument( argument, arg );

    /*
     * Allow reference by vnum for saints+			-Thoric
     */
    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    count  = 0;
    for ( obj = first_object; obj; obj = obj->next )
	if ( (nifty_is_name( arg, obj->name )
	||   vnum == obj->pIndexData->vnum) )
	    if ( (count += obj->count) >= number )
		return obj;

    /* bail out if looking for a vnum */
    if ( vnum != -1 )
	return NULL;

    /*
     * If we didn't find an exact match, run through the list of objects
     * again looking for prefix matching, ie swo == sword.
     * Added by Narn, Sept/96
     */
    count  = 0;
    for ( obj = first_object; obj; obj = obj->next )
	if ( nifty_is_name_prefix( arg, obj->name ) )
	    if ( (count += obj->count) >= number )
		return obj;

    return NULL;
}


/*
 * How mental state could affect finding an object		-Thoric
 * Used by get/drop/put/quaff/recite/etc
 * Increasingly freaky based on mental state and drunkeness
 */
bool ms_find_obj( CHAR_DATA *ch )
{
    int ms = 0;
    int drunk = IS_NPC(ch) ? 0 : ch->drunk;
    char *t;

    /*
     * we're going to be nice and let nothing weird happen unless
     * you're a tad messed up
     */
    drunk = UMAX( 1, drunk );
    if ( abs(ms) + (drunk/3) < 30 )
	return FALSE;
    if ( (number_percent() + (ms < 0 ? 15 : 5))> abs(ms)/2 + drunk/4 )
	return FALSE;
    if ( ms > 15 )	/* range 1 to 20 -- feel free to add more */
	switch( number_range( UMAX(1, (ms/5-15)), (ms+4) / 5 ) )
	{
	    default:
	    case  1: t="As you reach for it, you forgot what it was...\n\r";					break;
	    case  2: t="As you reach for it, something inside stops you...\n\r";				break;
	    case  3: t="As you reach for it, it seems to move out of the way...\n\r";				break;
	    case  4: t="You grab frantically for it, but can't seem to get a hold of it...\n\r";		break;
	    case  5: t="It disappears as soon as you touch it!\n\r";						break;
	    case  6: t="You would if it would stay still!\n\r";							break;
	    case  7: t="Whoa!  It's covered in blood!  Ack!  Ick!\n\r";						break;
	    case  8: t="Wow... trails!\n\r";									break;
	    case  9: t="You reach for it, then notice the back of your hand is growing something!\n\r";		break;
	    case 10: t="As you grasp it, it shatters into tiny shards which bite into your flesh!\n\r";		break;
	    case 11: t="What about that huge dragon flying over your head?!?!?\n\r";				break;
	    case 12: t="You stratch yourself instead...\n\r";							break;
	    case 13: t="You hold the universe in the palm of your hand!\n\r";					break;
	    case 14: t="You're too scared.\n\r";								break;
	    case 15: t="Your mother smacks your hand... 'NO!'\n\r";						break;
	    case 16: t="Your hand grasps the worst pile of revoltingness that you could ever imagine!\n\r";	break;
	    case 17: t="You stop reaching for it as it screams out at you in pain!\n\r";			break;
	    case 18: t="What about the millions of burrow-maggots feasting on your arm?!?!\n\r";		break;
	    case 19: t="That doesn't matter anymore... you've found the true answer to everything!\n\r";	break;
	    case 20: t="A supreme entity has no need for that.\n\r";						break;
	}
    else
    {
	int sub = URANGE(1, abs(ms)/2 + drunk, 60);
	switch( number_range( 1, sub/10 ) )
	{
	    default:
	    case  1: t="In just a second...\n\r";					break;
	    case  2: t="You can't find that...\n\r";					break;
	    case  3: t="It's just beyond your grasp...\n\r";				break;
	    case  4: t="...but it's under a pile of other stuff...\n\r";		break;
	    case  5: t="You go to reach for it, but pick your nose instead.\n\r";	break;
	    case  6: t="Which one?!?  I see two... no three...\n\r";			break;
	}
    }
    send_to_char( t, ch );
    return TRUE;
}


/*
 * Generic get obj function that supports optional containers.	-Thoric
 * currently only used for "eat" and "quaff".
 */
OBJ_DATA *find_obj( CHAR_DATA *ch, char *argument, bool carryonly )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj = NULL;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !str_cmp( arg2, "from" )
    &&   argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg2[0] == '\0' )
    {
	if ( carryonly && ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return NULL;
	}
	else
	if ( !carryonly && ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    act( AT_PLAIN, "I see no $T here.", ch, NULL, arg1, TO_CHAR );
	    return NULL;
	}
	return obj;
    }
    else
    {
	OBJ_DATA *container = NULL;
	
	if ( carryonly
	&& ( container = get_obj_carry( ch, arg2 ) ) == NULL
	&& ( container = get_obj_wear( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return NULL;
	}
	if ( !carryonly && ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return NULL;
	}
	
	if ( !IS_OBJ_STAT(container, ITEM_COVERING )
	&&    IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return NULL;
	}

	obj = get_obj_list( ch, arg1, container->first_content );
	if ( !obj )
	    act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
		"I see nothing like that beneath $p." :
		"I see nothing like that in $p.",
		ch, container, NULL, TO_CHAR );
	return obj;
    }
    return NULL;
}

int get_obj_number( OBJ_DATA *obj )
{
    return obj->count;
}

/*
 * Return TRUE if an object is, or nested inside a magic container
 */
bool in_magic_container( OBJ_DATA *obj )
{
    if ( obj->item_type == ITEM_CONTAINER && IS_OBJ_STAT(obj, ITEM_MAGIC) )
	return TRUE;
    if ( obj->in_obj )
	return in_magic_container(obj->in_obj);
    return FALSE;
}

/*
 * Return weight of an object, including weight of contents (unless magic).
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->count * obj->weight;

    /* magic containers */
    if ( obj->item_type != ITEM_CONTAINER || !IS_OBJ_STAT(obj, ITEM_MAGIC) )
	for ( obj = obj->first_content; obj; obj = obj->next_content )
	    weight += get_obj_weight(obj);

    return weight;
}

/*
 * Return real weight of an object, including weight of contents.
 */
int get_real_obj_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->count * obj->weight;

    for ( obj = obj->first_content; obj; obj = obj->next_content )
	weight += get_real_obj_weight(obj);

    return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( !pRoomIndex )
    {
	bug( "room_is_dark: NULL pRoomIndex", 0 );
	return TRUE;
    }

    if ( pRoomIndex->light > 0 )
	return FALSE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
    ||   pRoomIndex->sector_type == SECT_CITY )
	return FALSE;

    if ( time_info.sunlight == SUN_SET
    ||   time_info.sunlight == SUN_DARK )
	return FALSE;

    return FALSE;
}


/*
 * If room is "do not disturb" return the pointer to the imm with dnd flag
 * NULL if room is not "do not disturb".
 */
CHAR_DATA *room_is_dnd( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;

    if ( !pRoomIndex )
    {
        bug( "room_is_dnd: NULL pRoomIndex", 0 );
        return NULL;
    }

/*   if ( !IS_SET(pRoomIndex->room_flags, ROOM_DND) )
      return NULL;*/

   for ( rch = pRoomIndex->first_person; rch; rch = rch->next_in_room )
   {
      if ( !IS_NPC(rch) && rch->pcdata && IS_IMMORTAL(rch)
      &&   xIS_SET(rch->pcdata->flags, PCFLAG_DND)
      &&   get_trust(ch) < get_trust(rch)
      &&   can_see(ch, rch, FALSE) )
           return rch;
   }
    return NULL;
}


/*
 * True if room is private.
 */
bool room_is_private( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int count;

    if ( !pRoomIndex )
    {
	bug( "room_is_private: NULL pRoomIndex", 0 );
	return FALSE;
    }

    count = 0;
    for ( rch = pRoomIndex->first_person; rch; rch = rch->next_in_room )
	count++;

    if ( IS_SET(pRoomIndex->room_flags2, ROOM_PLR_HOME) && ch->plr_home != pRoomIndex && ch->level < 60)
        return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
	return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
	return TRUE;

    return FALSE;
}



/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim, bool override )
{
    if ( !victim )            /* Gorog - panicked attempt to stop crashes */
       return FALSE;

    if ( !ch )
    {
      if ( xIS_SET(victim->act, PLR_WIZINVIS) ) 
	return FALSE;
      else
	return TRUE;
    }

    if ( ch == victim )
	return TRUE;

    if ( IS_PLR_FLAG( victim, PLR_WIZINVIS ) && (ch->level < victim->pcdata->wizinvis && ch->trust < victim->pcdata->wizinvis ))
	return FALSE;

    /* SB */
    if ( IS_ACT_FLAG( victim, ACT_MOBINVIS ) && ch->level < victim->mobinvis )
        return FALSE;

/* Deadlies link-dead over 2 ticks aren't seen by mortals -- Blodkai */
    if ( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && !IS_NPC( victim ) && IS_PKILL( victim ) && victim->timer > 1 && !victim->desc )
	return FALSE;

    if ( ( IS_PLR_FLAG( ch, PLR_ONMAP ) || IS_ACT_FLAG( ch, ACT_ONMAP ) ) && override == FALSE )
    {
	if( !is_same_map( ch, victim ) )
	   return FALSE;
    }

    if ( IS_PLR_FLAG( ch, PLR_HOLYLIGHT ) )
	return TRUE;


    return TRUE;
}






/*
 * True if char can see obj.
 */

int getsamemap( int map );
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( IS_OBJ_STAT( obj, ITEM_ONMAP ) )
    {
	if( !IS_NPC( ch ) && !IS_PLR_FLAG( ch, PLR_ONMAP ) )
	    return FALSE;

	if( IS_NPC( ch ) && !IS_ACT_FLAG( ch, ACT_ONMAP ) )
	    return FALSE;
        if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
            return TRUE;
	if( ch->map != obj->map || obj->x != ch->x || obj->y != ch->y )
	    return FALSE;
    }
    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
	return TRUE;

    if ( IS_OBJ_STAT( obj, ITEM_BURIED ) )
	return FALSE;

    if ( IS_OBJ_STAT(obj, ITEM_HIDDEN) )
	return FALSE;


    /* can see lights in the dark */
    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	return TRUE;

    if ( room_is_dark(ch->in_room) )
    {
	/* can see glowing items in the dark... invisible or not */
	if ( IS_OBJ_STAT(obj, ITEM_GLOW) )
	    return TRUE;
    }

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_OBJ_STAT(obj, ITEM_NODROP) )
	return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;

    if ( IS_NPC(ch) && ch->pIndexData->vnum == 3 )
	return TRUE;

    return FALSE;
}


/*
 * Return ascii name of an item type.
 */
char *item_type_name( OBJ_DATA *obj )
{
    if ( obj->item_type < 1 || obj->item_type > MAX_ITEM_TYPE )
    {
	bug( "Item_type_name: unknown type %d.", obj->item_type );
	return "(unknown)";
    }

    return o_types[obj->item_type];
}





/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( EXT_BV *extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( xIS_SET(*extra_flags, ITEM_GLOW)	) strcat( buf, " glow"         );
    if ( xIS_SET(*extra_flags, ITEM_HUM)	) strcat( buf, " hum"          );
    if ( xIS_SET(*extra_flags, ITEM_DARK)	) strcat( buf, " dark"         );
    if ( xIS_SET(*extra_flags, ITEM_LOYAL)	) strcat( buf, " loyal"        );
    if ( xIS_SET(*extra_flags, ITEM_EVIL)	) strcat( buf, " evil"         );
    if ( xIS_SET(*extra_flags, ITEM_INVIS)	) strcat( buf, " invis"        );
    if ( xIS_SET(*extra_flags, ITEM_MAGIC)	) strcat( buf, " magic"        );
    if ( xIS_SET(*extra_flags, ITEM_NODROP)	) strcat( buf, " nodrop"       );
    if ( xIS_SET(*extra_flags, ITEM_BLESS)	) strcat( buf, " bless"        );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_GOOD)	) strcat( buf, " anti-good"    );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_EVIL)	) strcat( buf, " anti-evil"    );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_NEUTRAL)) strcat( buf, " anti-neutral" );
    if ( xIS_SET(*extra_flags, ITEM_NOREMOVE)	) strcat( buf, " noremove"     );
    if ( xIS_SET(*extra_flags, ITEM_INVENTORY)	) strcat( buf, " inventory"    );
    if ( xIS_SET(*extra_flags, ITEM_DEATHROT)	) strcat( buf, " deathrot"	);
    if ( xIS_SET(*extra_flags, ITEM_GROUNDROT)	) strcat( buf, " groundrot"	);
    if ( xIS_SET(*extra_flags, ITEM_ANTI_MAGE)	) strcat( buf, " anti-mage"    );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_THIEF)	) strcat( buf, " anti-thief"   );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_WARRIOR)) strcat( buf, " anti-warrior" );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_CLERIC)) strcat( buf, " anti-cleric"  );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_DRUID)	) strcat( buf, " anti-druid"   );
    if ( xIS_SET(*extra_flags, ITEM_ANTI_VAMPIRE)) strcat( buf, " anti-vampire" );
    if ( xIS_SET(*extra_flags, ITEM_ORGANIC)	) strcat( buf, " organic"      );
    if ( xIS_SET(*extra_flags, ITEM_METAL)	) strcat( buf, " metal"        );
    if ( xIS_SET(*extra_flags, ITEM_DONATION)	) strcat( buf, " donation"     );
    if ( xIS_SET(*extra_flags, ITEM_CLANOBJECT)	) strcat( buf, " clan"         );
    if ( xIS_SET(*extra_flags, ITEM_CLANCORPSE)	) strcat( buf, " clanbody"     );
    if ( xIS_SET(*extra_flags, ITEM_PROTOTYPE)	) strcat( buf, " prototype"    );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
 * Return ascii name of magic flags vector. - Scryn
 */
char *magic_bit_name( int magic_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( magic_flags & ITEM_RETURNING     ) strcat( buf, " returning"     );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
 * Return ascii name of pulltype exit setting.
 */
char *pull_type_name( int pulltype )
{
    if ( pulltype >= PT_FIRE )
	return ex_pfire[pulltype-PT_FIRE];
    if ( pulltype >= PT_AIR )
	return ex_pair[pulltype-PT_AIR];
    if ( pulltype >= PT_EARTH )
	return ex_pearth[pulltype-PT_EARTH];
    if ( pulltype >= PT_WATER )
	return ex_pwater[pulltype-PT_WATER];
    if ( pulltype < 0 )
	return "ERROR";

    return ex_pmisc[pulltype];
}



/*
 * return TRUE if an object contains a trap			-Thoric
 */
bool is_trapped( OBJ_DATA *obj )
{
    OBJ_DATA *check;

    if ( !obj->first_content )
      return FALSE;

    for ( check = obj->first_content; check; check = check->next_content )
      if ( check->item_type == ITEM_TRAP )
	return TRUE;

    return FALSE;
}

/*
 * If an object contains a trap, return the pointer to the trap	-Thoric
 */
OBJ_DATA *get_trap( OBJ_DATA *obj )
{
    OBJ_DATA *check;

    if ( !obj->first_content )
      return NULL;

    for ( check = obj->first_content; check; check = check->next_content )
      if ( check->item_type == ITEM_TRAP )
	return check;

    return NULL;
}

/*
 * Return a pointer to the first object of a certain type found that
 * a player is carrying/wearing
 */
OBJ_DATA *get_objtype( CHAR_DATA *ch, sh_int type )
{
    OBJ_DATA *obj;

    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	if ( obj->item_type == type )
	    return obj;

    return NULL;
}

/*
 * Remove an exit from a room					-Thoric
 */
void extract_exit( ROOM_INDEX_DATA *room, EXIT_DATA *pexit )
{
    UNLINK( pexit, room->first_exit, room->last_exit, next, prev );
    if ( pexit->rexit )
      pexit->rexit->rexit = NULL;
    STRFREE( pexit->keyword );
    STRFREE( pexit->description );
    DISPOSE( pexit );
}

/*
 * Remove a room
 */
void extract_room( ROOM_INDEX_DATA *room )
{
  bug( "extract_room: not implemented", 0 );
  /*
  (remove room from hash table)
  clean_room( room )
  DISPOSE( room );
   */
  return;
}

/*
 * clean out a room (leave list pointers intact )		-Thoric
 */
void clean_room( ROOM_INDEX_DATA *room )
{
   EXTRA_DESCR_DATA	*ed, *ed_next;
   EXIT_DATA		*pexit, *pexit_next;
   MPROG_DATA *mprog, *mprog_next;

   STRFREE( room->description );
   STRFREE( room->name );

  for( mprog = room->mudprogs; mprog; mprog = mprog_next )
  {
          mprog_next = mprog->next;
          STRFREE( mprog->arglist );
          STRFREE( mprog->comlist );
          DISPOSE( mprog );
   }
   for ( ed = room->first_extradesc; ed; ed = ed_next )
   {
	ed_next = ed->next;
	STRFREE( ed->description );
	STRFREE( ed->keyword );
	DISPOSE( ed );
	top_ed--;
   }
   room->first_extradesc	= NULL;
   room->last_extradesc		= NULL;
   for ( pexit = room->first_exit; pexit; pexit = pexit_next )
   {
	pexit_next = pexit->next;
	STRFREE( pexit->keyword );
	STRFREE( pexit->description );
	DISPOSE( pexit );
	top_exit--;
   }
   room->first_exit = NULL;
   room->last_exit = NULL;
   room->room_flags = 0;
   room->sector_type = 0;
   room->light = 0;
}

/*
 * clean out an object (index) (leave list pointers intact )	-Thoric
 */
void clean_obj( OBJ_INDEX_DATA *obj )
{
	EXTRA_DESCR_DATA *ed;
	EXTRA_DESCR_DATA *ed_next;
        MPROG_DATA *mprog, *mprog_next;

	STRFREE( obj->name );
	STRFREE( obj->short_descr );
	STRFREE( obj->description );
	STRFREE( obj->action_desc );
	obj->item_type		= 0;
	xCLEAR_BITS(obj->extra_flags);
	obj->wear_flags		= 0;
	obj->count		= 0;
	obj->weight		= 0;
	obj->cost		= 0;
	obj->value[0]		= 0;
	obj->value[1]		= 0;
	obj->value[2]		= 0;
	obj->value[3]		= 0;
	obj->value[4]		= 0;
	obj->value[5]		= 0;
	obj->pShip		= NULL;
	for ( ed = obj->first_extradesc; ed; ed = ed_next )
	{
	    ed_next		= ed->next;
	    STRFREE( ed->description );
	    STRFREE( ed->keyword     );
	    DISPOSE( ed );
	    top_ed--;
	}
	obj->first_extradesc	= NULL;
	obj->last_extradesc	= NULL;
        for( mprog = obj->mudprogs; mprog; mprog = mprog_next )
        {
          mprog_next = mprog->next;
          STRFREE( mprog->arglist );
          STRFREE( mprog->comlist );
          DISPOSE( mprog );
        }
}

/*
 * clean out a mobile (index) (leave list pointers intact )	-Thoric
 */
void clean_mob( MOB_INDEX_DATA *mob )
{
	MPROG_DATA *mprog, *mprog_next;

	STRFREE( mob->player_name );
	STRFREE( mob->short_descr );
	STRFREE( mob->long_descr  );
	STRFREE( mob->description );
	mob->spec_fun	= NULL;
	mob->pShop	= NULL;
	mob->rShop	= NULL;
	xCLEAR_BITS(mob->progtypes);
	
	for ( mprog = mob->mudprogs; mprog; mprog = mprog_next )
	{
	    mprog_next = mprog->next;
	    STRFREE( mprog->arglist );
	    STRFREE( mprog->comlist );
	    DISPOSE( mprog );
	}
	mob->count	 = 0;	   mob->killed		= 0;
	mob->sex	 = 0;	   mob->level		= 0;
	mob->alignment	 = 0;	   mob->mobthac0	= 0;
	mob->ac		 = 0;	   mob->hitnodice	= 0;
	mob->hitsizedice = 0;	   mob->hitplus		= 0;
	mob->damnodice	 = 0;	   mob->damsizedice	= 0;
	mob->damplus	 = 0;	   mob->gold		= 0;
	mob->exp	 = 0;	   mob->position	= 0;
	mob->defposition = 0;	   mob->height		= 0;
	mob->weight	 = 0;	/* mob->vnum		= 0;	*/
	xCLEAR_BITS(mob->attacks); xCLEAR_BITS(mob->defenses);
}

extern int top_reset;

/*
 * Remove all resets from an area				-Thoric
 */
/*
 * Remove all resets from a room -Thoric
 */
void clean_resets( ROOM_INDEX_DATA *room )
{
    RESET_DATA *pReset, *pReset_next;

    for( pReset = room->first_reset; pReset; pReset = pReset_next )
    {
	pReset_next = pReset->next;
	delete_reset( pReset );
	--top_reset;
    }
    room->first_reset	= NULL;
    room->last_reset	= NULL;
}



/*
 * "Roll" players stats based on the character name		-Thoric
 */
void name_stamp_stats( CHAR_DATA *ch )
{
    int x, a, b, c;

    ch->perm_str = UMIN( 8, ch->perm_str );
    ch->perm_wis = UMIN( 5, ch->perm_wis );
    ch->perm_dex = UMIN( 5, ch->perm_dex );
    ch->perm_int = UMIN( 5, ch->perm_int );
    ch->perm_con = UMIN( 5, ch->perm_con );
    ch->perm_cha = UMIN( 5, ch->perm_cha );
    ch->perm_lck = UMIN( 5, ch->perm_lck );
    ch->perm_str = UMAX( 8, ch->perm_str );
    ch->perm_wis = UMAX( 5, ch->perm_wis );
    ch->perm_dex = UMAX( 5, ch->perm_dex );
    ch->perm_int = UMAX( 5, ch->perm_int );
    ch->perm_con = UMAX( 5, ch->perm_con );
    ch->perm_cha = UMAX( 5, ch->perm_cha );
    ch->perm_lck = UMAX( 5, ch->perm_lck );

    for ( x = 0; x < strlen(ch->name); x++ )
    {
	c = ch->name[x] + x;
	b = c % 14;
	a = (c % 1) + 1;
	switch (b)
	{
	   case  0:
	     ch->perm_str = UMIN( 8, ch->perm_str );
	     break;
	   case  1:
	     ch->perm_dex = UMIN( 5, ch->perm_dex  );
	     break;
	   case  2:
	     ch->perm_wis = UMIN( 5, ch->perm_wis );
	     break;
	   case  3:
	     ch->perm_int = UMIN( 5, ch->perm_int );
	     break;
	   case  4:
	     ch->perm_con = UMIN( 5, ch->perm_con );
	     break;
	   case  5:
	     ch->perm_cha = UMIN( 5, ch->perm_cha );
	     break;
	   case  6:
	     ch->perm_lck = UMIN( 5, ch->perm_lck);
	     break;
	   case  7:
	     ch->perm_str = UMAX(  8, ch->perm_str  );
	     break;
	   case  8:
	     ch->perm_dex = UMAX(  5, ch->perm_dex );
	     break;
	   case  9:
	     ch->perm_wis = UMAX(  5, ch->perm_wis );
	     break;
	   case 10:
	     ch->perm_int = UMAX(  5, ch->perm_int );
	     break;
	   case 11:
	     ch->perm_con = UMAX(  5, ch->perm_con );
	     break;
	   case 12: 
	     ch->perm_cha = UMAX(  5, ch->perm_cha );
	     break;
	   case 13:
	     ch->perm_lck = UMAX(  5, ch->perm_lck );
	     break;
	}
    }
}


/*
* "Fix" a character's stats        -Thoric
*/
void fix_char( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    de_equip_char( ch );

    ch->hit             = UMAX( 1, ch->hit  );
    ch->mana            = UMAX( 1, ch->mana );
    ch->move            = UMAX( 1, ch->move );
    ch->armory          = 0;
    ch->mod_str         = 0;
    ch->patk		= 0;
    ch->pdef            = 0;
    ch->eatk            = 0;
    ch->edef            = 0;
    ch->height_mod      = 0;
    ch->weight_mod      = 0;
    ch->mod_dex         = 0;
    ch->mod_wis         = 0;
    ch->mod_int         = 0;
    ch->mod_con         = 0;
    ch->mod_cha         = 0;
    ch->mod_lck         = 0;
    ch->damroll         = 0;
    ch->hitroll         = 0;
    ch->alignment       = URANGE( -1000, ch->alignment, 1000 );

    ch->carry_weight    = 0;
    ch->carry_number    = 0;

    for( obj = ch->first_carrying; obj; obj = obj->next_content )
    {
      if( obj->wear_loc == WEAR_NONE )
            ch->carry_number += get_obj_number(obj);
      if( !xIS_SET(obj->extra_flags, ITEM_MAGIC) )
            ch->carry_weight += get_obj_weight(obj);
    }

    re_equip_char( ch );
}


/*
 * Set the current global object to obj				-Thoric
 */
void set_cur_obj( OBJ_DATA *obj )
{
    cur_obj = obj->serial;
    cur_obj_extracted = FALSE;
    global_objcode = rNONE;
}

/*
 * Check the recently extracted object queue for obj		-Thoric
 */
bool obj_extracted( OBJ_DATA *obj )
{
    OBJ_DATA *cod;

    if ( obj->serial == cur_obj
    &&   cur_obj_extracted )
	return TRUE;

    for (cod = extracted_obj_queue; cod; cod = cod->next )
	if ( obj == cod )
	     return TRUE;
    return FALSE;
}

/*
 * Stick obj onto extraction queue
 */
void queue_extracted_obj( OBJ_DATA *obj )
{
    
    ++cur_qobjs;
    obj->next = extracted_obj_queue;
    extracted_obj_queue = obj;
}

/*
 * Clean out the extracted object queue
 */
void clean_obj_queue()
{
    OBJ_DATA *obj;

    while ( extracted_obj_queue )
    {
	obj = extracted_obj_queue;
	extracted_obj_queue = extracted_obj_queue->next;
    STRFREE( obj->name        );
    STRFREE( obj->description );
    STRFREE( obj->short_descr );
    STRFREE( obj->action_desc );
    obj->pShip = NULL;
    DISPOSE( obj );
	--cur_qobjs;
    }
}

/*
 * Set the current global character to ch			-Thoric
 */
void set_cur_char( CHAR_DATA *ch )
{
    cur_char	   = ch;
    cur_char_died  = FALSE;
    cur_room	   = ch->in_room;
    global_retcode = rNONE;
}

/*
 * Check to see if ch died recently				-Thoric
 */
bool char_died( CHAR_DATA *ch )
{
    EXTRACT_CHAR_DATA *ccd;

    if ( ch == cur_char && cur_char_died )
	return TRUE;

    for (ccd = extracted_char_queue; ccd; ccd = ccd->next )
	if ( ccd->ch == ch )
	     return TRUE;
    return FALSE;
}

/*
 * Add ch to the queue of recently extracted characters		-Thoric
 */
void queue_extracted_char( CHAR_DATA *ch, bool extract )
{
    EXTRACT_CHAR_DATA *ccd;

    if ( !ch )
    {
	bug( "queue_extracted char: ch = NULL", 0 );
	return;
    }
    CREATE( ccd, EXTRACT_CHAR_DATA, 1 );
    ccd->ch			= ch;
    ccd->room			= ch->in_room;
    ccd->extract		= extract;
    if ( ch == cur_char )
      ccd->retcode		= global_retcode;
    else
      ccd->retcode		= rCHAR_DIED;
    ccd->next			= extracted_char_queue;
    extracted_char_queue	= ccd;
    cur_qchars++;
}

/*
 * clean out the extracted character queue
 */
void clean_char_queue()
{
    EXTRACT_CHAR_DATA *ccd;

    for ( ccd = extracted_char_queue; ccd; ccd = extracted_char_queue )
    {
	extracted_char_queue = ccd->next;
	if ( ccd->extract )
	  free_char( ccd->ch );
	DISPOSE( ccd );
	--cur_qchars;
    }
}

/*
 * Add a timer to ch for ki attacks
 */
 
void addtimer( CHAR_DATA *ch, const char *name, sh_int value)
{
  int gsn;
  SKILLTYPE *skill;

  if ( IS_NPC(ch) || IS_IMMORTAL(ch))
  return;
  if ( ( gsn = sskill_lookup( ch, name ) ) >= 0 )
  {
   skill = sskill_table[gsn];
   ch->pcdata->timer[gsn] = value;
   kitimer = URANGE( kitimer, value, 1000 );

  }
  else if ( ( gsn = askill_lookup( ch, name ) ) >= 0 )
  {
   ch->pcdata->timer2[gsn] = value;
   kitimer = URANGE( kitimer, value, 1000 );
   skill = askill_table[gsn];
  }
  else
   return;

  return;
}

/*
 * Read the ki timer off a char
 */
sh_int gettimer( CHAR_DATA *ch, const char *name )
{
  int gsn;
  SKILLTYPE *skill;

  if ( IS_NPC(ch))
  return 0;
  if ( ( gsn = sskill_lookup( ch, name ) ) >= 0 )
  {
   skill = sskill_table[gsn];
   return ch->pcdata->timer[gsn];
  }
  else if ( ( gsn = askill_lookup( ch, name ) ) >= 0 )
  {
   skill = askill_table[gsn];
   return ch->pcdata->timer2[gsn];
  }
  else
   return 0;

  return ch->pcdata->timer[gsn];
}
int kitimer;

/*
 * Update ki timers
 */
void update_timers()
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    sh_int sn;

    
/*    if ( kitimer <= 0 )
    return;*/
    kitimer = 0;
    for (d = first_descriptor; d; d = d->next)
    {
     if( d->connected == CON_PLAYING
     || d->connected == CON_NOTE_TO
     || d->connected == CON_NOTE_SUBJECT
     || d->connected == CON_NOTE_EXPIRE
     || d->connected == CON_NOTE_TEXT
     || d->connected == CON_NOTE_FINISH
     || d->connected == CON_EDITING )
     {
       ch = d->original ? d->original : d->character;
       if ( IS_NPC(ch) )
       continue;

/* Skills dont have timers but left this in incase
       for ( sn = 0; sn < top_sskill; sn++ )
       {
          if ( ch->pcdata->timer[sn] <= 0 )
          continue;
          ch->pcdata->timer[sn]--;
          kitimer--;
          continue;
       }
*/
       if ( ch->pcdata->timer[gsn_blast] > 0 )
	ch->pcdata->timer[gsn_blast]--;
       for ( sn = 0; sn < top_askill; sn++ )
       {
          if ( ch->pcdata->timer2[sn] <= 0 )
          continue;
          ch->pcdata->timer2[sn]--;
//          kitimer--;
          continue;
       }
     }
     continue;
    }
    if ( kitimer < 0 )
    kitimer = 0;
    return;
}

/*
 * Add a timer to ch						-Thoric
 * Support for "call back" time delayed commands
 */


void add_timer( CHAR_DATA *ch, sh_int type, sh_int count, DO_FUN *fun, int value )
{
    TIMER *timer;

    for ( timer = ch->first_timer; timer; timer = timer->next )
	if ( timer->type == type )
	{
	   timer->count  = count;
	   timer->do_fun = fun;
	   timer->value	 = value;
	   break;
	}	
    if ( !timer )
    {
	CREATE( timer, TIMER, 1 );
	timer->count	= count;
	timer->type	= type;
	timer->do_fun	= fun;
	timer->value	= value;
	LINK( timer, ch->first_timer, ch->last_timer, next, prev );
    }
}

TIMER *get_timerptr( CHAR_DATA *ch, sh_int type )
{
    TIMER *timer;

    for ( timer = ch->first_timer; timer; timer = timer->next )
      if ( timer->type == type )
        return timer;
    return NULL;
}

sh_int get_timer( CHAR_DATA *ch, sh_int type )
{
    TIMER *timer;

    if ( (timer = get_timerptr( ch, type )) != NULL )
      return timer->count;
    else
      return 0;
}

void extract_timer( CHAR_DATA *ch, TIMER *timer )
{
    if ( !timer )
    {
	bug( "extract_timer: NULL timer", 0 );
	return;
    }

    UNLINK( timer, ch->first_timer, ch->last_timer, next, prev );
    DISPOSE( timer );
    return;
}

void remove_timer( CHAR_DATA *ch, sh_int type )
{
    TIMER *timer;

    for ( timer = ch->first_timer; timer; timer = timer->next )
       if ( timer->type == type )
         break;

    if ( timer )
      extract_timer( ch, timer );
}

bool in_soft_range( CHAR_DATA *ch, AREA_DATA *tarea )
{
  if ( IS_IMMORTAL(ch) )
    return TRUE;
  else
  if ( IS_NPC(ch) )
    return TRUE;
  else
  if ( ch->level >= tarea->low_soft_range || ch->level <= tarea->hi_soft_range )
    return TRUE;
  else
    return FALSE;
}

bool can_astral( CHAR_DATA *ch, CHAR_DATA *victim )
{
  if ( victim == ch
  ||   !victim->in_room
  ||   IS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
  ||   IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
  ||   IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL )
  ||   IS_SET( victim->in_room->room_flags, ROOM_DEATH )
  ||   IS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
  ||   IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
  ||   victim->level >= ch->level + 15
  || ( CAN_PKILL( victim ) && !IS_NPC( ch ) && !CAN_PKILL( ch ) )
  || ( IS_NPC( victim ) && xIS_SET(victim->act, ACT_PROTOTYPE) )
  || ( IS_NPC( victim ) && saves_spell_staff( ch->level, victim ) )
  || ( IS_SET( victim->in_room->area->flags, AFLAG_NOPKILL )
  &&   IS_PKILL( ch ) ))
    return FALSE;
  else
    return TRUE;
}
  
bool in_hard_range( CHAR_DATA *ch, AREA_DATA *tarea )
{
  if ( IS_IMMORTAL(ch) )
    return TRUE;
  else
  if ( IS_NPC(ch) )
    return TRUE;
  else
  if ( ch->level >= tarea->low_hard_range && ch->level <= tarea->hi_hard_range )
    return TRUE;
  else
    return FALSE;
}


/*
 * Scryn, standard luck check 2/2/96
 */
bool chance( CHAR_DATA *ch, sh_int percent ) 
{

  if ( (number_percent() - get_curr_lck(ch) + 13) <= percent )
    return TRUE;
  else
    return FALSE;
}

bool chance_attrib( CHAR_DATA *ch, sh_int percent, sh_int attrib )
{
/* Scryn, standard luck check + consideration of 1 attrib 2/2/96*/ 
  sh_int  deity_factor;

  if (!ch)
  {
    bug("Chance: null ch!", 0);
    return FALSE;
  }

    deity_factor = 0;

  if (number_percent() - get_curr_lck(ch) + 13 - attrib + 13 + deity_factor <= percent )
    return TRUE;
  else
    return FALSE;

}


/*
 * Make a simple clone of an object (no extras...yet)		-Thoric
 */
OBJ_DATA *clone_object( OBJ_DATA *obj )
{
    OBJ_DATA *clone;

    CREATE( clone, OBJ_DATA, 1 );
    clone->pIndexData	= obj->pIndexData;
    clone->name		= QUICKLINK( obj->name );
    clone->short_descr	= QUICKLINK( obj->short_descr );
    clone->description	= QUICKLINK( obj->description );
    clone->action_desc	= QUICKLINK( obj->action_desc );
    clone->item_type	= obj->item_type;
    clone->extra_flags	= obj->extra_flags;
    clone->magic_flags	= obj->magic_flags;
    clone->wear_flags	= obj->wear_flags;
    clone->wear_loc	= obj->wear_loc;
    clone->weight	= obj->weight;
    clone->cost		= obj->cost;
    clone->level	= obj->level;
    clone->timer	= obj->timer;
    clone->map = obj->map;
    clone->x = obj->x;
    clone->y = obj->y;
    clone->value[0]	= obj->value[0];
    clone->value[1]	= obj->value[1];
    clone->value[2]	= obj->value[2];
    clone->value[3]	= obj->value[3];
    clone->value[4]	= obj->value[4];
    clone->value[5]	= obj->value[5];
    clone->count	= 1;
    ++obj->pIndexData->count;
    ++numobjsloaded;
    ++physicalobjects;
    cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1), 1);
    clone->serial = clone->pIndexData->serial = cur_obj_serial;
    LINK( clone, first_object, last_object, next, prev );
    return clone;
}

/*
 * If possible group obj2 into obj1				-Thoric
 * This code, along with clone_object, obj->count, and special support
 * for it implemented throughout handler.c and save.c should show improved
 * performance on MUDs with players that hoard tons of potions and scrolls
 * as this will allow them to be grouped together both in memory, and in
 * the player files.
 */
OBJ_DATA *group_object( OBJ_DATA *obj1, OBJ_DATA *obj2 )
{
    if ( !obj1 || !obj2 )
	return NULL;
    if ( obj1 == obj2 )
	return obj1;

    if ( obj1->pIndexData == obj2->pIndexData
/*
    &&	!obj1->pIndexData->mudprogs
    &&  !obj2->pIndexData->mudprogs
*/
       && !str_cmp( obj1->name, obj2->name )
       && !str_cmp( obj1->short_descr, obj2->short_descr )
       && !str_cmp( obj1->description, obj2->description )
       && !str_cmp( obj1->action_desc, obj2->action_desc )
    &&   obj1->item_type	== obj2->item_type
    &&   xSAME_BITS(obj1->extra_flags, obj2->extra_flags)
    &&   obj1->magic_flags	== obj2->magic_flags
    &&   obj1->wear_flags	== obj2->wear_flags
    &&   obj1->wear_loc		== obj2->wear_loc
    &&	 obj1->weight		== obj2->weight
    &&	 obj1->cost		== obj2->cost
    &&   obj1->level		== obj2->level
    &&   obj1->timer		== obj2->timer
    &&	 obj1->value[0]		== obj2->value[0]
    &&	 obj1->value[1]		== obj2->value[1]
    &&	 obj1->value[2]		== obj2->value[2]
    &&	 obj1->value[3]		== obj2->value[3]
    &&	 obj1->value[4]		== obj2->value[4]
    &&	 obj1->value[5]		== obj2->value[5]
    &&	!obj1->first_extradesc  && !obj2->first_extradesc
    &&  !obj1->first_content	&& !obj2->first_content
    && obj1->map == obj2->map && obj1->x == obj2->x && obj1->y == obj2->y
    &&	 obj1->count + obj2->count > 0 ) /* prevent count overflow */
    {
	obj1->count += obj2->count;
	obj1->pIndexData->count += obj2->count;	/* to be decremented in */
	numobjsloaded += obj2->count;		/* extract_obj */
	extract_obj( obj2 );
	return obj1;
    }
    return obj2;
}

/*
 * Split off a grouped object					-Thoric
 * decreased obj's count to num, and creates a new object containing the rest
 */
void split_obj( OBJ_DATA *obj, int num )
{
    int count = obj->count;
    OBJ_DATA *rest;

    if ( count <= num || num == 0 )
      return;

    rest = clone_object(obj);
    --obj->pIndexData->count;	/* since clone_object() ups this value */
    --numobjsloaded;
    rest->count = obj->count - num;
    obj->count = num;

    if ( obj->carried_by )
    {
	LINK( rest, obj->carried_by->first_carrying,
		   obj->carried_by->last_carrying,
		   next_content, prev_content );
	rest->carried_by	 	= obj->carried_by;
	rest->in_room	 		= NULL;
	rest->in_obj	 		= NULL;
    }
    else
    if ( obj->in_room )
    {
	LINK( rest, obj->in_room->first_content, obj->in_room->last_content,
		    next_content, prev_content );
	rest->carried_by	 	= NULL;
	rest->in_room	 		= obj->in_room;
	rest->in_obj	 		= NULL;
    }
    else
    if ( obj->in_obj )
    {
	LINK( rest, obj->in_obj->first_content, obj->in_obj->last_content,
		    next_content, prev_content );
	rest->in_obj			 = obj->in_obj;
	rest->in_room			 = NULL;
	rest->carried_by		 = NULL;
    }
}

void separate_obj( OBJ_DATA *obj )
{
    split_obj( obj, 1 );
}

/*
 * Empty an obj's contents... optionally into another obj, or a room
 */
bool empty_obj( OBJ_DATA *obj, OBJ_DATA *destobj, ROOM_INDEX_DATA *destroom )
{
    OBJ_DATA *otmp, *otmp_next;
    CHAR_DATA *ch = obj->carried_by;
    bool movedsome = FALSE;

    if ( !obj )
    {
	bug( "empty_obj: NULL obj", 0 );
	return FALSE;
    }
    if ( destobj || (!destroom && !ch && (destobj = obj->in_obj) != NULL) )
    {
	for ( otmp = obj->first_content; otmp; otmp = otmp_next )
	{
	    otmp_next = otmp->next_content;
	    /* only keys on a keyring */
	    if ( destobj->item_type == ITEM_KEYRING && otmp->item_type != ITEM_KEY )
		continue;
	    if ( destobj->item_type == ITEM_QUIVER && otmp->item_type != ITEM_PROJECTILE )
		continue;
	    if ( (destobj->item_type == ITEM_CONTAINER || destobj->item_type == ITEM_KEYRING
	    ||    destobj->item_type == ITEM_QUIVER)
	    &&    get_real_obj_weight( otmp ) + get_real_obj_weight( destobj )
		> destobj->value[0] )
		continue;
	    obj_from_obj( otmp );
	    obj_to_obj( otmp, destobj );
	    movedsome = TRUE;
	}
	return movedsome;
    }
    if ( destroom || (!ch && (destroom = obj->in_room) != NULL) )
    {
	for ( otmp = obj->first_content; otmp; otmp = otmp_next )
	{
	    otmp_next = otmp->next_content;
	    if ( ch && HAS_PROG(otmp->pIndexData, DROP_PROG) && otmp->count > 1 ) 
	    {
		separate_obj( otmp );
		obj_from_obj( otmp );
		if ( !otmp_next )
		  otmp_next = obj->first_content;
	    }
	    else
		obj_from_obj( otmp );
	    otmp = obj_to_room( otmp, destroom, NULL );
	    if ( ch )
	    {
		oprog_drop_trigger( ch, otmp );		/* mudprogs */
		if ( char_died(ch) )
		  ch = NULL;
	    }
	    movedsome = TRUE;
	}
	return movedsome;
    }
    if ( ch )
    {
	for ( otmp = obj->first_content; otmp; otmp = otmp_next )
	{
	    otmp_next = otmp->next_content;
	    obj_from_obj( otmp );
	    obj_to_char( otmp, ch );
	    movedsome = TRUE;
	}
	return movedsome;
    }
    bug( "empty_obj: could not determine a destination for vnum %d",
	obj->pIndexData->vnum );
    return FALSE;
}


/*
 * Add gold to an area's economy				-Thoric
 */
void boost_economy( AREA_DATA *tarea, int gold )
{
    while ( gold >= 1000000000 )
    {
	++tarea->high_economy;
	gold -= 1000000000;
    }
    tarea->low_economy += gold;
    while ( tarea->low_economy >= 1000000000 )
    {
	++tarea->high_economy;
	tarea->low_economy -= 1000000000;
    }
}

/*
 * Take gold from an area's economy				-Thoric
 */
void lower_economy( AREA_DATA *tarea, int gold )
{
    while ( gold >= 1000000000 )
    {
	--tarea->high_economy;
	gold -= 1000000000;
    }
    tarea->low_economy -= gold;
    while ( tarea->low_economy < 0 )
    {
	--tarea->high_economy;
	tarea->low_economy += 1000000000;
    }
}

/*
 * Check to see if economy has at least this much gold		   -Thoric
 */
bool economy_has( AREA_DATA *tarea, int gold )
{
    int hasgold = ((tarea->high_economy > 0) ? 1 : 0) * 1000000000
		+ tarea->low_economy;

    if ( hasgold >= gold )
	return TRUE;
    return FALSE;
}

/*
 * Used in db.c when resetting a mob into an area		    -Thoric
 * Makes sure mob doesn't get more than 10% of that area's gold,
 * and reduces area economy by the amount of gold given to the mob
 */
void economize_mobgold( CHAR_DATA *mob )
{
    int gold;
    AREA_DATA *tarea;

    /* make sure it isn't way too much */
    mob->gold = UMIN( mob->gold, mob->level * mob->level * 400 );
    if ( !mob->in_room )
	return;
    tarea = mob->in_room->area;

    gold = ((tarea->high_economy > 0) ? 1 : 0) * 1000000000 + tarea->low_economy;
    mob->gold = URANGE( 0, mob->gold, gold / 10 );
    if ( mob->gold )
	lower_economy( tarea, mob->gold );
}


/*
 * Add another notch on that there belt... ;)
 * Keep track of the last so many kills by vnum			-Thoric
 */
void add_kill( CHAR_DATA *ch, CHAR_DATA *mob )
{
    int x;
//    sh_int vnum, track;
    int vnum;
    sh_int track;

    if ( IS_NPC(ch) )
    {
	bug( "add_kill: trying to add kill to npc", 0 );
	return;
    }
    if ( !IS_NPC(mob) )
    {
	bug( "add_kill: trying to add kill non-npc", 0 );
	return;
    }
    vnum = mob->pIndexData->vnum;
    track = URANGE( 2, ((ch->level+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK );
    for ( x = 0; x < track; x++ )
	if ( ch->pcdata->killed[x].vnum == vnum )
	{
	    if ( ch->pcdata->killed[x].count < 50 )
		++ch->pcdata->killed[x].count;
	    return;
	}
	else
	if ( ch->pcdata->killed[x].vnum == 0 )
	    break;
    memmove( (char *) ch->pcdata->killed+sizeof(KILLED_DATA),
		ch->pcdata->killed, (track-1) * sizeof(KILLED_DATA) );
    ch->pcdata->killed[0].vnum  = vnum;
    ch->pcdata->killed[0].count = 1;
    if ( track < MAX_KILLTRACK )
	ch->pcdata->killed[track].vnum = 0;
}

/*
 * Return how many times this player has killed this mob	-Thoric
 * Only keeps track of so many (MAX_KILLTRACK), and keeps track by vnum
 */
int times_killed( CHAR_DATA *ch, CHAR_DATA *mob )
{
    int x;
//    sh_int vnum, track;
   int vnum;
   sh_int track;

    if ( IS_NPC(ch) )
    {
	bug( "times_killed: ch is not a player", 0 );
	return 0;
    }
    if ( !IS_NPC(mob) )
    {
	bug( "add_kill: mob is not a mobile", 0 );
	return 0;
    }

    vnum = mob->pIndexData->vnum;
    track = URANGE( 2, ((ch->level+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK );
    for ( x = 0; x < track; x++ )
	if ( ch->pcdata->killed[x].vnum == vnum )
	    return ch->pcdata->killed[x].count;
	else
	if ( ch->pcdata->killed[x].vnum == 0 )
	    break;
    return 0;
}

/*
 * returns area with name matching input string
 * Last Modified : July 21, 1997
 * Fireblade
 */
AREA_DATA *get_area(char *name)
{
	AREA_DATA *pArea;
	
	if(!name)
	{
		bug("get_area: NULL input string.");
		return NULL;
	}
	
	for(pArea = first_area; pArea; pArea = pArea->next)
	{
		if(nifty_is_name(name, pArea->name))
			break;
	}
	
	if(!pArea)
	{
		for(pArea = first_build; pArea; pArea = pArea->next)
		{
			if(nifty_is_name(name, pArea->name))
				break;
		}
	}
	
	return pArea;
}

AREA_DATA * get_area_obj ( OBJ_INDEX_DATA *pObjIndex )
{
  AREA_DATA *pArea;

  if ( !pObjIndex )
  {
  	bug("get_area_obj: pObjIndex is NULL.");
	return NULL;
  }
  for ( pArea = first_area; pArea; pArea = pArea->next )
  {
   	if ( pObjIndex->vnum >= pArea->low_o_vnum && pObjIndex->vnum <=
		pArea->hi_o_vnum )
		break;
  }	
  return pArea;
}

/*double Abs(double Nbr)
{
	if( Nbr >= 0 )
		return Nbr;
	else
		return -Nbr;
}

double sqrt(double Nbr)
{
	double Number = Nbr / 2;
	const double Tolerance = 1.0e-7;

	do Number = (Number + Nbr / Number) / 2;
	while( Abs(Number * Number - Nbr) > Tolerance);

	return Number;
}*/

char *htmlcolor( char *src )
{
  char *newarg;
  /* This function will format color codes into <font color> strings */
  if(strlen(src) == 0)
  {
    bug("Htmlcolor: emptry string!");
    return "(err)";
  }

    newarg = STRALLOC(src);
    newarg = strrep(newarg,"<","&lt;"); 
    newarg = strrep(newarg,">","&gt;");
    newarg = strrep(newarg,"&x","</font><font color='#333333'>");
    newarg = strrep(newarg,"&r","</font><font color='#400000'>");
    newarg = strrep(newarg,"&g","</font><font color='#004000'>");
    newarg = strrep(newarg,"&O","</font><font color='#404000'>");
    newarg = strrep(newarg,"&b","</font><font color='#000040'>");
    newarg = strrep(newarg,"&p","</font><font color='#404000'>");
    newarg = strrep(newarg,"&c","</font><font color='#004040'>");
    newarg = strrep(newarg,"&w","</font><font color='#FFFFFF'>");
    newarg = strrep(newarg,"&z","</font><font color='#404040'>");
    newarg = strrep(newarg,"&R","</font><font color='#FF0000'>");
    newarg = strrep(newarg,"&G","</font><font color='#00FF00'>");
    newarg = strrep(newarg,"&Y","</font><font color='#FFFF00'>");
    newarg = strrep(newarg,"&B","</font><font color='#0000FF'>");
    newarg = strrep(newarg,"&P","</font><font color='#FF00FF'>");
    newarg = strrep(newarg,"&C","</font><font color='#00FFFF'>");
    newarg = strrep(newarg,"&W","</font><font color='#C0C0C0'>");
    newarg = strrep(newarg,"\r","");
 return STRALLOC(newarg);
}




sh_int get_curr_accuracy( CHAR_DATA *ch )
{
  sh_int value;

  value = race_mod_table[ch->race].accuracy * ( (get_curr_int(ch)/2)  + (get_curr_lck(ch)/3)) ;
  value += ch->accuracy;
 return URANGE( 5, value, 500 );
}

int can_carry_w( CHAR_DATA *ch )
{
  int value;

  value = race_mod_table[ch->race].weight * ( get_curr_str(ch) * get_curr_con(ch) );


 return URANGE( 1000, value, 85000 );
}

sh_int get_curr_evasion( CHAR_DATA *ch )
{
  sh_int value;
  OBJ_DATA *shield;

  shield = get_eq_char( ch, WEAR_SHIELD );

  value = race_mod_table[ch->race].evasion * ( get_curr_dex(ch) / 1.5 );
  if ( shield )
  value *= 0.80;
  value += ch->evasion;
 return URANGE( 5, value, 500 );
}

sh_int get_curr_critical( CHAR_DATA *ch )
{
  sh_int value;
 
  value = race_mod_table[ch->race].critical * ( get_curr_lck(ch) / 1.5 );
  value += ch->critical;
 return URANGE( 5, value, 500 );
}

sh_int get_curr_patk( CHAR_DATA *ch )
{
  sh_int value;

  value = race_mod_table[ch->race].patk * ( get_curr_str(ch) );
  value += ch->patk / 3.5;
  value += ch->patk2;

 return URANGE( 5, value, 1000 );
}

sh_int get_curr_pdef( CHAR_DATA *ch )
{
  sh_int value;

  value = race_mod_table[ch->race].pdef * ( (get_curr_str(ch) / 3) + (get_curr_con(ch) / 3));
  value += ch->pdef / 5;
  value += ch->pdef2;

 return URANGE( 5, value, 1000 );
}

sh_int get_curr_edef( CHAR_DATA *ch )
{
  sh_int value;

  value = race_mod_table[ch->race].edef * ( (get_curr_con(ch) / 3) + (get_curr_dex(ch) / 3) );
  value += ch->edef / 5;
  value += ch->edef2;

 return URANGE( 5, value, 1000 );
}

sh_int get_curr_eatk( CHAR_DATA *ch )
{
  sh_int value;

  value = race_mod_table[ch->race].eatk * ( (get_curr_con(ch) / 3) + (get_curr_str(ch) / 3) );
  value += ch->eatk / 3.5;
  value += ch->eatk2;
 return URANGE( 5, value, 1000 );
}


sh_int get_curr_health( CHAR_DATA *ch )
{
  sh_int hp;
   
  hp = (sqrt(sqrt(ch->exp)) * race_mod_table[ch->race].health) + (get_curr_con(ch) * 4);

  return URANGE( 150, hp * 0.75, 2000 );
}
