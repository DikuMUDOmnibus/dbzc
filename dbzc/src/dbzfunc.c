/******************************************************************
 *                [D]ragon [B]all [Z] [C]hronicles                *
 *----------------------------------------------------------------*
 * DBZB, DBZDL, DBZC versions done by: Seventeen and Vegeta       *
 *----------------------------------------------------------------*
 * All modifications made by Seventeen and Vegeta are sole        *
 * property of them. Credit must be given if any part is ripped   *
 * or used from this version or any other version of DBZC.        *
 *****************************************************************/

/**************************************************************************
*     Dragon Ball Z Skills, created by Seventeen and Vegeta               *
*     This file was created for all the other functions in Powerups and   *
*     dbzskills								  *
**************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


bool can_userace( CHAR_DATA *ch, int gsn );

char *hesheit( short gender )
{
        switch( gender )
        {
          case 1:
          return  "he";
          case 2:
          return  "she";
          default:
          return  "it";
        }
  return "it";
}

char *himherit( short gender )
{
        switch( gender )
        {
          case 1:
          return  "him";
          case 2:
          return  "her";
          default:
          return  "it";
        }
  return "it";
}

long long int max_pl_char( CHAR_DATA *ch )
{
   int be;

   be = ch->pcdata->learned[gsn_be] * 100000;
   return be;

}

void do_null( CHAR_DATA *ch, char *argument )
{
   send_to_char( "Huh?\n\r", ch);
   return;
}

char *hisherit( short gender )
{
        switch( gender )
        {
          case 1:
          return  "his";
          case 2:
          return  "her";
          default:
          return  "it";
        }
  return "it";
}

char * get_aura_color( sh_int aura )
{
	switch( aura )
	{
		case AURA_BLACK:
			return "&z";
                case AURA_RED:
                        return "&R";
                case AURA_BLUE:
                        return "&C";
                case AURA_YELLOW:
                        return "&Y";
                case AURA_GREEN:
                        return "&G";
                case AURA_WHITE:
                        return "&W";
                case AURA_DRED:
                        return "&r";
                case AURA_DBLUE:
                        return "&b";
                case AURA_BROWN:
                        return "&O";
                case AURA_CYAN:
                        return "&c";
                case AURA_DGREEN:
                        return "&g";
                case AURA_PURPLE:
                        return "&p";
                case AURA_PINK:
 	                return "&P";
                default:
                        return "&W";
	}
  return "&W";      
}

char * get_aura_name( sh_int aura )
{
        switch( aura )
        {
                case AURA_BLACK:
                        return "black";
                case AURA_RED:
                        return "red";
                case AURA_BLUE:
                        return "blue";
                case AURA_YELLOW:
                        return "yellow";
                case AURA_GREEN:
                        return "green";
                case AURA_WHITE:
                        return "white";
                case AURA_DRED:
                        return "dark red";
                case AURA_DBLUE:
                        return "dark blue";
                case AURA_BROWN:
                        return "orange";
                case AURA_CYAN:
                        return "cyan";
                case AURA_DGREEN:
                        return "dark green";
                case AURA_PURPLE:
                        return "purple";
                case AURA_PINK:
                        return "pink";
                default:
                        return "white";
        }
  return "white";
}


int convert_pos( int number )
{
   int temp;
   if (number > 0)
   return number;
   temp = number * number;
   temp = sqrt(temp);  

   return temp;
}

bool is_clanned( CHAR_DATA *ch )
{
  if ( IS_NPC(ch))
  return FALSE;

  if (!ch->pcdata->clan)
  return FALSE;

 return TRUE;
}


void release_ki( CHAR_DATA *ch )
{
   if ( xIS_SET(ch->powerup, KI_EBALL ) )
    xREMOVE_BIT(ch->powerup, KI_EBALL);
   if ( xIS_SET(ch->powerup, KI_SPHERE ) )
    xREMOVE_BIT(ch->powerup, KI_SPHERE);
   if ( xIS_SET(ch->powerup, KI_DDD ) )
    xREMOVE_BIT(ch->powerup, KI_DDD);
   if ( xIS_SET(ch->powerup, KI_DISK ) )
    xREMOVE_BIT(ch->powerup, KI_DISK);
   if ( xIS_SET(ch->powerup, KI_BAR ) )
    xREMOVE_BIT(ch->powerup, KI_BAR);
   if ( xIS_SET(ch->powerup, KI_DONUT ) )
    xREMOVE_BIT(ch->powerup, KI_DONUT);
   if ( xIS_SET(ch->powerup, KI_BOMB ) )
    xREMOVE_BIT(ch->powerup, KI_BOMB);
   if ( xIS_SET(ch->powerup, KI_SHIELD ) )
    xREMOVE_BIT(ch->powerup, KI_SHIELD);
   if ( xIS_SET(ch->powerup, KI_BARRICADE ) )
    xREMOVE_BIT(ch->powerup, KI_BARRICADE);

  return;
}
bool controlling_ki( CHAR_DATA *ch )
{

   if ( xIS_SET(ch->powerup, KI_EBALL )
   ||   xIS_SET(ch->powerup, KI_SPHERE)
   ||   xIS_SET(ch->powerup, KI_DDD)
   ||   xIS_SET(ch->powerup, KI_DISK)
   ||   xIS_SET(ch->powerup, KI_BAR)
   ||   xIS_SET(ch->powerup, KI_DONUT)
   ||   xIS_SET(ch->powerup, KI_BOMB)
   ||   xIS_SET(ch->powerup, KI_SHIELD)
   ||   xIS_SET(ch->powerup, KI_BARRICADE))
   return TRUE;

 return FALSE;
}

long long int calc_power( CHAR_DATA *ch )
{
   if (xIS_SET(ch->powerup, PU_SUPPRESS))
       return ( (ch->suppress) * 100/ 2000000000 );
   else
       return ( (ch->exp * ch->mod) * 100/ 2000000000);

 return -1;
}


void destroy_area( CHAR_DATA *victim )
{
   sh_int chance = number_range( 0, 5 );

   if ( chance == 2 )
   damage_area( victim->map, victim->x, victim->y);      

  return;
}

void warcry( CHAR_DATA *ch, CHAR_DATA *victim )
{
  char buf[MSL];
   if ( ch->position != POS_FIGHTING  )
   {
    if ( !IS_NPC(victim))
    {
      sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
      if ( IS_PKILL(victim) || victim->exp >= 5000000)
        do_wartalk( victim, buf );
      else
        do_yell( victim, buf );
    }
   }
}


bool is_fused( CHAR_DATA *ch )
{
   if (xIS_SET(ch->act, PLR_FUSED))
   {
     send_to_char( "You can't do that while fused with someone.\n\r", ch);
     return TRUE;
   }
  return FALSE;
}

void clear_mods( CHAR_DATA *ch )
{
  ch->mod_str = ch->mod_int = ch->mod_wis = ch->mod_dex = ch->mod_cha = ch->mod_lck = ch->mod_con = 0;
  ch->mod = ch->kimod = 1;
}
void clear_mutateflags( CHAR_DATA *ch )
{
  xREMOVE_BIT(ch->powerup, PU_MUTATE1 );
  xREMOVE_BIT(ch->powerup, PU_MUTATE2 );
  xREMOVE_BIT(ch->powerup, PU_MUTATE3 );
  xREMOVE_BIT(ch->powerup, PU_MUTATE4 );
  xREMOVE_BIT(ch->powerup, PU_MONSTER );
}

/* Messages used in Attacks */
void show_blockmsg( CHAR_DATA *ch, CHAR_DATA *victim )
{
      if ( !ch || !victim )
      {
          bug( "Null Victim or Ch in Showblock_msg");
          return;
      }
      if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "You block $n's attack." , ch, NULL, victim, TO_VICT );
      if (!IS_NPC(ch) && (xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "$N blocks your attack.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$N blocks $n's attack.", ch, NULL, victim, TO_NOTVICT );

 return;
}

