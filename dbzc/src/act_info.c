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
 *			     Informational module			    *
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
#include <sys/stat.h>
#ifdef CYGWIN
#include <crypt.h>
#endif
#include "mud.h"

/*
* Needed in the do_ignore function. -Orion
*/
bool    check_parse_name        args( ( char *name, bool newchar ) );
sh_int str_similarity( const char *astr, const char *bstr );
sh_int str_prefix_level( const char *astr, const char *bstr );
void similar_help_files(CHAR_DATA *ch, char *argument);

/*
 * Keep players from defeating examine progs -Druid
 * False = do not trigger
 * True = Trigger
 */
int get_extra( sh_int grade, sh_int value );
bool EXA_prog_trigger = TRUE;

/* Had to add unknowns because someone added new positions and didn't
 * update them.  Just a band-aid till I have time to fix it right.
 * This was found thanks to mud@mini.axcomp.com pointing it out :)
 * --Shaddai
 */

bool has_blackdb(CHAR_DATA *ch )
{
  OBJ_DATA *obj;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
      if (obj->pIndexData->vnum == 270)
         return TRUE;
      if (obj->pIndexData->vnum == 271)
         return TRUE;
      if (obj->pIndexData->vnum == 272)
         return TRUE;
      if (obj->pIndexData->vnum == 273)
         return TRUE;
      if (obj->pIndexData->vnum == 274)
         return TRUE;
      if (obj->pIndexData->vnum == 275)
         return TRUE;
      if (obj->pIndexData->vnum == 276)
         return TRUE;
    }
  return FALSE;
}

bool has_namekdb(CHAR_DATA *ch )
{
  OBJ_DATA *obj;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
      if (obj->pIndexData->vnum == 253)
         return TRUE;
      if (obj->pIndexData->vnum == 254)
         return TRUE;
      if (obj->pIndexData->vnum == 255)
         return TRUE;
      if (obj->pIndexData->vnum == 256)
         return TRUE;
      if (obj->pIndexData->vnum == 257)
         return TRUE;
      if (obj->pIndexData->vnum == 258)
         return TRUE;
      if (obj->pIndexData->vnum == 259)
         return TRUE;
    }
  return FALSE;
}

bool has_earthdb(CHAR_DATA *ch )
{
  OBJ_DATA *obj;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
      if (obj->pIndexData->vnum == 260)
         return TRUE;
      if (obj->pIndexData->vnum == 261)
         return TRUE;
      if (obj->pIndexData->vnum == 262)
         return TRUE;
      if (obj->pIndexData->vnum == 263)
         return TRUE;
      if (obj->pIndexData->vnum == 264)
         return TRUE;
      if (obj->pIndexData->vnum == 265)
         return TRUE;
      if (obj->pIndexData->vnum == 266)
         return TRUE;
    }
  return FALSE;
}



char *getmobcolor( CHAR_DATA *mobile, CHAR_DATA *ch )
{

   float          new, mob, other;

   mob = get_curr_pl2( mobile );
   other = get_curr_pl2( ch );
   new = mob / other;

   if ( new < 0.5 )
     return "&z[{b  {x&z]&w ";
   else if ( new < 1 )
     return "&z[{c  {x&z]&w ";
   else if ( new < 1.5 )
     return "&z[{g  {x&z]&w ";
   else if ( new < 2 )
     return "&z[{w  {x&z]&w ";
   else if ( new < 2.5 )
     return "&z[{O  {x&z]&w ";
   else if ( new < 3 )
     return "&z[{p  {x&z]&w ";

  return "&z[{r  {x&z]&w ";
}


char *	const	where_name	[] =
{
    "&b[&Wused as light    &b]&w ",
    "&b[&Wworn on finger   &b]&w ",
    "&b[&Wworn on finger   &b]&w ",
    "&b[&Wworn around neck &b]&w ",
    "&b[&Wworn around neck &b]&w ",
    "&b[&Wworn on body     &b]&w ",
    "&b[&Wworn on head     &b]&w ",
    "&b[&Wworn on legs     &b]&w ",
    "&b[&Wworn on feet     &b]&w ",
    "&b[&Wworn on hands    &b]&w ",
    "&b[&Wworn on arms     &b]&w ",
    "&b[&Wworn as shield   &b]&w ",
    "&b[&Wworn about body  &b]&w ",
    "&b[&Wworn about waist &b]&w ",
    "&b[&Wworn around wrist&b]&w ",
    "&b[&Wworn around wrist&b]&w ",
    "&b[&Wwielded          &b]&w ",
    "&b[&Wheld             &b]&w ",
    "&b[&Wdual wielded     &b]&w ",
    "&b[&Wworn on ears     &b]&w ",
    "&b[&Wworn on eyes     &b]&w ",
    "&b[&Wmissile wielded  &b]&w ",
    "&b[&Wworn on back     &b]&w ",
    "&b[&Wworn over face   &b]&w ",
    "&b[&Wworn around ankle&b]&w ",
    "&b[&Wworn around ankle&b]&w ",
    "&b[&Wfitted into slot1&b]&w ",
    "&b[&Wfitted into slot2&b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",
    "&b[&Wwielded on left  &b]&w ",
    "&b[&Wwielded on right &b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",
    "&b[&WBUG Inform Sev   &b]&w ",


};


/*
 * Local functions.
 */
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );
void    show_condition          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


char *get_weapon_name( const char *str, sh_int number )
{
    static char strcap[MAX_STRING_LENGTH];
    int i, s;

    i = s = 0;
    switch( number )
    {
	case 2:
    	 for ( i = 0; str[i] != '*'; i++ )
       	 	strcap[i] = str[i];
	 return strcap;
        case 1:
         for ( i = 0; strcap[i] != '\0'; i++ )
         strcap[i] = '\0'; 
         for ( i = 0; str[i] != '*'; i++ )
         continue;
         i++;
         s = 0;
         for ( i = i; str[i] != '\0'; i++ ) 
         {
           strcap[s] = str[i];
	   s++;
         }
         return strcap;
    }
    return "Null";
}


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];
    bool glowsee = FALSE;

    /* can see glowing invis items in the dark */
    if ( IS_OBJ_STAT(obj, ITEM_GLOW) && IS_OBJ_STAT(obj, ITEM_INVIS))
	glowsee = TRUE;
   
    buf[0] = '\0';
    if ( (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_JEWELRY || obj->item_type == ITEM_WEAPON) && obj->value[4] > 0 )
      sprintf( buf, "&Y+%d ", obj->value[4]);
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "&z(&YInvis&z)&R "     );

    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming Red) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming Grey) "  );
    if ( ch->class==CLASS_PALADIN
	 && (!IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming White) "  );


    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Red-Grey) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Red-White) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Grey-White) "  );

    if ( !glowsee && IS_OBJ_STAT(obj, ITEM_GLOW) )   strcat( buf, "&r(&RGl&Oo&Yw&Oi&Rng&r)&R "   );
    if ( IS_OBJ_STAT(obj, ITEM_DARK)      )   strcat( buf, "&R(Spaceship&R)");
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "&G(&wHumming&G)&R "   );
    if ( IS_OBJ_STAT(obj, ITEM_HIDDEN)	  )   strcat( buf, "&z(&wHidden&z)&R "	  );
    if ( IS_OBJ_STAT(obj, ITEM_BURIED)	  )   strcat( buf, "(Buried) "	  );
    if ( IS_IMMORTAL(ch)
	 && IS_OBJ_STAT(obj, ITEM_PROTOTYPE) ) strcat( buf, "&z(&YPROTO&z)&R "	  );

    if ( fShort )
    {
/*	if ( glowsee && !IS_IMMORTAL(ch) )
	    strcat( buf, "the faint glow of something" );
	else*/
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( glowsee )
	    strcat( buf, "You see the faint glow of something nearby." );
	if ( obj->description )
	    strcat( buf, obj->description );
    }

    return buf;
}


/*
 * Some increasingly freaky hallucinated objects		-Thoric
 * (Hats off to Albert Hoffman's "problem child")
 */
char *hallucinated_object( int ms, bool fShort )
{
    int sms = URANGE( 1, (ms+10)/5, 20 );

    if ( fShort )
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "a sword";
	case  2: return "a stick";
	case  3: return "something shiny";
	case  4: return "something";
	case  5: return "something interesting";
	case  6: return "something colorful";
	case  7: return "something that looks cool";
	case  8: return "a nifty thing";
	case  9: return "a cloak of flowing colors";
	case 10: return "a mystical flaming sword";
	case 11: return "a swarm of insects";
	case 12: return "a deathbane";
	case 13: return "a figment of your imagination";
	case 14: return "your gravestone";
	case 15: return "the long lost boots of Ranger Thoric";
	case 16: return "a glowing tome of arcane knowledge";
	case 17: return "a long sought secret";
	case 18: return "the meaning of it all";
	case 19: return "the answer";
	case 20: return "the key to life, the universe and everything";
    }
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "A nice looking sword catches your eye.";
	case  2: return "The ground is covered in small sticks.";
	case  3: return "Something shiny catches your eye.";
	case  4: return "Something catches your attention.";
	case  5: return "Something interesting catches your eye.";
	case  6: return "Something colorful flows by.";
	case  7: return "Something that looks cool calls out to you.";
	case  8: return "A nifty thing of great importance stands here.";
	case  9: return "A cloak of flowing colors asks you to wear it.";
	case 10: return "A mystical flaming sword awaits your grasp.";
	case 11: return "A swarm of insects buzzes in your face!";
	case 12: return "The extremely rare Deathbane lies at your feet.";
	case 13: return "A figment of your imagination is at your command.";
	case 14: return "You notice a gravestone here... upon closer examination, it reads your name.";
	case 15: return "The long lost boots of Ranger Thoric lie off to the side.";
	case 16: return "A glowing tome of arcane knowledge hovers in the air before you.";
	case 17: return "A long sought secret of all mankind is now clear to you.";
	case 18: return "The meaning of it all, so simple, so clear... of course!";
	case 19: return "The answer.  One.  It's always been One.";
	case 20: return "The key to life, the universe and everything awaits your hand.";
    }
    return "Whoa!!!";
}


