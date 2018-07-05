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
 *			      Regular update module			    *
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
#include <string.h>
#include <time.h>
#include <math.h>
#include "mud.h"


/* From newarena.c -- Tawnos */
extern void start_arena();
extern void do_game();
extern int in_start_arena;
extern int ppl_in_arena;
extern int ppl_challenged;
extern int num_in_arena();

/* From newarena.c -- Tawnos */
extern void start_koth();
extern void do_koth_game();
extern int in_start_koth;
extern int ppl_in_koth;
extern int num_in_koth();
int moonphase;
/*
 * Local functions.
 */
int     get_sensu_carried 	args( ( CHAR_DATA *ch ) );
OBJ_DATA *get_obj_ship args( (int vnum) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	update_shuttles args( ( void ) );
void	weather_update	args( ( void ) );
void	time_update	args( ( void ) );	/* FB */
void	char_update	args( ( void ) );
void    space_update    args( ( void ) );
void    db_update       args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void	room_act_update	args( ( void ) );
void	obj_act_update	args( ( void ) );
void	char_check	args( ( void ) );
void    drunk_randoms	args( ( CHAR_DATA *ch ) );
void	subtract_times	args( ( struct timeval *etime,
				struct timeval *stime ) );
void reset_sector ( void );

/* weather functions - FB */
void	adjust_vectors		args( ( WEATHER_DATA *weather) );
void	get_weather_echo	args( ( WEATHER_DATA *weather) );
void	get_time_echo		args( ( WEATHER_DATA *weather) );
bool    is_kistunned		args( ( CHAR_DATA *ch ) );
/* Overland Map movement - Samson 7-31-99 */
bool map_wander( CHAR_DATA *ch, short map, short x, short y, short sector, short newsector );
int ki_consumed( CHAR_DATA *ch );
/*
 * Global Variables
 */

CHAR_DATA *	gch_prev;
CHAR_DATA *     gch_next;
OBJ_DATA *	gobj_prev;

CHAR_DATA *	timechar;




void moonchan(char *argument)
{
  char buf1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *i;

  sprintf(buf1, "&w%s\r\n", argument);

  for (i = first_descriptor; i; i = i->next)
  {
    if (!i->connected && i->character)
    {
      send_to_char(buf1, i->character);
    }
  }
}

char * corpse_descs[] =
   { 
     "&zThe corpse of %s is in the last stages of decay.&w", 
     "&zThe corpse of %s is crawling with vermin.&w",
     "&zThe corpse of %s fills the air with a foul stench.&w",
     "&zThe corpse of %s is buzzing with flies.&w",
     "&zThe corpse of %s lies here.&w"
   };

extern int      top_exit;

/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int add_mana;
    int add_prac;

/*	save_char_obj( ch );*/
   /* sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
    set_title( ch, buf );*/

    add_mana	= class_table[ch->class]->fMana
		    ? number_range(2, (2*get_curr_int(ch)+get_curr_wis(ch))/8)
		    : 0;
    add_prac	= wis_app[get_curr_wis(ch)].practice;
    add_mana	= UMAX(  0, add_mana );


    /* bonus for deadlies */
    if(IS_PKILL(ch)){
        add_mana = add_mana + add_mana*.3;
        sprintf(buf,"Gravoc's Pandect steels your sinews.\n\r");
    }

    ch->max_mana	+= add_mana;

    if ( !IS_NPC(ch) )
	xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    if ( ch->level == LEVEL_AVATAR )
    {
	DESCRIPTOR_DATA *d;

	sprintf( buf, "%s has become a god! ", ch->name );
	for ( d = first_descriptor; d; d = d->next )
	   if ( d->connected == CON_PLAYING && d->character != ch )
	   {
		set_char_color( AT_IMMORT, d->character );
		send_to_char( buf,	d->character );
		send_to_char( "\n\r",	d->character );
	   }
	set_char_color( AT_WHITE, ch );
	do_help( ch, "M_ADVHERO_" );
    }
    if ( ch->level < LEVEL_IMMORTAL )
    {
      set_char_color( AT_WHITE, ch );
      send_to_char( buf, ch );
      if ( !IS_NPC( ch ) )
      {
	sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
	  ch->name,
	  ch->level,
	  get_age( ch ),
	  ch->in_room == NULL ? 0 : ch->in_room->vnum,
	  capitalize(race_table[ch->race]->race_name),
	  class_table[ch->class]->who_name );
	append_to_file( PLEVEL_FILE, buf2 );
      }
    }
    return;
}   



void gain_exp( CHAR_DATA *ch, int gain )
{
    int modgain;
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) || ch->level >= LEVEL_AVATAR )
	return;


    /* Bonus for deadly lowbies */
    modgain = gain;
    if(modgain>0 && IS_PKILL(ch) && ch->level<17){
       if(ch->level<=6){
          sprintf(buf,"The Favor of Gravoc fosters your learning.\n\r");
          modgain*=2;
       }
       if(ch->level<=10 && ch->level>=7){
          sprintf(buf,"The Hand of Gravoc hastens your learning.\n\r");
          modgain*=1.75;
       }
       if(ch->level<=13 && ch->level>=11){
          sprintf(buf,"The Cunning of Gravoc succors your learning.\n\r");
          modgain*=1.5;
       }
       if(ch->level<=16 && ch->level>=14){
          sprintf(buf,"The Patronage of Gravoc reinforces your learning.\n\r");
          modgain*=1.25;
       }
	send_to_char(buf, ch);
    }

    /* per-race experience multipliers */
    modgain *= ( race_table[ch->race]->exp_multiplier/100.0);

    /* Deadly exp loss floor is exp floor of level */
    if(IS_PKILL(ch)&& modgain<0){
       if( ch->exp + modgain < exp_level(ch, ch->level)){
          modgain = exp_level(ch, ch->level) - ch->exp;
//          (buf,"Gravoc's Pandect protects your insight.\n\r");
       }
    }


    /* xp cap to prevent any one event from giving enuf xp to */
    /* gain more than one level - FB */
    modgain = UMIN(modgain,
    	exp_level(ch, ch->level+2) - exp_level(ch, ch->level+1));

    ch->exp = UMAX( 0, ch->exp + modgain );

    if (NOT_AUTHED(ch) && ch->exp <= 15000)
    {
	send_to_char("You can not grow stronger until you are authorized.\n\r", ch);
	return;
    }

    while ( ch->level < 1 && ch->exp >= exp_level(ch, ch->level))
    {
	set_char_color( AT_WHITE + AT_BLINK, ch );
	ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->level );
	advance_level( ch );
    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;

	gain = UMIN( 5, 1 );
        if ( ch->position == POS_DEAD)
        gain = 0;
        else if ( ch->position == POS_STUNNED)
        gain = 1;
        else if ( ch->position == POS_STANDING || ch->position == POS_SITTING || ch->position == POS_FIGHTING)
        {
        gain = 1;
        if ( get_curr_con(ch) >= 10 && get_curr_con(ch) <= 6 )
        gain += 2;
        if ( get_curr_con(ch) >= 15 && get_curr_con(ch) <= 10 )
        gain += 4;
        if ( get_curr_con(ch) >= 20 && get_curr_con(ch) <= 15 )
        gain += 6;
        }
        else if (ch->position == POS_SLEEPING)
        {
        gain = 10;
        if ( get_curr_con(ch) >= 10 && get_curr_con(ch) <= 8 )
        gain += 2;
        if ( get_curr_con(ch) >= 15 && get_curr_con(ch) <= 12 )
        gain += 4;
        if ( get_curr_con(ch) >= 20 && get_curr_con(ch) <= 17 )
        gain += 8;
        }
        else if (ch->position == POS_RESTING)
        {
        gain = 10;
        if ( get_curr_con(ch) >= 10 && get_curr_con(ch) <= 7 )
        gain += 2;
        if ( get_curr_con(ch) >= 15 && get_curr_con(ch) <= 11 )
        gain += 4;
        if ( get_curr_con(ch) >= 20 && get_curr_con(ch) <= 16 )
        gain += 8;
        }
       if ((can_use_skill2(ch, number_percent(), gsn_regen )) && !IN_ARENA(ch) && !IS_NPC(ch))
       {
        gain += URANGE( 5, ch->pcdata->skill_level2[gsn_regen]*2, 30);
        learn_from_success2(ch, gsn_regen);
       }
       else
       {
        gain += 10;
        learn_from_failure2(ch, gsn_regen);
       }
       if (ch->hit <= 0)
       gain = 1;
       if (IS_NPC(ch))
       gain = number_range(5, 15);
    return UMIN(gain, ch->max_hit - ch->hit);
}


int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int gainper;
    int kiper;
    gainper = get_curr_con(ch)/2;
    kiper = (ch->mana * 100) / ch->max_mana;

    if (ch->position == POS_SLEEPING)
    {
     if (IN_HEAL(ch)){gainper *= 5;}
     else {gainper *= 2.5;}
    }
    else if (ch->position == POS_RESTING)
    {
     if (IN_HEAL(ch)){ gainper *= 4; }
     else { gainper *= 2;}
    }
    gainper += kiper + 5;
    if ( ki_consumed(ch) > 0)
    gainper -= number_range( 2, 5 );
    gain = (gainper * ch->max_mana) / 100;

    if (gain >= ch->max_mana)
    gain = ch->max_mana;

    return UMIN(gain, 100000);
}

void ki_learn_from_success( CHAR_DATA *ch, int gsn )
{
   sh_int value = number_range( -3, 1 );
   sh_int adept = 50 + (get_curr_wis(ch)*2);

   ch->pcdata->learned[gsn] += URANGE( 0, value, 100 );
   ch->pcdata->learned[gsn] = URANGE( 0, ch->pcdata->learned[gsn], adept );
   return;
}

void ki_learn_from_success2( CHAR_DATA *ch, int gsn )
{
   sh_int value = number_range( -3, 1 );
   sh_int adept = 50 + (get_curr_wis(ch)*2);

   ch->pcdata->learned2[gsn] += URANGE( 0, value, 100 );
   ch->pcdata->learned2[gsn] = URANGE( 0, ch->pcdata->learned2[gsn], adept );
   return;
}

