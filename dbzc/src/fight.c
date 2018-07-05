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
 *			    Battle & death module			    *
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
#include "mud.h"

double sqrt    args(  (double Nbr) );
extern char		lastplayercmd[MAX_INPUT_LENGTH];
extern CHAR_DATA *	gch_prev;

OBJ_DATA *used_weapon;   /* Used to figure out which weapon later */
extern int time_left_in_koth;
/*
 * Local functions.
 */
bool    can_attack args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


void	new_dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt, OBJ_DATA *obj ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
int	align_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
ch_ret	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int	obj_hitroll	args( ( OBJ_DATA *obj ) );
void    show_condition  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* From newarena.c */
void lost_arena(CHAR_DATA *ch);

float calc_barrier( CHAR_DATA *ch )
{
   if (IS_NPC(ch))
   return 1;
   sh_int lvl = ch->pcdata->skill_level2[gsn_ebar];

   switch ( lvl )
   {
	case 0:
        case 1:
        case 2:
		return 0.95;
        case 3:
        case 4:
        case 5:
		return 0.93;
        case 6:
        case 7:
        case 8:
		return 0.91;
        case 9:
        case 10:
        case 11:
		return 0.89;
        case 12:
        case 13:
        case 14:
		return 0.87;
        case 15:
		return 0.85;
	default:
           return 1;
  }
  return 1;
}

bool has_dragonball2(CHAR_DATA *ch )
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


bool has_dragonball( CHAR_DATA *ch )
{
  OBJ_DATA *obj;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
      if (obj->pIndexData->vnum >= 253 && obj->pIndexData->vnum <= 266)
         return TRUE;
    }
  return FALSE;
}
bool has_timer( CHAR_DATA *ch )
{
  if (get_timer( ch, TIMER_PROPAGANDA ) > 0 )
  return TRUE;
  if (get_timer( ch, TIMER_RECENTFIGHT) > 0 )
  return TRUE;


 return FALSE;
}
bool  can_attack( CHAR_DATA *ch, CHAR_DATA *victim )
{
   /* Always like this, NPCs can be attacked, Specs cant */
   if ( IS_NPC(ch) && xIS_SET(ch->act, ACT_PACIFIST) )
   return FALSE;
   if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_PACIFIST) )
   return FALSE;

   if ( IS_NPC(ch) ||  IS_NPC(victim) )
   return TRUE;
   if ( xIS_SET(victim->act, PLR_SPAR) || xIS_SET(ch->act, PLR_SPAR))
   return TRUE;
   if ( xIS_SET(ch->act, PLR_SPEC) || xIS_SET(victim->act, PLR_SPEC) )
   return FALSE;
   /* Always being able to attack outlaw */
   if ( xIS_SET(victim->act, PLR_OUTLAW) )
   return TRUE;
   /* Always able to attack on Mortal Games */
   if ( IS_SET(victim->in_room->room_flags2, ROOM_KOTH) || IS_SET(victim->in_room->room_flags, ROOM_STADIUM) || IS_SET(victim->in_room->room_flags2, ROOM_FREEPK) || IS_SET(victim->in_room->room_flags2, ROOM_CTF) )
   return TRUE;
   /* Let them attack if they have dragonballs */
   if ( has_dragonball(victim) || has_dragonball2(victim) )
   return TRUE;
   /* Let them attack if they have the timer */
   if ( has_timer(victim) == TRUE )
   return TRUE;
   if ( IS_SET(victim->in_room->area->flags, AFLAG_PKILL) )
   return TRUE;
   if ( IS_SET(victim->in_room->room_flags2, ROOM_PKILL) )
   return TRUE;
   /* Dont let them attack on non-pkill zones */
   if ( IS_SET(victim->in_room->area->flags, AFLAG_NOPKILL) )
   return FALSE;
   if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) )
   return FALSE;


  return FALSE;
}
bool can_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
   /* Always like this, NPCs can be killed, Specs killed */
   if ( IS_NPC(ch) ||  IS_NPC(victim) )
   return TRUE;
   if ( xIS_SET(victim->act, PLR_OUTLAW) )
   return TRUE;
   if ( IS_SET(victim->in_room->room_flags2, ROOM_FREEPK))
   return TRUE;
   if ( xIS_SET(ch->act, PLR_SPEC) || xIS_SET(victim->act, PLR_SPEC) )
   return FALSE;
   if ( xIS_SET(victim->act, PLR_SPAR) || xIS_SET(ch->act, PLR_SPAR))
   return FALSE;
   /* Always being able to kill outlaw */
   /* Always able to kill on Mortal Games */
   if ( IS_SET(victim->in_room->room_flags2, ROOM_KOTH) || IS_SET(victim->in_room->room_flags, ROOM_STADIUM) || IS_SET(victim->in_room->room_flags2, ROOM_CTF))
   return FALSE;
   /* Let them kill if they have Black DBs */
   if ( has_dragonball2(victim) )
   return TRUE;
   if ( IS_SET(victim->in_room->area->flags, AFLAG_PKILL) )
   return TRUE;
   if ( IS_SET(victim->in_room->room_flags2, ROOM_PKILL) && victim->exp > 5000000)
   return TRUE;
   /* Dont let them kill if they have DBs */
   if ( has_dragonball(victim) )
   return FALSE;  
   /* Dont let them kill on non-pkill zones */
   if ( IS_SET(victim->in_room->area->flags, AFLAG_NOPKILL) )
   return FALSE;
   if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) )
   return FALSE;

  return FALSE;
}


void give_rage( CHAR_DATA *ch )
{
  if ( ch->rage >= 2000 )
  return;
  if (!IS_MONKEY(ch))
  return;
  if (IS_SET(ch->in_room->room_flags2, ROOM_SPAR))
  return;

  if (xIS_SET(ch->powerup, PLR_BOOST))
  ch->rage += number_range( 1, 2 );

  ch->rage++;

  if ( ch->rage > 250 && !xIS_SET(ch->act, PLR_SSJ1))
  {
    xREMOVE_BIT(ch->powerup, PU_SSJ2);
    xREMOVE_BIT(ch->powerup, PU_SSJ3);
    xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
    xREMOVE_BIT(ch->powerup, PU_MYSTIC);
    xREMOVE_BIT(ch->powerup, PU_OOZARU);
    xREMOVE_BIT(ch->powerup, PU_GOLDEN);
    xREMOVE_BIT(ch->powerup, PU_USSJ);
    xREMOVE_BIT(ch->powerup, PU_POWER1);
    xREMOVE_BIT(ch->powerup, PU_POWER2);
    xREMOVE_BIT(ch->powerup, PU_POWER3);
    xREMOVE_BIT(ch->powerup, PU_POWER4);
    xREMOVE_BIT(ch->powerup, PU_POWER5);
    xREMOVE_BIT(ch->powerup, PU_POWER6);
    xREMOVE_BIT(ch->powerup, PU_POWER7);
    xSET_BIT(ch->act, PLR_SSJ1);
    ch->mod_str = 0;
    ch->mod_int = 0;
    ch->mod_dex = 0;
    ch->mod_wis = 0;
    ch->mod_lck = 0;
    ch->mod_con = 0;
    ch->mod_cha = 0;
    do_ssj(ch, "1");
  }
  else if ( ch->rage > 450 && !xIS_SET(ch->act, PLR_SSJ2))
  {
    xREMOVE_BIT(ch->powerup, PU_SSJ1);
    xREMOVE_BIT(ch->powerup, PU_SSJ2);
    xREMOVE_BIT(ch->powerup, PU_SSJ3);
    xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
    xREMOVE_BIT(ch->powerup, PU_MYSTIC);
    xREMOVE_BIT(ch->powerup, PU_OOZARU);
    xREMOVE_BIT(ch->powerup, PU_GOLDEN);
    xREMOVE_BIT(ch->powerup, PU_USSJ);
    xREMOVE_BIT(ch->powerup, PU_POWER1);
    xREMOVE_BIT(ch->powerup, PU_POWER2);
    xREMOVE_BIT(ch->powerup, PU_POWER3);
    xREMOVE_BIT(ch->powerup, PU_POWER4);
    xREMOVE_BIT(ch->powerup, PU_POWER5);
    xREMOVE_BIT(ch->powerup, PU_POWER6);
    xREMOVE_BIT(ch->powerup, PU_POWER7);
    xSET_BIT(ch->act, PLR_SSJ2);
    ch->mod_str = 0; ch->mod_int = 0; ch->mod_dex = 0; ch->mod_wis = 0; ch->mod_lck = 0;
    ch->mod_con = 0; ch->mod_cha = 0;
    do_ssj(ch, "2");
  }
  else if ( ch->rage > 650 && !xIS_SET(ch->act, PLR_SSJ3))
  {
    xREMOVE_BIT(ch->powerup, PU_SSJ1);
    xREMOVE_BIT(ch->powerup, PU_SSJ2);
    xREMOVE_BIT(ch->powerup, PU_SSJ3);
    xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
    xREMOVE_BIT(ch->powerup, PU_MYSTIC);
    xREMOVE_BIT(ch->powerup, PU_OOZARU);
    xREMOVE_BIT(ch->powerup, PU_GOLDEN);
    xREMOVE_BIT(ch->powerup, PU_USSJ);
    xREMOVE_BIT(ch->powerup, PU_POWER1);
    xREMOVE_BIT(ch->powerup, PU_POWER2);
    xREMOVE_BIT(ch->powerup, PU_POWER3);
    xREMOVE_BIT(ch->powerup, PU_POWER4);
    xREMOVE_BIT(ch->powerup, PU_POWER5);
    xREMOVE_BIT(ch->powerup, PU_POWER6);
    xREMOVE_BIT(ch->powerup, PU_POWER7);
    xSET_BIT(ch->act, PLR_SSJ3);
    ch->mod_str = 0; ch->mod_int = 0; ch->mod_dex = 0; ch->mod_wis = 0; ch->mod_lck = 0;
    ch->mod_con = 0; ch->mod_cha = 0;
    do_ssj(ch, "3");
  }
  else if ( ch->rage == 1000 )
    send_to_char("Your energy raises as you feel the rage inside of you from all the blood lost.\n\r", ch);
  else if ( ch->rage == 1250 )
    send_to_char("You feel enraged from all the blood you have lost.\n\r", ch);
  else if ( ch->rage == 1500 )
    send_to_char("You feel energy crackling off your body from all the rage inside of you.\n\r", ch);
  else if ( ch->rage == 1750 )
    send_to_char("You feel your heart beating faster as the adrenaline enrages you.\n\r", ch);

}

void give_nd( CHAR_DATA *ch, CHAR_DATA *victim )
{
  char logbuf[MIL];
  int nd, pl, chance;
  chance = number_range( 0, 2 );
  if ( chance == 0 || chance == 1)
  {
   raw_kill( victim, ch );
   ch->ndcount++;
   return;
  }
  if ( ch->exp >= 95000000 )
  return;
  if (!IS_NPC(ch) && xIS_SET(ch->act, PLR_TOGGLE))
  return;
  if (IS_NPC(ch) && xIS_SET(ch->act, ACT_TOGGLED))
  return;
  if ( ch->hit != -4 )
  return;
  if (!IS_HALFBREED(ch) || !IS_SAIYAN(ch) || !IS_BIODROID(ch))
  return;
  if (IS_SET(ch->in_room->room_flags2, ROOM_SPAR))
  return;
  nd = ch->exp / 40;
  if (IS_BIODROID(ch))
  nd = ch->exp / 45;
  if ( ch->exp >= 5000000 && ch->exp < 25000000)  {pl = ch->pcdata->learned[gsn_chikone] * 250000;}
  else if ( ch->exp >= 25000000 && ch->exp < 55000000){pl = ch->pcdata->learned[gsn_chiktwo] * 550000; }
  else if ( ch->exp >= 55000000 && ch->exp < 75000000){pl = ch->pcdata->learned[gsn_chikthree] * 750000; }
  else if ( ch->exp >= 75000000 && ch->exp < 100000000){pl = ch->pcdata->learned[gsn_chikfour] * 1000000;}
  else if ( ch->exp >= 100000000 && ch->exp < 135000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}
  if (ch->exp >= 5000000)
  {
   if (  (pl+max_pl_char(ch)) < ch->exp )
   return;
  }
  ch->ndcount++;
  if (ch->ndcount > 5)
  return;
  ch->exp += nd;
  if ( is_clanned(ch) == FALSE )
  {
     if ( (nd + ch->exp) > 100000000 )
     {
        ch->exp = 100010000;
        return;
     }
  }
  else
  {
     if ( (nd + ch->exp) > 135000000 )
     {
        ch->exp = 135010000;
        return;
     }
  }

  ch_printf(ch, "You feel stronger due to your near death experience.\n\r");
  sprintf(logbuf, "%s has achieved a near death.\n\r", ch->name );
  log_string( logbuf );
  give_rage( ch );

}


/*
 * Check to see if player's attacks are (still?) suppressed
 * #ifdef TRI
 */
bool is_attack_supressed( CHAR_DATA *ch )
{
  TIMER *timer;

  if (IS_NPC(ch))
    return FALSE;

  timer = get_timerptr( ch, TIMER_ASUPRESSED );

  if ( !timer )
    return FALSE;

  /* perma-supression -- bard? (can be reset at end of fight, or spell, etc) */
  if ( timer->value == -1 )
    return TRUE;

  /* this is for timed supressions */
  if ( timer->count >= 1 )
    return TRUE;

  return FALSE;
}
  
/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( !used_weapon )
    	return FALSE;

    if ( (obj=get_eq_char(ch, WEAR_WIELD)) != NULL
    &&    used_weapon == obj
    &&    IS_OBJ_STAT(obj, ITEM_POISONED) )
	return TRUE;

    return FALSE;
}

/*
 * hunting, hating and fearing code				-Thoric
 */
bool is_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hunting || ch->hunting->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hating || ch->hating->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->fearing || ch->fearing->who != victim )
      return FALSE;
    
    return TRUE;    
}

void stop_hunting( CHAR_DATA *ch )
{
    if ( ch->hunting )
    {
	STRFREE( ch->hunting->name );
	DISPOSE( ch->hunting );
	ch->hunting = NULL;
    }
    return;
}

void stop_hating( CHAR_DATA *ch )
{
    if ( ch->hating )
    {
	STRFREE( ch->hating->name );
	DISPOSE( ch->hating );
	ch->hating = NULL;
    }
    return;
}

void stop_fearing( CHAR_DATA *ch )
{
    if ( ch->fearing )
    {
	STRFREE( ch->fearing->name );
	DISPOSE( ch->fearing );
	ch->fearing = NULL;
    }
    return;
}

void start_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hunting )
      stop_hunting( ch );

    CREATE( ch->hunting, HHF_DATA, 1 );
    ch->hunting->name = QUICKLINK( victim->name );
    ch->hunting->who  = victim;
    return;
}

void start_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hating )
      stop_hating( ch );

    CREATE( ch->hating, HHF_DATA, 1 );
    ch->hating->name = QUICKLINK( victim->name );
    ch->hating->who  = victim;
    return;
}

void start_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fearing )
      stop_fearing( ch );

    CREATE( ch->fearing, HHF_DATA, 1 );
    ch->fearing->name = QUICKLINK( victim->name );
    ch->fearing->who  = victim;
    return;
}

/*
 * Get the current armor class for a vampire based on time of day
 */
sh_int VAMP_AC( CHAR_DATA * ch )
{
  if ( IS_VAMPIRE( ch ) && IS_OUTSIDE( ch ) )
  {
    switch(time_info.sunlight)
    {
    case SUN_DARK:
      return -10;
    case SUN_RISE:
      return 5;
    case SUN_LIGHT:
      return 10;
    case SUN_SET:
      return 2;
    default:
      return 0;
    }
  }
  else
    return 0;
}

int max_fight( CHAR_DATA *ch )
{
    return 8;
}



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Many hours spent fixing bugs in here by Thoric, as noted by residual
 * debugging checks.  If you never get any of these error messages again
 * in your logs... then you can comment out some of the checks without
 * worry.
 *
 * Note:  This function also handles some non-violence updates.
 */
void violence_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *lst_ch;
    CHAR_DATA *victim;
    CHAR_DATA *rch, *rch_next;
    TIMER	*timer, *timer_next;
    ch_ret	retcode;
    int		attacktype, cnt;
    static int	pulse = 0;

    lst_ch = NULL;
    pulse = (pulse+1) % 100;

    for ( ch = last_char; ch; lst_ch = ch, ch = gch_prev )
    {
	set_cur_char( ch );

	if ( ch == first_char && ch->prev )
	{
	   bug( "ERROR: first_char->prev != NULL, fixing...", 0 );
	   ch->prev = NULL;
	}

	gch_prev	= ch->prev;

	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: violence_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    ch->prev = NULL;
	    gch_prev = NULL;
	    do_shout( ch, "Thoric says, 'Prepare for the worst!'" );
	}

	/*
	 * See if we got a pointer to someone who recently died...
	 * if so, either the pointer is bad... or it's a player who
	 * "died", and is back at the healer...
	 * Since he/she's in the char_list, it's likely to be the later...
	 * and should not already be in another fight already
	 */
	if ( char_died(ch) )
	    continue;

	/*
	 * See if we got a pointer to some bad looking data...
	 */
	if ( !ch->in_room || !ch->name )
	{
	    log_string( "violence_update: bad ch record!  (Shortcutting.)" );
	    sprintf( buf, "ch: %d  ch->in_room: %d  ch->prev: %d  ch->next: %d",
	    	(int) ch, (int) ch->in_room, (int) ch->prev, (int) ch->next );
	    log_string( buf );
	    log_string( lastplayercmd );
	    if ( lst_ch )
	      sprintf( buf, "lst_ch: %d  lst_ch->prev: %d  lst_ch->next: %d",
	      		(int) lst_ch, (int) lst_ch->prev, (int) lst_ch->next );
	    else
	      strcpy( buf, "lst_ch: NULL" );
	    log_string( buf );
	    gch_prev = NULL;
	    continue;
	}

        /*
         * Experience gained during battle deceases as battle drags on
         */
	if ( ch->fighting )
	  if ( (++ch->fighting->duration % 24) == 0 )
	    ch->fighting->xp = ((ch->fighting->xp * 9) / 10);



	for ( timer = ch->first_timer; timer; timer = timer_next )
	{
	    timer_next = timer->next;
	    if ( --timer->count <= 0 )
	    {
                if ( timer->type == TIMER_ASUPRESSED )
                {
                    if ( timer->value == -1 )
                    {
                        timer->count = 1000;
                        continue;
                    }
                }
		if ( timer->type == TIMER_DO_FUN )
		{
		    int tempsub;
		    // Slight fix to timers - Gatz
		    DO_FUN *fun;
		
		    tempsub = ch->substate;
		    fun = timer->do_fun;
		    ch->substate = timer->value;
		    extract_timer( ch, timer );
		    // Seems redundent but, why not?
		    timer = NULL;
		    (fun)( ch, "" );
		    if ( char_died(ch) )
		      break;
		    ch->substate = tempsub;
		}
		if(timer)
			extract_timer( ch, timer );
	    }
	}