/* This is the punct snippet from Desden el Chaman Tibetano - Nov 1998
   Email: jlalbatros@mx2.redestb.es
*/
char *num_punct( int foo )
{
   int index_new, rest, x;
   unsigned int nindex;
   char buf[16];
   static char buf_new[16];

   sprintf( buf, "%d", foo );
   rest = strlen( buf ) % 3;

   for( nindex = index_new = 0; nindex < strlen( buf ); nindex++, index_new++ )
   {
      x = nindex - rest;
      if( nindex != 0 && ( x % 3 ) == 0 )
      {
         buf_new[index_new] = ',';
         index_new++;
         buf_new[index_new] = buf[nindex];
      }
      else
         buf_new[index_new] = buf[nindex];
   }
   buf_new[index_new] = '\0';
   return buf_new;
}
/*

char *num_punct(int foo)
{
    int index, index_new, rest;
    char buf[16];
    static char buf_new[16];

    sprintf(buf,"%d",foo);
    rest = strlen(buf)%3;

    for (index=index_new=0;index<strlen(buf);index++,index_new++)
    {
	if (index!=0 && (index-rest)%3==0 )
	{
	    buf_new[index_new]=',';
	    index_new++;
	    buf_new[index_new]=buf[index];
        }
        else
	    buf_new[index_new] = buf[index];
    }
    buf_new[index_new]='\0';
    return buf_new;
}
*/


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char **prgpstrShow;
    int *prgnShow;
    int *pitShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count, offcount, tmp, ms, cnt;
    bool fCombine;

    if ( !ch->desc )
	return;

    /*
     * if there's no list... then don't do all this crap!  -Thoric
     */
    if ( !list )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   send_to_char( "&RNothing.\n\r", ch );
	}
	return;
    }
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
	count++;

    ms  = ch->drunk;

    /*
     * If not mentally stable...
     */
    if ( abs(ms) > 40 )
    {
	offcount = URANGE( -(count), (count * ms) / 100, count*2 );
	if ( offcount < 0 )
	  offcount += number_range(0, abs(offcount));
	else
	if ( offcount > 0 )
	  offcount -= number_range(0, offcount);
    }
    else
	offcount = 0;

    if ( count + offcount <= 0 )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   send_to_char( "&RNothing.\n\r", ch );
	}
	return;
    }

    CREATE( prgpstrShow,	char*,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( prgnShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( pitShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    nShow	= 0;
    tmp		= (offcount > 0) ? offcount : 0;
    cnt		= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( offcount < 0 && ++cnt > (count + offcount) )
	    break;
	if ( tmp > 0 && number_bits(1) == 0 )
	{
	    prgpstrShow [nShow] = str_dup( hallucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	    --tmp;
	}
	if ( obj->wear_loc == WEAR_NONE
	&& can_see_obj( ch, obj )
	&& (obj->item_type != ITEM_TRAP ))
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow] += obj->count;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    pitShow[nShow] = obj->item_type;
	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = obj->count;
		nShow++;
	    }
	}
    }
    if ( tmp > 0 )
    {
	int x;
	for ( x = 0; x < tmp; x++ )
	{
	    prgpstrShow [nShow] = str_dup( hallucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	}
    }

    /*
     * Output the formatted list.		-Color support by Thoric
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	switch(pitShow[iShow]) {
	default:
	  set_char_color( AT_OBJECT, ch );
	  break;
	case ITEM_BLOOD:
	  set_char_color( AT_BLOOD, ch );
	  break;
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
          set_char_color( AT_CORPSE, ch );
          break;
	case ITEM_MONEY:
	case ITEM_TREASURE:
	  set_char_color( AT_YELLOW, ch );
	  break;
	case ITEM_COOK:
	case ITEM_FOOD:
	  set_char_color( AT_RED, ch );
	  break;
	case ITEM_DRINK_CON:
	case ITEM_FOUNTAIN:
	  set_char_color( AT_THIRSTY, ch );
	  break;
	case ITEM_FIRE:
	  set_char_color( AT_FIRE, ch );
	  break;
	case ITEM_SCROLL:
	case ITEM_WAND:
	case ITEM_STAFF:
	  set_char_color( AT_MAGIC, ch );
	  break;
	}
	if ( fShowNothing )
	    send_to_char( "     ", ch );
	send_to_char( prgpstrShow[iShow], ch );
/*	if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) ) */
	{
	    if ( prgnShow[iShow] != 1 )
		ch_printf( ch, " (%d)", prgnShow[iShow] );
	}

	send_to_char( "\n\r", ch );
	DISPOSE( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "&RNothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    DISPOSE( prgpstrShow );
    DISPOSE( prgnShow	 );
    DISPOSE( pitShow	 );
    return;
}


/*
 * Show fancy descriptions for certain spell affects		-Thoric
 */
void show_visible_affects_to_char( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char name[MAX_STRING_LENGTH];

    if ( IS_NPC( victim ) )
      strcpy( name, victim->short_descr );
    else
      strcpy( name, victim->name);
    name[0] = toupper(name[0]);

     if (xIS_SET(victim->powerup, PU_STEAM))
     {
	ch_printf( ch, "&W%s has steam surrounding them and a look of rage in %s eyes!&w\r\n", name, (victim->sex == 1)? "his" :
		(victim->sex == 2)? "her" : "its");
     }
     if (xIS_SET(victim->powerup, PU_USSJ))
     {
       ch_printf( ch, "&Y%s has bulked up muscles and an enourmous aura.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_SSJ1))
     {
       ch_printf( ch, "&Y%s glows with a golden aura.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_SUPERHUMAN))
     {
       ch_printf( ch, "&z%s is flaming with a silver aura.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_ULTRA))
     {
       ch_printf( ch, "&g%s is transformed into an ULTRA Namek.&w\n\r", name );
     }

     if (xIS_SET(victim->powerup, PU_METAL))
     {
       ch_printf( ch, "&W%s is covered in an icy metallic armor.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_GOLDEN))
     {
       ch_printf( ch, "&Y%s is turned into a HUGE GOLDEN APE!.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_OOZARU) && !xIS_SET(victim->powerup, PU_GOLDEN))
     {
       ch_printf( ch, "&Y%s is turned into a HUGE APE!.&w\n\r", name );
     }

     if (xIS_SET(victim->powerup, PU_SSJ2))
     {
       ch_printf( ch, "&Y%s is engulfed in a golden aura with crackling electricity.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_SSJ3))
     {
       ch_printf( ch, "&Y%s has long golden hair and is radiating extreme power!&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_SSJ4))
     {
       ch_printf( ch, "&R%s is covered in red fur, and emenating incredible power!&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, PU_KAIOKEN))
     {
       ch_printf( ch, "&R%s is flaming with a red and white aura.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_SPHERE))
     {
       ch_printf( ch, "&RA Ki Sphere surrounds %s.&w\n\r", name );            
     }
     if (xIS_SET(victim->powerup, KI_BOMB))
     {
       ch_printf( ch, "&zA Ki Bomb surrounds %s.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_DONUT))
     {
       ch_printf( ch, "&YA Galactic Donut surrounds %s.&w\n\r", name );
     }

     if (xIS_SET(victim->powerup, KI_EBALL))
     {
       ch_printf( ch, "&YAn Energy Ball controlled by %s.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_DDD))
     {
       ch_printf( ch, "&R%s wields a red Destructo Disk in each hand.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_DISK))
     {
       ch_printf( ch, "&R%s is wielding a red Destructo Disk.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_BAR))
     {
       ch_printf( ch, "&RAn Energy Barrier is surrounding %s.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_SHIELD))
     {
       ch_printf( ch, "&GAn Energy Shield is surrounding %s.&w\n\r", name );
     }
     if (xIS_SET(victim->powerup, KI_BARRICADE))
     {
       ch_printf( ch, "&OAn Energy Barricade is surrounding %s.&w\n\r", name );
     }

}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    buf[0] = '\0';

    set_char_color( AT_PERSON, ch );
    if ( get_timer( victim, TIMER_CLOAK) > 0 && (!IS_IMMORTAL(ch) || xIS_SET(ch->act, PLR_SEMI) ) )
    return;
    if ( !IS_NPC(victim) && !victim->desc )
    {
	if ( !victim->switched )         send_to_char_color( "&P[(Link Dead)] ", ch );
	else 
						strcat( buf, "(Switched) " );
    }
    if ( !IS_NPC(victim)
    &&   xIS_SET(victim->act, PLR_AFK) )	strcat( buf, "&z[&WAFK&z]&Y ");        

    if ( (!IS_NPC(victim) && xIS_SET(victim->act, PLR_WIZINVIS))
      || (IS_NPC(victim) && xIS_SET(victim->act, ACT_MOBINVIS))) 
    {
        if (!IS_NPC(victim))
	sprintf( buf1,"(Invis %d) ", victim->pcdata->wizinvis );
        else sprintf( buf1,"(Mobinvis %d) ", victim->mobinvis);
        strcat( buf, buf1 );
    }
    

    if ( !IS_NPC( victim) )
    {
	if ( IS_IMMORTAL( victim ) && !xIS_SET(victim->act, PLR_SEMI))
	  send_to_char_color( "&P(&WImmortal&P)&W&w ", ch );
        else if ( xIS_SET(victim->act, PLR_SEMI))
          send_to_char_color( "&P(&WSemi-Mortal&P)&W&w ", ch );

	if ( victim->exp > 5000000 && !victim->pcdata->clan && !IS_IMMORTAL(victim) )
              send_to_char_color( "&P(&wUnclanned&P)&W&w ", ch );
        else
         send_to_char_color("&W&w", ch );
    }
    set_char_color( AT_PERSON, ch );
       if (!IS_NPC(ch)) strcat( buf, "&w" );       

     if (!IS_NPC(ch))
     {
       if (can_use_skill(ch, number_percent(), gsn_truesense ))
       {
       if (victim->alignment > 900)  strcat(buf, "&z(&G|||&z)&w  ");
       else if (victim->alignment > 700) strcat(buf, "&z(&G||&w|&z)&w  ");
       else if (victim->alignment > 350) strcat(buf, "&z(&G|&w||&z)&w  ");
       else if (victim->alignment > 100) strcat(buf, "&z(&w|||&z)&w  ");
       else if (victim->alignment > -100) strcat(buf, "&z(&w|||&z)&w  ");
       else if (victim->alignment > -350) strcat(buf, "&z(&R|&w||&z)&w  ");
       else if (victim->alignment > -700) strcat(buf, "&z(&R||&w|&z)&w  ");
       else if (victim->alignment > -1001) strcat(buf, "&z(&R|||&z)&w  ");

       learn_from_success( ch, gsn_truesense );
       }
       else
       {
       learn_from_failure( ch, gsn_truesense );
       }
    }
       if (!IS_NPC(ch)) strcat( buf, "&w" );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_OUTLAW ) )
                                                strcat( buf, "&R(OUTLAW)&w "     );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_ATTACKER ) )
						strcat( buf, "&R(ATTACKER)&w "   );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_KILLER ) )
						strcat( buf, "&R(KILLER)&w "     );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "&R(THIEF)&w "      );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_LITTERBUG  ) )
						strcat( buf, "&R(LITTERBUG)&w "  );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_FUSED ) )
                                                strcat( buf, "&G(&WFused&G)&w " );
    if ( IS_NPC(victim) && IS_IMMORTAL(ch)
	 && xIS_SET(victim->act, ACT_PROTOTYPE) )strcat( buf, "&z(&YPROTO&z)&w "      );
    if ( IS_NPC(victim) && ch->mount && ch->mount == victim
         && ch->in_room == ch->mount->in_room ) strcat( buf, "&G(Mount)&w "      );
    if ( victim->desc && victim->desc->connected == CON_EDITING )
						strcat( buf, "&G(Writing)&w "    );

    set_char_color( AT_PERSON, ch );


       if ( IS_NPC(victim))
       strcat( buf, getmobcolor( victim, ch ) );

  if ((victim->position == victim->defposition && victim->long_descr[0] != '\0'))
  {

        strcat (buf, victim->long_descr);
	send_to_char( buf, ch );
	show_visible_affects_to_char( victim, ch );
	return;
  }
  else if ( xIS_SET(victim->act, PLR_FUSED) && victim->fusionname && !IS_NPC(victim) )
  {
   strcat( buf, victim->fusionname );
  }
  else
   strcat( buf, PERS( victim, ch, FALSE ) );

   strcat( buf, "&w" );
 
    if ( !IS_NPC(victim) && !xIS_SET(ch->act, PLR_BRIEF) )
	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );			break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );		break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );		break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." );	break;
    case POS_SLEEPING:
        if (ch->position == POS_SITTING
        ||  ch->position == POS_RESTING )
            strcat( buf, " is sleeping nearby." );
	else
            strcat( buf, " is deep in slumber here." );
        break;
    case POS_RESTING:
        if (ch->position == POS_RESTING)
            strcat ( buf, " is sprawled out alongside you." );
        else
	if (ch->position == POS_MOUNTED)
	    strcat ( buf, " is sprawled out at the foot of your mount." );
	else
            strcat (buf, " is sprawled out here." );
        break;
    case POS_SITTING:
        if (ch->position == POS_SITTING)
            strcat( buf, " sits here with you." );
        else
        if (ch->position == POS_RESTING)
            strcat( buf, " sits nearby as you lie around." );
        else
            strcat( buf, " sits upright here." );
        break;
    case POS_STANDING:
	if ( IS_IMMORTAL(victim) )
            strcat( buf, " is here before you." );
	else
        if ( ( victim->in_room->sector_type == SECT_UNDERWATER )
        && !IS_NPC(victim) )
            strcat( buf, " is drowning here." );
	else
	if ( victim->in_room->sector_type == SECT_UNDERWATER )
            strcat( buf, " is here in the water." );
	else
	if ( ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	&& !IS_NPC(victim) )
	    strcat( buf, " is drowning here." );
	else
	if ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	    strcat( buf, " is standing here in the water." );
	else
          strcat( buf, " is standing here." );
        break;
    case POS_SHOVE:    strcat( buf, " is being shoved around." );	break;
    case POS_DRAG:     strcat( buf, " is being dragged around." );	break;
    case POS_MOUNTED:
	strcat( buf, " is here, upon " );
	if ( !victim->mount )
	    strcat( buf, "thin air???" );
	else
	if ( victim->mount == ch )
	    strcat( buf, "your back." );
	else
	if ( victim->in_room == victim->mount->in_room )
	{
	    strcat( buf, PERS( victim->mount, ch, FALSE ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    case POS_FIGHTING:
        if (xIS_SET(victim->act, PLR_SPAR))
        strcat( buf, " is here, sparring " );
        else
	strcat( buf, " is here, fighting " );
	if ( !victim->fighting ) 
        {
	    strcat( buf, "thin air???" );

            /* some bug somewhere.... kinda hackey fix -h */
            if(! victim->mount)
               victim->position = POS_STANDING;
            else
               victim->position = POS_MOUNTED;
        }
	else if ( who_fighting( victim ) == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->who->in_room )
	{
	    strcat( buf, PERS( victim->fighting->who, ch, FALSE ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char buf[MSL];
    int iWear;
    bool found;

    if ( can_see( victim, ch, FALSE ) && !IS_NPC( ch ) && !xIS_SET( ch->act, PLR_WIZINVIS && !xIS_SET( ch->act, PLR_SPEC  ) ))
    {
      act( AT_ACTION, "$n looks at you.", ch, NULL, victim, TO_VICT    );
      if ( victim != ch )
	act( AT_ACTION, "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
      else
        act( AT_ACTION, "$n looks at $mself.", ch, NULL, victim, TO_NOTVICT );
    }

    if ( victim->description[0] != '\0' )
    send_to_char (victim->description, ch);
    else
    {
	if ( IS_NPC( victim ) )
	  act( AT_PLAIN, "\n\rYou see nothing special about $M.", ch, NULL, victim, TO_CHAR );
	else if ( ch != victim )
        {
          sprintf( buf, "\n\r%s isn't much to look at...",victim->sex == 0 ? "It" : victim->sex == 2 ? "She" : "He");
	  act( AT_PLAIN, buf, ch, NULL, victim, TO_CHAR );
        }
	else if ( ch == victim )
	  act( AT_PLAIN, "\n\rYou're not much to look at...", ch, NULL, NULL, TO_CHAR );
    }

    show_race_line( ch, victim );
    show_condition( ch, victim );

    if (xIS_SET(victim->powerup, PU_STEAM))
    {
	if( ch != victim)
		act(AT_WHITE, "$E is surrounded by steam and has a look of rage!", ch, NULL, victim, TO_CHAR);
	else
		act(AT_WHITE, "You are surrounded by steam and have a look of rage!", ch, NULL, NULL, TO_CHAR);
    }
     if (xIS_SET(victim->powerup, PU_KAIOKEN))
     {
        if ( ch != victim )
        act( AT_GREEN, "$E is glowing with a red and white flaming aura!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_GREEN, "You are glowing with a red and white flaming aura!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_ADULT))
     {
        if ( ch != victim )
        act( AT_RED, "$E is in $S adult form.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are in your adult form.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_SEMIP))
     {
        if ( ch != victim )
        act( AT_RED, "$E is in $S Semi-Perfect Stadium.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are in your Semi-Perfect Stadium.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_PERFECT))
     {
        if ( ch != victim )
        act( AT_RED, "$E is in $S Perfect form.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are in your Perfect form.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_USSJ))
     {
        if ( ch != victim )
        act( AT_YELLOW, "$E is an ULTRA Super Saiya-Jin.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_YELLOW, "You are an ULTRA Super Saiya-Jin.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_GOLDEN))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is turned into a HUGE GOLDEN APE!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are turned into a HUGE GOLDEN APE!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_OOZARU) && !xIS_SET(victim->powerup, PU_GOLDEN))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is turned into a HUGE APE!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are turned into a HUGE APE!&w", ch, NULL, NULL, TO_CHAR );
     }

     if (xIS_SET(victim->powerup, PU_METAL))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is covered in an icy metallic armor.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are covered in an icy metallic armor.&w", ch, NULL, NULL, TO_CHAR );
     }

     if (xIS_SET(victim->powerup, PU_SUPERHUMAN))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is flaming with a silver aura.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are flaming with a silver aura.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_ULTRA))
     {
        if ( ch != victim )
        act( AT_DGREEN, "$E is transformed into an ULTRA Namek.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_DGREEN, "You is transformed into an ULTRA Namek.&w", ch, NULL, NULL, TO_CHAR );
     }



     if (xIS_SET(victim->powerup, PU_SSJ1))
     {
        if ( ch != victim )
        act( AT_YELLOW, "$E is a mighty Super Saiya-Jin.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_YELLOW, "You are a mighty Super Saiya-Jin.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_SSJ2))
     {
        if ( ch != victim )
        act( AT_YELLOW, "$E is a very mighty Super Saiya-Jin 2!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_YELLOW, "You are a very mighty Super Saiya-Jin 2!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_SSJ3))
     {
        if ( ch != victim )
        act( AT_YELLOW, "$E is a vicious Super Saiya-Jin 3!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_YELLOW, "You are a vicious Super Saiya-Jin 3!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_SSJ4))
     {
        if ( ch != victim )
        act( AT_RED, "$E is a ultimate Super Saiya-Jin 4!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are a ultimate Super Saiya-Jin 4!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_MUTATE2))
     {
        if ( ch != victim )
        act( AT_GREEN, "$E is a mutant mutated into $S second form!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_GREEN, "You are a mutant mutated into your second form!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_MUTATE3))
     {
        if ( ch != victim )
        act( AT_GREEN, "$E is a powerful mutant mutated into $S third form!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_GREEN, "You are a powerful mutant mutated into your third form!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_MUTATE4))
     {
        if ( ch != victim )
        act( AT_GREEN, "$E is a vicious mutant mutated into $S fourth form!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_GREEN, "You are a vicious mutant mutated into your fourth form!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_MONSTER))
     {
        if ( ch != victim )
        act( AT_RED, "$E is mutated in $S Monster Form!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are mutated in your Monster Form!&w", ch, NULL, NULL, TO_CHAR );
     }


     if (xIS_SET(victim->powerup, PU_MYSTIC))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is glowing with a large clear aura!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are glowing with a large clear aura!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_TURBO))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is a super-human engulfed in an enourmous white aura!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are a super-human engulfed in an enormous white aura!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_TRANSFORM2))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is transformed into stage 2!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are transformed into stage 2!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_TRANSFORM3))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is transformed into stage 3!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are transformed into stage 3!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_TRANSFORM4))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is transformed into stage 4!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are transformed into stage 4!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, PU_TRANSFORM5))
     {
        if ( ch != victim )
        act( AT_WHITE, "$E is transformed into stage 5!&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_WHITE, "You are transformed into stage 5!&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->act, PLR_TAIL))
     {
        if ( ch != victim )
          act( AT_GREEN, "$E has a tail.&w", ch, NULL, victim, TO_CHAR );
        else
          act( AT_GREEN, "You have a tail.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_EBALL))
     {
        if ( ch != victim )
          act( AT_RED, "$E is controlling an Energy Ball.&w", ch, NULL, victim, TO_CHAR );
        else
          act( AT_RED, "You are controlling an Energy Ball.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_SPHERE))
     {
        if ( ch != victim )
          act( AT_RED, "$E is wielding a Ki Sphere.&w", ch, NULL, victim, TO_CHAR );
        else
          act( AT_RED, "You are wielding a Ki Sphere.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_BOMB))
     {
        if ( ch != victim )
          act( AT_RED, "&z$E is wielding a Ki Bomb.&w", ch, NULL, victim, TO_CHAR );
        else
          act( AT_RED, "&zYou are wielding a Ki Bomb.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_DONUT))
     {
        if ( ch != victim )
          act( AT_YELLOW, "$E is wielding a Galactic Donut.&w", ch, NULL, victim, TO_CHAR );
        else
          act( AT_YELLOW, "You are wielding a Galactic Donut.&w", ch, NULL, NULL, TO_CHAR );
     }

     if (xIS_SET(victim->powerup, KI_DDD))
     {
        if ( ch != victim )
        act( AT_RED, "$E is holding an Energy Disk in each hand.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are holding an Energy Disk in each hand.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_DISK))
     {
        if ( ch != victim )
        act( AT_RED, "$E is holding an Energy Disk in $s right hand.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are holding an Energy Disk in your right hand.&w", ch, NULL, NULL, TO_CHAR );
     } 
     if (xIS_SET(victim->powerup, KI_BAR))
     {
        if ( ch != victim )
        act( AT_RED, "$E is surrounded by an Energy Barrier.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_RED, "You are surrounded by an Energy Barrier.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_SHIELD))
     {
        if ( ch != victim )
        act( AT_GREEN, "$E is surrounded by an Energy Shield.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_GREEN, "You are surrounded by an Energy Shield.&w", ch, NULL, NULL, TO_CHAR );
     }
     if (xIS_SET(victim->powerup, KI_BARRICADE))
     {
        if ( ch != victim )
        act( AT_ORANGE, "$E is surrounded by an Energy Barricade.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_ORANGE, "You are surrounded by an Energy Barricade.&w", ch, NULL, NULL, TO_CHAR );
     }

     if ( xIS_SET(victim->act, PLR_FLY))
     {
        if ( ch != victim )
        act( AT_BLUE, "$E is hovering above the ground.&w", ch, NULL, victim, TO_CHAR );
        else
        act( AT_BLUE, "You are hovering above the ground.&w", ch, NULL, NULL, TO_CHAR );
     }     
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		if ( victim != ch )
		  act( AT_PLAIN, "$N is using:", ch, NULL, victim, TO_CHAR );
		else
		  act( AT_PLAIN, "You are using:", ch, NULL, NULL, TO_CHAR );
		found = TRUE;
	    }
            if ( obj->item_type == ITEM_WEAPON && (obj->value[0] == W_DSWORD || obj->value[0] == W_DDAGGER) && obj->wear_loc == WEAR_WIELD )
            {
               send_to_char( where_name[WEAR_WIELD_L], ch );
               send_to_char(  get_weapon_name( obj->short_descr, 2 ), ch );
               send_to_char( "\n\r", ch );
               send_to_char( where_name[WEAR_WIELD_R], ch );
               send_to_char( get_weapon_name( obj->short_descr, 1 ), ch );
               send_to_char( "\n\r", ch );
            }
            else 
            {  
	      send_to_char( where_name[iWear], ch );
	      send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	      send_to_char( "\n\r", ch );
            }
	}
    }

    /*
     * Crash fix here by Thoric
     */
    if ( IS_NPC(ch) || victim == ch )
      return;

    if ( IS_IMMORTAL( ch ) )
    {
      if ( IS_NPC( victim ) )
	ch_printf( ch, "\n\rMobile #%d '%s' ",
	    victim->pIndexData->vnum,
	    victim->name );
	else
	  ch_printf( ch, "\n\r%s ", victim->name );
      ch_printf( ch, "is a level %d %s %s.\n\r",
	victim->level,
	IS_NPC(victim)?victim->race<MAX_NPC_RACE&&victim->race>=0?
	npc_race[victim->race]:"unknown":victim->race<MAX_PC_RACE&&
	race_table[victim->race]->race_name&&
	race_table[victim->race]->race_name[0] != '\0'?
	race_table[victim->race]->race_name:"unknown",
	IS_NPC(victim)?victim->class<MAX_NPC_CLASS&&victim->class>=0?
	npc_class[victim->class] : "unknown":victim->class<MAX_PC_CLASS&&
	class_table[victim->class]->who_name&&
	class_table[victim->class]->who_name[0] != '\0'?
	class_table[victim->class]->who_name:"unknown");
/* Fix so it shows what is in class table
	victim->race<MAX_NPC_RACE&&victim->race>=0?npc_race[victim->race] : "unknown",
	victim->class<MAX_NPC_CLASS&&victim->class>=0?npc_class[victim->class] : "unknown" );
*/
    }

    if ( IS_IMMORTAL(ch) )
    {
	ch_printf( ch, "\n\rYou peek at %s inventory:\n\r",
	  victim->sex == 1 ? "his" : victim->sex == 2 ? "her" : "its" );
	show_list_to_char( victim->first_carrying, ch, TRUE, TRUE );
    }

    return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( can_see( ch, rch, FALSE ) )
	{
	    show_char_to_char_0( rch, ch );
            show_visible_affects_to_char( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
        && !(!IS_NPC(rch) && IS_IMMORTAL(rch)) )
	{
	    set_char_color( AT_BLOOD, ch );
	    send_to_char( "The red form of a living creature is here.\n\r", ch );
	}
    }

    return;
}



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;
	

    return TRUE;
}

/*
 * Returns classical DIKU door direction based on text in arg	-Thoric
 */
int get_door( char *arg )
{
    int door;

	 if ( !str_cmp( arg, "n"  ) || !str_cmp( arg, "north"	  ) ) door = 0;
    else if ( !str_cmp( arg, "e"  ) || !str_cmp( arg, "east"	  ) ) door = 1;
    else if ( !str_cmp( arg, "s"  ) || !str_cmp( arg, "south"	  ) ) door = 2;
    else if ( !str_cmp( arg, "w"  ) || !str_cmp( arg, "west"	  ) ) door = 3;
    else if ( !str_cmp( arg, "u"  ) || !str_cmp( arg, "up"	  ) ) door = 4;
    else if ( !str_cmp( arg, "d"  ) || !str_cmp( arg, "down"	  ) ) door = 5;
    else if ( !str_cmp( arg, "ne" ) || !str_cmp( arg, "northeast" ) ) door = 6;
    else if ( !str_cmp( arg, "nw" ) || !str_cmp( arg, "northwest" ) ) door = 7;
    else if ( !str_cmp( arg, "se" ) || !str_cmp( arg, "southeast" ) ) door = 8;
    else if ( !str_cmp( arg, "sw" ) || !str_cmp( arg, "southwest" ) ) door = 9;
    else door = -1;
    return door;
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char arg  [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MIL];
    char buf2[MIL];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *original;
    char *pdesc;
    sh_int door;
    int number, cnt;
    if ( !ch->desc )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !xIS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	set_char_color( AT_DGREY, ch );
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->first_person, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
       
      if( IS_PLR_FLAG( ch, PLR_ONMAP ) || IS_ACT_FLAG( ch, ACT_ONMAP ) )
	{
	    display_map( ch );
         show_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
       	 show_char_to_char( ch->in_room->first_person,  ch );
	    return;
	}
//        ch_printf( ch, "\n\r%s", get_name_color(ch));
       if ( !xIS_SET(ch->pcdata->flags, PCFLAG_NOLINE) )
       send_to_char( "\n\r&z-----------------------------------------------------------------------------\n\r", ch);
       else
       send_to_char( "\n\r\n\r", ch);
       sprintf( buf2, "&w-&W<&z( &Y%s &z)&W>&w-", ch->in_room->name );
       ch_printf( ch, "%s", centertext( buf2, 75 ));
       if ( !xIS_SET(ch->pcdata->flags, PCFLAG_NOLINE) )
          send_to_char( "\n\r&z-----------------------------------------------------------------------------\n\r", ch);
       else
       send_to_char( "\n\r", ch);

        if (xIS_SET(ch->act, PLR_ROOMVNUM))
        ch_printf( ch, "&w[%d] ", ch->in_room->vnum, ch );

	/* Room flag display installed by Samson 12-10-97 */
	if( !IS_NPC(ch) && IS_IMMORTAL(ch) && xIS_SET( ch->pcdata->flags, PCFLAG_AUTOFLAGS ))
	{
           set_char_color( AT_WHITE, ch );
	   ch_printf( ch, "&z[&W%s&z]", flag_string( ch->in_room->area->flags, area_flags ) );
	   ch_printf( ch, "&z[&W%s&z]", flag_string( ch->in_room->room_flags, r_flags ) );
           ch_printf( ch, "&z[&W%s&z]\n\r", flag_string( ch->in_room->room_flags2, r_flags2 ) );
	}
/*
        if( !IS_NPC(ch) && IS_IMMORTAL(ch) && xIS_SET( ch->pcdata->flags, PCFLAG_SECTORD ) )
	{
	   ch_printf( ch, "&z[&W%s&z]", sec_flags[ch->in_room->sector_type] );
	}

	if( !IS_NPC(ch) && IS_IMMORTAL(ch) && xIS_SET( ch->pcdata->flags, PCFLAG_ANAME ) )
	{
           ch_printf( ch, "AName : %s \n\r", ch->in_room->area->name );
        if ( ch->level >= LEVEL_CREATOR )
           ch_printf( ch, "File  : %s \n\r", ch->in_room->area->filename );
        else
	    send_to_char( "\n\r", ch );
         }	
*/
	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !xIS_SET(ch->act, PLR_BRIEF) ) )
        {
          ship = ship_from_entrance(ch->in_room->vnum);

          if ( ship != NULL )
          {
            if ( IN_CONTROL(ch) && ship->name)
            ch_printf(ch, "&Y[Spaceship '%s' Control Room]\n\r", ship->name);
          }
          set_char_color( AT_DGREEN, ch);
          send_to_char( ch->in_room->description, ch );
       if ( !xIS_SET(ch->pcdata->flags, PCFLAG_NOLINE) )
          send_to_char( "&z-----------------------------------------------------------------------------\n\r", ch);
       else
       send_to_char( "\n\r", ch);

        }

        if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOMAP) )
            do_showmap( ch, "" );
	else if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOEXIT) )
	    do_exits( ch, "auto" );


	show_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->first_person,  ch );
	return;
    }
   if ( !str_cmp( arg1, "sky" ) )
   {
     if ( !IS_OUTSIDE(ch) )
     {
       send_to_char( "You can't see the sky indoors.\n\r", ch );
       return;
     }
     else
     {
       do_looksky(ch, "");
       return;
     }
   }


    if ( !str_cmp( arg1, "console" ))
    {
      int parsecs; 
      int estimated;
      int dist1, dist2, dist3, dist4;

      if (!IN_CONTROL(ch))
      {
       send_to_char("You aren't in a control room", ch);
       return;
      }
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  == NULL )
      {
           send_to_char("&RYou must be in the control room of a ship to do that!\n\r",ch);
           return;
      }
 
            dist1 = ship->x1 - ship->x2;
            dist2 = ship->y1 - ship->y2;
            dist3 = ship->z1 - ship->z2;
            dist4 = ship->d1 - ship->d2;

            parsecs = convert_pos(dist1) + convert_pos(dist2) + convert_pos(dist3) + convert_pos(dist4);     

      estimated = (parsecs / ship->speed) * 60;
      if (estimated <= 0)
      estimated = 0;

      ch_printf(ch, "\n\r&G-=[ &w%s BOARDCOMPUTER&G ]=-", ship->name);
      sprintf( buf, "%s", get_dockname(ship->x1, ship->y1, ship->z1, ship->d1 ));
      ch_printf(ch, "\n\r&G- Current position: &w%d.%d.%d.%d (%s)", ship->x1, ship->y1, ship->z1, ship->d1, buf);
      sprintf( buf, "%s", get_dockname(ship->x2, ship->y2, ship->z2, ship->d2)  );
      ch_printf(ch, "\n\r&G- Target position : &w%d.%d.%d.%d (%s)", ship->x2, ship->y2, ship->z2, ship->d2, buf);
      ch_printf(ch, "\n\r&G- Maximum speed   : &w%d parsecs/minute", ship->speed);
      ch_printf(ch, "\n\r&G- Distance        : &w%d parsecs", parsecs + 10);
      ch_printf(ch, "\n\r&G- Status          : &w%s", ship->ship_state == 0  ? "SHUTDOWN" : ship->ship_state == 1 ? "STAND BY" : ship->ship_state == 2 ? "IN ORBIT" : "SHUTDOWN"); 
      ch_printf(ch, "\n\r&G- Flight time     : &w%d seconds estimated.", estimated + 20);
      ch_printf(ch, "\n\r&G-=[ End of Report ]=-\n\r");
      return;
    }
           
    if ( !str_cmp( arg1, "under" ) )
    {
	int count;

	/* 'look under' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look beneath what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}
	if ( !CAN_WEAR( obj, ITEM_TAKE ) && ch->level < sysdata.level_getobjnotake )
	{
	    send_to_char( "You can't seem to get a grip on it.\n\r", ch );
	    return;
	}
	if ( ch->carry_weight + obj->weight > can_carry_w( ch ) )
	{
	    send_to_char( "It's too heavy for you to look under.\n\r", ch );
	    return;
	}
	count = obj->count;
	obj->count = 1;
	act( AT_PLAIN, "You lift $p and look beneath it:", ch, obj, NULL, TO_CHAR );
	act( AT_PLAIN, "$n lifts $p and looks beneath it:", ch, obj, NULL, TO_ROOM );
	obj->count = count;
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	   show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	else
	   send_to_char( "&RNothing.\n\r", ch );
	if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	int count;

	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
	        if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		break;
	    }

	    ch_printf( ch, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

	    if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
	    break;

	case ITEM_PORTAL:
	    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    {
		if ( pexit->vdir == DIR_PORTAL
		&&   IS_SET(pexit->exit_info, EX_PORTAL) )
		{
   	          if ( room_is_private( ch, pexit->to_room ) && ch->level < sysdata.level_override_private )
	          {
	   	        set_char_color( AT_WHITE, ch );
			send_to_char( "The room ahead is private!\n\r", ch );
			return;
	    	  }

	    	  if( IS_EXIT_FLAG( pexit, EX_OVERLAND ) )
	    	  {
			original = ch->in_room;
			enter_map( ch, pexit->x, pexit->y, -1 );
			leave_map( ch, NULL, original );
	    	  }
	    	  else
	    	  {
	      		original = ch->in_room;
	      		char_from_room( ch );
	      		char_to_room( ch, pexit->to_room );
	      		do_look( ch, "auto" );
	      		char_from_room( ch );
	      		char_to_room( ch, original );
	    	  }
		}
	    }
	    send_to_char( "You see swirling chaos...\n\r", ch );
	    break;
	case ITEM_CONTAINER:
	case ITEM_QUIVER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	case ITEM_KEYRING:
	    count = obj->count;
	    obj->count = 1;
	    if ( obj->item_type == ITEM_CONTAINER )
		act( AT_PLAIN, "$p contains:", ch, obj, NULL, TO_CHAR );
	    else
		act( AT_PLAIN, "$p holds:", ch, obj, NULL, TO_CHAR );
	    obj->count = count;
	    show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	    if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
	    break;
	}
	return;
    }

    if ( (pdesc=get_extra_descr(arg1, ch->in_room->first_extradesc)) != NULL )
    {
	send_to_char_color( pdesc, ch );
	return;
    }

    door = get_door(arg1);
    if ( (pexit=find_door(ch, arg1, TRUE)) != NULL )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&  !IS_SET(pexit->exit_info, EX_WINDOW) )
	{
	    if ( (IS_SET(pexit->exit_info, EX_SECRET)
	    ||    IS_SET(pexit->exit_info, EX_DIG)) && door != -1 )
		send_to_char( "Nothing special there.\n\r", ch );
	    else
		act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}
	if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	    act(AT_RED, "The $d has been bashed from its hinges!",ch, NULL, pexit->keyword, TO_CHAR);

	if ( pexit->description && pexit->description[0] != '\0' )
	    send_to_char( pexit->description, ch );
	else
	    send_to_char( "Nothing special there.\n\r", ch );

	/*
	 * Ability to look into the next room			-Thoric
	 */
	if ( pexit->to_room
	||   (IS_SET( pexit->exit_info, EX_xLOOK )
	||   get_trust(ch) >= LEVEL_IMMORTAL ) )
     	{
            if ( ch->level < 49 && !IS_SET( pexit->exit_info, EX_xLOOK ) )
            return;

	    if ( room_is_private( ch, pexit->to_room ) && ch->level < sysdata.level_override_private )
	    {
		set_char_color( AT_WHITE, ch );
		send_to_char( "The room ahead is private!\n\r", ch );
		return;
	    }

	    if( IS_EXIT_FLAG( pexit, EX_OVERLAND ) )
	    {
		original = ch->in_room;
		enter_map( ch, pexit->x, pexit->y, -1 );
		leave_map( ch, NULL, original );
	    }
	    else
	    {
	      original = ch->in_room;
	      char_from_room( ch );
	      char_to_room( ch, pexit->to_room );
	      do_look( ch, "auto" );
	      char_from_room( ch );
	      char_to_room( ch, original );
	    }
	}
	return;
    }
    else
    if ( door != -1 )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( (victim = get_char_room(ch, arg1)) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }


    /* finally fixed the annoying look 2.obj desc bug	-Thoric */
    number = number_argument( arg1, arg );
    for ( cnt = 0, obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		return;
	    }
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char_color( "\n\rYou see nothing special.\r\n", ch );
		else
		  send_to_char_color( pdesc, ch );
		if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		return;
	    }
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char( "You see nothing special.\r\n", ch );
		else
		  send_to_char_color( pdesc, ch );
		if( EXA_prog_trigger ) oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    send_to_char( "You do not see that here.\n\r", ch );
    return;
}

void show_race_line( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    int feet, inches, mod;


	if ( !IS_NPC(victim) && (victim != ch) )
	{
                mod = victim->height + victim->height_mod;
		feet =   mod  / 12;
		inches =  mod % 12;
 		sprintf( buf, "%s is %d'%d\" and weighs %d pounds.\n\r", PERS(victim, ch, FALSE), feet, inches, (victim->weight + victim->weight_mod) );
		send_to_char( buf, ch);
                return;
        } 
	if ( !IS_NPC(victim) && (victim == ch) )
	{
                mod = victim->height + victim->height_mod;
                feet =   mod  / 12;
                inches =  mod % 12;
		sprintf( buf, "You are %d'%d\" and weigh %d pounds.\n\r",  feet, inches, (victim->weight + victim->weight_mod ));
		send_to_char( buf, ch);
                return;
        } 

}


void show_condition( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    int percent;

    if ( victim->max_hit > 0 )
        percent = ( 100 * victim->hit ) / victim->max_hit;
    else
        percent = -1;


    if ( victim != ch )
    {
	strcpy( buf, PERS(victim, ch, FALSE) );
             if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r" );
	else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
        else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
	else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
	else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
	else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
	else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
	else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
	else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
	else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
	else                       strcat( buf, " is DYING.\n\r"              );
    }
    else
    {
	strcpy( buf, "You" );
	     if ( percent >= 100 ) strcat( buf, " are in perfect health.\n\r" );
	else if ( percent >=  90 ) strcat( buf, " are slightly scratched.\n\r");
        else if ( percent >=  80 ) strcat( buf, " have a few bruises.\n\r"    );
        else if ( percent >=  70 ) strcat( buf, " have some cuts.\n\r"        );
        else if ( percent >=  60 ) strcat( buf, " have several wounds.\n\r"   );
        else if ( percent >=  50 ) strcat( buf, " have many nasty wounds.\n\r");
	else if ( percent >=  40 ) strcat( buf, " are bleeding freely.\n\r"   );
	else if ( percent >=  30 ) strcat( buf, " are covered in blood.\n\r"  );
	else if ( percent >=  20 ) strcat( buf, " are leaking guts.\n\r"      );
	else if ( percent >=  10 ) strcat( buf, " are almost dead.\n\r"       );
	else                       strcat( buf, " are DYING.\n\r"             );
    }

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}

/* A much simpler version of look, this function will show you only
the condition of a mob or pc, or if used without an argument, the
same you would see if you enter the room and have config +brief.
-- Narn, winter '96
*/
void do_glance( CHAR_DATA *ch, char *argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  bool brief;
 
  if ( !ch->desc )
    return;
 
  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "You can't see anything but stars!\n\r", ch );
    return;
  }
 
  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
    return;
  }
 
  if ( !check_blind( ch ) )
    return;

  set_char_color( AT_ACTION, ch ); 
  argument = one_argument( argument, arg1 );
 
  if ( arg1[0] == '\0' )
  {
    if ( xIS_SET(ch->act, PLR_BRIEF) )
	brief = TRUE;
    else
	brief = FALSE;
    xSET_BIT( ch->act, PLR_BRIEF );
    do_look( ch, "auto" );
    if ( !brief )
	xREMOVE_BIT(ch->act, PLR_BRIEF);
    return;
  }
 
  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They're not here.\n\r", ch );
    return;
  }
  else
  {
    if ( can_see( victim, ch, FALSE ) )
    {
	act( AT_ACTION, "$n glances at you.", ch, NULL, victim, TO_VICT );
	act( AT_ACTION, "$n glances at $N.",  ch, NULL, victim, TO_NOTVICT );
    }
    if ( IS_IMMORTAL( ch ) && victim != ch )
    {
	if ( IS_NPC( victim ) )
	    ch_printf( ch, "Mobile #%d '%s' ",
		victim->pIndexData->vnum, victim->name );
	else
	    ch_printf( ch, "%s ", victim->name );
	ch_printf( ch, "is a level %d %s %s.\n\r",
	    victim->level,
	    IS_NPC(victim)?victim->race<MAX_NPC_RACE&&victim->race>=0?
            npc_race[victim->race] : "unknown":victim->race<MAX_PC_RACE&&
            race_table[victim->race]->race_name&&
            race_table[victim->race]->race_name[0] != '\0'?
            race_table[victim->race]->race_name:"unknown",
            IS_NPC(victim)?victim->class<MAX_NPC_CLASS&&victim->class>=0?
            npc_class[victim->class] : "unknown":victim->class<MAX_PC_CLASS&&
            class_table[victim->class]->who_name&&
            class_table[victim->class]->who_name[0] != '\0'?
            class_table[victim->class]->who_name:"unknown");
/* New Change
	    victim->race<MAX_NPC_RACE&&victim->race>=0?npc_race[victim->race] : "unknown",
	    victim->class<MAX_NPC_CLASS&&victim->class>=0?npc_class[victim->class] : "unknown" );
*/
    }
    show_condition( ch, victim );

    return;
  }

  return;
}


void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH], buf2[MSL];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    BOARD_DATA *board;
    sh_int dam;

    if ( !argument )
    {
	bug( "do_examine: null argument.", 0);
	return;
    }

    if ( !ch )
    {
	bug( "do_examine: null ch.", 0);
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

/*   EXA_prog_trigger = FALSE;
   do_look( ch, arg );
   EXA_prog_trigger = TRUE;
*/
    /*
     * Support for looking at boards, checking equipment conditions,
     * and support for trigger positions by Thoric
     */
    if ( (obj = get_obj_here(ch, arg)) != NULL )
    {
	if ( (board = get_board(obj)) != NULL )
	{
	    if ( board->num_posts )
		ch_printf( ch, "There are about %d notes posted here.  Type 'note list' to list them.\n\r", board->num_posts );
	    else
		send_to_char( "There aren't any notes posted here.\n\r", ch );
	}

	switch ( obj->item_type )
	{
	default:
	    break;

        case ITEM_CHIP:
            ch_printf( ch, "\n\r&WYou begin to examine this piece of equipment...\n\r" );
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            sprintf( buf2, "&w-&W<&z( &Y%s &z)&W>&w-", obj->short_descr );
            ch_printf( ch, "  %s\n\r", centertext( buf2, 50 ));
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            ch_printf( ch, "  &z|                                                   |" );
              ch_printf( ch, "\n\r  &z|  Grade&O: &z[&w%s&z]                                       &z|",get_grade(obj->value[3]));
            ch_printf( ch, "\n\r  &z| Weight&O: &w%-6d                                    &z|", obj->weight );
            ch_printf( ch, "\n\r  &z|                                                   |" );
            ch_printf( ch, "\n\r  &z+---------------------------------------------------+" );

            break;


	case ITEM_ARMOR:
        case ITEM_JEWELRY:
            ch_printf( ch, "\n\r&WYou begin to examine this piece of equipment...\n\r" );
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            sprintf( buf2, "&w-&W<&z( &Y%s &z)&W>&w-", obj->short_descr );
            ch_printf( ch, "  %s\n\r", centertext( buf2, 50 ));
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            ch_printf( ch, "  &z|                                                   |" );
            ch_printf( ch, "\n\r  &z|  Grade&O: &z[&w%s&z]                                       &z|", get_grade(obj->value[3]));
            if ( obj->item_type != ITEM_JEWELRY )
            ch_printf( ch, "\n\r  &z|   Type&O: &w%-10.10s                                &z|", get_armortype(obj->value[0]));
            ch_printf( ch, "\n\r  &z|  P.Def&O: &w%-6d                                    &z|", obj->value[1] + get_extra( obj->value[3], obj->value[4] ));
            ch_printf( ch, "\n\r  &z|  E.Def&O: &w%-6d                                    &z|", obj->value[2] + get_extra( obj->value[3], obj->value[4] ));
            if ( obj->value[4] > 0 )
            ch_printf( ch, "\n\r  &z| Status&O: &Y+%-6d                                   &z|", obj->value[4] );
            ch_printf( ch, "\n\r  &z| Weight&O: &w%-6d                                    &z|", obj->weight );
            ch_printf( ch, "\n\r  &z|                                                   |" );
            ch_printf( ch, "\n\r  &z+---------------------------------------------------+" );

	    break;
        case ITEM_SCOUTER:
             sprintf(buf, "You reckon this armor offers about &G%d%%&w of protection.\n\rThe powerlevel range is &G%d &wto &G%d&w.\n\rThe Color of this scouter is: %s\n\r&wSpecial Properties: %s", obj->value[1] + 100, obj->value[4], obj->value[5], obj->value[3] == 1 ? "&BBlue" : obj->value[3] == 2 ? "&RRed" : obj->value[3] == 3 ? "&GGreen" : "&pPurple",  ext_flag_string(&obj->extra_flags,o_flags) );
            strcat( buf, "\n\r" );
            send_to_char( buf, ch );
            break;


	case ITEM_WEAPON:
            ch_printf( ch, "\n\r&WYou begin to examine this piece of equipment...\n\r" );
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            sprintf( buf2, "&w-&W<&z( &Y%s &z)&W>&w-", obj->short_descr );
            ch_printf( ch, "  %s\n\r", centertext( buf2, 50 ));
            ch_printf( ch, "  &z+---------------------------------------------------+\n\r" );
            ch_printf( ch, "  &z|                                                   |" );
             ch_printf( ch, "\n\r  &z|  Grade&O: &z[&w%s&z]                                       &z|", get_grade(obj->value[3]));
             ch_printf( ch, "\n\r  &z|   Type&O: &w%-10.10s                                &z|",  get_weapontype(obj->value[0]));
             ch_printf( ch, "\n\r  &z|  P.Atk&O: &w%-6d                                    &z|", obj->value[1] + get_extra( obj->value[3], obj->value[4] ));
            ch_printf( ch, "\n\r  &z|  E.Atk&O: &w%-6d                                    &z|", obj->value[2] + get_extra( obj->value[3], obj->value[4] ));
            if ( obj->value[4] > 0 )
            ch_printf( ch, "\n\r  &z| Status&O: &Y+%-10d                               &z|", obj->value[4] );
            ch_printf( ch, "\n\r  &z| Weight&O: &w%-6d                                    &z|", obj->weight );
            ch_printf( ch, "\n\r  &z| AtkSpd&O: &w%-10.10s                                &z|", obj->value[5] == ATK_VF ? "Very Fast" : obj->value[5] == ATK_F ? "Fast" : obj->value[5] == ATK_N ? "Normal" : obj->value[5] == ATK_S ? "Slow" : "Unknown" );
            ch_printf( ch, "\n\r  &z|                                                   |" );
            ch_printf( ch, "\n\r  &z+---------------------------------------------------+" );
	    break;

	case ITEM_COOK:
	    strcpy( buf, "As you examine it carefully you notice that it " );
	    dam = obj->value[2];
	     if (dam >= 3) strcat( buf, "is burned to a crisp.");
	else if (dam == 2) strcat( buf, "is a little over cooked."); 
	else if (dam == 1) strcat( buf, "is perfectly roasted.");
	else		   strcat( buf, "is raw.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	case ITEM_FOOD:
	    if ( obj->timer > 0 && obj->value[1] > 0 )
	      dam = (obj->timer * 10) / obj->value[1];
	    else
	      dam = 10;
	    if ( obj->item_type == ITEM_FOOD )	
	      strcpy( buf, "As you examine it carefully you notice that it " );
	    else
	      strcpy( buf, "Also it " );
	    if (dam >= 10) strcat( buf, "is fresh.");
       else if (dam ==  9) strcat( buf, "is nearly fresh.");
       else if (dam ==  8) strcat( buf, "is perfectly fine.");
       else if (dam ==  7) strcat( buf, "looks good.");
       else if (dam ==  6) strcat( buf, "looks ok.");
       else if (dam ==  5) strcat( buf, "is a little stale.");
       else if (dam ==  4) strcat( buf, "is a bit stale.");
       else if (dam ==  3) strcat( buf, "smells slightly off.");
       else if (dam ==  2) strcat( buf, "smells quite rank.");
       else if (dam ==  1) strcat( buf, "smells revolting!");
       else if (dam <=  0) strcat( buf, "is crawling with maggots!");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;


	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "You notice that it is in the up position.\n\r", ch );
	    else
		send_to_char( "You notice that it is in the down position.\n\r", ch );
	    break;
	case ITEM_BUTTON:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "You notice that it is depressed.\n\r", ch );
	    else
		send_to_char( "You notice that it is not depressed.\n\r", ch );
	    break;

/* Not needed due to check in do_look already
	case ITEM_PORTAL:
	    sprintf( buf, "in %s noprog", arg );
	    do_look( ch, buf );
	    break;
*/

        case ITEM_CORPSE_PC:
	case ITEM_CORPSE_NPC:
            {
		sh_int timerfrac = obj->timer;
		if ( obj->item_type == ITEM_CORPSE_PC )
		timerfrac = (int)obj->timer / 8 + 1; 

		switch (timerfrac)
		{
		    default:
			send_to_char( "This corpse has recently been slain.\n\r", ch );
			break;
		    case 4:
			send_to_char( "This corpse was slain a little while ago.\n\r", ch );
			break;
		    case 3:
			send_to_char( "A foul smell rises from the corpse, and it is covered in flies.\n\r", ch );
			break;
		    case 2:
			send_to_char( "A writhing mass of maggots and decay, you can barely go near this corpse.\n\r", ch );
			break;
		    case 1:
		    case 0:
			send_to_char( "Little more than bones, there isn't much left of this corpse.\n\r", ch );
			break;
		}
            }
	case ITEM_CONTAINER:
	    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
		break;
	case ITEM_DRINK_CON:
	case ITEM_QUIVER:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	case ITEM_KEYRING:
	EXA_prog_trigger = FALSE;
	sprintf( buf, "in %s",arg );
	do_look( ch, buf );
	EXA_prog_trigger = TRUE;
	    break;
	}
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	{
	   EXA_prog_trigger = FALSE;
	   sprintf( buf, "under %s", arg );
	   do_look( ch, buf );
	   EXA_prog_trigger = TRUE;
	}
	oprog_examine_trigger( ch, obj );
	if ( char_died(ch) || obj_extracted(obj) )
	    return;

    }
    return;
}


void do_exits( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;

    set_char_color( AT_EXITS, ch );
    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind(ch) )
	return;
    if ( IN_NOEXIT(ch))
        return;
    if ( xIS_SET(ch->act, PLR_ONMAP) )
    return;
    strcpy( buf, fAuto ? "&WExits:&w" : "\n\r&zObvious exits&w:&W\n\r" );

    found = FALSE;
    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
    {
	if ( pexit->to_room
	&&  !IS_SET(pexit->exit_info, EX_CLOSED)
	&& (!IS_SET(pexit->exit_info, EX_WINDOW)
	||   IS_SET(pexit->exit_info, EX_ISDOOR))
	&&  !IS_SET(pexit->exit_info, EX_HIDDEN) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[pexit->vdir] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "&W%-5s &Y-&W %s\n\r",
		    capitalize( dir_name[pexit->vdir] ),
		    room_is_dark( pexit->to_room )
			?  "&wToo dark to tell&W"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " None.\n\r" : "None.\n\r" );
    else
      if ( fAuto )
	strcat( buf, "\n\r" );
    send_to_char( buf, ch );
    return;
}

char *	const	day_name	[7] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thrusday",
    "Friday", "Saturday"
};

char *	const	month_name	[12] =
{
    "January", "February", "March", "April", "May", "June", "July", "August",
    "September", "October", "November", "December"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    extern char reboot_time[];
    char *suf;
    int day;

    day     = time_info.day + 1;

	 if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    set_char_color( AT_YELLOW, ch );
    send_to_char( "\n\r",ch);
    ch_printf(ch, "It is %d o'clock %s, %s, %s %d%s.\n\r", (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12, time_info.hour >= 12 ? "pm" : "am", day_name[day % 7], month_name[time_info.month], day, suf);
    ch_printf(ch, "The mud started up at   :  %s", str_boot_time);
    ch_printf(ch, "The system time (E.S.T.):  %s", (char *) ctime( &current_time ));
    ch_printf(ch, "Next Reboot is set for  :  %s", reboot_time );
    if( sysdata.CLEANPFILES && IS_IMMORTAL(ch))
	ch_printf( ch, "Next pfile cleanup is scheduled for: %s\n\r", (char *)ctime( &new_pfile_time_t ) );

/*	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
        month_name[time_info.month],
	day, suf,
	str_boot_time,
	(char *) ctime( &current_time ),
	reboot_time
	);*/

    return;
}

/*
void do_weather( CHAR_DATA *ch, char *argument )
{
    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the sky from here.\n\r", ch );
	return;
    }

    set_char_color( AT_BLUE, ch );
    ch_printf( ch, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    return;
}
*/

/*
 * Produce a description of the weather based on area weather using
 * the following sentence format:
 *		<combo-phrase> and <single-phrase>.
 * Where the combo-phrase describes either the precipitation and
 * temperature or the wind and temperature. The single-phrase
 * describes either the wind or precipitation depending upon the
 * combo-phrase.
 * Last Modified: July 31, 1997
 * Fireblade - Under Construction
 */
void do_weather(CHAR_DATA *ch, char *argument)
{
	char *combo, *single;
	char buf[MAX_INPUT_LENGTH];
	int temp, precip, wind;	


      if( !IS_PLR_FLAG( ch, PLR_ONMAP ) )
      {
        if ( !IS_OUTSIDE(ch) || INDOOR_SECTOR(ch->in_room->sector_type) )
        {
	    send_to_char( "You can't see the sky from here.\n\r", ch );
	    return;
        }
      }
	if ( !IS_OUTSIDE(ch) )
	{
	    ch_printf(ch, "You can't see the sky from here.\n\r");
	    return;
	}

	temp = (ch->in_room->area->weather->temp + 3*weath_unit - 1)/
		weath_unit;
	precip = (ch->in_room->area->weather->precip + 3*weath_unit - 1)/
		weath_unit;
	wind = (ch->in_room->area->weather->wind + 3*weath_unit - 1)/
		weath_unit;
	
	if ( precip >= 3 )
	{
	    combo = preciptemp_msg[precip][temp];
	    single = wind_msg[wind];
	}
	else
	{
	    combo = windtemp_msg[wind][temp];
	    single = precip_msg[precip];
	}
	
	sprintf(buf, "%s and %s.\n\r", combo, single);
	
	set_char_color(AT_BLUE, ch);
	
	ch_printf(ch, buf);
}

/*
 * Moved into a separate function so it can be used for other things
 * ie: online help editing				-Thoric
 */
HELP_DATA *get_help( CHAR_DATA *ch, char *argument )
{
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    char argnew[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;
    int lev;

    if ( argument[0] == '\0' )
	argument = "summary";

    if ( isdigit(argument[0]) )
    {
	lev = number_argument( argument, argnew );
	argument = argnew;
    }
    else
	lev = -2;
    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while ( argument[0] != '\0' )
    {
	argument = one_argument( argument, argone );
	if ( argall[0] != '\0' )
	    strcat( argall, " " );
	strcat( argall, argone );
    }

    for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;
	if ( lev != -2 && pHelp->level != lev )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	    return pHelp;
    }

    return NULL;
}

/*
 * LAWS command
 */
void do_laws( CHAR_DATA *ch, char *argument )
{
    char buf[1024];

    if ( argument == NULL)
	do_help( ch, "laws" );
    else
    {
	sprintf( buf, "law %s", argument );
	do_help( ch, buf );
    }
}

//  Ranks by number of matches between two whole words. Coded for the Similar Helpfiles
//  Snippet by Senir.
sh_int str_similarity( const char *astr, const char *bstr )
{
   sh_int matches=0;

    if (!astr || !bstr)
       return matches;

    for ( ; *astr; astr++)
    {
        if ( LOWER(*astr) == LOWER(*bstr) )
           matches++;
        
        if (++bstr == '\0')
           return matches;                
    }
    
    return matches;
}

//  Ranks by number of matches until there's a nonmatching character between two words.
//  Coded for the Similar Helpfiles Snippet by Senir.
sh_int str_prefix_level( const char *astr, const char *bstr )
{
   sh_int matches=0;

    if (!astr || !bstr)
       return matches;

    for ( ; *astr; astr++)
    {
        if ( LOWER(*astr) == LOWER(*bstr) )
           matches++;
        else
           return matches;

        if (++bstr == '\0')
        return matches;
    }

    return matches;
}

// Main function of Similar Helpfiles Snippet by Senir. It loops through all of the
// helpfiles, using the string matching function defined to find the closest matching
// helpfiles to the argument. It then checks for singles. Then, if matching helpfiles
// are found at all, it loops through and prints out the closest matching helpfiles.
// If its a single(there's only one), it opens the helpfile.
void similar_help_files(CHAR_DATA *ch, char *argument)
{
   HELP_DATA *pHelp=NULL;
   char buf[MAX_STRING_LENGTH];
   char *extension;
   sh_int lvl=0;
   bool single=FALSE;
    if (is_number(argument))
    return; 
    send_to_char( "&C&YSimilar Help Files&O:&w\n\r", ch);
        
    for ( pHelp = first_help; pHelp; pHelp=pHelp->next)
    {
        buf[0]='\0';      
        extension=pHelp->keyword;
        
        if (pHelp->level > get_trust(ch))
           continue;

        while ( extension[0] != '\0' )
        {
              extension= one_argument(extension, buf);
              
              if ( str_similarity(argument, buf) > lvl)
              {
                 lvl=str_similarity(argument, buf);
                 single=TRUE;
              }        
              else if ( str_similarity(argument, buf) == lvl && lvl > 0)
              {
                 single=FALSE;
              }
        }
    }
        
    if (lvl==0)
    {
       send_to_char( "&RThere are no similar helpfiles.\n\r", ch);   
       return;
    }

    for ( pHelp = first_help; pHelp; pHelp=pHelp->next)
    {
        buf[0]='\0';      
        extension=pHelp->keyword;

        while ( extension[0] != '\0' )
        {
              extension=one_argument(extension, buf);

              if ( str_similarity(argument, buf) >= lvl
                   && pHelp->level <= get_trust(ch))
              {
                 if (single)
                 {
                    send_to_char( "&ROpening only similar helpfile&r:&w\n\r", ch);
                    do_help( ch, buf);
                    return;
                 }

                 ch_printf(ch, "&C   %s\n\r", pHelp->keyword);
                 break;

              }

        }
    }
    return;
}


/*
 * Now this is cleaner
 */
void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    set_char_color( AT_WHITE, ch );

    if ( (pHelp = get_help( ch, argument )) == NULL )
    {
//	send_to_char( "&wNo help on that word.\n\r", ch );
    	//  Looks better printing out the missed argument before going to similar
        //  helpfiles. - Senir
        ch_printf( ch, "\n\r&C&WNo help on &R\'&c%s&R\'&W found.\n\r", argument );
        similar_help_files(ch, argument);

	return;
    }

    /* Make newbies do a help start. --Shaddai */ 
    if ( !IS_NPC(ch) && !str_cmp( argument, "start" ) )
	xSET_BIT(ch->pcdata->flags, PCFLAG_HELPSTART);

    send_to_char( "\n\r", ch);
  
    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
    {
	send_to_char( pHelp->keyword, ch );
	send_to_char( "&w\n\r", ch );
    }

    /*
     * Strip leading '.' to allow initial blanks.
     */
    send_to_char( "\n\r", ch);
    if ( pHelp->text[0] == '.' )
    {
        send_to_char( "&z==========================================================================&w\n\r", ch);
	send_to_char( pHelp->text+1, ch );
        send_to_char( "&z==========================================================================&w\n\r", ch);
    }
    else
    {
        send_to_char( "&z==========================================================================&w\n\r", ch);
	send_to_char( pHelp->text  , ch );
        send_to_char( "&z==========================================================================&w\n\r", ch);
    }

    return;
}

void do_news( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_NOTE, ch );
    do_help( ch, "news" );
}

extern char * help_greeting;	/* so we can edit the greeting online */
extern char * help_greeting2;    /* so we can edit the greeting online */
extern char * help_greeting3;    /* so we can edit the greeting online */
extern char * help_greeting4;    /* so we can edit the greeting online */

/*
 * Help editor							-Thoric
 */
void do_hedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor.\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_HELP_EDIT:
	  if ( (pHelp = ch->dest_buf) == NULL )
	  {
		bug( "hedit: sub_help_edit: NULL ch->dest_buf", 0 );
		stop_editing( ch );
		return;
	  }
	  if ( help_greeting == pHelp->text )
		help_greeting = NULL;
          if ( help_greeting2 == pHelp->text )
                help_greeting2 = NULL;

          if ( help_greeting3 == pHelp->text )
                help_greeting3 = NULL;

          if ( help_greeting4 == pHelp->text )
                help_greeting4 = NULL;


	  STRFREE( pHelp->text );
	  pHelp->text = copy_buffer( ch );
	  if ( !help_greeting )
		help_greeting = pHelp->text;
	  stop_editing( ch );
	  return;
    }
    if ( (pHelp = get_help(ch, argument)) == NULL )     /* new help */
    {
        HELP_DATA *tHelp;
        char argnew[MAX_INPUT_LENGTH];
        int lev;
        bool new_help = TRUE;

        for ( tHelp=first_help; tHelp; tHelp = tHelp->next )
           if ( !str_cmp( argument, tHelp->keyword) )
           {
                pHelp = tHelp;
                new_help = FALSE;
                break;
           }
        if ( new_help ) {
        if ( isdigit(argument[0]) )
        {
            lev = number_argument( argument, argnew );
            argument = argnew;
        }
        else
            lev = get_trust(ch);
        CREATE( pHelp, HELP_DATA, 1 );
        pHelp->keyword = STRALLOC( strupper(argument) );
        pHelp->text    = STRALLOC( "" );
        pHelp->level   = lev;
        add_help( pHelp );
        }
    }

    ch->substate = SUB_HELP_EDIT;
    ch->dest_buf = pHelp;
    start_editing( ch, pHelp->text );
    editor_desc_printf( ch, "Help topic, keyword '%s', level %d.", pHelp->keyword, pHelp->level );

}

/*
 * Stupid leading space muncher fix				-Thoric
 */
char *help_fix( char *text )
{
    char *fixed;

    if ( !text )
	return "";
    fixed = strip_cr(text);
    if ( fixed[0] == ' ' )
	fixed[0] = '.';
    return fixed;
}

void do_hset( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: hset <field> [value] [help page]\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  level keyword remove save\n\r",		ch );
	return;
    }

    if ( !str_cmp( arg1, "save" ) )
    {
	FILE *fpout;

	log_string_plus( "Saving help.are...", LOG_NORMAL, LEVEL_GREATER );

	rename( "help.are", "help.are.bak" );
	fclose( fpReserve );
	if ( ( fpout = fopen( "help.are", "w" ) ) == NULL )
	{
	   bug( "hset save: fopen", 0 );
	   perror( "help.are" );
	   fpReserve = fopen( NULL_FILE, "r" );
	   return;
	}
    
	fprintf( fpout, "#HELPS\n\n" );
	for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
	    fprintf( fpout, "%d %s~\n%s~\n\n",
		pHelp->level, pHelp->keyword, help_fix(pHelp->text) );

	fprintf( fpout, "0 $~\n\n\n#$\n" );
	fclose( fpout );
	fpReserve = fopen( NULL_FILE, "r" );
	send_to_char( "Saved.\n\r", ch );
	return;
    }
    if ( str_cmp( arg1, "remove" ) )
	argument = one_argument( argument, arg2 );

    if ( (pHelp = get_help(ch, argument)) == NULL )
    {
	send_to_char( "Cannot find help on that subject.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "remove" ) )
    {
	UNLINK( pHelp, first_help, last_help, next, prev );
	STRFREE( pHelp->text );
	STRFREE( pHelp->keyword );
	DISPOSE( pHelp );
	send_to_char( "Removed.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "level" ) )
    {
	pHelp->level = atoi( arg2 );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "keyword" ) )
    {
	STRFREE( pHelp->keyword );
	pHelp->keyword = STRALLOC( strupper(arg2) );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    do_hset( ch, "" );
}

void do_hl( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to use HLIST, spell it out.\n\r", ch );
    return;
}

/*
 * Show help topics in a level range				-Thoric
 * Idea suggested by Gorog
 * prefix keyword indexing added by Fireblade
 */
void do_hlist( CHAR_DATA *ch, char *argument )
{
    int min, max, minlimit, maxlimit, cnt;
    char arg[MAX_INPUT_LENGTH];
    HELP_DATA *help;
    bool minfound, maxfound;
    char *idx;

    maxlimit = get_trust(ch);
    minlimit = maxlimit >= LEVEL_GREATER ? -1 : 0;
    
    min = minlimit;
    max  = maxlimit;
    
    idx = NULL;
    minfound = FALSE;
    maxfound = FALSE;
    
    for ( argument = one_argument(argument, arg); arg[0] != '\0';
	  argument = one_argument(argument, arg))
    {
	if( !isdigit(arg[0]) )
	{
 	    if ( idx )
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "You may only use a single keyword to index the list.\n\r");
		return;
	    }
	    idx = STRALLOC(arg);
    	}
	else
	{
	    if ( !minfound )
	    {
		min = URANGE(minlimit, atoi(arg), maxlimit);
		minfound = TRUE;
	    }
	    else
	    if ( !maxfound )
	    {
		max = URANGE(minlimit, atoi(arg), maxlimit);
		maxfound = TRUE;
	    }
	    else
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "You may only use two level limits.\n\r");
		return;
	    }
	}
    }
    
    if ( min > max )
    {
	int temp = min;

	min = max;
	max = temp;
    }
    
    set_char_color( AT_GREEN, ch );
    ch_printf( ch, "\n\r&WHelp Topics in level range &R%d&W to&R %d&z:\n\r\n\r", min, max );
    for ( cnt = 0, help = first_help; help; help = help->next )
	if ( help->level >= min && help->level <= max
	&&  (!idx || nifty_is_name_prefix(idx, help->keyword)) )
	{
	    ch_printf( ch, "  &C%3d &W- &R%s\n\r", help->level, help->keyword );
	    ++cnt;
	}
    if ( cnt )
	ch_printf( ch, "\n\r&Y%d&z pages found.\n\r", cnt );
    else
	send_to_char( "None found.\n\r", ch );

    if ( idx )
    	STRFREE(idx);
    
    return;
}


/* 
 * New do_who with WHO REQUEST, clan, race and homepage support.  -Thoric
 *
 * Latest version of do_who eliminates redundant code by using linked lists.
 * Shows imms separately, indicates guest and retired immortals.
 * Narn, Oct/96
 *
 * Who group by Altrag, Feb 28/97
 */
struct whogr_s
{
  struct whogr_s *next;
  struct whogr_s *follower;
  struct whogr_s *l_follow;
  DESCRIPTOR_DATA *d;
  int indent;
} *first_whogr, *last_whogr;

struct whogr_s *find_whogr(DESCRIPTOR_DATA *d, struct whogr_s *first)
{
  struct whogr_s *whogr, *whogr_t;
  
  for (whogr = first; whogr; whogr = whogr->next)
    if (whogr->d == d)
      return whogr;
    else if (whogr->follower && (whogr_t = find_whogr(d, whogr->follower)))
      return whogr_t;
  return NULL;
}

void indent_whogr(CHAR_DATA *looker, struct whogr_s *whogr, int ilev)
{
  for ( ; whogr; whogr = whogr->next )
  {
    if (whogr->follower)
    {
      int nlev = ilev;
      CHAR_DATA *wch =
          (whogr->d->original ? whogr->d->original : whogr->d->character);
      
      if (can_see(looker, wch, TRUE) && !IS_IMMORTAL(wch))
        nlev += 3;
      indent_whogr(looker, whogr->follower, nlev);
    }
    whogr->indent = ilev;
  }
}

/* This a great big mess to backwards-structure the ->leader character
   fields */
void create_whogr(CHAR_DATA *looker)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *wch;
  struct whogr_s *whogr, *whogr_t;
  int dc = 0, wc = 0;
  
  while ((whogr = first_whogr) != NULL)
  {
    first_whogr = whogr->next;
    DISPOSE(whogr);
  }
  first_whogr = last_whogr = NULL;
  /* Link in the ones without leaders first */
  for (d = last_descriptor; d; d = d->prev)
  {
    if (d->connected != CON_PLAYING && d->connected != CON_EDITING)
      continue;
    ++dc;
    wch = (d->original ? d->original : d->character);
    if (!wch->leader || wch->leader == wch || !wch->leader->desc ||
         IS_NPC(wch->leader) || IS_IMMORTAL(wch) || IS_IMMORTAL(wch->leader))
    {
      CREATE(whogr, struct whogr_s, 1);
      if (!last_whogr)
        first_whogr = last_whogr = whogr;
      else
      {
        last_whogr->next = whogr;
        last_whogr = whogr;
      }
      whogr->next = NULL;
      whogr->follower = whogr->l_follow = NULL;
      whogr->d = d;
      whogr->indent = 0;
      ++wc;
    }
  }
  /* Now for those who have leaders.. */
  while (wc < dc)
    for (d = last_descriptor; d; d = d->prev)
    {
      if (d->connected != CON_PLAYING && d->connected != CON_EDITING)
        continue;
      if (find_whogr(d, first_whogr))
        continue;
      wch = (d->original ? d->original : d->character);
      if (wch->leader && wch->leader != wch && wch->leader->desc &&
         !IS_NPC(wch->leader) && !IS_IMMORTAL(wch) &&
         !IS_IMMORTAL(wch->leader) &&
         (whogr_t = find_whogr(wch->leader->desc, first_whogr)))
      {
        CREATE(whogr, struct whogr_s, 1);
        if (!whogr_t->l_follow)
          whogr_t->follower = whogr_t->l_follow = whogr;
        else
        {
          whogr_t->l_follow->next = whogr;
          whogr_t->l_follow = whogr;
        }
        whogr->next = NULL;
        whogr->follower = whogr->l_follow = NULL;
        whogr->d = d;
        whogr->indent = 0;
        ++wc;
      }
    }
  /* Set up indentation levels */
  indent_whogr(looker, first_whogr, 0);
  
  /* And now to linear link them.. */
  for (whogr_t = NULL, whogr = first_whogr; whogr; )
    if (whogr->l_follow)
    {
      whogr->l_follow->next = whogr;
      whogr->l_follow = NULL;
      if (whogr_t)
        whogr_t->next = whogr = whogr->follower;
      else
        first_whogr = whogr = whogr->follower;
    }
    else
    {
      whogr_t = whogr;
      whogr = whogr->next;
    }
}

