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
 *			     Player skills module			    *
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


extern  char *  const           sec_flags[];
extern	int			get_secflag( char *flag );
double calc_angle( short chX, short chY, short lmX, short lmY, double *ipDistan );
double distance( short chX, short chY, short lmX, short lmY );
bool validate_spec_fun( char *name );
OBJ_DATA *      gobj_prev;


/* Checks if a weapon is able to cut */
bool is_sharp_weapon( sh_int value )
{
    switch ( value )
    {
       case WEAPON_KNIFE:
       case WEAPON_DAGGER:
       case WEAPON_SWORD:
       case WEAPON_DAGGER2:
          return TRUE;
       default:
          return FALSE;
    }
    return FALSE;
}

char * const spell_flag[] =
{ "water", "earth", "air", "astral", "area", "distant", "reverse",
"noself", "_unused2_", "accumulative", "recastable", "noscribe",
"nobrew", "group", "object", "character", "secretskill", "pksensitive",
"stoponfail", "nofight", "nodispel", "randomtarget", "r2", "r3", "r4",
"r5", "r6", "r7", "r8", "r9", "r10", "r11"
};

char * const spell_saves[] =
{ "none", "poison_death", "wands", "para_petri", "breath", "spell_staff" };

char * const spell_save_effect[] =
{ "none", "negate", "eightdam", "quarterdam", "halfdam", "3qtrdam",
"reflect", "absorb" };

char * const spell_damage[] =
{ "none", "fire", "cold", "electricity", "energy", "acid", "poison", "drain" };

char * const spell_action[] =
{ "none", "create", "destroy", "resist", "suscept", "divinate", "obscure",
"change" };

char * const spell_power[] =
{ "none", "minor", "greater", "major" };

char * const spell_class[] =
{ "none", "lunar", "solar", "travel", "summon", "life", "death", "illusion" };

char * const target_type[] =
{ "ignore", "offensive", "defensive", "self", "objinv" };


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch );
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, 
	bool fShowN );

bool check_illegal_psteal( CHAR_DATA *ch, CHAR_DATA *victim );

/* from magic.c */


/*
 * Dummy function
 */
void skill_notfound( CHAR_DATA *ch, char *argument )
{
    send_to_char( "Huh?\n\r", ch );
    return;
}


int get_ssave( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_saves) / sizeof(spell_saves[0]); x++ )
      if ( !str_cmp( name, spell_saves[x] ) )
        return x;
    return -1;
}

int get_starget( char *name )
{
    int x;

    for ( x = 0; x < sizeof(target_type) / sizeof(target_type[0]); x++ )
      if ( !str_cmp( name, target_type[x] ) )
        return x;
    return -1;
}

int get_sflag( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_flag) / sizeof(spell_flag[0]); x++ )
      if ( !str_cmp( name, spell_flag[x] ) )
        return x;
    return -1;
}

int get_sdamage( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_damage) / sizeof(spell_damage[0]); x++ )
      if ( !str_cmp( name, spell_damage[x] ) )
        return x;
    return -1;
}

int get_saction( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_action) / sizeof(spell_action[0]); x++ )
      if ( !str_cmp( name, spell_action[x] ) )
        return x;
    return -1;
}

int get_ssave_effect( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_save_effect) / sizeof(spell_save_effect[0]); x++ )
      if ( !str_cmp( name, spell_save_effect[x] ) )
        return x;
    return -1;
}

int get_spower( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_power) / sizeof(spell_power[0]); x++ )
      if ( !str_cmp( name, spell_power[x] ) )
        return x;
    return -1;
}

int get_sclass( char *name )
{
    int x;

    for ( x = 0; x < sizeof(spell_class) / sizeof(spell_class[0]); x++ )
      if ( !str_cmp( name, spell_class[x] ) )
        return x;
    return -1;
}

bool is_legal_kill(CHAR_DATA *ch, CHAR_DATA *vch)
{
  if ( IS_NPC(ch) || IS_NPC(vch) )
    return TRUE;
  if ( !IS_PKILL(ch) || !IS_PKILL(vch) )
    return FALSE;
  if ( ch->pcdata->clan && ch->pcdata->clan == vch->pcdata->clan )
    return FALSE;
  return TRUE;
}


extern char *target_name;	/* from magic.c */


bool can_userace( CHAR_DATA *ch, int gsn )
{
  SKILLTYPE *skill;

  skill = askill_table[gsn];

  if ( !skill )
  return FALSE;

  if (IS_BIODROID(ch) && xIS_SET(ch->act, PLR_MASENKO) && gsn == gsn_masenko)
  return TRUE;
  if (IS_BIODROID(ch) && xIS_SET(ch->act, PLR_TRIBEAM) && gsn == gsn_tribeam)
  return TRUE;
  if (IS_BIODROID(ch) && xIS_SET(ch->act, PLR_MT) && gsn == gsn_mental)
  return TRUE;
  if (IS_BIODROID(ch) && xIS_SET(ch->act, PLR_SCATTER) && gsn == gsn_scattershot)
  return TRUE;


  /* Human */
  if (xIS_SET(skill->raceskill, RACESKILL_HUMAN) && ch->race == RACE_HUMAN)
  return TRUE;
  /* Android */
  if (xIS_SET(skill->raceskill, RACESKILL_ANDROID) && ch->race == RACE_ANDROID)
  return TRUE;
  /* Saiyan */
  if (xIS_SET(skill->raceskill, RACESKILL_SAIYAN) && ch->race == RACE_SAIYAN)
  return TRUE;
  /* Halfbreed */
  if (xIS_SET(skill->raceskill, RACESKILL_HALFBREED) && ch->race == RACE_HALFBREED)
  return TRUE;
  /* Biodroid */
  if (xIS_SET(skill->raceskill, RACESKILL_BIODROID) && ch->race == RACE_BIODROID)
  return TRUE;
  /* Mutant */
  if (xIS_SET(skill->raceskill, RACESKILL_MUTANT) && ch->race == RACE_MUTANT)
  return TRUE;
  /* Icer */
  if (xIS_SET(skill->raceskill, RACESKILL_ICER) && ch->race == RACE_ICER)
  return TRUE;
  /* Buu */
  if (xIS_SET(skill->raceskill, RACESKILL_BUU) && ch->race == RACE_BUU)
  return TRUE;
  /* Namek */
  if (xIS_SET(skill->raceskill, RACESKILL_NAMEK) && ch->race == RACE_NAMEK)
  return TRUE;

 return FALSE;
}