/*

	for ( timer = ch->first_timer; timer; timer = timer_next )
	{
	    timer_next = timer->next;
	    if ( --timer->count <= 0 )
	    {
		if ( timer->type == TIMER_ASUPRESSED )
		{
		    if ( timer->value == -1 )
		    {
			timer->count = 1000;
			continue;
		    }
		}
		if ( timer->type == TIMER_DO_FUN )
		{
		    int tempsub;

		    tempsub = ch->substate;
		    ch->substate = timer->value;
		    (timer->do_fun)( ch, "" );
		    if ( char_died(ch) )
			break;
		    ch->substate = tempsub;
		}
		extract_timer( ch, timer );
	    }
	}
*/
	if ( char_died(ch) )
	    continue;

	
	if ( char_died(ch) )
	    continue;

	/* check for exits moving players around */
	if ( (retcode=pullcheck(ch, pulse)) == rCHAR_DIED || char_died(ch) )
	    continue;

	/* Let the battle begin! */

	if ( ( victim = who_fighting( ch ) ) == NULL)
	    continue;

        retcode = rNONE;

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE ) )
	{
	   sprintf( buf, "violence_update: %s fighting %s in a SAFE room.",
	   	ch->name, victim->name );
	   log_string( buf );
	   stop_fighting( ch, TRUE );
	}
	else
	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( char_died(ch) )
	    continue;

	if ( retcode == rCHAR_DIED
	|| ( victim = who_fighting( ch ) ) == NULL )
	    continue;

	/*
	 *  Mob triggers
	 *  -- Added some victim death checks, because it IS possible.. -- Alty
	 */
	rprog_rfight_trigger( ch );
	if ( char_died(ch) || char_died(victim) )
	    continue;
	mprog_hitprcnt_trigger( ch, victim );
	if ( char_died(ch) || char_died(victim) )
	    continue;
	mprog_fight_trigger( ch, victim );
	if ( char_died(ch) || char_died(victim) )
	    continue;

	/*
	 * NPC special attack flags				-Thoric
	 */
	if ( IS_NPC(ch) ) 
	{
	    if ( !xIS_EMPTY(ch->attacks) )
	    {
		attacktype = -1;
		if ( 30 + (ch->level/4) >= number_percent( ) )
		{
		    cnt = 0;
		    for ( ;; )
		    {
			if ( cnt++ > 10 )
			{
			    attacktype = -1;
			    break;
			}
			attacktype = number_range( 7, MAX_ATTACK_TYPE-1 );
			if ( xIS_SET( ch->attacks, attacktype ) )
			    break;
		    }
		    if ( attacktype != -1 && (retcode == rCHAR_DIED || char_died(ch)) )
			continue;
		}
	    }
	    /*
	     * NPC special defense flags				-Thoric
	     */
	    if ( !xIS_EMPTY(ch->defenses) )
	    {
		attacktype = -1;
		if ( 50 + (ch->level/4) > number_percent( ) )
		{
		    cnt = 0;
		    for ( ;; )
		    {
			if ( cnt++ > 10 )
			{
			    attacktype = -1;
			    break;
			}
			attacktype = number_range( 2, MAX_DEFENSE_TYPE-1 );
			if ( xIS_SET( ch->defenses, attacktype ) )
			    break;
		    }

		    if ( attacktype != -1 && (retcode == rCHAR_DIED || char_died(ch)) )
			continue;
		}
	    }
	}

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->first_person; rch; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

            /*
             *   Group Fighting Styles Support:
             *   If ch is tanking
             *   If rch is using a more aggressive style than ch
             *   Then rch is the new tank   -h
             */
            /* &&( is_same_group(ch, rch)      ) */

            if( ( !IS_NPC(ch) && !IS_NPC(rch) )
              &&( rch!=ch                     )
              &&( rch->fighting               )
              &&( who_fighting(rch->fighting->who) == ch    )
              &&( !xIS_SET( rch->fighting->who->act, ACT_AUTONOMOUS ) )
              &&( rch->style < ch->style      )
              )
            {
                 rch->fighting->who->fighting->who = rch; 

            }

	    if ( IS_AWAKE(rch) && !rch->fighting )
	    {
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch)  )
		{
		    if ( ( ( !IS_NPC(rch) && rch->desc ))
		    && is_same_group(ch, rch)
		    && !is_safe( rch, victim, TRUE) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch)
		&&  !xIS_SET(rch->act, ACT_NOASSIST) )
		{
		    if ( char_died(ch) )
			break;
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;			
                        for ( vch = ch->in_room->first_person; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch, FALSE )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				if ( vch->mount && vch->mount == rch )
				  target = NULL;
				else
				{
				  target = vch;
				  number++;
				}
			    }
			}

			if ( target )
			    multi_hit( rch, target, TYPE_UNDEFINED );
		    }
		}
	    }
	}
    }

    return;
}



/*
 * Do one group of attacks.
 */
ch_ret multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;
    ch_ret  retcode;

    /* add timer to pkillers */
    if ( !IS_NPC(ch) && !IS_NPC(victim) && !xIS_SET(ch->act, PLR_SPAR) && !xIS_SET(victim->act, PLR_SPAR) && !in_arena(ch) && !in_arena(victim))
    {
        add_timer( ch,     TIMER_RECENTFIGHT, 40, NULL, 0 );
        add_timer( victim, TIMER_RECENTFIGHT, 40, NULL, 0 );
    }

    if ( is_attack_supressed( ch ) )
      return rNONE;

    if ( IS_NPC(ch) && xIS_SET(ch->act, ACT_NOATTACK) )
      return rNONE;

    if ( (retcode = one_hit( ch, victim, dt )) != rNONE )
      return retcode;

    if ( who_fighting( ch ) != victim )
	return rNONE;
	
    /* Very high chance of hitting compared to chance of going berserk */
    /* 40% or higher is always hit.. don't learn anything here though. */
    /* -- Altrag */
    chance = IS_NPC(ch) ? 100 : 0;



    /*
     * NPC predetermined number of attacks			-Thoric
     */
    if ( IS_NPC(ch) && ch->numattacks > 0 )
    {
	for ( chance = 0; chance < ch->numattacks; chance++ )
	{
	   retcode = one_hit( ch, victim, dt );
	   if ( retcode != rNONE || who_fighting( ch ) != victim )
	     return retcode;
	}
	return retcode;
    }

    chance = IS_NPC(ch) ? ch->level
	   : (int) ((LEARNED(ch, gsn_second_attack)));
    if ( number_percent( ) < chance )
    {
	learn_from_success( ch, gsn_second_attack );
	retcode = one_hit( ch, victim, dt );
	if ( retcode != rNONE || who_fighting( ch ) != victim )
	    return retcode;
    }
    else
	learn_from_failure( ch, gsn_second_attack );

    retcode = rNONE;

    chance = IS_NPC(ch) ? (int) (ch->level / 2) : 0;
    if ( number_percent( ) < chance )
	retcode = one_hit( ch, victim, dt );

    return retcode;
}


/*
 * Weapon types, haus
 */
int weapon_prof_bonus_check( CHAR_DATA *ch, OBJ_DATA *wield, int *gsn_ptr )
{
    int bonus;

    bonus = 0;	*gsn_ptr = -1;
    if ( !IS_NPC(ch) && ch->level > 5 && wield )   
    {
	switch(wield->value[3])
	{
	   default:		*gsn_ptr = -1;			break;
           case DAM_HIT:
	   case DAM_SUCTION:
	   case DAM_BITE:
	   case DAM_BLAST:					break;
           case DAM_SLASH:
           case DAM_SLICE:					break;
           case DAM_PIERCE:
           case DAM_STAB:					break;
           case DAM_WHIP:					break;
           case DAM_CLAW:					break;
           case DAM_POUND:
           case DAM_CRUSH:					break;
	   case DAM_BOLT:
	   case DAM_ARROW:
	   case DAM_DART:
	   case DAM_STONE:
	   case DAM_PEA:					break;

	}
	if ( *gsn_ptr != -1 )
	  bonus = (int) ((LEARNED(ch, *gsn_ptr) -50)/10);

       /* Reduce weapon bonuses for misaligned clannies.
       if ( IS_CLANNED(ch) )
       {
          bonus = bonus * 1000 / 
          ( 1 + abs( ch->alignment - ch->pcdata->clan->alignment ) );
       }*/


    }
    return bonus;
}

/*
 * Calculate the tohit bonus on the object and return RIS values.
 * -- Altrag
 */
int obj_hitroll( OBJ_DATA *obj )
{
	int tohit = 0;
	
	return tohit;
}

/*
 * Offensive shield level modifier
 */
sh_int off_shld_lvl( CHAR_DATA *ch, CHAR_DATA *victim )
{
    sh_int lvl;

    if ( !IS_NPC(ch) )          /* players get much less effect */
    {
        lvl = UMAX( 1, (ch->level - 10) / 2 );
        if ( number_percent() + (victim->level - lvl) < 40 )
	{
	  if ( CAN_PKILL( ch ) && CAN_PKILL( victim ) )
	    return ch->level;
	  else
	    return lvl;
	}
        else
          return 0;
    }
    else
    {
	lvl = ch->level / 2;
	if ( number_percent() + (victim->level - lvl) < 70 )
	  return lvl;
	else
	  return 0;
    }
}

/*
 * Hit one guy once.
 */
ch_ret one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int dam=0;
    int attacktype, cnt;
    int chaextra;
    int bardown;
    int extra;
    int strdown;
    int condown;
    int lckup;
    int chance2;
    int chance;
    int armor;
    int dodge;
    int anti;
 
    ch_ret retcode = rNONE;

    chance = number_range(0, 4);
    chance2 = number_range(0, 4);
    chaextra = extra = bardown = dodge = anti = armor = 0;
      if ( victim->style == STYLE_DEFENSIVE && victim->mana > 0)
      {
       victim->mana -= (victim->max_mana / 66);
       act( AT_GREEN, "You block $N's attack." , victim, NULL, ch, TO_CHAR );
       act( AT_GREEN, "$N blocks your attack.", ch, NULL, victim, TO_CHAR );
       act( AT_GREEN, "$N blocks $n's attack.", ch, NULL, victim, TO_NOTVICT );
      }
      if (victim->mana <= 0 && !IS_NPC(victim))
      {
         act( AT_GREY, "$n falls unconsious for the loss of ki.", victim, NULL, NULL, TO_ROOM );
         act( AT_GREY, "You fall unconsious for the loss of ki.", victim, NULL, NULL, TO_CHAR );
         stop_fighting(victim, TRUE);
         victim->hit = -1; 
         update_pos( victim );
         victim->mana = 1;
         if (IS_NPC(ch))
         {
            stop_hating( ch );
            stop_hunting( ch );
            stop_fearing( ch );
         }
         release_ki( ch );
      }
      strdown  = (get_curr_str(victim) - get_curr_str(ch) )/ 4;
      condown  = (get_curr_con(victim) - get_curr_con(ch) )/ 4;
      lckup =    (get_curr_critical(ch) - get_curr_critical(victim));
    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;

    used_weapon = NULL;
    /*
     * Figure out the weapon doing the damage			-Thoric
     * Dual wield support -- switch weapons each attack
     */
	wield = get_eq_char( ch, WEAR_WIELD );

    used_weapon = wield;

    if ( ch->fighting		/* make sure fight is already started */
    &&   dt == TYPE_UNDEFINED
    &&   IS_NPC(ch)
    &&  !xIS_EMPTY(ch->attacks) )
    {
	cnt = 0;
	for ( ;; )
	{
	    attacktype = number_range( 0, 6 );
	    if ( xIS_SET( ch->attacks, attacktype ) )
		break;
	    if ( cnt++ > 16 )
 	    {
		attacktype = -1;
		break;
	    }
	}
    }

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[0] + 1;
    }

    if ( dt == TYPE_SLAM) { dt = TYPE_SLAM; lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_BURN) { dt = TYPE_BURN;lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_CUT)  { dt = TYPE_CUT; lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_CUTDDD)  { dt = TYPE_CUTDDD; lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_KICK) { dt = TYPE_KICK;}
    if ( dt == TYPE_DK)    { dt = TYPE_DK;}
    if ( dt == TYPE_DP)    { dt = TYPE_DP;}
    if ( dt == TYPE_TAIL)  { dt = TYPE_TAIL; }
    if ( dt == TYPE_FINGER){ dt = TYPE_FINGER;}
    if ( dt == TYPE_EYE){ dt = TYPE_EYE;}
    if ( dt == TYPE_MASENKO){ dt = TYPE_MASENKO; lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_NGALIC ){ dt = TYPE_NGALIC;lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_SBC )   { dt = TYPE_SBC; lckup = 0; strdown = 0; condown = 0;   }
    if ( dt == TYPE_KAME )  { dt = TYPE_KAME;  lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_FINALFLASH )  { dt = TYPE_FINALFLASH;  lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_DRAGONFIST )  { dt = TYPE_DRAGONFIST;  lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_HELLSFLASH )  { dt = TYPE_HELLSFLASH;  lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_ROCKETPUNCH )  { dt = TYPE_ROCKETPUNCH;  lckup = 0; strdown = 0; condown = 0; }
    if ( dt == TYPE_DSLAM ) { dt = TYPE_DSLAM;lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_NBLAST ){ dt = TYPE_NBLAST;lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_MENTAL) { dt = TYPE_MENTAL; lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_TRIBEAM){ dt = TYPE_TRIBEAM;lckup = 0; strdown = 0; condown = 0;}
    if ( dt == TYPE_NPUNCH) { dt = TYPE_NPUNCH; }
    if ( dt == TYPE_CLAW)   { dt = TYPE_CLAW; }
    if ( dt == TYPE_SCATTERSHOT)   { dt = TYPE_SCATTERSHOT;lckup = 0; strdown = 0; condown = 0;}

    dam += sqrt( get_curr_patk( ch ) ) / 2;
    dam -= sqrt( get_curr_pdef( victim ) ) / 3;
    dam += number_range( -1, 1 ); /*Spice it up*/
    /*
     * Calculate Damage Modifiers from Victim's Fighting Style
     */
    if ( victim->style == STYLE_DEFENSIVE )
       dam = 0.80 * dam;
    else if ( victim->style == STYLE_AGGRESSIVE )
       dam = 1.2 * dam;
    /*
     * Calculate Damage Modifiers from Attacker's Fighting Style
     */
    if ( ch->style == STYLE_DEFENSIVE )
       dam = 0.80 * dam;
    else if ( ch->style == STYLE_AGGRESSIVE )
       dam = 1.2 * dam;
    if ( IS_NPC(ch))
     dam *= 1.5;