int ki_consumed( CHAR_DATA *ch )
{
  int energy = sqrt(ch->max_mana) / 3;
  int value = 0;
/* Abilities */
  if (xIS_SET(ch->skill, SKILL_ENRAGE) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_enrage );
  }
  if (xIS_SET(ch->skill, SKILL_BERSERK) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_berserk );
  }
  if (xIS_SET(ch->skill, SKILL_OVERDRIVE) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_overdrive );
  }
  if (xIS_SET(ch->skill, SKILL_KYODAIKA) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_kyodaika );
  }
  if (xIS_SET(ch->skill, SKILL_SOULCRY) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_soulcry );
  }
  if (xIS_SET(ch->skill, SKILL_TIMEFREEZE) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_timefreeze );
  }
  if (xIS_SET(ch->skill, SKILL_BBARRIER) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_bbarrier );
  }
  if (xIS_SET(ch->skill, SKILL_PBARRIER) )
  {
      value += energy;
      ki_learn_from_success2( ch, gsn_pbarrier );
  }
  if (xIS_SET(ch->skill, SKILL_REGENERATION) )
  {
      value += energy*4;
      ch->hit += number_range( 1, 3 );
      ch->hit = URANGE( 0, ch->hit, ch->max_hit );
      ki_learn_from_success2( ch, gsn_regen );
  }
/* Skills */
  if (xIS_SET(ch->skill, SKILL_VSTANCE) )
  {
      value += energy;
      ki_learn_from_success( ch, gsn_vstance );
  }
  if (xIS_SET(ch->skill, SKILL_ZANZOKEN) )
  {
      value += energy;
      ki_learn_from_success( ch, gsn_zanzoken );
  }
  if (xIS_SET(ch->skill, SKILL_ACCURACY) )
  {
      value += energy;
      ki_learn_from_success( ch, gsn_accuracy );
  }
  if ( ch->mana - value <= 0 )
  {
      xCLEAR_BITS(ch->skill);
      send_to_char( "Some skills were removed due to the lack of energy.\n\r", ch);
  }
   return value;
}




/*
 * Put this in a seperate function so it isn't called three times per tick
 * This was added after a suggestion from Cronel	--Shaddai
 */