bool check_askill( CHAR_DATA *ch, char *command, char *argument )
{
    int sn, ki, lvl;
    struct timeval time_used;

    /* bsearch for the skill */

      if ( ( sn = askill_lookup( ch, command ))  < 0)
          return FALSE;
/*    for (;;)
    {

        if ( LOWER(command[0]) == LOWER(askill_table[sn]->name[0])
        &&  !str_prefix(command, askill_table[sn]->name)
        &&  (askill_table[sn]->skill_fun || askill_table[sn]->spell_fun != spell_null)
        &&  (IS_NPC(ch)
        ||  ( ch->pcdata->learned[sn] > 0 )) )
                break;
        if (first >= top)
            return FALSE;
        if (strcmp( command, askill_table[sn]->name) < 1)
            top = sn - 1;
        else
            first = sn + 1;
    }*/
    if ( !IS_NPC(ch) && ch->pcdata->learned2[sn] < 1 )
        return FALSE;
    if ( !check_pos( ch, askill_table[sn]->minimum_position ) )
        return TRUE;

    if ( can_userace( ch, sn ) == FALSE )
    {
       send_to_char("Huh?\n\r", ch);
       return TRUE;
    }

    if ( askill_table[sn]->min_mana )
    {
       ki = (askill_table[sn]->min_mana * askill_table[sn]->min_mana ); /*Since we using negative values, turn it positive*/
       ki = sqrt( ki );
       if ( askill_table[sn]->min_mana < 0 )
       {
        if ( !IS_NPC(ch) && ch->mana < (ch->max_mana / ki) )
        {
             send_to_char( "You don't have enough energy.\n\r", ch );
             return TRUE;
        }
       }
       else if ( !IS_NPC(ch) && ch->mana < askill_table[sn]->min_mana )
       {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return TRUE;
       }
    }
    if ( askill_table[sn]->pl )
    {
       if ( !IS_NPC(ch) && ch->exp < askill_table[sn]->pl )
       {
            send_to_char( "You are not strong enough to use this.\n\r", ch );
            return TRUE;
       }
    }
    if ( !IS_NPC(ch))
    lvl = ch->pcdata->skill_level2[sn];
    else
    lvl = 0;
    /*
     * Is this a real do-fun, or a really a spell?
     */
    if ( !askill_table[sn]->skill_fun )
    {
        ch_ret retcode = rNONE;
        void *vo = NULL;
        CHAR_DATA *victim = NULL;
        OBJ_DATA *obj = NULL;

        target_name = "";

        switch ( askill_table[sn]->target )
        {
        default:
            bug( "Check_askill: bad target for sn %d.", sn );
            send_to_char( "Something went wrong...\n\r", ch );
            return TRUE;
        case TAR_IGNORE:
            vo = NULL;
            if ( argument[0] == '\0' )
            {
                if ( (victim=who_fighting(ch)) != NULL )
                    target_name = victim->name;
            }
            else
                target_name = argument;
            break;
        case TAR_CHAR_OFFENSIVE:
            if ( argument[0] == '\0'
            &&  (victim=who_fighting(ch)) == NULL )
            {
                ch_printf( ch, "%s who?\n\r", capitalize( askill_table[sn]->name ) );
                return TRUE;
            }
            else
            if ( argument[0] != '\0'
            &&  (victim=get_char_room(ch, argument)) == NULL )
            {
                send_to_char( "They aren't here.\n\r", ch );
                return TRUE;
            }
            vo = (void *) victim;
            break;

        case TAR_CHAR_DEFENSIVE:
            if ( argument[0] != '\0'
            &&  (victim=get_char_room(ch, argument)) == NULL )
            {
                send_to_char( "They aren't here.\n\r", ch );
                return TRUE;
            }
            if ( !victim )
                victim = ch;
            vo = (void *) victim;
            break;
        case TAR_CHAR_SELF:
            vo = (void *) ch;
            break;
        case TAR_OBJ_INV:
            if ( (obj=get_obj_carry(ch, argument)) == NULL )
            {
                send_to_char( "You can't find that.\n\r", ch );
                return TRUE;
            }
            vo = (void *) obj;
            break;
        }

        /* waitstate */
        WAIT_STATE( ch, askill_table[sn]->beats );
        /* check for failure */
        if ( (number_percent( ) + askill_table[sn]->difficulty * 5)
           > (IS_NPC(ch) ? 75 : ch->pcdata->learned2[sn]) )
        {
            learn_from_failure2( ch, sn );
            return TRUE;
        }

        start_timer(&time_used);
        retcode = (*askill_table[sn]->spell_fun) ( sn, ch->level, ch, vo );
        end_timer(&time_used);
        update_userec(&time_used, &askill_table[sn]->userec);

        if ( retcode == rCHAR_DIED || retcode == rERROR )
            return TRUE;

        if ( char_died(ch) )
            return TRUE;


        if ( retcode == rSPELL_FAILED )
        {
            learn_from_failure2( ch, sn );
            retcode = rNONE;
        }
        else
            learn_from_success2( ch, sn );

        if ( askill_table[sn]->target == TAR_CHAR_OFFENSIVE
        &&   victim != ch
        &&  !char_died(victim) )
        {
            CHAR_DATA *vch;
            CHAR_DATA *vch_next;

            for ( vch = ch->in_room->first_person; vch; vch = vch_next )
            {
                vch_next = vch->next_in_room;
                if ( victim == vch && !victim->fighting && victim->master != ch )
                {
                    retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
                    break;
                }
            }
        }
        return TRUE;
    }
    ch->prev_cmd = ch->last_cmd;    /* haus, for automapping */

    ch->last_cmd = askill_table[sn]->skill_fun;

    start_timer(&time_used);
    (*askill_table[sn]->skill_fun) ( ch, argument );

    end_timer(&time_used);
    update_userec(&time_used, &askill_table[sn]->userec);

    tail_chain( );
    return TRUE;
}

bool check_sskill( CHAR_DATA *ch, char *command, char *argument )
{
    int sn, lvl;
    struct timeval time_used;


    /* bsearch for the skill */

      if ( ( sn = sskill_lookup( ch, command ))  < 0)
          return FALSE;
/*    for (;;)
    {

        if ( LOWER(command[0]) == LOWER(sskill_table[sn]->name[0])
        &&  !str_prefix(command, sskill_table[sn]->name)
        &&  (sskill_table[sn]->skill_fun || sskill_table[sn]->spell_fun != spell_null)
        &&  (IS_NPC(ch)
        ||  ( ch->pcdata->learned[sn] > 0 )) )
                break;
        if (first >= top)
            return FALSE;
        if (strcmp( command, sskill_table[sn]->name) < 1)
            top = sn - 1;
        else
            first = sn + 1;
    }
*/
    if ( !IS_NPC(ch) && ch->pcdata->learned[sn] < 1 )
        return FALSE;
    if ( !check_pos( ch, sskill_table[sn]->minimum_position ) )
        return TRUE;
    if ( sskill_table[sn]->spell_fun == spell_null )
        return TRUE;
    if ( sskill_table[sn]->min_mana )
    {
       if ( !IS_NPC(ch) && ch->mana < sskill_table[sn]->min_mana )
       {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return TRUE;
       }
    }
    if ( sskill_table[sn]->pl )
    {
       if ( !IS_NPC(ch) && ch->exp < sskill_table[sn]->pl )
       {
            send_to_char( "You are not strong enough to use this.\n\r", ch );
            return TRUE;
       }
    }
    if ( !IS_NPC(ch))
    lvl = ch->pcdata->skill_level[sn];
    else
    lvl = 0;
    /*
     * Is this a real do-fun, or a really a spell?
     */
/*    if ( !sskill_table[sn]->skill_fun )
    {
        ch_ret retcode = rNONE;
        void *vo = NULL;
        CHAR_DATA *victim = NULL;
        OBJ_DATA *obj = NULL;

        target_name = "";

        switch ( sskill_table[sn]->target )
        {
        default:
            bug( "Check_sskill: bad target for sn %d.", sn );
            send_to_char( "Something went wrong...\n\r", ch );
            return TRUE;
        case TAR_IGNORE:
            vo = NULL;
            if ( argument[0] == '\0' )
            {
                if ( (victim=who_fighting(ch)) != NULL )
                    target_name = victim->name;
            }
            else
                target_name = argument;
            break;

        case TAR_CHAR_OFFENSIVE:
            if ( argument[0] == '\0'
            &&  (victim=who_fighting(ch)) == NULL )
            {
                ch_printf( ch, "%s who?\n\r", capitalize( sskill_table[sn]->name ) );
                return TRUE;
            }
            else
            if ( argument[0] != '\0'
            &&  (victim=get_char_room(ch, argument)) == NULL )
            {
                send_to_char( "They aren't here.\n\r", ch );
                return TRUE;
            }
            vo = (void *) victim;
            break;

        case TAR_CHAR_DEFENSIVE:
            if ( argument[0] != '\0'
            &&  (victim=get_char_room(ch, argument)) == NULL )
            {
                send_to_char( "They aren't here.\n\r", ch );
                return TRUE;
            }
            if ( !victim )
                victim = ch;
            vo = (void *) victim;
            break;

        case TAR_CHAR_SELF:
            vo = (void *) ch;
            break;
        case TAR_OBJ_INV:
            if ( (obj=get_obj_carry(ch, argument)) == NULL )
            {
                send_to_char( "You can't find that.\n\r", ch );
                return TRUE;
            }
            vo = (void *) obj;
            break;
        }

        WAIT_STATE( ch, sskill_table[sn]->beats );
        if ( (number_percent( ) + sskill_table[sn]->difficulty * 5)
           > (IS_NPC(ch) ? 75 : ch->pcdata->learned[sn]) )
        {
            learn_from_failure( ch, sn );
            return TRUE;
        }
        start_timer(&time_used);
        retcode = (*sskill_table[sn]->spell_fun) ( sn, ch->level, ch, vo );
        end_timer(&time_used);
        update_userec(&time_used, &sskill_table[sn]->userec);

        if ( retcode == rCHAR_DIED || retcode == rERROR )
            return TRUE;

        if ( char_died(ch) )
            return TRUE;

        if ( retcode == rSPELL_FAILED )
        {
            learn_from_failure( ch, sn );
            retcode = rNONE;
        }
        else
            learn_from_success( ch, sn );
        if ( sskill_table[sn]->target == TAR_CHAR_OFFENSIVE
        &&   victim != ch
        &&  !char_died(victim) )
        {
            CHAR_DATA *vch;
            CHAR_DATA *vch_next;

            for ( vch = ch->in_room->first_person; vch; vch = vch_next )
            {
                vch_next = vch->next_in_room;
                if ( victim == vch && !victim->fighting && victim->master != ch )
                {
                    retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
                    break;
                }
            }
        }
        return TRUE;
    }
   */
    ch->prev_cmd = ch->last_cmd;    /* haus, for automapping */
    ch->last_cmd = sskill_table[sn]->skill_fun;

    start_timer(&time_used);
    (*sskill_table[sn]->skill_fun) ( ch, argument );

    end_timer(&time_used);
    update_userec(&time_used, &sskill_table[sn]->userec);

    tail_chain( );
    return TRUE;
}