/* Cha Damage */
    if (ch->leader && (ch->in_room == ch->leader->in_room))
    {
      if (victim->leader && get_curr_cha(victim->leader) > get_curr_cha(victim))
      chaextra = get_curr_cha(ch->leader) - get_curr_cha(victim->leader);
      else
      chaextra = get_curr_cha(ch->leader) - get_curr_cha(victim);
    } 
    dam += chaextra / 3;
    if ( xIS_SET(victim->act, PLR_SK))
       dam *= 0.8;
    if ( xIS_SET(victim->act, PLR_MS))
       dam *= 0.95;
    if ( xIS_SET(ch->act, PLR_FUSED))
       dam *=  1.2;


   if ( xIS_SET(victim->powerup, KI_BAR))
   dam *= calc_barrier(victim);
   if ( xIS_SET(victim->powerup, KI_BAR) || xIS_SET(victim->powerup, KI_BARRICADE) || xIS_SET(victim->powerup, KI_SHIELD))
   {
       victim->barrier -= number_range(100, 5000);
       if ( victim->barrier <= 0 )
       {
	 release_ki( ch );
         if (xIS_SET(victim->powerup, KI_SHIELD))
          send_to_char("&RYour energy shield breaks as it dissolves into nothing.\n\r", victim);
         if (xIS_SET(victim->powerup, KI_BAR))
          send_to_char("&RYour energy barrier breaks as it dissolves into nothing.\n\r", victim);
         if (xIS_SET(victim->powerup, KI_BARRICADE))
          send_to_char("&RYour energy barricade breaks as it dissolves into nothing.\n\r", victim);
       }
   } 
   if (get_curr_pl(ch) > get_curr_pl(victim))
   {
    extra = get_curr_pl(ch) / get_curr_pl(victim);
    dam += extra;
   }
   else if (get_curr_pl(ch) < get_curr_pl(victim))
   {
    extra = get_curr_pl(ch) / get_curr_pl(victim);
    dam -= extra;
   }

    if (chance2 == 0 )
    {
        dodge  = (get_curr_accuracy(victim) - get_curr_evasion(ch) );
     if ( number_percent() < dodge )
     {
       if ( !IS_NPC(victim) && !xIS_SET( victim->pcdata->flags, PCFLAG_SUPER)  )
       act( AT_SKILL, "You dodge $n's attack.", ch, NULL, victim, TO_VICT);
       if ( !IS_NPC(ch) && !xIS_SET( ch->pcdata->flags, PCFLAG_SUPER) )
       act( AT_SKILL, "$N dodges your attack.", ch, NULL, victim, TO_CHAR);
       dam = 0;
       damage( ch, victim, dam, dt );
       return rNONE;
     }
    }
    if (chance2 == 1 )
    {
     anti  = (get_curr_int(victim) - get_curr_int(ch) );
     if ( number_percent() < anti)
     {
       if ( !IS_NPC(victim) && !xIS_SET( victim->pcdata->flags, PCFLAG_SUPER)  )
       act( AT_SKILL, "You anticipate $n's attack.", ch, NULL, victim, TO_VICT);
       if ( !IS_NPC(ch) && !xIS_SET( ch->pcdata->flags, PCFLAG_SUPER) )
       act( AT_SKILL, "$N anticipates your attack.", ch, NULL, victim, TO_CHAR);
       dam = 0;
       damage( ch, victim, dam, dt );
       return rNONE;
     }
    }
    if ( chance == 0 && strdown >= 1 )
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "You grin as $N strikes you." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "$N grins as you strike $m.", ch, NULL, victim, TO_CHAR );
       act( AT_GREEN, "$N grins as $n strikes $M.", ch, NULL, victim, TO_NOTVICT );
       dam *= 0.75;
    }
    if ( chance == 1 && condown >= 1)
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "You do not get much damage because of your well built body." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "$N does not get much damage because of $s well built body.", ch, NULL, victim, TO_CHAR );
       act( AT_GREEN, "$N does not get much damage because of $s well built body.", ch, NULL, victim, TO_NOTVICT );
       dam *= 0.75;
    }
    if ( (chance == 2) && lckup > number_percent() )
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "$N hits you in the right place." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_GREEN, "You hit $N in the right place.", ch, NULL, victim, TO_CHAR );
       act( AT_GREEN, "$n hits $N in the right place.", ch, NULL, victim, TO_NOTVICT );
       dam *= 1.2;
    }
    if ( xIS_SET(victim->powerup, KI_BAR))
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "$N's attack damages your Energy Barrier." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "Your attack damages $N's Energy Barrier.", ch, NULL, victim, TO_CHAR );
       act( AT_RED, "$n's attack damages $N's Energy Barrier.", ch, NULL, victim, TO_NOTVICT );
    }
    else if ( xIS_SET(victim->powerup, KI_BARRICADE))
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "$N's attack damages your Energy Barricade." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "Your attack damages $N's Energy Barricade.", ch, NULL, victim, TO_CHAR );
       act( AT_RED, "$n's attack damages $N's Energy Barricade.", ch, NULL, victim, TO_NOTVICT );
    }
    else if ( xIS_SET(victim->powerup, KI_SHIELD))
    {
       if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "$N's attack damages your Energy Shield." , victim, NULL, ch, TO_CHAR );
       if (!IS_NPC(ch) && (!xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
       act( AT_RED, "Your attack damages $N's Energy Shield.", ch, NULL, victim, TO_CHAR );
       act( AT_RED, "$n's attack damages $N's Energy Shield.", ch, NULL, victim, TO_NOTVICT );
    }



    if ( (retcode = damage( ch, victim, dam, dt )) != rNONE )
	return retcode;
    if ( char_died(ch) )
	return rCHAR_DIED;
    if ( char_died(victim) )
	return rVICT_DIED;

    retcode = rNONE;

    tail_chain( );
    return retcode;
}

/*
 * Hit one guy with a projectile.
 * Handles use of missile weapons (wield = missile weapon)
 * or thrown items/weapons
 */
ch_ret projectile_hit( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield,
		       OBJ_DATA *projectile, sh_int dist )
{
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int	prof_bonus;
    int	prof_gsn = -1;
    int proj_bonus;
    int dt;
    ch_ret retcode;

    if ( !projectile )
	return rNONE;

    if ( projectile->item_type == ITEM_PROJECTILE 
    ||   projectile->item_type == ITEM_WEAPON )
    {
	dt = TYPE_HIT + projectile->value[3];
	proj_bonus = number_range(projectile->value[1], projectile->value[2]);
    }
    else
    {
	dt = TYPE_UNDEFINED;
	proj_bonus = number_range(1, URANGE(2, get_obj_weight(projectile), 100) );
    }

    /*
     * Can't beat a dead char!
     */
    if ( victim->position == POS_DEAD || char_died(victim) )
    {
	extract_obj(projectile);
	return rVICT_DIED;
    }

    if ( wield )
	prof_bonus = weapon_prof_bonus_check( ch, wield, &prof_gsn );
    else
	prof_bonus = 0;

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_MISSILE_WEAPON )
	    dt += wield->value[3];
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = ch->mobthac0;
	thac0_32 =  0;
    }
    else
    {
	thac0_00 = class_table[ch->class]->thac0_00;
	thac0_32 = class_table[ch->class]->thac0_32;
    }
    thac0     = interpolate( ch->level, thac0_00, thac0_32 ) - GET_HITROLL(ch) + (dist*2);
    victim_ac = UMAX( -19, (int) (GET_AC(victim) / 10) );

    /* if you can't see what's coming... */
    if ( !can_see_obj( victim, projectile) )
	victim_ac += 1;
    if ( !can_see( ch, victim, FALSE ) )
	victim_ac -= 4;

    /* Weapon proficiency bonus */
    victim_ac += prof_bonus;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	if ( prof_gsn != -1 )
	    learn_from_failure( ch, prof_gsn );

	/* Do something with the projectile */
	if ( number_percent() < 50 )
	    extract_obj(projectile);
	else
	{
	    if ( projectile->in_obj )
		obj_from_obj(projectile);
	    if ( projectile->carried_by )
		obj_from_char(projectile);
	    obj_to_room(projectile, victim->in_room, victim);
	}
	damage( ch, victim, 0, dt );
	tail_chain( );
	return rNONE;
    }

    /*
     * Hit.
     * Calc damage.
     */

    if ( !wield )
	dam = GET_HITROLL(ch);
    else
	dam = wield->value[2] / wield->value[1];

    /*
     * Bonuses.
     */
//    dam += GET_DAMROLL(ch);

/*    if ( prof_bonus )
	dam += prof_bonus / 4;*/

    /*
     * Calculate Damage Modifiers from Victim's Fighting Style
     */
    if ( victim->style == STYLE_AGGRESSIVE )
       dam = 1.1 * dam;
    else if ( victim->style == STYLE_DEFENSIVE )
       dam = .85 * dam;


    if ( !IS_AWAKE(victim) )
	dam *= 2;

    if ( dam <= 0 )
	dam = 1;


    if ( prof_gsn != -1 )
    {
      if ( dam > 0 )
        learn_from_success( ch, prof_gsn );
      else
        learn_from_failure( ch, prof_gsn );
    }

    /* immune to damage */
    if ( dam == -1 )
    {
	if ( dt >= 0 && dt < top_sskill )
	{
	    SKILLTYPE *skill = sskill_table[dt];
	    bool found = FALSE;

	    if ( skill->imm_char && skill->imm_char[0] != '\0' )
	    {
		act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	    {
		act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
		found = TRUE;
	    }
	    if ( skill->imm_room && skill->imm_room[0] != '\0' )
	    {
		act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
		found = TRUE;
	    }
	    if ( found )
	    {
		if ( number_percent() < 50 )
		    extract_obj(projectile);
		else
		{
		    if ( projectile->in_obj )
			obj_from_obj(projectile);
		    if ( projectile->carried_by )
			obj_from_char(projectile);
		    obj_to_room(projectile, victim->in_room, victim);
		}
		return rNONE;
	    }
	}
	dam = 0;
    }
    if ( (retcode = damage( ch, victim, dam, dt )) != rNONE )
    {
	extract_obj(projectile);
	return retcode;
    }
    if ( char_died(ch) )
    {
	extract_obj(projectile);
	return rCHAR_DIED;
    }
    if ( char_died(victim) )
    {
	extract_obj(projectile);
	return rVICT_DIED;
    }

    retcode = rNONE;
    if ( dam == 0 )
    {
	if ( number_percent() < 50 )
	    extract_obj(projectile);
	else
	{
	    if ( projectile->in_obj )
		obj_from_obj(projectile);
	    if ( projectile->carried_by )
		obj_from_char(projectile);
	    obj_to_room(projectile, victim->in_room, victim);
	}
	return retcode;
    }

    extract_obj(projectile);

    tail_chain( );
    return retcode;
}


/*
 * Changed is_safe to have the show_messg boolian.  This is so if you don't
 * want to show why you can't kill someone you can't turn it off.  This is
 * useful for things like area attacks.  --Shaddai
 */
bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim, bool show_messg )
{
    if ( char_died(victim) || char_died(ch) )
    	return TRUE;

    /* Thx Josh! */
    if ( who_fighting( ch ) == ch )
	return FALSE;

    if ( !victim ) /*Gonna find this is_safe crash bug -Blod*/
    {
        bug( "Is_safe: %s opponent does not exist!", ch->name );
        return TRUE;
    }
    
    if ( get_timer( victim, TIMER_PROPAGANDA > 0))  
    return FALSE;
    
    if ( !victim->in_room )
    {
	bug( "Is_safe: %s has no physical location!", victim->name );
	return TRUE;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
        if ( show_messg ) {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "A magical force prevents you from attacking.\n\r", ch );
	}
	return TRUE;
    }
    
    if(IS_PACIFIST(ch)) /* Fireblade */
    {
        if ( show_messg ) {
    	set_char_color(AT_MAGIC, ch);
    	ch_printf(ch, "You are a pacifist and will not fight.\n\r");
	}
	return TRUE;
    }

    if ( IS_PACIFIST(victim) ) /* Gorog */
    {
        char buf[MAX_STRING_LENGTH];
        if ( show_messg ) {
        sprintf(buf, "%s is a pacifist and will not fight.\n\r",
                capitalize(victim->short_descr));
        set_char_color( AT_MAGIC, ch );
        send_to_char( buf, ch);
	}
        return TRUE;
    }

    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
        return FALSE;

    if( !IS_NPC( ch ) && !IS_NPC( victim )
    &&   ch != victim 
    &&   IS_SET( victim->in_room->area->flags, AFLAG_NOPKILL ) && !xIS_SET(ch->act, PLR_SPAR) && !xIS_SET(victim->act, PLR_SPAR) && !xIS_SET(victim->act, PLR_OUTLAW) && !xIS_SET(ch->act, PLR_OUTLAW) && get_timer(victim, TIMER_PROPAGANDA) <= 0) 
    {
        if (has_dragonball(victim) || has_dragonball(ch))
        return FALSE;
        if (has_dragonball2(victim) || has_dragonball2(ch))
        return FALSE;

        if ( show_messg ) {
        set_char_color( AT_IMMORT, ch );
        send_to_char( "The gods have forbidden player killing in this area.\n\r", ch );
	}
        return TRUE;
    }
    if (IS_NPC(ch) || IS_NPC(victim))
    return FALSE;

    if (get_timer(victim, TIMER_PROPAGANDA) > 0)
    return FALSE;
    if (IS_SET(victim->in_room->area->flags, AFLAG_NOPKILL) && !IS_SET(victim->in_room->area->flags, AFLAG_PLANET))
    return TRUE;
    if (IS_SET(victim->in_room->area->flags, AFLAG_NOPKILL) && IS_SET(victim->in_room->area->flags, AFLAG_PLANET) && get_timer(victim, TIMER_PROPAGANDA) > 0)
    return FALSE;
   
    if ( get_timer(victim, TIMER_PKILLED) > 0 )
    {
        if ( show_messg ) {
	set_char_color( AT_GREEN, ch );
        send_to_char( "That character has died within the last 5 minutes.\n\r", ch);
	}
        return TRUE;
    }
  
    if ( get_timer(ch, TIMER_PKILLED) > 0 )
    {
        if ( show_messg ) {
	set_char_color( AT_GREEN, ch );
        send_to_char( "You have been killed within the last 5 minutes.\n\r", ch );
	}
        return TRUE;
    }

    return FALSE;
}

/*
 * just verify that a corpse looting is legal
 */
bool legal_loot( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /* anyone can loot mobs */
    if ( IS_NPC(victim) )
      return TRUE;
    /* non-charmed mobs can loot anything */
    if ( IS_NPC(ch) && !ch->master )
      return TRUE;
    /* members of different clans can loot too! -Thoric */
    if ( !IS_NPC(ch) && !IS_NPC(victim)
    &&    xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) 
    &&    xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) ) 
	return TRUE;
    return FALSE;
}

/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * NPC's are fair game.
     */
    if ( IS_NPC(victim) )
    {
	if ( !IS_NPC( ch ) )
	{
	  if ( ch->pcdata->clan )
	    ch->pcdata->clan->mkills++;
	  ch->pcdata->mkills++;
	  ch->in_room->area->mkills++;
	}
	return;
    }

    /*
     * If you kill yourself nothing happens.
     */

    if ( ch == victim || ch->level >= LEVEL_IMMORTAL )
      return;

    /*
     * Any character in the arena is ok to kill.
     * Added pdeath and pkills here
     */
    
    if ( !IS_NPC( ch ) )
    {
   	if ( !IS_NPC(ch) && !IS_NPC(victim) )
	{
	  ch->pcdata->pkills++;
	  victim->pcdata->pdeaths++;
	}
  
    }
    if ( IS_SET(ch->in_room->room_flags2, ROOM_FREEPK) || IS_SET(ch->in_room->room_flags2, ROOM_FREEPK) )
    return;
    /* clan checks					-Thoric */
    if ( !IS_NPC(ch) && !IS_NPC(victim)
    &&    xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY )
    &&    xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
    {
      /* not of same clan? Go ahead and kill!!! */
      if ( ch->pcdata->clan != victim->pcdata->clan )
      {
	if ( ch->pcdata->clan ) 
	{
            if ( victim->exp < 10000000 ) /* 5M-9M */
              ch->pcdata->clan->pkills[0]++;
            else if ( victim->exp < 20000000 ) /* 10M-19M */
              ch->pcdata->clan->pkills[1]++;
            else if ( victim->exp < 40000000 ) /* 20M-39M */
              ch->pcdata->clan->pkills[2]++;
            else if ( victim->exp < 60000000 ) /* 40M-59M */
              ch->pcdata->clan->pkills[3]++;
            else if ( victim->exp < 80000000 ) /* 60M-79M */
              ch->pcdata->clan->pkills[4]++;
            else if ( victim->exp < 95000000 ) /* 80M-95M */
              ch->pcdata->clan->pkills[5]++;
            else
              ch->pcdata->clan->pkills[6]++;   /* Supreme */

	}
	update_pos(victim);
	
       if ( victim != ch )
	{
	  act( AT_MAGIC, "Bolts of blue energy rise from the corpse, seeping into $n.", ch, victim->name, NULL, TO_ROOM );
	  act( AT_MAGIC, "Bolts of blue energy rise from the corpse, seeping into you.", ch, victim->name, NULL, TO_CHAR ); 
	}
	if ( victim->pcdata->clan )
	{
            if ( ch->exp < 10000000 )
              victim->pcdata->clan->pdeaths[0]++;
            else if ( ch->exp < 20000000 )
              victim->pcdata->clan->pdeaths[1]++;
            else if ( ch->exp < 40000000 )
              victim->pcdata->clan->pdeaths[2]++;
            else if ( ch->exp < 60000000 )
              victim->pcdata->clan->pdeaths[3]++;
            else if ( ch->exp < 80000000)
              victim->pcdata->clan->pdeaths[4]++;
            else if ( ch->exp < 95000000 )
              victim->pcdata->clan->pdeaths[5]++;
            else
              victim->pcdata->clan->pdeaths[6]++;
	}
	add_timer( victim, TIMER_PKILLED, 115, NULL, 0 );
	WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
	return;
      }
    }
   
    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch) )
    {
      if ( !IS_NPC(victim) )
      {
        if ( victim->pcdata->clan )
          victim->pcdata->clan->mdeaths++;
        victim->pcdata->mdeaths++;
	victim->in_room->area->mdeaths++;
      }
      return;
    }

      
    if ( !IS_NPC(ch) )
    {
     if ( ch->pcdata->clan == victim->pcdata->clan )
     {
      if ( ch->pcdata->clan )
        ch->pcdata->clan->illegal_pk++;
      ch->pcdata->illegal_pk++;
      ch->in_room->area->illegal_pk++;
     }
    }
    if ( !IS_NPC(victim) )
    {
      if ( victim->pcdata->clan )
      {
            if ( ch->level < 10 )
              victim->pcdata->clan->pdeaths[0]++;
            else if ( ch->level < 15 )
              victim->pcdata->clan->pdeaths[1]++;
            else if ( ch->level < 20 )
              victim->pcdata->clan->pdeaths[2]++;
            else if ( ch->level < 30 )
              victim->pcdata->clan->pdeaths[3]++;
            else if ( ch->level < 40 )
              victim->pcdata->clan->pdeaths[4]++;
            else if ( ch->level < 50 )
              victim->pcdata->clan->pdeaths[5]++;
            else
              victim->pcdata->clan->pdeaths[6]++;
      }
//      victim->pcdata->pdeaths++;
      victim->in_room->area->pdeaths++;
    }

    if ( xIS_SET(ch->act, PLR_KILLER) )
      return;
    if ( xIS_SET(ch->act, PLR_OUTLAW) || xIS_SET(victim->act, PLR_OUTLAW))
    return;
    if ( xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) && xIS_SET(victim->pcdata->flags, PCFLAG_DEADLY))
    return;
    set_char_color( AT_WHITE, ch );
    send_to_char( "A strange feeling grows deep inside you, and a tingle goes up your spine...\n\r", ch );
    set_char_color( AT_IMMORT, ch );
    send_to_char( "A deep voice booms inside your head, 'Thou shall now be known as a deadly murderer!!!'\n\r", ch );
    set_char_color( AT_WHITE, ch );
    send_to_char( "You feel as if your soul has been revealed for all to see.\n\r", ch );
    xSET_BIT(ch->act, PLR_KILLER);
    if ( xIS_SET( ch->act, PLR_ATTACKER) )
      xREMOVE_BIT(ch->act, PLR_ATTACKER);
    save_char_obj( ch );
    return;
}