void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char clan_name[MAX_INPUT_LENGTH];
    char invis_str[MAX_INPUT_LENGTH];
    char char_name[MAX_INPUT_LENGTH];
   // char *extra_title;
    char class_text[MAX_INPUT_LENGTH];
    struct whogr_s *whogr, *whogr_p;
    DESCRIPTOR_DATA *d;
    int iClass, iRace;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_RACE];
    bool fClassRestrict;
    bool fRaceRestrict;
    bool fImmortalOnly;
    bool fNewbie;
    bool fTrue;
    bool fAscended;
    bool fSupreme;
    bool fXtreme;
    bool fLeader;
    bool fPkill;
    bool fShowHomepage;
    bool fClanMatch; /* SB who clan */
    bool fGroup;
    CLAN_DATA *pClan = NULL;
    FILE *whoout = NULL;


    WHO_DATA *cur_who = NULL;
    WHO_DATA *next_who = NULL;
    WHO_DATA *first_mortal = NULL;
    WHO_DATA *first_imm = NULL;
    WHO_DATA *first_true  = NULL;
    WHO_DATA *first_asce = NULL; 
    WHO_DATA *first_supreme = NULL;
    WHO_DATA *first_xtreme = NULL;
    WHO_DATA *first_grouped = NULL;
    WHO_DATA *first_groupwho = NULL;


    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fRaceRestrict  = FALSE;
    fImmortalOnly  = FALSE;
    fNewbie        = FALSE;
    fTrue          = FALSE;
    fAscended      = FALSE;  
    fSupreme       = FALSE;
    fXtreme	   = FALSE;
    fPkill         = FALSE;
    fShowHomepage  = FALSE;
    fClanMatch	   = FALSE; /* SB who clan */
    fGroup	   = FALSE; /* Alty who group */
    fLeader	   = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	rgfRace[iRace] = FALSE;