void do_skin( CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *korps, *corpse, *obj, *skin;
  bool found;
  char *name;
  char buf[MAX_STRING_LENGTH];
  found = FALSE;

  if ( !IS_PKILL(ch) && !IS_IMMORTAL(ch) )
  {
     send_to_char( "Leave the hideous defilings to the killers!\n", ch );
     return;
  }
  if ( argument[0] == '\0' )
  {
     send_to_char("What corpse do you wish to decapitate?\n\r", ch);
     return;
  }
  if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
  ||   ( obj->value[3] != 1 && obj->value[3] != 2 && obj->value[3] != 3
      && obj->value[3] != 11) )
  {
    send_to_char("You need to wield a sharp weapon.\n\r", ch);
    return;
  }
  
  if ( (corpse = get_obj_here( ch, argument )) == NULL)
  {
  send_to_char("You can't find that here.\n\r", ch);
  return;
  }
  
  if (corpse->item_type != ITEM_CORPSE_PC )
  {
    send_to_char("You can only decapitate the bodies of other players.\n\r", ch);
    return;
  }
  
  if( corpse->value[4] == 1 )/*line added by Daht*/
  {
     korps = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
     skin                = create_object( get_obj_index(OBJ_VNUM_SKIN), 0 );
     name                = IS_NPC(ch) ? korps->short_descr : corpse->short_descr;

     sprintf( buf, skin->short_descr, name );
     STRFREE( skin->short_descr );
     skin->short_descr = STRALLOC( buf );

     sprintf( buf, skin->description, name );
     STRFREE( skin->description );
     skin->description = STRALLOC( buf );

     sprintf( buf, skin->name, name );
     STRFREE( skin->name );
     skin->name = STRALLOC( buf );

     act( AT_BLOOD, "$n slices the head of $p.", ch, corpse, NULL, TO_ROOM);
     act( AT_BLOOD, "You slice the head of $p.", ch, corpse, NULL, TO_CHAR);
     obj_to_char( skin, ch );
     corpse->value[4] = 0;/* reset value to 0 to indicate it already has been skinned*/
     skin->cost = 10000;
     return;
  }
  send_to_char ("That corpse has already been decapitated!\n\r", ch);
  return;
}



/*
 * Lookup a skills information
 * High god command
 */
void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;
    bool teacher=FALSE;
    SKILLTYPE *skill = NULL;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }
    send_to_char("\n\r", ch);
    if ( !str_cmp( arg, "all" ) )
    {
        send_to_char("Skills:\n\r", ch);
	for ( sn = 0; sn < top_sskill && sskill_table[sn] && sskill_table[sn]->name; sn++ )
        {
            if (!sskill_table[sn]->teachers || sskill_table[sn]->teachers[0] == '\0'  )
             teacher = FALSE;
            else
	     teacher = TRUE;
	    pager_printf( ch, "Sn: %4d Slot: %4d Skill '%-20s' Teacher: %s\n\r",
		sn, sskill_table[sn]->slot, sskill_table[sn]->name, teacher ? "Yes" : "No"  );
        }
        send_to_char("Abilities:\n\r", ch);
     
        for ( sn = 0; sn < top_askill && askill_table[sn] && askill_table[sn]->name; sn++ )
        {
            if (!askill_table[sn]->teachers || askill_table[sn]->teachers[0] == '\0'  )
             teacher = FALSE;
	    else
	     teacher = TRUE;
            pager_printf( ch, "Sn: %4d Slot: %4d Skill: '%-20s' Teacher: %s\n\r",
                sn, askill_table[sn]->slot, askill_table[sn]->name, teacher ? "Yes" : "No" );
        }
    }
    else
    if ( !str_cmp( arg, "sskills" ) )
    {
        for ( sn = 0; sn < top_sskill && sskill_table[sn] && sskill_table[sn]->name; sn++ )
           pager_printf( ch, "%d) %s\n\r", sn, sskill_table[sn]->name );
    }
    else
    if ( !str_cmp( arg, "askills" ) )
    {
        for ( sn = 0; sn < top_askill && askill_table[sn] && askill_table[sn]->name; sn++ )
           pager_printf( ch, "%d) %s\n\r", sn, askill_table[sn]->name );
    }
    else
    {
	SMAUG_AFF *aff;
	int cnt = 0;

        if ( ( sn = sskill_lookup( ch, arg ) ) >= 0 )
            skill = sskill_table[sn];
        else
        if ( ( sn = askill_lookup( ch, arg ) ) >= 0 )
            skill = askill_table[sn];
	else
	{
	    send_to_char( "No such skill, spell, proficiency or tongue.\n\r", ch );
	    return;
	}
	if ( !skill )
	{
	    send_to_char( "Not created yet.\n\r", ch );
	    return;
	}

	ch_printf( ch, "Sn: %4d Slot: %4d %s: '%-20s'\n\r",
	    sn, skill->slot, skill_tname[skill->type], skill->name );
	if ( skill->info )
	    ch_printf( ch, "DamType: %s  ActType: %s   ClassType: %s   PowerType: %s\n\r",
		spell_damage[SPELL_DAMAGE(skill)],
		spell_action[SPELL_ACTION(skill)],
		spell_class[SPELL_CLASS(skill)],
		spell_power[SPELL_POWER(skill)] );
	if ( skill->flags )
	{
	    int x;

	    strcpy( buf, "Flags:" );
	    for ( x = 0; x < 32; x++ )
	      if ( SPELL_FLAG( skill, 1 << x ) )
	      {
		strcat( buf, " " );
		strcat( buf, spell_flag[x] );
	      }
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	}
	ch_printf( ch, "Saves: %s  SaveEffect: %s\n\r",
	    spell_saves[(int) skill->saves],
	    spell_save_effect[SPELL_SAVE(skill)] );

	if ( skill->difficulty != '\0' )
	    ch_printf( ch, "Difficulty: %d\n\r", (int) skill->difficulty );
        ch_printf( ch, "Mana: %d  \n\r", skill->min_mana);
	ch_printf( ch, "Type: %s  Target: %s  Minpos: %d   Beats: %d  Range: %d   Radius: %d\n\r",
		skill_tname[skill->type],
		target_type[URANGE(TAR_IGNORE, skill->target, TAR_OBJ_INV)],
		skill->minimum_position,
		skill->beats, skill->range, skill->radius );
        if ( skill->pl )
        ch_printf(ch, "Powerlevel Required: %lld\n\r", skill->pl );
        if ( skill->name )
        ch_printf( ch, "Name: %s  ", skill->name );
        send_to_char( "\n\r", ch );
        if ( skill->keyword )
        ch_printf( ch, "Keyword: %s  ", skill->keyword ); 

	ch_printf( ch, "\n\rFlags: %d  Value: %d  Info: %d  Code: %s\n\r",
		skill->flags,
		skill->value,
		skill->info,
                skill->skill_fun ? skill->skill_fun_name : skill->spell_fun_name );

        ch_printf( ch, "Sectors Allowed: %s\n",
		skill->spell_sector?flag_string(skill->spell_sector,sec_flags):
			"All");
	ch_printf( ch, "Dammsg: %s\n\rWearoff: %s\n",
		skill->noun_damage,
		skill->msg_off ? skill->msg_off : "(none set)" );
	if ( skill->dice && skill->dice[0] != '\0' )
	    ch_printf( ch, "Dice: %s\n\r", skill->dice );
	if ( skill->teachers && skill->teachers[0] != '\0' )
	    ch_printf( ch, "Teachers: %s\n\r", skill->teachers );
	if ( skill->components && skill->components[0] != '\0' )
	    ch_printf( ch, "Components: %s\n\r", skill->components );
	if ( skill->participants )
	    ch_printf( ch, "Participants: %d\n\r", (int) skill->participants );
	if ( skill->userec.num_uses )
	    send_timer(&skill->userec, ch);
	for ( aff = skill->affects; aff; aff = aff->next )
	{
	    if ( aff == skill->affects )
	      send_to_char( "\n\r", ch );
	    sprintf( buf, "Affect %d", ++cnt );
	    if ( aff->location )
	    {
		strcat( buf, " modifies " );
		strcat( buf, a_types[aff->location % REVERSE_APPLY] );
		strcat( buf, " by '" );
		strcat( buf, aff->modifier );
		if ( aff->bitvector != -1 )
		  strcat( buf, "' and" );
		else
		  strcat( buf, "'" );
	    }
	    if ( aff->bitvector != -1 )
	    {
		strcat( buf, " applies " );
		strcat( buf, a_flags[aff->bitvector] );
	    }
	    if ( aff->duration[0] != '\0' && aff->duration[0] != '0' )
	    {
		strcat( buf, " for '" );
		strcat( buf, aff->duration );
		strcat( buf, "' rounds" );
	    }
	    if ( aff->location >= REVERSE_APPLY )
		strcat( buf, " (affects caster only)" );
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    
	    if ( !aff->next )
	      send_to_char( "\n\r", ch );
	}

	if ( skill->hit_char && skill->hit_char[0] != '\0' )
	    ch_printf( ch, "Hitchar   : %s\n\r", skill->hit_char );
	if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
	    ch_printf( ch, "Hitvict   : %s\n\r", skill->hit_vict );
	if ( skill->hit_room && skill->hit_room[0] != '\0' )
	    ch_printf( ch, "Hitroom   : %s\n\r", skill->hit_room );
	if ( skill->hit_dest && skill->hit_dest[0] != '\0' )
	    ch_printf( ch, "Hitdest   : %s\n\r", skill->hit_dest );
	if ( skill->miss_char && skill->miss_char[0] != '\0' )
	    ch_printf( ch, "Misschar  : %s\n\r", skill->miss_char );
	if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
	    ch_printf( ch, "Missvict  : %s\n\r", skill->miss_vict );
	if ( skill->miss_room && skill->miss_room[0] != '\0' )
	    ch_printf( ch, "Missroom  : %s\n\r", skill->miss_room );
	if ( skill->die_char && skill->die_char[0] != '\0' )
	    ch_printf( ch, "Diechar   : %s\n\r", skill->die_char );
	if ( skill->die_vict && skill->die_vict[0] != '\0' )
	    ch_printf( ch, "Dievict   : %s\n\r", skill->die_vict );
	if ( skill->die_room && skill->die_room[0] != '\0' )
	    ch_printf( ch, "Dieroom   : %s\n\r", skill->die_room );
	if ( skill->imm_char && skill->imm_char[0] != '\0' )
	    ch_printf( ch, "Immchar   : %s\n\r", skill->imm_char );
	if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	    ch_printf( ch, "Immvict   : %s\n\r", skill->imm_vict );
	if ( skill->imm_room && skill->imm_room[0] != '\0' )
	    ch_printf( ch, "Immroom   : %s\n\r", skill->imm_room );
	 send_to_char( "\n\r", ch );
            ch_printf( ch, "Racetype  : %s\n\r", ext_flag_string(&skill->raceskill, raceskills_name));

    }

    return;
}