/*
 * See if an attack justifies a ATTACKER flag.
 */
void check_attacker( CHAR_DATA *ch, CHAR_DATA *victim )
{

/* 
 * Made some changes to this function Apr 6/96 to reduce the prolifiration
 * of attacker flags in the realms. -Narn
 */
    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||  xIS_SET(victim->act, PLR_KILLER)
    ||  xIS_SET(victim->act, PLR_THIEF) )
	return;

    /* deadly char check */
    if ( !IS_NPC(ch) && !IS_NPC(victim)
         && CAN_PKILL( ch ) && CAN_PKILL( victim ) )
	return;


    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch)
    ||   ch == victim
    ||   ch->level >= LEVEL_IMMORTAL
    ||   xIS_SET(ch->act, PLR_ATTACKER)
    ||   xIS_SET(ch->act, PLR_KILLER) )
	return;
     if ( can_attack( ch, victim ) == FALSE )
     xSET_BIT(ch->act, PLR_ATTACKER);
    save_char_obj( ch );
    return;
}


/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
//    int ndgaina;
    int pl;


    pl = 0;

    if ( !victim )
    {
      bug( "update_pos: null victim", 0 );
      return;
    }

    if ( victim->hit > 0 )
    {
	if ( victim->position <= POS_STUNNED )
	  victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
    {
	if ( victim->mount )
	{
	  act( AT_ACTION, "$n falls from $N.",
		victim, NULL, victim->mount, TO_ROOM );
	  xREMOVE_BIT( victim->mount->act, ACT_MOUNTED );
	  victim->mount = NULL;
	}
	victim->position = POS_DEAD;
	return;
    }
	 if ( victim->hit < -4 ) victim->position = POS_DEAD;
    else if ( victim->hit <= 0 ) victim->position = POS_STUNNED;
    else                          victim->position = POS_STUNNED;

    if ( victim->mount )
    {
	act( AT_ACTION, "$n falls unconscious from $N.",
		victim, NULL, victim->mount, TO_ROOM );
	xREMOVE_BIT( victim->mount->act, ACT_MOUNTED );
	victim->mount = NULL;
    }
    return;
}


/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    FIGHT_DATA *fight;

    if ( ch->fighting )
    {
	char buf[MAX_STRING_LENGTH];

	sprintf( buf, "Set_fighting: %s -> %s (already fighting %s)",
		ch->name, victim->name, ch->fighting->who->name );
	bug( buf, 0 );
	return;
    }

    /* Limit attackers -Thoric */
    if ( victim->num_fighting > max_fight(victim) )
    {
	send_to_char( "There are too many people fighting for you to join in.\n\r", ch );
	return;
    }

    CREATE( fight, FIGHT_DATA, 1 );
    fight->who	 = victim;
    fight->xp	 = (int) xp_compute( ch, victim ) * 0.85;
    fight->align = align_compute( ch, victim );
    if ( !IS_NPC(ch) && IS_NPC(victim) )
      fight->timeskilled = times_killed(ch, victim);
    ch->num_fighting = 1;
    ch->fighting = fight;
    /* ch->position = POS_FIGHTING; */
	if ( IS_NPC(ch) )
		ch->position = POS_FIGHTING;
	else
		ch->position = POS_FIGHTING;
    victim->num_fighting++;
    if ( victim->switched )
    {
	send_to_char( "You are disturbed!\n\r", victim->switched );
	do_return( victim->switched, "" );
    }
    return;
}


CHAR_DATA *who_fighting( CHAR_DATA *ch )
{
    if ( !ch )
    {
	bug( "who_fighting: null ch", 0 );
	return NULL;
    }
    if ( !ch->fighting )
      return NULL;
    return ch->fighting->who;
}

void free_fight( CHAR_DATA *ch )
{
   if ( !ch )
   {
	bug( "Free_fight: null ch!", 0 );
	return;
   }
   if ( ch->fighting )
   {
     if ( !char_died(ch->fighting->who) )
       --ch->fighting->who->num_fighting;
     DISPOSE( ch->fighting );
   }
   ch->fighting = NULL;
   if ( ch->mount )
     ch->position = POS_MOUNTED;
   else
     ch->position = POS_STANDING;
   return;
}


/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    free_fight( ch );
    update_pos( ch );

    if ( !fBoth )   /* major short cut here by Thoric */
      return;

    for ( fch = first_char; fch; fch = fch->next )
    {
	if ( who_fighting( fch ) == ch )
	{
	    free_fight( fch );
	    update_pos( fch );
	}
    }
    return;
}

/* Vnums for the various bodyparts */
int part_vnums[] =
{	12,	/* Head */
	14,	/* arms */
	15,	/* legs */
	13,	/* heart */
	44,	/* brains */
	16,	/* guts */
	45,	/* hands */
	46,	/* feet */
	47,	/* fingers */
	48,	/* ear */
	49,	/* eye */
	50,	/* long_tongue */
	51,	/* eyestalks */
	52,	/* tentacles */
	53,	/* fins */
	54,	/* wings */
	55,	/* tail */
	56,	/* scales */
	59,	/* claws */
	87,	/* fangs */
	58,	/* horns */
	57,	/* tusks */
	55,	/* tailattack */
	85,	/* sharpscales */
	84,	/* beak */
	86,	/* haunches */
	83,	/* hooves */
	82,	/* paws */
	81,	/* forelegs */
	80,	/* feathers */
	0,	/* r1 */
	0	/* r2 */
};

/* Messages for flinging off the various bodyparts */
char* part_messages[] =
{
	"$n's severed head plops from its neck.",
	"$n's arm is sliced from $s dead body.",
	"$n's leg is sliced from $s dead body.",
	"$n's heart is torn from $s chest.",
	"$n's brains spill grotesquely from $s head.",
	"$n's guts spill grotesquely from $s torso.",
	"$n's hand is sliced from $s dead body.",
	"$n's foot is sliced from $s dead body.",
	"A finger is sliced from $n's dead body.",
	"$n's ear is sliced from $s dead body.",
	"$n's eye is gouged from its socket.",
	"$n's tongue is torn from $s mouth.",
	"An eyestalk is sliced from $n's dead body.",
	"A tentacle is severed from $n's dead body.",
	"A fin is sliced from $n's dead body.",
	"A wing is severed from $n's dead body.",
	"$n's tail is sliced from $s dead body.",
	"A scale falls from the body of $n.",
	"A claw is torn from $n's dead body.",
	"$n's fangs are torn from $s mouth.",
	"A horn is wrenched from the body of $n.",
	"$n's tusk is torn from $s dead body.",
	"$n's tail is sliced from $s dead body.",
	"A ridged scale falls from the body of $n.",
	"$n's beak is sliced from $s dead body.",
	"$n's haunches are sliced from $s dead body.",
	"A hoof is sliced from $n's dead body.",
	"A paw is sliced from $n's dead body.",
	"$n's foreleg is sliced from $s dead body.",
	"Some feathers fall from $n's dead body.",
	"r1 message.",
	"r2 message."
};
			
/*
 * Improved Death_cry contributed by Diavolo.
 * Additional improvement by Thoric (and removal of turds... sheesh!)  
 * Support for additional bodyparts by Fireblade
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
//    EXIT_DATA *pexit;
    int vnum, shift, index, i;

    if ( !ch )
    {
      bug( "DEATH_CRY: null ch!", 0 );
      return;
    }

    vnum = 0;
    msg = NULL;
    
    switch ( number_range(0, 5) )
    {
    default: msg  = "\n\r&rYou hear $n's death cry.";				break;
    case  0:
      msg = "\n\r&r$n screams furiously as $e falls to the ground in a heap!"; break;
    case  1:
      msg  = "\n\r&r$n hits the ground ... DEAD.";			        break;
    case  2:
      msg = "\n\r&r$n catches $s guts in $s hands as they pour through $s fatal"
            " &rwound!";                                                  break;
    case  3: msg  = "\n\r&r$n splatters blood on your armor.";		break;
    case  4: msg  = "\n\r&r$n gasps $s last breath and blood spurts out of $s "
                    "&rmouth and ears.";                                  break;
    case  5:
    	shift = number_range(0, 31);
    	index = 1 << shift;
    	
       	for(i = 0;i < 32 && ch->xflags;i++)
    	{
    		if(HAS_BODYPART(ch, index))
    		{
    			msg = part_messages[shift];
    			vnum = part_vnums[shift];
    			break;
    		}
    		else
    		{
    			shift = number_range(0, 31);
    			index = 1 << shift;
    		}
    	}
    	
    	if(!msg)
    		msg = "&rYou hear $n's death cry.";
    	break;
    }

    act( AT_CARNAGE, msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	if(!get_obj_index(vnum))
	{
		bug("death_cry: invalid vnum", 0);
		return;
	}

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	STRFREE( obj->short_descr );
	obj->short_descr = STRALLOC( buf );

	sprintf( buf, obj->description, name );
	STRFREE( obj->description );
	obj->description = STRALLOC( buf );

	obj = obj_to_room( obj, ch->in_room, ch );
    }

    if ( IS_NPC(ch) )
	msg = "&rYou hear something's death cry.";
    else
	msg = "&rYou hear someone's death cry.";

    was_in_room = ch->in_room;
/*    for ( pexit = was_in_room->first_exit; pexit; pexit = pexit->next )
    {
	if ( pexit->to_room
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( AT_CARNAGE, msg, ch, NULL, NULL, TO_ROOM );
	}
    }*/
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int deathloss;
    int biomass;

    if ( !victim )
    {
      bug( "raw_kill: null victim!", 0 );
      return;
    }
/* backup in case hp goes below 1 */
    if (NOT_AUTHED(victim))
    {
      bug( "raw_kill: killing unauthed", 0 );
      return;
    }

    stop_fighting( victim, TRUE );

    mprog_death_trigger( ch, victim );
    if ( char_died(victim) )
      return;
 /* death_cry( victim ); */

    rprog_death_trigger( ch, victim );
    if ( char_died(victim) )
      return;
    if ( !IS_SET( victim->in_room->room_flags2, ROOM_FREEPK)) 
    make_corpse( victim, ch );


    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed += 1;;
	extract_char( victim, TRUE );
	victim = NULL;
	return;
    }
    
/* 
      if (IS_BIODROID(ch))
        {
         if (plcheck <= 45 && ch->biomass < 10000)
         {
           act( AT_WHITE, "\n\rYou pierce the dying body with your tail and absorb their biomass.", ch, NULL, victim, TO_CHAR);
           act( AT_WHITE, "\n\r$N pierces the dying body with $S tail and absorbs their biomass." , victim, NULL, ch, TO_NOTVICT);
          ch->biomass += number_range(1, 5);
         }
        }
*/
    set_char_color( AT_DIEMSG, victim );
    do_help( victim, "_DIEMSG_" );
    victim->barrier = 0;
    victim->suppress = 0;
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
  if ( !IS_NPC(victim) && !IS_NPC(ch) && ch->pcdata->clan && victim->pcdata->clan && ch->pcdata->clan != victim->pcdata->clan && !IS_SET(victim->in_room->room_flags2, ROOM_FREEPK))
  {
     if ( victim->pcdata->learned[gsn_be] > 1)
     victim->pcdata->learned[gsn_be] = URANGE( 1, victim->pcdata->learned[gsn_be] - number_range(0, 3), 100 );
     if ( ch->pcdata->learned[gsn_be] > 0 && (ch->exp-victim->exp) < 15000000)
     ch->pcdata->learned[gsn_be] = URANGE( 1, ch->pcdata->learned[gsn_be] + number_range(0, 1), 100 );
  }
  if ( sysdata.plloss && !IS_SET(victim->in_room->room_flags2, ROOM_FREEPK))
  {
   if (IS_BIODROID(victim))
    deathloss = victim->exp / 20;
   else
    deathloss = victim->exp / 10;
   if (IS_BIODROID(victim))
   {
    biomass = victim->biomass / 40;
    victim->biomass -= biomass;
    victim->mod = 1;
    victim->mod_str = 0;
    victim->mod_int = 0;
    victim->mod_dex = 0;
    victim->mod_con = 0;
    victim->mod_cha = 0;
    victim->mod_lck = 0;
    xREMOVE_BIT(victim->powerup, PU_PERFECT);
    xREMOVE_BIT(victim->powerup, PU_SEMIP);
    xREMOVE_BIT(victim->powerup, PU_ADULT);
    xSET_BIT(victim->powerup, PU_LARVAE);
   }
    victim->exp -= deathloss;
  }
    xREMOVE_BIT(victim->powerup, PU_SUPPRESS);
    xREMOVE_BIT(victim->powerup, KI_EBALL);
    xREMOVE_BIT(victim->powerup, KI_BAR);
    xREMOVE_BIT(victim->powerup, KI_DISK);
    xREMOVE_BIT(victim->powerup, KI_DDD);
    xREMOVE_BIT(victim->powerup, KI_SPHERE);

    extract_char( victim, FALSE );
    if ( !victim )
    {
      bug( "oops! raw_kill: extract_char destroyed pc char", 0 );
      return;
    }
    victim->carry_weight= 0;
  
    victim->damroll	= 0;
    victim->hitroll	= 0;

    victim->position	= POS_STANDING;
    if (ch->alignment > 1000)
    ch->alignment = 1000;
    if (ch->alignment < -1000)
    ch->alignment = -1000;
    ch->hit  = ch->max_hit;
    ch->mana = ch->max_mana;
    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;


    /*
     * Pardon crimes...						-Thoric
     */
    if ( xIS_SET( victim->act, PLR_KILLER) )
    {
      xREMOVE_BIT( victim->act, PLR_KILLER);
      send_to_char("The gods have pardoned you for your murderous acts.\n\r",victim);
    }
    if ( xIS_SET( victim->act, PLR_THIEF) )
    {
      xREMOVE_BIT( victim->act, PLR_THIEF);
      send_to_char("The gods have pardoned you for your thievery.\n\r",victim);
    }
    if ( IS_SET( sysdata.save_flags, SV_DEATH ) )
	save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
//    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch, *gch_next;
    CHAR_DATA *lch;
//    int xp;
    int members;
    long long int plcheck;

    plcheck = 0;    
    if ( IS_BIODROID(ch) )
    {
     plcheck = 0;
     if (get_curr_pl(ch) >= get_curr_pl(victim))
     plcheck = (get_curr_pl(ch) * 100) / get_curr_pl(victim);
    }
   
    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || victim == ch )
	return;

        if (IS_BIODROID(ch))
        {
         if (plcheck <= 45 && ch->biomass < 10000)
         {
          act( AT_WHITE, "\n\r&RYou pierce the dying body with your tail and absorb their biomass.", ch, NULL, victim, TO_CHAR);
          act( AT_WHITE, "\n\r&R$N pierces the dying body with $S tail and absorbs their biomass." , victim, NULL, ch, TO_NOTVICT);
          ch->biomass += number_range(1, 5);
         }
        }


    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = ch->leader ? ch->leader : ch;

    for( gch = ch->in_room->first_person; gch; gch = gch_next )
    {
	  OBJ_DATA *obj;
	  OBJ_DATA *obj_next;
    
	  gch_next = gch->next_in_room;

	if ( !is_same_group( gch, ch ) )
	    continue;

/*	if ( gch->level - lch->level >  8 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}*/

/*	if ( gch->level - lch->level < -8 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}

	xp = (int) (xp_compute( gch, victim ) * 0.1765) / members;
	if ( !gch->fighting )
	  xp /= 2;
	gch->alignment = align_compute( gch, victim );
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );*/
/*
        if (IS_BIODROID(ch))
        {       
         if (plcheck <= 45 && ch->biomass < 10000)
         {
          act( AT_WHITE, "\n\r$N pierces the dying body with $S tail and absorbs their biomass." , victim, NULL, ch, TO_CHAR);
          act( AT_WHITE, "\n\rYou piece the dying body with your tail and absorb their biomass.", ch, NULL, victim, TO_CHAR);
          act( AT_WHITE, "\n\r$N pieces the dying body with $S tail and absorbs their biomass." , victim, NULL, ch, TO_CHAR);
          ch->biomass += 1;
         }
        }
*/
	for( obj = gch->first_carrying; obj; obj = obj_next )
	{
	  obj_next = obj->next_content;
	  if( obj->wear_loc == WEAR_NONE )
	    continue;

	  if( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) && IS_EVIL( gch ) ) ||
	    ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) && IS_GOOD( gch ) ) ||
	    ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL( gch ) ) )
	  {
	    act( AT_MAGIC, "You are zapped by $p.", gch, obj, NULL, TO_CHAR );
	    act( AT_MAGIC, "$n is zapped by $p.", gch, obj, NULL, TO_ROOM );

	    obj_from_char( obj );
	    obj = obj_to_room( obj, gch->in_room, gch );
	    oprog_zap_trigger( gch, obj );   /* mudprogs */
	    if( char_died( gch ) )
	      break;
	    }
	}
    }

    return;
}