#ifdef REQWHOARG
    /*
     * The who command must have at least one argument because we often
     * have up to 500 players on. Too much spam if a player accidentally
     * types "who" with no arguments.           --Gorog
     */
    if ( ch && argument[0] == '\0' )
    {
        send_to_char(
        "\n\r&GYou must specify at least one argument.\n\rUse 'who 1' to view the entire who list.\n\r", ch);
        return;
    }
#endif
	
    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		send_to_char( "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( strlen(arg) < 3 )
	    {
		send_to_char( "Arguments must be longer than that.\n\r", ch );
		return;
	    }

	    /*
	     * Look for classes to turn on.
	     */
            if ( !str_cmp( arg, "deadly" ) )
              fPkill = TRUE;
            else
	    if ( !str_cmp( arg, "imm" ) || !str_cmp( arg, "gods" ) )
		fImmortalOnly = TRUE;
	    else
            if ( !str_cmp( arg, "newbie") || !str_cmp( arg, "new" ))
               fNewbie = TRUE;
            else
            if ( !str_cmp( arg, "trained") || !str_cmp( arg, "true" ))
               fTrue = TRUE;
	    else
	    if ( !str_cmp( arg, "www" ) )
		fShowHomepage = TRUE;
            else
            if ( !str_cmp( arg, "ascended" ))
               fAscended = TRUE;
            else
            if ( !str_cmp( arg, "supreme" ))
               fSupreme = TRUE;
            else
            if ( !str_cmp( arg, "xtreme" ))
               fXtreme = TRUE;
            else
            if ( !str_cmp( arg, "leader" ))
               fLeader = TRUE;
            else
             if  ( ( pClan = get_clan (arg) ) )
	   	fClanMatch = TRUE;
	    else
	    {
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( !str_cmp( arg, class_table[iClass]->who_name ) )
		    {
			rgfClass[iClass] = TRUE;
			break;
		    }
		}
		if ( iClass != MAX_CLASS )
		  fClassRestrict = TRUE;

		for ( iRace = 0; iRace < MAX_RACE; iRace++ )
		{
		    if ( !str_cmp( arg, race_table[iRace]->race_name ) )
		    {
			rgfRace[iRace] = TRUE;
			break;
		    }
		}
		if ( iRace != MAX_RACE )
		  fRaceRestrict = TRUE;

		if ( iClass == MAX_CLASS && iRace == MAX_RACE && fClanMatch == FALSE )
		{
		 send_to_char( "That's not a class, race or clan.", ch);
		    return;
		}
	    }
	}
    }

    /*
     * Now find matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    if ( ch )
	set_char_color( AT_GREEN, ch );
    else
    {
	if ( fShowHomepage )
	  whoout = fopen( WEBWHO_FILE, "w" );
	else
	  whoout = fopen( WHO_FILE, "w" );
	if ( !whoout )
	{
	  bug( "do_who: cannot open who file!" );
	  return;
	}
    }

/* start from last to first to get it in the proper order */
    if (fGroup)
    {
      create_whogr(ch);
      whogr = first_whogr;
      d = whogr->d;
    }
    else
    {
      whogr = NULL;
      d = last_descriptor;
    }
    whogr_p = NULL;
    for ( ; d; whogr_p = whogr, whogr = (fGroup ? whogr->next : NULL),
    			d = (fGroup ? (whogr ? whogr->d : NULL) : d->prev))
    {
	CHAR_DATA *wch;
	char const *class;

	if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING
           && d->connected != CON_NOTE_TO && d->connected != CON_NOTE_SUBJECT
           && d->connected != CON_NOTE_EXPIRE && d->connected != CON_NOTE_TEXT
           && d->connected != CON_NOTE_FINISH)
	||   !can_see( ch, d->character, TRUE ) || d->original)
	    continue;
	wch   = d->original ? d->original : d->character;
	if ( wch->level < iLevelLower
	||   wch->level > iLevelUpper
	|| ( fPkill && !CAN_PKILL( wch ) ) 
	|| ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fNewbie && (wch->exp >= 5000000 || wch->level > 49))
        || ( fTrue && ((wch->exp < 5000000 || wch->exp >= 45000000) || (wch->level > 49)))
        || ( fAscended && ((wch->exp < 45000000 || wch->exp >= 95000000) || (wch->level > 49)))
        || ( fSupreme && ((wch->exp < 95000000) || wch->exp >= 129000000))
        || ( fXtreme && ((wch->exp < 129000000) || wch->level > 49))
	|| ( fClassRestrict && !rgfClass[wch->class] )
	|| ( fRaceRestrict && !rgfRace[wch->race] )
	|| ( fClanMatch && ( pClan != wch->pcdata->clan )))  /* SB */
	    continue;
	if ( fLeader &&
	     !(wch->pcdata->clan && ((wch->pcdata->clan->deity
             &&!str_cmp(wch->pcdata->clan->deity,wch->name) )
	     || (wch->pcdata->clan->leader
	     && !str_cmp(wch->pcdata->clan->leader, wch->name ) )
	     || (wch->pcdata->clan->number1
	     && !str_cmp(wch->pcdata->clan->number1, wch->name ) )
	     || (wch->pcdata->clan->number2 
	     && !str_cmp(wch->pcdata->clan->number2, wch->name )))))
	     continue;


	nMatch++;