void do_slevel( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value, sn;
    bool fAll=FALSE;
    bool ability=FALSE;

    sn = value = 0;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
       send_to_char( "Syntax: slevel <victim> <skill/all> <value>\n\r", ch);
       return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
         send_to_char( "No such person in this world.\n\r", ch);
         return;
    }
    if ( IS_NPC(ch) || IS_NPC(victim) )
    {
       send_to_char("No mobs on ch nor victim allowed.\n\r", ch);
       return;
    }
    if ( !str_cmp( arg2, "all" ) )
    {
         fAll = TRUE;
    }
    else
    {
      if ( ( sn = sskill_lookup( ch, arg2 ) ) > 0 )
      {
          if (value < 0 )
          sn = -1;
          ability = FALSE;
      }
      else if ( ( sn = askill_lookup( ch, arg2 ) ) > 0 )
      {
          if (value < 0 )
          sn = -1;
          ability = TRUE;
      }
      if ( sn < 0 )
      {
          send_to_char( "\n\rNo such skill.\n\r", ch );
          return;
      }
    }
    if ( !is_number(argument) )
    {
      send_to_char("\n\rValue must be a number.\n\r", ch);
      return;
    }
    value = atoi(argument);
    if ( value < 0 || value > 15 )
    {
      send_to_char("\n\rInvalid Value.\n\r", ch);
      return;
    }
    if ( fAll )
    {
       for ( sn = 0; sn < top_sskill; sn++ )
       {
         if (sskill_table[sn]->value < 0 )
         continue;
         victim->pcdata->skill_level[sn] = value;
       }
       for ( sn = 0; sn < top_askill; sn++ )
       {
         if (askill_table[sn]->value < 0 )
         continue;
         victim->pcdata->skill_level2[sn] = value;
       }
       ch_printf( ch, "Setting %s skills to: %d", ch == victim ? "your" : victim->name, value );
       return;
    }
    if ( ability )
    {
       victim->pcdata->skill_level2[sn] = value; 
       ch_printf( ch, "Setting %s %s to: %d", ch == victim ? "your" : victim->name, askill_table[sn]->name, value );
    }
    else
    {
       victim->pcdata->skill_level[sn] = value;
       ch_printf( ch, "Setting %s %s to: %d", ch == victim ? "your" : victim->name, sskill_table[sn]->name, value );
    }

 
 return;
}
/*
 * Set a skill's attributes or what skills a player has.
 * High god command, with support for creating skills/spells/herbs/etc
 */