int align_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align, newalign, divalign;

    align = gch->alignment - victim->alignment;

    /* slowed movement in good & evil ranges by a factor of 5, h */
    /* Added divalign to keep neutral chars shifting faster -- Blodkai */
    /* This is obviously gonna take a lot more thought */

    if ( gch->alignment > -350 && gch->alignment < 350 )
	divalign = 4;
    else
	divalign = 20;
    
    if ( align >  500 )
	newalign  = UMIN( gch->alignment + (align-500)/divalign,  1000 );
    else
    if ( align < -500 )
	newalign  = UMAX( gch->alignment + (align+500)/divalign, -1000 );
    else
	newalign  = gch->alignment - (int) (gch->alignment/divalign);

    return newalign;
}


/*
 * Calculate how much XP gch should gain for killing victim
 * Lots of redesigning for new exp system by Thoric
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;
    int xp_ratio;
    int gchlev = gch->level;

    xp	  = (get_exp_worth( victim )
    	  *  URANGE( 0, (victim->level - gchlev) + 10, 13 )) / 10;
    align = gch->alignment - victim->alignment;

    /* bonus for attacking opposite alignment */
    if ( align >  990 || align < -990 )
	xp = (xp*5) >> 2;
    else
    /* penalty for good attacking same alignment */
    if ( gch->alignment > 300 && align < 250 )
	xp = (xp*3) >> 2;

    xp = number_range( (xp*3) >> 2, (xp*5) >> 2 );

    /* get 1/4 exp for players					-Thoric */
    if ( !IS_NPC( victim ) )
      xp /= 4;
    else
    /* reduce exp for killing the same mob repeatedly		-Thoric */
    if ( !IS_NPC( gch ) )
    {
	int times = times_killed( gch, victim );

	if ( times >= 20 )
	   xp = 0;
	else
	if ( times )
	{
	   xp = (xp * (20-times)) / 20;
	   if ( times > 15 )
	     xp /= 3;
	   else
	   if ( times > 10 )
	     xp >>= 1;
	}
    }

    /*
     * semi-intelligent experienced player vs. novice player xp gain
     * "bell curve"ish xp mod by Thoric
     * based on time played vs. level
     */
    if ( !IS_NPC( gch ) && gchlev > 5 )
    {
	xp_ratio = (int) gch->played / gchlev;
	if ( xp_ratio > 20000 )		/* 5/4 */
	    xp = (xp*5) >> 2;
	else
	if ( xp_ratio < 16000 )		/* 3/4 */
	    xp = (xp*3) >> 2;
	else
	if ( xp_ratio < 10000 )		/* 1/2 */
	    xp >>= 1;
	else
	if ( xp_ratio < 5000 )		/* 1/4 */
	    xp >>= 2;
	else
	if ( xp_ratio < 3500 )		/* 1/8 */
	    xp >>= 3;
	else
	if ( xp_ratio < 2000 )		/* 1/16 */
	    xp >>= 4;
    }

    /*
     * Level based experience gain cap.  Cannot get more experience for
     * a kill than the amount for your current experience level   -Thoric
     */
    return URANGE(0, xp, exp_level(gch, gchlev+1) - exp_level(gch, gchlev) );
}


/*
 * Revamped by Thoric to be more realistic
 * Added code to produce different messages based on weapon type - FB
 * Added better bug message so you can track down the bad dt's -Shaddai
 */
void new_dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, OBJ_DATA *obj )
{
    char buf1[256], buf2[256], buf3[256], buf4[256], buf5[256], buf6[256];
    char bugbuf[MAX_STRING_LENGTH];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;
    sh_int dampc=0;
    int pl=0;
    long int charexp;
    long int vicexp;
    long int checkexp;
    long int xp_gain=0, gainexp=0;
    struct skill_type *skill = NULL;
    bool gcflag = FALSE;
    bool gvflag = FALSE;
    int d_index;
    ROOM_INDEX_DATA *was_in_room;

 
    if (xIS_SET(ch->powerup, PU_SUPPRESS))
    charexp = ch->suppress;
    else
    charexp = ch->exp;

    if (xIS_SET(victim->powerup, PU_SUPPRESS))
    vicexp = victim->suppress;
    else
    vicexp = victim->exp;

/* New PL Formula */
   if (!IS_NPC(ch))
   {
    if (ch->exp < 5000000)
    xp_gain = (ch->exp / 5000)+100;
    else
    xp_gain = (ch->exp / 10000)+100;
    checkexp = sqrt(charexp)-sqrt(vicexp);

    if (ch->exp < 5000000)
    checkexp -= 40;
        if (checkexp < 20)
            xp_gain = xp_gain;
        else if (checkexp  < 30)
            xp_gain *= 0.75;
        else if (checkexp  < 40)
            xp_gain *= 0.6;
        else if (checkexp  < 50)
            xp_gain *= 0.5;
        else if (checkexp  < 60)
            xp_gain *= 0.4;
        else if (checkexp  < 70)
            xp_gain *= 0.3;
        else if (checkexp  < 80)
            xp_gain *= 0.2;
        else if (checkexp  < 100)
            xp_gain *= 0.1;
        else
            xp_gain = 0;

        if (xp_gain < 0)
            xp_gain = 0;

    if ( check_weightabout(ch))
    xp_gain *= 1.1;
    if ( check_weightfeet(ch))
    xp_gain *= 1.1;
    if ( check_weighthead(ch))
    xp_gain *= 1.1;
    if ( check_weightwristleft(ch))
    xp_gain *= 1.1;
    if ( check_weightwristright(ch))
    xp_gain *= 1.1;
    if ( check_weightlegs(ch))
    xp_gain *= 1.1;
    if (xIS_SET(ch->powerup, PLR_BOOST))
    xp_gain *= 1.2;


    if (NOT_AUTHED(ch) && ch->exp >= 15000)
    {
        send_to_char("&BYou can not grow stronger until you have been  authorized.\n\r", ch);
        return;
    }
   if ( ch->exp >= 5000000 && ch->exp < 25000000)  {pl = ch->pcdata->learned[gsn_chikone] * 250000;}
   else if ( ch->exp >= 25000000 && ch->exp < 55000000){pl = ch->pcdata->learned[gsn_chiktwo] * 550000; }
   else if ( ch->exp >= 55000000 && ch->exp < 75000000){pl = ch->pcdata->learned[gsn_chikthree] * 750000; }
   else if ( ch->exp >= 75000000 && ch->exp < 100000000){pl = ch->pcdata->learned[gsn_chikfour] * 1000000;}
  else if ( ch->exp >= 100000000 && ch->exp < 135000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}
   if (ch->exp >= 5000000)
   {
   if (  (pl+max_pl_char(ch)) < ch->exp) xp_gain = 0;
   }
   gainexp = URANGE( 0, xp_gain, 12000);
  }

/* Ki Stun *//*
    if (ch->mana <= 0 && !IS_NPC(ch))
    {
      act( AT_GREY, "$n falls unconsious for the loss of ki.", ch, NULL, NULL, TO_ROOM );
      act( AT_GREY, "You fall unconsious for the loss of ki.", ch, NULL, NULL, TO_CHAR );
      stop_fighting( ch, TRUE );
      ch->hit = -1; update_pos( ch );
      ch->mana = 1;
      xREMOVE_BIT(victim->powerup, KI_EBALL);  xREMOVE_BIT(victim->powerup, KI_BAR);
      xREMOVE_BIT(victim->powerup, KI_SPHERE); xREMOVE_BIT(victim->powerup, KI_DISK);
      xREMOVE_BIT(victim->powerup, KI_DDD);
    }*/
/* End */
    if ( ! dam )
	dampc = 0;
    if ( ch->in_room != victim->in_room )
    {
	was_in_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, victim->in_room);
    }
    else
	was_in_room = NULL;


/*
    if ( dt > 0 && dt < top_sskill )
    {
        w_index = 0;
    }
    else
    if ( dt >= TYPE_HIT && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
    {
        w_index = dt - TYPE_HIT;
    }
    else
    {
        sprintf(bugbuf, "Dam_message: bad dt %d from %s in %d.",
                dt, ch->name, ch->in_room->vnum );
        bug( bugbuf, 0);
        dt = TYPE_HIT;
        w_index = 0;
    }
*/
    /* get the damage index */
    if(dam == 0)
    	d_index = 0;
    else if(dampc < 0)
    	d_index = 1;
    else if(dampc <= 100)
   	d_index = 1 + dampc/10;
    else if(dampc <= 200)
   	d_index = 11 + (dampc - 100)/20;
    else if(dampc <= 900)
   	d_index = 16 + (dampc - 200)/100;
    else
   	d_index = 23;

   
         if ( dam ==      0 ) { vs = "miss";    vp = "misses";          }
    else if ( dam <=    1 ) { vs = "scratch"; vp = "scratches";       }
    else if ( dam <=    2 ) { vs = "graze";   vp = "grazes";          }
    else if ( dam <=    3 ) { vs = "nick";    vp = "nicks";           }
    else if ( dam <=    4 ) { vs = "jolt";    vp = "jolts";           }
    else if ( dam <=    5 ) { vs = "wound";   vp = "wounds";          }
    else if ( dam <=    6 ) { vs = "injure";  vp = "injures";         }
    else if ( dam <=    7 ) { vs = "hit";      vp = "hits";           }
    else if ( dam <=    8 ) { vs = "jar";     vp = "jars";            }
    else if ( dam <=    9 ) { vs = "thrash";  vp = "thrashes";        }
    else if ( dam <=   11 ) { vs = "maul";    vp = "mauls";           }
    else if ( dam <=   13 ) { vs = "decimate"; vp = "decimates";      }
    else if ( dam <=   15 ) { vs = "_traumatize_";vp = "_traumatizes_";}
    else if ( dam <=   17 ) { vs = "_devastate_";vp = "_devastates_"; }
    else if ( dam <=   19 ) { vs = "_maim_";    vp = "_maims_";           }
    else if ( dam <=   28 ) { vs = "_demolishes_";   vp = "_demolishes_"; }
    else if ( dam <=   39 ) { vs = "MUTILATE";vp = "MUTILATES";	}
    else if ( dam <=   48 ) { vs = "MASSACRE";  vp = "MASSACRES";     }
    else if ( dam <=  54 ) { vs = "PULVERIZE"; vp = "PULVERIZES";    }
    else if ( dam <=  67 ) { vs = "DESTROY";vp = "DESTROYS";   }
    else if ( dam <=  88 ) { vs = "*** ANNIHILATE ***";vp = "*** ANNIHILATES ***";   }
    else                      { vs = "**** SMITE ****";   vp = "**** SMITES ****";      }   
    punct   = (dam <= 30) ? '.' : '!';