/*	if ( fShowHomepage
	&&   wch->pcdata->homepage
	&&   wch->pcdata->homepage[0] != '\0' )
	  sprintf( char_name, "<A HREF=\"%s\">%s</A>",
		show_tilde( wch->pcdata->homepage ), wch->name );
	else*/
	  strcpy( char_name, wch->name );

//	sprintf( class_text, "%s %s ", IS_WAITING_FOR_AUTH(wch) ? "&GN" : " ", race_table[wch->race]->race_name );
        sprintf( class_text, "%s", get_transformation( wch ));
	class = class_text;
	switch ( wch->level )
	{
	default: break;
	case MAX_LEVEL -  0:
	case MAX_LEVEL -  1: class = "Owner       ";       break;
	case MAX_LEVEL -  2: class = "Co-Owner    ";	break;
	case MAX_LEVEL -  3: class = "Supervisor  ";    	break;
	case MAX_LEVEL -  4: class = "Head Blder. ";	break;
	case MAX_LEVEL -  5: class = "Head RP     ";       break;
	case MAX_LEVEL -  6: class = "Head Enf.   ";	break;
	case MAX_LEVEL -  7: class = "Anc. Blder. ";	break;
	case MAX_LEVEL -  8: class = "Great Bldr. ";	break;
	case MAX_LEVEL -  9: class = "Less Blder. ";	break;
	case MAX_LEVEL - 10: class = "Manager     ";	break;
	case MAX_LEVEL - 11: class = "Specialist  ";	break;
	case MAX_LEVEL - 12: class = "Sr. Blder.  ";       break;
	case MAX_LEVEL - 13: class = "Jr. Blder.  ";	break;
	case MAX_LEVEL - 14: class = "New Blder.  ";	break;
	case MAX_LEVEL - 15: class = "P-Tester    ";	break;
	case MAX_LEVEL - 16: class = "RP Assist   ";	break;
	}

        if ( IS_RETIRED( wch ) )
          class = "Retired"; 
        else if ( IS_GUEST( wch ) )
          class = "Guest";
	else if ( wch->pcdata->clan
             &&  !str_cmp( wch->name, wch->pcdata->clan->leader )
             &&   wch->pcdata->clan->leadrank[0] != '\0' )
                class = wch->pcdata->clan->leadrank;
        else if ( wch->pcdata->clan
	     &&  !str_cmp( wch->name, wch->pcdata->clan->number1 )
             &&   wch->pcdata->clan->onerank[0] != '\0' )
                class = wch->pcdata->clan->onerank;
	else if ( wch->pcdata->clan
             &&  !str_cmp( wch->name, wch->pcdata->clan->number2 )
             &&   wch->pcdata->clan->tworank[0] != '\0' )
                class = wch->pcdata->clan->tworank;
  	else if ( wch->pcdata->rank && wch->pcdata->rank[0] != '\0' )
	  class = wch->pcdata->rank;

	if ( wch->pcdata->clan )
	{
            CLAN_DATA *pclan = wch->pcdata->clan;
	    strcpy( clan_name, " &z(&W" );
	    if ( !str_cmp( wch->name, pclan->deity ) )
	      strcat( clan_name, "&WDeity of " );
	    else
	    if ( !str_cmp( wch->name, pclan->leader ) )
	      strcat( clan_name, "&WLeader of " );
	    else
	    if ( !str_cmp( wch->name, pclan->number1 ) )
	      strcat( clan_name, "&WNumber One " );
	    else
	    if ( !str_cmp( wch->name, pclan->number2 ) )
	      strcat( clan_name, "&WNumber Two " );
              strcat( clan_name, pclan->name );
  	      strcat( clan_name, "&z)&w" );
	}
	else
	  clan_name[0] = '\0';

	if ( xIS_SET(wch->act, PLR_WIZINVIS) )
	  sprintf( invis_str, "(%d) ", wch->pcdata->wizinvis );
	else
	  invis_str[0] = '\0';
	sprintf( buf, "&W%-15s %s%s%s%s%s%s%s%s%s%s&c%s&w%s%s%s%s\n\r",
	  class,
	  invis_str,
          (wch->desc && wch->desc->connected) ? "&G(Writing) " : "",
	   xIS_SET(wch->pcdata->flags, PCFLAG_DND) ? "&r[DND] " : "",
          xIS_SET(wch->act, PLR_AFK) ? "&W[AFK] " : "",
	  xIS_SET(wch->act, PLR_SPEC) ? "&B[&RSpectator&B] " : "",
          xIS_SET(wch->act, PLR_ATTACKER) ? "&R(ATTACKER) " : "",
	  xIS_SET(wch->act, PLR_KILLER) ? "&R(KILLER) " : "",
          xIS_SET(wch->act, PLR_OUTLAW) ? "&R(OUTLAW) " : "",
          xIS_SET(wch->powerup, PLR_BOOST) ? "&C" : "&Y",
          char_name,
	  wch->pcdata->title,
	  clan_name,
          has_earthdb(wch) ? " &R*" : "",
          has_namekdb(wch) ? " &c*" : "",
          has_blackdb(wch) ? " &z*" : "");

          /*  
           * This is where the old code would display the found player to the ch.
           * What we do instead is put the found data into a linked list
           */ 

          /* First make the structure. */
          CREATE( cur_who, WHO_DATA, 1 );
          cur_who->text = str_dup( buf );
          if ( IS_IMMORTAL( wch ) || xIS_SET(wch->act, PLR_SEMI) )
          {cur_who->type = WT_IMM;}
          else if (wch->exp < 5000000)
          {cur_who->type = WT_MORTAL;}
          else if (wch->exp < 45000000)
          {cur_who->type = WT_TRUE;}
          else if (wch->exp < 95000000)
          {cur_who->type = WT_ASCE;}
          else if (wch->exp < 130000000)
          {cur_who->type = WT_SUPREME;}
	  else
          {cur_who->type = WT_XTREME;}


         if ( fGroup )
         {
            if ( wch->leader || (whogr_p && whogr_p->indent) )
              cur_who->type = WT_GROUPED;
            else
              cur_who->type = WT_GROUPWHO;
          } 
          /* Then put it into the appropriate list. */
          switch ( cur_who->type )
          {
            case WT_MORTAL:
             cur_who->next = first_mortal;
              first_mortal = cur_who;
              break;
            case WT_TRUE:
              cur_who->next = first_true;
              first_true = cur_who;
              break;
            case WT_ASCE:
              cur_who->next = first_asce;
              first_asce = cur_who;
              break;
            case WT_SUPREME:
              cur_who->next = first_supreme;
              first_supreme = cur_who;
              break;
            case WT_XTREME:
              cur_who->next = first_xtreme;
              first_xtreme = cur_who;
              break;

            case WT_GROUPED:
              cur_who->next = first_grouped;
              first_grouped = cur_who;
              break;
            case WT_GROUPWHO:
              cur_who->next = first_groupwho;
              first_groupwho = cur_who;
              break;
            case WT_IMM:
              cur_who->next = first_imm;
              first_imm = cur_who;
              break;
          }

    }


    /* Ok, now we have three separate linked lists and what remains is to 
     * display the information and clean up.
     */
    /*
     * Two extras now for grouped and groupwho (wanting group). -- Alty
     */
    if ( first_mortal )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor("&W&b=============================&z[&G&Y NEWBIE FIGHTERS&z ]&b===========================&W\n\n"));
      else
         send_to_char( "\n\r&W&b=============================&z[&G&Y NEWBIE FIGHTERS&z ]&b===========================&W\n\r\n\r", ch);
    }
    for ( cur_who = first_mortal; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    } 

    if ( first_true )
    {
       if ( !ch )
        fprintf( whoout, htmlcolor("&W&b==============================&z[&G&Y TRUE FIGHTERS &z]&b============================&W\n\n"));
      else 
          send_to_char( "\n\r&W&b==============================&z[&G&Y TRUE FIGHTERS &z]&b============================&W\n\r\n\r", ch);

    }

    for ( cur_who = first_true; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    }

    if ( first_asce )
    {
       if ( !ch )
        fprintf( whoout, htmlcolor("&W&b============================&z[&G&Y ASCENDED FIGHTERS &z]&b==========================&W\n\n"));
      else
         send_to_char( "\n\r&W&b============================&z[&G&Y ASCENDED FIGHTERS &z]&b==========================&W\n\r\n\r", ch);

    }

    for ( cur_who = first_asce; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout,htmlcolor( cur_who->text ) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
    }

    if ( first_supreme )
    {
       if ( !ch )
        fprintf( whoout, htmlcolor("&W&b============================&z[&G&Y SUPREME  FIGHTERS &z]&b==========================&W\n\n"));
      else
         send_to_char( "\n\r&W&b============================&z[&G&Y SUPREME  FIGHTERS &z]&b==========================&W\n\r\n\r", ch);

    }

    for ( cur_who = first_supreme; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
    }

    if ( first_xtreme )
    {
       if ( !ch )
        fprintf( whoout, htmlcolor("&W&b============================&z[&G&Y EXTREME  FIGHTERS &z]&b==========================&W\n\n"));
      else
         send_to_char( "\n\r&W&b============================&z[&G&Y EXTREME  FIGHTERS &z]&b==========================&W\n\r\n\r", ch);

    }

    for ( cur_who = first_xtreme; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who );
    }

    
    if (first_grouped)
    {
      if ( !ch )
        fprintf( whoout, htmlcolor("\n\r-----------------------------[ GROUPED CHARACTERS ]---------------------------\n\r\n\r") );
      else
       send_to_char( "\n\r-----------------------------[ GROUPED CHARACTERS ]---------------------------\n\r\n\r", ch );
    }
    for ( cur_who = first_grouped; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    }

    if (first_groupwho)
    {
      if ( !ch )
        fprintf( whoout, htmlcolor("\n\r-------------------------------[ WANTING GROUP ]------------------------------\n\r\n\r") );
      else
       send_to_char( "\n\r-------------------------------[ WANTING GROUP ]------------------------------\n\r\n\r", ch );
    }
    for ( cur_who = first_groupwho; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    }

    if ( first_imm )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor("\n\r&W&b================================&z[&G&Y IMMORTALS &z]&b==============================&W\n\r\n\r") );
      else
       send_to_char( "\n\r&W&b================================&z[&G&Y IMMORTALS &z]&b==============================&W\n\r\n\r", ch );
    }

    for ( cur_who = first_imm; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, htmlcolor(cur_who->text) );
      else
        send_to_char( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    } 

    if ( !ch )
    {
	fprintf( whoout, htmlcolor("&Y%d player%s.\n\r"), nMatch, nMatch == 1 ? "" : "s" );
	fclose( whoout );
	return;
    }

    set_char_color( AT_YELLOW, ch );
    ch_printf( ch, "\n\r&b==============================&z[  &Y%2d Player%s  &z]&b=============================\n\r", nMatch, nMatch == 1 ? " " : "s" );
    return;
}