void do_sset( CHAR_DATA *ch, char *argument )
{

    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;
    bool ability=FALSE;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     sset <victim> all     <value>\n\r",	ch );
	if ( get_trust(ch) > LEVEL_SUB_IMPLEM )
	{
	  send_to_char( "or:     sset save sskill table\n\r",		ch );
	  send_to_char( "or:     sset save askill table\n\r",		ch );
	  send_to_char( "or:     sset create sskill 'new skill'\n\r",	ch );
	  send_to_char( "or:     sset create askill 'new ability'\n\r",	ch );
	}
	if ( get_trust(ch) > LEVEL_GREATER )
	{
	  send_to_char( "or:     sset <sn>     <field> <value>\n\r",	ch );
	  send_to_char( "\n\rField being one of:\n\r",			ch );
          send_to_char( "  name code keyword mana \n\r", ch );
	  send_to_char( "  target minpos slot beats radius dammsg wearoff minlevel\n\r", ch );
	  send_to_char( "  type damtype acttype classtype powertype seffect flag dice value difficulty\n\r", ch );
	  send_to_char( "  affect rmaffect level adept hit miss die imm (char/vict/room)\n\r", ch );
	  send_to_char( "  components teachers powerlevel\n\r",ch );
	  send_to_char( "  sector\n\r", ch );
	  send_to_char( "Affect having the fields: <location> <modfifier> [duration] [bitvector]\n\r", ch );
	  send_to_char( "(See AFFECTTYPES for location, and AFFECTED_BY for bitvector)\n\r", ch );
	}
	send_to_char( "Skill being any skill or spell.\n\r",		ch );
	return;
    }

    if ( get_trust(ch) > LEVEL_SUB_IMPLEM
    &&  !str_cmp( arg1, "save" )
    &&	!str_cmp( argument, "table" ) )
    {
	if ( !str_cmp( arg2, "skill" ) )
	{
	    send_to_char( "Saving skill tables...\n\r", ch );
	    save_sskill_table();
            save_askill_table();
	    return;
	}
        else if ( !str_cmp( arg2, "sskill" ) )
        {
            send_to_char( "Saving sskill tables...\n\r", ch );
            save_sskill_table();
            return;
        }
        else if ( !str_cmp( arg2, "askill" ) )
        {
            send_to_char( "Saving askill tables...\n\r", ch );
            save_askill_table();
            return;
        }

    }
    if ( get_trust(ch) > LEVEL_SUB_IMPLEM
    &&  !str_cmp( arg1, "create" )
    && (!str_cmp( arg2, "sskill" ) || !str_cmp( arg2, "askill" ) )   )
    {
	struct skill_type *skill;
	sh_int type = SKILL_UNKNOWN;

	if ( !str_cmp( arg2, "sskill" ) )
	{
	    type = SKILL_SSKILL;
	    if ( top_sskill >= MAX_SSKILL )
	    {
		ch_printf( ch, "The current top sskill is %d, which is the maximum.  "
			   "To add more skills,\n\rMAX_SSKILL will have to be "
			   "raised in mud.h, and the mud recompiled.\n\r",
			   top_sskill );
		return;
	    }
	}
	else
        if ( !str_cmp( arg2, "askill" ) )
        {
            type = SKILL_ASKILL;
            if ( top_askill >= MAX_ASKILL )
            {
                ch_printf( ch, "The current top askill is %d, which is the maximum.  "
                           "To add more skills,\n\rMAX_ASKILL will have to be "
                           "raised in mud.h, and the mud recompiled.\n\r",
                           top_askill );
                return;
            }
        }
	CREATE( skill, struct skill_type, 1 );
	skill->slot = 0;
	if ( type == SKILL_SSKILL )
	{
	    int max, x;

	    sskill_table[top_sskill++] = skill;
	    for ( max = x = 0; x < top_sskill-1; x++ )
		if ( sskill_table[x] && sskill_table[x]->slot > max )
		    max = sskill_table[x]->slot;
	    skill->slot = max+1;
	}
        else if ( type == SKILL_ASKILL )
        {
            int max, x;

            askill_table[top_askill++] = skill;
            for ( max = x = 0; x < top_askill-1; x++ )
                if ( askill_table[x] && askill_table[x]->slot > max )
                    max = askill_table[x]->slot;
            skill->slot = max+1;
        }
	skill->min_mana = 0;
	skill->name = str_dup( argument );
	skill->noun_damage = str_dup( "" );
	skill->msg_off = str_dup( "" );
	skill->spell_fun = spell_smaug;
	skill->type = type;
	skill->spell_sector = 0;
        if (!str_cmp( arg2, "sskill" ) )
          skill->type  = SKILL_SSKILL;
        else if (!str_cmp( arg2, "askill" ) )
          skill->type  = SKILL_ASKILL;


	send_to_char( "Done.\n\r", ch );
	return;
    }

    if ( str_cmp( arg1, "skill" ) )   
    {
	struct skill_type *skill;

        if ( ( victim = get_char_world( ch, arg1 ) ) != NULL )
        {
            command( ch, do_sset, "skill %s %s %s", arg1, arg2, argument );
            return;
        } 
        if ( ( sn = sskill_lookup( ch, arg1 )) >= 0 )
          skill = sskill_table[sn];
        else if ( ( sn = askill_lookup( ch, arg1 ) ) >= 0 )
          skill = askill_table[sn];
        else
        {
		send_to_char( "Sskill number out of range.\n\r", ch );
		return;
	}



	if ( !str_cmp( arg2, "difficulty" ) )
	{
	    skill->difficulty = atoi( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "participants" ) )
	{
	    skill->participants = atoi( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
        if ( !str_cmp( arg2, "raceskill" ) )
        {
          if ( argument[0] == '\0' )
          {
             send_to_char( "Usage: sset <skill> raceskill <race>\n\r", ch );
             return;
          }
          while ( argument[0] != '\0' )
          {
             argument = one_argument( argument, arg2 );
             value = get_racetype( arg2 );
             if ( value < 0 || value > MAX_BITS )
               ch_printf( ch, "\n\r&wUnknown flag: %s\n\r", arg2 );
               xTOGGLE_BIT( skill->raceskill, value );
             ch_printf(ch, "\n\rSetting %s's raceskill to: %s\n\r", arg1, capitalize(arg2));
          }
          return;
        }

	if ( !str_cmp( arg2, "damtype" ) )
	{
	    int x = get_sdamage( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell damage type.\n\r", ch );
	    else
	    {
		SET_SDAM( skill, x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "acttype" ) )
	{
	    int x = get_saction( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell action type.\n\r", ch );
	    else
	    {
		SET_SACT( skill, x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "classtype" ) )
	{
	    int x = get_sclass( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell class type.\n\r", ch );
	    else
	    {
		SET_SCLA( skill, x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "powertype" ) )
	{
	    int x = get_spower( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell power type.\n\r", ch );
	    else
	    {
		SET_SPOW( skill, x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "seffect" ) )
	{
	    int x = get_ssave_effect( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell save effect type.\n\r", ch );
	    else
	    {
		SET_SSAV( skill, x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "flag" ) )
	{
	    int x = get_sflag( argument );

	    if ( x == -1 )
		send_to_char( "Not a spell flag.\n\r", ch );
	    else
	    {
		TOGGLE_BIT( skill->flags, 1 << x );
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "saves" ) )
	{
	    int x = get_ssave( argument );

	    if ( x == -1 )
		send_to_char( "Not a saving type.\n\r", ch );
	    else
	    {
		skill->saves = x;
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "code" ) )
	{
	   SPELL_FUN *spellfun;
	   DO_FUN *dofun;

	   if( !str_prefix( "do_", argument ) && ( dofun = skill_function( argument ) ) != skill_notfound )
	   {
		skill->skill_fun = dofun;
		skill->spell_fun = NULL;
		DISPOSE( skill->skill_fun_name );
		skill->skill_fun_name = str_dup( argument );
	   }		
	   else if( ( spellfun = spell_function( argument ) ) != spell_notfound )
	   {
		skill->spell_fun = spellfun;
		skill->skill_fun = NULL;
		DISPOSE( skill->skill_fun_name );
		skill->spell_fun_name = str_dup( argument );
	   }
	   else if( validate_spec_fun( argument ) )
	   {
		send_to_char( "Cannot use a spec_fun for skills or spells.\n\r", ch );
		return;
	   }
	   else
	   {
		send_to_char( "Not a spell or skill.\n\r", ch );
		return;
	   }
	   send_to_char( "Ok.\n\r", ch );
	   return;
	}
	if ( !str_cmp( arg2, "target" ) )
	{
	    int x = get_starget( argument );

	    if ( x == -1 )
		send_to_char( "Not a valid target type.\n\r", ch );
	    else
	    {
		skill->target = x;
		send_to_char( "Ok.\n\r", ch );
	    }
	    return;
	}
	if ( !str_cmp( arg2, "minpos" ) )
	{
	    skill->minimum_position = URANGE( POS_DEAD, atoi( argument ), POS_DRAG );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "minlevel" ) )
	{
	    skill->min_level = URANGE( 1, atoi( argument ), MAX_LEVEL );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "sector") )
	{
	    char tmp_arg[MAX_STRING_LENGTH];

	    while ( argument[0] != '\0' )
	    {
	       argument = one_argument( argument, tmp_arg );
	       value = get_secflag( tmp_arg );
	       if ( value < 0 || value > MAX_SECFLAG )
	          ch_printf( ch, "Unknown flag: %s\n\r", tmp_arg );
	       else
	          TOGGLE_BIT( skill->spell_sector, (1 << value) );
            }
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "slot" ) )
	{
	    skill->slot = URANGE( 0, atoi( argument ), 30000 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "mana" ) )
	{
	    skill->min_mana = URANGE( -50, atoi( argument ), 10000 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}

	if ( !str_cmp( arg2, "beats" ) )
	{
	    skill->beats = URANGE( 0, atoi( argument ), 120 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
        if ( !str_cmp( arg2, "radius" ) )
        {
            skill->radius = URANGE( 0, atoi( argument ), 40 );
            send_to_char( "Ok.\n\r", ch );
            return;
        }

	if ( !str_cmp( arg2, "range" ) )
	{
	    skill->range = URANGE( 0, atoi( argument ), 20 );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
        if ( !str_cmp( arg2, "powerlevel" ) )
        {
            skill->pl = URANGE( 0, atoi( argument ), 115000000 );
            send_to_char( "Ok.\n\r", ch );
            return;
        }

	if ( !str_cmp( arg2, "value" ) )
	{
	    skill->value = atoi( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "type" ) )
	{
	    skill->type = get_skill( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}


	if ( !str_cmp( arg2, "name" ) )
	{
	    DISPOSE(skill->name);
	    skill->name = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}

	if ( !str_cmp( arg2, "dammsg" ) )
	{
	    DISPOSE(skill->noun_damage);
	    if ( !str_cmp( argument, "clear" ) )
	      skill->noun_damage = str_dup( "" );
	    else
	      skill->noun_damage = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "wearoff" ) )
	{
	    DISPOSE(skill->msg_off);
	    if ( str_cmp( argument, "clear" ) )
	      skill->msg_off = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "hitchar" ) )
	{
	    if ( skill->hit_char )
	      DISPOSE(skill->hit_char);
	    if ( str_cmp( argument, "clear" ) )
	      skill->hit_char = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "hitvict" ) )
	{
	    if ( skill->hit_vict )
	      DISPOSE(skill->hit_vict);
	    if ( str_cmp( argument, "clear" ) )
	      skill->hit_vict = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "hitroom" ) )
	{
	    if ( skill->hit_room )
	      DISPOSE(skill->hit_room);
	    if ( str_cmp( argument, "clear" ) )
	      skill->hit_room = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "hitdest" ) )
	{
	    if ( skill->hit_dest )
	      DISPOSE(skill->hit_dest);
	    if ( str_cmp( argument, "clear" ) )
	      skill->hit_dest = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
        if ( !str_cmp( arg2, "keyword" ) )
        {
            if ( skill->keyword )
              DISPOSE(skill->keyword);
            if ( str_cmp( argument, "clear" ) )
              skill->keyword = str_dup( argument );
            send_to_char( "Ok.\n\r", ch );
            return;
        }

	if ( !str_cmp( arg2, "misschar" ) )
	{
	    if ( skill->miss_char )
	      DISPOSE(skill->miss_char);
	    if ( str_cmp( argument, "clear" ) )
	      skill->miss_char = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "missvict" ) )
	{
	    if ( skill->miss_vict )
	      DISPOSE(skill->miss_vict);
	    if ( str_cmp( argument, "clear" ) )
	      skill->miss_vict = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "missroom" ) )
	{
	    if ( skill->miss_room )
	      DISPOSE(skill->miss_room);
	    if ( str_cmp( argument, "clear" ) )
	      skill->miss_room = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "diechar" ) )
	{
	    if ( skill->die_char )
	      DISPOSE(skill->die_char);
	    if ( str_cmp( argument, "clear" ) )
	      skill->die_char = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "dievict" ) )
	{
	    if ( skill->die_vict )
	      DISPOSE(skill->die_vict);
	    if ( str_cmp( argument, "clear" ) )
	      skill->die_vict = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "dieroom" ) )
	{
	    if ( skill->die_room )
	      DISPOSE(skill->die_room);
	    if ( str_cmp( argument, "clear" ) )
	      skill->die_room = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "immchar" ) )
	{
	    if ( skill->imm_char )
	      DISPOSE(skill->imm_char);
	    if ( str_cmp( argument, "clear" ) )
	      skill->imm_char = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "immvict" ) )
	{
	    if ( skill->imm_vict )
	      DISPOSE(skill->imm_vict);
	    if ( str_cmp( argument, "clear" ) )
	      skill->imm_vict = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "immroom" ) )
	{
	    if ( skill->imm_room )
	      DISPOSE(skill->imm_room);
	    if ( str_cmp( argument, "clear" ) )
	      skill->imm_room = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "dice" ) )
	{
	    if ( skill->dice )
	      DISPOSE(skill->dice);
	    if ( str_cmp( argument, "clear" ) )
	      skill->dice = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "components" ) )
	{
	    if ( skill->components )
	      DISPOSE(skill->components);
	    if ( str_cmp( argument, "clear" ) )
	      skill->components = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	if ( !str_cmp( arg2, "teachers" ) )
	{
	    if ( skill->teachers )
	      DISPOSE(skill->teachers);
	    if ( str_cmp( argument, "clear" ) )
	      skill->teachers = str_dup( argument );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
	do_sset( ch, "" );
	return;
    }
    else
    {
       char arg3[MSL];
    argument = one_argument( argument, arg3 );
    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg3, "all" );
    sn   = 0;
    if ( !fAll && ( sn = sskill_lookup( ch, arg3 ) ) < 0 )
    {
     if ( !fAll && ( sn = askill_lookup( ch, arg3 ) ) < 0 )
     {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
     }
    }
    if ( ( sn = sskill_lookup( ch, arg3 ) ) >= 0 )
    ability = FALSE;
    else if ( ( sn = askill_lookup( ch, arg3 ) ) >= 0 )
    ability = TRUE;
    /*
     * Snarf the value.
     */
    if ( !is_number( argument ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( argument );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < top_sskill; sn++ )
	{
            /* Fix by Narn to prevent ssetting skills the player shouldn't have. */ 
	    if ( sskill_table[sn]->name 
            && ( victim->exp >= sskill_table[sn]->pl ) ) 
	    {
		  victim->pcdata->learned[sn] = URANGE( 0, value, 100 );
	    }
	}
        for ( sn = 0; sn < top_askill; sn++ )
        {
            /* Fix by Narn to prevent ssetting skills the player shouldn't have. */
            if ( askill_table[sn]->name
            && ( victim->exp >= askill_table[sn]->pl ) )
            {
                  victim->pcdata->learned2[sn] = URANGE( 0, value, 100 );
            }
        }

    }
    else
    {
        if ( !ability )
	victim->pcdata->learned[sn] = value;
        else if ( ability )
	victim->pcdata->learned2[sn] = value;
    }
  }
    return;
}