/* Messages used in Attacks */
void show_bbmsg( CHAR_DATA *ch, CHAR_DATA *victim )
{
      if ( !ch || !victim )
      {
          bug( "Null Victim or Ch in Showbluebarrier_msg");
          return;
      }
      if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
      act( AT_BLUE, "$N's energy attack is partially absorbed by your blue barrier chip." , ch, NULL, victim, TO_VICT );
      if (!IS_NPC(ch) && (xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
      act( AT_BLUE, "Your energy attack is partially absorbed by $N's blue barrier chip.", ch, NULL, victim, TO_CHAR );
      act( AT_BLUE, "$n's energy attack is partially absorbed by $N's blue barrier chip.", ch, NULL, victim, TO_NOTVICT );

 return;
}

void planet_support( CHAR_DATA *ch )
{
   CLAN_DATA *clan, *isclan;

   clan = ch->pcdata->clan;
   isclan = get_clan_short( ch->in_room->area->governed_by );

    if ( isclan == clan )
    {
       ch->in_room->area->popsupport += number_range( 5, 30 );
       send_to_char( "&wIntimidation towards your Organization has increased.\n\rPopular support for your organization increases.\n\r", ch );
    }
    else
    {
       ch->in_room->area->popsupport -=  number_range( 5, 30 );
       send_to_char( "&wIntimidation towards your Organization has increased.\n\rPopular support for the current organization decreases.\n\r", ch );
    }

    if ( ch->in_room->area->popsupport > 100 )
        ch->in_room->area->popsupport = 100;
    if ( ch->in_room->area->popsupport < -100 )
        ch->in_room->area->popsupport = -100;
     fold_area( ch->in_room->area, ch->in_room->area->filename, FALSE, FALSE );

}

void show_lckmsg( CHAR_DATA *ch, CHAR_DATA *victim)
{
      if ( !ch || !victim )
      {
          bug( "Null Victim or Ch in Showblock_msg");
          return;
      }
//      if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "$n hits you in the right place." , ch, NULL, victim, TO_VICT );
//      if (!IS_NPC(ch) && (xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "You hit $N in the right place.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$n hits $N in the right place.", ch, NULL, victim, TO_NOTVICT );
  return;
}

void show_conmsg( CHAR_DATA *ch, CHAR_DATA *victim )
{
      if ( !ch || !victim )
      {
          bug( "Null Victim or Ch in Showblock_msg");
          return;
      }
      if (!IS_NPC(victim) && (!xIS_SET(victim->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "You do not get much damage because of your well built body." , victim, NULL, ch, TO_CHAR );
      if (!IS_NPC(ch) && (xIS_SET(ch->pcdata->flags, PCFLAG_SUPER)))
      act( AT_GREEN, "$N doesn't get much damage because of $S well built body.", ch, NULL, victim, TO_CHAR );
      act( AT_GREEN, "$N doesn't get much damage because of $S well built body.", ch, NULL, victim, TO_NOTVICT );
   return;
}
int pl_dmg( CHAR_DATA *ch, CHAR_DATA *victim )
{
   long long int pl = 0;
   int dam = 0;

   if (get_curr_pl(ch) > get_curr_pl(victim))
   {
      pl = get_curr_pl(ch) / get_curr_pl(victim);
      dam += pl;
   }
   else if (get_curr_pl(ch) < get_curr_pl(victim))
   {
      pl = get_curr_pl(victim) / get_curr_pl(ch);
      dam -= pl;
   }

  return dam;
}
int add_kidamage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
   long long int pl;
   sh_int lck, con, chance;
   pl = 0;
   chance = number_range( 0, 5 );
   if ( !ch || !victim )
   {
     bug( "Null CH or null VICTIM, add_kidamage");
     return 0;
   }

   lck = (get_curr_critical(ch) - get_curr_critical(victim));
   con = (get_curr_con(victim) - get_curr_con(ch));
   if (victim->style == STYLE_DEFENSIVE && (get_curr_pl(ch) <= get_curr_pl(victim)))
   {
      show_blockmsg( ch, victim );
      victim->mana -= (victim->max_mana / number_range( 20, 30));
   }
   if ( xIS_SET(victim->skill, SKILL_BBARRIER) )
   {
      show_bbmsg( ch, victim );
      dam *= 0.65;
   }
   if (get_curr_pl(ch) > get_curr_pl(victim))
   {
    pl = get_curr_pl(ch) / get_curr_pl(victim);
    dam += pl;
   }
   else if (get_curr_pl(ch) < get_curr_pl(victim))
   {
    pl = get_curr_pl(victim) / get_curr_pl(ch);
    dam -= pl;
   }
   if (victim->style == STYLE_DEFENSIVE)
   dam *= 0.85;
   if (ch->kimod > 0)
   dam *= ch->kimod;
    dam += sqrt( get_curr_eatk( ch ) ) / 2;
    dam -= sqrt( get_curr_edef( victim ) ) / 3;
   if (lck > 1 && chance == 2)
   {
     show_lckmsg( ch, victim );
     dam *= 1.3;
   }
   if (con > 1 && chance == 3)
   {
     show_conmsg( ch, victim );
     dam *= 0.5;
   }
   dam *= 1.5;
//   show_lckmsg( ch, victim );   
  return dam;
}
/* Used in melee attacks makes it easier to change everything */
int add_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
   long long int pl;
   int lck, con, chaextra, chance;

   pl = lck = con = chaextra = 0;
   chance = number_range( 0, 5 );

   if ( !ch || !victim )
   {
     bug( "Null CH or null VICTIM, add_damage");
     return 0;
   }
   lck = (get_curr_critical(ch) - get_curr_critical(victim));
   con = (get_curr_con(victim) - get_curr_con(ch));
   if (victim->style == STYLE_DEFENSIVE)
   {
     show_blockmsg( ch, victim );
     dam *= 0.8;
   }
   if (victim->style == STYLE_DEFENSIVE && (get_curr_pl(ch) <= get_curr_pl(victim)))
     victim->mana -= (victim->max_mana / 50);

   if (ch->leader && (ch->in_room == ch->leader->in_room))
   {
     if (victim->leader && get_curr_cha(victim->leader) > get_curr_cha(victim))
     chaextra = get_curr_cha(ch->leader) - get_curr_cha(victim->leader);
     else
     chaextra = get_curr_cha(ch->leader) - get_curr_cha(victim);
   }
   if (chaextra > 0)
   dam += chaextra / 3;
   if (get_curr_pl(ch) > get_curr_pl(victim))
   {
    pl = get_curr_pl(ch) / get_curr_pl(victim);
    dam += pl;
   }
   else if (get_curr_pl(ch) < get_curr_pl(victim))
   {
    pl = get_curr_pl(victim) / get_curr_pl(ch);
    dam -= pl;
   }
   if (xIS_SET(victim->act, PLR_SK))
   dam -= 2;
   if (xIS_SET(victim->act, PLR_MS))
   dam -= 1;
   if (xIS_SET(ch->act, PLR_FUSED))
   dam *= 1.5;

   if (xIS_SET(victim->powerup, KI_SHIELD) )
   dam *= 0.85;
   else if (xIS_SET(victim->powerup, KI_BAR) )
   dam *= 0.80;
   else if (xIS_SET(victim->powerup, KI_BARRICADE) )
   dam *= 0.75;

   if (lck > 1 && chance == 2)
   {
     show_lckmsg( ch, victim );
     dam *= 1.3;
   }
   if (con > 1 && chance == 3)
   {
     show_conmsg( ch, victim );
     dam *= 0.5;
   }
    dam += sqrt( get_curr_patk( ch ) ) / 2;
    dam -= sqrt( get_curr_pdef( victim ) ) / 3;
  return dam;
}
int check_extra( int ch, int vic, int div )
{
   int extra;

   extra = (ch - vic) / div;

   if (extra < 0 )
   extra = 0;
   return URANGE( 0, extra, 5 );
}
void consume_ki( CHAR_DATA *ch, const char *name)
{
  int ki, gsn, lvl;
  SKILLTYPE *skill;

  if ( IS_NPC(ch))
  return;
  if ( ( gsn = sskill_lookup( ch, name ) ) >= 0 )
  {
   lvl = ch->pcdata->skill_level[gsn];
   skill = sskill_table[gsn];
  }
  else if ( ( gsn = askill_lookup( ch, name ) ) >= 0 )
  {
   lvl = ch->pcdata->skill_level2[gsn];
   skill = askill_table[gsn];
  }
  else
   return;
 
    if ( skill->min_mana && skill->min_mana < 0)
    {
      ki = (skill->min_mana * skill->min_mana ); /*Since we using negative values, turn it positive*/
      ki = sqrt( ki );
      ch->mana -= (ch->max_mana / ki);
      return;
    }
    else if ( skill->min_mana )
    ch->mana -= skill->min_mana;

  return;
}

void waitstate( CHAR_DATA *ch, const char *name )
{
  SKILLTYPE *skill;
  int gsn;
  if ( ( gsn = sskill_lookup( ch, name ) ) > 0 )
   skill = sskill_table[gsn];
  else if ( ( gsn = askill_lookup( ch, name ) ) >= 0 )
   skill = askill_table[gsn];
  else
   return;

  if ( skill->beats )
  WAIT_STATE( ch, skill->beats );
  return;
}

bool check_spec( CHAR_DATA *ch )
{  
  if (xIS_SET(ch->act, PLR_SPEC))
  {
    send_to_char("&RSpectators cant do that.\n\r", ch);
    return TRUE;
  }
  return FALSE;
}

int getsamemap( int map )
{
  switch( map )
  {
 	case MAP_EARTH:
	case MAP_AEARTH:
	case MAP_BEARTH:
	  return MAP_EARTH;
        case MAP_NAMEK:
        case MAP_ANAMEK:
        case MAP_BNAMEK:
          return MAP_NAMEK;
        default:
	  return -1;
  }
 return -1;
}

CHAR_DATA *check_map( CHAR_DATA *ch, char *argument, int gsn )
{
  int x, y;
  CHAR_DATA *victim;
     if ( ( victim = get_char_world( ch, argument )) == NULL )
     {
      send_to_char( "They aren't here.\n\r", ch);
      return NULL;
     }
     if ( getsamemap(ch->map) != getsamemap(victim->map) )
     {
      send_to_char( "They must be in the same place as you..\n\r", ch);
      return NULL;
     }
     if (ch->x > victim->x)
     x = ch->x - victim->x;
     else
     x = victim->x - ch->x;
     if (ch->y > victim->y)
     y = ch->y - victim->y;
     else
     y = victim->y - ch->y;
     if ( IS_VALID_SSN( gsn ) )
     {
      if ( x > sskill_table[gsn]->radius || y > sskill_table[gsn]->radius )
      {
       send_to_char( "You are too far away.\n\r", ch);
       return NULL;
      }
     }
     else if ( IS_VALID_ASN( gsn ) )
     {
      if ( x > askill_table[gsn]->radius || y > askill_table[gsn]->radius )
      {
       send_to_char( "You are too far away.\n\r", ch);
       return NULL;
      }
     }

    return victim;
}


bool is_kistunned( CHAR_DATA *ch )
{

   if (ch->mana <= 0)
   {
    act( AT_GREY, "$n falls unconsious for the loss of ki.", ch, NULL, NULL, TO_ROOM );
    act( AT_GREY, "You fall unconsious for the loss of ki.", ch, NULL, NULL, TO_CHAR );
    if ( ch->position > POS_FIGHTING)
    {
     stop_fighting( ch, TRUE );
    }
    ch->mana = 1;
    ch->hit = -1;
    update_pos( ch );
    xREMOVE_BIT(ch->powerup, KI_EBALL);
    xREMOVE_BIT(ch->powerup, KI_BAR);
    xREMOVE_BIT(ch->powerup, KI_SPHERE);
    xREMOVE_BIT(ch->powerup, KI_DISK);
    xREMOVE_BIT(ch->powerup, KI_DDD);
    xREMOVE_BIT(ch->powerup, KI_DONUT);
    xREMOVE_BIT(ch->powerup, KI_BOMB);
    xREMOVE_BIT(ch->powerup, KI_SHIELD);
    xREMOVE_BIT(ch->powerup, KI_BARRICADE);
    return TRUE;
   }

  return FALSE;
}
bool is_awake( CHAR_DATA *ch )
{
  if (ch->position == POS_RESTING || ch->position == POS_SLEEPING || ch->position == POS_STUNNED || ch->position == POS_SITTING)
  {
    send_to_char("&RStand up first.\n\r", ch);
    return TRUE;
  }
  return FALSE;
}

int calculatelevel( CHAR_DATA *ch )
{
  int sn, sn2, count;
  sn = sn2 = count = 0;
  for ( sn = 0; sn < top_sskill; sn++ )
  {
    	if ( sskill_table[sn]->value >= 0 )
   	 count++;
  }
  for ( sn2 = 0; sn2 < top_askill; sn2++ )
  {
        if ( askill_table[sn2]->value >= 0 )
        {
		if ( can_userace( ch, sn2) == TRUE )
		count++;
        }
  }
  count *= 15;
  count /= 2;

  return count;
}

int calculatelevel2( CHAR_DATA *ch )
{
  int sn, sn2, count;
  sn = sn2 = count = 0;
  for ( sn = 0; sn < top_sskill; sn++ )
  {
        count += ch->pcdata->skill_level[sn] ;
  }
  for ( sn2 = 0; sn2 < top_askill; sn2++ )
  {
        count += ch->pcdata->skill_level2[sn];
  }
  count += ch->skill_lvl;
  return count;
}