void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->first_carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( !msg )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( AT_PLAIN, msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0]!='\0' 
    &&   (victim=get_char_world(ch, arg)) && !IS_NPC(victim)
    &&   xIS_SET(victim->pcdata->flags, PCFLAG_DND)
    &&   get_trust(ch) < get_trust(victim) )
    {
         act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
         return;
   }

    set_char_color( AT_PERSON, ch );
    if ( arg[0] == '\0' )
    {
        ch_printf( ch, "\n\rPlayers near you in %s:\n\r", ch->in_room->area->name );
	found = FALSE;
	for ( d = first_descriptor; d; d = d->next )
	    if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim, TRUE )
            && ( get_trust(ch) >= get_trust(victim) 
            ||   !xIS_SET(victim->pcdata->flags, PCFLAG_DND) ) 
               ) /* if victim has the DND flag ch must outrank them */ 

	    {
		found = TRUE;
/*		if ( CAN_PKILL( victim ) )
		  set_char_color( AT_PURPLE, ch );
		else
		  set_char_color( AT_PERSON, ch );
*/
		ch_printf( ch, "&P%-13s  ", victim->name );
		if (  victim->level >= 49 )
		  send_to_char( "&P(&WImmortal&P)\t", ch );
		else if ( victim->exp > 5000000 && !victim->pcdata->clan )
		  send_to_char( "(&wUnclanned&P)\t", ch );
		else
		  send_to_char( "\t\t\t", ch );
		ch_printf( ch, "&P%s\n\r", victim->in_room->name );
	    }
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = first_char; victim; victim = victim->next )
	    if ( victim->in_room
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim, FALSE )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		ch_printf( ch, "%-28s %s\n\r",
		    PERS(victim, ch, TRUE), victim->in_room->name );
		break;
	    }
	if ( !found )
	    act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "You decide you're pretty sure you could take yourself in a fight.\n\r", ch );
	return;
    }
    diff = victim->level - ch->level;

	 if ( diff <= -10 ) msg = "You are far more experienced than $N.";
    else if ( diff <=  -5 ) msg = "$N is not nearly as experienced as you.";
    else if ( diff <=  -2 ) msg = "You are more experienced than $N.";
    else if ( diff <=   1 ) msg = "You are just about as experienced as $N.";
    else if ( diff <=   4 ) msg = "You are not nearly as experienced as $N.";
    else if ( diff <=   9 ) msg = "$N is far more experienced than you!";
    else                    msg = "$N would make a great teacher for you!";
    act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

    diff = (int) (victim->max_hit - ch->max_hit) / 6;

	 if ( diff <= -200) msg = "$N looks like a feather!";
    else if ( diff <= -150) msg = "You could kill $N with your hands tied!";
    else if ( diff <= -100) msg = "Hey! Where'd $N go?";
    else if ( diff <=  -50) msg = "$N is a wimp.";
    else if ( diff <=    0) msg = "$N looks weaker than you.";
    else if ( diff <=   50) msg = "$N looks about as strong as you.";
    else if ( diff <=  100) msg = "It would take a bit of luck...";
    else if ( diff <=  150) msg = "It would take a lot of luck, and equipment!";
    else if ( diff <=  200) msg = "Why don't you dig a grave for yourself first?";
    else                    msg = "$N is built like a TANK!";
    act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    sh_int result;
    sh_int *pAbility;
    char *pOutput;
    int cost;
    int chance;
    int pl;
    int chik;

    chance = number_range(0, 3);
    chik = number_range(0, 1);
    pl = 0;
    if ( IS_NPC(ch) )
        return;

        for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && xIS_SET(mob->act, ACT_TRAIN) )
            break;
    }

    if ( mob == NULL)
    {
      if ( !str_cmp( argument, "str" ) || !str_cmp( argument, "int" ) || !str_cmp( argument,  "wis" ) || !str_cmp( argument, "dex" ) || !str_cmp( argument, "con" ) || !str_cmp( argument,"cha" ) || !str_cmp( argument, "lck" ) || !str_cmp( argument, "primary" ) || !str_cmp( argument, "chikara1" ) || !str_cmp( argument, "chikara2" )|| !str_cmp( argument, "chikara3") || !str_cmp( argument, "chikara4" ) || !str_cmp( argument, "primaryattack") || !str_cmp(argument, "chikara5") )
     {
         send_to_char( "\n\rYou can't do that here.\n\r", ch );
         return;
     }
    }

    cost = 1;   /* Other then prime att cost */

    if ( !str_cmp( argument, "str" ) )
    {
        cost    = 1;   
        pAbility    = &ch->perm_str;
        pOutput     = "strength";
    }
    else if ( !str_cmp( argument, "int" ) )
    {
        cost    = 1; 
        pAbility    = &ch->perm_int;
        pOutput     = "intelligence";
    }
    else if ( !str_cmp( argument, "wis" ) )
    {
        cost    = 1;  
        pAbility    = &ch->perm_wis;
        pOutput     = "wisdom";
    }
    else if ( !str_cmp( argument, "dex" ) )
    {
        cost    = 1;   
        pAbility    = &ch->perm_dex;
        pOutput     = "dexterity";
    }
    else if ( !str_cmp( argument, "con" ) )
    {
        cost    = 1;  
        pAbility    = &ch->perm_con;
        pOutput     = "constitution";
    }
    else if ( !str_cmp( argument, "cha" ) )
    {
        cost    = 1;  
        pAbility    = &ch->perm_cha;
        pOutput     = "charisma";
    }
    else if ( !str_cmp( argument, "lck" ) )
    {
        cost    = 1;  
        pAbility    = &ch->perm_lck;
        pOutput     = "luck";
    }
    else if ( !str_cmp( argument, "gravity" ) )
    {
      int plcheck, success, chance, gravtotal, chance2, needed2;
    
      char  logbuf[MAX_STRING_LENGTH];
      bool nogain=FALSE;
     
 
     int gainpl;
     double needed, charpl, setgrav;
     charpl = ch->exp;
     needed = sqrt(ch->gravset);
     needed2 = needed;
     plcheck = (ch->gravset * 10000);
     success = ch->gravsucc;
     setgrav = ch->gravset;
     gainpl = (sqrt(setgrav) * sqrt(charpl)) * 2;   
     chance = number_range( 0, 2);
     gravtotal = ch->gravtotal;  
     chance2 = number_range( 0, 1);
     if (needed > 100){needed = 100;}
     if (!IS_SET(ch->in_room->room_flags2, ROOM_GRAVITY) && !IS_SET(ch->in_room->room_flags2, ROOM_UGRAVITY))
     {
      send_to_char( "You aren't in a gravity room.\n\r", ch );
      return;
     }
  /*   if ( ch->exp >= 5000000 || IS_IMMORTAL(ch))
     {
      send_to_char("\n\rTraining gravity at your level wont do any difference.\n\r", ch);
      return;
     }
   */   
     if (setgrav == 0)
     {
      send_to_char("\n\rYou should probably set the machine to a gravity level.\n\r", ch);
      return;
     }   
     if (needed <= 0)
     needed = 1;
     if ( IS_NPC(ch) )
     return;
     if (!IS_AWAKE(ch))
     {
      send_to_char("You aren't awake...\n\r", ch);
      return;
     }
     if  ( get_timer(ch, TIMER_GRAVITY) > 0 && !IS_IMMORTAL(ch))
     {
      send_to_char("\n\rYou can't train again yet, your body needs to recover.\n\r", ch);
      return;
     }
     if ( plcheck > ch->exp && ch->gravset > 1)
     {
      if (chance2 == 1)
      {
       send_to_char( "\n\r&rAs you turn on the gravity... You feel that you are squashed like a pancake.&w\n\r", ch);
       act(AT_BLOOD,"\n\rAs $n turns on the gravity... $e gets squashed like a pancake.", ch, NULL, NULL, TO_CANSEE);
       raw_kill(ch, ch);
       sprintf(logbuf, "&O%s has died in the Gravity Chamber.", ch->name );
       log_string( logbuf );
      }
      else
      {
       ch->hit = -3;
       update_pos(ch);
      }
      return;
     }
     if ( ch->hit <= 9)
     {
      send_to_char("Your too tired to continue on with your gravity training.\n\r", ch);
      return;
     }
     if (ch->gravsucc >= 20) chance = 1;

     if (chance == 0 ) /*Unsuccessful*/
     {
      if (gravtotal == 0)
      act(AT_GREY,"$n starts $s gravity workout.", ch, NULL, NULL, TO_CANSEE);
      ch_printf(ch, "\n\rYou turn on the gravity of the gravitron and you start your workout.\n\r");
      ch_printf(ch, "Current workout stats for level &Y%d&w: success: &Y%d&w  needed: &Y%d&w.\n\r", ch->gravset, ch->gravsucc, needed2);
      ch_printf(ch, "Your workout is &Runsuccessful!\n\r");
      ch->gravtotal += 1;
      ch->hit -= 1;
     }
     else if (chance == 1 || chance == 2) /*Successful*/
     {
      if (gravtotal == 0)
      act(AT_GREY,"$n starts $s gravity workout.\n\r", ch, NULL, NULL, TO_CANSEE);
      ch->gravtotal += 1;
      ch->gravsucc += 1;
      ch_printf(ch, "\n\rYou turn on the gravity of the gravitron and you start your workout.\n\r");
      ch_printf(ch, "Current workout stats for level &Y%d&w: success: &Y%d&w  needed: &Y%d&w.\n\r", ch->gravset, ch->gravsucc, needed2);
      ch_printf(ch, "Your workout is &Gsuccessful!\n\r");
      ch->hit -= 1;
      if (success >= needed) 
      {
      if ( check_weightabout(ch))
        gainpl *= 1.05;
      if ( check_weightfeet(ch))
        gainpl *= 1.05;
      if ( check_weighthead(ch))
        gainpl *= 1.05;
      if ( check_weightwristleft(ch))
        gainpl *= 1.05;
      if ( check_weightwristright(ch))
        gainpl *= 1.05;
      if ( check_weightlegs(ch))
        gainpl *= 1.05;
      if (xIS_SET(ch->powerup, PLR_BOOST))
        gainpl *= 1.15;
      if (IS_SET(ch->in_room->room_flags2, ROOM_UGRAVITY))
        gainpl *= 1.8;
        ch_printf(ch, "&YYou gained &G%d&Y Powerlevel!\n\r", gainpl);
        if ( ch->exp >= 5000000 && ch->exp < 25000000)  {pl = ch->pcdata->learned[gsn_chikone] * 250000;}
        else if ( ch->exp >= 25000000 && ch->exp < 55000000){pl = ch->pcdata->learned[gsn_chiktwo] * 550000; }
        else if ( ch->exp >= 55000000 && ch->exp < 75000000){pl = ch->pcdata->learned[gsn_chikthree] * 750000; }
        else if ( ch->exp >= 75000000 && ch->exp < 100000000){pl = ch->pcdata->learned[gsn_chikfour] * 1000000;}
        else if ( ch->exp >= 100000000 && ch->exp < 125000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}
        if (ch->exp >= 5000000)
        {
         if (  pl < ch->exp) nogain = TRUE;
         }
        if ( nogain )
        send_to_char("\n\rYour powerlevel is too high, gravity training won't do any effect on it.\n\r",ch);

         if ( !nogain)
        ch->exp += gainpl;
        if ( is_clanned(ch) == TRUE )
        {
          if (ch->exp > 125000000)
           ch->exp = 125005000;
        }
        else
        {
          if (ch->exp > 100000000)
           ch->exp = 100005000;
        }
        ch->gravsucc = 0;
        ch->gravtotal = 0;
        add_timer( ch, TIMER_GRAVITY, 60, NULL, 0 );
        }
     }
     return;
    }
    else if( !str_cmp( argument, "chikara1" ) )
    {
     save_char_obj( ch );
     if(IS_BUU(ch) && !xIS_SET(ch->act, PLR_SKINNY) && !xIS_SET(ch->act, PLR_FAT)) 
     {
	send_to_char("&RYou need to absorb more victims before your body can take Chikara 1!\r\n", ch);
	return;
     }
     if (ch->pcdata->learned[gsn_chikone] == 0) { send_to_char("You don't know Chikara 1.\n\r",ch); return; }
     if (ch->mana < 250){ send_to_char("You need more energy to train.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikone] >= 100 )
     { send_to_char("You have fully trained Chikara 1.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikone] < 101 )
     {
      ch->mana -= 250;
      send_to_char("\n\r&wYou feel the power rushing through your body as you train, excellent!.\n\r", ch);
      act( AT_YELLOW, "$n trains Chikara 1.", ch, NULL, NULL, TO_ROOM );
      if (chik == 0)
      ch->pcdata->learned[gsn_chikone] += 1;
     }
     return;
    }
    else if( !str_cmp( argument, "chikara2" ) )
    {
     save_char_obj( ch );
     if (ch->pcdata->learned[gsn_chiktwo] == 0) { send_to_char("You don't know Chikara 2.\n\r",ch); return; }
     if (ch->mana < 500){ send_to_char("You need more energy to train.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chiktwo] >= 100 )
     { send_to_char("You have fully trained Chikara 2.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chiktwo] < 101 )
     {
      ch->mana -= 500;
       send_to_char("\n\r&wYou feel the power rushing through your body as you train,excellent!.\n\r", ch);
      act( AT_YELLOW, "$n trains Chikara 2.", ch, NULL, NULL, TO_ROOM );
      if (chik == 0)
      ch->pcdata->learned[gsn_chiktwo] += 1;
     }
     return;
    }
    else if( !str_cmp( argument, "chikara3" ) )
    {
     save_char_obj( ch );
     if (ch->pcdata->learned[gsn_chikthree] == 0) { send_to_char("You don't know Chikara 3.\n\r",ch); return; }
     if (ch->mana < 750){ send_to_char("You need more energy to train.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikthree] >= 100 )
     { send_to_char("You have fully trained Chikara 3.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikthree] < 101 )
     {
      ch->mana -= 750;
      send_to_char("\n\r&wYou feel the power rushing through your body as you train, excellent!.\n\r", ch);
      act( AT_YELLOW, "$n trains Chikara 3.", ch, NULL, NULL, TO_ROOM );
      if (chik == 0)
      ch->pcdata->learned[gsn_chikthree] += 1;
     }
    return;
    }
    else if( !str_cmp( argument, "chikara4" ) )
    {
     if ( ch->in_room->vnum != ROOM_VNUM_CHIK4 )
     {
        send_to_char( "You can't train that here.", ch );
        return;
     }
     save_char_obj( ch );
     if (ch->pcdata->learned[gsn_chikfour] == 0) { send_to_char("You don't know Chikara 4.\n\r",ch); return; }
     if (ch->mana < 1000){ send_to_char("You need more energy to train.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikfour] >= 100 )
     { send_to_char("You have fully trained Chikara 4.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikfour] < 101 )
     {
      ch->mana -= 1000;
      send_to_char("\n\r&wYou feel the power rushing through your body as you train,excellent!.\n\r", ch);
      act( AT_YELLOW, "\n\r$n trains Chikara 4.", ch, NULL, NULL, TO_ROOM );
      if (chik == 0)
      ch->pcdata->learned[gsn_chikfour] += 1;
     }
     return;
    }
    else if( !str_cmp( argument, "chikara5" ) )
    {
     if ( ch->in_room->vnum != ROOM_VNUM_CHIK5 )
     {
        send_to_char( "You can't train that here.", ch );
        return;
     }
     save_char_obj( ch );
     if (ch->pcdata->learned[gsn_chikfive] == 0) { send_to_char("You don't know Chikara 5.\n\r",ch); return; }
     if (ch->mana < 1000){ send_to_char("You need more energy to train.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikfive] >= 100 )
     { send_to_char("You have fully trained Chikara 5.\n\r", ch); return;}
     if (ch->pcdata->learned[gsn_chikfive] < 101 )
     {
      if ( ch->mana <=  (ch->max_mana/2))
      {
         send_to_char("\n\rNot enough energy.\n\r", ch);
         return;
      }
      ch->mana -= (ch->max_mana / 2)-1;
      send_to_char("\n\r&wYou feel the power rushing through your body as you train,excellent!.\n\r", ch);
      act( AT_YELLOW, "\n\r$n trains Chikara 5.", ch, NULL, NULL, TO_ROOM );
      if (chik == 0)
      ch->pcdata->learned[gsn_chikfive] += 1;
     }
     return;
    }
    else if( !str_cmp( argument, "primaryattack" ) )
    {
     save_char_obj( ch );
     saving_char = NULL;
     if (!IS_BIODROID(ch)){ send_to_char("Huh?\n\r", ch ); return;}
     if (ch->mana < 50)
     {
      send_to_char("You need more ki.\n\r", ch); return;
     }
     if (ch->pcdata->learned2[gsn_primaryatt] == 0) { send_to_char("Practice it First.\n\r",ch); return; }
     if (ch->pcdata->learned2[gsn_primaryatt] >= 100) {send_to_char("Its at maximum.\n\r",ch); return;}
     if (ch->pcdata->learned2[gsn_primaryatt] < 100 ) {
     send_to_char("\n\r&YYou train your primary attack.\n\r", ch);
     ch->mana -= 50;
     ch->pcdata->learned2[gsn_primaryatt] += 1; }
     if (ch->pcdata->learned2[gsn_primaryatt] == 50 )
     {
       if (xIS_SET(ch->act, PLR_TRIBEAM) || xIS_SET(ch->act, PLR_MASENKO) || xIS_SET(ch->act, PLR_MT) || xIS_SET(ch->act, PLR_SCATTER))
       return;
       result = (sh_int) number_range( 0, 12 );
       if ( result >= 0 && result <= 4  )
       {
        send_to_char("&WAs you train your primary attack,\n\r&Byou feel that you have learned the skill of TRIBEAM!!!\n\r", ch);
        xSET_BIT(ch->act, PLR_TRIBEAM);
        ch->pcdata->learned2[gsn_tribeam] = 100;
        return;
       }
       if ( result >= 5 && result <= 10 )
       {
        send_to_char("&YAs you train your primary attack,\n\r&Ryou feel that you have learned the skill of MASENKO!!!\n\r", ch);
        xSET_BIT(ch->act, PLR_MASENKO);
        ch->pcdata->learned2[gsn_masenko] = 100;
        return;
       }
       if ( result == 11 )
       {
        send_to_char("&zAs you train your primary attack,\n\r&pyou feel that you have learned the skill of SCATTERSHOT!!!\n\r", ch);
        xSET_BIT(ch->act, PLR_SCATTER);
        ch->pcdata->learned2[gsn_scattershot] = 100;
        return;
       }
       if ( result == 12 )
       {
        send_to_char("&wAs you train your primary attack,\n\r&Wyou feel that you have learned the skill of MENTAL THROW!!!\n\r", ch);
        xSET_BIT(ch->act, PLR_MT);
        ch->pcdata->learned2[gsn_mental] = 100;
        return;
       }
     }
    return;
    }
    else if( !str_cmp( argument, "secondary" ) )
    {
     save_char_obj( ch );
     saving_char = NULL;
     if (!IS_MUTANT(ch)){ send_to_char("Huh?\n\r", ch ); return;}
     if (ch->mana < 500)
     {
      send_to_char("You need more ki.\n\r", ch); return;
     }
     if (ch->pcdata->learned2[gsn_secondary] == 0) { send_to_char("Practice it First.\n\r", ch); return; }
     if (ch->pcdata->learned2[gsn_secondary] >= 100) {send_to_char("Its at maximum.\n\r",ch); return;}
     if (ch->pcdata->learned2[gsn_secondary] < 100 ) {
     send_to_char("\n\r&YYou train your secondary skill.\n\r", ch);
     ch->mana -= 500;
     ch->pcdata->learned2[gsn_secondary] += 1; }
     if (ch->pcdata->learned2[gsn_secondary] == 50 )
     {
       if (xIS_SET(ch->act, PLR_AS) || xIS_SET(ch->act, PLR_IS) || xIS_SET(ch->act, PLR_MS) || xIS_SET(ch->act, PLR_EK) || xIS_SET(ch->act, PLR_KC))
       return;
       result = (sh_int) number_range( 0, 8 );
       switch( result = number_range( 0, 8 ) )
       {
         case 1:
	 case 0:
	 case 2:
           send_to_char("&RYou feel that your muscles grow without showing any physical change.\n\r&WYour strength has reached its limits!\n\r", ch);
           xSET_BIT(ch->act, PLR_AS);
	   add_mutate( ch, 1 );
           return;
	 case 3:
         case 4:
         case 8:
           send_to_char("&BYou move your body and you feel that it moves swifter than before.\n\r&WYour speed has increased to its maximum!\n\r", ch );
           xSET_BIT(ch->act, PLR_IS);
           add_mutate( ch, 1 );
           return;
         case 5:
           send_to_char("&GYour energy begins to flow faster throughout your body.\n\r&WYour energy has now increased.\n\r",ch);
           xSET_BIT(ch->act, PLR_KC);
           ch->max_mana *= 1.5;
           add_mutate( ch, 1 );
           return;
         case 6:
           send_to_char("&zA metallic skin begins to cover your entire body, head to toe.\n\r&WYour body has grown a new layer of metallic skin!", ch);
           xSET_BIT(ch->act, PLR_MS);
           add_mutate( ch, 1 );
           return;
         case 7:
           send_to_char("&CYour brain develops further.\n\rYou are blessed with the world's knowledge.\n\r", ch);
           xSET_BIT(ch->act, PLR_EK);
           add_mutate( ch, 1 );
           return;
       }
     }
    return;
    }
    else if( !str_cmp( argument, "primary" ) )
    {
     save_char_obj( ch );
     saving_char = NULL;
     if (!IS_MUTANT(ch)){ send_to_char("Huh?\n\r", ch ); return;}
     if (ch->mana < 50)
     {
      send_to_char("You need more ki.\n\r", ch); return;
     }
     if (ch->pcdata->learned2[gsn_primary] == 0) { send_to_char("Practice it First.\n\r", ch); return; }
     if (ch->pcdata->learned2[gsn_primaryatt] >= 100) {send_to_char("Its at maximum.\n\r",ch); return;}
     if (ch->pcdata->learned2[gsn_primary] < 100 ) {
     send_to_char("\n\r&YYou train your primary skill.\n\r", ch); 
     ch->mana -= 50;
     ch->pcdata->learned2[gsn_primary] += 1; }
     if (ch->pcdata->learned2[gsn_primary] == 50 )
     {
        if (xIS_SET(ch->act, PLR_LS) || xIS_SET(ch->act, PLR_SS) || xIS_SET(ch->act, PLR_MB) || xIS_SET(ch->act, PLR_SK) || xIS_SET(ch->act, PLR_HK))
       return;
       result = (sh_int) number_range( 0, 8 );
       switch( result = number_range( 0, 8 ) )
       {
         case 0:
         case 1:
         case 2:
           send_to_char("&RYou grow in size as your muscles develop further.\n\rYour strength has become enormous, but your speed has decreased.\n\r", ch);
           xSET_BIT(ch->act, PLR_LS);
           add_mutate( ch, 1 );
           ch->weight += number_range( 50, 80);
           ch->height += number_range(9, 12);
           return;
         case 3:
	 case 4:
	 case 8:
           send_to_char("&BYou shrink in size as your speed develops further.\n\rYou can't be touched now, but you ain't that strong anymore.\n\r", ch );
           xSET_BIT(ch->act, PLR_SS);
           add_mutate( ch, 1 );
           ch->weight -= number_range( 50, 80);
           ch->height -= number_range(9, 12);
           return;
         case 5:
           send_to_char("&wYour body has develop further its power to use ki.\n\rYour energy has now increased.\n\r", ch);
           xSET_BIT(ch->act, PLR_HK);
           add_mutate( ch, 1 );
           ch->max_mana *= 1.5;
           return;
         case 6:
           send_to_char("&OAs your body mutates you feel the essence of stone overwhelm you.\n\rAs you look at your self, you notice it has turned into a tissue harder\n\rthan rock.", ch);
           xSET_BIT(ch->act, PLR_SK);
           return;
         case 7:
           send_to_char("&CYour brain develops further.\n\rYou are now wiser and more intelligent than before.\n\r", ch);
           xSET_BIT(ch->act, PLR_MB);
           add_mutate( ch, 1 );
           ch->stat += 1;
           return;
       }
     }
    return;
    }
    else 
    {

            send_to_char( "\n\rTrain what?\n\r", ch );
        return;
    }

    if ( *pAbility >= 25  && (!str_cmp( argument, "str" ) || !str_cmp( argument, "int" ) || !str_cmp( argument, "wis"  ) || !str_cmp( argument, "dex" ) || !str_cmp( argument, "con" ) || !str_cmp( argument, "cha" ) || !str_cmp( argument, "lck" ) ))  /* 25 is the max you can train a stat */
   {
        act(AT_BLUE, "\n\rYour $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
        return;
    }

    if ( cost > ch->stat )
    {
        send_to_char( "\n\rYou don't have enough trains.\n\r", ch );
        return;
    }

    ch->stat       -= cost;
    *pAbility           += 1;
         act(AT_YELLOW, "\n\rYour $T increases!", ch, NULL, pOutput, TO_CHAR );
         act(AT_YELLOW, "\n\r$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
         return;
}

void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    set_char_color( AT_YELLOW, ch );
    one_argument( argument, arg );
    if ( !str_cmp( arg, "max" ) )
    {
      if ( IS_PKILL( ch ) )
	wimpy = (int) ch->max_hit / 2.25;
      else
	wimpy = (int) ch->max_hit / 1.2;
    }
    else    
    if ( arg[0] == '\0' )
      wimpy = (int) ch->max_hit / 5;
    else
      wimpy = atoi( arg );

    if ( wimpy < 0 ) {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }
    if ( IS_PKILL( ch ) && wimpy > (int) ch->max_hit / 2.25 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }
    else if ( wimpy > (int) ch->max_hit / 1.2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }
    ch->wimpy	= wimpy;
    ch_printf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char log_buf[MAX_STRING_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <new> <again>.\n\r", ch );
	send_to_char( "Syntax: password <new> <again>.\n\r", ch );
	return;
    }

/*
    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }
*/

/* This should stop all the mistyped password problems --Shaddai */
    if ( strcmp( arg1, arg2 ))
    {
	send_to_char("Passwords don't match try again.\n\r", ch );
	return;
    }
    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

	if (arg1[0] == '!' || arg2[0] == '!')
	 {
	  send_to_char( "New password cannot begin with the '!' character.",ch);
	  return;
	 }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    DISPOSE( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    if ( IS_SET(sysdata.save_flags, SV_PASSCHG) )
	save_char_obj( ch );
    if ( ch->desc && ch->desc->host[0]  != '\0' )
       sprintf(log_buf, "%s changing password from site %s\n", ch->name, 
       		ch->desc->host );
    else
       sprintf(log_buf, "%s changing thier password with no descriptor!", ch->name);
    log_string( log_buf );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    int iHash;
    int col = 0;
    SOCIALTYPE *social;

    set_char_color( AT_PLAIN, ch );
    for ( iHash = 0; iHash < 27; iHash++ )
	for ( social = social_index[iHash]; social; social = social->next )
	{
	    ch_printf( ch, "%-12s", social->name );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}


void do_commands( CHAR_DATA *ch, char *argument )
{
    int col;
    bool found;
    int hash;
    CMDTYPE *command;

    col = 0;
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
     send_to_char("\n\rCurrent Mortal Commands:\n\r\n\r", ch);
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_IMMORTAL
		&&   command->level <= get_trust( ch )
		&&  (command->name[0] != 'm'
		||   command->name[1] != 'p') )
		{
		    ch_printf( ch, "%-12s", command->name );
		    if ( ++col % 6 == 0 )
			send_to_char( "\n\r", ch );
		}
	if ( col % 6 != 0 )
	    send_to_char( "\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_IMMORTAL
		&&   command->level <= get_trust( ch )
		&&  !str_prefix(argument, command->name)
		&&  (command->name[0] != 'm'
		||   command->name[1] != 'p') )
		{
		    ch_printf( ch, "%-12s", command->name );
		    found = TRUE;
		    if ( ++col % 6 == 0 )
			send_to_char( "\n\r", ch );
		}

	if ( col % 6 != 0 )
	    send_to_char( "\n\r", ch );
	if ( !found )
	    ch_printf( ch, "No command found under %s.\n\r", argument);
    }
    return;
}

void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    one_argument( argument, arg );

    if ( IS_NPC( ch ) )
	return;

    if ( arg[0] == '\0' )
    {
        if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
        {
	    set_char_color( AT_GREEN, ch );
            send_to_char( "You are silenced.\n\r", ch );
            return;
        }
    
        send_to_char_color( "\n\r &gPublic channels  (severe penalties for abuse)&G:\n\r  ", ch );
	ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_RACETALK )?
						" &G+RACETALK" :
						" &g-racetalk" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_CHAT )    ?
				                " &G+CHAT" 	          :
						" &g-chat" );
        if ( get_trust( ch ) > 2 && !NOT_AUTHED( ch ) )
          ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_AUCTION ) ?
				                " &G+AUCTION" 	          :
						" &g-auction" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_QUEST )   ?
				                " &G+QUEST" 	          :
						" &g-quest" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_WARTALK ) ?
				                " &G+WARTALK" 	          :
						" &g-wartalk" );
        if ( IS_HERO( ch ) )
          ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_AVTALK )  ?
				                " &G+AVATAR"	          :
						" &g-avatar" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_MUSIC )   ?
				                " &G+MUSIC" 	          :
						" &g-music" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_ASK )     ?
					        " &G+ASK"                 :
						" &g-ask" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_SHOUT )   ?
						" &G+SHOUT"	          :
						" &g-shout" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_YELL )    ?
				                " &G+YELL"	          :
						" &g-yell" );
       ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_OOC )      ?
				                " &G+OOC"	          :
						" &g-ooc" );
       ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_GEMOTE)    ?
				                " &G+GEMOTE"	          :
						" &g-GEMOTE" );
  	ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_SPORTS )  ?
						" &G+SPORTS" 		  :
						" &g-sports" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_INFO )  ?
                                                " &G+INFO"              :
                                                " &g-info" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_INFO2 )  ?
                                                " &G+INFO2"              :
                                                " &g-info2" );

        send_to_char_color( "\n\r &gPrivate channels (severe penalties for abuse)&G:\n\r ", ch );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_TELLS )   ?
                                                " &G+TELLS"               :
                                                " &g-tells" );
        ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_WHISPER )   ?
						" &G+WHISPER"		  :
						" &g-whisper" );
        if ( !IS_NPC( ch ) && ch->pcdata->clan )
        {
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_CLAN )  ?
		" &G+CLAN"	:	" &g-clan", ch );
        }
        if ( IS_IMMORTAL(ch))
          ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_NEWBIE) ?
    				 	        " &G+NEWBIE"	     	  :
						" &g-newbie" );

	/* Immortal channels */
        if ( IS_IMMORTAL( ch ) )
        {
            send_to_char_color( "\n\r &gImmortal Channels&G:\n\r  ", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_IMMTALK )    ?
		" &G+IMMTALK"	:	" &g-immtalk", ch );