void learn_from_success( CHAR_DATA *ch, int sn )
{
    int adept, learn, percent, chance;

    if ( IS_NPC(ch) || ch->pcdata->learned[sn] <= 0 )
	return;
    adept = 50 + (get_curr_wis(ch)*2);
    if ( ch->pcdata->learned[sn] < adept )
    {
	chance = ch->pcdata->learned[sn];
	percent = number_percent();
        if ( percent < 30 )
        return;
        learn = 2;
        learn += get_curr_int(ch)/5;
        if (xIS_SET(ch->powerup, PLR_BOOST))
        learn += number_range(1, 3);

	ch->pcdata->learned[sn] = UMIN( adept, ch->pcdata->learned[sn] + learn );
    }
}


void learn_from_success2( CHAR_DATA *ch, int sn )
{
    int adept, learn, percent, chance;

    if ( IS_NPC(ch) || ch->pcdata->learned2[sn] <= 0 )
        return;
    adept = 50 + (get_curr_wis(ch)*2);
    if ( ch->pcdata->learned2[sn] < adept )
    {
        chance = ch->pcdata->learned2[sn];
        percent = number_percent();
        if ( percent < 30 )
        return;
        learn = 2;
        learn += get_curr_int(ch)/5;
        if (xIS_SET(ch->powerup, PLR_BOOST))
        learn += number_range(1, 3);

        ch->pcdata->learned2[sn] = UMIN( adept, ch->pcdata->learned2[sn] + learn );
    }
}



void learn_from_failure( CHAR_DATA *ch, int sn )
{
    int adept;

    if ( IS_NPC(ch) || ch->pcdata->learned[sn] <= 0 )
	return;
    adept = 50 + (get_curr_wis(ch)*2);
    ch->pcdata->learned[sn] += number_range( 1, 3 );
    if ( ch->pcdata->learned[sn] > adept )
    ch->pcdata->learned[sn] = adept;

}

void learn_from_failure2( CHAR_DATA *ch, int sn )
{
    int adept;

    if ( IS_NPC(ch) || ch->pcdata->learned2[sn] <= 0 )
        return;
    adept = 50 + (get_curr_wis(ch)*2);
    ch->pcdata->learned2[sn]++;
    if ( ch->pcdata->learned2[sn] > adept )
    ch->pcdata->learned2[sn] = adept;

}


void do_aid( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int percent, lvl;


    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Aid whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) || IS_NPC(ch) )                        /* Gorog */
    {
	send_to_char( "Not on mobs neither ends.\n\r", ch );
	return;
    }
    lvl = ch->pcdata->skill_level[gsn_aid];
    if ( ch->mount )
    {
	send_to_char( "You can't do that while mounted.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Aid yourself?\n\r", ch );
	return;
    }

    if ( victim->position > POS_STUNNED )
    {
	act( AT_PLAIN, "$N doesn't need your help.", ch, NULL, victim,
	     TO_CHAR);
	return;
    }

    if ( victim->hit <= -6 )
    {
	act( AT_PLAIN, "$N's condition is beyond your aiding ability.", ch,
	     NULL, victim, TO_CHAR);
	return;
    }

    percent = number_percent( ) - (get_curr_lck(ch) - 13);
    waitstate( ch, "aid" );
    consume_ki( ch, "aid" );

    if ( !can_use_skill( ch, percent, gsn_aid ) )
    {
	send_to_char( "You fail.\n\r", ch );
	learn_from_failure( ch, gsn_aid );
	return;
    }

       act( AT_GREEN, "You aid $N!",  ch, NULL, victim, TO_CHAR    );
       act( AT_GREEN, "$n aids $N!",  ch, NULL, victim, TO_NOTVICT );
       if ( victim->hit < 1 )
         victim->hit = 1;
       update_pos( victim );
       act( AT_GREEN, "$n aids you!", ch, NULL, victim, TO_VICT    );

    learn_from_success( ch, gsn_aid );
    return;
}



bool check_anticipate( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chances;

    if ( !IS_AWAKE(victim) )
        return FALSE;


        chances  = (get_curr_int(victim) - get_curr_int(ch) );

    if ( chances == 0 )
    return FALSE;

    if ( !chance( victim, chances ) )
    return FALSE;

    if ( !IS_NPC(victim) && !xIS_SET( victim->pcdata->flags, PCFLAG_SUPER) )
    act( AT_SKILL, "You anticipate $n's attack.\n\r&Y$n misses you. [0]", ch, NULL, victim, TO_CHAR    );
    if ( !IS_NPC(ch) && !xIS_SET( ch->pcdata->flags, PCFLAG_SUPER) )
    act( AT_SKILL, "$N anticipates your attack.\n\r&WYou miss $N. [0]", ch, NULL, victim, TO_VICT    );

    return TRUE;
}

/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chances;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) && !xIS_SET(victim->defenses, DFND_DODGE) )
      return FALSE;

        chances  = (get_curr_dex(victim) - get_curr_dex(ch) );

    if ( chances == 0 )
    return FALSE;
    if ( !chance( victim, chances ) )
    return FALSE;

    if ( !IS_NPC(victim) && !xIS_SET( victim->pcdata->flags, PCFLAG_SUPER)  )   
    act( AT_SKILL, "You dodge $n's attack.", ch, NULL, victim, TO_VICT);
    if ( !IS_NPC(ch) && !xIS_SET( ch->pcdata->flags, PCFLAG_SUPER) ) 
    act( AT_SKILL, "$N dodges your attack.", ch, NULL, victim, TO_CHAR);

    damage( ch, victim, 0, TYPE_HIT);

    return TRUE;
}

bool check_grip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) && !xIS_SET(victim->defenses, DFND_GRIP) )
      return FALSE;

    if ( IS_NPC(victim) )
	chance  = UMIN( 60, 2 * victim->level );
    else
        chance  = (int) (LEARNED(victim, gsn_grip) / 2);

    /* Consider luck as a factor */
    chance += (2 * (get_curr_lck(victim) - 13 ) );

    if ( number_percent( ) >= chance + victim->level - ch->level )
    {
	learn_from_failure( victim, gsn_grip );
        return FALSE;
    }
    act( AT_SKILL, "You evade $n's attempt to disarm you.", ch, NULL, victim, TO_VICT    );
    act( AT_SKILL, "$N holds $S weapon strongly, and is not disarmed.", 
  	 ch, NULL, victim, TO_CHAR    );
    learn_from_success( victim, gsn_grip );
    return TRUE;
}