/*
         if ( dam ==      0 ) { vs = "miss";    vp = "misses";          }
    else if ( dampc <=    8 ) { vs = "scratch"; vp = "scratches";       }
    else if ( dampc <=   20 ) { vs = "graze";   vp = "grazes";          }
    else if ( dampc <=   25 ) { vs = "nick";    vp = "nicks";           }
    else if ( dampc <=   30 ) { vs = "jolt";    vp = "jolts";           }
    else if ( dampc <=   35 ) { vs = "wound";   vp = "wounds";          }
    else if ( dampc <=   40 ) { vs = "injure";  vp = "injures";         }
    else if ( dampc <=   50 ) { vs = "hit";      vp = "hits";           }
    else if ( dampc <=   60 ) { vs = "jar";     vp = "jars";            }
    else if ( dampc <=   70 ) { vs = "thrash";  vp = "thrashes";        }
    else if ( dampc <=   80 ) { vs = "maul";    vp = "mauls";           }
    else if ( dampc <=   90 ) { vs = "decimate"; vp = "decimates";      }
    else if ( dampc <=  125 ) { vs = "_traumatize_";vp = "_traumatizes_";}
    else if ( dampc <=  150 ) { vs = "_devastate_";vp = "_devastates_"; }
    else if ( dampc <=  200 ) { vs = "_maim_";    vp = "_maims_";           }
    else if ( dampc <=  300 ) { vs = "_demolishes_";   vp = "_demolishes_"; }
    else if ( dampc <=  400 ) { vs = "MUTILATE";vp = "MUTILATES"; }
    else if ( dampc <=  500 ) { vs = "MASSACRE";  vp = "MASSACRES";     }
    else if ( dampc <=  600 ) { vs = "PULVERIZE"; vp = "PULVERIZES";    }
    else if ( dampc <=  750 ) { vs = "DESTROY";vp = "DESTROYS";   }
    else if ( dampc <=  900 ) { vs = "*** ANNIHILATE ***";vp = "*** ANNIHILATES ***";   }
    else                      { vs = "**** SMITE ****";   vp = "**** SMITES ****";      }
*/
    punct   = (dampc <= 30) ? '.' : '!';
    if ( dam == 0 && (!IS_NPC(ch) && (xIS_SET(ch->pcdata->flags, PCFLAG_GAG)))) gcflag = TRUE;

    if ( dam == 0 && (!IS_NPC(victim) &&
       (xIS_SET(victim->pcdata->flags, PCFLAG_GAG)))) gvflag = TRUE;

    if ( dt == TYPE_HIT )
    {
        sprintf( buf1, "&w$n %s $N%c",  vp, punct );
        sprintf( buf2, "&WYou %s $N%c", vs, punct );
        sprintf( buf3, "&Y$n %s you%c", vp, punct );
    }
    else if (dt == TYPE_SLAM )
    {
       sprintf( buf1, "&R$n's energy ball slams into $N%c", punct );
       sprintf( buf2, "&RYour energy ball slams into $N%c", punct );
       sprintf( buf3, "&R$n's energy ball slams into you%c", punct );
       xREMOVE_BIT(ch->powerup, KI_EBALL);
    }
    else if (dt == TYPE_DSLAM )
    {
       sprintf( buf1, "&R$n's ball slams into $N%c", punct );
       sprintf( buf2, "&RYour ball slams into $N%c", punct );
       sprintf( buf3, "&R$n's ball slams into you%c", punct );
    }
    else if (dt == TYPE_FINGER )
    {
        sprintf( buf1, "&w$n's fingerbeam %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour fingerbeam %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's fingerbeam %s you%c", vp, punct );
    }
    else if (dt == TYPE_WAVE )
    {
        sprintf( buf1, "&w$n's wave %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour wave %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's wave %s you%c", vp, punct );
    }
    else if (dt == TYPE_FIREBALL )
    {
        sprintf( buf1, "&w$n's fireball %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour fireball %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's fireball %s you%c", vp, punct );
    }
    else if (dt == TYPE_DEATHBALL )
    {
        sprintf( buf1, "&w$n's deathball %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour deathball %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's deathball %s you%c", vp, punct );
    }
    else if (dt == TYPE_DEATHWAVE )
    {
        sprintf( buf1, "&w$n's deathwave %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour deathwave %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's deathwave %s you%c", vp, punct );
    }
    else if (dt == TYPE_DEATHBEAM )
    {
        sprintf( buf1, "&w$n's deathbeam %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour deathbeam %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's deathbeam %s you%c", vp, punct );
    }
    else if (dt == TYPE_TAILWHIP )
    {
        sprintf( buf1, "&w$n's tail whips $N%c",  punct );
        sprintf( buf2, "&WYour tail whips $N%c",  punct );
        sprintf( buf3, "&Y$n's tail whips you%c", punct );
    }
    else if (dt == TYPE_TAILSTRIKE )
    {
        sprintf( buf1, "&w$n's tail strikes $N%c",  punct );
        sprintf( buf2, "&WYour tail strikes $N%c",  punct );
        sprintf( buf3, "&Y$n's tail strikes you%c", punct );
    }
    else if (dt == TYPE_TAILPIERCE )
    {
        sprintf( buf1, "&w$n's tail pierces $N%c",  punct );
        sprintf( buf2, "&WYour tail pierces $N%c",  punct );
        sprintf( buf3, "&Y$n's tail pierces you%c", punct );
    }

    else if (dt == TYPE_DUALBEAM )
    {
        sprintf( buf1, "&w$n's dualbeam %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour dualbeam %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's dualbeam %s you%c", vp, punct );
    }
    else if (dt == TYPE_DODOMPA )
    {
        sprintf( buf1, "&w$n's dodompa %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour dodompa %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's dodompa %s you%c", vp, punct );
    }
    else if (dt == TYPE_MAKOSEN )
    {
        sprintf( buf1, "&w$n's makosen %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour makosen %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's makosen %s you%c", vp, punct );
    }
    else if (dt == TYPE_MASENKO )
    {
        sprintf( buf1, "&w$n's masenko %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour masenko %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's masenko %s you%c", vp, punct );
    }
    else if (dt == TYPE_MENTAL )
    {
        sprintf( buf1, "&w$n's rock %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour rock %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's rock %s you%c", vp, punct );
    }
    else if (dt == TYPE_TRIBEAM )
    {
        sprintf( buf1, "&w$n's tribeam %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour tribeam %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's tribeam %s you%c", vp, punct );
    }
    else if (dt == TYPE_SCATTERSHOT )
    {
        sprintf( buf1, "&w$n's bolts %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour bolts %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's bolts %s you%c", vp, punct );
    }
    else if (dt == TYPE_EYE )
    {
        sprintf( buf4, "&w$n's eyebeam %s $N%c",  vp, punct );
        sprintf( buf5, "&WYour eyebeam %s $N%c", vp, punct );
        sprintf( buf6, "&Y$n's eyebeam %s you%c", vp, punct );
    }
    else if (dt == TYPE_DK )
    {
        sprintf( buf4, "&w$n's doublekick %s $N%c",  vp, punct );
        sprintf( buf5, "&WYour doublekick %s $N%c", vp, punct );
        sprintf( buf6, "&Y$n's doublekick %s you%c", vp, punct );
    }
    else if (dt == TYPE_DP )
    {
        sprintf( buf4, "&w$n's doublepunch %s $N%c",  vp, punct );
        sprintf( buf5, "&WYour doublepunch %s $N%c", vp, punct );
        sprintf( buf6, "&Y$n's doublepunch %s you%c", vp, punct );
    }
    else if (dt == TYPE_KICK )
    {
        sprintf( buf1, "&w$n's kick %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour kick %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's kick %s you%c", vp, punct );
    }
    else if (dt == TYPE_NGALIC)
    {
       sprintf( buf1, "&YThe massive energy blast hits $N fully%c", punct );
       sprintf( buf2, "&YYour massive energy blast hits $N fully%c", punct );
       sprintf( buf3, "&Y$n's massive energy blast hits you fully%c", punct );
    }
    else if (dt == TYPE_KAME)
    {
       sprintf( buf1, "&R$n's energy wave blasts $N away%c", punct );
       sprintf( buf2, "&RYour energy wave blasts $N away%c", punct );
       sprintf( buf3, "&R$n's energy wave blasts you away%c", punct );
    }
    else if (dt == TYPE_HELLSFLASH)
    {
       sprintf( buf1, "&R$n's beam blasts $N away%c", punct );
       sprintf( buf2, "&RYour beam blasts $N away%c", punct );
       sprintf( buf3, "&R$n's beam you away%c", punct );
    }
    else if (dt == TYPE_ROCKETPUNCH)
    {
       sprintf( buf1, "&W$n's punch $N away%c", punct );
       sprintf( buf2, "&WYour punch $N away%c", punct );
       sprintf( buf3, "&W$n's punch you away%c", punct );
    }
    else if (dt == TYPE_FINALFLASH)
    {
       sprintf( buf1, "&R$n's beam $N away%c", punct );
       sprintf( buf2, "&RYour beam $N away%c", punct );
       sprintf( buf3, "&R$n's beam you away%c", punct );
    }
    else if (dt == TYPE_DRAGONFIST)
    {
       sprintf( buf1, "&W$n's fist $N away%c", punct );
       sprintf( buf2, "&WYour fist $N away%c", punct );
       sprintf( buf3, "&W$n's fist you away%c", punct );
    }
    else if (dt == TYPE_SBC)
    {
       sprintf( buf1, "&R$n's energy beam blasts $N away%c", punct );
       sprintf( buf2, "&RYour energy beam blasts $N away%c", punct );
       sprintf( buf3, "&R$n's energy beam blasts you away%c", punct );
    }
    else if (dt == TYPE_NBLAST )
    {
        sprintf( buf1, "&w$n's blast %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour blast %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's blast %s you%c", vp, punct );
    }   
    else if (dt == TYPE_GHOST )
    {
        sprintf( buf1, "&R$n's ghost has inflicted heavy damage on $N%c", punct );
        sprintf( buf2, "&RYour ghost has inflicted heavy damage on $N%c", punct );
        sprintf( buf3, "&R$n's ghost has inflicted heavy damage on you%c", punct );
    }
    else if (dt == TYPE_CUT )
    {
       sprintf( buf1, "&R$n's energy disk cuts $N%c", punct );
       sprintf( buf2, "&RYour energy disk cuts $N%c", punct );
       sprintf( buf3, "&R$n's energy disk cuts you%c", punct );
       xREMOVE_BIT(ch->powerup, KI_DISK);
    }
    else if (dt == TYPE_CUTDDD )
    {
       sprintf( buf4, "&R$n's energy disk cuts $N%c", punct );
       sprintf( buf5, "&RYour energy disk cuts $N%c", punct );
       sprintf( buf6, "&R$n's energy disk cuts you%c", punct );
       xREMOVE_BIT(ch->powerup, KI_DDD);
    }
    else if (dt == TYPE_NPUNCH )
    {
        sprintf( buf1, "&w$n's punch %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour punch %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's punch %s you%c", vp, punct );
    }
    else if (dt == TYPE_UPPERCUT )
    {
        sprintf( buf1, "&w$n's uppercut %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour uppercut %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's uppercut %s you%c", vp, punct );
    }
    else if (dt == TYPE_SPINKICK )
    {
        sprintf( buf1, "&w$n's spinkick %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour spinkick %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's spinkick %s you%c", vp, punct );
    }
    else if (dt == TYPE_HACK )
    {
        sprintf( buf1, "&w$n's hack %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour hack %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's hack %s you%c", vp, punct );
    }
    else if (dt == TYPE_CLAW )
    {
        sprintf( buf1, "&w$n's claw %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour claw %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's claw %s you%c", vp, punct );
    }
    else if (dt == TYPE_SLASH )
    {
        sprintf( buf1, "&w$n's slash %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour slash %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's slash %s you%c", vp, punct );
    }

    else if (dt == TYPE_TAIL )
    {
        sprintf( buf1, "&w$n's tail %s $N%c",  vp, punct );
        sprintf( buf2, "&WYour tail %s $N%c", vp, punct );
        sprintf( buf3, "&Y$n's tail %s you%c", vp, punct );
    }
    else if (dt == TYPE_BURN )
    {
       sprintf( buf1, "&R$n's energy sphere burns into $N%c", punct );
       sprintf( buf2, "&RYour energy sphere burns into $N%c", punct );
       sprintf( buf3, "&R$n's energy sphere burns into you%c", punct );
       xREMOVE_BIT(ch->powerup, KI_SPHERE);
    }

    else
    if ( dt > TYPE_HIT && is_wielding_poisoned( ch ) )
    {
	if ( dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	    attack	= attack_table[dt - TYPE_HIT];
	else
	{
         sprintf(bugbuf, "Dam_message: bad dt %d from %s in %d.",
                dt, ch->name, ch->in_room->vnum );
         bug( bugbuf, 0);
	    dt  = TYPE_HIT;
	    attack  = attack_table[0];
        }

	sprintf( buf1, "$n's poisoned %s %s $N%c", attack, vp, punct );
	sprintf( buf2, "Your poisoned %s %s $N%c", attack, vp, punct );
	sprintf( buf3, "$n's poisoned %s %s you%c", attack, vp, punct ); 
    }
    else
    {
	if ( skill )
	{
	    attack	= skill->noun_damage;
	    if ( dam == 0 )
	    {
		bool found = FALSE;

		if ( skill->miss_char && skill->miss_char[0] != '\0' )
		{
		   act( AT_HIT, skill->miss_char, ch, NULL, victim, TO_CHAR );
		   found = TRUE;
		}
		if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
		{
		   act( AT_HITME, skill->miss_vict, ch, NULL, victim, TO_VICT );
		   found = TRUE;
		}
		if ( skill->miss_room && skill->miss_room[0] != '\0' )
		{
		   if (strcmp( skill->miss_room,"supress" ) )
			act( AT_ACTION, skill->miss_room, ch, NULL, victim, TO_NOTVICT );
		   found = TRUE;
		}
		if ( found )	/* miss message already sent */
		{
		   if ( was_in_room )
		   {
			char_from_room(ch);
			char_to_room(ch, was_in_room);
		   }
		   return;
		}
	    }
	    else
	    {
		if ( skill->hit_char && skill->hit_char[0] != '\0' )
		  act( AT_HIT, skill->hit_char, ch, NULL, victim, TO_CHAR );
		if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
		  act( AT_HITME, skill->hit_vict, ch, NULL, victim, TO_VICT );
		if ( skill->hit_room && skill->hit_room[0] != '\0' )
		  act( AT_ACTION, skill->hit_room, ch, NULL, victim, TO_NOTVICT );
	    }
	}
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	{
	    if ( obj )
		attack = obj->short_descr;
	    else
		attack = attack_table[dt - TYPE_HIT];
	}
	else
	{
            sprintf(bugbuf, "Dam_message: bad dt %d from %s in %d.",
                dt, ch->name, ch->in_room->vnum );
            bug( bugbuf, 0);
	    dt  = TYPE_HIT;
	    attack  = attack_table[0];
	}

	sprintf( buf1, "$n's %s %s $N%c",  attack, vp, punct );
	sprintf( buf2, "Your %s %s $N%c",  attack, vp, punct );
	sprintf( buf3, "$n's %s %s you%c", attack, vp, punct );
    }
    if ( dt == TYPE_SLAM || dt == TYPE_CUT || dt == TYPE_BURN || dt == TYPE_KAME || dt == TYPE_SBC)
    {
       sprintf( buf1, "%s [%d]", buf1, dam);
       sprintf( buf2, "%s [%d]", buf2, dam);
       sprintf( buf3, "%s [%d]", buf3, dam);
    }
    else if ( dt == TYPE_DK ||  dt == TYPE_DP || dt == TYPE_EYE || dt == TYPE_CUTDDD)
    {
       sprintf( buf1, "%s [%d]\n\r%s [%d]", buf4, dam, buf4, dam);
       if ( dam > 0 && ( (is_clanned(ch) == TRUE && ch->exp < (125000000+max_pl_char(ch))) || (is_clanned(ch) == FALSE && ch->exp < 100000000) ) && gainexp > 0)
       {
       ch->exp += gainexp;
       ch->exp += gainexp;
       sprintf( buf2, "%s [%d]\n\rYou gain %ld PL. Your new PL is %ld\n\r%s [%d]\n\rYou gain %ld PL. Your new PL is %ld", 
       buf5, dam, gainexp, (ch->exp - gainexp), buf5, dam, gainexp, ch->exp);
       }
       else
       sprintf( buf2, "%s [%d]\n\r%s [%d]", buf5, dam, buf5, dam);
       sprintf( buf3, "%s [%d]\n\r%s [%d]", buf6, dam, buf6, dam);
       victim->hit -= dam;      
       update_pos(victim);
    }
    else
    { 
       sprintf( buf1, "%s [%d]", buf1, dam);
       sprintf( buf2, "%s [%d]", buf2, dam);
       if (!IS_NPC(ch))
       {
       if ( dam > 0 && ( (is_clanned(ch) == TRUE && ch->exp < (125000000+max_pl_char(ch))) || (is_clanned(ch) == FALSE && ch->exp < 100000000) ) && gainexp > 0)
       {
       if (dt == TYPE_SCATTERSHOT && (IS_MUTANT(ch) || IS_BIODROID(ch)))
       gainexp *= 10;
       if (dt == TYPE_SCATTERSHOT && IS_NAMEK(ch))
       gainexp *= 5;
    
       ch->exp += gainexp;
       sprintf( buf2, "%s\n\rYou gain %ld PL. Your new PL is %ld", buf2, gainexp, ch->exp);
       }
       }
       sprintf( buf3, "%s [%d]", buf3, dam);
    }

    act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
    if (!gcflag)  act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
    if (!gvflag)  act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );

   if ( was_in_room )
   {
	char_from_room(ch);
	char_to_room(ch, was_in_room);
   }
    return;
}

#ifndef dam_message
void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    new_dam_message(ch, victim, dam, dt);
}
#endif

void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    
    if (xIS_SET(ch->act, PLR_SPEC) )
    {
         send_to_char("&RSpectators cant do that.\n\r", ch);
         return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->hit <= 10)
    {
    send_to_char( "&RYou are too hurt to do that!\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
        send_to_char( "You have been stupified by Felten.\n\r", ch );
        return;
    }

    if ( !IS_NPC(victim) )
    {
	if ( !xIS_SET(victim->act, PLR_KILLER)
	&&   !xIS_SET(victim->act, PLR_THIEF) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }


    if ( is_safe( ch, victim, TRUE ) )
        return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if ( !IS_SET(ch->in_room->room_flags, ROOM_STADIUM) &&!IS_SET(ch->in_room->room_flags2, ROOM_KOTH) && IS_SET(ch->in_room->area->flags, AFLAG_DRAGON ))
    check_attacker( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    //set_fighting(ch, victim);
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if (xIS_SET(ch->act, PLR_SPEC) )
       {
         send_to_char("&RSpectators cant do that.\n\r", ch);
         return;
       }   
   
    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (ch->exp < 5000000 && !IN_ARENA(ch) && !has_dragonball(victim))
    {
        send_to_char( "You murder??? Pfft, stick to killing puny mobs.\n\r", ch );
        return;
    }
    if ( can_attack( ch, victim ) == FALSE )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }
    if ( ch->hit <= 10)
    {
    send_to_char( "&RYou are too hurt to do that!\n\r", ch );
    return;
    }
    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }
    if ( xIS_SET(ch->act, PLR_SPAR))
    {
        send_to_char( "You are still sparring.\n\r", ch );
        return;
    }

    if ( is_safe( ch, victim, TRUE ) && !xIS_SET(victim->act, PLR_OUTLAW))
	return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }


    if (!IS_NPC (victim) || !in_arena(ch) )
       {
       sprintf( log_buf, "%s: murder %s.", ch->name, victim->name );
       log_string_plus( log_buf, LOG_NORMAL, ch->level );
       }
      if (xIS_SET(victim->act, PLR_SPAR))
      xREMOVE_BIT(victim->act, PLR_SPAR);
      WAIT_STATE( ch, 1 );
      sprintf( buf, "Help!  I am being attacked by %s!", 
                   IS_NPC( ch ) ? ch->short_descr : ch->name );
      if ( IS_PKILL(victim) || victim->exp >= 5000000)
  	do_wartalk( victim, buf );
      else
	do_yell( victim, buf );
      check_illegal_pk( ch, victim );
      check_attacker( ch, victim );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      //set_fighting(ch, victim);
      return;
}

/*
 * Check to see if the player is in an "Arena".
 */
bool in_arena( CHAR_DATA *ch )
{
    if ( IS_SET(ch->in_room->room_flags, ROOM_STADIUM) )
	return TRUE;
    if ( IS_SET(ch->in_room->area->flags, AFLAG_FREEKILL) )
	return TRUE;
    if ( IS_SET(ch->in_room->room_flags2, ROOM_KOTH) )
        return TRUE;
    if ( IS_SET(ch->in_room->room_flags2, ROOM_CTF) )
        return TRUE;
    if ( ch->in_room->vnum >= 29 && ch->in_room->vnum <= 37 )
	return TRUE;
    if ( !str_cmp(ch->in_room->area->filename, "arena.are") )
	return TRUE;

    return FALSE;
}

bool check_illegal_pk( CHAR_DATA *ch, CHAR_DATA *victim )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  if ( xIS_SET(victim->act, PLR_OUTLAW))
  return FALSE;

  if ( !IS_NPC(victim) && !IS_NPC(ch) )
  {
	if ( ( !xIS_SET(victim->pcdata->flags, PCFLAG_DEADLY)
	|| !xIS_SET(ch->pcdata->flags, PCFLAG_DEADLY) ) 
	&& !in_arena(ch)
	&& ch != victim  )
//	&& !( IS_IMMORTAL(ch) && IS_IMMORTAL(victim) ) )
	{
           if ( can_attack( ch, victim) == FALSE )
           {
	    if ( IS_NPC(ch) )
		sprintf(buf, " (%s)", ch->name);
	    if ( IS_NPC(victim) )
		sprintf(buf2, " (%s)", victim->name);

	    sprintf( log_buf, "&p%s on %s%s in &W***&rILLEGAL PKILL&W*** &pattempt at %d",
		(lastplayercmd),
		(IS_NPC(victim) ? victim->short_descr : victim->name),
		(IS_NPC(victim) ? buf2 : ""),
		victim->in_room->vnum );
	    last_pkroom = victim->in_room->vnum;
	    log_string(log_buf);
	    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
	    return TRUE;
           }
	}
    }
    return FALSE;
}


void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int attempt=number_range(-2, 2);
    sh_int door;
    EXIT_DATA *pexit;
    int oldmap = ch->map;
    int oldx = ch->x;
    int oldy = ch->y;


    if (xIS_SET(ch->act, PLR_SPAR))
    {
	send_to_char("You can't flee in a spar.\n\r", ch);
	return;
    }
    if ( !who_fighting(ch) )
    {
	if ( ch->position == POS_FIGHTING)
	{
	  if ( ch->mount )
	    ch->position = POS_MOUNTED;
	  else
	    ch->position = POS_STANDING;
	}
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( IS_NPC( ch ) && ch->position <= POS_SLEEPING )
	return;
    was_in = ch->in_room;
    for ( attempt = 0; attempt < 3; attempt++ )
    {
	door = number_door( );
	if (( pexit = get_exit( was_in, door ) ) == NULL
	||    !pexit->to_room
	||   IS_SET( pexit->exit_info, EX_NOFLEE )
	|| ( IS_SET( pexit->exit_info, EX_CLOSED ))
        || ( IS_SET( pexit->to_room->room_flags, ROOM_PROPA ) )
	|| ( IS_NPC( ch )
	&&   IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) ) )
	    continue;
	if ( ch->mount && ch->mount->fighting )
	    stop_fighting( ch->mount, TRUE );

	move_char( ch, pexit, 0, door );
	if( IS_PLR_FLAG( ch, PLR_ONMAP ) || IS_ACT_FLAG( ch, ACT_ONMAP ) )
	{
	   now_in = ch->in_room;
	   if( ch->map == oldmap && ch->x == oldx && ch->y == oldy )
		continue;
	}
	else
	{
	   if ( ( now_in = ch->in_room ) == was_in )
	     continue;
	}

	ch->in_room = was_in;
	act( AT_FLEE, "$n flees head over heels!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;
	act( AT_FLEE, "$n glances around for signs of pursuit.", ch, NULL, NULL, TO_ROOM );
	if ( !IS_NPC( ch ) )
	{
	    act( AT_YELLOW, "You flee head over heels from combat!", ch, NULL, NULL, TO_CHAR );
	}
	stop_fighting( ch, TRUE );
        WAIT_STATE(ch, 2);
	return;
    }
    act( AT_YELLOW, "You attempt to flee from combat but can't escape!", ch, NULL, NULL, TO_CHAR );
    WAIT_STATE(ch, 2);
    return;
}