void check_alignment( CHAR_DATA *ch )
{
     /*
      *  Race alignment restrictions, h
      */
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int door;
    ch_ret     retcode;

    retcode = rNONE;

    /* Examine all mobs. */
    for ( ch = last_char; ch; ch = gch_prev )
    {
	set_cur_char( ch );
	if ( ch == first_char && ch->prev )
	{
	    bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	  
	gch_prev = ch->prev;
	
	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    gch_prev = NULL;
	    ch->prev = NULL;
	    do_shout( ch, "Thoric says, 'Prepare for the worst!'" );
	}

	if ( !IS_NPC(ch) )
	    continue;

	if ( !ch->in_room)
	    continue;
/* Clean up 'animated corpses' that are not charmed' - Scryn */


	if ( !xIS_SET( ch->act, ACT_RUNNING )
	&&   !xIS_SET( ch->act, ACT_SENTINEL )
	&&   !ch->fighting && ch->hunting )
	{
	  WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	  /* Commented out temporarily to avoid spam - Scryn 
	  sprintf( buf, "%s hunting %s from %s.", ch->name,
	  	ch->hunting->name,
		ch->in_room->name );
	  log_string( buf ); */
	  hunt_victim( ch );
	  continue;
	}  

	/* Examine call for special procedure */
	if ( !xIS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_fun )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}

	/* Check for mudprogram script on mob */
	if ( HAS_PROG( ch->pIndexData, SCRIPT_PROG ) )
	{
	    mprog_script_trigger( ch );
	    continue;
	}

	if ( ch != cur_char )
	{
	    bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
	    continue;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE )
	&&  (xIS_SET(ch->act, ACT_AGGRESSIVE) || xIS_SET(ch->act, ACT_META_AGGR)) )
	    do_emote( ch, "glares around and snarls." );


	/* MOBprogram random trigger */
	if ( ch->in_room->area->nplayer > 0 )
	{
	    mprog_random_trigger( ch );
	    if ( char_died(ch) )
		continue;
	    if ( ch->position < POS_STANDING )
	        continue;
	}

        /* MOBprogram hour trigger: do something for an hour */
        mprog_hour_trigger(ch);

	if ( char_died(ch) )
	  continue;

	rprog_hour_trigger(ch);
	if ( char_died(ch) )
	  continue;

	if ( ch->position < POS_STANDING )
	  continue;

	/* Scavenge */
	if ( xIS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->first_content
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = NULL;
	    for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max 
		&& !IS_OBJ_STAT( obj, ITEM_BURIED ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

      /* Map wanderers - Samson 7-29-00 */
        if( IS_NPC(ch) && xIS_SET( ch->act, ACT_ONMAP ) )
	{
           short per = number_percent();
	   short sector = get_terrain( ch->map, ch->x, ch->y );
           short newsector = 0;
	   short map = ch->map;
	   short x = ch->x;
	   short y = ch->y;
           short dir = number_range( 0, 4);

           
 	   if( per > 40 && dir < DIR_SOMEWHERE && dir != DIR_UP && dir != DIR_DOWN )
	   {
		switch( dir )
		{
                   default:
		   break;
		   case DIR_NORTH:
                        newsector = get_terrain( map, x, y-1);
			if( map_wander( ch, map, x, y-1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_NORTH );
		   break;
		   case DIR_NORTHEAST:
                        newsector = get_terrain( map, x+1, y-1);
			if( map_wander( ch, map, x+1, y-1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_NORTHEAST );
		   break;
		   case DIR_EAST:
                        newsector = get_terrain( map, x+1, y);
			if( map_wander( ch, map, x+1, y, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_EAST );
		   break;
		   case DIR_SOUTHEAST:
                        newsector = get_terrain( map, x+1, y+1);
			if( map_wander( ch, map, x+1, y+1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_SOUTHEAST );
		   break;
		   case DIR_SOUTH:
                        newsector = get_terrain( map, x, y+1);
			if( map_wander( ch, map, x, y+1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_SOUTH );
		   break;
		   case DIR_SOUTHWEST:
                        newsector = get_terrain( map, x-1, y+1);
			if( map_wander( ch, map, x-1, y+1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_SOUTHWEST );
		   break;
		   case DIR_WEST:
                        newsector = get_terrain( map, x-1, y);
			if( map_wander( ch, map, x-1, y, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_WEST );
		   break;
		   case DIR_NORTHWEST:
                        newsector = get_terrain( map, x-1, y-1);
			if( map_wander( ch, map, x-1, y-1, sector, newsector ) )
			   move_char( ch, NULL, 0, DIR_NORTHWEST );
		   break;
		}
	   }
	   if( char_died(ch) )
		continue;
         continue;
	}


	/* Wander */
	if ( !xIS_SET(ch->act, ACT_RUNNING)
	&&   !xIS_SET(ch->act, ACT_SENTINEL)
	&&   !xIS_SET(ch->act, ACT_PROTOTYPE)
	&& ( door = number_bits( 5 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room, door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_DEATH)
	&& ( !xIS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    retcode = move_char( ch, pexit, 0, door );
						/* If ch changes position due
						to it's or someother mob's
						movement via MOBProgs,
						continue - Kahn */
	    if ( char_died(ch) )
	      continue;
	    if ( retcode != rNONE || xIS_SET(ch->act, ACT_SENTINEL)
	    ||    ch->position < POS_STANDING )
	        continue;
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 4 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room,door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = ch->in_room->first_person;
		  rch;
		  rch  = rch->next_in_room )
	    {
		if ( is_fearing(ch, rch) )
		{
		    switch( number_bits(2) )
		    {
			case 0:
			  sprintf( buf, "Get away from me, %s!", rch->name );
			  break;
			case 1:
			  sprintf( buf, "Leave me be, %s!", rch->name );
			  break;
			case 2:
			  sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
			  break;
			case 3:
			  sprintf( buf, "Someone save me from %s!", rch->name );
			  break;
		    }
		    do_yell( ch, buf );
		    found = TRUE;
		    break;
		}
	    }
	    if ( found )
		retcode = move_char( ch, pexit, 0, pexit->vdir );
	}
    }

    return;
}

void check_boost( void )
{
    CHAR_DATA *i;
    DESCRIPTOR_DATA *d;
    int count=0;

    search_boost();
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
       i = d->original ? d->original : d->character;
       if (xIS_SET(i->powerup, PLR_BOOST))
       count++;
     }
    }
    if ( count != 1 )
    {
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
         i = d->original ? d->original : d->character;
         xREMOVE_BIT(i->powerup, PLR_BOOST);
      }
     }
    }
}
void search_boost( void )
{
    CHAR_DATA *i;
    DESCRIPTOR_DATA *d;
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

         i = d->original ? d->original : d->character;
         if ( i->exp < 125000000 && !xIS_SET(i->act, PLR_AFK) && i->level < 50)
         {
           xSET_BIT(i->powerup, PLR_BOOST);
           return;
         }
       }
     }

}

void breath_update( void )
{
    CHAR_DATA *i;
    DESCRIPTOR_DATA *d;
      for (d = first_descriptor; d; d = d->next)
      {
       if( d->connected == CON_PLAYING )
       {
         i = d->original ? d->original : d->character;
         if ( IS_NPC(i))
         continue;
         if ( !xIS_SET(i->act, PLR_ONMAP))
         continue;
         if ( get_mapbelow( i->map ) == FALSE )
         continue;
         if ( IS_ANDROID(i) )
         continue;
         if ( IS_ICER(i))
         continue;
         i->breath -= number_range( 2, 5 );
         i->breath = URANGE( 0, i->breath, 100 );
         if ( i->breath < 10 && i->breath > 0)
           send_to_char( "&RYou are almost running out of air.\n\r", i );
         if ( i->breath <= 0 )
         {
           send_to_char( "&RYou begin to feel dizzy and in need of air!\n\r", i);
           i->hit -= number_range( 0, 2 );
         }
       }
      }

}

int skill_timer;

void skill_update( void )
{
   CHAR_DATA *ch;
   DESCRIPTOR_DATA *d;


   skill_timer++;

   if ( skill_timer >= 3 )
   {
      for (d = first_descriptor; d; d = d->next)
      {
       if( d->connected == CON_PLAYING )
       {
         ch = d->original ? d->original : d->character;
         ch->mana -= ki_consumed(ch);
         if ( ch->mana <= 0 )
         {
           ch->mana = 1;
           xCLEAR_BITS(ch->skill);
         }
       }
      }
      skill_timer = 0;
   }
      return;
}
/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_save;
    sh_int result;
    OBJ_DATA *cloak;
    int kicheck, kigain, statcheck;
    ch_save     = NULL;
    for ( ch = last_char; ch; ch = gch_prev )
    {
	if ( ch == first_char && ch->prev )
	{
	    bug( "char_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	gch_prev = ch->prev;
	set_cur_char( ch );
	if ( gch_prev && gch_prev->next != ch )
	{
	    bug( "char_update: ch->prev->next != ch", 0 );
	    return;
	}
        if ( ch->position > POS_DRAG )
        ch->position = POS_STANDING;
        /*
	 *  Do a room_prog rand check right off the bat
	 *   if ch disappears (rprog might wax npc's), continue
	 */

        if ( ( cloak = get_eq_char( ch, WEAR_ABOUT ) ) != NULL )
        {
          if ( cloak->pIndexData->vnum == 8552 )
          add_timer( ch, TIMER_CLOAK, 5, NULL, 0 );
        }

           
	if(!IS_NPC(ch))
	    rprog_random_trigger( ch );

	if( char_died(ch) )
	    continue;

	if(IS_NPC(ch))
	    mprog_time_trigger(ch);   

	if( char_died(ch) )
	    continue;

	rprog_time_trigger(ch);

	if( char_died(ch) )
	    continue;

	/*
	 * See if player should be auto-saved.
	 */
	if ( !IS_NPC(ch)
	&& ( !ch->desc || ch->desc->connected == CON_PLAYING )
	&&    ch->level >= 2
	&&    current_time - ch->save_time > (sysdata.save_frequency*60) )
	    ch_save	= ch;
	else
	    ch_save	= NULL;
   ch->max_hit = 150;
   if (!IS_NPC(ch))
   {
    int totalgain=0;
    int skillcheck;
    check_boost();
    if (ch->drunk > 0)
    ch->drunk -=1;

    skillcheck = (ch->skillgain * ch->skillgain) * 10000;

    while ( skillcheck <= 100000000 && skillcheck <= ch->exp && ch->skillgain <= calculatelevel(ch))
    {
     ch->skill_lvl++;
     ch->skillgain++;
     skillcheck = (ch->skillgain * ch->skillgain) * 10000;
     totalgain++;
    }
     if (totalgain > 0)
             ch_printf(ch, "You have gained %d skill leveling session(s)\n\r", totalgain);

    if ( ch->exp > 105000000 && calculatelevel(ch) > calculatelevel2(ch) )
    {
      ch->skill_lvl += calculatelevel(ch) - calculatelevel2(ch);
      ch_printf(ch, "You have gained %d skill leveling session(s)\n\r", calculatelevel(ch) - calculatelevel2(ch));

    }
    totalgain = 0;
    kicheck = (ch->skigain * ch->skigain) * 1000;
     
    while ( kicheck <= 100489000 && kicheck <= ch->exp)
    {
     kigain = number_range( 40, 60);
     kigain += get_curr_con(ch) * 4;
     ch->max_mana += kigain;
     totalgain += kigain;
     ch->skigain += 1;
     kicheck = (ch->skigain * ch->skigain) * 1000;    
    }
    
     if (totalgain > 0)
     ch_printf(ch, "You feel stronger all of the sudden. You gain %d energy!\n\r", totalgain);

    totalgain = 0;
    statcheck = (ch->statgain * ch->statgain) * 17300;
    //while (statcheck <= ch->exp && ch->exp <= 100020000)

    while(statcheck <= ch->exp && ch->statgain < 81 )
    {
     ch->stat++;
     ch->statgain++;
     totalgain++;
     statcheck = (ch->statgain * ch->statgain) * 17300;
    }

    if(ch->statgain == 77)
    {
	ch->stat += 4;
	ch->statgain += 4;
	totalgain += 4;
    }

     if (totalgain > 0)
	     ch_printf(ch, "You have gained %d attribute training session(s)\n\r", totalgain);

   }
/*
        if ( !IS_NPC(ch) && ch->balance && (time_info.day == 5 || time_info.day == 15 || time_info.day == 25 || time_info.day == 10 || time_info.day == 20 || time_info.day == 30) && time_info.hour == 1)
        {
	    	  char buf[MAX_STRING_LENGTH];
		  amount = ch->balance * 0.015;
            if (ch->balance + amount < 1500000000)
            {
		  ch->balance += amount;
		  sprintf(buf, "&B[&YBank Info&W: %s&B] &WYou made&W: &Y%s&W interest this month.\n\r", ch->name, num_punct(amount));
		  send_to_char(buf, ch);
            }
        }
*/
      if ( ch->position <= POS_STUNNED )
        xCLEAR_BITS(ch->skill);

	if ( ch->position >= POS_STUNNED )
	{
            if ( IN_HEAL(ch) && ch->position == POS_SLEEPING)
            {
                ch->hit = URANGE( -10, ch->hit + (ch->max_hit / 14), ch->max_hit);
                ch_printf(ch, "You turn around in your sleep as your body recovers in the chamber.\n\r");
            }
            if ( ch->hit  < ch->max_hit )
                ch->hit  = URANGE( -10, ch->hit + (hit_gain(ch)), ch->max_hit) ;

            if (ch->hit >= ch->max_hit * 2)
                ch->hit = ch->max_hit;

	    if ( ch->mana < ch->max_mana )
		ch->mana = mana_gain(ch);
	}
        result = (sh_int) number_range( 0, 5 );
        if ( result == 0 )
        do_growtail(ch, "" );
	is_kistunned(ch);
	if ( ch->position == POS_STUNNED )
        update_pos( ch );



	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room )
		{
		    ch->in_room->light -= obj->count;
		    if (ch->in_room->light < 0 )
				ch->in_room->light=0;
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
		    if ( obj->serial == cur_obj )
		      global_objcode = rOBJ_EXPIRED;
		    extract_obj( obj );
		}
	    }


	    if ( ++ch->timer >= 12 ) 
	    {
		if ( !IS_IDLE( ch ) )
		{
	
		    ch->was_in_room = ch->in_room;
		    
		    if ( ch->fighting )
			stop_fighting( ch, TRUE );
		    act( AT_ACTION, "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    if ( IS_SET( sysdata.save_flags, SV_IDLE ) )
			save_char_obj( ch );
		    xSET_BIT(ch->pcdata->flags, PCFLAG_IDLE);
		     xREMOVE_BIT(ch->act, PLR_SPEC);
                    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }


      if ( !IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->pcdata->release_date > 0 &&
           ch->pcdata->release_date <= current_time )
      {
         ROOM_INDEX_DATA *location;
          location = get_room_index(ROOM_VNUM_TEMPLE);
         if ( !location )
          location = ch->in_room;
         MOBtrigger = FALSE;
         char_from_room(ch);
         char_to_room( ch, location );
         send_to_char( "The gods have released you from hell as your sentance is up!\n\r", ch );
         do_look(ch, "auto");
         STRFREE( ch->pcdata->helled_by );
	 ch->pcdata->helled_by = NULL;
         ch->pcdata->release_date = 0;
         save_char_obj( ch );
      }
      if ( !IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->pcdata->silenced_date > 0 &&
           ch->pcdata->silenced_date <= current_time )
      {
         xREMOVE_BIT(ch->act, PLR_SILENCE);
         send_to_char( "You can use channels again!\n\r", ch );
         STRFREE( ch->pcdata->silenced_by );
         ch->pcdata->silenced_by = NULL;
         ch->pcdata->silenced_date = 0;
         save_char_obj( ch );
      }


	if ( !char_died(ch) )
	{
	    /*
	     * Careful with the damages here,
	     *   MUST NOT refer to ch after damage taken, without checking
	     *   return code and/or char_died as it may be lethal damage.
	     */
	    if ( char_died(ch) )
		continue;

	    if ( ch->timer > 24 )
		do_quit( ch, "" );

	}
    }    
  }
    return;
}
OBJ_DATA *get_db( int db )
{
   OBJ_DATA *obj;

    for ( obj = first_object; obj; obj = obj->next )
      if ( obj->pIndexData->vnum == db )
         return obj;

  return NULL;
}
int income;
 
void income_update( void )
{
  AREA_DATA *pArea;
  CLAN_DATA *clan;
  income += 1;

  if ( income == 58 )
  {
     if ( find_fighters() >= 4 )
     do_chaos( supermob, "8");
  }

  if (income < 60 )
   return;
    for ( pArea = first_area; pArea; pArea = pArea->next )
    {
         if (!IS_SET(pArea->flags, AFLAG_PLANET))
         continue;
         if (!pArea->income)
         continue;
         if (!pArea->governed_by || !str_cmp(pArea->governed_by, "(null)"))
          pArea->governed_by = "None";
         if (pArea->governed_by && pArea->popsupport > 0)
         {
           clan = get_clan_short(pArea->governed_by);
           if (!clan)
           continue;
           clan->gold += pArea->income;
         }
         if (pArea->popsupport >= -75 && str_cmp( pArea->governed_by, "None" )) 
          pArea->popsupport -= number_range( 1,3);
         if (pArea->popsupport <= -75 && str_cmp( pArea->governed_by, "None" ) )
         {
          pArea->governed_by = 0;
          pArea->popsupport = 0;
         }
         if ( pArea->popsupport != 100 && !str_cmp( pArea->governed_by, "None" ) )
          pArea->popsupport += number_range( 5, 10 );

         if (pArea->popsupport > 100 )
         pArea->popsupport = 100;
         if (pArea->popsupport < -100 )
         pArea->popsupport = -100;

         do_foldarea( supermob, "allnomsg" );

       if (pArea == last_area)
       break;
    }
  if (income >= 60)
  income = 0;

 return;
}
int charsave;

void charsave_update( void )
{
  CHAR_DATA *pfs;
  charsave += 1;

   if ( charsave >= 10 )
   {
    for ( pfs = first_char; pfs; pfs = pfs->next )
    {
       if ( !IS_NPC( pfs ) )
       {
          save_char_obj( pfs );
          save_home( pfs );
          saving_char = NULL;
       }
     }
     charsave = 0;
   }
}

void moon_update(void)
{
   char buf[MSL];

   if (time_info.hour == 20)
   {

       moonphase += 1;
       switch ( moonphase )
       {
          case 1:
            sprintf(buf, "&WThe sun vanishes from sight, bringing the world into the darkness of a moonless night.");
            break;
          case 2:
          case 8:
            sprintf(buf, "&WAs the sun dissappears from the horizon, you can see a small portion of the moon.");
            break;
          case 3:
          case 7:
            sprintf(buf, "&WAs the sun sets, you look up in the sky noticing the half-moon");
            break;
          case 4:
          case 6:
            sprintf(buf, "&WThe moon is almost full tonight.");
            break;
          case 5:
            sprintf(buf, "&WThe moon glows brightly in the sky as night falls.");
            break;
       }             
       if ( moonphase == 8 )
       moonphase = 0; 
       moonchan(buf);
  }
  return;
}

int find_fighters()
{
   int count=0;
   CHAR_DATA *i;
   DESCRIPTOR_DATA *d;
      for (d = first_descriptor; d; d = d->next)
      {
       if( d->connected == CON_PLAYING )
       {
         i = d->original ? d->original : d->character;
         if ( i->exp >= 75000000 && !xIS_SET(i->act, PLR_AFK) && !IS_IMMORTAL(i))
         count++;
       }
      }
  return count;
}
void db_update(void)
{
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex=NULL;
  int level=1;
  int x, y, sector, vnum, db, created, created2, created3;

  created = created2 = created3 = 0;
  if ( dbtimer < 60 )
  {
    for ( db = 260; db <= 266; db++ )
    {
      if ( ( obj = get_db( db ) ) == NULL )
      continue;
      extract_obj( obj );
    }
  }
  if ( dbtimer2 < 125 )
  {
    for ( db = 270; db <= 276; db++ )
    {
      if ( ( obj = get_db( db ) ) == NULL )
      continue;
      extract_obj( obj );
    }
  }
  if ( dbtimer3 < 80 )
  {
    for ( db = 253; db <= 259; db++ )
    {
      if ( ( obj = get_db( db ) ) == NULL )
      continue;
      extract_obj( obj );
    }
  }
  for ( db = 253; db <= 276; db++ )
  {
    if ( ( obj = get_db( db ) ) == NULL )
    continue;
    if ( !IS_OBJ_FLAG( obj, ITEM_ONMAP) )
    continue;
    if ( map_sector[obj->map][obj->x][obj->y] == SECT_INSIDE )
    extract_obj( obj );
    else
    continue;
  }

  dbtimer += 1;
  dbtimer2 += 1;
  dbtimer3 += 1;
   if ( dbtimer > 1000 )
   dbtimer = 0;
   if ( dbtimer2 > 1000 )
   dbtimer2 = 0;
   if ( dbtimer3 > 1000 )
   dbtimer3 = 0;


   if (dbtimer >= 60)
   {
    created = 0;
    for ( db = 260; db <= 266; db++ )
    {
      if ( ( obj = get_db( db ) ) != NULL )
      continue;
      if ( ( pObjIndex = get_obj_index( db ) ) == NULL )
      continue;
      x = number_range( 0, MAX_X-1);
      y = number_range( 0, MAX_Y-1);
      sector = map_sector[MAP_EARTH][x][y];

      if ( sector == SECT_INSIDE || sector == SECT_EXIT || sector == SECT_BUILDING)
      {
         db--;
         continue;
      }
      created++;
      obj = create_object( pObjIndex, level );
      if ( sector != SECT_OCEAN && sector != SECT_RIVER)
      {
         obj = obj_to_room( obj, get_room_index(6801), NULL);
         obj->map = MAP_EARTH;
      }
      else
      {
         obj = obj_to_room( obj, get_room_index(6949), NULL);
         obj->map = MAP_BEARTH;
      }

      xSET_BIT(obj->extra_flags, ITEM_ONMAP);
      obj->x = x;
      obj->y = y;
    }
   }
   if (dbtimer2 >= 125)
   {
    created2 = 0;
    for ( db = 270; db <= 276; db++ )
    {
      if ( ( obj = get_db( db ) ) != NULL )
      continue;
      if ( ( pObjIndex = get_obj_index( db ) ) == NULL )
      continue;

      vnum = 0;
      if (db == 270)
      vnum = number_range( 6552, 6581);
      else if (db == 271)
      vnum = number_range( 501, 545);
      else if (db == 272)
      vnum = number_range( 801, 840);
      else if (db == 273)
      vnum = number_range( 902, 927);
      else if (db == 274)
      vnum = number_range( 7302, 7399);
      else if (db == 275)
      vnum = number_range( 251, 251);
      else if (db == 276)
      vnum = number_range( 8152, 8232);
         
      if ( (get_room_index(vnum)) == NULL )
      db--;   
      created2++;
      obj = create_object( pObjIndex, level );
      obj = obj_to_room( obj, get_room_index(vnum), NULL);
    }
   }
   if (dbtimer3 >= 80)
   {
    created3 = 0;
    for ( db = 253; db <= 260; db++ )
    {
      if ( ( obj = get_db( db ) ) != NULL )
      continue;
      if ( ( pObjIndex = get_obj_index( db ) ) == NULL )
      continue;

      vnum = number_range(8060, 8144 );
         
      if ( (get_room_index(vnum)) == NULL )
      {
      db--;   
      continue;
      }
      created3++;
      obj = create_object( pObjIndex, level );
      obj = obj_to_room( obj, get_room_index(vnum), NULL);
    }
   }

      if (created > 4)
        echo_to_all( AT_YELLOW, "\n\rThe stones that were once Dragonballs have turned into orange spheres\n\rwith red stars and they have been scattered across Planet Earth.\n\r", ECHOTAR_ALL );
      if (created2 > 4)
       echo_to_all( AT_YELLOW, "\n\r&zThe stones that were once &ODragonballs&z have turned into orange spheres\n\rwith &Wblack stars&z and they have been scattered across the Universe.\n\r", ECHOTAR_ALL );
      if (created3 > 4)
        echo_to_all( AT_YELLOW, "\n\r&WThe &cstones&W that were once &cDragonballs&W have turned into big &Oorange&C spheres\n\r&Wwith &Rred&W stars and they have been scattered across &YPlanet Namek&W.\n\r", ECHOTAR_ALL );

}

void update_shuttles( void )
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    char buf[MSL];
    for ( ship = first_ship; ship; ship = ship->next )
    {
       if (ship->lock != SHIP_SHUTTLE || !ship->first_stop || !ship->second_stop || ship->ship_state != 0)
       continue;
  
       if (time_info.hour == ship->launch1 )
       {
         if (ship->current_loc == ship->first_stop->room )
         continue;
         obj = get_obj_ship( ship->obj_vnum ) ;
         if ( obj )
         extract_obj( obj );

         ship->x2 = ship->first_stop->x;
         ship->y2 = ship->first_stop->y;
         ship->z2 = ship->first_stop->z;
         ship->d2 = ship->first_stop->d;
         ship->current_loc = 0;
         ship->ship_state = 2;
         ship->destination = ship->first_stop->room;
         echo_to_room( AT_WHITE, get_room_index( ship->entrance ),
         "\n\rSomeone enters new target coordinates.\n\r"
         "Someone use its controlcard to active the ship's powersupply...\n\r\n\r"                                        
	 "The monitors and controls switch glow up...the system is online.\n\r\n\r"
	 "Someone presses the launch button to start your journey...\n\r\n\r"
	 "You feel the spaceship trembling and shaking as the engines power up...\n\r\n\r"
	 "You get firmly pressed into your chair as the spaceship blasts off into orbit!\n\r\n\r");
         sprintf(buf, "\n\rYou hear space-engines hauling as they are being throttled up...\n\r\n\rSmoke and bright light surrounds %s as it rises\n\roff the ground slowly, turning around, preparing for lift-off...\n\r\n\rWith a loud explosion of power the spaceship blasts off into orbit!\n\r", ship->name);
         echo_to_room( AT_WHITE, get_room_index( ship->current_loc ), buf );
       }
       if (time_info.hour == ship->launch2 )
       {
         if (ship->current_loc == ship->second_stop->room )
         continue;
         obj = get_obj_ship( ship->obj_vnum );
         if ( obj )
         extract_obj( obj );
         ship->x2 = ship->second_stop->x;
         ship->y2 = ship->second_stop->y;
         ship->z2 = ship->second_stop->z;
	 ship->current_loc = 0;
         ship->d2 = ship->second_stop->d;
         ship->ship_state = 2;
         ship->destination = ship->second_stop->room;
         echo_to_room( AT_WHITE, get_room_index( ship->entrance ),
         "\n\rSomeone enters new target coordinates.\n\r"
         "Someone use its controlcard to active the ship's powersupply...\n\r\n\r"
         "The monitors and controls switch glow up...the system is online.\n\r\n\r"
         "Someone presses the launch button to start your journey...\n\r\n\r"
         "You feel the spaceship trembling and shaking as the engines power up...\n\r\n\r"
         "You get firmly pressed into your chair as the spaceship blasts off into orbit!\n\r\n\r");
         sprintf(buf, "\n\rYou hear space-engines hauling as they are being throttled up...\n\r\n\rSmoke and bright light surrounds %s as it rises\n\roff the ground slowly, turning around, preparing for lift-off...\n\r\n\rWith a loud explosion of power the spaceship blasts off into orbit!\n\r", ship->name);
         echo_to_room( AT_WHITE, get_room_index( ship->current_loc ), buf );
       }
       if ( ship == last_ship )
       return;
     }

}
void space_update(void)
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    int vnum;
    bool found=FALSE;
    for ( ship = first_ship; ship; ship = ship->next )
    {
     switch (ship->ship_state)
     {
         case 0:
         break;
         case 1:
            echo_to_room(AT_YELLOW, get_room_index( ship->entrance ), "The ship keeps hovering in mid-air...");
            found = TRUE;
         break;
         case 2:
            do_land( ship );
         break;
     }
     vnum = ship->obj_vnum;
     obj = get_obj_ship( vnum );
     if ( !obj )
     {
      if (ship->ship_state == 0)
      {
       obj = create_object( get_obj_index( ship->obj_vnum ), 0 );
       obj_to_room( obj, get_room_index( ship->home ), NULL );
       ship->ship_state = 0;
       ship->current_loc = ship->home;
       ship->destination = 0;
      }
      continue;
     }
     if ( ship == last_ship )
     break;
   }
   return;
}
/*
 * Update all objs.
 * This function is performance sensitive.
 */