bool check_illegal_psteal( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if (!IS_NPC (victim) && !IS_NPC(ch))
    {
	if ( ( !xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY )
	|| ch->level - victim->level > 10
	|| !xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
	&& ( ch->in_room->vnum < 29 || ch->in_room->vnum > 43 )
	&& ch != victim )
	{
	    /* 
	    sprintf( log_buf, "%s illegally stealing from %s at %d",
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->name,
		victim->in_room->vnum );
	    log_string( log_buf );
	    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
	    */
	    return TRUE;
	}
    }
    return FALSE;
}

int getsamemap( int map );
void do_sensemap( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  char buf[MSL];
  char buf2[MSL];
  char buf3[MSL];
  char buf4[MSL];
  bool found=FALSE;

       send_to_char( "\n\r&WYou close your eyes as you try to sense the powerlevels around you.\n\r", ch);
       send_to_char( "&z----------------------------------------------------\n\r", ch);
        for ( d = first_descriptor; d; d = d->next )
            if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
            && ( victim = d->character ) != NULL
            &&   !IS_NPC(victim)
            &&   victim->in_room
            &&   victim->in_room->area == ch->in_room->area
            &&   victim != ch
            &&   can_see( ch, victim, TRUE )
            && ( (IS_NPC(victim) && IS_ACT_FLAG( victim, ACT_ONMAP )) || (!IS_NPC(victim) && IS_PLR_FLAG( victim, PLR_ONMAP)))
            && getsamemap(ch->map) == getsamemap(victim->map) )
            {

                if (victim->y == ch->y && victim->x == ch->x && ch->map == victim->map)
                 continue;
                else
                {
                  sprintf( buf, " ");
                  sprintf( buf4, " ");
                 if ( victim->y < ch->y && victim->x == ch->x)            
                   sprintf( buf, "north");
                 else if ( victim->y > ch->y && victim->x == ch->x)
                   sprintf( buf, "south");
                 else if ( victim->x > ch->x && victim->y == ch->y)
                   sprintf( buf, "east" );
                 else if ( victim->x < ch->x && victim->y == ch->y)
                   sprintf( buf, "west" );
                 else if ( victim->y < ch->y && victim->x > ch->x )
                   sprintf( buf, "northeast" );
                 else if ( victim->y < ch->y && victim->x < ch->x )
                   sprintf( buf, "northwest" );
                 else if ( victim->y > ch->y && victim->x > ch->x )
                   sprintf( buf, "southeast" );
                 else if ( victim->y > ch->y && victim->x < ch->x )
                   sprintf( buf, "southwest" );
                }

                sprintf( buf2, " " );
                if ( victim->exp < 1000000 )
                sprintf(buf2, "&Rfaint");
                else if ( victim->exp < 5000000 )
                sprintf(buf2, "&Rweak");
                else if ( victim->exp < 50000000 )
                sprintf(buf2, "&Rstrong");
                else
                sprintf(buf2, "&Rvery strong");
                
                if ( ch->map == getmapabove(victim->map))
                sprintf(buf4, "below");
                else if (ch->map == getmapbelow(victim->map))
                sprintf(buf4, "above");
                else if (get_mapabove(ch->map) == TRUE )
                sprintf(buf4, "below");
                else if (get_mapbelow(ch->map) == TRUE )
                sprintf(buf4, "above");
                if ( ch->map == victim->map)
                 sprintf( buf3, "&YJudging from the &R%s&Y powerlevel, you think &w%s&Y is %s of here.&w\n\r", buf2, victim->name, buf);
                else if ( ch->x != victim->x || ch->y != victim->y )
                 sprintf( buf3, "&YJudging from the &R%s&Y powerlevel, you think &w%s&Y is %s and %s of here.&w\n\r", buf2, victim->name, buf, buf4);
                else
                 sprintf( buf3, "&YJudging from the &R%s&Y powerlevel, you think &w%s&Y is %s of here.&w\n\r", buf2, victim->name, buf4 );
                if (!IS_ANDROID(victim) && victim->exp > 50000)
                {
                  if (!xIS_SET(victim->act, PLR_WIZINVIS))
                  {
                    send_to_char( buf3, ch);
                    found = TRUE;
                  }
                }
            }
        if ( !found )
            send_to_char( "None\n\r", ch );





}

void do_search( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  bool found;
  int x, y, map, dir, dist;
  double angle;
  double dist1;
  x = ch->x;
  y = ch->y;
  map = ch->map;
  dist1 = 20;
  dir = 1;
  if ( IS_NPC(ch))
  {
    send_to_char( "Mobs can't use this.\n\r", ch);
    return;
  }
  if ( !xIS_SET(ch->act, PLR_ONMAP) )
  {
    send_to_char( "You can only use this command outside.\n\r", ch);
    return;
  }
  found = FALSE;
  send_to_char("\n\r&RSearching your surroundings....\n\r&z-------------------------------------------------\n\r", ch);

    for ( obj = last_object; obj; obj = gobj_prev )
    {
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
        if ( !xIS_SET(obj->extra_flags, ITEM_ONMAP) )
        continue;
        if ( ch->map != obj->map )
        continue;   
        angle = calc_angle( ch->x, ch->y, obj->x, obj->y, &dist1 );

      dist = distance( ch->x, ch->y, obj->x, obj->y );

      /*
       * Save the math if it's too far away anyway
       */
      if( dist <= 20 )
      {
         found = TRUE;
         if( angle == -1 )
            dir = -1;
         else if( angle >= 360 )
            dir = DIR_NORTH;
         else if( angle >= 315 )
            dir = DIR_NORTHWEST;
         else if( angle >= 270 )
            dir = DIR_WEST;
         else if( angle >= 225 )
            dir = DIR_SOUTHWEST;
         else if( angle >= 180 )
            dir = DIR_SOUTH;
         else if( angle >= 135 )
            dir = DIR_SOUTHEAST;
         else if( angle >= 90 )
            dir = DIR_EAST;
         else if( angle >= 45 )
            dir = DIR_NORTHEAST;
         else if( angle >= 0 )
            dir = DIR_NORTH;

         if( dir == -1 )
            ch_printf( ch, "&cNearby &O- &C %s\n\r", obj->short_descr);
         else
            ch_printf( ch, "&c%s &O- &C%s\n\r", capitalize(dir_name[dir]), obj->short_descr );

      }       
    }
    if ( !found )
    send_to_char( "Nothing found.\n\r", ch);

  send_to_char("&z-------------------------------------------------\n\r", ch);

}