/*          send_to_char_color( !IS_SET( ch->deaf, CHANNEL_PRAY )       ?
		" &G+PRAY"	:	" &g-pray", ch ); */
            if ( get_trust( ch ) >= sysdata.muse_level )
              send_to_char_color( !IS_SET( ch->deaf, CHANNEL_HIGHGOD )  ?
		" &G+MUSE"	:	" &g-muse", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_MONITOR )    ?
		" &G+MONITOR"	:	" &g-monitor", ch );
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_AUTH )	?
	       " &G+AUTH"	:	" &g-auth", ch );	

        }
        if ( get_trust( ch ) >= sysdata.log_level )
        {
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_LOG ) 	?
		" &G+LOG"	:	" &g-log", ch);
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_BUILD)       ?
		" &G+BUILD"	:	" &g-build", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_COMM ) 	?
		" &G+COMM"	:	" &g-comm", ch );
            send_to_char_color( !IS_SET (ch->deaf, CHANNEL_WARN)
                        ? " &G+WARN" : " &g-warn", ch);
            if ( get_trust( ch ) >= sysdata.think_level )
              send_to_char_color( !IS_SET( ch->deaf, CHANNEL_HIGH ) 	?
		" &G+HIGH"	:	" &g-high", ch );
        }
        if ( ch->level > 65 )
        {
            ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_ADMIN )  ?
                                                    " &G+ADMIN"              :
                                                    " &g-admin" );
        }
        send_to_char( "\n\r", ch );   
    }
    else
    {
	bool fClear;
	bool ClearAll;
	int bit;

        bit=0;
        ClearAll = FALSE;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
	else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "clan"     ) ) bit = CHANNEL_CLAN;
	else if ( !str_cmp( arg+1, "quest"    ) ) bit = CHANNEL_QUEST;
	else if ( !str_cmp( arg+1, "tells"    ) ) bit = CHANNEL_TELLS;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
        else if ( !str_cmp( arg+1, "admin"    ) ) bit = CHANNEL_ADMIN;
	else if ( !str_cmp( arg+1, "log"      ) ) bit = CHANNEL_LOG;
	else if ( !str_cmp( arg+1, "build"    ) ) bit = CHANNEL_BUILD;
	else if ( !str_cmp( arg+1, "high"     ) ) bit = CHANNEL_HIGH;
	else if ( !str_cmp( arg+1, "pray"     ) ) bit = CHANNEL_PRAY;
	else if ( !str_cmp( arg+1, "avatar"   ) ) bit = CHANNEL_AVTALK;
	else if ( !str_cmp( arg+1, "monitor"  ) ) bit = CHANNEL_MONITOR;
	else if ( !str_cmp( arg+1, "auth"     ) ) bit = CHANNEL_AUTH;
	else if ( !str_cmp( arg+1, "newbie"   ) ) bit = CHANNEL_NEWBIE;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "muse"     ) ) bit = CHANNEL_HIGHGOD;
	else if ( !str_cmp( arg+1, "ask"      ) ) bit = CHANNEL_ASK;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "ooc"      ) ) bit = CHANNEL_OOC;
	else if ( !str_cmp( arg+1, "gemote"   ) ) bit = CHANNEL_GEMOTE;
	else if ( !str_cmp( arg+1, "comm"     ) ) bit = CHANNEL_COMM;
        else if ( !str_cmp (arg+1, "warn"     ) ) bit = CHANNEL_WARN;
        else if ( !str_cmp( arg+1, "wartalk"  ) ) bit = CHANNEL_WARTALK;
	else if ( !str_cmp( arg+1, "whisper"  ) ) bit = CHANNEL_WHISPER;
	else if ( !str_cmp( arg+1, "racetalk" ) ) bit = CHANNEL_RACETALK;
	else if ( !str_cmp( arg+1, "sports"   ) ) bit = CHANNEL_SPORTS;
        else if ( !str_cmp( arg+1, "info"   ) ) bit = CHANNEL_INFO;
        else if ( !str_cmp( arg+1, "info2"   ) ) bit = CHANNEL_INFO2;
	else if ( !str_cmp( arg+1, "all"      ) ) ClearAll = TRUE;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if (( fClear ) && ( ClearAll ))
	{
	    REMOVE_BIT (ch->deaf, CHANNEL_RACETALK);
            REMOVE_BIT (ch->deaf, CHANNEL_AUCTION);
            REMOVE_BIT (ch->deaf, CHANNEL_CHAT);
            REMOVE_BIT (ch->deaf, CHANNEL_QUEST);
            REMOVE_BIT (ch->deaf, CHANNEL_WARTALK);
            REMOVE_BIT (ch->deaf, CHANNEL_PRAY);
            REMOVE_BIT (ch->deaf, CHANNEL_MUSIC);
            REMOVE_BIT (ch->deaf, CHANNEL_ASK);
            REMOVE_BIT (ch->deaf, CHANNEL_SHOUT);
            REMOVE_BIT (ch->deaf, CHANNEL_YELL);
	    REMOVE_BIT (ch->deaf, CHANNEL_SPORTS);
            REMOVE_BIT (ch->deaf, CHANNEL_OOC);
            REMOVE_BIT (ch->deaf, CHANNEL_GEMOTE);
            REMOVE_BIT (ch->deaf, CHANNEL_INFO);
            REMOVE_BIT (ch->deaf, CHANNEL_INFO2);

            if (ch->pcdata->clan)
              REMOVE_BIT (ch->deaf, CHANNEL_CLAN);

            if (ch->level >= LEVEL_IMMORTAL)
              REMOVE_BIT (ch->deaf, CHANNEL_AVTALK);
 
	    /*
	    if (ch->level >= sysdata.log_level )
	      REMOVE_BIT (ch->deaf, CHANNEL_COMM);
	    */

        } else if ((!fClear) && (ClearAll))
        {
	    SET_BIT (ch->deaf, CHANNEL_RACETALK);
            SET_BIT (ch->deaf, CHANNEL_AUCTION);
            SET_BIT (ch->deaf, CHANNEL_CHAT);
            SET_BIT (ch->deaf, CHANNEL_QUEST);
            SET_BIT (ch->deaf, CHANNEL_PRAY);
            SET_BIT (ch->deaf, CHANNEL_MUSIC);
            SET_BIT (ch->deaf, CHANNEL_ASK);
            SET_BIT (ch->deaf, CHANNEL_SHOUT);
	    SET_BIT (ch->deaf, CHANNEL_WARTALK);
            SET_BIT (ch->deaf, CHANNEL_YELL);
	    SET_BIT (ch->deaf, CHANNEL_SPORTS);
            SET_BIT (ch->deaf, CHANNEL_OOC);
            SET_BIT (ch->deaf, CHANNEL_GEMOTE);
            SET_BIT (ch->deaf, CHANNEL_INFO);
            SET_BIT (ch->deaf, CHANNEL_INFO2);

            if (ch->pcdata->clan)
              SET_BIT (ch->deaf, CHANNEL_CLAN);

            if (ch->level >= LEVEL_IMMORTAL)
              SET_BIT (ch->deaf, CHANNEL_AVTALK);

	    /*
	    if (ch->level >= sysdata.log_level)
	      SET_BIT (ch->deaf, CHANNEL_COMM);
	    */

         } else if (fClear)
         {
	    REMOVE_BIT (ch->deaf, bit);
         } else
         {
	    SET_BIT    (ch->deaf, bit);
         }

	  send_to_char( "Ok.\n\r", ch );
    }

    return;
}


/*
 * display WIZLIST file						-Thoric
 */
void do_wizlist( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_IMMORT, ch );
    show_file( ch, WIZLIST_FILE );
}