void obj_update( void )
{   
    OBJ_DATA *obj;
    sh_int AT_TEMP;
    int newmap, sector;
    for ( obj = last_object; obj; obj = gobj_prev )
    {
	CHAR_DATA *rch;
	char *message;
        sector = 0;
	if ( obj == first_object && obj->prev )
	{
	    bug( "obj_update: first_object->prev != NULL... fixed", 0 );
	    obj->prev = NULL;
	}
	gobj_prev = obj->prev;
	if ( gobj_prev && gobj_prev->next != obj )
	{
	    bug( "obj_update: obj->prev->next != obj (%s)", 0, obj->name ? obj->name : "None" );
	    return;
	}
	set_cur_obj( obj );
	if ( obj->carried_by )
	  oprog_random_trigger( obj ); 
	else
	if( obj->in_room && obj->in_room->area->nplayer > 0 )
	  oprog_random_trigger( obj ); 

        if( obj_extracted(obj) )
	  continue;
        newmap = obj->map;
        if ( xIS_SET(obj->extra_flags, ITEM_ONMAP) && obj->map != -1)
        {
          if ( get_mapbelow( newmap ) != TRUE )
          {
           if ( get_mapabove( newmap ) == TRUE && sector == SECT_AIR)
           {
             newmap = getmapbelow( newmap );
           }
           sector = map_sector[newmap][obj->x][obj->y];
           if ( get_mapmiddle( newmap ) == TRUE && (sector == SECT_OCEAN || sector == SECT_RIVER) )
           {
             newmap = getmapbelow( newmap );
           }
          }
          obj->map = newmap;
        }

	if ( obj->item_type == ITEM_PIPE )
	{
	    if ( IS_SET( obj->value[3], PIPE_LIT ) )
	    {
		if ( --obj->value[1] <= 0 )
		{
		  obj->value[1] = 0;
		  REMOVE_BIT( obj->value[3], PIPE_LIT );
		}
		else
		if ( IS_SET( obj->value[3], PIPE_HOT ) )
		  REMOVE_BIT( obj->value[3], PIPE_HOT );
		else
		{
		  if ( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
		  {
		    REMOVE_BIT( obj->value[3], PIPE_LIT );
		    REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
		  }
		  else
		    SET_BIT( obj->value[3], PIPE_GOINGOUT );
		}
		if ( !IS_SET( obj->value[3], PIPE_LIT ) )
		  SET_BIT( obj->value[3], PIPE_FULLOFASH );
	    }
	    else
	      REMOVE_BIT( obj->value[3], PIPE_HOT );
	}


/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

        if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
        {
          sh_int timerfrac = UMAX(1, obj->timer - 1);
          if ( obj->item_type == ITEM_CORPSE_PC )
	    timerfrac = (int)(obj->timer / 8 + 1);

	  if ( obj->timer > 0 && obj->value[2] > timerfrac )
	  {
            char buf[MAX_STRING_LENGTH];
            char name[MAX_STRING_LENGTH];
            char *bufptr;
            bufptr = one_argument( obj->short_descr, name ); 
            bufptr = one_argument( bufptr, name ); 
            bufptr = one_argument( bufptr, name ); 

	    separate_obj(obj);
            obj->value[2] = timerfrac; 
            sprintf( buf, corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
                          bufptr ); 

            STRFREE( obj->description );
            obj->description = STRALLOC( buf ); 
          }  
        }
       
	/* don't let inventory decay */
	if ( IS_OBJ_STAT(obj, ITEM_INVENTORY) )
	  continue;

	/* groundrot items only decay on the ground */
	if ( IS_OBJ_STAT(obj, ITEM_GROUNDROT) && !obj->in_room )
	  continue;

	if ( ( obj->timer <= 0 || --obj->timer > 0 ) )
	  continue;
	
	/* if we get this far, object's timer has expired. */
 
         AT_TEMP = AT_PLAIN;
	 switch ( obj->item_type )
	 {
	 default:
	   message = "$p mysteriously vanishes.";
           AT_TEMP = AT_PLAIN;
	   break;
	 case ITEM_CONTAINER:
	   message = "$p falls apart, tattered from age.";
	   AT_TEMP = AT_OBJECT;
	   break; 
	 case ITEM_PORTAL:
	   message = "$p unravels and winks from existence.";
           remove_portal(obj);
	   obj->item_type = ITEM_TRASH;		/* so extract_obj	 */
           AT_TEMP = AT_MAGIC;			/* doesn't remove_portal */
	   break;
	 case ITEM_FOUNTAIN:
	   message = "$p dries up.";
           AT_TEMP = AT_BLUE;
	   break;
	 case ITEM_CORPSE_NPC:
	   message = "$p decays into dust and blows away.";
           AT_TEMP = AT_CORPSE;
	   break;
	 case ITEM_CORPSE_PC:
	   message = "$p is sucked into a swirling vortex of colors...";
           AT_TEMP = AT_MAGIC;
	   break;
	 case ITEM_COOK:
	 case ITEM_FOOD:
	   message = "$p is devoured by a swarm of maggots.";
           AT_TEMP = AT_HUNGRY;
	   break;
         case ITEM_BLOOD:
           message = "$p slowly seeps into the ground.";
           AT_TEMP = AT_BLOOD;
           break;
         case ITEM_BLOODSTAIN:
           message = "$p dries up into flakes and blows away.";
           AT_TEMP = AT_BLOOD;
	   break;
         case ITEM_SCRAPS:
           message = "$p crumble and decay into nothing.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_FIRE:
	   /* This is removed because it is done in obj_from_room
	    * Thanks to gfinello@mail.karmanet.it for pointing this out.
	    * --Shaddai
	   if (obj->in_room)
	   {
	     --obj->in_room->light;
	     if ( obj->in_room->light < 0 )
			obj->in_room->light = 0;
	   }
	   */
	   message = "$p burns out.";
	   AT_TEMP = AT_FIRE;
	 }

	if ( obj->carried_by )
	{
	    act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room
	&&      ( rch = obj->in_room->first_person ) != NULL
	&&	!IS_OBJ_STAT( obj, ITEM_BURIED ) )
	{
	    act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
            act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
	}

	if ( obj->serial == cur_obj )
	  global_objcode = rOBJ_EXPIRED;
	extract_obj( obj );
    }
    return;
}


/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check( void )
{
    CHAR_DATA *ch, *ch_next;
    EXIT_DATA *pexit;
    static int cnt = 0;
    int door, retcode;

    /* This little counter can be used to handle periodic events */
    cnt = (cnt+1) % SECONDS_PER_TICK;
    for ( ch = first_char; ch; ch = ch_next )
    {
	set_cur_char(ch);
	ch_next = ch->next;
	will_fall(ch, 0);

	if ( char_died( ch ) )
	  continue;

	if ( IS_NPC( ch ) )
	{
	    if ( (cnt & 1) )
		continue;

	    /* running mobs	-Thoric */
	    if ( xIS_SET(ch->act, ACT_RUNNING) )
	    {
		if ( !xIS_SET(ch->act, ACT_SENTINEL)
		&&   !ch->fighting && ch->hunting )
		{
		    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		    hunt_victim( ch );
		    continue;
		}

		if ( ch->spec_fun )
		{
		    if ( (*ch->spec_fun) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}

      	/* Map wanderers - Samson 7-28-00 */
          	if( IS_ACT_FLAG( ch, ACT_ONMAP ) )
		{
	   	   short sector = ch->sector;
	   	   short map = ch->map;
	   	   short x = ch->x;
	   	   short y = ch->y;
                   short newsector = 0;
         	   int dir = number_bits( 5 );

	   	   if( dir < DIR_SOMEWHERE && dir != DIR_UP && dir != DIR_DOWN )
	   	   {
			switch( dir )
			{
		   	   case DIR_NORTH:
                                newsector = get_terrain( map, x, y-1 );
				if( map_wander( ch, map, x, y-1, sector, newsector ) )
			   	   move_char( ch, NULL, 0, DIR_NORTH );
		   		break;
		   	   case DIR_NORTHEAST:
                                newsector = get_terrain( map, x+1, y-1 );
				if( map_wander( ch, map, x+1, y-1, sector, newsector ) )
			   	   move_char( ch, NULL, 0, DIR_NORTHEAST );
		   		break;
		   	   case DIR_EAST:
                                newsector = get_terrain( map, x+1, y );
				if( map_wander( ch, map, x+1, y, sector, newsector) )
			   	   move_char( ch, NULL, 0, DIR_EAST );
		   		break;
		   	   case DIR_SOUTHEAST:
                                newsector = get_terrain( map, x+1, y+1 );
				if( map_wander( ch, map, x+1, y+1, sector, newsector) )
			   	   move_char( ch, NULL, 0, DIR_SOUTHEAST );
		   		break;
		   	   case DIR_SOUTH:
                                newsector = get_terrain( map, x, y+1 );
				if( map_wander( ch, map, x, y+1, sector, newsector) )
			   	   move_char( ch, NULL, 0, DIR_SOUTH );
		   		break;
		   	   case DIR_SOUTHWEST:
                                newsector = get_terrain( map, x-1, y+1 );
				if( map_wander( ch, map, x-1, y+1, sector, newsector) )
			   	   move_char( ch, NULL, 0, DIR_SOUTHWEST );
		   		break;
		   	   case DIR_WEST:
                                newsector = get_terrain( map, x-1, y );
				if( map_wander( ch, map, x-1, y, sector, newsector) )
			   	   move_char( ch, NULL, 0, DIR_WEST );
		   		break;
		   	   case DIR_NORTHWEST:
                                newsector = get_terrain( map, x-1, y-1 );
				if( map_wander( ch, map, x-1, y-1, sector, newsector ) )
			   	   move_char( ch, NULL, 0, DIR_NORTHWEST );
		   		break;
			}
	   	   }
	   	   if( char_died(ch) )
			continue;
		}

		if ( !xIS_SET(ch->act, ACT_SENTINEL)
		&&   !xIS_SET(ch->act, ACT_PROTOTYPE)
		&& ( door = number_bits( 4 ) ) <= 9
		&& ( pexit = get_exit(ch->in_room, door) ) != NULL
		&&   pexit->to_room
		&&   !IS_SET(pexit->exit_info, EX_CLOSED)
		&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		&&   !IS_SET(pexit->to_room->room_flags, ROOM_DEATH)
		&& ( !xIS_SET(ch->act, ACT_STAY_AREA)
		||   pexit->to_room->area == ch->in_room->area ) )
		{
		    retcode = move_char( ch, pexit, 0, pexit->vdir );
		    if ( char_died(ch) )
			continue;
		    if ( retcode != rNONE || xIS_SET(ch->act, ACT_SENTINEL)
		    ||    ch->position < POS_STANDING )
			continue;
		}
	    }
	    continue;
	}
	else
	{
	    if ( ch->mount
	    &&   ch->in_room != ch->mount->in_room )
	    {
		xREMOVE_BIT( ch->mount->act, ACT_MOUNTED );
		ch->mount = NULL;
		ch->position = POS_STANDING;
		send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
	    }

	
	    if ( char_died( ch ) )
		continue; 

	    /* beat up on link dead players */
	    if ( !ch->desc )
	    {
		CHAR_DATA *wch, *wch_next;

		for ( wch = ch->in_room->first_person; wch; wch = wch_next )
		{
		    wch_next	= wch->next_in_room;

		    if (!IS_NPC(wch)
		    ||   wch->fighting
		    ||   !IS_AWAKE(wch)
		    || (xIS_SET(wch->act, ACT_WIMPY) && IS_AWAKE(ch) )
		    ||   !can_see( wch, ch, FALSE ) )
			continue;

		    if ( is_hating( wch, ch ) )
		    {
			found_prey( wch, ch );
			continue;
		    }

		    if ( (!xIS_SET(wch->act, ACT_AGGRESSIVE)
		    &&    !xIS_SET(wch->act, ACT_META_AGGR))
		    ||    xIS_SET(wch->act, ACT_MOUNTED)
		    ||    IS_SET(wch->in_room->room_flags, ROOM_SAFE ) )
			continue;
		    global_retcode = multi_hit( wch, ch, TYPE_UNDEFINED );
		}
	    }
	}
    }
}


/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update( void )
{
    DESCRIPTOR_DATA *d, *dnext;
    CHAR_DATA *wch;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;
    struct act_prog_data *apdtmp;

#ifdef UNDEFD
  /*
   *  GRUNT!  To do
   *
   */
        if ( IS_NPC( wch ) && wch->mpactnum > 0
	    && wch->in_room->area->nplayer > 0 )
        {
            MPROG_ACT_LIST * tmp_act, *tmp2_act;
	    for ( tmp_act = wch->mpact; tmp_act;
		 tmp_act = tmp_act->next )
	    {
                 oprog_wordlist_check( tmp_act->buf,wch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, ACT_PROG );
                 DISPOSE( tmp_act->buf );
            }
	    for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
	    {
                 tmp2_act = tmp_act->next;
                 DISPOSE( tmp_act );
            }
            wch->mpactnum = 0;
            wch->mpact    = NULL;
        }
#endif

    /* check mobprog act queue */
    while ( (apdtmp = mob_act_list) != NULL )
    {
	wch = mob_act_list->vo;
	if ( !char_died(wch) && wch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST * tmp_act;

	    while ( (tmp_act = wch->mpact) != NULL )
	    {
		if ( tmp_act->obj && obj_extracted(tmp_act->obj) )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !char_died(tmp_act->ch) )
		  mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		wch->mpact = tmp_act->next;
		DISPOSE(tmp_act->buf);
		DISPOSE(tmp_act);
	    }
	    wch->mpactnum = 0;
	    wch->mpact    = NULL;
        }
	mob_act_list = apdtmp->next;
	DISPOSE( apdtmp );
    }


    /*
     * Just check descriptors here for victims to aggressive mobs
     * We can check for linkdead victims in char_check	-Thoric
     */
    for ( d = first_descriptor; d; d = dnext )
    {
	dnext = d->next;
	if ( d->connected != CON_PLAYING || (wch=d->character) == NULL )
	   continue;

	if ( char_died(wch)
	||   IS_NPC(wch)
	||   wch->level >= LEVEL_IMMORTAL
	||  !wch->in_room )
	    continue;

	for ( ch = wch->in_room->first_person; ch; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   ch->fighting
	    ||   !IS_AWAKE(ch)
	    ||   (xIS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch, FALSE ) )
		continue;

	    if ( is_hating(ch, wch) )
	    {
		found_prey(ch, wch);
		continue;
	    }

	    if ( (!xIS_SET(ch->act, ACT_AGGRESSIVE)
	    &&    !xIS_SET(ch->act, ACT_META_AGGR))
	    ||     xIS_SET(ch->act, ACT_MOUNTED)
	    ||     IS_SET(ch->in_room->room_flags, ROOM_SAFE ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     *
	     * Depending on flags set, the mob may attack another mob
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->first_person; vch; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( (!IS_NPC(vch) || xIS_SET(ch->act, ACT_META_AGGR))
		&&   vch->level < LEVEL_IMMORTAL
		&&   (!xIS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch, FALSE ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( !victim )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

	    global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}

/* From interp.c */
bool check_social  args( ( CHAR_DATA *ch, char *command, char *argument ) );

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms( CHAR_DATA *ch )
{
    CHAR_DATA *rvch = NULL;
    CHAR_DATA *vch;
    sh_int drunk;
    sh_int position;

    if ( IS_NPC( ch ) || ch->drunk <= 0 )
	return;

    if ( number_percent() < 30 )
	return;

    drunk = ch->drunk;
    position = ch->position;
    ch->position = POS_STANDING;

    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "burp", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "hiccup", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "drool", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "sigh", "" );
    else
    if ( drunk > (10+(get_curr_con(ch)/5))
    &&   number_percent() < ( 2 * drunk / 18 ) )
    {
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	    if ( number_percent() < 10 )
		rvch = vch;
	check_social( ch, "puke", (rvch ? rvch->name : "") );
    }

    ch->position = position;
    return;
}


void tele_update( void )
{
    TELEPORT_DATA *tele, *tele_next;

    if ( !first_teleport )
      return;
    
    for ( tele = first_teleport; tele; tele = tele_next )
    {
	tele_next = tele->next;
	if ( --tele->timer <= 0 )
	{
	    if ( tele->room->first_person )
	    {
		if ( IS_SET( tele->room->room_flags, ROOM_TELESHOWDESC ) )
		  teleport( tele->room->first_person, tele->room->tele_vnum,
			TELE_SHOWDESC | TELE_TRANSALL );
		else
		  teleport( tele->room->first_person, tele->room->tele_vnum,
			TELE_TRANSALL );
	    }
	    UNLINK( tele, first_teleport, last_teleport, next, prev );
	    DISPOSE( tele );
	}
    }
}

#if FALSE
/* 
 * Write all outstanding authorization requests to Log channel - Gorog
 */ 
void auth_update( void ) 
{ 
  CHAR_DATA *victim; 
  DESCRIPTOR_DATA *d; 
  char log_buf [MAX_INPUT_LENGTH];
  bool first_time = TRUE;         /* so titles are only done once */

  for ( d = first_descriptor; d; d = d->next ) 
      {
      victim = d->character;
      if ( victim && IS_WAITING_FOR_AUTH(victim) )
         {
         if ( first_time )
            {
            first_time = FALSE;
            strcpy (log_buf, "Pending authorizations:" ); 
            /*log_string( log_buf ); */
            to_channel( log_buf, CHANNEL_AUTH, "Auth", 1);
            }
         sprintf( log_buf, " %s@%s new %s %s", victim->name,
            victim->desc->host, race_table[victim->race]->race_name, 
            class_table[victim->class]->who_name ); 
/*         log_string( log_buf ); */
         to_channel( log_buf, CHANNEL_AUTH, "Auth", 1);
         }
      }
} 
#endif

void auth_update( void ) 
{ 
    CHAR_DATA *victim; 
    DESCRIPTOR_DATA *d; 
    char buf [MAX_INPUT_LENGTH], log_buf [MAX_INPUT_LENGTH];
    bool found_hit = FALSE;         /* was at least one found? */

    strcpy (log_buf, "Pending authorizations:\n\r" );
    for ( d = first_descriptor; d; d = d->next ) 
    {
	if ( (victim = d->character) && IS_WAITING_FOR_AUTH(victim) )
	{
	    found_hit = TRUE;
	    sprintf( buf, " %s@%s new %s %s\n\r", victim->name,
		victim->desc->host, race_table[victim->race]->race_name, 
		class_table[victim->class]->who_name );
	    strcat (log_buf, buf);
	}
    }
    if (found_hit)
    {
/*	log_string( log_buf ); */
	to_channel( log_buf, CHANNEL_AUTH, "Auth", 1);
    }
} 

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void do_changes( CHAR_DATA *ch, char *argument );

void update_handler( void )
{
    static  int     pulse_start_arena = PULSE_AUCTION * 2;
    static  int     pulse_arena = PULSE_AUCTION * 2;
    static  int     pulse_bet;
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_space;
    static  int	    pulse_second;
    static  int     pulse_db;
    struct timeval stime;
    struct timeval etime;

    if ( timechar )
    {
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Starting update timer.\n\r", timechar );
      gettimeofday(&stime, NULL);
    }
    
    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update  ( );
    }
    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
        breath_update( );
    }
    if(in_start_arena || ppl_challenged)
    if( --pulse_start_arena <= 0)
    {
      pulse_start_arena = PULSE_AUCTION * 2;
      start_arena();
    }
    if( in_start_koth )
    if( --pulse_start_arena <= 0)
    {
      pulse_start_arena = PULSE_AUCTION * 2;
      start_koth();
    }
    if( ppl_in_koth )
    if(( --pulse_arena <= 0) || (num_in_koth()==1))
    {
      pulse_arena = PULSE_AUCTION * 2;
      do_koth_game();
    }
    if(ppl_in_arena)
    if(( --pulse_arena <= 0) || (num_in_arena()==1))
    {
      pulse_arena = PULSE_AUCTION * 2;
      do_game();
    }
    if( -- pulse_bet <= 0 )
    {
      pulse_bet = PULSE_BET;
      bet_update();
    }
    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK * 0.75, PULSE_TICK * 0.95 );
 
        auth_update     ( );			/* Gorog */
        time_update	( );
	weather_update	( );
	do_who(NULL, "www");
	do_clans( NULL, "www" );
	do_planets( NULL, "www" );
        do_changes( NULL, "www" );
	char_update	( );
	obj_update	( );
	clear_vrooms	( );			/* remove virtual rooms */
        moon_update     ( );
        update_shuttles ( );        
    }
    if ( --pulse_db       <= 0 )
    {
        pulse_db	= PULSE_DB;
        db_update       ( );
        income_update   ( );
    }

    if ( --pulse_space    <= 0 )
    {
       pulse_space     = PULSE_SPACE;
       
       space_update     ( );
    }

    if ( --pulse_second   <= 0 )
    {
	pulse_second	= PULSE_PER_SECOND;
	char_check( );
	check_pfiles(0);
 	reboot_check(0);
 	update_timers( );
      skill_update( );
    }

    if ( --auction->pulse <= 0 )
    {                                                  
	auction->pulse = PULSE_AUCTION;                     
	auction_update( );
	reset_sector( );
    }

    mpsleep_update(); 
    tele_update( );
    aggr_update( );
    obj_act_update ( );
    room_act_update( );
    clean_obj_queue( );		/* dispose of extracted objects */
    clean_char_queue( );       /* dispose of dead mobs/quitting chars */
    if ( timechar )
    {
      gettimeofday(&etime, NULL);
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Update timing complete.\n\r", timechar );
      subtract_times(&etime, &stime);
      ch_printf( timechar, "Timing took %d.%06d seconds.\n\r",
          etime.tv_sec, etime.tv_usec );
      timechar = NULL;
    }
    tail_chain( );
    return;
}


void remove_portal( OBJ_DATA *portal )
{
    ROOM_INDEX_DATA *fromRoom, *toRoom;
    EXIT_DATA *pexit;
    bool found;

    if ( !portal )
    {
	bug( "remove_portal: portal is NULL", 0 );
	return;
    }

    fromRoom = portal->in_room;
    found = FALSE;
    if ( !fromRoom )
    {
	bug( "remove_portal: portal->in_room is NULL", 0 );
	return;
    }

    for ( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
	if ( IS_SET( pexit->exit_info, EX_PORTAL ) )
	{
	    found = TRUE;
	    break;
	}

    if ( !found )
    {
	bug( "remove_portal: portal not found in room %d!", fromRoom->vnum );
	return;
    }

    if ( pexit->vdir != DIR_PORTAL )
	bug( "remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir );

    if ( ( toRoom = pexit->to_room ) == NULL )
      bug( "remove_portal: toRoom is NULL", 0 );
 
    extract_exit( fromRoom, pexit );

    return;
}

void reboot_check( time_t reset )
{
  DESCRIPTOR_DATA *i;
  int count=0;
  static char *tmsg[] =
  { "You feel the ground shake as the end comes near!",
    "Lightning crackles in the sky above!",
    "Crashes of thunder sound across the land!",
    "The sky has suddenly turned midnight black.",
    "You notice the life forms around you slowly dwindling away.",
    "The seas across the realm have turned frigid.",
    "The aura of magic that surrounds the realms seems slightly unstable.",
    "You sense a change in the magical forces surrounding you."
  };
  static const int times[] = { 60, 120, 180, 240, 300, 600, 900, 1800 };
  static const int timesize =
      UMIN(sizeof(times)/sizeof(*times), sizeof(tmsg)/sizeof(*tmsg));
  char buf[MAX_STRING_LENGTH];
  static int trun;
  static bool init = FALSE;
  
  if ( !init || reset >= current_time )
  {
    for ( trun = timesize-1; trun >= 0; trun-- )
      if ( reset >= current_time+times[trun] )
        break;
    init = TRUE;
    return;
  }
  for (i = first_descriptor; i; i = i->next)
  {
    count++;
  }
  
  if ( (current_time % 1800) == 0 )
  {
    sprintf(buf, "%.24s: %d players", ctime(&current_time), count);
    append_to_file(USAGE_FILE, buf);
    sprintf(buf, "%.24s:  %dptn  %dpll  %dsc %dbr  %d global loot",
	ctime(&current_time),
	sysdata.upotion_val,
	sysdata.upill_val,
	sysdata.scribed_used,
	sysdata.brewed_used,
	sysdata.global_looted );
    append_to_file( ECONOMY_FILE, buf );
  }
  
  if ( new_boot_time_t - boot_time < 60*60*18 &&
      !set_boot_time->manual )
    return;
  
  if ( new_boot_time_t <= current_time )
  {
    CHAR_DATA *vch;
    extern bool mud_down;
    
    if ( auction->item )
    {
      sprintf(buf, "Sale of %s has been stopped by mud.",
          auction->item->short_descr);
      talk_auction(buf);
      obj_to_char(auction->item, auction->seller);
      auction->item = NULL;
      if ( auction->buyer && auction->buyer != auction->seller )
      {
        auction->buyer->gold += auction->bet;
        send_to_char("Your money has been returned.\n\r", auction->buyer);
      }
    }
    echo_to_all(AT_YELLOW, "You are forced from these realms by a strong "
        "magical presence\n\ras life here is reconstructed.", ECHOTAR_ALL);
    log_string( "Automatic Reboot" ); 
    for ( vch = first_char; vch; vch = vch->next )
      if ( !IS_NPC(vch) )
        save_char_obj(vch);
    mud_down = TRUE;
    return;
  }
  
  if ( trun != -1 && new_boot_time_t - current_time <= times[trun] )
  {
    echo_to_all(AT_YELLOW, tmsg[trun], ECHOTAR_ALL);
    if ( trun <= 5 )
      sysdata.DENY_NEW_PLAYERS = TRUE;
    --trun;
    return;
  }
  return;
}
  
/* the auction update*/

void auction_update (void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];

    if(!auction->item)
    {
    	if(AUCTION_MEM > 0 && auction->history[0] &&
    			++auction->hist_timer == 6*AUCTION_MEM)
    	{
    		int i;
    		
    		for(i = AUCTION_MEM - 1; i >= 0; i--)
    		{
    			if(auction->history[i])
    			{
    				auction->history[i] = NULL;
    				auction->hist_timer = 0;
    				break;
    			}
    		}
    	}
    	return;
    }

    switch (++auction->going) /* increase the going state */
    {
	case 1 : /* going once */
	case 2 : /* going twice */
	    if (auction->bet > auction->starting)
		sprintf (buf, "%s&C: going %s for &Y%s&C.", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"), num_punct( auction->bet ) );
	    else
		sprintf (buf, "%s&C: going %s (bid not received yet).",  auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"));

	    talk_auction (buf);
	    break;

	case 3 : /* SOLD! */
	    if (!auction->buyer && auction->bet)
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d gold bid", auction->bet );
		auction->bet = 0;
 	    }
            if ( !auction->item )
            {
                bug( "No auction Item" );
                auction->bet = 0;
            }

	    if (auction->bet > 0 && auction->buyer != auction->seller)
	    {
		sprintf (buf, "%s &Csold to %s for &Y%s.",
			auction->item->short_descr,
			IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
			num_punct(auction->bet) );
		talk_auction(buf);

		act(AT_ACTION, "The auctioneer materializes before you, and hands you $p&w.",
			auction->buyer, auction->item, NULL, TO_CHAR);
		act(AT_ACTION, "The auctioneer materializes before $n, and hands $m $p&w.",
			auction->buyer, auction->item, NULL, TO_ROOM);
		if ( auction->item->item_type == ITEM_SENSU && get_sensu_carried( auction->buyer ) >= 20 )
                {
                    act( AT_PLAIN, "You can't carry any more $p.", auction->buyer,auction->item, NULL, TO_CHAR );
                     act( AT_PLAIN, "$n is carrying too much $p and $e drops it.", auction->buyer,auction->item, NULL, TO_ROOM );
                    obj_to_room( auction->item, auction->buyer->in_room, auction->buyer );
                }
		else if ( (auction->buyer->carry_weight 
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->buyer ) )
		{
 		    act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item, NULL, TO_CHAR );
     		    act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room, auction->buyer );
		}
		else
		    obj_to_char( auction->item, auction->buyer );
	        pay = (int)auction->bet * 0.9;
		tax = (int)auction->bet * 0.1;
		boost_economy( auction->seller->in_room->area, tax );
                auction->seller->gold += pay; /* give him the money, tax 10 % */
        ch_printf( auction->seller, "The auctioneer pays you %s gold, charging an auction fee of ",
               num_punct(pay));
        ch_printf( auction->seller, "%s.\n\r", num_punct(tax));

                auction->item = NULL; /* reset item */
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		{
		    save_char_obj( auction->buyer );
		    save_char_obj( auction->seller );
		}
            }
            else /* not sold */
            {
                sprintf (buf, "No bids received for %s - removed from auction.\n\r",auction->item->short_descr);
                talk_auction(buf);
                act (AT_ACTION, "The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act (AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
                if ( auction->item->item_type == ITEM_SENSU && get_sensu_carried( auction->seller ) >= 20 )
                {
                    act( AT_PLAIN, "You can't carry any more $p.", auction->seller,auction->item, NULL, TO_CHAR );
                    act( AT_PLAIN, "$n is carrying too much $p and $e drops it.", auction->seller,auction->item, NULL, TO_ROOM );
                    obj_to_room( auction->item, auction->seller->in_room, auction->seller );
                }

		if ( (auction->seller->carry_weight
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->seller ) )
		{
		    act( AT_PLAIN, "You drop $p as it is just too much to carry"
			" with everything else you're carrying.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( AT_PLAIN, "$n drops $p as it is too much extra weight"
			" for $m with everything else.", auction->seller,
			auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room, auction->seller );
		}
		else
		    obj_to_char (auction->item,auction->seller);
		tax = (int)auction->item->cost * 0.05;
		boost_economy( auction->seller->in_room->area, tax );
		sprintf(buf, "The auctioneer charges you an auction fee of %s.\n\r", num_punct(tax) );
		send_to_char(buf, auction->seller);
		if ((auction->seller->gold - tax) < 0)
		  auction->seller->gold = 0;
		else
		  auction->seller->gold -= tax;
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		    save_char_obj( auction->seller );
	    } /* else */
	    auction->item = NULL; /* clear auction */
    } /* switch */
} /* func */

void subtract_times(struct timeval *etime, struct timeval *stime)
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while ( etime->tv_usec < 0 )
  {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}

/*
 * Function to update weather vectors according to climate
 * settings, random effects, and neighboring areas.
 * Last modified: July 18, 1997
 * - Fireblade
 */
void adjust_vectors(WEATHER_DATA *weather)
{
	NEIGHBOR_DATA *neigh;
	double dT, dP, dW;

	if(!weather)
	{
		bug("adjust_vectors: NULL weather data.", 0);
		return;
	}

	dT = 0;
	dP = 0;
	dW = 0;

	/* Add in random effects */
	dT += number_range(-rand_factor, rand_factor);
	dP += number_range(-rand_factor, rand_factor);
	dW += number_range(-rand_factor, rand_factor);
	
	/* Add in climate effects*/
	dT += climate_factor *
		(((weather->climate_temp - 2)*weath_unit) -
		(weather->temp))/weath_unit;
	dP += climate_factor *
		(((weather->climate_precip - 2)*weath_unit) -
		(weather->precip))/weath_unit;
	dW += climate_factor *
		(((weather->climate_wind - 2)*weath_unit) -
		(weather->wind))/weath_unit;
	
		
	/* Add in effects from neighboring areas */
	for(neigh = weather->first_neighbor; neigh;
			neigh = neigh->next)
	{
		/* see if we have the area cache'd already */
		if(!neigh->address)
		{
			/* try and find address for area */
			neigh->address = get_area(neigh->name);
			
			/* if couldn't find area ditch the neigh */
			if(!neigh->address)
			{
				NEIGHBOR_DATA *temp;
				bug("adjust_weather: "
					"invalid area name.", 0);
				temp = neigh->prev;
				UNLINK(neigh,
				       weather->first_neighbor,
				       weather->last_neighbor,
				       next,
				       prev);
				STRFREE(neigh->name);
				DISPOSE(neigh);
				neigh = temp;
				continue;
			}
		}
		
		dT +=(neigh->address->weather->temp -
		      weather->temp) / neigh_factor;
		dP +=(neigh->address->weather->precip -
		      weather->precip) / neigh_factor;
		dW +=(neigh->address->weather->wind -
		      weather->wind) / neigh_factor;
	}
	
	/* now apply the effects to the vectors */
	weather->temp_vector += (int)dT;
	weather->precip_vector += (int)dP;
	weather->wind_vector += (int)dW;

	/* Make sure they are within the right range */
	weather->temp_vector = URANGE(-max_vector,
		weather->temp_vector, max_vector);
	weather->precip_vector = URANGE(-max_vector,
		weather->precip_vector, max_vector);
	weather->wind_vector = URANGE(-max_vector,
		weather->wind_vector, max_vector);
	
	return;
}

/*
 * function updates weather for each area
 * Last Modified: July 31, 1997
 * Fireblade
 */
void weather_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	int limit;
	
	limit = 3 * weath_unit;
	
	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		/* Apply vectors to fields */
		pArea->weather->temp +=
			pArea->weather->temp_vector;
		pArea->weather->precip +=
			pArea->weather->precip_vector;
		pArea->weather->wind +=
			pArea->weather->wind_vector;
		
		/* Make sure they are within the proper range */
		pArea->weather->temp = URANGE(-limit,
			pArea->weather->temp, limit);
		pArea->weather->precip = URANGE(-limit,
			pArea->weather->precip, limit);
		pArea->weather->wind = URANGE(-limit,
			pArea->weather->wind, limit);
		
		/* get an appropriate echo for the area */
		get_weather_echo(pArea->weather);
	}

	for(pArea = first_area; pArea;
		pArea = (pArea == last_area) ? first_build : pArea->next)
	{
		adjust_vectors(pArea->weather);
	}
	
	/* display the echo strings to the appropriate players */
	for(d = first_descriptor; d; d = d->next)
	{
		WEATHER_DATA *weath;
		
		if(d->connected == CON_PLAYING &&
			IS_OUTSIDE(d->character) &&
			!NO_WEATHER_SECT(d->character->in_room->sector_type) &&
			IS_AWAKE(d->character))
		{
			weath = d->character->in_room->area->weather;
			if(!weath->echo)
				continue;
			set_char_color(weath->echo_color, d->character);
			ch_printf(d->character, weath->echo);
		}
	}
	
	return;
}