void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}


void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "immolate" ) )
    {
      act( AT_FIRE, "Your fireball turns $N into a blazing inferno.",  ch, NULL, victim, TO_CHAR    );
      act( AT_FIRE, "$n releases a searing fireball in your direction.", ch, NULL, victim, TO_VICT    );
      act( AT_FIRE, "$n points at $N, who bursts into a flaming inferno.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "shatter" ) )
    {
      act( AT_LBLUE, "You freeze $N with a glance and shatter the frozen corpse into tiny shards.",  ch, NULL, victim, TO_CHAR    );
      act( AT_LBLUE, "$n freezes you with a glance and shatters your frozen body into tiny shards.", ch, NULL, victim, TO_VICT    );
      act( AT_LBLUE, "$n freezes $N with a glance and shatters the frozen body into tiny shards.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "demon" ) )
    {
      act( AT_IMMORT, "You gesture, and a slavering demon appears.  With a horrible grin, the",  ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "foul creature turns on $N, who screams in panic before being eaten alive.",  ch, NULL, victim, TO_CHAR );
      act( AT_IMMORT, "$n gestures, and a slavering demon appears.  The foul creature turns on",  ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "you with a horrible grin.   You scream in panic before being eaten alive.",  ch, NULL, victim, TO_VICT );
      act( AT_IMMORT, "$n gestures, and a slavering demon appears.  With a horrible grin, the",  ch, NULL, victim, TO_NOTVICT );
      act( AT_IMMORT, "foul creature turns on $N, who screams in panic before being eaten alive.",  ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "pounce" ) )
    {
      act( AT_BLOOD, "Leaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...",  ch, NULL, victim, TO_CHAR );
      act( AT_BLOOD, "In a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your life ends...", ch, NULL, victim, TO_VICT );
      act( AT_BLOOD, "Leaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N's dying body away.",  ch, NULL, victim, TO_NOTVICT );
    }
 
    else if ( !str_cmp( arg2, "slit" ) )
    {
      act( AT_BLOOD, "You calmly slit $N's throat.", ch, NULL, victim, TO_CHAR );
      act( AT_BLOOD, "$n reaches out with a clawed finger and calmly slits your throat.", ch, NULL, victim, TO_VICT );
      act( AT_BLOOD, "$n calmly slits $N's throat.", ch, NULL, victim, TO_NOTVICT );
    }

    else if ( !str_cmp( arg2, "dog" ) )
    {
      act( AT_BLOOD, "You order your dogs to rip $N to shreds.", ch, NULL, victim, TO_CHAR );
      act( AT_BLOOD, "$n orders $s dogs to rip you apart.", ch, NULL, victim, TO_VICT );
      act( AT_BLOOD, "$n orders $s dogs to rip $N to shreds.", ch, NULL, victim, TO_NOTVICT );
    }

    else
    {
      act( AT_IMMORT, "You slay $N in cold blood!",  ch, NULL, victim, TO_CHAR    );
      act( AT_IMMORT, "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
      act( AT_IMMORT, "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    }

    set_cur_char(victim);
    raw_kill( ch, victim );
    return;
}


void do_toggle( CHAR_DATA *ch, char *argument )
{
     if ( (!IS_NPC(ch) && xIS_SET(ch->act, PLR_TOGGLE)) || (IS_NPC(ch) && xIS_SET(ch->act, ACT_TOGGLED)) )
     {
        if (!IS_NPC(ch))
    	xREMOVE_BIT(ch->act, PLR_TOGGLE);
	else
	xREMOVE_BIT(ch->act, ACT_TOGGLED);
	send_to_char( "&wYou will now be finishing people off!\n\r", ch );
     }
     else
     {
	if (!IS_NPC(ch))
	xSET_BIT(ch->act, PLR_TOGGLE);
	else
	xSET_BIT(ch->act, ACT_TOGGLED);
	send_to_char( "&wYou will now be sparring with people.\n\r", ch );
	return;
     }
     return;    
}

/*
 * Inflict damage from a hit.   This is one damn big function.
 */
ch_ret damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char filename[256]; 
//    long int dameq;
    int vnum=0;
    long int maxdam;
    bool npcvict;
    bool loot;
    int  xp_gain;
    int location;
    ch_ret retcode;
    sh_int dampmod;
    CHAR_DATA *gch /*, *lch */;
    int init_gold, new_gold, gold_diff;
    sh_int anopc = 0;  /* # of (non-pkill) pc in a (ch) */
    sh_int bnopc = 0;  /* # of (non-pkill) pc in b (victim) */
    bool chonmap;
    bool viconmap;
    OBJ_DATA *obj=NULL;

    if (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP))
    chonmap = TRUE;
    else if (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP))
    chonmap = TRUE;
    else
    chonmap = FALSE;

    if (IS_NPC(victim) && IS_ACT_FLAG(victim, ACT_ONMAP))
    viconmap = TRUE;
    else if (!IS_NPC(ch) && IS_PLR_FLAG(victim, PLR_ONMAP))
    viconmap = TRUE;
    else
    viconmap = FALSE;

    location = 152;
    retcode = rNONE;

    if ( !ch )
    {
	bug( "Damage: null ch!", 0 );
	return rERROR;
    }
    if ( !victim )
    {
	bug( "Damage: null victim!", 0 );
	return rVICT_DIED;
    }

    if ( victim->position == POS_DEAD )
	return rVICT_DIED;

    npcvict = IS_NPC(victim);

    /*
     * Check damage types for RIS                               -Thoric
     */
    if ( dam && dt != TYPE_UNDEFINED )
    {

        if ( dam == -1 )
        {
            if ( dt >= 0 && dt < top_sskill )
            {
                bool found = FALSE;
                SKILLTYPE *skill = sskill_table[dt];

                if ( skill->imm_char && skill->imm_char[0] != '\0' )
                {
                   act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
                   found = TRUE;
                }
                if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
                {
                   act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
                   found = TRUE;
                }
                if ( skill->imm_room && skill->imm_room[0] != '\0' )
                {
                   act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
                   found = TRUE;
                }
                if ( found )
                   return rNONE;
            }
            dam = 0;
        }
    }

    /*
     * Precautionary step mainly to prevent people in Hell from finding
     * a way out. --Shaddai
     */
    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) )
	dam = 0;
    if ( can_attack( ch, victim ) == FALSE )
    {
       send_to_char("You can't do that.\n\r", ch);
       return rNONE;
    }

    if ( dam && npcvict && ch != victim )
    {
	if ( !xIS_SET( victim->act, ACT_SENTINEL ) )
 	{
	    if ( victim->hunting )
	    {
		if ( victim->hunting->who != ch )	
		{
		    STRFREE( victim->hunting->name );
		    victim->hunting->name = QUICKLINK( ch->name );
		    victim->hunting->who  = ch;
		}
            }
	    else
            if ( !xIS_SET(victim->act, ACT_PACIFIST)  ) /* Gorog */
		start_hunting( victim, ch );
	}

	if ( victim->hating )
	{
   	    if ( victim->hating->who != ch )
 	    {
		STRFREE( victim->hating->name );
		victim->hating->name = QUICKLINK( ch->name );
		victim->hating->who  = ch;
	    }
	}
	else
	if ( !xIS_SET(victim->act, ACT_PACIFIST)  ) /* Gorog */
	    start_hating( victim, ch );
    }

    /*
     * Stop up any residual loopholes.
     */
      maxdam = 5000000;

    if ( dam > maxdam )
    {
	sprintf( buf, "Damage: %d more than %ld points!", dam, maxdam );
	bug( buf, dam );
	sprintf(buf, "** %s (lvl %d) -> %s **",
		ch->name,
		ch->level,
	        victim->name);
	bug( buf, 0 );
	dam = maxdam;
    }

    if (victim != ch)
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if (is_safe(ch, victim, TRUE))
	    return rNONE;
	check_attacker(ch, victim);

	if (victim->position > POS_STUNNED)
	{

             if (!victim->fighting && viconmap && chonmap && ch->map == victim->map && ch->x == victim->x && ch->y == victim->y)
               set_fighting(victim, ch);
  	    else if (!viconmap && !chonmap && !victim->fighting && victim->in_room == ch->in_room)
		set_fighting(victim, ch);

	    /*
	       vwas: victim->position = POS_FIGHTING; 
	     */
	    if ( IS_NPC(victim) && victim->fighting )
		victim->position = POS_FIGHTING;
	    else if (victim->fighting)
                victim->position = POS_FIGHTING;

	}

	if (victim->position > POS_STUNNED)
	{
             if (!ch->fighting && viconmap && chonmap && ch->map == victim->map && ch->x == victim->x && ch->y == victim->y)
               set_fighting(ch, victim);
            else if (!viconmap && !chonmap && !ch->fighting && victim->in_room == ch->in_room)
                set_fighting(ch, victim);


	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if (IS_NPC(ch)
		&& npcvict
		&& victim->master
		&& victim->master->in_room == ch->in_room
		&& number_bits(3) == 0)
	    {
		stop_fighting(ch, FALSE);
		retcode = multi_hit(ch, victim->master, TYPE_UNDEFINED);
		return retcode;
	    }
	}


	/*
	 * More charm stuff.
	 */
	if (victim->master == ch)
	    stop_follower(victim);

	/*
	 * Pkill stuff.  If a deadly attacks another deadly or is attacked by
	 * one, then ungroup any nondealies.  Disabled untill I can figure out
	 * the right way to do it.
	 */


	/*
	   count the # of non-pkill pc in a ( not including == ch ) 
	 */
	for (gch = ch->in_room->first_person; gch; gch = gch->next_in_room)
	    if (is_same_group(ch, gch) && !IS_NPC(gch)
		&& !IS_PKILL(gch) && (ch != gch))
		anopc++;

	/*
	   count the # of non-pkill pc in b ( not including == victim ) 
	 */
	for (gch = victim->in_room->first_person; gch; gch = gch->next_in_room)
	    if (is_same_group(victim, gch) && !IS_NPC(gch)
		&& !IS_PKILL(gch) && (victim != gch))
		bnopc++;


	/*
	   only consider disbanding if both groups have 1(+) non-pk pc,
	   or when one participant is pc, and the other group has 1(+)
	   pk pc's (in the case that participant is only pk pc in group) 
	 */
	if ((bnopc > 0 && anopc > 0)
	    || (bnopc > 0 && !IS_NPC(ch))
	    || (anopc > 0 && !IS_NPC(victim)))
	{
	    /*
	       Disband from same group first 
	     */
	    if (is_same_group(ch, victim))
	    {
		/*
		   Messages to char and master handled in stop_follower 
		 */
		act(AT_ACTION, "$n disbands from $N's group.",
		    (ch->leader == victim) ? victim : ch, NULL,
		    (ch->leader == victim) ? victim->master : ch->master,
		    TO_NOTVICT);
		if (ch->leader == victim)
		    stop_follower(victim);
		else
		    stop_follower(ch);
	    }
	    /*
	       if leader isnt pkill, leave the group and disband ch 
	     */
	    if (ch->leader != NULL && !IS_NPC(ch->leader) &&
		!IS_PKILL(ch->leader))
	    {
		act(AT_ACTION, "$n disbands from $N's group.", ch, NULL,
		    ch->master, TO_NOTVICT);
		stop_follower(ch);
	    }
	    else
	    {
		for (gch = ch->in_room->first_person; gch; gch = gch->next_in_room)
		    if (is_same_group(gch, ch) && !IS_NPC(gch) &&
			!IS_PKILL(gch) && gch != ch)
		    {
			act(AT_ACTION, "$n disbands from $N's group.", ch, NULL,
			    gch->master, TO_NOTVICT);
			stop_follower(gch);
		    }
	    }
	    /*
	       if leader isnt pkill, leave the group and disband victim 
	     */
	    if (victim->leader != NULL && !IS_NPC(victim->leader) &&
		!IS_PKILL(victim->leader))
	    {
		act(AT_ACTION, "$n disbands from $N's group.", victim, NULL,
		    victim->master, TO_NOTVICT);
		stop_follower(victim);
	    }
	    else
	    {
		for (gch = victim->in_room->first_person; gch; gch = gch->next_in_room)
		    if (is_same_group(gch, victim) && !IS_NPC(gch) &&
			!IS_PKILL(gch) && gch != victim)
		    {
			act(AT_ACTION, "$n disbands from $N's group.", gch, NULL,
			    gch->master, TO_NOTVICT);
			stop_follower(gch);
		    }
	    }
	}

	if (dam < 0)
	    dam = 0;

	/*
	 * Check control panel settings and modify damage
	 */
	if (IS_NPC(ch))
	{
	    if (npcvict)
		dampmod = sysdata.dam_mob_vs_mob;
	    else
		dampmod = sysdata.dam_mob_vs_plr;
	}
	else
	{
	    if (npcvict)
		dampmod = sysdata.dam_plr_vs_mob;
	    else
		dampmod = sysdata.dam_plr_vs_plr;
	}
	if (dampmod > 0)
	    dam = (dam * dampmod) / 100;
	dam_message(ch, victim, dam, dt);
    }

    /*
     * Code to handle equipment getting damaged, and also support  -Thoric
     * bonuses/penalties for having or not having equipment where hit
     */
/*    if (dam > 10 && dt != TYPE_UNDEFINED  )
    {
	dameq  = number_range(WEAR_LIGHT, WEAR_EYES);
	damobj = get_eq_char(victim, dameq);
	if ( damobj )
	{
	    if ( dam > get_obj_resistance(damobj)
	    &&   number_bits(1) == 0 )
	    {
		set_cur_obj(damobj);
		damage_obj(damobj);
	    }
	    dam -= 5;  
	}
	else
	    dam += 5;  
    }*/

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;

    /*
     * Get experience based on % of damage done			-Thoric
     */
    if ( dam && ch != victim
    &&  !IS_NPC(ch) && ch->fighting && ch->fighting->xp )
    {
	if ( ch->fighting->who == victim )
	    xp_gain = (int) (ch->fighting->xp * dam) / victim->max_hit;
	else
	    xp_gain = (int) (xp_compute( ch, victim ) * 0.85 * dam) / victim->max_hit;
	gain_exp( ch, xp_gain );
    }

    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 && !xIS_SET(victim->act, PLR_SEMI))
       victim->hit = 1;

    /* Make sure newbies dont die */

    if (!IS_NPC(victim) && NOT_AUTHED(victim) && victim->hit < 1)
	victim->hit = 1;
    if ( can_kill( ch, victim ) == FALSE && victim->hit < 1)
        victim->hit = -1;
    if ( victim->hit < 1 && !IS_NPC(victim) && !IS_NPC(ch) && IS_SET(victim->in_room->area->flags, AFLAG_DRAGON ) )
    {
       if (!has_dragonball2(victim))
       {
        stop_fighting( victim, TRUE );
        victim->hit = 1;
        update_pos(victim);
        if (has_dragonball(victim) || has_dragonball2(victim))
          command(victim, do_drop, "all.dragonball");
                  victim->hit = -1;
        act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
        act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
        update_pos( victim );
       }

    }

    
    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags2, ROOM_KOTH))
    {
        stop_fighting( victim, TRUE );
        victim->hit = -1;
        act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
        act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
        update_pos( victim );
        if ( time_left_in_koth <= 0 )
        {
          char_from_room( victim );
          char_to_room( victim, get_room_index( KOTH_LOSER_ROOM ));
        }
        else
        {
          char_from_room( victim );
          char_to_room( victim, get_room_index( KOTH_HEALING_ROOM ));
        }
        sprintf( buf, "%s has been stunned by %s in the King of the Hill!", victim->name, ch->name);
        victim->hit = victim->max_hit;
        ch->hit = victim->max_hit;
        update_pos(victim);
        sportschan(buf);
        return rNONE;
    }
    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags2, ROOM_CTF))
    {
        stop_fighting( victim, TRUE );
        victim->hit = -1;
        if ( ( obj = get_eq_char( victim, WEAR_LIGHT ) ) != NULL )
        {
            int objvnum;

            objvnum = obj->pIndexData->vnum;
            if ( objvnum != 11503 && objvnum != 11504 )
            vnum = 11502;
            else if ( objvnum == 11503 )
            {
              vnum = 11503;
              do_drop( victim, "red flag" );
            }
            else if ( objvnum == 11504 )
            {
              vnum = 11558;
              do_drop(victim, "blue flag" );
            }
        }
        else
        vnum = 11502;
        act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
        act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
        update_pos( victim );
        char_from_room( victim );
        char_to_room( victim, get_room_index( vnum ));
        victim->hit = 100;
        ch->hit = 100;
        update_pos(victim);
        return rNONE;
    }

    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags, ROOM_STADIUM))
    {
        char_from_room(victim);
        char_to_room(victim,victim->pcdata->roomarena);
        if ( xIS_SET( victim->act, PLR_WASONMAP) )
        {
           xREMOVE_BIT(victim->act, PLR_WASONMAP);
           xSET_BIT(victim->act, PLR_ONMAP);
           victim->x = victim->wasx;
           victim->y = victim->wasy;
           victim->map = victim->wasmap;
           victim->wasx = victim->wasy = victim->wasmap = -1;
        }

        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        if( num_in_arena() == 1)
         find_game_winner();
        do_look(victim, "auto");
        stop_fighting( victim, TRUE );
        lost_arena(victim);
        return rNONE;
     }
     if ( victim->hit < 1 && !IS_NPC(victim) && xIS_SET(ch->act, PLR_SPAR) && IS_SET(victim->in_room->room_flags2, ROOM_SPAR))
     {
      act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
      act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
      ch_printf(ch, "You smack %s KNOCK OUT!", victim->name);
      sprintf(buf, "%s smacks you KNOCK OUT!", ch->name);
      send_to_char(buf, victim);
      xREMOVE_BIT(ch->act, PLR_SPAR);
      xREMOVE_BIT(victim->act, PLR_SPAR);
      stop_fighting( victim, TRUE );
      victim->hit = -1;
      update_pos( victim );
      return rNONE;
     }
    if ( victim->hit < 1 && !IS_NPC(victim) && xIS_SET(ch->act, PLR_TOGGLE) && !IS_SET(victim->in_room->room_flags, ROOM_STADIUM) && !IS_SET(victim->in_room->room_flags2,ROOM_KOTH))
    {
      act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
      act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
      stop_fighting( victim, TRUE );
      victim->hit = -1;
      update_pos( victim );
      return rNONE;
     }
     if ( victim->hit < 1 && !IS_NPC(victim) && IS_NPC(ch) && xIS_SET(ch->act, ACT_TOGGLED) )
     {
      act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
      act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
      stop_fighting( victim, TRUE );
      victim->hit = -1;
      update_pos( victim );
      return rNONE;
     }
   
    if ( !npcvict &&   get_trust(victim) >= LEVEL_IMMORTAL
    &&	 get_trust(ch)	   >= LEVEL_IMMORTAL && !xIS_SET(victim->act, PLR_SEMI)
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );
    switch( victim->position )
    {
    case POS_MORTAL:
	act( AT_DYING, "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	act( AT_DANGER, "You are mortally wounded, and will die soon, if not aided.",
	victim, NULL, NULL, TO_CHAR );
	break;

    case POS_INCAP:
	act( AT_DYING, "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	act( AT_DANGER, "You are incapacitated and will slowly die, if not aided.",
	victim, NULL, NULL, TO_CHAR );
	break;

    case POS_STUNNED:
        if ( victim->hit == -4)
        give_nd( victim, ch );
	  act( AT_ACTION, "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	  act( AT_HURT, "You are stunned, but will probably recover.",
	    victim, NULL, NULL, TO_CHAR );
        xREMOVE_BIT(victim->powerup, KI_EBALL);  xREMOVE_BIT(victim->powerup, KI_BAR);
        xREMOVE_BIT(victim->powerup, KI_SPHERE); xREMOVE_BIT(victim->powerup, KI_DISK);
        xREMOVE_BIT(victim->powerup, KI_DDD);
	break;

    case POS_DEAD:
        act( AT_RED, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	act( AT_RED, "You have been KILLED!!\n\r", victim, 0, 0, TO_CHAR );
        break;

    default:
	if ( dam >= 20 )
	{
          give_rage( victim );
                 
        }
        if ( dam > victim->max_hit / 5 && xIS_SET(victim->act, PLR_TAIL))
        {
           act( AT_HURT, "&RYour tail is damaged and it falls off!", victim, 0, 0, TO_CHAR );
           act( AT_HURT, "&R$N's tail is damaged and it falls off!", ch, NULL, victim, TO_CHAR  );
           act( AT_HURT, "&R$N's tail is damaged and it falls off!", ch, NULL, victim, TO_NOTVICT );
           xREMOVE_BIT(victim->act, PLR_TAIL);
        
    if (xIS_SET(victim->powerup, PU_OOZARU)){
    send_to_char("&YAs you cut your tail, you lose your power of Oozaru", victim);
    act(AT_HURT, "$n loses the power of Oozaru as $s tail fells off!", victim, 0, 0, TO_ROOM);
    victim->mod = 1;
    ch->height_mod = 0;
    ch->weight_mod = 0;
    victim->mod_str = 0;
    victim->mod_dex = 0;
    victim->mod_lck = 0;
    victim->mod_int = 0;
    xREMOVE_BIT(victim->powerup, PU_OOZARU);
    xREMOVE_BIT(victim->powerup, PU_GOLDEN);
   }
      }
	if ( victim->hit < victim->max_hit / 4 && (!xIS_SET(ch->act,PLR_TOGGLE) || !xIS_SET(ch->act, PLR_SPAR)))
	{
	   act( AT_DANGER, "&RYou wish that your wounds would stop BLEEDING so much!",
		victim, 0, 0, TO_CHAR );
           give_rage( victim );
        }
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim))		/* lets make NPC's not slaughter PC's */
    {
	if ( victim->fighting
	&&   victim->fighting->who->hunting
	&&   victim->fighting->who->hunting->who == victim )
	   stop_hunting( victim->fighting->who );

	if ( victim->fighting
	&&   victim->fighting->who->hating
	&&   victim->fighting->who->hating->who == victim )
	   stop_hating( victim->fighting->who );

	if (!npcvict && IS_NPC(ch))
	  stop_fighting( victim, TRUE );
	else
	  stop_fighting( victim, FALSE );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !npcvict )
	{
	    sprintf( log_buf, "%s (%d) killed by %s at %d",
		victim->name,
		victim->level,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );
	    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );

            if (!IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->clan
            &&   victim != ch )
            {
                sprintf( filename, "%s%s.record", CLAN_DIR, ch->pcdata->clan->name );
                sprintf( log_buf, "&P(%2d) %-12s &wvs &P(%2d) %s &P%s ... &w%s",
		  ch->level,
                  ch->name,
		  victim->level,
		  !CAN_PKILL( victim ) ? "&W<Peaceful>" :
		    victim->pcdata->clan ? victim->pcdata->clan->badge :
		      "&P(&WUnclanned&P)",
		  victim->name,
                  ch->in_room->area->name );
		if ( victim->pcdata->clan &&
		     victim->pcdata->clan->name == ch->pcdata->clan->name)
		;
		else
		  append_to_file( filename, log_buf );
            }

	    /*
	     * Dying penalty:
	     * 1/2 way back to previous level.
	     */
	    if ( victim->exp > exp_level(victim, victim->level) )
		gain_exp( victim, (exp_level(victim, victim->level) - victim->exp)/2 );

	    /*
	     * New penalty... go back to the beginning of current level.
	     victim->exp = exp_level( victim, victim->level );
	     */
	}
      else if( !IS_NPC( ch ) && IS_NPC( victim ) )   /* keep track of mob vnum killed */
      {
         add_kill( ch, victim );

         /*
          * Add to kill tracker for grouped chars, as well. -Halcyon
          */
         for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
            if( is_same_group( gch, ch ) && !IS_NPC( gch ) && gch != ch )
               add_kill( gch, victim );
      }

	check_killer( ch, victim );

	if ( ch->in_room == victim->in_room )
	    loot = legal_loot( ch, victim );
	else
	    loot = FALSE;

	set_cur_char(victim);
	raw_kill( ch, victim );

	victim = NULL;

	if ( !IS_NPC(ch) && loot )
	{
	   /* Autogold by Scryn 8/12 */
	    if ( xIS_SET(ch->act, PLR_AUTOGOLD) )
	    {
		init_gold = ch->gold;
		do_get( ch, "coins corpse" );
		new_gold = ch->gold;
		gold_diff = (new_gold - init_gold);
		if (gold_diff > 0)
                {
                  sprintf(buf1,"%d",gold_diff);
		  do_split( ch, buf1 );
		} 
	    }
	    if ( xIS_SET(ch->act, PLR_AUTOLOOT)
	    &&   victim != ch )  /* prevent nasty obj problems -- Blodkai */
		do_get( ch, "all corpse" );
	    else
		do_look( ch, "in corpse" );

	    if ( xIS_SET(ch->act, PLR_AUTOSAC) )
		do_sacrifice( ch, "corpse" ); 
           }

	if ( IS_SET( sysdata.save_flags, SV_KILL ) )
	   save_char_obj( ch );
	return rVICT_DIED;
    }

    if ( victim == ch )
	return rNONE;

    /*
     * Take care of link dead people.
     */
    if ( !npcvict && !victim->desc  )
    {
	if ( number_range( 0, victim->wait ) == 0)
	{
	    return rNONE;
	}
    }

    /*
     * Wimp out?
     */
    if ( npcvict && dam > 0 )
    {
	if ( ( xIS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 ))
	{
	    start_fearing( victim, ch );
	    stop_hunting( victim );
	    do_flee( victim, "" );
	}
    }

    if ( !npcvict
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );
    else
    if ( !npcvict && xIS_SET( victim->act, PLR_FLEE ) )
	do_flee( victim, "" );

    tail_chain( );
    return rNONE;
}