/*
 * Contributed by Grodyn.
 * Display completely overhauled, 2/97 -- Blodkai
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
 
    if ( IS_NPC(ch) )
        return;
 
    one_argument( argument, arg );
 
    set_char_color( AT_GREEN, ch );
 
    if ( arg[0] == '\0' )
    {
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\rConfigurations ", ch );
      set_char_color( AT_GREEN, ch );
      send_to_char( "(use 'config +/- <keyword>' to toggle, see 'help config')\n\r\n\r", ch );
      set_char_color( AT_DGREEN, ch );
      send_to_char( "Display:   ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s   %-12s\n\r           %-12s   %-12s   %-12s   %-12s\n\r           %-12s   %-12s   %-12s",
        xIS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) 	? "[+] PAGER"
                                                    	: "[-] pager",
        xIS_SET( ch->pcdata->flags, PCFLAG_GAG )     	? "[+] GAG"
                                                    	: "[-] gag",
        xIS_SET(ch->act, PLR_BRIEF )                	? "[+] BRIEF"
                                                    	: "[-] brief",
        xIS_SET(ch->act, PLR_COMBINE )              	? "[+] COMBINE"
                                                    	: "[-] combine",
        xIS_SET(ch->act, PLR_BLANK )                	? "[+] BLANK"
                                                    	: "[-] blank",
        xIS_SET(ch->act, PLR_PROMPT )               	? "[+] PROMPT"
                                                    	: "[-] prompt",
        xIS_SET(ch->act, PLR_ANSI )                 	? "[+] ANSI"
                                                    	: "[-] ansi",
        xIS_SET( ch->pcdata->flags, PCFLAG_SUPER )       ? "[+] SUPERGAG"
                                                        : "[-] supergag",
        xIS_SET(ch->act, PLR_RIP )                  	? "[+] RIP"
                                                        : "[-] rip",
        xIS_SET(ch->act, PLR_AUTOMAP )                  ? "[+] COMPASS"
                                                        : "[-] compass",
        xIS_SET(ch->pcdata->flags, PCFLAG_NOLINE )      ? "[+] NOLINE"
                                                        : "[-] noline" );
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rScore:     ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s   %-12s",
        xIS_SET(ch->pcdata->flags, PCFLAG_STAT  )       ? "[+] STAT"
                                                        : "[-] stat",
        xIS_SET(ch->pcdata->flags, PCFLAG_COMBAT )      ? "[+] COMBAT"
                                                        : "[-] combat",
        xIS_SET(ch->pcdata->flags, PCFLAG_INFO )        ? "[+] INFO"
                                                        : "[-] info",
        xIS_SET(ch->pcdata->flags, PCFLAG_PKILL )       ? "[+] PKILL"
                                                        : "[-] pkill" );
      ch_printf( ch, "\n\r           %-12s   ",
        xIS_SET(ch->pcdata->flags, PCFLAG_CLAN  )       ? "[+] CLAN"
                                                        : "[-] clan");


      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rAuto:      ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s   %-12s",
        xIS_SET(ch->act, PLR_AUTOSAC  )             	? "[+] AUTOSAC"
                                                    	: "[-] autosac",
        xIS_SET(ch->act, PLR_AUTOGOLD )             	? "[+] AUTOGOLD"
                                                    	: "[-] autogold",
        xIS_SET(ch->act, PLR_AUTOLOOT )             	? "[+] AUTOLOOT"
                                                    	: "[-] autoloot",
        xIS_SET(ch->act, PLR_AUTOEXIT )             	? "[+] AUTOEXIT"
                                                    	: "[-] autoexit" );
 
      set_char_color( AT_DGREEN, ch );
 
 
      set_char_color( AT_DGREEN, ch );
 
 
      if ( IS_IMMORTAL( ch ) )
      {
	set_char_color( AT_DGREEN, ch );
        send_to_char( "\n\r\n\rImmortal:  ", ch );
        set_char_color( AT_GREY, ch );
        ch_printf( ch, "%-12s   %-12s   %-12s   %-12s",
	  xIS_SET(ch->act, PLR_ROOMVNUM ) 		? "[+] ROOMVNUM"
							: "[-] roomvnum",
          xIS_SET(ch->pcdata->flags, PCFLAG_AUTOFLAGS )    ? "[+] ROOMFLAGS"
                                                        : "[-] roomflags",
          xIS_SET(ch->pcdata->flags, PCFLAG_SECTORD )      ? "[+] SECTORTYPES"
                                                        : "[-] sectortypes",
          xIS_SET(ch->pcdata->flags, PCFLAG_ANAME )        ? "[+] FILENAME"
                                                        : "[-] filename" );

      }
  	/* Config option for Room Flag display added by Samson 12-10-97 */
	/* Config option for Sector Type display added by Samson 12-10-97 */
	/* Config option Area name and filename added by Samson 12-13-97 */
     /* if( IS_IMMORTAL( ch ) )
      {
	  set_char_color( AT_DGREEN, ch );
        send_to_char( "\n\rMore Immortal toggles:  ", ch );
        set_char_color( AT_GREY, ch );
        ch_printf( ch, "Roomflags [%s] Sectortypes [%s] Filename [%s]\n\r",
        xIS_SET(ch->pcdata->flags, PCFLAG_AUTOFLAGS) ? "+"
							          : " ",
	  xIS_SET(ch->pcdata->flags, PCFLAG_SECTORD)   ? "+"
							          : " ",
	  xIS_SET(ch->pcdata->flags, PCFLAG_ANAME)     ? "+"
								    : " " );
      }*/

      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rSentences imposed on you (if any):", ch );
      set_char_color( AT_YELLOW, ch );
      ch_printf( ch, "\n\r%s%s%s%s%s%s",
          xIS_SET(ch->act, PLR_SILENCE )  ?
            " For your abuse of channels, you are currently silenced.\n\r" : "",
          xIS_SET(ch->act, PLR_NO_EMOTE ) ?
            " The gods have removed your emotes.\n\r"                      : "", 
          xIS_SET(ch->act, PLR_NO_TELL )  ?
            " You are not permitted to send 'tells' to others.\n\r"        : "", 
          xIS_SET(ch->act, PLR_LITTERBUG )?
            " A convicted litterbug.  You cannot drop anything.\n\r"       : "", 
          xIS_SET(ch->act, PLR_THIEF )    ?
            " A proven thief, you will be hunted by the authorities.\n\r"  : "", 
          xIS_SET(ch->act, PLR_KILLER )   ?
            " For the crime of murder you are sentenced to death...\n\r"   : "" );
    }
    else
    {
	bool fSet;
	int bit = 0;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

	     if ( !str_prefix( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_prefix( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_prefix( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_prefix( arg+1, "autogold" ) ) bit = PLR_AUTOGOLD;
	else if ( !str_prefix( arg+1, "blank"    ) ) bit = PLR_BLANK;


	else if ( !str_prefix( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_prefix( arg+1, "combine"  ) ) bit = PLR_COMBINE;
	else if ( !str_prefix( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_prefix( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else if ( !str_prefix( arg+1, "ansi"     ) ) bit = PLR_ANSI;
	else if ( !str_prefix( arg+1, "rip"      ) ) bit = PLR_RIP;
        else if ( !str_prefix( arg+1, "compass"     ) ) bit = PLR_AUTOMAP;

	else if ( IS_IMMORTAL( ch )
	     &&  !str_prefix( arg+1, "vnum"     ) ) bit = PLR_ROOMVNUM;
        else if ( IS_IMMORTAL( ch )
             &&  !str_prefix( arg+1, "roomvnum" ) ) bit = PLR_ROOMVNUM;
	if (bit)
        {
  	  if ( (bit == PLR_FLEE || bit == PLR_NICE || bit == PLR_SHOVEDRAG) 
	  &&  xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
          {
	    send_to_char( "Pkill characters can not config that option.\n\r", ch );
	    return;
          }
          if( (bit == PLR_ANSI) && xIS_SET(ch->act, PLR_ANSI ))
            ch->desc->ansi = FALSE;
          else if( (bit == PLR_ANSI) && !xIS_SET(ch->act, PLR_ANSI))
	    ch->desc->ansi  = TRUE;

	  if ( fSet )
	    xSET_BIT   (ch->act, bit);
	  else
	    xREMOVE_BIT(ch->act, bit);
	  send_to_char( "Ok.\n\r", ch );
          return;
        }
        else
        {
	       if ( !str_prefix( arg+1, "nointro"  ) ) bit = PCFLAG_NOINTRO;
          else if ( !str_prefix( arg+1, "gag"      ) ) bit = PCFLAG_GAG;
          else if ( !str_prefix( arg+1, "supergag" ) ) bit = PCFLAG_SUPER; 
          else if ( !str_prefix( arg+1, "char"    ) ) bit = PCFLAG_PAGERON;
          else if ( !str_prefix( arg+1, "groupwho" ) ) bit = PCFLAG_GROUPWHO;
          else if ( !str_prefix( arg+1, "stat"    ) ) bit = PCFLAG_STAT;
          else if ( !str_prefix( arg+1, "info"  ) ) bit = PCFLAG_INFO;
          else if ( !str_prefix( arg+1, "combat"   ) ) bit = PCFLAG_COMBAT;
          else if ( !str_prefix( arg+1, "pkill" ) ) bit = PCFLAG_PKILL;
          else if ( !str_prefix( arg+1, "clan"     ) ) bit = PCFLAG_CLAN;
          else if ( !str_prefix( arg+1, "noline"  ) ) bit = PCFLAG_NOLINE;

	    else if ( IS_IMMORTAL ( ch )
               && ( !str_prefix( arg+1, "roomflags" ) ) ) bit = PCFLAG_AUTOFLAGS;
	    else if ( IS_IMMORTAL ( ch )
		   && ( !str_prefix( arg+1, "sectortypes" ) ) ) bit = PCFLAG_SECTORD;
	    else if ( IS_IMMORTAL ( ch )
		   && ( !str_prefix( arg+1, "filename" ) ) ) bit = PCFLAG_ANAME;

	  else if ( !str_prefix( arg+1, "@hgflag_" ) ) bit = PCFLAG_HIGHGAG;
          else
	  {
	    send_to_char( "Config which option?\n\r", ch );
	    return;
    	  }

          if ( fSet )
	    xSET_BIT    (ch->pcdata->flags, bit);
	  else
	    xREMOVE_BIT (ch->pcdata->flags, bit);

	  send_to_char( "Ok.\n\r", ch );
          return;
        }
    }

    return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
  do_help( ch, "credits" );
}


extern int top_area;

/*
void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;

    iAreaHalf = (top_area + 1) / 2;
    pArea1    = first_area;
    pArea2    = first_area;
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	ch_printf( ch, "%-39s%-39s\n\r",
	    pArea1->name, pArea2 ? pArea2->name : "" );
	pArea1 = pArea1->next;
	if ( pArea2 )
	    pArea2 = pArea2->next;
    }

    return;
}
*/

/* 
 * New do_areas with soft/hard level ranges 
void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    set_char_color( AT_PLAIN, ch );
    send_to_char("\n\r   Author    |             Area                     | Recommended |  Enforced\n\r", ch);
    send_to_char("-------------+--------------------------------------+-------------+-----------\n\r", ch);

    for ( pArea = first_area; pArea; pArea = pArea->next )
	ch_printf(ch, "%-12s | %-36s | %4d - %-4d | %3d - %-3d \n\r", 
	 	pArea->author, pArea->name, pArea->low_soft_range, 
		pArea->hi_soft_range, pArea->low_hard_range, 
		pArea->hi_hard_range);
    return;
}
*/

/*
 * New do_areas, written by Fireblade, last modified - 4/27/97
 *
 *   Syntax: area            ->      lists areas in alphanumeric order
 *           area <a>        ->      lists areas with soft max less than
 *                                                    parameter a
 *           area <a> <b>    ->      lists areas with soft max bewteen
 *                                                    numbers a and b
 *           area old        ->      list areas in order loaded
 *
 */
void do_areas( CHAR_DATA *ch, char *argument )
{ 
    char *header_string1 = "\n\r   Author    |             Area"
                                    "                     | "
                                    "Recommended |  Enforced\n\r";
    char *header_string2 = "-------------+-----------------"
                                    "---------------------+----"
                                    "---------+-----------\n\r";
    char *print_string = "%-12s | %-36s | %4d - %-4d | %3d - "
                                    "%-3d \n\r";
 
    AREA_DATA *pArea;
    int lower_bound = 0;
    int upper_bound = MAX_LEVEL + 1;
    /* make sure is to init. > max area level */
    char arg[MAX_STRING_LENGTH];
   
    argument = one_argument(argument,arg);
        
    if(arg[0] != '\0')
    {
      if(!is_number(arg))
      {
        if(!strcmp(arg,"old"))
        {
          set_char_color( AT_PLAIN, ch );
          send_to_char(header_string1, ch);
          send_to_char(header_string2, ch);
          for (pArea = first_area; pArea;pArea = pArea->next)
          {   
            ch_printf(ch, print_string,
              pArea->author, pArea->name,
              pArea->low_soft_range,
              pArea->hi_soft_range,
              pArea->low_hard_range,
              pArea->hi_hard_range);
          }  
          return;
        }
        else
        {
          send_to_char("Area may only be followed by numbers, or 'old'.\n\r", ch);
          return;
        }
      }
                                 
      upper_bound = atoi(arg);
      lower_bound = upper_bound;
                                                
      argument = one_argument(argument,arg);
                                                
      if(arg[0] != '\0')
      {
        if(!is_number(arg))
        {
          send_to_char("Area may only be followed by numbers.\n\r", ch);
          return;
        }
                                
        upper_bound = atoi(arg);
                 
        argument = one_argument(argument,arg);
        if(arg[0] != '\0')
        {
          send_to_char("Only two level numbers allowed.\n\r",ch);
          return;
        }
      }
    }
                         
    if(lower_bound > upper_bound)
    {
      int swap = lower_bound;
      lower_bound = upper_bound;
      upper_bound = swap;
    }
 
    set_char_color( AT_PLAIN, ch );
    send_to_char(header_string1, ch);
    send_to_char(header_string2, ch);
                                
    for (pArea = first_area_name; pArea; pArea = pArea->next_sort_name)
    {
      if (pArea->hi_soft_range >= lower_bound
      &&  pArea->low_soft_range <= upper_bound)
      {
        ch_printf(ch, print_string,
          pArea->author, pArea->name,
          pArea->low_soft_range,
          pArea->hi_soft_range,
          pArea->low_hard_range,
          pArea->hi_hard_range);
      }
    }
    return;
}

void do_afk( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;
     
     if xIS_SET(ch->act, PLR_AFK)
     {
    	xREMOVE_BIT(ch->act, PLR_AFK);
	send_to_char( "You are no longer afk.\n\r", ch );
/*	act(AT_GREY,"$n is no longer afk.", ch, NULL, NULL, TO_ROOM);*/
	act(AT_GREY,"$n is no longer afk.", ch, NULL, NULL, TO_CANSEE);
     }
     else
     {
	xSET_BIT(ch->act, PLR_AFK);
	send_to_char( "You are now afk.\n\r", ch );
/*	act(AT_GREY,"$n is now afk.", ch, NULL, NULL, TO_ROOM);*/
	act(AT_GREY,"$n is now afk.", ch, NULL, NULL, TO_CANSEE);
	return;
     }
         
}
/*
void do_slist( CHAR_DATA *ch, char *argument )
{
   int sn, i, lFound;
   char skn[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH]; 
   int lowlev, hilev;
   sh_int lasttype = SKILL_SPELL;

   if ( IS_NPC(ch) )
     return;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   lowlev=1;
   hilev=50;

   if (arg1[0]!='\0')
      lowlev=atoi(arg1);

   if ((lowlev<1) || (lowlev>LEVEL_IMMORTAL))
      lowlev=1;

   if (arg2[0]!='\0')
      hilev=atoi(arg2);

   if ((hilev<0) || (hilev>=LEVEL_IMMORTAL))
      hilev=LEVEL_HERO;

   if(lowlev>hilev)
      lowlev=hilev;

   set_char_color( AT_MAGIC, ch );
   send_to_char("\n\rSKILL LIST\n\r",ch);
   send_to_char("------------------\n\r",ch);

   for (i=lowlev; i <= hilev; i++)
   {
	lFound= 0;
	sprintf(skn,"Spell");   
	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;

	    if ( skill_table[sn]->type != lasttype )
	    {
		lasttype = skill_table[sn]->type;
		strcpy( skn, skill_tname[lasttype] );
	    }

	    if ( ch->pcdata->learned[sn] <= 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL) )
		continue;
	    if( i==SKILL_TABLE_LEVEL(ch,sn)  )
	    {
		if( !lFound )
		{
		    lFound=1;
		}
                switch (skill_table[sn]->minimum_position)
                {
     default:
        sprintf(buf, "Invalid");
	bug( "do_slist: skill with invalid minpos, skill=%s",
		   skill_table[sn]->name );
        break;
                    case POS_DEAD:
                            sprintf(buf, "any");
                            break;
                    case POS_MORTAL:
                            sprintf(buf, "mortally wounded");
                            break;
                    case POS_INCAP:
                            sprintf(buf, "incapacitated");
                            break;
                    case POS_STUNNED:
                            sprintf(buf, "stunned");
                            break;
                    case POS_SLEEPING:
                            sprintf(buf, "sleeping");
                            break;
                    case POS_RESTING:
                            sprintf(buf, "resting");
                            break;
                    case POS_STANDING:
                            sprintf(buf, "standing");
                            break;
                    case POS_FIGHTING:
                            sprintf(buf, "fighting");
                            break;
                    case POS_MOUNTED:
                            sprintf(buf, "mounted");
                            break;
                    case POS_SITTING:
                            sprintf(buf, "sitting");
                            break;
                }

		ch_printf(ch, "&C%7s&c: &W%-20.20s   &CCurrent&c: &Y%-3d &CMax&c: &Y%-3d  \n\r",
                        skn, skill_table[sn]->name, 
                        ch->pcdata->learned[sn],
                        SKILL_TABLE_ADEPT(ch,sn));
	    }
	}
   }
   return;
}
*/
void do_whois( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg1 );


  if(IS_NPC(ch))
    return;

  if(arg1[0] == '\0')
  {
    send_to_char("You must input the name of an online character.\n\r", ch);
    return;
  }

  if( ( ( victim = get_char_world(ch, arg1) ) == NULL ))
  {
    send_to_char("No such character online.\n\r", ch);
    return;
  }

  if(IS_NPC(victim))
  return;


  ch_printf(ch, "\n\r &w%s%s.&w\n\r&w %s is a %s %s , %d years of age.\n\r",
	victim->name,
	victim->pcdata->title,
        victim->sex == SEX_MALE ? "He" :
        victim->sex == SEX_FEMALE ? "She" : "It",
	victim->sex == SEX_MALE ? "male" : 
	victim->sex == SEX_FEMALE ? "female" : "neutral",
	capitalize(race_table[victim->race]->race_name),
	get_age(victim) );

  ch_printf(ch, " %s is a %sdeadly player",
	victim->sex == SEX_MALE ? "He" :
	victim->sex == SEX_FEMALE ? "She" : "It",
	xIS_SET(victim->pcdata->flags, PCFLAG_DEADLY) ? "" : "non-");

  if ( victim->pcdata->clan )
  {
	   send_to_char( ", and belongs to Clan ", ch );
  	   send_to_char( victim->pcdata->clan->name, ch );
  }
  send_to_char( ".\n\r", ch );

  if(victim->pcdata->bio && victim->pcdata->bio[0] != '\0')
    ch_printf(ch, " %s's personal bio:\n\r%s",
	victim->name,
	victim->pcdata->bio);
  else
    ch_printf(ch, " %s has yet to create a bio.\n\r",
	victim->name );

  if(IS_IMMORTAL(ch))
  {  
    send_to_char("-------------------\n\r", ch);
    send_to_char("Info for immortals:\n\r", ch);

    if ( victim->pcdata->authed_by && victim->pcdata->authed_by[0] != '\0' )
    ch_printf(ch, "%s was authorized by %s.\n\r", victim->name, victim->pcdata->authed_by );
    ch_printf(ch, "%s has killed %d mobiles, and been killed by a mobile %d times.\n\r",
		victim->name, victim->pcdata->mkills, victim->pcdata->mdeaths );
    if ( victim->pcdata->pkills || victim->pcdata->pdeaths )
	ch_printf(ch, "%s has killed %d players, and been killed by a player %d times.\n\r",
		victim->name, victim->pcdata->pkills, victim->pcdata->pdeaths );
    if ( victim->pcdata->illegal_pk )
	ch_printf(ch, "%s has committed %d illegal player kills.\n\r",
		victim->name, victim->pcdata->illegal_pk );
    
    
    ch_printf(ch, "%s is %shelled at the moment.\n\r",
	victim->name, 
	(victim->pcdata->release_date == 0) ? "not " : "");

    if(victim->pcdata->release_date != 0)
      ch_printf(ch, "%s was helled by %s, and will be released on %24.24s.\n\r",
	victim->sex == SEX_MALE ? "He" :
	victim->sex == SEX_FEMALE ? "She" : "It",
        victim->pcdata->helled_by,
	ctime(&victim->pcdata->release_date));

    ch_printf(ch, "%s is %ssilenced at the moment.\n\r",
        victim->name,
        (victim->pcdata->silenced_date == 0) ? "not " : "");

    if(victim->pcdata->silenced_date != 0)
      ch_printf(ch, "%s was silenced by %s, and will be silenced on %24.24s.\n\r",
        victim->sex == SEX_MALE ? "He" :
        victim->sex == SEX_FEMALE ? "She" : "It",
        victim->pcdata->silenced_by,
        ctime(&victim->pcdata->silenced_date));
    
    
    
    if(get_trust(victim) < get_trust(ch))
    {
      sprintf(buf2, "list %s", buf);
      do_comment(ch, buf2);
    }

  if ( victim->desc && victim->desc->host[0]!='\0' )   
     {
     sprintf(buf2, "%s's IP info: %s ", victim->name, victim->desc->host);
     send_to_char(buf2, ch);
     }
  }
}

void do_pager( CHAR_DATA *ch, char *argument )
{
}

/*
 * The ignore command allows players to ignore up to MAX_IGN
 * other players. Players may ignore other characters whether
 * they are online or not. This is to prevent people from
 * spamming someone and then logging off quickly to evade
 * being ignored.
 * Syntax:
 *	ignore		-	lists players currently ignored
 *	ignore none	-	sets it so no players are ignored
 *	ignore <player>	-	start ignoring player if not already
 *				ignored otherwise stop ignoring player
 *	ignore reply	-	start ignoring last player to send a
 *				tell to ch, to deal with invis spammers
 * Last Modified: June 26, 1997
 * - Fireblade
 */
void do_ignore(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	IGNORE_DATA *temp, *next;
	char fname[1024];
	struct stat fst;
	CHAR_DATA *victim;
	
	if(IS_NPC(ch))
		return;
	
	argument = one_argument(argument, arg);
	
	sprintf(fname, "%s%c/%s", PLAYER_DIR,
		tolower(arg[0]), capitalize(arg));
	
	victim = NULL;
	
	/* If no arguements, then list players currently ignored */
	if(arg[0] == '\0')
	{
		ch_printf(ch, "\n\r&B=====================================\n\r");
		ch_printf(ch, "&CYou are currently ignoring:\n\r");
		ch_printf(ch, "&B=====================================\n\r");
		set_char_color(AT_IGNORE, ch);
		
		if(!ch->pcdata->first_ignored)
		{
			ch_printf(ch, "\t    &GNo One\n\r");
			return;
		}
		
		for(temp = ch->pcdata->first_ignored; temp;
				temp = temp->next)
		{
			ch_printf(ch,"\t  &O- &Y%s\n\r",temp->name);
		}
		
		return;
	}
	/* Clear players ignored if given arg "none" */
	else if(!strcmp(arg, "none"))
	{
		for(temp = ch->pcdata->first_ignored; temp; temp = next)
		{
			next = temp->next;
			UNLINK(temp, ch->pcdata->first_ignored,
					ch->pcdata->last_ignored,
					next, prev);
			STRFREE(temp->name);
			DISPOSE(temp);
		}
		
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch, "You now ignore no one.\n\r");
		
		return;
	}
	/* Prevent someone from ignoring themself... */
	else if(!strcmp(arg, "self") || nifty_is_name(arg, ch->name))
	{
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch, "Did you type something?\n\r");
		return;
	}
	else
	{
		int i;
		
		/* get the name of the char who last sent tell to ch */		
		if(!strcmp(arg, "reply"))
		{
			if(!ch->reply)
			{
				set_char_color(AT_IGNORE, ch);
				ch_printf(ch, "They're not here.\n\r");
				return;
			}
			else
			{
				strcpy(arg, ch->reply->name);
			}
		}
		
		/* Loop through the linked list of ignored players */
		/* 	keep track of how many are being ignored     */
		for(temp = ch->pcdata->first_ignored, i = 0; temp;
				temp = temp->next, i++)
		{
			/* If the argument matches a name in list remove it */
			if(!strcmp(temp->name, capitalize(arg)))
			{
				UNLINK(temp, ch->pcdata->first_ignored,
					ch->pcdata->last_ignored,
					next, prev);
				set_char_color(AT_IGNORE, ch);
				ch_printf(ch,"You no longer ignore %s.\n\r",
					temp->name);
				STRFREE(temp->name);
				DISPOSE(temp);
				return;
			}
		}
		
		/* if there wasn't a match check to see if the name   */
		/* is valid. This if-statement may seem like overkill */
		/* but it is intended to prevent people from doing the*/
		/* spam and log thing while still allowing ya to      */
		/* ignore new chars without pfiles yet...             */
		if( stat(fname, &fst) == -1 &&
			(!(victim = get_char_world(ch, arg)) ||
			IS_NPC(victim) ||
			strcmp(capitalize(arg),victim->name) != 0))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"No player exists by that"
				" name.\n\r");
			return;
		}
		
		if(victim)
		{
			strcpy(capitalize(arg),victim->name);
		}

		    if (!check_parse_name( capitalize(arg), TRUE ))
		   {
		       set_char_color( AT_IGNORE, ch );
		       send_to_char( "No player exists by that name.\n\r", ch );
		       return;
		   }

		/* If its valid and the list size limit has not been */
		/* reached create a node and at it to the list	     */
		if(i < MAX_IGN)
		{
			IGNORE_DATA *new;
			CREATE(new, IGNORE_DATA, 1);
			new->name = STRALLOC(capitalize(arg));
			new->next = NULL;
			new->prev = NULL;
			LINK(new, ch->pcdata->first_ignored,
				ch->pcdata->last_ignored, next, prev);
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"You now ignore %s.\n\r", new->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"You may only ignore %d players.\n\r",
				MAX_IGN);
			return;
		}
	}
}

/*
 * This function simply checks to see if ch is ignoring ign_ch.
 * Last Modified: October 10, 1997
 * - Fireblade
 */
bool is_ignoring(CHAR_DATA *ch, CHAR_DATA *ign_ch)
{
	IGNORE_DATA *temp;
	
	if(IS_NPC(ch) || IS_NPC(ign_ch))
		return FALSE;
	
	for(temp = ch->pcdata->first_ignored; temp; temp = temp->next)
	{
		if(nifty_is_name(temp->name, ign_ch->name))
			return TRUE;
	}
	
	return FALSE;
}

/* Version info -- Scryn */

void do_version(CHAR_DATA* ch, char* argument)
{
	if(IS_NPC(ch))
	  return;

	set_char_color(AT_YELLOW, ch);
	ch_printf(ch, "SMAUG %s.%s\n\r", SMAUG_VERSION_MAJOR, SMAUG_VERSION_MINOR);

	if(IS_IMMORTAL(ch))
	  ch_printf(ch, "Compiled on %s at %s.\n\r", __DATE__, __TIME__);

	return;
}

/*Old scan*/

void do_scan( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * eyes;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf2[MSL];
    if ( ( eyes = get_eq_char( ch, WEAR_EYES ) ) == NULL )
    {
        send_to_char( "You don't have anything in your eyes.\n\r", ch );
        return;
    }
    if ( eyes->item_type != ITEM_SCOUTER )
    {
        send_to_char( "You can only scan a person with a scouter..\n\r", ch );
        return;
    }
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Scan who?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    switch ( eyes->value[3] )
    {
       case 1:
          sprintf(buf2, "&B");
	  break;
       case 2:
	  sprintf( buf2, "&R");
          break;
       case 3:
	  sprintf( buf2, "&G");
	  break;
       case 4:
	  sprintf( buf2, "&P");
	  break;
       case 5:
	  sprintf( buf2, "&R");
          break;
       default:
	  sprintf( buf2, "&p");
          break;
    }

   if (eyes->value[3] == 5)
   {
        ch_printf(ch,"\n\r");
        ch_printf(ch,"&c\\     &R___________________&R&W\n\r");
        ch_printf(ch,"&c |  &R /                   &R&R\\&W\n\r");
        ch_printf(ch,"&c |--&R| &cF O X H O U N D_    &R|&W\n\r");
        ch_printf(ch,"&c || &R|  &R-&WLoading...        &R|&W\n\r");
        ch_printf(ch,"&c || &R|  &R-&WScaning... &R O     &R|&W\n\r");
        ch_printf(ch,"&c || &R|             &R /|\\    &R|&W\n\r");
        ch_printf(ch,"&c |--&R| &WPowerlevel: &R  ^     &R|&W\n\r");
        ch_printf(ch,"&c |  &R| &R%-13s &w&R      |&R\n\r", num_punct(get_curr_pl2(victim)));
        ch_printf(ch,"&c/    &R\\___________________&R/&W\n\r");
   } 
   else
   {
        ch_printf(ch,"\n\r");
        ch_printf(ch,"&w\\     %s______________{x&W\n\r", buf2);
        ch_printf(ch,"&w |  %s /&YScanning...   %s\\{x&W\n\r", buf2, buf2);
        ch_printf(ch,"&w |--%s| &YProccessing... %s|{x&W\n\r", buf2, buf2);
        ch_printf(ch,"&w |  %s|                |{x&W\n\r", buf2);
        ch_printf(ch,"&w |--%s|                |{x&W\n\r", buf2);
        ch_printf(ch,"&w |  %s|                |{x&W\n\r", buf2);
        ch_printf(ch,"&w |--%s| &YPowerlevel&O:    %s|{x&W\n\r", buf2, buf2);
        ch_printf(ch,"&w |  %s| &r%-13s &w%s |{x&W\n\r", buf2, num_punct(get_curr_pl2(victim)), buf2);
        ch_printf(ch,"&w/    %s\\______________/{x&W\n\r", buf2);
   }
       if (!strcmp(arg, "self") || nifty_is_name(arg, ch->name))
       {
        send_to_char("\n\r", ch);
        return;
       }
       else
       {
       sprintf( buf , "%s presses a button on %s scouter and looks at you." , ch->name,
       ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its" );
       act( AT_GREY, buf , victim, NULL, ch, TO_CHAR );
       sprintf( buf, "%s presses a button on %s scouter and looks at %s." , ch->name,
       ch->sex == 1 ? "his" :
       ch->sex == 2 ? "her" : "its", victim->name );
       act( AT_GREY, buf, ch, NULL, victim, TO_NOTVICT );
       }
  return;
}

/* SMAUG Bank Support
 * Coded by Minas Ravenblood for The Apocalypse Theatre
 * (email: krisco7@hotmail.com)
 */
void do_bank( CHAR_DATA *ch, char *argument )
{
  char arg1[MIL];
  char buf [MSL];
  int amount;
  
  if ( !IN_BANK(ch))
  {
   send_to_char("You aren't inside a bank.", ch);
   return;
  }
  if ( IS_NPC( ch ) )
  return;
  if ( argument[0] == '\0' )
  {
    send_to_char("\n\rSyntax: Bank <deposit|balance|withdraw|transfer>\n\r", ch );
    return;
  }
  
  argument = one_argument( argument, arg1 );
  
  if ( !str_cmp( arg1, "balance" ) )
  {
    long long int total = ch->balance + ch->gold;
    send_to_char( "\n\r", ch);
    sprintf( buf, "You are carrying %s gold coin(s).\n\r", num_punct(ch->gold) );
    send_to_char( buf, ch );
    sprintf( buf, "You also have %s gold coin(s) in the bank.\n\r", num_punct(ch->balance));
    send_to_char( buf, ch );
    sprintf( buf, "Making a total of %lld gold coin(s).\n\r", total );
    send_to_char( buf, ch );
    return;
  }
             
  if ( !str_cmp( arg1, "deposit" ) )
  {
    char arg2[MIL];
    
    argument = one_argument( argument, arg2 );
    
    if ( arg2 == '\0' )
    {
      send_to_char( "\n\r", ch);
      sprintf( buf, "How much gold do you wish to deposit?\n\r" );
      send_to_char( buf, ch );
      return;
    }
    
    if ( str_cmp( arg2, "all" ) && !is_number( arg2 ) )
    {
      send_to_char( "", ch);
      send_to_char( "How much gold do you wish to deposit?\n\r", ch );
      return;
    }
    
    if ( !str_cmp( arg2, "all" ) )
      amount = ch->gold;
    else
      amount = atoi( arg2 );
    
    if ( amount > ch->gold )
    {
      send_to_char( "\n\rSorry, but you don't have that much gold to deposit.\n\r", ch );
      return;
    }
    
    if ( amount <= 0 )
    {
      send_to_char( "The bank likes positive numbers", ch);
      return;
    }
    if ( ch->balance + amount > 1500000000 )
    {
      send_to_char( "&r[&RBANK&r]&z: &WBANK ERROR, \"Sorry, but we do not accept accounts above 1.5 billion gold coins.\"\n\r", ch);
      return;
    }

    ch->gold		-= amount;
    ch->balance	+= amount;
    sprintf( buf, "\n\rYou deposit %d gold coin%s.\n\r", amount, (amount != 1) ? "s" : "" );
    set_char_color( AT_PLAIN, ch );
    send_to_char( buf, ch );
    sprintf( buf, "$n deposits %d gold coin%s.\n\r", amount, (amount != 1) ? "s" : "" );
    act( AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM );
    return;
  }
  
  if ( !str_cmp( arg1, "withdraw" ) )
  {
    char arg2[MIL];
    
    argument = one_argument( argument, arg2 );
    
    if ( arg2 == '\0' )
    {
      send_to_char( "\n\rHow much gold do you wish to withdraw?\n\r", ch );
      return;
    }
    if ( str_cmp( arg2, "all" ) && !is_number( arg2 ) )
    { 
      send_to_char( "How much gold do you wish to withdraw?\n\r", ch );
      return;
    }
    
    if ( !str_cmp( arg2, "all" ) )
      amount = ch->balance;    
    else
      amount = atoi( arg2 );
    
    if ( amount > ch->balance )
    {
      send_to_char( "But you do not have that much gold in your account!\n\r", ch);
      return;
    }
    
    if ( amount <= 0 )
    {
      send_to_char( "The bank likes positive numbers.\n\r", ch);
      return;
    }
    if ( ch->gold + amount > 1500000000 )
    {
      send_to_char( "&r[&RBANK&r]&z: &WBANK ERROR, \"Sorry, but for security reasons, you are not able to carry more than 1.5 billion gold coins.\"\n\r", ch);
      return;
    }
    
    ch->balance	-= amount;
    ch->gold		+= amount;
    sprintf( buf, "\n\rYou withdraw %d gold coin%s.\n\r", amount, (amount != 1) ? "s" : "" );
    set_char_color( AT_PLAIN, ch );
    send_to_char( buf, ch );
    sprintf( buf, "$n withdraws %d gold coin%s.\n\r", amount, (amount != 1) ? "s" : "" );
    act( AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM );
    return;
  }
  
  if ( !str_cmp( arg1, "transfer" ) )
  {
    CHAR_DATA *victim;
    char arg2[MIL];
    char arg3[MIL];
    
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    
    if ( arg2 == '\0' || arg3 == '\0' )
    {
      send_to_char( "\n\rHow much gold do you wish to send to who?\n\r", ch);
      return;
    }
    if ( str_cmp( arg2, "all" ) && !is_number( arg2 ) )
    {
      send_to_char( "\n\rHow much gold do you wish to send to who?\n\r", ch);    
      return;
    }
    
    if ( !( victim = get_char_world( ch, arg3 ) ) )
    {
      sprintf( buf, "\n\r%s isn't in this Universe.\n\r", capitalize(ch->name)  );
      return;
    }
    
    if ( IS_NPC( victim ) )
    return;

    
    if ( !str_cmp( arg2, "all" ) )
      amount = ch->balance;
    else
      amount = atoi( arg2 );
    
    if ( amount > ch->balance )
    {
      send_to_char( "\n\rYou are very generous, but you don't have that much gold!\n\r", ch);
      return;
    }
    
    if ( amount <= 0 )
    {
      send_to_char( "\n\rThe bank likes positive numbers.\n\r", ch );
      return;
    }
    if ( victim->balance + amount > 1500000000 )
    {
      send_to_char( "&r[&RBANK&r]&z: &WBANK ERROR, \"Sorry, but we do not accept accounts above 1.5 billion gold coins.\"\n\r", ch);
      return;
    }
    
    ch->balance     -= amount;
    victim->balance += amount;
    sprintf( buf, "\n\rYou transfer %d gold coin%s to %s's bank account.\n\r",
             amount, (amount != 1) ? "s" : "", victim->name );
    set_char_color( AT_GREEN, ch );
    send_to_char( buf, ch );
    sprintf( buf, "\n\r%s just transferred %d gold coin%s to your bank account.\n\r",
             ch->name, amount, (amount != 1) ? "s" : "" );
    set_char_color( AT_GREEN, victim );
    send_to_char( buf, victim );
    return;
  }
  return;
}