/*
 * get weather echo messages according to area weather...
 * stores echo message in weath_data.... must be called before
 * the vectors are adjusted
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_weather_echo(WEATHER_DATA *weath)
{
	int n;
	int temp, precip, wind;
	int dT, dP, dW;
	int tindex, pindex, windex;
	
	/* set echo to be nothing */
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	/* get the random number */
	n = number_bits(2);
	
	/* variables for convenience */
	temp = weath->temp;
	precip = weath->precip;
	wind = weath->wind;

	dT = weath->temp_vector;
	dP = weath->precip_vector;
	dW = weath->wind_vector;
	
	tindex = (temp + 3*weath_unit - 1)/weath_unit;
	pindex = (precip + 3*weath_unit - 1)/weath_unit;
	windex = (wind + 3*weath_unit - 1)/weath_unit;
	
	/* get the echo string... mainly based on precip */
	switch(pindex)
	{
		case 0:
			if(precip - dP > -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The clouds disappear.\n\r",
					"The clouds disappear.\n\r",
					"The sky begins to break through "
						"the clouds.\n\r",
					"The clouds are slowly "
						"evaporating.\n\r"
				};
				
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			break;

		case 1:
			if(precip - dP <= -2*weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky is getting cloudy.\n\r",
					"The sky is getting cloudy.\n\r",
					"Light clouds cast a haze over "
						"the sky.\n\r",
					"Billows of clouds spread through "
						"the sky.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_GREY;
			}
			break;
			
		case 2:
			if(precip - dP > 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The rain stops.\n\r",
						"The rain stops.\n\r",
						"The rainstorm tapers "
							"off.\n\r",
						"The rain's intensity "
							"breaks.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"The snow stops.\n\r",
						"The snow stops.\n\r",
						"The snow showers taper "
							"off.\n\r",
						"The snow flakes disappear "
							"from the sky.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			break;
			
		case 3:
			if(precip - dP <= 0)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"It starts to rain.\n\r",
						"It starts to rain.\n\r",
						"A droplet of rain falls "
							"upon you.\n\r",
						"The rain begins to "
							"patter.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_CYAN;
				}
				else
				{
					char *echo_strings[4] =
					{
						"It starts to snow.\n\r",
						"It starts to snow.\n\r",
						"Crystal flakes begin to "
							"fall from the "
							"sky.\n\r",
						"Snow flakes drift down "
							"from the clouds.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_WHITE;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"The temperature drops and the rain "
						"becomes a light snow.\n\r",
					"Flurries form as the rain freezes.\n\r",
					"Large snow flakes begin to fall "
						"with the rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The snow flurries are gradually "
						"replaced by pockets of rain.\n\r",
					"The falling snow turns to a cold drizzle.\n\r",
					"The snow turns to rain as the air warms.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			} 
			break;
		
		case 4:
			if(precip - dP > 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"The lightning has stopped.\n\r",
						"The lightning has stopped.\n\r",
						"The sky settles, and the "
							"thunder surrenders.\n\r",
						"The lightning bursts fade as "
							"the storm weakens.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_GREY;
				}
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The cold rain turns to snow.\n\r",
					"The cold rain turns to snow.\n\r",
					"Snow flakes begin to fall "
						"amidst the rain.\n\r",
					"The driving rain begins to freeze.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The snow becomes a freezing rain.\n\r",
					"The snow becomes a freezing rain.\n\r",
					"A cold rain beats down on you "
						"as the snow begins to melt.\n\r",
					"The snow is slowly replaced by a heavy "
						"rain.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		case 5:
			if(precip - dP <= 2*weath_unit)
			{
				if(tindex > 1)
				{
					char *echo_strings[4] =
					{
						"Lightning flashes in the "
							"sky.\n\r",
						"Lightning flashes in the "
							"sky.\n\r",
						"A flash of lightning splits "
							"the sky.\n\r",
						"The sky flashes, and the "
							"ground trembles with "
							"thunder.\n\r"
					};
					weath->echo = echo_strings[n];
					weath->echo_color = AT_YELLOW;
				}
			}
			else if(tindex > 1 && temp - dT <= -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The sky rumbles with thunder as "
						"the snow changes to rain.\n\r",
					"The falling turns to freezing rain "
						"amidst flashes of "
						"lightning.\n\r",
					"The falling snow begins to melt as "
						"thunder crashes overhead.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_WHITE;
			}
			else if(tindex < 2 && temp - dT > -weath_unit)
			{
				char *echo_strings[4] =
				{
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The lightning stops as the rainstorm "
						"becomes a blinding "
						"blizzard.\n\r",
					"The thunder dies off as the "
						"pounding rain turns to "
						"heavy snow.\n\r",
					"The cold rain turns to snow and "
						"the lightning stops.\n\r"
				};
				weath->echo = echo_strings[n];
				weath->echo_color = AT_CYAN;
			}
			break;
			
		default:
			bug("echo_weather: invalid precip index");
			weath->precip = 0;
			break;
	}

	return;
}

/*
 * get echo messages according to time changes...
 * some echoes depend upon the weather so an echo must be
 * found for each area
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_time_echo(WEATHER_DATA *weath)
{
	int n;
	int pindex;
	n = number_bits(2);
	pindex = (weath->precip + 3*weath_unit - 1)/weath_unit;
	weath->echo = NULL;
	weath->echo_color = AT_GREY;
	
	switch(time_info.hour)
	{
		case 5:
		{
			char *echo_strings[4] =
			{
				"The day has begun.\n\r",
				"The day has begun.\n\r",
				"The sky slowly begins to glow.\n\r",
				"The sun slowly embarks upon a new day.\n\r"
			};
			time_info.sunlight = SUN_RISE;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_YELLOW;
			break;
		}
		case 6:
		{
			char *echo_strings[4] =
			{
				"The sun rises in the east.\n\r",
				"The sun rises in the east.\n\r",
				"The hazy sun rises over the horizon.\n\r",
				"Day breaks as the sun lifts into the sky.\n\r"
			};
			time_info.sunlight = SUN_LIGHT;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_ORANGE;
			break;
		}
		case 12:
		{
			if(pindex > 0)
			{
				weath->echo = "It's noon.\n\r";
			}
			else
			{
				char *echo_strings[2] =
				{
					"The intensity of the sun "
						"heralds the noon hour.\n\r",
					"The sun's bright rays beat down "
						"upon your shoulders.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_LIGHT;
			weath->echo_color = AT_WHITE;
			break;
		}
		case 19:
		{
			char *echo_strings[4] =
			{
				"The sun slowly disappears in the west.\n\r",
				"The reddish sun sets past the horizon.\n\r",
				"The sky turns a reddish orange as the sun "
					"ends its journey.\n\r",
				"The sun's radiance dims as it sinks in the "
					"sky.\n\r"
			};
			time_info.sunlight = SUN_SET;
			weath->echo = echo_strings[n];
			weath->echo_color = AT_RED;
			break;
		}
		case 20:
		{
			if(pindex > 0)
			{
				char *echo_strings[2] =
				{
					"The night begins.\n\r",
					"Twilight descends around you.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			else
			{
				char *echo_strings[2] =
				{
					"The night sky gleams with glittering starlight.\n\r",
					"The night sky gleams with glittering starlight.\n\r"
				};
				weath->echo = echo_strings[n%2];
			}
			time_info.sunlight = SUN_DARK;
			weath->echo_color = AT_DBLUE;
			break;
		}
	}
	
	return;
}

/*
 * update the time
 */
void time_update()
{
	AREA_DATA *pArea;
	DESCRIPTOR_DATA *d;
	WEATHER_DATA *weath;
//        SHIP_DATA *ship;


	
	switch(++time_info.hour)
	{
		case 5:
		case 6:
		case 12:
		case 19:
		case 20:
			for(pArea = first_area; pArea;
				pArea = (pArea == last_area) ? first_build : pArea->next)
			{
				get_time_echo(pArea->weather);
			}
			
			for(d = first_descriptor; d; d = d->next)
			{
				if(d->connected == CON_PLAYING &&
					IS_OUTSIDE(d->character) &&
					IS_AWAKE(d->character))
				{
					weath = d->character->in_room->area->weather;
					if(!weath->echo)
						continue;
					set_char_color(weath->echo_color,
						d->character);
					ch_printf(d->character, weath->echo);
				}
			}
			break;
		case 24:
			time_info.hour = 0;
			time_info.day++;
			break;
	}
	
	if(time_info.day >= 30)
	{
		time_info.day = 0;
		time_info.month++;
	}
	
	if(time_info.month >= 12)
	{
		time_info.month = 0;
		time_info.year++;
	}
	
	return;
}