ch_ret sensedamage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    char buf[MAX_STRING_LENGTH];
    char filename[256]; 
    long int maxdam;
    bool npcvict;
    bool loot;
    int location;
    ch_ret retcode;



    location = 152;
    retcode = rNONE;

    if ( !ch )
    {
	bug( "SenseDamage: null ch!", 0 );
	return rERROR;
    }
    if ( !victim )
    {
	bug( "SenseDamage: null victim!", 0 );
	return rVICT_DIED;
    }

    if ( victim->position == POS_DEAD )
	return rVICT_DIED;

    npcvict = IS_NPC(victim);

    /*
     * Precautionary step mainly to prevent people in Hell from finding
     * a way out. --Shaddai
     */
    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) )
	dam = 0;

    if ( dam && npcvict && ch != victim )
    {
	if ( !xIS_SET( victim->act, ACT_SENTINEL ) )
 	{
	    if ( victim->hunting )
	    {
		if ( victim->hunting->who != ch )	
		{
		    STRFREE( victim->hunting->name );
		    victim->hunting->name = QUICKLINK( ch->name );
		    victim->hunting->who  = ch;
		}
          }
	    else
            if ( !xIS_SET(victim->act, ACT_PACIFIST)  ) /* Gorog */
		start_hunting( victim, ch );
	}

	if ( victim->hating )
	{
   	    if ( victim->hating->who != ch )
 	    {
		STRFREE( victim->hating->name );
		victim->hating->name = QUICKLINK( ch->name );
		victim->hating->who  = ch;
	    }
	}
	else
	if ( !xIS_SET(victim->act, ACT_PACIFIST)  ) /* Gorog */
	    start_hating( victim, ch );
    }

    /*
     * Stop up any residual loopholes.
     */
      maxdam = 50000;

    if ( dam > maxdam )
    {
	sprintf( buf, "SenseDamage: %d more than %ld points!", dam, maxdam );
	bug( buf, dam );
	sprintf(buf, "** %s (lvl %d) -> %s **",
		ch->name,
		ch->level,
	        victim->name);
	bug( buf, 0 );
	dam = maxdam;
    }

    if (victim != ch)
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if (is_safe(ch, victim, TRUE))
	    return rNONE;
	check_attacker(ch, victim);
	/*
	 * Damage modifiers.
	 */
	if (dam < 0)
	    dam = 0;
	dam_message(ch, victim, dam, dt);
    }
    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;

    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 && !xIS_SET(victim->act, PLR_SEMI))
       victim->hit = 1;

    /* Make sure newbies dont die */

    if (!IS_NPC(victim) && NOT_AUTHED(victim) && victim->hit < 1)
	victim->hit = 1;

    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags2, ROOM_KOTH))
    {
        stop_fighting( victim, TRUE );
        victim->hit = -1;
        act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
        act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
        update_pos( victim );
        char_from_room( victim );
        char_to_room( victim, get_room_index( ROOM_VNUM_HEAL ));
        sprintf( buf, "%s has been stunned by %s in the King of the Hill!", victim->name, ch->name);
        WAIT_STATE(victim, 8);
        victim->hit = 100;
        ch->hit = 100;
        update_pos(victim);
        return rNONE;
    }
    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags, ROOM_STADIUM))
    {
        char_from_room(victim);
        char_to_room(victim,victim->pcdata->roomarena);
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        if( num_in_arena() == 1)
        find_game_winner();
        do_look(victim, "auto");
        stop_fighting( victim, TRUE );
        lost_arena(victim);
        return rNONE;
     }
     if ( victim->hit < 1 && !IS_NPC(victim) && xIS_SET(ch->act, PLR_SPAR) && IS_SET(victim->in_room->room_flags2, ROOM_SPAR))
     {
      act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
      act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
      ch_printf(ch, "You smack %s KNOCK OUT!", victim->name);
      sprintf(buf, "%s smacks you KNOCK OUT!", ch->name);
      send_to_char(buf, victim);
      xREMOVE_BIT(ch->act, PLR_SPAR);
      xREMOVE_BIT(victim->act, PLR_SPAR);
      stop_fighting( victim, TRUE );
      victim->hit = -1;
      update_pos( victim );
      return rNONE;
     }
     if ( victim->hit < 1 && !IS_NPC(victim) && xIS_SET(ch->act, PLR_TOGGLE) && !IS_SET(victim->in_room->room_flags, ROOM_STADIUM) && !IS_SET(victim->in_room->room_flags2,ROOM_KOTH))
     {
      act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
      act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
      stop_fighting( victim, TRUE );
      victim->hit = -1;
      update_pos( victim );
      return rNONE;
     }
   
    if ( !npcvict &&   get_trust(victim) >= LEVEL_IMMORTAL
    &&	 get_trust(ch)	   >= LEVEL_IMMORTAL && !xIS_SET(victim->act, PLR_SEMI)
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );
    switch( victim->position )
    {
     case POS_STUNNED:
	    act( AT_ACTION, "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	    act( AT_HURT, "You are stunned, but will probably recover.",
	    victim, NULL, NULL, TO_CHAR );
	  break;
    case POS_DEAD:
        act( AT_RED, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	  act( AT_RED, "You have been KILLED!!\n\r", victim, 0, 0, TO_CHAR );
        break;

    default:
	if ( dam >= 20 )
	{
	   act( AT_HURT, "&RThat really did HURT!", victim, 0, 0, TO_CHAR );
           give_rage( victim );
      }
      

      if ( dam > victim->max_hit / 5 && xIS_SET(victim->act, PLR_TAIL))
      {
           act( AT_HURT, "&RYour tail is damaged and it falls off!", victim, 0, 0, TO_CHAR );
           act( AT_HURT, "&R$N's tail is damaged and it falls off!", ch, NULL, victim, TO_CHAR  );
           act( AT_HURT, "&R$N's tail is damaged and it falls off!", ch, NULL, victim, TO_NOTVICT );
           xREMOVE_BIT(victim->act, PLR_TAIL);
           if (xIS_SET(victim->powerup, PU_OOZARU))
           {
            send_to_char("&YAs you cut your tail, you lose your power of Oozaru", victim);
            act(AT_HURT, "$n loses the power of Oozaru as $s tail fells off!", victim, 0, 0, TO_ROOM);
            victim->mod = 1;
    		ch->height_mod = 0;
    		ch->weight_mod = 0;
    		victim->mod_str = 0;
    		victim->mod_dex = 0;
    		victim->mod_lck = 0;
    		victim->mod_int = 0;
    		xREMOVE_BIT(victim->powerup, PU_OOZARU);
           }
      
        }
	if ( victim->hit < victim->max_hit / 4 )

	{
	   act( AT_DANGER, "&RYou wish that your wounds would stop BLEEDING so much!",
		victim, 0, 0, TO_CHAR );
           give_rage( victim );
          }
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim))		/* lets make NPC's not slaughter PC's */
    {
	if ( victim->fighting
	&&   victim->fighting->who->hunting
	&&   victim->fighting->who->hunting->who == victim )
	   stop_hunting( victim->fighting->who );

	if ( victim->fighting
	&&   victim->fighting->who->hating
	&&   victim->fighting->who->hating->who == victim )
	   stop_hating( victim->fighting->who );

	if (!npcvict && IS_NPC(ch))
	  stop_fighting( victim, TRUE );
	else
	  stop_fighting( victim, FALSE );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	if ( !npcvict )
	{
	    sprintf( log_buf, "%s (%d) killed by %s at %d", victim->name, victim->level, (IS_NPC(ch) ? ch->short_descr : ch->name),
   	    victim->in_room->vnum );
	    log_string( log_buf );
	    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
          if (!IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->clan
          &&   victim != ch )
          {
             sprintf( filename, "%s%s.record", CLAN_DIR, ch->pcdata->clan->name );
             sprintf( log_buf, "&P(%2d) %-12s &wvs &P(%2d) %s &P%s ... &w%s",
 	       ch->level,
             ch->name,
		 victim->level,
		 !CAN_PKILL( victim ) ? "&W<Peaceful>" :
		 victim->pcdata->clan ? victim->pcdata->clan->badge :
		 "&P(&WUnclanned&P)",
		 victim->name,
             ch->in_room->area->name );
		if ( victim->pcdata->clan &&
		     victim->pcdata->clan->name == ch->pcdata->clan->name);
		else
	    	  append_to_file( filename, log_buf );
         }
	}
	else
	if ( !IS_NPC(ch) )		/* keep track of mob vnum killed */
	    add_kill( ch, victim );

	check_killer( ch, victim );

	if ( ch->in_room == victim->in_room )
	    loot = legal_loot( ch, victim );
	else
	    loot = FALSE;

	set_cur_char(victim);
	raw_kill( ch, victim );

	victim = NULL;

	if ( IS_SET( sysdata.save_flags, SV_KILL ) )
	   save_char_obj( ch );
	return rVICT_DIED;
    }

    if ( victim == ch )
	return rNONE;

    tail_chain( );
    return rNONE;
}