void do_sense( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in_room;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *inroom;
    sh_int dir = -1;
    sh_int dist;
    sh_int max_dist = 4;
    char  buf[MIL];
    char  buf2[MIL];
    location = ch->in_room;
 
    if ( IS_NPC(ch))
    return;
      if (IS_PLR_FLAG( ch, PLR_ONMAP ) )
      {
        do_sensemap( ch, "" );
        return;
      }

    if ( argument[0] == '\0' )
    {
      was_in_room = ch->in_room;
      location = ch->in_room;
      send_to_char("\n\r&RScanning Surroundings...&G\n\r", ch );
      for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
      {
        if ( IS_SET(pexit->exit_info, EX_CLOSED) || location == pexit->to_room )
        {
            if ( IS_SET(pexit->exit_info, EX_SECRET)
            ||   IS_SET(pexit->exit_info, EX_DIG) )
                act( AT_GREY, "Your cant sense any further.", ch,
                    NULL, NULL, TO_CHAR );
            else
                act( AT_GREY, "Your cant sense any further.", ch,
                    NULL, NULL, TO_CHAR );
            break;
        }
        if ( room_is_private( ch, pexit->to_room )
        &&   ch->level < LEVEL_GREATER && !IN_HOUSE(ch) )
        {
            act( AT_GREY, "Your cant sense any further.", ch,
                NULL, NULL, TO_CHAR );
            break;
        }
        char_from_room( ch );
        char_to_room( ch, pexit->to_room );
        set_char_color( AT_GREEN, ch );
        send_to_char( ch->in_room->name, ch );
        send_to_char( "\n\r", ch );
        for ( inroom = ch->in_room->first_person; inroom; inroom = inroom->next_in_room )
        {
          if (inroom == ch)
          continue;
         if ( inroom->exp < 1000000 )
         sprintf(buf2, "&Rfaint");
         else if ( inroom->exp < 5000000 )
         sprintf(buf2, "&Rweak");
         else if ( inroom->exp < 50000000 )
         sprintf(buf2, "&Rstrong");
         else 
         sprintf(buf2, "&Rvery strong");
       
          sprintf( buf, "&YJudging from the &R%s&Y powerlevel, you think &w%s&Y is here.&w\n\r", buf2, inroom->name );
          if (!IS_ANDROID(inroom))
          {
           if (!xIS_SET(inroom->act, PLR_WIZINVIS))
            if (!IN_NOSENSE(inroom))
             send_to_char( buf, ch);
          }
	}
      }         
      char_from_room( ch );
      char_to_room( ch, was_in_room );
      return;
    }
    else if ( ( dir = get_door( argument ) ) == -1 )
    {
	send_to_char( "Sense in WHAT direction?\n\r", ch );
	return;
    }
    else
    {
    was_in_room = ch->in_room;
    act( AT_GREY, "You close your eyes as you sense $t...", ch, dir_name[dir], NULL, TO_CHAR );
    act( AT_GREY, "$n tries to pick up new powerlevels from $t.", ch, dir_name[dir], NULL, TO_ROOM );


    if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
    {
	act( AT_GREY, "You can't see $t.", ch, dir_name[dir], NULL, TO_CHAR );
	return;
    }
    if (IS_SET(pexit->exit_info, EX_SECRET))
    {
        act( AT_GREY, "You can't see $t.", ch, dir_name[dir], NULL, TO_CHAR );
        return;
    }

/*    if ( location == pexit->to_room )
    {
        send_to_char( "Trouble confuses you as it looks the same.\n\r", ch );
        return;
    }*/
    if ( IS_NAMEK(ch) || IS_HUMAN(ch) )
    {
     max_dist++;
     max_dist++;
     max_dist++;
    }

    for ( dist = 1; dist <= max_dist; )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) || location == pexit->to_room )
	{
	    if ( IS_SET(pexit->exit_info, EX_SECRET)
	    ||   IS_SET(pexit->exit_info, EX_DIG) )
		act( AT_GREY, "Your cant sense $t any further.", ch,
		    dir_name[dir], NULL, TO_CHAR );
	    else
		act( AT_GREY, "Your cant sense $t any further.", ch, 
		    dir_name[dir], NULL, TO_CHAR );
	    break;
	}
	if ( room_is_private( ch, pexit->to_room )
	&&   ch->level < LEVEL_GREATER && !IN_HOUSE(ch) )
	{
	    act( AT_GREY, "Your cant sense $t any further.", ch, 
		dir_name[dir], NULL, TO_CHAR );
	    break;
	}
	char_from_room( ch );
	char_to_room( ch, pexit->to_room );    
       /* if ( location == ch->in_room )
        {
          send_to_char( "Trouble confuses you as it looks the same.\n\r", ch );
          return;
        }*/
        set_char_color(AT_GREEN, ch );
	send_to_char( ch->in_room->name, ch );
	send_to_char( "\n\r", ch );
        for ( inroom = ch->in_room->first_person; inroom; inroom = inroom->next_in_room )
        {
          if (inroom == ch)
          continue;
          if (IN_NOSENSE(inroom))
          continue;
         if ( inroom->exp < 1000000 )
         sprintf(buf2, "&Rfaint");
         else if ( inroom->exp < 5000000 )
         sprintf(buf2, "&Rweak");
         else if ( inroom->exp < 50000000 )
         sprintf(buf2, "&Rstrong");
         else
         sprintf(buf2, "&Rvery strong");

          sprintf( buf, "&YJudging from the &R%s&Y powerlevel, you think &w%s&Y is here.&w\n\r", buf2, inroom->name );
          if (!IS_ANDROID(inroom))
          {
           if(!xIS_SET(inroom->act, PLR_WIZINVIS))
            if (!IN_NOSENSE(inroom))
             send_to_char( buf, ch);
          } 
       }
            dist++;
	if ( dist >= max_dist )
	{
	    act( AT_GREY, "You can't pick up any more powerlevels coming from $t.", ch, dir_name[dir], NULL, TO_CHAR );
	    break;
	}
	if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
	{
	    act( AT_GREY, "You can't pick up any more powerlevels coming from $t.", ch, 
		dir_name[dir], NULL, TO_CHAR );
	    break;
	}
    }
  
    char_from_room( ch );
    char_to_room( ch, was_in_room );
   }
    return;
}


/*
 * Basically the same guts as do_scan() from above (please keep them in
 * sync) used to find the victim we're firing at.	-Thoric
 */
CHAR_DATA *scan_for_victim( CHAR_DATA *ch, EXIT_DATA *pexit, char *name )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *was_in_room;
    sh_int dist, dir;
    sh_int max_dist = 8;

    if ( !pexit )
	return NULL;
 
    was_in_room = ch->in_room;
    if ( IS_VAMPIRE(ch) && time_info.hour < 21 && time_info.hour > 5 )
	max_dist = 1;

    if ( ch->level < 50 ) --max_dist;
    if ( ch->level < 40 ) --max_dist;
    if ( ch->level < 30 ) --max_dist;

    for ( dist = 1; dist <= max_dist; )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    break;

	if ( room_is_private( ch, pexit->to_room )
	&&   ch->level < LEVEL_GREATER && !IN_HOUSE(ch) )
	    break;

	char_from_room( ch );
	char_to_room( ch, pexit->to_room );    

	if ( (victim=get_char_room(ch, name)) != NULL )
	{
	    char_from_room(ch);
	    char_to_room(ch, was_in_room);
	    return victim;
	}

	switch( ch->in_room->sector_type )
	{
	    default: dist++; break;
	    case SECT_AIR:
		if ( number_percent() < 80 ) dist++; break;
	    case SECT_INSIDE:
	    case SECT_FIELD:
	    case SECT_UNDERGROUND:
		dist++; break;
	    case SECT_FOREST:
	    case SECT_CITY:
	    case SECT_DESERT:
	    case SECT_HILLS:
		dist += 2; break;
	    case SECT_MOUNTAIN:
	    case SECT_UNDERWATER:
	    case SECT_OCEANFLOOR:
		dist += 4; break;
	}

	if ( dist >= max_dist )
	    break;

	dir = pexit->vdir;
	if ( (pexit=get_exit(ch->in_room, dir)) == NULL )
	    break;
    }
  
    char_from_room(ch);
    char_to_room(ch, was_in_room);

    return NULL;
}

/*
 * Search inventory for an appropriate projectile to fire.
 * Also search open quivers.					-Thoric
 */
OBJ_DATA *find_projectile( CHAR_DATA *ch, int type )
{
    OBJ_DATA *obj, *obj2;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
	if ( can_see_obj(ch, obj) )
	{
	    if ( obj->item_type == ITEM_QUIVER && !IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		for ( obj2 = obj->last_content; obj2; obj2 = obj2->prev_content )
		{
		    if ( obj2->item_type == ITEM_PROJECTILE
		    &&   obj2->value[3] == type )
			return obj2;
		}
	    }
	    if ( obj->item_type == ITEM_PROJECTILE && obj->value[3] == type )
		return obj;
	}
    }

    return NULL;
}


ch_ret spell_attack( int, int, CHAR_DATA *, void * );

/*
 * Perform the actual attack on a victim			-Thoric
 */
ch_ret ranged_got_target( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *weapon,
	OBJ_DATA *projectile, sh_int dist, sh_int dt, char *stxt, sh_int color )
{
    if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
    {
	/* safe room, bubye projectile */
	if ( projectile )
	{
	    ch_printf(ch, "Your %s is blasted from existance by a godly presense.",
		myobj(projectile) );
	    act( color, "A godly presence smites $p!", ch, projectile, NULL, TO_ROOM );
	    extract_obj(projectile);
	}
	else
	{
	    ch_printf(ch, "Your %s is blasted from existance by a godly presense.",
		stxt );
	    act( color, "A godly presence smites $t!", ch, aoran(stxt), NULL, TO_ROOM );
	}
	return rNONE;
    }

    if ( IS_NPC(victim) && xIS_SET(victim->act, ACT_SENTINEL)
    &&   ch->in_room != victim->in_room )
    {
	/*
	 * letsee, if they're high enough.. attack back with fireballs
	 * long distance or maybe some minions... go herne! heh..
	 *
	 * For now, just always miss if not in same room  -Thoric
	 */

	if ( projectile )
	{

	    /* 50% chance of projectile getting lost */
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
	}
	return damage( ch, victim, 0, dt );
    }

    if ( number_percent() > 50 || (projectile && weapon
    &&   can_use_skill(ch, number_percent(), gsn_scan)) )
    {
	if ( projectile )
	    global_retcode = projectile_hit(ch, victim, weapon, projectile, dist );
	else
	    global_retcode = spell_attack( dt, ch->level, ch, victim );
    }
    else
    {
	learn_from_failure( ch, gsn_scan );
	global_retcode = damage( ch, victim, 0, dt );

	if ( projectile )
	{
	    /* 50% chance of getting lost */
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
	}
    }
    return global_retcode;
}


bool can_use_skill( CHAR_DATA *ch, int percent, int gsn )
{
   bool check = FALSE;

   if ( IS_NPC(ch) && percent < 85 ) check = TRUE;
   else if ( !IS_NPC(ch) && percent <= ch->pcdata->learned[gsn]  ) check = TRUE;

   if (gsn >= 100)
   check = TRUE;
   return check;
}

bool can_use_skill2( CHAR_DATA *ch, int percent, int gsn )
{
   bool check = FALSE;

   if ( IS_NPC(ch) && percent < 85 ) check = TRUE;
   else if ( !IS_NPC(ch) && percent <= ch->pcdata->learned2[gsn]  ) check = TRUE;

   if (gsn >= 100)
   check = TRUE;
   return check;
}

