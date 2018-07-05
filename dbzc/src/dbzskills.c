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

unsigned char map_sector[MAP_MAX][MAX_X][MAX_Y];   /* Initializes the sector array */
unsigned char wasmap_sector[MAP_MAX][MAX_X][MAX_Y];   /* Initializes the sector array */
void update_skill( CHAR_DATA *ch );


void do_direct( CHAR_DATA *ch, char *argument )
{
   char              arg1[MIL];  
   CHAR_DATA       * victim;
   sh_int dam, extra;
   dam = extra = 0;

   argument = one_argument( argument, arg1 );
   if ( IS_NPC(ch))
   return;
   if ( arg1[0] == '\0' )
   {
   	send_to_char( "Usage: Direct <Target>\n\r", ch );
     	return;
   }
   if ( xIS_SET(ch->powerup, KI_BAR))
   {
  	send_to_char( "&RYou can't direct an Energy Barrier.\n\r", ch);
    	return;
   }
   if ( controlling_ki( ch ) == FALSE )
   {
   	send_to_char( "&RYou are not controlling an energy attack.\n\r", ch);
    	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "They aren't here.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
   
   extra = ch->charge / 8;
   if (xIS_SET(ch->powerup, KI_EBALL))
    dam = number_range( 1, 2 );
   else if (xIS_SET(ch->powerup, KI_SPHERE))
    dam = number_range( 2, 3 );
   else if (xIS_SET(ch->powerup, KI_DISK))
    dam = number_range( 3, 4 );
   else if (xIS_SET(ch->powerup, KI_DDD))
    dam = number_range( 4, 9  );
   else if (xIS_SET(ch->powerup, KI_BOMB))
    dam = number_range( 5, 6 );
   else if (xIS_SET(ch->powerup, KI_DONUT))
    dam = number_range( 7, 8 );
   dam -= URANGE( 1, dam - number_range(0,2), 10 );
   if (extra > 0 )
   dam += extra;
   dam = add_kidamage( ch, victim, dam );
   extra = (get_curr_evasion( victim )) - get_curr_accuracy(ch);
   if ( (can_use_skill(ch, number_percent(), gsn_direct ) ) && (number_percent()*1.8) >= extra)
   {                         
        act( AT_RED, "$n directs $s energy attack towards you!", ch, NULL, victim, TO_VICT );      
        act( AT_RED, "\n\rYou direct your energy attack at $N.", ch, NULL, victim, TO_CHAR );
        act( AT_RED, "$n directs $s energy attack towards $N.", ch, NULL, victim, TO_NOTVICT );  
        if (xIS_SET(ch->powerup, KI_DDD))   	{ damage( ch, victim, dam, TYPE_CUTDDD );	 }
        if (xIS_SET(ch->powerup, KI_EBALL)) 	{ damage( ch, victim, dam, TYPE_SLAM );	 }
        if (xIS_SET(ch->powerup, KI_DISK))  	{ damage( ch, victim, dam, TYPE_CUT );	 }
        if (xIS_SET(ch->powerup, KI_SPHERE))	{ damage( ch, victim, dam, TYPE_BURN ); 	 }
        if (xIS_SET(ch->powerup, KI_DONUT))  	{ damage( ch, victim, dam, TYPE_CUT );	 }
        if (xIS_SET(ch->powerup, KI_BOMB))	{ damage( ch, victim, dam, TYPE_BURN ); 	 }
        learn_from_success( ch, gsn_direct );              
   }
   else
   {
        act( AT_ACTION, "\n\rYou direct your energy attack towards $N but it misses.", ch, NULL, victim, TO_CHAR );         
        act( AT_ACTION, "$n's energy attack misses you.", ch, NULL, victim, TO_VICT );  
        act( AT_ACTION, "$n's energy attack misses $N.", ch, NULL, victim, TO_NOTVICT );
        learn_from_failure( ch, gsn_direct );
   }
   consume_ki( ch, "direct");
   waitstate( ch, "direct");
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "direct" );
   ch->charge = 0;
   release_ki( ch );
   return;
}

void do_energyball( CHAR_DATA *ch, char *argument )
{

   if (IS_NPC(ch) || check_spec(ch))
   return;
   if ( controlling_ki( ch ) == TRUE )
   {
    send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
    return;
   }
   if (can_use_skill(ch, number_percent(), gsn_energyball ))
   {
     ch->charge = calc_power( ch );
   	xSET_BIT(ch->powerup, KI_EBALL);
   	send_to_char( "\n\r&RYou ball your fist up directing your energy into your closed hand\n\rlight emits faintly as you begin to open it and create an Energy Ball.\n\r", ch);
   	ch_printf(ch, "&RYou estimate that the Energy ball has an attack power of %s.\n\r", num_punct(get_curr_pl2(ch)));
    	act( AT_RED, "$n balls up $s fist as a faint light begins to glow as $n opens $s\n\rhand reavealing an energy ball.", ch, NULL, NULL, TO_ROOM );
     learn_from_success( ch, gsn_energyball );             
   }
   else
   {
     send_to_char( "\n\r&RYou ball your fist up, and concentrate as create nothing.\n\r", ch);
     act( AT_RED, "$n balls $s fist up and tries to create an energy ball...", ch, NULL, NULL, TO_ROOM );
     learn_from_failure( ch, gsn_energyball );
   }
   consume_ki( ch, "ball" );
   waitstate( ch, "ball");
   return;
}           

void do_sphere( CHAR_DATA *ch, char *argument )
{
   if (IS_NPC(ch) || check_spec(ch))
   return;
   if ( controlling_ki( ch ) == TRUE )
   {
    send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
    return;
   }
   if (can_use_skill2(ch, number_percent(), gsn_sphere ))
   {
     ch->charge = calc_power( ch );
     xSET_BIT(ch->powerup, KI_SPHERE);
     send_to_char( "\n\r&RYou close your hands and the air begins to crackle around\n\ryou as you slowly open them you create an Energy Sphere.\n\r", ch);
     ch_printf(ch, "&RYou estimate that the Energy Sphere has an attack power of %s\n\r",num_punct(get_curr_pl2(ch)));
     act( AT_RED, "$n closes $s hands and the air begins to crackle around you as\n\r$n slowly opens $s hands to reveal an energy sphere.", ch, NULL, NULL, TO_ROOM );
     learn_from_success2( ch, gsn_sphere );
   }
   else
   {
     send_to_char( "\n\r&RYou close your hands and you try to create an Energy Sphere,\n\rbut you create nothing....\n\r", ch);
     act( AT_RED, "$n closes $s hands as the air around them snap crackles and sparks out....", ch, NULL, NULL,TO_ROOM );
     learn_from_failure2( ch, gsn_sphere );
   }
   consume_ki( ch, "sphere" );
   waitstate( ch, "sphere");
   return;
}

void do_charge( CHAR_DATA *ch, char *argument )
{
   char              arg1[MIL];  
   int               charge, ki, currcharge, arg, charpl, pl, mod;
   pl = mod = 0;
   argument = one_argument( argument, arg1 );
   charge = ch->charge;
   ki = ch->mana+1;
   currcharge = 20000000;
   arg = atoi(arg1);
   charpl = (pl * mod);

   if ( IS_NPC(ch))
   return;
   if ( arg1[0] == '\0' )
   {
    	send_to_char( "Usage: Charge <Percentage>\n\r", ch );
    	return;
   }

   if ( !xIS_SET(ch->powerup, PU_SUPPRESS))
   { pl = ch->exp;  mod = ch->mod;}
   else
   { pl = ch->suppress; mod = 1;}

   if (check_spec(ch))
   return;
   if (xIS_SET(ch->powerup, KI_BAR) || xIS_SET(ch->powerup, KI_SHIELD) || xIS_SET(ch->powerup, KI_BARRICADE) )
   {
   	ch_printf(ch, "You can't re-charge energy barriers...\n\r" );
    	return;
   }   
   if ( controlling_ki(ch) == FALSE)
   {
    	send_to_char( "You are not controlling an energy attack.\n\r", ch);
    	return;
   }
   if ( !is_number(arg1))
   {
    	send_to_char( "The percentage must be a number.\n\r", ch);
    	return;
   }
   if ( atoi(arg1) <= 0 )
   {
    	send_to_char( "No point in that.\n\r", ch);
    	return;
   }
   if ( charge + atoi(arg1)  > 100 )
   {
    	send_to_char( "You cannot control an attack of that potential.\n\r", ch);
    	return;
   }  
   if ( atoi(arg1) * 30  > ki)
   {
    	send_to_char( "You don't have enough energy...\n\r", ch);
    	return;
   }
   if ( (charpl + ((atoi(arg1) + charge) * currcharge) >= 1500000000) )
   {
    	send_to_char( "You cannot control an attack of that potential.\n\r", ch);
    	return;
   }
   if (can_use_skill(ch, number_percent(), gsn_charge ))
   {
	ch->charge += atoi(arg1);
    	send_to_char( "\n\r&RYou concentrate and you tunnel more energy into your attack.\n\r", ch);
    	ch_printf(ch, "&RYou estimate that the attack's power is at %d.\n\r",     (charpl + ((arg + charge) * currcharge)) );
    	act( AT_RED, "$n begins to concentrate and $e tunnels more energy into the attack.", ch, NULL, NULL, TO_ROOM );
    	learn_from_success( ch, gsn_charge );   
    	ch->mana -= atoi(arg1) * 30;
   }
   else
   {
    	send_to_char( "\n\r&RYou concentrate and you try to tunnel more energy into the attack... but it dissolves into nothing...\n\r", ch);
    	act( AT_RED, "$n concentrates and tries to tunnel more energy into the attack... but it dissolves into nothing...", ch, NULL, NULL, TO_ROOM );
    	learn_from_failure( ch, gsn_charge );
    	ch->mana -= (charge*10);
	release_ki( ch );
   }
   waitstate( ch, "charge" );
   if (is_kistunned(ch))
   return;

 return;
}

void do_release( CHAR_DATA *ch, char *argument )
{
   if (check_spec(ch) || IS_NPC(ch))
   return;
   if ( controlling_ki( ch ) == FALSE )
   {
	send_to_char( "&RYou are not controlling an energy attack.\n\r", ch);
   	return;
   }
   send_to_char( "\n\r&GYou lower your arms and watch as your Energy Attack slowly fades away into nothing...\n\r", ch);
   act( AT_GREEN, "$n lowers $s arms and watches as $s Energy Attack slowly fades away into nothing...", ch, NULL, NULL, TO_ROOM );
   release_ki( ch );
   return;
}

void putterr( short map, short x, short y, short terr );
bool can_kitrail( short map, short x, short y );


int get_aura_sector( CHAR_DATA *ch )
{
  int aura;

  aura = ch->auracolor;
  if ( ch->aurapowerup > 0 )
  aura = ch->aurapowerup;

	switch ( aura )
	{
                case AURA_BLACK:
			return SECT_AURA_BLACK;
                case AURA_RED:
                        return SECT_AURA_RED;
                case AURA_BLUE:
                        return SECT_AURA_BLUE;
                case AURA_YELLOW:
                        return SECT_AURA_YELLOW;
                case AURA_GREEN:
                        return SECT_AURA_GREEN;
                case AURA_WHITE:
                        return SECT_AURA_WHITE;
                case AURA_DRED:
                        return SECT_AURA_DRED;
                case AURA_DBLUE:
                        return SECT_AURA_DBLUE;
                case AURA_BROWN:
                        return SECT_AURA_BROWN;
                case AURA_CYAN:
                        return SECT_AURA_CYAN;
                case AURA_DGREEN:
                        return SECT_AURA_DGREEN;
                case AURA_PURPLE:
                        return SECT_AURA_PURPLE;
                case AURA_PINK:
                        return SECT_AURA_PINK;
		default:
			return SECT_AURA_WHITE;
	}
	return SECT_AURA_WHITE;
}

bool is_aura_sector( sh_int sector );

void ki_trail( CHAR_DATA *ch, int dist, int dir )
{
   int x, y, count;
   x = ch->x;
   y = ch->y;

   if ( dist < 8 )
   return;
   switch( dir )
   {
       case DIR_NORTH:
	for ( count = 0; count < ch->flyset; count++ )
	{
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
	   y--;
	   continue;
        }
        break;
       case DIR_SOUTH:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           y++;
           continue;
        }
        break;
       case DIR_EAST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x++;
           continue;
        }
	break;
       case DIR_WEST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x--;
           continue;
        }
	break;
       case DIR_SOUTHEAST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x++;
	   y++;
           continue;
        }
        break;
       case DIR_SOUTHWEST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x--;
           y++;
           continue;
        }
        break;
       case DIR_NORTHEAST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x++;
           y--;
           continue;
        }
        break;
       case DIR_NORTHWEST:
        for ( count = 0; count < ch->flyset; count++ )
        {
           if ( is_aura_sector( map_sector[ch->map][x][y] ) == FALSE )
           wasmap_sector[ch->map][x][y] = map_sector[ch->map][x][y];
           if (can_kitrail( ch->map, x, y) == TRUE )
           putterr( ch->map, x, y, get_aura_sector( ch ) );
           x--;
           y--;
           continue;
        }
        break;
	case DIR_DOWN:
	case DIR_UP:
	break;
    }


}

void do_flymap( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *fch;
  CHAR_DATA *nextinroom;
  ROOM_INDEX_DATA *from_room = ch->in_room;
  sh_int count = 0, chars = 0;
  char arg[MSL], arg2[MSL];
  short dist, direction, newx, newy, ki;

  argument = one_argument( argument, arg);
  argument = one_argument( argument, arg2);
  newx = newy = -1;
  ki = 0;
  if (!xIS_SET(ch->act, PLR_FLY))
  {
    	send_to_char("You should use fly instead of run.\n\r", ch);
    	return;
  }

  if (IS_NPC(ch) && !IS_ACT_FLAG(ch, ACT_ONMAP) )
  {
    	send_to_char("You must be outside to use this command.\n\r", ch);
    	return;
  }
  if ( ch->position <= POS_FIGHTING )
  {
    	send_to_char("You must be standing.\n\r", ch);
    	return;
  }
  if ( arg[0] == '\0' || (arg2[0] == '\0' && ch->flyset <= 0))
  {
    	send_to_char("\n\r&RSyntax&r: &YFly &z<&wdirection&z> &z<&w# of blocks&z>", ch);
    	send_to_char("\n\r&RSyntax&r: &YFly set &z<&w# of blocks to default fly>\n\r", ch);
    	return;
  }

  if ( !str_cmp( arg, "set") )
  {
     if (!is_number(arg2))
     {
       	send_to_char("It must be a number.\n\r", ch);
       	return;
     }
     dist = atoi(arg2);
     if ( dist < 2 || dist > 15 )
     {
       	send_to_char("Invalid value. Range is 2 and 15.\n\r", ch);
       	return;
     }
     ch_printf(ch, "Standard Flying set to %d.\n\r", dist );
     if ( dist > 5 )
     send_to_char("&RWarning, flying faster than 5 spaces at a time will consume your energy.\n\r", ch);
     ch->flyset = dist;
     return;
  }

  if ( !is_number( arg2 ) && ch->flyset <= 0 )
  {
    	send_to_char("How many rooms would you like to fly?\n\r", ch);
    	return;
  }
  if ( arg2[0] == '\0' )
  dist = ch->flyset;
  else
  dist = atoi( arg2 );
  if ( dist > 15 || dist < 2 )
  {
    	send_to_char("Invalid distance. Max is 15, minimum is 2.\n\r", ch);
   	return;
  }

  if ( !str_cmp( arg, "n" ) || !str_cmp(arg, "north")) direction = DIR_NORTH;
  else if ( !str_cmp( arg, "e" ) || !str_cmp(arg, "east")) direction = DIR_EAST;
  else if ( !str_cmp( arg, "s" ) || !str_cmp(arg, "south")) direction = DIR_SOUTH;
  else if ( !str_cmp( arg, "w" ) || !str_cmp(arg, "west")) direction = DIR_WEST;
  else if ( !str_cmp( arg, "ne" ) || !str_cmp(arg, "northeast")) direction = DIR_NORTHEAST;
  else if ( !str_cmp( arg, "nw" ) || !str_cmp(arg, "northwest")) direction = DIR_NORTHWEST;
  else if ( !str_cmp( arg, "se" ) || !str_cmp(arg, "southeast")) direction = DIR_SOUTHEAST;
  else if ( !str_cmp( arg, "sw" ) || !str_cmp(arg, "southwest")) direction = DIR_SOUTHWEST;
  else
  {
    	send_to_char("Invalid direction.\n\r", ch);
    	return;
  }

  if (dist > 5 )
  ki = dist * 3.3;
  if ( (ch->mana - dist) < 0 )
  {
    	send_to_char("You don't have enough energy to continue flying that fast.\n\r", ch);
    	return;
  }
 
  switch( direction )
  {
           default:
                break;
           case DIR_NORTH:
                newy = ch->y - dist;
                if ( newy <= -1)
                {
                  send_to_char( "You cannot fly that far north.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_NORTH );
                break;
           case DIR_EAST:
                newx = ch->x + dist;
                if ( newx >= MAX_X)
                {
                  send_to_char( "You cannot fly that far east.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_EAST );
                break;
           case DIR_SOUTH:
                newy = ch->y + dist;
                if ( newy >= MAX_Y)
                {
                  send_to_char( "You cannot fly that far south.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_SOUTH );
                break;

           case DIR_WEST:
                newx = ch->x - dist;
                if ( newx <= -1)
                {
                  send_to_char( "You cannot fly that far west.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_WEST );
                break;
           case DIR_NORTHEAST:
                newx = ch->x + dist; newy = ch->y - dist;
                if ( newx >= MAX_X || newy <= -1)
                {
                  send_to_char( "You cannot fly that far northeast.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_NORTHEAST );
                break;
           case DIR_NORTHWEST:
                newx = ch->x - dist; newy = ch->y - dist;
                if ( newx <= -1 || newy <= -1)
                {
                  send_to_char( "You cannot fly that far northeast.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_NORTHWEST );
                break;

           case DIR_SOUTHEAST:
                newx = ch->x + dist; newy = ch->y + dist;
                if ( newx >= MAX_X || newy >= MAX_Y)
                {
                  send_to_char( "You cannot fly that far northeast.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_SOUTHEAST );
                break;

           case DIR_SOUTHWEST:
                newx = ch->x - dist; newy = ch->y + dist;
                if ( newx <= -1 || newy >= MAX_Y)
                {
                  send_to_char( "You cannot fly that far northeast.\n\r", ch );
                  return;
                }
                ki_trail( ch, dist, DIR_SOUTHWEST );
                break;

  }
  send_to_char( "You start flying...\n\r", ch);
  act(AT_WHITE, "$n starts flying...\n\r", ch, NULL, NULL, TO_ROOM);
  if ( newx > -1 && newy > -1)
  {
    if (map_sector[ch->map][newx][newy] == SECT_INSIDE || map_sector[ch->map][newx][newy] == SECT_BUILDING)
    {
     send_to_char("You can't fly ontop of that.\n\r", ch);
     return;
    }
  }
  else if ( newx > -1 && newy == -1)
  {
    if (map_sector[ch->map][newx][ch->y] == SECT_INSIDE || map_sector[ch->map][newx][newy] == SECT_BUILDING)
    {
     send_to_char("You can't fly ontop of that.\n\r", ch);
     return;
    }
  }
  else if ( newx == -1 && newy > -1)
  {
    if (map_sector[ch->map][ch->x][newy] == SECT_INSIDE || map_sector[ch->map][newx][newy] == SECT_BUILDING)
    {
     send_to_char("You can't fly ontop of that.\n\r", ch);
     return;
    }
  }
  for( fch = from_room->first_person; fch; fch = fch->next_in_room )
      chars++;

  for( fch = from_room->first_person; fch && ( count < chars ); fch = nextinroom )
  {
     nextinroom = fch->next_in_room;
     count++;
     if( fch != ch  /* loop room bug fix here by Thoric */
     && fch->master == ch
     && ( fch->position == POS_STANDING || fch->position == POS_MOUNTED )
     && fch->x == ch->x && fch->y == ch->y && fch->map == ch->map
     && !IS_NPC( fch ))
     {
          if (!xIS_SET(fch->act, PLR_FLY))
          {
            send_to_char( "You were not able to follow your leader because you are not flying.\n\r", fch);
            continue;
          }
          act( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR );
          if ( newx > -1 )
          fch->x = URANGE( 0, newx, (MAX_X - 1) );
          if ( newy > -1 )
          fch->y = URANGE( 0, newy, (MAX_Y - 1) );
     }
  }

  if ( newx > -1 )
  ch->x = URANGE( 0, newx, 499 );
  if ( newy > -1 )
  ch->y = URANGE( 0, newy, 499 );
  act(AT_WHITE, "$n finishes flying...\n\r", ch, NULL, NULL, TO_ROOM);
  ch->mana -= ki;
  WAIT_STATE( ch, 3 );
  do_look( ch, "" );

}


void  do_fly( CHAR_DATA *ch, char *argument)
{
  char arg[MSL];
  char arg2[MSL];
  sh_int gsn;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  gsn = gsn_fly;

  if (check_spec(ch))
  return;
  if ( ch->hit <= 10)
  {
   send_to_char("You don't have enough energy to try and fly...", ch);
   return;
  }
  if ( !str_cmp( arg, "up" ) )
  {
       if (xIS_SET(ch->powerup, PU_FLYUP) )
       {
         send_to_char("&RYour already flying in the air!\n\r", ch);
         return;
       }
       else if (!xIS_SET(ch->act, PLR_FLY))
       {
         send_to_char("&RYou need to be atleast hovering above the ground!\n\r", ch);
         return;
       }
       act( AT_BLUE, "\n\rYou soar up into the sky leaving a ki trail behind you.", ch, NULL, NULL, TO_CHAR );
       act( AT_BLUE, "$n stops hovering and soars up into the sky leaving a ki trail behind.", ch, NULL, NULL, TO_ROOM );
       xSET_BIT(ch->powerup, PU_FLYUP);  
       return;
  }  
  else if ( !str_cmp( arg, "n" ) || !str_cmp(arg, "north")
       || !str_cmp( arg, "e" ) || !str_cmp(arg, "east") 
       || !str_cmp( arg, "s" ) || !str_cmp(arg, "south")
       || !str_cmp( arg, "w" ) || !str_cmp(arg, "west")
       || !str_cmp( arg, "ne" ) || !str_cmp(arg, "northeast")
       || !str_cmp( arg, "nw" ) || !str_cmp(arg, "northwest")
       || !str_cmp( arg, "se" ) || !str_cmp(arg, "southeast")
       || !str_cmp( arg, "sw" ) || !str_cmp(arg, "southwest") 
       || !str_cmp( arg, "set" ) )
  {
      command( ch, do_flymap, "%s %s", arg, arg2);
      return;
  }

  if ( xIS_SET(ch->act, PLR_FLY ))
  {
   if ( xIS_SET(ch->act, PLR_ONMAP) && get_mapabove( ch->map ) == TRUE )
   {
     send_to_char("You cannot land while your in the air.\n\r", ch);
     return;
   }
    if ( xIS_SET(ch->act, PLR_ONMAP) && ( (get_mapmiddle( ch->map ) == TRUE && (map_sector[ch->map][ch->x][ch->y] == SECT_OCEAN || map_sector[ch->map][ch->x][ch->y] == SECT_RIVER)) || get_mapbelow( ch->map)) )
   {
     send_to_char("You cannot land while your above water.\n\r", ch);
     return;
   }

   act( AT_BLUE, "\n\rYou descend towards the ground.", ch, NULL, NULL, TO_CHAR );
   act( AT_BLUE, "$n descends towards the ground.", ch, NULL, NULL, TO_ROOM );   
   xREMOVE_BIT(ch->powerup, PU_FLYUP);
   xREMOVE_BIT(ch->act, PLR_FLY);
  }
  else if ( !xIS_SET( ch->act, PLR_FLY))
  {
   if ( can_use_skill(ch, number_percent(), gsn_fly ))
   {
    consume_ki( ch, "fly");
    waitstate( ch, "fly" );
    act( AT_BLUE, "\n\rYou begin to hover above the ground.", ch, NULL, NULL, TO_CHAR );
    act( AT_BLUE, "$n begins to hover above the ground.", ch, NULL, NULL, TO_ROOM );
    xSET_BIT(ch->act, PLR_FLY);
    learn_from_success( ch, gsn );
   }
   else
   {
    consume_ki( ch, "fly");
    waitstate( ch, "fly" );
    act( AT_GREEN, "\n\rYou try to fly but crash back into the ground.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREEN, "$n tries to fly but crashes back into the ground.", ch, NULL, NULL, TO_ROOM );
    ch->hit -= 1;
    learn_from_failure( ch, gsn );
    return;
   }
 }
}  

void do_disk( CHAR_DATA *ch, char *argument )
{

  if (IS_NPC(ch) || check_spec(ch))
  return;

  if ( controlling_ki( ch ) == TRUE )
  {
    send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
    return;
  }
  if (can_use_skill2(ch, number_percent(), gsn_disk ))
  {
    ch->charge = calc_power( ch );
       xSET_BIT(ch->powerup, KI_DISK);
       send_to_char( "\n\r&RYou raise your arm and you create a Destructo Disk.\n\r", ch);
       ch_printf(ch, "&RYou estimate that the disk has an attack power of %s\n\r", num_punct(get_curr_pl2(ch)) );
       act( AT_RED, "$n raises $s arms and creates an energy disk.", ch, NULL, NULL, TO_ROOM );
    learn_from_success2( ch, gsn_disk );             
  }
  else
  {
       send_to_char( "\n\r&RYou raise your arm and you try to create a Destructo Disk, but you create nothing.\n\r", ch);
       act( AT_RED, "$n raises $s hands and tries to create a Destructo Disk, but fails...", ch, NULL, NULL, TO_ROOM );
    learn_from_failure2( ch, gsn_disk );
  }
  waitstate(ch, "disk");
  consume_ki( ch, "disk");
  if (is_kistunned(ch))
  return;
}        

void do_donut( CHAR_DATA *ch, char *argument )
{
 
   if (IS_NPC(ch) || check_spec(ch))
   return;
   if ( controlling_ki( ch ) == TRUE )
   {
     send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
     return;
   }
   if ( !xIS_SET(ch->act, PLR_FUSED))
   {
     send_to_char( "&RYou need to be fused to use this technique.\n\r", ch);
     return;
   }   
   if (can_use_skill2(ch, number_percent(), gsn_donut ))
   {
     ch->charge = calc_power( ch );
     xSET_BIT(ch->powerup, KI_DONUT);
     send_to_char( "\n\r&YYou raise your arm and you create a Galactic Donut.\n\r", ch);
     ch_printf(ch, "&YYou estimate that the disk has an attack power of %s\n\r", num_punct(get_curr_pl2(ch)));
     act( AT_YELLOW, "$n raises $s arms and creates a galactic donut.", ch, NULL, NULL, TO_ROOM );
     learn_from_success2( ch, gsn_donut );
   }
   else
   {
     send_to_char( "\n\r&RYou raise your arm and you try to create a Galactic Donut, but you create nothing.\n\r", ch);
     act( AT_RED, "$n raises $s hands and tries to create a Galactic Donut, but fails...", ch, NULL, NULL, TO_ROOM );
     learn_from_failure( ch, gsn_donut );
   }
   consume_ki( ch, "donut" );
   waitstate( ch, "donut");
   return;
}

void do_ddd( CHAR_DATA *ch, char *argument )
{
 
   if (IS_NPC(ch) || check_spec(ch))
   return;
   if ( controlling_ki( ch ) == TRUE )
   {
    send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
    return;
   }
   if (can_use_skill2(ch, number_percent(), gsn_ddd ))
   {
     ch->charge = calc_power( ch );
     xSET_BIT(ch->powerup, KI_DDD);
     send_to_char( "\n\r&RYou raise your arms and you create a Destructo Disk with each hand.\n\r", ch);
     ch_printf(ch, "&RYou estimate that both of the disk have an attack power of %s\n\r", num_punct(get_curr_pl2(ch)));
     act( AT_RED, "$n raises $s arms and creates an energy disk with each hand.", ch, NULL, NULL, TO_ROOM );
     learn_from_success2( ch, gsn_ddd );
   }
   else
   {
     send_to_char( "\n\r&RYou raise your arms and you try to create a Destructo Disk with each hand, but you create nothing.\n\r", ch);
     act( AT_RED, "$n raises $s arms and tries to create a Destructo Disk with each hand, but fails to do so...",ch, NULL, NULL, TO_ROOM );
     learn_from_failure( ch, gsn_ddd );
   }
   consume_ki( ch, "ddd" );
   waitstate( ch, "ddd");
   return;
}

void do_nkick( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = 0;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level[gsn_kick];
  if ( ( victim = who_fighting( ch ) ) == NULL )
  {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
  }
  extra = (get_curr_evasion( victim )) - get_curr_accuracy(ch);
  if ( (can_use_skill(ch, number_percent(), gsn_kick ) ) && (number_percent()*1.6) >= extra)
  {
     lvl = URANGE( 0, lvl / 2, 15 );
     dam = number_range(3+(lvl), 6+(lvl) );
     dam = add_damage( ch, victim, dam );
     damage(ch, victim, dam, TYPE_KICK );
     learn_from_success( ch, gsn_kick );
  }
  else
  {
     damage(ch, victim, 0, TYPE_KICK);
     learn_from_failure( ch, gsn_kick );
  }
   waitstate( ch, "kick" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "kick" );
   return;
}

void do_dk( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = 0;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_dk];
  if ( ( victim = who_fighting( ch ) ) == NULL )
  {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
  }
  extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_dk )) && (number_percent()*1.8) >= extra)
  {
     lvl = URANGE( 0, lvl / 2, 15 );
     dam = number_range(4+(lvl), 8+(lvl) );
     dam = add_damage( ch, victim, dam );
     dam /= 1.5;
     damage(ch, victim, dam, TYPE_DK );
     learn_from_success2( ch, gsn_dk );
  }
  else
  {
     damage(ch, victim, 0, TYPE_DK);
     learn_from_failure2( ch, gsn_dk );
  }
   waitstate( ch, "dk" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "dk" );
   return;
}

void do_npunch( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level[gsn_punch];
  if ( ( victim = who_fighting( ch ) ) == NULL )
  {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
  }
  extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
  if ( (can_use_skill(ch, number_percent(), gsn_punch )) && (number_percent()*1.8) >= extra)
  {
     lvl = URANGE( 0, lvl / 2, 15 );
     dam = number_range(3+(lvl), 6+(lvl) );
     dam = add_damage( ch, victim, dam );
     damage(ch, victim, dam, TYPE_NPUNCH );
     learn_from_success( ch, gsn_punch );
  }
  else
  {
     damage(ch, victim, 0, TYPE_NPUNCH);
     learn_from_failure( ch, gsn_punch );
  }
   waitstate( ch, "punch" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "punch" );
   return;
}

void do_dp( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = 0;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_dp];
  if ( ( victim = who_fighting( ch ) ) == NULL )
  {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
  }
  extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_dp )) && (number_percent()*1.8) >= extra)
  {
     lvl = URANGE( 0, lvl / 2, 15 );
     dam = number_range(4+(lvl), 8+(lvl) );
     dam = add_damage( ch, victim, dam );
     dam /= 1.5;
     damage(ch, victim, dam, TYPE_DP );
     learn_from_success2( ch, gsn_dp );
  }
  else
  {
     damage(ch, victim, 0, TYPE_DP);
     learn_from_failure2( ch, gsn_dp );
  }
   waitstate( ch, "dp" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "dp" );
   return;
}

void do_claw( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   sh_int dam, extra, lvl;

   dam = extra = 0;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_claw];
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
     	send_to_char( "You aren't fighting anyone.\n\r", ch );
     	return;
   }
   extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_claw )) && (number_percent()*1.8) >= extra)
   {
      lvl = URANGE( 0, lvl / 2, 15 );
      dam = number_range(5+(lvl), 8+(lvl));
      dam = add_damage( ch, victim, dam );
      damage(ch, victim, dam, TYPE_CLAW);
      learn_from_success2( ch, gsn_claw );
   }
   else
   {
      damage(ch, victim, 0, TYPE_CLAW);
      learn_from_failure2( ch, gsn_claw );
   }
   waitstate( ch, "claw");
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "claw");
   return;
}

void do_eyebeam( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   sh_int dam, extra, lvl;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   dam = extra = 0;
   lvl = ch->pcdata->skill_level2[gsn_eyebeam];
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
     send_to_char( "You aren't fighting anyone.\n\r", ch );
     return;
   }
   act( AT_RED, "\n\rYou look at $N and your eyes glow up.\n\rYou launch two energy beams from your eyes.", ch, NULL, victim, TO_CHAR );
   act( AT_RED, "$n looks at you and $s eyes glow up.\n\r$n launches two beams from $s eyes.", ch, NULL, victim, TO_VICT );
   act( AT_RED, "$n looks at $N and $s eyes glow up.\n\r$n launches two energy beams from $s eyes.", ch, NULL, victim, TO_NOTVICT );
   extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_eyebeam )) && (number_percent()*1.8) >= extra)
   {
      lvl = URANGE( 0, lvl / 2, 15 );
      dam = number_range(2+(lvl), 7+(lvl));
      dam /= 1.6;
      dam = add_damage( ch, victim, dam );
      learn_from_success2( ch, gsn_eyebeam );
      damage( ch, victim, dam, TYPE_EYE );
   }
   else
   {
      damage(ch, victim, 0, TYPE_EYE);
      learn_from_failure2( ch, gsn_eyebeam );
   }
   waitstate( ch, "eyebeam" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "eyebeam" );
   return;
}

void do_deathbeam( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   sh_int dam, extra, lvl;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   dam = extra = 0;
   lvl = ch->pcdata->skill_level2[gsn_dbeam];
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
     send_to_char( "You aren't fighting anyone.\n\r", ch );
     return;
   }
   act( AT_RED, "&R$n points $s finger at you.\n\r&z$n launches a death beam from $s finger. ", ch, NULL, victim, TO_VICT );
   act( AT_RED, "\n\r&RYou point your finger at $N.\n\r&zYou launch a death beam off your finger.", ch, NULL, victim, TO_CHAR );
   act( AT_RED, "&R$n points $s finger at $N.\n\r&z$n launches a death beam from $s finger.", ch, NULL, victim, TO_NOTVICT );
   extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_dbeam )) && (number_percent()*1.8) >= extra)
   {
      extra = check_extra( get_curr_str(ch), get_curr_int(victim), 5 );
      lvl = URANGE( 0, lvl / 2, 15 );
      dam = number_range(4+(lvl), 8+(lvl));
      dam = add_damage( ch, victim, dam );
      learn_from_success2( ch, gsn_dbeam );
      damage( ch, victim, dam, TYPE_DEATHBEAM );
   }
   else
   {
      damage(ch, victim, 0, TYPE_DEATHBEAM);
      learn_from_failure2( ch, gsn_dbeam );
   }
   waitstate( ch, "deathbeam" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "deathbeam" );
   return;
}



void do_fingerbeam( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   sh_int dam, extra, lvl;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   dam = extra = 0;
   lvl = ch->pcdata->skill_level2[gsn_fbeam];
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
     send_to_char( "You aren't fighting anyone.\n\r", ch );
     return;
   }
   act( AT_RED, "$n points $s finger at you.\n\r$n launches an energy beam from $s finger. ", ch, NULL, victim, TO_VICT );
   act( AT_RED, "\n\rYou point your finger at $N.\n\rYou launch an energy beam off your finger.", ch, NULL, victim, TO_CHAR );
   act( AT_RED, "$n points $s finger at $N.\n\r$n launches an energy beam from $s finger.", ch, NULL, victim, TO_NOTVICT );
   extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_fbeam )) && (number_percent()*1.8) >= extra)
   {
      lvl = URANGE( 0, lvl / 2, 15 );
      dam = number_range(3+(lvl), 5+(lvl));
      dam = add_damage( ch, victim, dam );
      learn_from_success2( ch, gsn_fbeam );
      damage( ch, victim, dam, TYPE_FINGER );
   }
   else
   {
      damage(ch, victim, 0, TYPE_FINGER);
      learn_from_failure2( ch, gsn_fbeam );
   }
   waitstate( ch, "fingerbeam" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "fingerbeam" );
   return;
}


void do_makosenbuild( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *obj;
 sh_int damage, lvl;
 char buf[MSL];

  if ( IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_makosen]/2;

  damage = number_range( 10+(lvl), 60+(lvl) );
  damage *= (get_curr_cha(ch))/1.5;
   
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
    	return;
  }
  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }

  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&R$n charges both of $s hands with energy. $n launches two blasts one after the other towards the structure." , ch, NULL, NULL, TO_ROOM);
  act( AT_WHITE, "\n\r&wYou raise your arms as you prepare for your attack.\n\r&RYou charge both of your hands with energy. And you launch to blasts one after the other towards the structure.", ch, NULL, NULL, TO_CHAR);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
  if (obj->value[0] <= 0)
  {
     make_debris(obj);
     planet_support(ch);
  }
  consume_ki( ch, "makosen" );
  waitstate( ch, "makosen" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "makosen");
  return;  
}


void do_masenkobuild( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *obj;
 sh_int damage, lvl;
 char buf[MSL];

  if ( IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_masenko]/2;

  damage = number_range( 10+(lvl), 60+(lvl) );
  damage *= (get_curr_cha(ch))/1.5;
   
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
    	return;
  }
  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }

  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  act( AT_WHITE, "\n\r&w$s raises both $s arms above $s head and starts to gather energy...\n\r&wA sphere of energy forms between $s hands.\n\r&Y$n yells MA-SEN-KO-HAAAA!!!!\n\r&w$n thrusts $s arms forward...\n\r&W$n hurls $s Energy Ball right at the structure!!" , ch, NULL, NULL, TO_ROOM); 
  act( AT_WHITE, "\n\r&wYou raise both arms above your head and you start to gather energy...\n\r&wA sphere of  energy forms between your hands.\n\r&YYou yell MA-SEN-KO-HAAAA!!!!\n\r&wYou thrust your arms forward...\n\r&WYou hurl your Energy Ball right at the structure!!" , ch, NULL, NULL, TO_CHAR);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
  if (obj->value[0] <= 0)
  {
     make_debris(obj);
     planet_support(ch);
  }
  consume_ki( ch, "masenko" );
  waitstate( ch, "masenko" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "masenko");
  return;  
}

void do_makosen( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *location = ch->in_room;
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  dam = extra = lvl = 0;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_makosen]/2;
  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' )
  {
    ch_printf(ch, "Target masenko at who?\n\r" );
    return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_makosenbuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
     if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
     {
        send_to_char("They aren't here\n\r", ch);
        return;
     }
     else
        map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }
  else
  map = FALSE;

  if ( can_attack( ch, victim ) == FALSE )
  {
      send_to_char("You can't do that.\n\r", ch);
      return;
  }
  if ( victim == ch )
  {
     send_to_char( "Target yourself? You been stupified by Felten?\n\r", ch );
     return;
  }
  if ( gettimer( ch, "makosen" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }
  warcry( ch, victim );
  if ( sense )
  {
    act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&R$n charges both of $s hands with energy. $n launches two blasts one after the other towards the sky!!" , ch, NULL, victim, TO_NOTVICT);
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
  }
  act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&R$n charges both of $s hands with energy. $n launches two blasts one after the other." , ch, NULL, victim, TO_VICT);
  act( AT_WHITE, "\n\r&wYou raise your arms as you prepare for your attack.\n\r&RYou charge both of your hands with energy. And you launch to blasts one after the other.", ch, NULL, victim, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&R$n charges both of $s hands with energy. $n launches two blasts one after the other." , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_makosen )) && (number_percent()*1.8) >= extra)
  {
        dam = number_range( 7+(lvl), 14+(lvl) );
     	dam = add_kidamage( ch, victim, dam );
     	if ( map )
     	destroy_area( victim );
     	if ( sense )
     	sensedamage( ch, victim, dam, TYPE_MAKOSEN );
     	else
     	damage(ch, victim, dam, TYPE_MAKOSEN);
     	learn_from_success2( ch, gsn_makosen );
  }
  else
  {
     	if ( sense )
     	sensedamage( ch, victim, 0, TYPE_MAKOSEN );
     	else
     	damage(ch, victim, 0, TYPE_MAKOSEN);
     	learn_from_failure2( ch, gsn_makosen );
  }
  if ( sense )
  {
    char_from_room( ch );
    char_to_room( ch, location );
  }
  addtimer( ch, "makosen", 6 );
  consume_ki( ch, "makosen" );
  waitstate( ch, "makosen" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "makosen");
  return;  
}



void do_masenko( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *location = ch->in_room;
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  dam = extra = lvl = 0;

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_masenko]/2;
  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' )
  {
    ch_printf(ch, "Target masenko at who?\n\r" );
    return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_masenkobuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
     if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
     {
        send_to_char("They aren't here\n\r", ch);
        return;
     }
     else
        map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }
  else
  map = FALSE;

  if ( can_attack( ch, victim ) == FALSE )
  {
      send_to_char("You can't do that.\n\r", ch);
      return;
  }
  if ( victim == ch )
  {
     send_to_char( "Target yourself? You been stupified by Felten?\n\r", ch );
     return;
  }
  if ( gettimer( ch, "masenko" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }
  warcry( ch, victim );

  if ( sense )
  {
     act( AT_WHITE, "\n\r&w$n raises both $s arms above $s head and starts to gather energy...\n\r&wA sphere of energy forms between $s hands.\n\r&Y$n yells MA-SEN-KO-HAAAA!!!!\n\r&w$n thrusts $s arms forward...\n\r&W$n hurls $s Energy Ball towards the sky!!" , ch, NULL, victim, TO_NOTVICT);
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
  }
  act( AT_WHITE, "\n\r&w$s raises both $s arms above $s head and starts to gather energy...\n\r&wA sphere of energy forms between $s hands.\n\r&Y$n yells MA-SEN-KO-HAAAA!!!!\n\r&w$n thrusts $s arms forward...\n\r&W$n hurls $s Energy Ball right at you!!" , ch, NULL, victim, TO_VICT); 
  act( AT_WHITE, "\n\r&wYou raise both arms above your head and you start to gather energy...\n\r&wA sphere of  energy forms between your hands.\n\r&YYou yell MA-SEN-KO-HAAAA!!!!\n\r&wYou thrust your arms forward...\n\r&WYou hurl your Energy Ball right at $N!!" , ch, NULL, victim, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n raises both $s arms above $s head and starts to gather energy...\n\r&wA sphere of energy forms between $s hands.\n\r&Y$n yells MA-SEN-KO-HAAAA!!!!\n\r&w$n thrusts $s arms forward...\n\r&W$n hurls $s Energy Ball right at $N!!" , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_masenko )) && (number_percent()*1.8) >= extra)
  {
     	dam = number_range( 4+(lvl), 7+(lvl) );
     	dam = add_kidamage( ch, victim, dam );
     	if ( map )
     	destroy_area( victim );
     	if ( sense )
     	sensedamage( ch, victim, dam, TYPE_MASENKO );
     	else
     	damage(ch, victim, dam, TYPE_MASENKO);
     	learn_from_success2( ch, gsn_masenko );
  }
  else
  {
     	if ( sense )
     	sensedamage( ch, victim, 0, TYPE_MASENKO);
     	else
     	damage(ch, victim, 0, TYPE_MASENKO);
     	learn_from_failure2( ch, gsn_masenko );
  }
  if ( sense )
  {
    char_from_room( ch );
    char_to_room( ch, location );
  }
  addtimer( ch, "masenko", 6 );
  consume_ki( ch, "masenko" );
  waitstate( ch, "masenko" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "masenko");
  return;  
}


void do_dodompabuild( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *obj;
 sh_int damage, lvl;
 char buf[MSL];

  if ( IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_dodompa]/2;
  damage = number_range( 10+lvl, 60+lvl );
  damage *= (get_curr_cha(ch))/2;
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
  	return;
  }

  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }
  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  act( AT_WHITE, "\n\r&w$n begins to gather some of $s energy in $s right hand.\n\r&Y$n raises $s hand, pointing at the structure as $e launches a small yellow beam!" , ch, NULL, NULL, TO_ROOM);
  act( AT_WHITE, "\n\r&wYou begin to gather some of your energy in your right hand.\n\r&YYou raise your hand, pointing at the structure as you launch a small yellow beam!", ch, NULL, NULL, TO_CHAR);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);

  if (obj->value[0] <= 0)
  {
     	make_debris(obj);
     	planet_support(ch);
  }
  consume_ki( ch, "dodompa");
  waitstate( ch, "dodompa" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "dodompa" );
  return;
}



void do_tribeambuild( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *obj;
 sh_int damage, lvl;
 char buf[MSL];

  if ( IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_tribeam]/2;
  damage = number_range( 10+lvl, 60+lvl );
  damage *= (get_curr_cha(ch))/2;
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
  	return;
  }

  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }
  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  act( AT_WHITE, "\n\r&wYou strech your arms and you prepare for a tribeam\n\r&wYou form a triangle with your hands and you start to gather energy.\n\r&YYou send your tribeam right at the structure!", ch, NULL, NULL, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n stretches $s arms and prepares for a tribeam.\n\r$n forms a triangle with $s hands and starts to gather energy.\n\r&Y$n sends $s tribeam right at the structure!" , ch, NULL, NULL, TO_ROOM);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);

  if (obj->value[0] <= 0)
  {
     	make_debris(obj);
     	planet_support(ch);
  }
  consume_ki( ch, "tribeam");
  waitstate( ch, "tribeam" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "tribeam" );
  return;
}

void do_dodompa( CHAR_DATA *ch, char *argument )
{
 ROOM_INDEX_DATA *location = ch->in_room;

  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  argument = one_argument( argument, arg1 );
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_dodompa]/2;
  if ( arg1[0] == '\0' )
  {
      ch_printf(ch, "Target dodompa at who?\n\r"   );
      return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_dodompabuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
     if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
     {
       send_to_char("They aren't here\n\r", ch);
       return;
     }
     else
     map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }
  if ( can_attack( ch, victim ) == FALSE || victim == ch)
  {
     send_to_char("You can't do that.\n\r", ch);
     return;
  }
  if ( gettimer( ch, "dodompa" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

  if ( !sense )
  warcry( ch, victim );
  if ( sense )
  {
    act( AT_WHITE, "\n\r&w$n begins to gather some of $s energy in $s right hand.\n\r&Y$n raises $s hand, pointing at $N as $e launches a small yellow beam towards the sky!" , ch, NULL, victim, TO_NOTVICT);
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
  }
  act( AT_WHITE, "\n\r&w$n begins to gather some of $s energy in $s right hand.\n\r&Y$n raises $s hand, pointing at you as $e launches a small yellow beam!" , ch, NULL, victim, TO_VICT);
  act( AT_WHITE, "\n\r&wYou begin to gather some of your energy in your right hand.\n\r&YYou raise your hand, pointing at $N as you launch a small yellow beam towards $M!", ch, NULL, victim, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n begins to gather some of $s energy in $s right hand.\n\r&Y$n raises $s hand, pointing at $N as $e launches a small yellow beam towards $m!" , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_dodompa )) && (number_percent()*1.8) >= extra)
  {
        dam = number_range( 6+(lvl), 13+(lvl) );
     	dam = add_kidamage( ch, victim, dam );
        if ( sense )
        sensedamage( ch, victim, dam, TYPE_DODOMPA );
        else
     	damage( ch, victim, dam, TYPE_DODOMPA );
     	if (map)
     	destroy_area( victim );
     	learn_from_success2( ch, gsn_dodompa );
  }
  else
  {
        if ( sense )
        sensedamage( ch, victim, 0, TYPE_DODOMPA );
        else
     	damage( ch, victim, 0, TYPE_DODOMPA );
    	learn_from_failure2( ch, gsn_dodompa );
  } 
  if ( sense )
  {
        char_from_room( ch );
        char_to_room( ch, location );
  }

  consume_ki( ch, "dodompa");
  waitstate( ch, "dodompa" );
  addtimer( ch, "dodompa", 8 );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "dodompa" );
  return;
}

void do_tribeam( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *location = ch->in_room;
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  argument = one_argument( argument, arg1 );
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_tribeam]/2;
  if ( arg1[0] == '\0' )
  {
      ch_printf(ch, "Target tribeam at who?\n\r"   );
      return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_tribeambuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
     if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
     {
       send_to_char("They aren't here\n\r", ch);
       return;
     }
     else
     map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }
  if ( can_attack( ch, victim ) == FALSE || victim == ch)
  {
     send_to_char("You can't do that.\n\r", ch);
     return;
  }
  if ( gettimer( ch, "tribeam" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

  if ( !sense )
  warcry( ch, victim );
  if ( sense )
  {
     act( AT_WHITE, "\n\r&w$n stretches $s arms and prepares for a tribeam.\n\r$n forms a triangle with $s hands and starts to gather energy.\n\r&Y$n sends $s tribeam towards the sky!" , ch, NULL, victim, TO_NOTVICT);
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
  }
  act( AT_WHITE, "\n\r&w$n stretches $s arms and prepares for a tribeam.\n\r$n forms a triangle with $s hands and starts to gather energy.\n\r&Y$n sends $s tribeam right at you!" , ch, NULL, victim, TO_VICT);
  act( AT_WHITE, "\n\r&wYou strech your arms and you prepare for a tribeam\n\r&wYou form a triangle with your hands and you start to gather energy.\n\r&YYou send your tribeam right at $N!", ch, NULL, victim, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n stretches $s arms and prepares for a tribeam.\n\r$n forms a triangle with $s hands and starts to gather energy.\n\r&Y$n sends $s tribeam right at $N!" , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_tribeam )) && (number_percent()*1.8) >= extra)
  {
     	dam = number_range( 6+(lvl), 10+(lvl) );
     	dam = add_kidamage( ch, victim, dam );
        if ( sense )
        sensedamage( ch, victim, dam, TYPE_TRIBEAM );
        else
     	damage( ch, victim, dam, TYPE_TRIBEAM );
     	if (map)
     	destroy_area( victim );
     	learn_from_success2( ch, gsn_tribeam );
  }
  else
  {
        if ( sense )
        sensedamage( ch, victim, 0, TYPE_TRIBEAM );
        else
     	damage( ch, victim, 0, TYPE_TRIBEAM );
    	learn_from_failure2( ch, gsn_tribeam );
  }
  if ( sense )
  {
  	char_from_room( ch );
  	char_to_room( ch, location );
  } 
  consume_ki( ch, "tribeam");
  waitstate( ch, "tribeam" );
  addtimer( ch, "tribeam", 8 );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "tribeam" );
  return;
}

void do_mouthblastbuild( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *obj;
 sh_int damage, lvl;
 char buf[MSL];

  if ( IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_mouthblast]/2;
  damage = number_range( 10+lvl, 60+lvl );
  damage *= (get_curr_cha(ch))/2;
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
  	return;
  }

  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }
  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  act( AT_WHITE, "\n\r&w$n begins to inhale the air around $m as $e prepares for $s attack.\n\r&p$n opens $s mouth as $e releases a massive energy beam from the inside towards the structure!" , ch, NULL, NULL, TO_NOTVICT);
  act( AT_WHITE, "\n\r&wYou begin to inhale the air around you as you prepares for your attack.\n\r&pYou open your mouth as you releases a massive energy beam from the inside towards the structure!", ch, NULL, NULL, TO_CHAR);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's blast impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour blast impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);

  if (obj->value[0] <= 0)
  {
     	make_debris(obj);
     	planet_support(ch);
  }
  consume_ki( ch, "mouthblast");
  waitstate( ch, "mouthblast" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "mouthblast" );
  return;
}


void do_mouthblast( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  argument = one_argument( argument, arg1 );
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_mouthblast]/2;
  if ( IS_MONKEY(ch) && !xIS_SET(ch->powerup, PU_OOZARU))
  {
      ch_printf(ch, "You can only do this while Oozaru.\n\r");
      return;
  }
  if ( arg1[0] == '\0' )
  {
      ch_printf(ch, "Target Mouthblast at who?\n\r"   );
      return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_mouthblastbuild(ch, "structure");
      return;
  }
  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
      send_to_char( "They aren't here.\n\r", ch );
      return;
  }
  if ( can_attack( ch, victim ) == FALSE || victim == ch)
  {
     send_to_char("You can't do that.\n\r", ch);
     return;
  }
  if ( gettimer( ch, "mouthblast" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

  warcry( ch, victim );
  act( AT_WHITE, "\n\r&w$n begins to inhale the air around $m as $e prepares for $s attack.\n\r&p$n opens $s mouth as $e releases a massive energy beam from the inside towards you!" , ch, NULL, victim, TO_VICT);
  act( AT_WHITE, "\n\r&wYou begin to inhale the air around you as you prepares for your attack.\n\r&pYou open your mouth as you releases a massive energy beam from the inside towards $N!", ch, NULL, victim, TO_CHAR);
  act( AT_WHITE, "\n\r&w$n begins to inhale the air around $m as $e prepares for $s attack.\n\r&p$n opens $s mouth as $e releases a massive energy beam from the inside towards $N!" , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_mouthblast )) && (number_percent()*1.8) >= extra)
  {
     	dam = number_range( 8+(lvl), 12+(lvl) );
     	dam = add_kidamage( ch, victim, dam );
     	damage( ch, victim, dam, TYPE_NBLAST );
     	learn_from_success2( ch, gsn_mouthblast );
  }
  else
  {
     	damage( ch, victim, 0, TYPE_NBLAST );
    	learn_from_failure2( ch, gsn_mouthblast );
  } 
  consume_ki( ch, "mouthblast");
  waitstate( ch, "mouthblast" );
  addtimer( ch, "mouthblast", 8 );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "mouthblast" );
  return;
}


void do_deathball( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_deathball]/2;
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
   }
   if ( gettimer( ch, "deathball" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }

   act( AT_WHITE, "\n\r&w$n raises $s right hand and lifts $s finger upwards as $e charges for $s attack.\n\r&RA red ball of energy is created above $s finger and it crackles with energy.\n\r&Y$n yells: 'DEATH-BALL' as $e throws $s attack towards you with amazing speed!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou raise your right hand and lift your finger upwards as you charge for your attack.\n\r&RA red ball of energy is created above your finger and it crackles with energy.\n\r&YYou yell: 'DEATH-BALL' as you throw your attack towards $N with amazing speed!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n raises $s right hand and lifts $s finger upwards as $e charges for $s attack.\n\r&RA red ball of energy is created above $s finger and it crackles with energy.\n\r&Y$n yells: 'DEATH-BALL' as $e throws $s attack towards $N with amazing speed!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_deathball )) && (number_percent()*1.8) >= extra)
   {
     dam = number_range( 15+(lvl), 20+(lvl) );
     dam = add_kidamage( ch, victim, dam );
     damage(ch, victim, dam, TYPE_DEATHBALL);
     learn_from_success2( ch, gsn_deathball );
   }
   else
   {
     damage(ch, victim, 0, TYPE_DEATHBALL);
     learn_from_failure2( ch, gsn_deathball );
   }
   consume_ki( ch, "deathball");
   waitstate(ch, "deathball");
   addtimer( ch, "deathball", 8 ); 
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate(ch->fused_with, "deathball");
   return;
}

void do_mental( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_mental]/2;
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
   }
   if ( gettimer( ch, "mentalthrow" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }

   act( AT_WHITE, "\n\r$n closes $s eyes and focuses $s psychic energy.\n\r&WSlowly a HUGE piece of rock starts to hover above the ground.\n\r&YAll of a sudden the massive rock gets sent towards you!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou close your eyes and focus your psychic energy.\n\r&WSlowly a HUGE piece of rock starts to hover above the ground.\n\r&YAll of a sudden the massive rock gets sent towards $N!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r$n closes $s eyes and focuses $s psychic energy.\n\r&WSlowly a HUGE piece of rock starts to hover above the ground.\n\r&YAll of a sudden the massive rock gets sent towards $N!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_mental )) && (number_percent()*1.8) >= extra)
   {
     dam = number_range( 11+(lvl), 16+(lvl) );
     dam = add_kidamage( ch, victim, dam );
     damage(ch, victim, dam, TYPE_MENTAL);
     learn_from_success2( ch, gsn_mental );
   }
   else
   {
     damage(ch, victim, 0, TYPE_MENTAL);
     learn_from_failure2( ch, gsn_mental );
   }
   consume_ki( ch, "mentalthrow");
   waitstate(ch, "mentalthrow");
   addtimer( ch, "mentalthrow", 8 ); 
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate(ch->fused_with, "mentalthrow");
   return;
}

void do_blast( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   sh_int dam, extra, x, lvl, max;
   extra = dam = x = lvl = max = 0;


   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level[gsn_blast]/2;
   if ( ( victim = who_fighting( ch ) ) == NULL )
   {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
   }
   if ( gettimer( ch, "blast" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
    act( AT_WHITE, "\n\r&W$N stretches $S arms and prepares for an Energy Blast.\n\r&Y$N sends three bolts right at you!" , victim, NULL, ch, TO_CHAR);
    act( AT_WHITE, "\n\r&WYou stretch your arms and prepare for an Energy Blast.\n\r&YYou send three bolts right at $N!", ch, NULL, victim, TO_CHAR);
    act( AT_WHITE, "\n\r$n stretches $s arms and prepares for an Energy Blast.\n\r&Y$n sends three bolts right at $N!", ch, NULL, victim, TO_NOTVICT);
    for ( x = 0; x < 3; x++ )
    {
      extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
      if ( (can_use_skill(ch, number_percent(), gsn_blast )) && (number_percent()*1.8) >= extra)
      {
        dam = number_range( 2+(lvl), 6+(lvl) );
        dam = add_kidamage( ch, victim, dam );
	dam /= 4;
        damage(ch, victim, dam, TYPE_NBLAST);
        learn_from_success( ch, gsn_blast );
     }
     else
     {
       damage(ch, victim, 0, TYPE_NBLAST);   
       learn_from_failure( ch, gsn_blast );
     }
    } 
    consume_ki( ch, "blast" );
    waitstate( ch, "blast" );
    addtimer( ch, "blast", 8 );
    if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
    waitstate( ch->fused_with, "blast" );
    return;
}


void do_renzokubuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];
   lvl = 0;

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_renzoku]/2;

   damage = number_range( 30+lvl, 90+lvl );
   damage *= (get_curr_int(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }
  
   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy within $s hands.\n\r&RCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 10 energy beams towards the structure!" , ch, NULL, NULL, TO_ROOM);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy within your hands.\n\r&RCrackling energy surrounds you as you launch your attack!\n\r&YYou send 10 energy beams towards the structure!", ch, NULL, NULL, TO_CHAR);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's bolts impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour bolts impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support(ch);
   }
   consume_ki( ch, "renzoku" );
   waitstate( ch, "renzoku" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "renzoku" );
   return;
}


void do_scatterbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];
   lvl = 0;

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_scattershot]/2;

   damage = number_range( 30+lvl, 90+lvl );
   damage *= (get_curr_int(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }
  
   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&w$n raises $s arms and screams as $e prepares $s attack.\n\r&BCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 15 electric bolts right at the structure!" , ch, NULL, NULL, TO_ROOM);
   act( AT_WHITE, "\n\r&wYou raise your arms and scream as you prepare for your attack.\n\r&BCrackling energy surrounds you as you launch your attack!\n\r&YYou send 15 electric bolts right at the structure!", ch, NULL, NULL, TO_CHAR);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's bolts impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour bolts impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support(ch);
   }
   consume_ki( ch, "scattershot" );
   waitstate( ch, "scattershot" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "scattershot" );
   return;
}


void do_renzoku( CHAR_DATA *ch, char *argument )
{
  char arg1[MSL];
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_renzoku]/2;

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    	ch_printf( ch, "Target renzoku at who?.\n\r" );
    	return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
     	do_renzokubuild(ch, "structure");
     	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "Target must be inside this room.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
  if ( gettimer( ch, "renzoku" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy within $s hands.\n\r&RCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 10 energy beams towards you!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy within your hands.\n\r&RCrackling energy surrounds you as you launch your attack!\n\r&YYou send 10 energy beams towards $N!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy within $s hands.\n\r&RCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 10 energy beams towards $n!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_renzoku )) && (number_percent()*1.8) >= extra)
   {
        dam = number_range( 10+(lvl), 15+(lvl) );
    	dam = add_kidamage( ch, victim, dam );
    	damage(ch, victim, dam, TYPE_SCATTERSHOT);
    	learn_from_success2( ch, gsn_renzoku );
   }
   else
   {
    	damage(ch, victim, 0, TYPE_SCATTERSHOT);
    	learn_from_failure2( ch, gsn_renzoku );
   }
   consume_ki( ch, "renzoku" );
   waitstate( ch, "renzoku" );
   addtimer( ch, "renzoku", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "renzoku" );
   return;
}


void do_scattershot( CHAR_DATA *ch, char *argument )
{
  char arg1[MSL];
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

   if (IS_NPC(ch) || check_spec(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_scattershot]/2;

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    	ch_printf( ch, "Target scattershot at who?.\n\r" );
    	return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      	do_scatterbuild(ch, "structure");
     	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "Target must be inside this room.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
  if ( gettimer( ch, "scattershot" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$n raises $s arms and screams as $e prepares $s attack.\n\r&BCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 10 electric bolts right at you!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou raise your arms and scream as you prepare for your attack.\n\r&BCrackling energy surrounds you as you launch your attack!\n\r&YYou send 10 electric bolts right at $N!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n raises $s arms and screams as $e prepares $s attack.\n\r&BCrackling energy surrounds $n as $e launches $s attack!\n\r&Y$n sends 10 electric bolts right at $N!" , ch, NULL, victim, TO_NOTVICT);
   extra = get_curr_evasion( victim ) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_scattershot )) && (number_percent()*1.8) > extra)
   {
        dam = number_range( 8+(lvl), 13+(lvl) );
   	dam = add_kidamage( ch, victim, dam );
    	damage(ch, victim, dam, TYPE_SCATTERSHOT);
    	learn_from_success2( ch, gsn_scattershot );
   }
   else
   {
    	damage(ch, victim, 0, TYPE_SCATTERSHOT);
    	learn_from_failure2( ch, gsn_scattershot );
   }
   consume_ki( ch, "scattershot" );
   waitstate( ch, "scattershot" );
   addtimer( ch, "scattershot", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "scattershot" );
   return;
}

void do_sbcbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];
  
  if ( IS_NPC(ch)) 
  return;
  lvl = ch->pcdata->skill_level2[gsn_sbc]/2;
  damage = number_range( 30+lvl, 60+lvl );
  damage *= (get_curr_int(ch))/2;
  if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
  {
      	act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      	return;
  }
  if ( obj->item_type != ITEM_BUILDING )
  {
     	send_to_char("That isn't a building!\n\r", ch);
     	return;
  }

  if (!xIS_SET(ch->powerup, PU_FLYUP))
  {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
  }
  add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
  	act( AT_WHITE, "\n\r&BYou cross your arms and close your eyes\n\r&BYou grin and whisper 'Special Beam Cannon'.\n\r&WYou point your fingers and blast a white with blue energy towards the structure!", ch, NULL, NULL, TO_CHAR);
  	act( AT_WHITE, "\n\r&B$n crosses $s arms and closes $s eyes.\n\r&B$n grins and whispers 'Special Beam Cannon'.\n\r&W$n points $s fingers and blast a white with blue energy beam towards the structure!" , ch, NULL, NULL, TO_ROOM);
  obj->value[0] -= damage;
  sprintf( buf, "&R$n's energy beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "&RYour energy beam impacts the structure! [%d]", damage );
  act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);

  if (obj->value[0] <= 0)
  {
     	make_debris(obj);
     	planet_support(ch);
  }
  waitstate( ch, "beamcannon" );
  consume_ki( ch, "beamcannon" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "beamcannon" );
  return;
}


void do_sbc( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location = ch->in_room;
  char              arg1[MIL]; 
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  dam = extra = lvl = 0;
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_sbc]/2;
  if ( arg1[0] == '\0' )
  {
     ch_printf(ch, "Target special beam cannon at who?\n\r" );
     return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_sbcbuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
    if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
    {
       send_to_char("They aren't here\n\r", ch);
       return;
    }
    else
       map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }
  if ( can_attack( ch, victim ) == FALSE || victim == ch)
  {
      send_to_char("You can't do that.\n\r", ch);
      return;
  }
  if ( gettimer( ch, "beamcannon" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

  if ( !sense )
  warcry( ch, victim );
  	if ( sense )
	{
		act( AT_WHITE, "\n\r&B$n crosses $s arms and closes $s eyes.\n\r&B$n grins and whispers 'Special Beam Cannon'.\n\r&W$n points $s fingers and blast a white with blue energy beam towards the sky!" , ch, NULL, victim, TO_NOTVICT);
   		char_from_room( ch );
   		char_to_room( ch, victim->in_room );
        }
  	act( AT_WHITE, "\n\r&B$N crosses $S arms and closes $S eyes.\n\r&B$N grins and whispers 'Special Beam Cannon'.\n\r&W$N points $S fingers and blast a white with blue energy beam towards you!" , victim, NULL, ch, TO_CHAR);
  	act( AT_WHITE, "\n\r&BYou cross your arms and close your eyes\n\r&BYou grin and whisper 'Special Beam Cannon'.\n\r&WYou point your fingers and blast a white with blue energy towards $N!", ch, NULL, victim, TO_CHAR);
  	act( AT_WHITE, "\n\r&B$N crosses $S arms and closes $S eyes.\n\r&B$N grins and whispers 'Special Beam Cannon'.\n\r&W$N points $S fingers and blast a white with blue energy beam towards $n!" , victim, NULL, ch, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 3, 5)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_sbc )) && (number_percent()*1.8) >= extra)
  {
     dam = number_range( 5+(lvl), 13+(lvl) );
     dam = add_kidamage( ch, victim, dam );
     if ( sense )
     sensedamage( ch, victim, dam, TYPE_SBC );
     else
     damage(ch, victim, dam, TYPE_SBC);
     learn_from_success2( ch, gsn_sbc );
  }
  else
  {
     if ( sense )
     sensedamage( ch, victim, 0, TYPE_SBC );
     else
     damage(ch, victim, 0, TYPE_SBC);
     learn_from_failure2( ch, gsn_sbc );
  }
  char_from_room( ch );
  char_to_room( ch, location );
  waitstate( ch, "beamcannon" );
  consume_ki( ch, "beamcannon" );
  addtimer( ch, "beamcannon", 8 );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "sbc" );
  return;
}



void do_kamebuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_kame]/2;
   damage = number_range( 30+lvl, 60+lvl );
   damage *= (get_curr_con(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     	send_to_char("That isn't a building!\n\r", ch);
     	return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     	send_to_char("You need to fly above the structure!\n\r", ch);
     	return;
   }
   if (ch->mana <= 3000)
   {
     	send_to_char("You need to more ki to use this technique.\n\r", ch);
     	return;
   }
   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   if ( lvl < 5 )
   {
      act( AT_WHITE, "\n\r&BYou spread your arms and turn towards the structure.\n\rYou yell 'KA-ME-HA-ME-HA!\n\r&WYou push your arms forward as you launch a white energy blast towards it!", ch, NULL, NULL, TO_CHAR);
      act( AT_WHITE, "\n\r&B$n spreads $s arms and turns towards the structure.\n\r$n yells 'KA-ME-HA-ME-HA!\n\r&W$n pushes $s arms forward as $e launches a white energy blast towards the structure!", ch, NULL, NULL, TO_ROOM);
   }
   else if ( lvl < 10 )
   {
      act( AT_WHITE, "\n\r&BYou place your hands together at your side and begin gathering energy.\n\r&WYou focus on the structure and instantly transport farther from it.\n\r&RYou yell 'KA-ME-HA-ME-HA!'\n\r&zYou push your arms forward as you launch a blue energy blast directly it!", ch, NULL, NULL, TO_CHAR);
      act( AT_WHITE, "\n\r&B$n places $s hands together at $s side and begins gathering energy.\n\r&W$n focuses on the structure and instantly transports farther from it.\n\r&R$n yells 'KA-ME-HA-ME-HA!'\n\r&z$n pushes $s arms forward as $e launches a blue energy blast directly at the structure!", ch, NULL, NULL, TO_ROOM);
   }
   else if ( lvl >= 10 )
   {
      act( AT_WHITE, "\n\r&BYou place your hands together at your side and begin gathering energy.\n\r&WYou almost lose your balance as your attack reaches extreme power!\n\r&RYou yell 'KA-ME-HA-ME-HA!'\n\r&zYou push your arms forward launching an enormous &wwhite&z and&B blue&z energy blast at the structure!", ch, NULL, NULL, TO_CHAR);
      act( AT_WHITE, "\n\r&B$n places $s hands together at $s side and begins gathering energy.\n\r&W$n almost loses $s balance as $s attack reaches extreme power!\n\r&R$n yells 'KA-ME-HA-ME-HA!'\n\r&z$n pushes $s arms forward launching an enormous &wwhite&z and&B blue&z energy blast at the structure!", ch, NULL, NULL, TO_ROOM);
   }
   sprintf( buf, "&R$n's energy wave impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour energy wave impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   obj->value[0] -= damage;

   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support(ch);
   }
   consume_ki( ch, "kamehameha" );
   waitstate( ch, "kamehameha" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "kamehameha" );
   return;
}

void do_kame( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location = ch->in_room;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  bool map=FALSE, sense=FALSE;
  dam = extra = lvl = 0;
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_kame]/2;
  if ( arg1[0] == '\0' )
  {
     ch_printf(ch, "Target kamehameha at who?\n\r"  );
     return;
  }
  if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
  {
      do_kamebuild(ch, "structure");
      return;
  }
  if ( (!IS_NPC(ch) && IS_PLR_FLAG(ch, PLR_ONMAP)) || (IS_NPC(ch) && IS_ACT_FLAG(ch, ACT_ONMAP)))
  {
     if ( (victim = check_map(ch, arg1, gsn_tribeam)) == NULL )
     {
        send_to_char("They aren't here\n\r", ch);
        return;
     }
     else
        map = TRUE;
  }
  else if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    if ( ( victim = sense_for_victim( ch, arg1 ) ) == NULL )
    {
      send_to_char( "You can't sense them.\n\r", ch );
      return;
    }
    else
    sense = TRUE;
  }

  if ( can_attack( ch, victim ) == FALSE || victim == ch)
  {
      send_to_char("You can't do that.\n\r", ch);
      return;
  }
  if ( gettimer( ch, "kamehameha" ) > 0 )
  {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
  }

  if (check_spec(victim))
  return;
  if ( !sense )
  warcry( ch, victim );
    	if ( sense )
    	{
      		act( AT_WHITE, "\n\r&B$n spreads $s arms as $e begins to tunnel energy into $s hands.\n\r$n yells 'KA-ME-HA-ME-HA!\n\r&W$n pushes $s arms forward as $e launches a white energy blast towards the sky!" , ch, NULL, victim, TO_NOTVICT);
      		char_from_room( ch );
      		char_to_room( ch, victim->in_room );
    	}
      	act( AT_WHITE, "\n\r&BYou spread your arms and turn towards $N.\n\rYou yell 'KA-ME-HA-ME-HA!\n\r&WYou push your arms forward as you launch a white energy blast towards $N!", ch, NULL, victim, TO_CHAR);
      	act( AT_WHITE, "\n\r&B$n spreads $s arms and turns towards you.\n\r$n yells 'KA-ME-HA-ME-HA!\n\r&W$n pushes $s arms forward as $e launches a white energy blast towards you!" , ch, NULL, victim, TO_VICT);
      	act( AT_WHITE, "\n\r&B$n spreads $s arms and turns towards $N.\n\r$n yells 'KA-ME-HA-ME-HA!\n\r&W$n pushes $s arms forward as $e launches a white energy blast towards $N!" , ch, NULL, victim, TO_NOTVICT);
  extra = (get_curr_evasion( victim ) / number_range( 4, 8)) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_kame )) && (number_percent()*1.8) >= extra)
  {
     dam = number_range( 5+(lvl), 13+(lvl) );
     dam = add_kidamage( ch, victim, dam );
     if (map)
     destroy_area( victim );
     if ( sense )
     sensedamage( ch, victim, dam, TYPE_KAME );
     else
     damage(ch, victim, dam, TYPE_KAME);
     learn_from_success2( ch, gsn_kame );
  }
  else
  {
     if ( sense )
     sensedamage( ch, victim, 0, TYPE_KAME );
     else
     damage(ch, victim, 0, TYPE_KAME);
     learn_from_failure2( ch, gsn_kame );
  }
  if ( sense )
  {
  	char_from_room( ch );
  	char_to_room( ch, location );
  }
  consume_ki( ch, "kamehameha" );
  waitstate( ch, "kamehameha" );
  addtimer( ch, "kamehameha", 8 );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "kamehameha" );
  return;
}


void do_tail( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

 
  if (IS_NPC(ch) || check_spec(ch))
  return;
  if ( !IS_ICER(ch))
  {
     send_to_char( "Huh?\n\r", ch);
     return;
  }
  lvl = ch->pcdata->skill_level2[gsn_tail];
  if ( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  extra = (get_curr_evasion( victim ) ) - get_curr_accuracy(ch);
  if ( (can_use_skill2(ch, number_percent(), gsn_tail )) && (number_percent()*1.8) >= extra)
  {
      lvl = URANGE( 0, lvl / 2, 15 );
      dam = number_range(3+(lvl), 10+(lvl));
      dam = add_damage( ch, victim, dam );
      damage(ch, victim, dam, TYPE_TAIL);
      learn_from_success2( ch, gsn_tail );
  }
  else
  {
      damage(ch, victim, 0, TYPE_TAIL);
      learn_from_failure2( ch, gsn_tail );
  }
  waitstate( ch, "tail" );
  if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
  waitstate( ch->fused_with, "tail" );
  return;
}

void do_barrier( CHAR_DATA *ch, char *argument )
{
  sh_int lvl;
  if (IS_NPC(ch) || check_spec(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_ebar];
  if ( controlling_ki(ch) == TRUE)
  {
    send_to_char( "&RYou are already controlling an energy attack.\n\r", ch);
    return;
  }
  if (can_use_skill2(ch, number_percent(), gsn_ebar ))
  {
    ch->barrier = get_curr_pl2(ch);
    xSET_BIT(ch->powerup, KI_BAR);
    send_to_char( "\n\r&RYou spread your arms and create an Energy Barrier.\n\r", ch);
    ch_printf(ch, "&RYou estimate that the Energy Barrier has a defense power of %s\n\r", num_punct(ch->barrier));
    act( AT_RED, "$n spreads $s arms and creates an Energy Barrier.", ch, NULL, NULL, TO_ROOM );
    learn_from_success2( ch, gsn_ebar );             
  }
  else
  {
    send_to_char( "\n\r&RYou spread your arms and try to create an Energy Barrier...\n\r", ch);
    act( AT_RED, "$n spreads $s arms and tries to create an Energy Barrier...", ch, NULL, NULL, TO_ROOM );
    learn_from_failure2( ch, gsn_ebar );
  }
   waitstate(ch, "barrier");
   consume_ki( ch, "barrier");
   return;
}


void do_unsuppress( CHAR_DATA *ch, char *argument )
{
  if (xIS_SET(ch->powerup, PU_SUPPRESS))
   do_suppress(ch, "0");
  else if (ch->exp >= 250000)
   send_to_char("You aren't suppressed...\n\r", ch);
  else
   send_to_char("Huh?\n\r", ch);

   return;
}

void do_suppress( CHAR_DATA *ch, char *argument )
{
   char              arg1[MIL];

   argument = one_argument( argument, arg1 );

     if ( IS_NPC(ch) )
     return;
     if (check_spec(ch))
     return;

     if ( arg1[0] == '\0' )
     {
      if (!xIS_SET(ch->powerup, PU_SUPPRESS))
      send_to_char( "Suppress to what Powerlevel?\n\r", ch );
      else
      do_suppress( ch, "0");
      return;
     }
     if ( !is_number(arg1))
     {
      send_to_char( "It must be a number.\n\r", ch);
      return;
     }
     if ( !str_cmp( arg1, "0" ) )
     {
       xREMOVE_BIT(ch->powerup, PU_SUPPRESS);
       ch->suppress = 0;
       send_to_char("&BYou reveal your true powerlevel.\n\r", ch);
       act( AT_BLUE, "&B$n reveals $s true powerlevel.", ch, NULL, NULL, TO_CANSEE );
       if (ch->mana <= 100)
       {
        send_to_char("&RWith some struggles, you manage to unsuppress...\n\r",ch);
       }
       return;
     }
     else if (atoi(arg1) > (ch->exp * ch->mod))
     {
        do_suppress(ch, "0");
        return;
     }
     else if (atoi(arg1) < (ch->exp * ch->mod))
     {
       if (can_use_skill(ch, number_percent(), gsn_suppress ))
       {
        send_to_char("&BYou suppress your powerlevel.\n\r", ch);
        act( AT_BLUE, "&B$n suppresses $s powerlevel.", ch, NULL, NULL, TO_CANSEE );
        ch->suppress = atoi(arg1);
        if (ch->suppress < 5)
        ch->suppress = 5;
        xSET_BIT(ch->powerup, PU_SUPPRESS);
        learn_from_success(ch, gsn_suppress);
       }
       else
       {
        send_to_char("&GYou try to suppress your powerlevel, but you fail.\n\r", ch);
        act( AT_BLUE, "&G$n tries to suppress $s powerlevel, but $e fails.", ch, NULL, NULL, TO_CANSEE );
        consume_ki( ch, "suppress" );
        learn_from_failure(ch, gsn_suppress);
       }
     }
     else if (atoi(arg1) == (ch->exp * ch->mod))
     {
       send_to_char("What would be the point of that?",ch);
       return;
     }
     return;
}


void do_absorb( CHAR_DATA *ch, char *argument )
{
   char              buf[MIL];
   CHAR_DATA       * victim;
   int               pl, plcheck, drain, bonus, todrain;
   sh_int            extra;

   plcheck = drain = 0;
   pl = (get_curr_pl(ch) / 25000) + 1000;
   extra = (get_curr_pl(ch) / 45000) + 1000;
   if ( ch->exp < 5000000 )
   bonus = 5;
   else
   bonus = 1; 

     if ( IS_NPC(ch) )
     return;
     if (check_spec(ch))
     return;
      if ( ( victim = who_fighting( ch ) ) == NULL )
      {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
      }
      if (check_spec(victim))
      return;
      if ( victim == ch )
      {
       send_to_char( "Absorb yourself?\n\r", ch );
       return;
      }
      if (can_use_skill2(ch, number_percent(), gsn_absorb ))
      {
       act( AT_RED, "&r$N grabs you by the throat and drains energy from you.",victim, NULL, ch, TO_CHAR);
       act( AT_RED, "&rYou grab $N by the throat and you drain energy from $m." , ch, NULL, victim, TO_CHAR);
       act( AT_RED, "&r$n grabs $N by the throat and $e drains energy from $M." , ch, NULL, victim, TO_NOTVICT);
       if (can_use_skill2(ch, number_percent(), gsn_grip ) && ch->style == STYLE_AGGRESSIVE)
       {
        act( AT_RED, "&r$N tightens the grip as $E continues to absorb you.",victim, NULL, ch, TO_CHAR);
        act( AT_RED, "&rYou tighten your grip as you continue to absorb $m." , ch, NULL, victim, TO_CHAR);
        act( AT_RED, "&r$n tightens the grip as $e continues to absorb $M." , ch, NULL, victim, TO_NOTVICT);
        pl += extra;
        learn_from_success2(ch, gsn_grip);
       }
       if ( ch->exp >= 5000000 && ch->exp < 25000000)  {plcheck = ch->pcdata->learned[gsn_chikone] * 250000;}
       else if ( ch->exp >= 25000000 && ch->exp < 55000000){plcheck = ch->pcdata->learned[gsn_chiktwo] * 550000; }
       else if ( ch->exp >= 55000000 && ch->exp < 75000000){plcheck = ch->pcdata->learned[gsn_chikthree] * 750000; }
       else if ( ch->exp >= 75000000 && ch->exp < 100000000){plcheck = ch->pcdata->learned[gsn_chikfour] * 1000000;}
       else if ( ch->exp >= 100000000 && ch->exp < 125000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}
       if ( pl <= 0 )
       pl = 2;
       todrain = pl;
       if (ch->exp >= 5000000)
          if (  plcheck < ch->exp)
            todrain = 0;
       if ( is_clanned(ch) == TRUE )
       {
        if (ch->exp > 125000000)
         todrain = 0;
       }
       else
       {
        if (ch->exp > 100000000)
         todrain = 0;
       }

       ch->exp += URANGE( 0, todrain * bonus, 125000);
       drain = pl / 20;
       if (!IS_NPC(victim))
       victim->exp -= URANGE( 0, drain, 125000);
/*       if ( is_clanned(ch) == TRUE )
       {
        if (ch->exp > 125000000)
         ch->exp = 125005000;
       }
       else
       {
        if (ch->exp > 100000000)
         ch->exp = 100005000;
       }*/
       sprintf(buf, "&rYou have just drawn %d PL from %s. Your new basic PL is %s.", todrain, victim->name, num_punct(ch->exp));
       act( AT_RED, buf, ch, NULL, victim, TO_CHAR);
       one_hit( ch, victim, TYPE_UNDEFINED );
      learn_from_success2(ch, gsn_absorb);
      }
      else
      {
       act( AT_RED, "&r$N grabs you by the throat and tries to drain energy from you.",victim, NULL, ch, TO_CHAR);
       act( AT_RED, "&rYou grab $N by the throat and you try to drain energy from $m." , ch, NULL, victim, TO_CHAR);
       act( AT_RED, "&r$n grabs $N by the throat and $e tries to drain energy from $M." , ch, NULL, victim, TO_NOTVICT);
       learn_from_failure2(ch, gsn_absorb);
      }
     consume_ki( ch, "absorb" );
     waitstate( ch, "absorb" );
     if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
     waitstate( ch->fused_with, "absorb" );
     one_hit(ch, victim, TYPE_HIT);
     return;
}

void do_heal( CHAR_DATA *ch, char *argument )
{
   char arg[MIL];
   CHAR_DATA *victim;
   sh_int heal, lvl;


   argument = one_argument( argument, arg );

   if ( IS_NPC(ch))
   return;

   if (IN_ARENA(ch))
   {
       send_to_char( "Huh?\n\r", ch);
       return;
   }
    lvl = ch->pcdata->skill_level2[gsn_heal]/2;
   if ( arg[0] == '\0' )
    {
        ch_printf( ch, "Heal who?\n\r");
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "You are fighting.\n\r", ch );
        return;
    }

    if (check_spec(ch))
    return;
    if  ( get_timer(ch, TIMER_HEAL) > 0)
    {
     send_to_char("\n\rYou can't heal again yet, your body needs to recover.\n\r", ch);
     WAIT_STATE( ch, 5 );
     return;
    }

    if ( !can_use_skill2( ch, number_percent(), gsn_heal ) )
    {
       act( AT_SKILL, "You failed.",  ch, NULL, victim, TO_CHAR    );
       learn_from_failure2( ch, gsn_heal );
    }
    else
    {
      if (victim == ch)
      {
            act( AT_SKILL, "You heal your wounds.",  ch, NULL, victim, TO_CHAR    );
            act( AT_SKILL, "$n heal $s wounds.",  ch, NULL, victim, TO_NOTVICT );
      }
      else
      {
            act( AT_SKILL, "You heal $N's wounds.",  ch, NULL, victim, TO_CHAR    );
            act( AT_SKILL, "$n heal $N's wounds.",  ch, NULL, victim, TO_NOTVICT );
            act( AT_SKILL, "$n heal your wounds.", ch, NULL, victim, TO_VICT    );
      }
      if ( get_timer( victim, TIMER_RECENTFIGHT) <= 0 )
      heal = URANGE( 20, lvl * number_range(5, 20), 150 );
      else
      heal = victim->max_hit * 0.80;

      learn_from_success2( ch, gsn_heal );
      victim->hit = URANGE( 0, victim->hit + heal, victim->max_hit);
      update_pos( victim );
      act( AT_SKILL, "$n heals your wounds.", ch, NULL, victim, TO_VICT    );
      add_timer( ch, TIMER_HEAL, 20, NULL, 0 );
    }
    waitstate( ch, "heal" );
    if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
    waitstate( ch->fused_with, "heal" );
    consume_ki( ch, "heal");
    return;
}

CHAR_DATA *sense_for_victim( CHAR_DATA *ch, char *name )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *was_in_room;
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    victim = NULL;

    was_in_room = ch->in_room;
     for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
     {
      char_from_room( ch );
      char_to_room( ch, pexit->to_room );

      if ( ( victim = get_char_room( ch, name ) ) == NULL )
      found = FALSE;

      if ( ( victim = get_char_room( ch, name ) ) != NULL )
      {
       found = TRUE;
       if (IS_SET(ch->in_room->room_flags2, ROOM_NOSNIPE))
       {
        found = FALSE;    
        break;
       }
       break;
      }
     }
      char_from_room( ch );
      char_to_room( ch, was_in_room );

      if ( found )
      return victim;

  return NULL;
}



void do_fuse(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MIL];
  char name1[MIL];
  char name2[MIL];
  char buf[MIL];
  sh_int length1, length2;


  length1 = length2 = 0;
  if (IS_NPC(ch))
  return;
  argument = one_argument( argument, arg );
  if (check_spec(ch))
  return;
  if (is_awake(ch))
  return;


	switch(ch->substate)
	{
		default:
                        if ( arg[0] == '\0' )
                        {
                            send_to_char( "With who would you like to perform the fusion dance?\n\r", ch );
                            return;
                        }
                        if ( ( victim = get_char_room( ch, arg ) ) == NULL )
                        {
                            send_to_char( "They aren't here.\n\r", ch );
                            return;
                        }                            
                        if ( victim == ch )
                        {
                            send_to_char( "What would be the point of this?\n\r", ch );
                            return;
                        }
                        if ( ch->exp < 125000000 )
                        {
                            send_to_char( "You aren't strong enough to perform a correct fusion.\n\r", ch );
                            return;
                        }
                        if ( victim->exp < 125000000 )
                        {
                            send_to_char( "They aren't strong enough to perform a correct fusion.\n\r", ch );
                            return;
                        }
                        if (xIS_SET(ch->act, PLR_FUSED))
                        {
                            send_to_char( "Your already fused with someone.\n\r", ch );
                            return;
                        }
                        if (xIS_SET(victim->act, PLR_FUSED))
                        {
                            send_to_char( "They are already fused with someone.\n\r", ch );
                            return;
                        }
                        if ( !victim->desc )
                        {
                            send_to_char( "That player is link-dead.\n\r", ch );
                            return;
                        }
                        if (ch->race != victim->race)
                        {
                            send_to_char( "Your fusing partner isn't the same race as you.\n\r", ch );
                            return;
                        }
                        if ( victim->consent != ch )
                        {
                            send_to_char( "They haven't given you their consent to fuse with you.\n\r", ch );
                            return;
                        }
                        if ( get_curr_pl2(ch) != get_curr_pl2(victim) )
                        {
                            send_to_char( "Your powerlevels need to be the same to perform the fusion dance.\n\r", ch );
                            return;
                        }
                        ch->fused_with = victim;
                        victim->fused_with = ch;
        	        add_timer( ch, TIMER_DO_FUN, 1, do_fuse, 1);
        	        add_timer( victim, TIMER_DO_FUN, 1, do_fuse, 1);
                        ch_printf( ch, "&RYou take two steps away from %s.\n\r", victim->name );
                        act( AT_ACTION, "&RYou take two steps away from $n.", ch, NULL, victim, TO_VICT    );
                        act( AT_ACTION, "&R$n and $N take two steps away from each other.",  ch, NULL, victim, TO_NOTVICT );
			ch->dest_buf = str_dup(arg);
		return;
		case 1:
			if(!ch->dest_buf)
                        return;
                        victim = ch->fused_with;
                        ch_printf( ch, "&RYou crouch as you take the fusion stance, pointing your fingers away from %s.\n\r", victim->name );
                        act( AT_ACTION, "&RYou crouch as you take the fusion stance, pointing your fingers away from $n.", ch, NULL, victim, TO_VICT    );
                        act( AT_ACTION, "&R$n and $N crouch as they both take the fusion stance, pointing their fingers away from each other.",  ch, NULL, victim, TO_NOTVICT );
        	        add_timer( ch, TIMER_DO_FUN, 1, do_fuse, 2);
        	        add_timer( victim, TIMER_DO_FUN, 1, do_fuse, 2);
			ch->dest_buf = str_dup(arg);

		return;
		case 2:
                        if(!ch->dest_buf)
                        return;
                        victim = ch->fused_with;
                        ch_printf( ch, "&RYou swing your arms up and towards %s as you take a few steps towards %s.\n\r", victim->name, victim->sex == 1 ? "him" : victim->sex == 2 ? "her" : "it" );
                        act( AT_ACTION, "&RYou swing your arms up and towards $n as you take a few steps towards $m.", ch, NULL, victim, TO_VICT    );
                        act( AT_ACTION, "&R$n and $N both swing their arms up and take a few steps towards each other.",  ch, NULL, victim, TO_NOTVICT );
        	        add_timer( ch, TIMER_DO_FUN, 1, do_fuse, 3);
        	        add_timer( victim, TIMER_DO_FUN, 1, do_fuse, 3);
			ch->dest_buf = str_dup(arg);

		return;
		case 3:
                        if(!ch->dest_buf)
                        return;
                        victim = ch->fused_with;
                        ch_printf( ch, "&RYou yell: 'FU-SION-HAAAAA!!!' as you make contact with your index fingers with %s.\n\r", victim->name );
                        act( AT_ACTION, "&RYou yell: 'FU-SION-HAAAA!!!' as you make contact with your index fingers with $n.", ch, NULL, victim, TO_VICT    );
                        act( AT_ACTION, "&R$n and $N both yell: 'FU-SION-HAAAA!!!' as they both make contact with their index fingers.",  ch, NULL, victim, TO_NOTVICT );
        	        add_timer( ch, TIMER_DO_FUN, 1, do_fuse, 4);
        	        add_timer( victim, TIMER_DO_FUN, 1, do_fuse, 4);
                        ch->dest_buf = str_dup(arg);

		return;
		case 4:
			if(ch->dest_buf ==NULL)
                        return;
                        victim = ch->fused_with;
                        if ( ch->dest_buf )
                        {
                          strcpy(arg, ch->dest_buf);
                          DISPOSE( ch->dest_buf);
                        }

                break;
/*		case SUB_TIMER_DO_ABORT:
                       victim = ch->fused_with;
			if ( xIS_SET(ch->act, PLR_INVIS2) )
                        DISPOSE(victim->dest_buf);
			else
                        DISPOSE( ch->dest_buf );
                        ch->substate = SUB_NONE; 
 		        victim->substate = SUB_NONE;
    			send_to_char("\n\r&RYou are interupted and fail to finish the fusion dance.\n\r", ch);
			send_to_char("\n\r&RYou are interupted and fail to finish the fusion dance.\n\r", victim);

			xREMOVE_BIT(ch->act, PLR_FUSED);
                        xREMOVE_BIT(victim->act, PLR_FUSED);
                        xREMOVE_BIT(victim->act, PLR_INVIS2);
                        xREMOVE_BIT(ch->act, PLR_INVIS2);
                        ch->fused_with = NULL;
                        victim->fused_with = NULL;
                        victim = NULL;
			ch = NULL;
			return;
		break;*/
	}

  victim = ch->fused_with;
  ch_printf( ch, "&RYou feel that you both have become one body, one mind and one soul. You both now share equal control upon yourselves.\n\r", victim->name );
  act( AT_ACTION, "&R$n and $N are blinded by a flashing light. As the light clears out, a new body has emerged from both of the fighters.",  ch, NULL, victim, TO_NOTVICT );
  if (strlen(ch->name) > 3)
  length1 = strlen(ch->name)/2;
  if (strlen(victim->name) > 3)
  length2 = strlen(victim->name)/2;
  xSET_BIT(ch->act, PLR_FUSED);
  xSET_BIT(victim->act, PLR_FUSED);
  xSET_BIT(victim->act, PLR_INVIS2);
  strcpy(name1, ch->name);
  name1[length1] = 0;
  strcpy(name2, victim->name);
  name2[length2] = 0;
  sprintf(buf, "%s%s", name1, strlower(name2));
  ch->fusionname = STRALLOC(buf);
  victim->fusionname = STRALLOC(buf);
  ch->desc->fused_with = victim->desc;
  ch->fused_with = victim;
  victim->fused_with = ch;
  victim->mod = ch->mod;
  ch->mod += victim->mod;
  if (ch->mod > 15)
  ch->mod = 15;
  if (victim->mod > 15)
  victim->mod = 15;
  ch->kimod += victim->kimod;
  if (ch->kimod > 1.3)
  ch->kimod = 1.3;
  ch->mod_str += get_curr_str(victim) / 15;
  ch->mod_int += get_curr_int(victim) / 15;
  ch->mod_dex += get_curr_dex(victim) / 15;
  ch->mod_wis += get_curr_wis(victim) / 15;
  ch->mod_con += get_curr_con(victim) / 15;
  ch->mod_cha += get_curr_cha(victim) / 15;
  char_from_room( victim );
  char_to_room( victim, get_room_index(98));
  learn_from_success2( ch, gsn_fuse );
  learn_from_success2( victim, gsn_fuse );

}


void do_defuse( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;

  if (!xIS_SET(ch->act, PLR_FUSED))
  {
   send_to_char( "You aren't fused with a person.\n\r", ch );
   return;
  }
  victim = ch->fused_with;
  if (!xIS_SET(victim->act, PLR_FUSED))
  {
   send_to_char( "No one is fused with you.\n\r", ch );
   return;
  }
  clear_mods( ch );
  clear_mods( victim );
  send_to_char("\n\r&YThe thirty minutes come up, as your fusion ends.\n\rYour bodies split into two again.", ch );
  send_to_char("\n\r&YThe thirty minutes come up, as your fusion ends.\n\rYour bodies split into two again.", victim );
  ch->desc->fused_with = NULL;
  victim->desc->fused_with = NULL;
  ch->fused_with = NULL;
  victim->fused_with = NULL;
  STRFREE(ch->fusionname);
  STRFREE(victim->fusionname);
  char_from_room( victim );
  char_to_room( victim, ch->in_room);
  xREMOVE_BIT(victim->act, PLR_FUSED);
  xREMOVE_BIT(ch->act, PLR_FUSED);
  xREMOVE_BIT(ch->act, PLR_INVIS2);
  xREMOVE_BIT(victim->act, PLR_INVIS2);

   xCLEAR_BITS( ch->powerup );
   xCLEAR_BITS( victim->powerup );
  if (IS_ICER(ch))
  xSET_BIT(victim->powerup, PU_TRANSFORM1);
  if (IS_MUTANT(ch))
  add_mutate( ch, 1 );
  if (IS_ICER(victim))
  xSET_BIT(victim->powerup, PU_TRANSFORM1);
  if (IS_MUTANT(victim))
  add_mutate( victim, 1 );
  return;
}

void do_consent( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MIL];

  argument = one_argument( argument, arg );

  if (IS_NPC(ch))
  return;
  if ( ch->pcdata->learned[gsn_fuse] <= 0 )
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char("They aren't here!\n\r", ch);
    return;
  }
  if ( victim == ch )
  {
    send_to_char("What would be the point of this?\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Thats a mob!\n\r", ch);
    return;
  }
  if ( victim->pcdata->learned[gsn_fuse] <= 0 )
  {
    send_to_char("There wouldn't be no point of this if they do not know fusion.\n\r", ch);
    return;
  }

  if (xIS_SET(ch->act, PLR_FUSED))
  {
    send_to_char("You are already fused!\n\r", ch);
    return;
  }
  if (xIS_SET(victim->act, PLR_FUSED))
  {
    send_to_char("They are already fused!\n\r", ch);
    return;
  }
  ch->consent = victim;
  ch_printf(ch, "\n\rYou have consented fusion with: %s.\n\r", ch->consent->name);
  ch_printf(victim, "\n\r%s has consented fusion with you.\n\r", ch->name);
  return;
}

void do_dragonfistbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_dragonfist]/2;
   damage = number_range( 60+lvl, 200+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }

   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&wYou concentrate gathering your ki in one of your hands.\n\r&YYou finish gathering most of your energy into your fist and you scream: 'DRAGON FIST'!!!\n\r&OYou lurge towards $N and as you throw your fist towards him, a golden dragon appears symbolizing its strength!", ch, NULL, NULL, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n concentrates in gathering $s ki in one of $s hands.\n\r&Y$n finishes gathering most of $s energy into $s fist and $e screams: 'DRAGON FIST'!!!\n\r&O$n lurges towards the structure as $e throws $s fist towards it and a golden dragon appears symbolizing its strength!" , ch, NULL, NULL, TO_ROOM);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's energy attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour energy impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   consume_ki( ch, "dfist" );
   waitstate( ch, "dfist" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "dfist" );
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support( ch );
   }
   return;
}

void do_finalflashbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_finalflash]/2;
   damage = number_range( 60+lvl, 200+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }

   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&wYou spread your arms as you begin to gather energy.\n\r&CLight Blue energyballs appear surrounding each of your hands.\n\r&WYou join both hands infront of you as you yell: 'FINAL-FLASH', and a beam of blue energy is released towards the structure!!!" , ch, NULL, NULL, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n spreads both of $s arms as $e begins to gather energy.\n\r&CLight Blue energyballs appear surrounding each of $s hands.\n\r&W$n joins both of $s hands infront of $m as $e yells: 'FINAL-FLASH', and a beam of blue energy is released towards the structure!!!" , ch, NULL, NULL, TO_ROOM);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's energy attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour energy impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   consume_ki( ch, "fflash" );
   waitstate( ch, "fflash" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "fflash" );
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support( ch );
   }
   return;
}


void do_dragonfist( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_dragonfist]/2;
  argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    ch_printf( ch, "Target Dragonfist at who?\n\r");
    return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      do_dragonfistbuild(ch, "structure");
      return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "Target must be inside this room.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if ( gettimer( ch, "dfist" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$N concentrates in gathering $s ki in one of $s hands.\n\r&Y$N finishes gathering most of $s energy into $s fist and $e screams: 'DRAGON FIST'!!!\n\r&O$N lurges towards you as $e throws $s fist towards you and a golden dragon appears symbolizing its strength!" , victim, NULL, ch, TO_CHAR);
   act( AT_WHITE, "\n\r&wYou concentrate gathering your ki in one of your hands.\n\r&YYou finish gathering most of your energy into your fist and you scream: 'DRAGON FIST'!!!\n\r&OYou lurge towards $N and as you throw your fist towards him, a golden dragon appears symbolizing its strength!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$N concentrates in gathering $s ki in one of $s hands.\n\r&Y$N finishes gathering most of $s energy into $s fist and $e screams: 'DRAGON FIST'!!!\n\r&O$N lurges towards $n as $e throws $s fist towards $m and a golden dragon appears symbolizing its strength!" , victim, NULL, ch, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_dragonfist )) && (number_percent()*1.8) >= extra)
   {
    dam = number_range( 15+(lvl), 20+(lvl) );
    dam = add_kidamage( ch, victim, dam );
    damage( ch, victim, dam, TYPE_DRAGONFIST );
    learn_from_success2( ch, gsn_dragonfist );
   }
   else
   {
    damage( ch, victim, dam, TYPE_DRAGONFIST );
    learn_from_failure2( ch, gsn_dragonfist );
   }
   consume_ki( ch, "dfist" );
   waitstate( ch, "dfist" );
   addtimer( ch, "dfist", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "dfist" );
}

void do_finalflash( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;
  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_finalflash]/2;
  argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    ch_printf( ch, "Target Final Flash at who?\n\r");
    return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      do_finalflashbuild(ch, "structure");
      return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "Target must be inside this room.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if ( gettimer( ch, "fflash" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$N spreads both of $s arms as $e begins to gather energy.\n\r&CLight Blue energyballs appear surrounding each of $s hands.\n\r&W$N joins both of $s hands infront of $m as $e yells: 'FINAL-FLASH', and a beam of blue energy is released towards you!!!" , victim, NULL, ch, TO_CHAR);
   act( AT_WHITE, "\n\r&wYou spread your arms as you begin to gather energy.\n\r&CLight Blue energyballs appear surrounding each of your hands.\n\r&WYou join both hands infront of you as you yell: 'FINAL-FLASH', and a beam of blue energy is released towards $N!!!" , ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$N spreads both of $s arms as $e begins to gather energy.\n\r&CLight Blue energyballs appear surrounding each of $s hands.\n\r&W$N joins both of $s hands infront of $m as $e yells: 'FINAL-FLASH', and a beam of blue energy is released towards $n!!!" , victim, NULL, ch, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_finalflash )) && (number_percent()*1.8) >= extra)
   {
    dam = number_range( 12+(lvl), 17+(lvl) );
    dam = add_kidamage( ch, victim, dam );
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_success2( ch, gsn_finalflash );
   }
   else
   {
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_failure2( ch, gsn_finalflash );
   }
   consume_ki( ch, "fflash" );
   waitstate( ch, "fflash" );
   addtimer( ch, "fflash", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "fflash" );
}


void do_galic( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim=NULL;
  char arg1[MIL];
  CHAR_DATA *inroom=NULL;
  ROOM_INDEX_DATA *location;
  sh_int dam, extra, manaf, lvl;
  long long int pl;
  dam = extra = manaf = pl = lvl = 0;
  location = ch->in_room;
  argument = one_argument( argument, arg1 );


  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_galic]/2;
   if (ch->mana <  (ch->max_mana/1.11))
   {
         send_to_char( "You need more ki to use this technique.\n\r", ch);
         return;
   }

   if ( arg1[0] == '\0' )
   {
       if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
       return;
     
       if (IS_SET( ch->in_room->area->flags, AFLAG_NOPKILL ))
       {
         send_to_char("You can only do this at a PK Zone...\n\r", ch);
         return;
       }
       if ( IS_SET(ch->in_room->area->flags, AFLAG_NOPKILL) && !IS_SET(ch->in_room->area->flags, AFLAG_PLANET))
       {
           send_to_char("You can't do that here", ch);
           return;
       }
       act( AT_RED, "\n\r&YYou raise your arms above your head and you start to tunnel energy.\n\r&zThe sky turns dark and crackling lightning charges you for your attack...\n\r&RYou scream GAAAAAALIC-GUN!!!! and you throw a massive blast of energy away from you!!!\n\r", ch, NULL, NULL, TO_CHAR );
       for ( inroom = ch->in_room->first_person; inroom; inroom = inroom->next_in_room )
       {
         extra = (get_curr_evasion( inroom ) / number_range(2, 3)) - get_curr_accuracy(ch);
         if ( (can_use_skill2(ch, number_percent(), gsn_galic )) && (number_percent()*1.8) >= extra)
         {

            if (inroom == ch
            || is_same_group( inroom, ch)
            || xIS_SET(inroom->act, PLR_SPEC)
	    || (xIS_SET(inroom->act, PLR_WIZINVIS) && ( ch->level < inroom->level)) 
            || can_attack( ch, inroom ) == FALSE)
            continue;

	    manaf = (ch->mana / 2700 ); 
	    dam = number_range( 13+(lvl), 20+(lvl) );
  	    if (ch->kimod > 0)
	    dam *= ch->kimod;
            if (manaf >= 10)
            dam += 10;
            else
            dam += manaf;
 
            dam = add_kidamage( ch, inroom, dam );
            act( AT_YELLOW, "&Y$N raises $S arms above $S head and starts to tunnel energy.\n\r&zThe sky turns dark and  crackling lightning charges $N for $S attack...\n\r&R$N screams GAAAAAAALIC-GUN!!! and throws a massive blast of energy away from $M", inroom, NULL, ch, TO_CHAR);
             if (inroom->style == STYLE_DEFENSIVE && (get_curr_pl(ch) <= get_curr_pl(inroom)))
             {
                 act( AT_GREEN, "You block $N's attack." , inroom, NULL, ch, TO_CHAR );
                 act( AT_GREEN, "$N blocks your attack.", ch, NULL, inroom, TO_CHAR );
      	         inroom->mana -= (inroom->max_mana / number_range( 20, 30) );
             }
             damage(ch, inroom, dam, TYPE_NGALIC);
             learn_from_success2(ch, gsn_galic);
             ch->mana = 1;
         }
         else
         {
            ch_printf(ch, "&YThe sky turns black as you try to gather enough energy for your attack but you fail.");
            if (inroom == ch)
            continue;
            if ( xIS_SET(inroom->act, PLR_SPEC))
            continue;
         }
        ch->mana -= 500;
       }
       learn_from_failure2( ch, gsn_galic );
       return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "They aren't here.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE )
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }

   manaf = (ch->mana / 2700 );
   dam = number_range( 13+(lvl), 20+(lvl) );
   if (ch->kimod > 0)
   dam *= ch->kimod;
   if (manaf >= 6)
   dam += 6;
   else
   dam += manaf;

   
   if (victim->style == STYLE_DEFENSIVE)
   dam *= 0.85;
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
   if (dam < 0)
   dam = 0;
   dam = add_kidamage( ch, victim, dam );
   if ( victim == ch )
   {
     send_to_char( "Target yourself? You been stupified by Felten?\n\r", ch );
     return;
   }
   if (check_spec(ch))
   return;
   if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
   {
    set_char_color( AT_MAGIC, ch );
    send_to_char( "A magical force prevents you from attacking.\n\r", ch );
    return;
   }

   warcry( ch, victim );
   act( AT_YELLOW, "&Y$n raises $s arms above $s head and starts to tunnel energy.\n\r&zThe sky turns dark and crackling lighting charges $n for $s attack...\n\r&R$n screams GAAAAAALIC-GUN!!! and throws a massive blast of energy away from $m", ch, NULL, victim, TO_VICT );
   act( AT_RED, "\n\r&YYou raise your arms above your head and you start to tunnel energy.\n\r&zThe sky turns dark and crackling lightning charges you for your attack...\n\r&RYou scream GAAAAAALIC-GUN!!!! and you throw a massive blast of energy away from you!!!\n\r", ch, NULL, victim, TO_CHAR );
   act( AT_YELLOW, "$n raises $s arms above $s head and starts to tunnel energy.\n\r&zThe sky turns dark and crackling lightning charges $n for $s attack...\n\r&R$n screams GAAAAAALIC-GUN!!! and throws a massive blast of energy away from $m!!\n\r", ch, NULL, victim, TO_NOTVICT );
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_galic )) && (number_percent()*1.8) >= extra)
   {
    ch->mana = 1;
    damage(ch, victim, dam, TYPE_NGALIC);
    learn_from_success2( ch, gsn_galic );
   }
   else
   {
    damage(ch, victim, 0, TYPE_NGALIC);
    learn_from_failure2( ch, gsn_galic );
    ch->mana -= 500;
   }
}

void do_rocketpunch( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_rocketpunch]/2;
  argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    	ch_printf(ch,  "Target Rocketpunch at who?\n\r" );
    	return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      	do_rocketpunchbuild(ch, "structure");
      	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "Target must be inside this room.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
   if ( gettimer( ch, "rocketpunch" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&B$n aims $s right arm towards you as $e yells: 'ROCKET PUNCH'!\n\r&b$n launches $s right hand towards you!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy in your right arm.\n\r&BYou aim your right arm towards $N as you yell: 'ROCKET PUNCH'!\n\r&bYou launch your right hand towards $N!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&B$n aims $s right arm towards $N as $e yells: 'ROCKET PUNCH'!\n\r&b$n launches $s right hand towards $N!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_rocketpunch )) && (number_percent()*1.8) >= extra)
   {
        dam = number_range( 12+(lvl), 17+(lvl) );
    	dam = add_kidamage( ch, victim, dam );
    	damage(ch, victim, dam, TYPE_ROCKETPUNCH);
    	learn_from_success2( ch, gsn_rocketpunch );
   }
   else
   {
    	damage(ch, victim, 0, TYPE_ROCKETPUNCH);
    	learn_from_failure2( ch, gsn_rocketpunch );
   }
   consume_ki( ch, "rocketpunch" );
   waitstate( ch, "rocketpunch" );
   addtimer( ch, "rocketpunch", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "rocketpunch" );
   return;
}


void do_hellsflash( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_hellsflash]/2;
  argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    	ch_printf(ch,  "Target Hell's Flash who?\n\r" );
    	return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      	do_hellsflashbuild(ch, "structure");
      	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "Target must be inside this room.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
   if ( gettimer( ch, "hellsflash" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&R$n takes off $s right hand as $e yells: 'HELL'S FLASH'!\n\r&r$n launches a large red beam towards you!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy on your right arm.\n\r&RYou take off your right hand as you yell: 'HELL'S FLASH'!\n\r&rYou launch a large red beam towards $N!", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&R$n takes off $s right hand as $e yells: 'HELL'S FLASH'!\n\r&r$N launches a large red beam towards $N!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_hellsflash )) && (number_percent()*1.8) >= extra)
   {
        dam = number_range( 15+(lvl), 20+(lvl) );
    	dam = add_kidamage( ch, victim, dam );
    	damage(ch, victim, dam, TYPE_HELLSFLASH);
    	learn_from_success2( ch, gsn_hellsflash );
   }
   else
   {
    	damage(ch, victim, 0, TYPE_HELLSFLASH);
    	learn_from_failure2( ch, gsn_hellsflash );
   }
   consume_ki( ch, "hellsflash" );
   waitstate( ch, "hellsflash" );
   addtimer( ch, "hellsflash", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "hellsflash" );
   return;
}

void do_rocketpunchbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_rocketpunch]/2;
   damage = number_range( 60+lvl, 250+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }
   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&B$n aims $s right arm towards the structure as $e yells: 'ROCKET PUNCH'!\n\r&b$n launches $s right hand towards it!" , ch, NULL, NULL, TO_ROOM);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy in your right arm.\n\r&BYou aim your right arm towards the structure as you yell: 'ROCKET PUNCH'!\n\r&bYou launch your right hand towards it!", ch, NULL, NULL, TO_CHAR);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's ki attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour ki attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   if (obj->value[0] <= 0)
   {
     	make_debris(obj);
     	planet_support( ch );
   }
   consume_ki( ch, "rocketpunch" );
   waitstate( ch, "rocketpunch" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "rocketpunch" );
   return;
}

void do_hellsflashbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_hellsflash]/2;
   damage = number_range( 60+lvl, 250+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }
   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&w$n grins as $e gathers energy in $s right arm.\n\r&R$n takes off $s right hand as $e yells: 'HELL'S FLASH'!\n\r&r$n launches a large red beam towards the structure!" , ch, NULL, NULL, TO_ROOM);
   act( AT_WHITE, "\n\r&wYou grin as you gather energy on your right arm.\n\r&RYou take off your right hand as you yell: 'HELL'S FLASH'!\n\r&rYou launch a large red beam towards the structure!", ch, NULL, NULL, TO_CHAR);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's ki attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour ki attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   if (obj->value[0] <= 0)
   {
     	make_debris(obj);
     	planet_support( ch );
   }
   consume_ki( ch, "hellsflash" );
   waitstate( ch, "hellsflash" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "hellsflash" );
   return;
}

void do_burnattack( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

   if (IS_NPC(ch))
   return;

   argument = one_argument( argument, arg1 );
   lvl = ch->pcdata->skill_level2[gsn_burnattack]/2;
   if ( arg1[0] == '\0' )
   {
    	ch_printf(ch,  "Target burning attack at who?\n\r" );
    	return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    	send_to_char( "Target must be inside this room.\n\r", ch );
    	return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      	send_to_char("You can't do that.\n\r", ch);
      	return;
   }
   if ( gettimer( ch, "burnattack" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }

   warcry(ch, victim);
   act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&zAfter making some strange arm moves $n straightens them out, launching $s attack." , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&wYou raise your arms as you prepare for your attack.\n\r&zAfter making some strange arm moves you straighten them out, launching your blast.", ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&w$n raises $s arms as $e prepares for $s attack\n\r&zAfter making some strange arm moves $n straightens them out, launching $s attack." , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_burnattack )) && (number_percent()*1.8) >= extra)
   {
        dam = number_range( 11+(lvl), 16+(lvl) );
    	dam = add_kidamage( ch, victim, dam );
    	damage(ch, victim, dam, TYPE_NBLAST);
    	learn_from_success2( ch, gsn_burnattack );
   }
   else
   {
    	damage(ch, victim, 0, TYPE_NBLAST);
    	learn_from_failure2( ch, gsn_burnattack );
   }
   consume_ki( ch, "burnattack" );
   waitstate (ch, "burnattack" );
   addtimer( ch, "burnattack", 8 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "burnattack");
   return;
}

void do_ghostattack( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL], buf[MSL];
  sh_int dam, dam1, dam2, dam3, dam4, dam5, dam6, lvl, extra, ghost;
  dam = dam1 = dam2 = dam3 = dam4 = dam5 = dam6 = 0;
  
   if (IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_ghostattack]/2;
   argument = one_argument( argument, arg1 );

   if ( IS_HALFBREED(ch) && !xIS_SET(ch->act, PLR_FUSED) )
   {
    send_to_char( "&RYou need to be fused to perform this attack!\n\r", ch );
    return;
   }

   if ( arg1[0] == '\0' )
   {
    send_to_char( "Target Super Ghost Kamikaze Attack at who?\n\r", ch );
    return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "Target must be inside this room.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch )
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if ( gettimer( ch, "ghostattack" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   ghost = number_range( 3, 6 );
   dam = number_range( 2+(lvl), 5+(lvl) );
   dam1 = add_kidamage( ch, victim, dam );
   dam2 = add_kidamage( ch, victim, dam );
   dam3 = add_kidamage( ch, victim, dam );
   dam4 = add_kidamage( ch, victim, dam );
   dam5 = add_kidamage( ch, victim, dam );
   dam6 = add_kidamage( ch, victim, dam );

   dam1 = (dam1 / 5) + number_range( 3, 8 );
   dam2 = (dam2 / 5) + number_range( 3, 8 );
   dam3 = (dam3 / 5) + number_range( 3, 8 );
   dam4 = (dam4 / 5) + number_range( 3, 8 );
   dam5 = (dam5 / 5) + number_range( 3, 8 );
   dam6 = (dam6 / 5) + number_range( 3, 8 );

   warcry( ch, victim );
   sprintf( buf, "\n\r&w$N's mouth expands as $E spits out %d ghosts made at $S image.\n\r&W$N yells: 'Super Ghost Kamikaze... ATTACK!!!'\n\r&c$N points at you as $E orders the Ghosts to attack!", ghost );
   act( AT_WHITE, buf, victim, NULL, ch, TO_CHAR);
   sprintf( buf, "\n\r&wYour mouth expands as you spit out %d ghosts made at your image.\n\r&WYou yell: 'Super Ghost Kamikaze... ATTACK!!!'\n\r&cYou point at $N as you order the Ghosts to attack $M!", ghost );
   act( AT_WHITE, buf , ch, NULL, victim, TO_CHAR);
   sprintf( buf, "\n\r&w$N's mouth expands as $E spits out %d ghosts made at $S image.\n\r&W$N yells: 'Super Ghost Kamikaze... ATTACK!!!'\n\r&c$N points at $n as $E orders the Ghosts to attack $m!", ghost );
   act( AT_WHITE, buf, victim, NULL, ch, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
/* 1 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra)
   {
    damage(ch, victim, dam1, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );
/* 2 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra)
   {
    damage(ch, victim, dam2, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );
/* 3 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra)
   {
    damage(ch, victim, dam3, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );
/* 4 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra && ghost >= 4)
   {
    damage(ch, victim, dam4, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );
/* 5 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra && ghost >= 5)
   {
    damage(ch, victim, dam5, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );
/* 6 */
   if ( (can_use_skill2(ch, number_percent(), gsn_ghostattack )) && (number_percent()*1.8) >= extra && ghost >= 6)
   {
    damage(ch, victim, dam6, TYPE_GHOST);
    learn_from_success2( ch, gsn_ghostattack );
   }
   else
    learn_from_failure2( ch, gsn_ghostattack );

    consume_ki( ch, "ghostattack" );
    waitstate (ch, "ghostattack" );
    addtimer( ch, "ghostattack", 8 );
    if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
    waitstate( ch->fused_with, "ghostattack");
    return;
}


void do_absorption(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim = NULL;
	char buf[100];
	long pl = 0;
	
	if(IS_NPC(ch))
		return;

	victim = who_fighting(ch);

	if(argument[0] != '\0')
		victim = NULL;

	if(!victim && (victim =get_char_room(ch, argument)) == FALSE)
	{
		send_to_char("They aren't here.\r\n", ch);
		return;
	}

	if(IS_NPC(victim) && victim->hit > (victim->max_hit / 6.5))
	{
		send_to_char("&pYour victim needs to be hurt more first!\r\n", ch);
		return;
	}
	else if(!IS_NPC(victim) && 
		 (victim->position != POS_STUNNED || !can_kill(ch, victim) || !can_attack(ch, victim)))
        {
		send_to_char("&pYour victim needs to be stunned first!\r\n", ch);
		return;
	}



	if(!xIS_SET(ch->act, PLR_FAT))
	{
		act(AT_PINK, "\n\rYou rip a piece of your body off and toss it at $N!", ch, NULL, victim, TO_CHAR);
		act(AT_PINK, "$n rips a piece of $s body off and tosses it at $N!", ch, NULL, victim, TO_NOTVICT);
		act(AT_PINK, "You feel yourself get consumed!", ch, NULL, victim, TO_VICT);
		act(AT_WHITE, "$N is consumed and reforms into $n!", ch, NULL, victim, TO_NOTVICT);
		act(AT_WHITE, "$N is consumed and re-absorbed into you!", ch, NULL, victim, TO_CHAR);
	}
	else
	{
		switch(number_range(0,3))
		{
			case 3:
				act(AT_PINK, "\n\rYou send a bright yellow beam at $N and turn $M into chocolate!",
					ch, NULL, victim, TO_CHAR);
				act(AT_PINK, "$n sends a bright yellow beam at $N and turns $M into chocolate!",
					ch, NULL,victim, TO_NOTVICT);
				act(AT_PINK, "$n sends a bright yellow beam at you and turns you into chocolate!",
					ch, NULL, victim, TO_VICT);
				act(AT_WHITE, "You grab the $N Chocolate Bar and greedily eat it!",
					ch, NULL, victim, TO_CHAR);
				act(AT_WHITE, "$n grabs the $N Chocolate Bar and greedily eats it!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_WHITE, "You get eaten up by $n!", ch, NULL, victim, TO_VICT);
			break;
			case 2:
				act(AT_PINK, "\n\rYou send a bright yellow beam at $N and turn $M into candy!",
					ch, NULL, victim, TO_CHAR);
				act(AT_PINK, "$n sends a bright yellow beam at $N and turns $M into candy!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_PINK, "$n sends a bright yellow beam at you and turns you into candy!",
					ch, NULL,victim, TO_VICT);
				act(AT_WHITE, "You grab the candy and chomp it down!",
					ch, NULL, victim, TO_CHAR);
				act(AT_WHITE, "$n grabs the candy and chomps it down!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_WHITE, "$n eats you completely!", ch, NULL, victim, TO_VICT);
			break;
			case 1:
				act(AT_PINK, "\n\rYou send a bright yellow beam at $N and turn $M into cheese!",
					ch, NULL, victim, TO_CHAR);
				act(AT_PINK, "$n sends a bright yellow beam at $N and turns $M into cheese!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_PINK, "$n sends a bright yellow beam at you and turns you into cheese!",
					ch, NULL, victim, TO_VICT);
				act(AT_WHITE, "You materialize a cracker and eat the cheese on it!",
					ch, NULL, victim, TO_CHAR);
				act(AT_WHITE, "$n materializes a cracker and eats the cheese on it!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_WHITE, "$n materializes a cracker and eats you on it!",
					ch, NULL, victim, TO_VICT);
			break;
			default:
				act(AT_PINK, "\n\rYou send a bright yellow beam at $N and turn $M into a gumball!",
					ch, NULL, victim, TO_CHAR);
				act(AT_PINK, "$n sends a bright yellow beam at $N and turns $M into a gumball!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_PINK, "$n sends a bright yellow beam at you and turns you into a gumball!",
					ch, NULL, victim, TO_VICT);
				act(AT_WHITE, "You chew the gumball and blow a really big bubble with it!",
					ch, NULL, victim, TO_CHAR);
				act(AT_WHITE, "$n chews the gumbal and blows a really big bubble with it!",
					ch, NULL, victim, TO_NOTVICT);
				act(AT_WHITE, "$n chews you up and blows a really big bubble with you!",
					ch, NULL, victim, TO_VICT);
			break;
		}				
	}
	if(get_curr_pl(victim) > victim->exp)
		pl = get_curr_pl(victim);
	else
		pl = victim->exp;

	if(IS_NPC(victim))
		ch->pcdata->kili += number_range(1, pl/10000 * 10);
	else
		ch->pcdata->kili += number_range(1, pl/10000 * 20);
	//raw_kill(ch, victim);
	damage(ch, victim, (victim->max_hit / 6.5) + 20, TYPE_HIT);
	learn_from_success2(ch, gsn_absorption);

	if(!xIS_SET(ch->act, PLR_ULTIMATE) && (xIS_SET(ch->act, PLR_SKINNY) || xIS_SET(ch->act, PLR_FAT))
		&& ch->pcdata->kili >= 100000000)
        {
		act(AT_PINK, "You feel a sudden change in your body!", ch, NULL, NULL, TO_CHAR);
		act(AT_PINK, "$n looks disoriented and has a strange expression!", ch, NULL, NULL, TO_ROOM);
		if(IS_EVIL(ch))
		{
			do_say(ch, "No! Not this!");
			act(AT_RED, "You scream as your body shifts into your pure evil form!", ch, NULL, NULL, TO_CHAR);
			act(AT_RED, "$n's body starts to morph into a smaller form!", ch, NULL, NULL, TO_ROOM);
			act(AT_RED, "You hop around childishly and feel the urge to kill.", ch, NULL, NULL, TO_CHAR);
			act(AT_RED, "$n hops around childishly and has a twisted expression.", ch, NULL, NULL, TO_ROOM);
			ch->height = number_range(24, 44);
			ch->mod = 9.77;
			do_suppress(ch, "0");
    		        xSET_BIT(ch->act, PLR_ULTIMATE);
		}
		else
		{
			sprintf(buf, "%s is mad now!", ch->name);
			do_say(ch, buf);
			act(AT_RED, "Your rage grows beyond anything you can fathom!", ch, NULL, NULL, TO_CHAR);
			do_steam(ch, "");
			xSET_BIT(ch->act, PLR_ULTIMATE);
		}
	}

	if(!xIS_SET(ch->act, PLR_SKINNY) && !xIS_SET(ch->act, PLR_FAT) && ch->pcdata->kili >= 5000000
		&& ch->alignment != 0)
	{
		act(AT_WHITE, "You feel a strange tingle inside your body.", ch, NULL, NULL, TO_CHAR);
		act(AT_PINK, "You feel yourself divide into two!", ch, NULL, NULL, TO_CHAR);
		act(AT_WHITE, "$n stops and gets an intense look in $s eyes!", ch, NULL, NULL, TO_ROOM);
		act(AT_WHITE, "$n splits in two! The two stare each other down!", ch, NULL, NULL, TO_ROOM);
		// It's mighty morphen Buu time!
		if(IS_EVIL(ch))
		{
			ch->mod_dex = 3;
			ch->mod_int = 3;
			ch->mod_str = 3;
			ch->mod_con = -5;
			ch->mod = 8.8;
			xSET_BIT(ch->act, PLR_SKINNY);
			act(AT_PINK, "You stare face to face with your Good self!", ch, NULL, NULL, TO_CHAR);
			act(AT_PINK, "$n stares evilly at $s good self!", ch, NULL, NULL, TO_ROOM);
			do_say(ch, "Die!");
			act(AT_YELLOW, "Your good self fires a giant Ki Blast at you but you easily dodge it!", ch, NULL, NULL, TO_CHAR);
			act(AT_YELLOW, "$n's good self fires a giant Ki Blast at $n, but $e easily dodges it!", ch, NULL, NULL, TO_ROOM);
			act(AT_SAY, "The Good You says, 'Time to turn you into Candy!'", ch, NULL, NULL, TO_CHAR);
			act(AT_SAY, "The Good $n says, 'Time to turn you into Candy!'", ch, NULL, NULL, TO_ROOM);
			act(AT_YELLOW, "Your good self sends a yellow beam at you but you deflect it and it is turned into candy!",
				ch, NULL, NULL, TO_CHAR);
			act(AT_YELLOW, "The Good $n sends a yellow beam at $n but $e deflects it and the Good $n is turned into candy!",
				ch, NULL, NULL, TO_ROOM);
			act(AT_WHITE, "You greedily eat the candy and absorb its power.", ch, NULL, NULL, TO_CHAR);
			act(AT_WHITE, "The Evil $n greedily eats the candy and absorbs the Good $n's power!", ch, NULL, NULL, TO_ROOM);
			act(AT_FIRE, "You feel your power rapidly increase!", ch, NULL, NULL, TO_CHAR);
		}
		else
		{
			act(AT_PINK, "You stare face to face with your Evil Self!", ch, NULL, NULL, TO_CHAR);
			act(AT_PINK, "$n stares with a twisted grin at $s evil self!", ch, NULL, NULL, TO_ROOM);
			act(AT_SAY, "The Evil You says, 'I will destroy you!'", ch, NULL, NULL, TO_CHAR);
			act(AT_SAY, "The Evil $n says, 'I will destroy you!", ch, NULL, NULL, TO_ROOM);
			act(AT_YELLOW, "Your evil self fires a gigantic Ki Blast towards you and it rips through your chest!",
				ch, NULL, NULL, TO_CHAR);
			act(AT_YELLOW, "The evil $n fires a gigantic Ki Blast at good $n which rips through $s chest!",
				ch, NULL, NULL, TO_ROOM);
			act(AT_PINK, "Your body reforms perfectly and you recover from the attack!", ch, NULL, NULL, TO_CHAR);
			act(AT_PINK, "$n's body reforms perfectly and $e recovers from the attack!", ch, NULL, NULL, TO_ROOM);
			do_say(ch, "I will turn you into yummy candy!");
			act(AT_YELLOW, "You launch a yellow beam at Evil $n and turn $e into a candy bar!", ch, NULL, NULL, TO_CHAR);
			act(AT_YELLOW, "$n launches a yellow beam at Evil $n and turns $e into a candy bar!", ch, NULL, NULL, TO_ROOM);
			do_say(ch, "Yummy chocolate!");
			act(AT_YELLOW, "You consume the entire candy bar and feel your powerlevel sky rocket!", ch, NULL, NULL, TO_CHAR);
			act(AT_YELLOW, "$n consumes the entire candy bar and gets a weird look in $s eye!", ch, NULL, NULL, TO_ROOM);
			act(AT_PINK, "Your body grows larger and fatter!", ch, NULL, NULL, TO_CHAR);
			act(AT_PINK, "$n's body grows larger and fatter!", ch, NULL, NULL, TO_ROOM);
			ch->mod_str = 4;
			ch->mod_int = -5;
			ch->mod_con = 5;
			ch->mod = 8.6;
			ch->weight += number_range( 80, 180);
			xSET_BIT(ch->act, PLR_FAT);
			
		}
	}
      return;
}
		
void do_analyze( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA       * victim;
   
   if (!IS_ANDROID(ch))
   {
    send_to_char("Huh?\n\r", ch);
    return;
   }
   if ( argument[0] == '\0' )
   {
     send_to_char( "Analyze Who?\n\r", ch );
     return;
   }
   if ( ( victim = get_char_room( ch, argument ) ) == NULL )
   {
     send_to_char( "They aren't here.\n\r", ch );
     return;
   }     
   if ( victim == ch )
   {
	send_to_char( "Analyze yourself? Use your prompt for that.\n\r", ch );
	return;
   }
       act( AT_WHITE, "$n analyzes you." , ch, NULL, victim, TO_VICT );
       ch_printf(ch,"\n\r&z+-------------------------------------+\n\r" );
       ch_printf(ch,"&z|{r  &RAnalyzing &Y%-17.17s&R          {x&z|\n\r", victim->name);
       ch_printf(ch,"&z|{r                                     {x&z|\n\r");
       ch_printf(ch,"&z|{r                                     {x&z|\n\r");
       ch_printf(ch,"&z|{r           &RRace&z: &W%-12.12s        {x&z|\n\r", race_table[victim->race]->race_name);
       ch_printf(ch,"&z|{r      &RLifeforce&z: &W%-12.12s        {x&z|\n\r", num_punct(victim->hit) );
       ch_printf(ch,"&z|{r         &REnergy&z: &W%-12.12s        {x&z|\n\r", num_punct(victim->mana) );
       ch_printf(ch,"&z|{r     &RPowerlevel&z: &W%-12.12s        {x&z|\n\r", num_punct(get_curr_pl2(victim)) );
       if (xIS_SET(ch->powerup, KI_BAR))
       ch_printf(ch,"&z|{r  &RAttack Energy&z: &W%-12.12s        {x&z|\n\r", num_punct(victim->barrier) );
       else
       ch_printf(ch,"&z|{r  &RAttack Energy&z: &W%-12.12s        {x&z|\n\r", num_punct(victim->charge * 20000000) );
       ch_printf(ch,"&z|{r                                     {x&z|\n\r");
       ch_printf(ch,"&z+-------------------------------------+\n\r");
       act( AT_WHITE, "&R$n analyzes the power of $N.", ch, NULL, victim, TO_NOTVICT );
       return;
}		


void do_fkamebuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_fkame]/2;
   damage = number_range( 60+lvl, 200+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }

   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&WYou spread your arms as you begin to gather energy.\n\r&zLight Blue energyballs appear surrounding each of your hands.\n\r&bYou join both hands infront of you as you yell: 'FINAL-KA-ME-HA-ME-HA', and a beam of blue energy is released towards the structure!!!" , ch, NULL, NULL, TO_CHAR);
   act( AT_WHITE, "\n\r&W$n spreads both of $s arms as $e begins to gather energy.\n\r&zLight Blue energyballs appear surrounding each of $s hands.\n\r&b$N joins both of $s hands infront of $m as $e yells: 'FINAL-KA-ME-HA-ME-HA', and a beam of blue energy is released towards the structure!!!" , ch, NULL, NULL, TO_NOTVICT);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's energy attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour energy impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   consume_ki( ch, "fkame" );
   waitstate( ch, "fkame" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "fkame" );
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support( ch );
   }
   return;
}

void do_bbkbuild( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   sh_int damage, lvl;
   char buf[MSL];

   if ( IS_NPC(ch))
   return;
   lvl = ch->pcdata->skill_level2[gsn_bbk]/2;
   damage = number_range( 60+lvl, 200+lvl );
   damage *= (get_curr_str(ch))/2;
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_BUILDING )
   {
     send_to_char("That isn't a building!\n\r", ch);
     return;
   }
   if (!xIS_SET(ch->powerup, PU_FLYUP))
   {
     send_to_char("You need to fly above the structure!\n\r", ch);
     return;
   }

   add_timer( ch,     TIMER_PROPAGANDA, 40, NULL, 0 );
   act( AT_WHITE, "\n\r&WYou place your right hand infront of you pointing towards the structure with your thumb finger crossed.\n\r&zA blue energy ball begins to form as you yell: 'BIG-BANG-KAMEHAMEHA-ATTACK'.\n\r&cThe energy ball turns into a blue energy beam as you fire it towards the structure at full speed!" , ch, NULL, NULL, TO_CHAR);
   act( AT_WHITE, "\n\r&W$n places $s right hand infront of $m pointing towards the structure with his thumb finger crossed.\n\r&zA blue energy ball begins to form as $n yells: 'BIG-BANG-KAMEHAMEHA-ATTACK'.\n\r&cThe energy ball turns into a blue energy beam as $n fires it towards the structure at full speed!" , ch, NULL, NULL, TO_NOTVICT);
   obj->value[0] -= damage;
   sprintf( buf, "&R$n's energy attack impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM);
   sprintf( buf, "&RYour energy impacts the structure! [%d]", damage );
   act( AT_WHITE, buf, ch, NULL, NULL, TO_CHAR);
   consume_ki( ch, "bbkame" );
   waitstate( ch, "bbkame" );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "bbkame" );
   if (obj->value[0] <= 0)
   {
     make_debris(obj);
     planet_support( ch );
   }
   return;
}

void do_fkame( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_fkame]/2;
  argument = one_argument( argument, arg1 );
   if ( !xIS_SET(ch->act, PLR_FUSED))
   {
    ch_printf( ch, "You can only do this while fused with another person.\n\r");
    return;
   }   
   if ( arg1[0] == '\0' )
   {
    ch_printf( ch, "Target Final Kamehameha Attack at who?\n\r");
    return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      do_fkamebuild(ch, "structure");
      return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "Target must be inside this room.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if ( gettimer( ch, "finalkamehameha" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&W$n spreads both of $s arms as $e begins to gather energy.\n\r&zLight Blue energyballs appear surrounding each of $s hands.\n\r&b$N joins both of $s hands infront of $m as $e yells: 'FINAL-KA-ME-HA-ME-HA', and a beam of blue energy is released towards you!!!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&WYou spread your arms as you begin to gather energy.\n\r&zLight Blue energyballs appear surrounding each of your hands.\n\r&bYou join both hands infront of you as you yell: 'FINAL-KA-ME-HA-ME-HA', and a beam of blue energy is released towards $N!!!" , ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&W$n spreads both of $s arms as $e begins to gather energy.\n\r&zLight Blue energyballs appear surrounding each of $s hands.\n\r&b$N joins both of $s hands infront of $m as $e yells: 'FINAL-KA-ME-HA-ME-HA', and a beam of blue energy is released towards $N!!!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_fkame )) && (number_percent()*1.8) >= extra)
   {
    dam = number_range( 20+(lvl), 30+(lvl) );
    dam = add_kidamage( ch, victim, dam );
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_success2( ch, gsn_fkame );
   }
   else
   {
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_failure2( ch, gsn_fkame );
   }
   consume_ki( ch, "fkame" );
   waitstate( ch, "fkame" );
   addtimer( ch, "fkame", 15 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "fkame" );
}

void do_bbk( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MIL];
  sh_int dam, extra, lvl;
  dam = extra = lvl = 0;

  if (IS_NPC(ch))
  return;
  lvl = ch->pcdata->skill_level2[gsn_bbk]/2;
  argument = one_argument( argument, arg1 );
   if ( !xIS_SET(ch->act, PLR_FUSED))
   {
    ch_printf( ch, "You can only do this while fused with another person.\n\r");
    return;
   }
   if ( !xIS_SET(ch->powerup, PU_SSJ4))
   {
    ch_printf( ch, "You need to be a Super Saiyan 4 to attempt to use this technique.\n\r");
    return;
   }
   
   if ( arg1[0] == '\0' )
   {
    ch_printf( ch, "Target Big Bang Kamehameha Attack at who?\n\r");
    return;
   }
   if ( !str_cmp(arg1, "structure") && ch->pcdata->clan)
   {
      do_bbkbuild(ch, "structure");
      return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
    send_to_char( "Target must be inside this room.\n\r", ch );
    return;
   }
   if ( can_attack( ch, victim ) == FALSE || victim == ch)
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if ( gettimer( ch, "bbkame" ) > 0 )
   {
      send_to_char("You can't do that. Your body needs to recover!\n\r", ch);
      WAIT_STATE( ch, 5 );
      return;
   }
   warcry( ch, victim );
   act( AT_WHITE, "\n\r&W$n places $s right hand infront of $m pointing towards you with his thumb finger crossed.\n\r&zA blue energy ball begins to form as $n yells: 'BIG-BANG-KAMEHAMEHA-ATTACK'.\n\r &cThe energy ball turns into a blue energy beam as $n fires it towards you at full speed!" , ch, NULL, victim, TO_VICT);
   act( AT_WHITE, "\n\r&WYou place your right hand infront of you pointing towards $N with your thumb finger crossed.\n\r&zA blue energy ball begins to form as you yell: 'BIG-BANG-KAMEHAMEHA-ATTACK'.\n\r &cThe energy ball turns into a blue energy beam as you fire it towards $N at full speed!" , ch, NULL, victim, TO_CHAR);
   act( AT_WHITE, "\n\r&W$n places $s right hand infront of $m pointing towards $N with his thumb finger crossed.\n\r&zA blue energy ball begins to form as $n yells: 'BIG-BANG-KAMEHAMEHA-ATTACK'.\n\r &cThe energy ball turns into a blue energy beam as $n fires it towards $N at full speed!" , ch, NULL, victim, TO_NOTVICT);
   extra = (get_curr_evasion( victim ) / number_range(3, 5)) - get_curr_accuracy(ch);
   if ( (can_use_skill2(ch, number_percent(), gsn_bbk )) && (number_percent()*1.8) >= extra)
   {
    dam = number_range( 22+(lvl), 32+(lvl) );
    dam = add_kidamage( ch, victim, dam );
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_success2( ch, gsn_bbk );
   }
   else
   {
    damage(ch, victim, dam, TYPE_FINALFLASH);
    learn_from_failure2( ch, gsn_bbk );
   }
   consume_ki( ch, "bbkame" );
   waitstate( ch, "bbkame" );
   addtimer( ch, "bbkame", 15 );
   if ( ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2) && xIS_SET(ch->fused_with->act, PLR_INVIS2))
   waitstate( ch->fused_with, "bbkame" );
}

void do_skillaccuracy( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_ACCURACY))
     {
      xREMOVE_BIT(ch->skill, SKILL_ACCURACY);
      update_skill( ch );
      send_to_char( "\n\r&WYour concentration dissappears as you completely lose focus.\n\r", ch);
      waitstate( ch, "accuracy" );
      return;
     }
     else if ( can_use_skill(ch, number_percent(), gsn_accuracy) )
     {
      xSET_BIT(ch->skill, SKILL_ACCURACY);
      update_skill( ch );
      learn_from_success( ch, gsn_accuracy );
      send_to_char( "\n\r&wYou close your eyes as you begin to concentrate. As you open them you feel more focused than before.\n\r", ch );
      consume_ki( ch, "accuracy" );
      waitstate( ch, "accuracy" );
      return;
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillzanzoken( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_ZANZOKEN))
     {
      xREMOVE_BIT(ch->skill, SKILL_ZANZOKEN);
      update_skill( ch );
      send_to_char( "\n\r&WYou concentrate as your speed begins to slow down back to normal.\n\r", ch);
      act(AT_GREY,"&W$n's body begins to slow down as $e concentrates.", ch, NULL, NULL, TO_CANSEE);
      waitstate( ch, "zanzoken" );
     }
     else if ( can_use_skill(ch, number_percent(), gsn_zanzoken) )
     {
      learn_from_success( ch, gsn_zanzoken );
      xSET_BIT(ch->skill, SKILL_ZANZOKEN);
      update_skill( ch );
      send_to_char( "\n\r&WYou concentrate your energy as you begin to move your body faster than the human eye.\n\r", ch);
      act(AT_GREY,"&W$n concentrates $s energy as $e begins to move $s body faster than the human eye.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "zanzoken" );
      waitstate( ch, "zanzoken" );
      return;
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillvstance( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_VSTANCE))
     {
      xREMOVE_BIT(ch->skill, SKILL_VSTANCE);
      update_skill( ch );
      send_to_char( "\n\r&WYou switch back to your normal stance.\n\r", ch);
      act(AT_GREY,"&p$n seems to have switched back to $s normal stance.", ch, NULL, NULL, TO_CANSEE);
      waitstate( ch, "vstance" );
     }
     else if ( can_use_skill(ch, number_percent(), gsn_vstance) )
     {
      xSET_BIT(ch->skill, SKILL_VSTANCE);
      update_skill( ch );
      learn_from_success( ch, gsn_vstance );
      send_to_char( "\n\r&pYou adopt a some sort of vicious stance giving you the upper hand in critical hits.\n\r", ch);
      act(AT_GREY,"&p$n seems to have dropped into a some sort of vicious stance.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "vstance" );
      waitstate( ch, "vstance" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillenrage( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_ENRAGE))
     {
      xREMOVE_BIT(ch->skill, SKILL_ENRAGE);
      update_skill( ch );
      send_to_char( "\n\r&WThe dark red aura begins to dissappear as your rage dissipates.\n\r", ch);
      act(AT_GREY,"&W$n's dark red aura begins to dissappear as $s rage dissipates.", ch, NULL, NULL, TO_CANSEE);
      waitstate( ch, "enrage" );
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_enrage) )
     {
      xSET_BIT(ch->skill, SKILL_ENRAGE);
      update_skill( ch );
      learn_from_success( ch, gsn_enrage );
      send_to_char( "\n\r&RYou are surrounded by a strange dark red aura as your energy and strength climbs to its limits.\n\r", ch);
      act(AT_GREY,"&R$n is surrounded by a strange dark aura as $s energy and strength climbs to its limits.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "enrage" );
      waitstate( ch, "enrage" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}


void do_skillberserk( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_BERSERK))
     {
      xREMOVE_BIT(ch->skill, SKILL_BERSERK);
      update_skill( ch );
      send_to_char( "\n\r&WYour body mass dramatically decreases as your rage dissappear.\n\r", ch);
      act(AT_GREY,"&W$n's body mass dramatically decreases as $s rage dissappears.", ch, NULL, NULL, TO_CANSEE);
      waitstate( ch, "berserk" );
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_berserk) )
     {
      xSET_BIT(ch->skill, SKILL_BERSERK);
      update_skill( ch );
      learn_from_success( ch, gsn_berserk );
      send_to_char( "\n\r&BYour body mass begins to grow as your rage builds up your spine.\n\r&WYou have gone berserk!\n\r", ch);
      act(AT_GREY,"&B$n's body mass begins to grow as $s rage builds up $s spine.\n\r&W$n has just gone berserk!", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "berserk" );
      waitstate( ch, "berserk" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skilloverdrive( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;
     if (xIS_SET(ch->skill, SKILL_OVERDRIVE))
     {
      xREMOVE_BIT(ch->skill, SKILL_OVERDRIVE);
      update_skill( ch );
      send_to_char( "\n\r&BThe blue glow that once adorned your skin, suddenly fades, as\n\r&GThe gears begin to turn slower, as you feel your powerlevel drastically decline.\n\r", ch);
      act(AT_GREY,"&BThe blue glow that once adorned $n's skin, suddenly fades from $m, as the\n\r&Ggears inside $m begin to turn slower, and $s powerlevel drastically declines.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_overdrive) )
     {
      xSET_BIT(ch->skill, SKILL_OVERDRIVE);
      update_skill( ch );
      learn_from_success( ch, gsn_overdrive );
      send_to_char( "\n\r&GThe gears inside you begin turning faster, and you feel your overall\n\rpower increasing, as you are enveloped by a bright blue light.\n\r&BThe light quickly soaks into your skin, and bathes you in a soft glow.\n\r", ch);
      act(AT_GREY,"&GThe gears inside $n begin turning faster, and $s overall power increases, as $e is envolped by a bright blue light.\n\r&BThe light quickly soaks into his skin, and bathes him in a soft glow.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "overdrive" );
      waitstate( ch, "overdrive" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillbbarrier( CHAR_DATA *ch, char *argument )
{
     OBJ_DATA *chip;
     if (IS_NPC(ch))
     return;
     if ( ( chip = get_eq_char( ch, WEAR_SLOT2 ) ) == NULL )
     {
	 send_to_char( "You need to be wearing a blue barrier chip to do that!\n\r", ch);
       return;
     }
     if ( chip->pIndexData->vnum != OBJ_VNUM_BBARRIER )
     {
	 send_to_char( "You aren't wearing a blue barrier chip!\n\r", ch);
       return;
     }
     if (xIS_SET(ch->skill, SKILL_BBARRIER))
     {
      xREMOVE_BIT(ch->skill, SKILL_BBARRIER);
      update_skill( ch );
      send_to_char( "\n\r&RYour protective skin dissappears as you stop directing some of your energy\n\r&Wtowards your blue barrier chip.\n\r", ch);
      act(AT_GREY,"&R$n's protective skin dissappears as $e stops directing some of $s energy\n\r&Wtowards $s blue barrier chip.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_bbarrier) )
     {
      xSET_BIT(ch->skill, SKILL_BBARRIER);
      update_skill( ch );
      learn_from_success( ch, gsn_bbarrier );
      send_to_char( "\n\r&BYou are covered by a protective skin as you begin to direct some of your energy\n\r&Wtowards your blue barrier chip in your head.\n\r", ch);
      act(AT_GREY,"&B$n is covered by a protective skin as $e begins to direct some of $s energy\n\r&Wtowards $s blue barrier chip in $s head.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "bbarrier" );
      waitstate( ch, "bbarrier" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillpbarrier( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;

     if (xIS_SET(ch->skill, SKILL_PBARRIER))
     {
      xREMOVE_BIT(ch->skill, SKILL_PBARRIER);
      update_skill( ch );
      send_to_char( "\n\r&GYou lower your arms as the &Ppink energy barrier&G that was once adorning your skin dissappears.\n\r", ch);
      act(AT_GREY,"&G$n lower $s arms as the &Ppink energy barrier&G that was once adorning $s skin dissappears.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_pbarrier) )
     {
      xSET_BIT(ch->skill, SKILL_PBARRIER);
      update_skill( ch );
      learn_from_success( ch, gsn_pbarrier );
      send_to_char( "\n\r&PYou spread your arms as you concentrate some of your energy towards your hands\n\r&PA pink energy barrier begins to form around your entire body.\n\r&pAs you focus your energy more, the barrier turns into a protective skin covering you.\n\r", ch);
      act(AT_GREY,"&P$n spreads $s arms as $e concentrates some of $s energy towards $s hands.\n\r&PA pink energy barrier begins to form around $s entire body.\n\r&pAs $e focuses $s energy more, the barrier turns into a protective skin covering $m.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "pbarrier" );
      waitstate( ch, "pbarrier" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillsoulcry( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;

     if (xIS_SET(ch->skill, SKILL_SOULCRY))
     {
      xREMOVE_BIT(ch->skill, SKILL_SOULCRY);
      update_skill( ch );
      send_to_char( "\n\r&WThe ghostly veil surrounding you fades away as the souls calm their fury.\n\r", ch);
      act(AT_GREY,"&WThe ghostly veil surrounding $n fades away as $e inner souls calm their fury.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_soulcry) )
     {
      xSET_BIT(ch->skill, SKILL_SOULCRY);
      update_skill( ch );
      learn_from_success( ch, gsn_soulcry );
      send_to_char( "\n\r&PYou let loose a high pitched scream, almost tearing a hole in\n\r&pspace and time itself, as all the souls you have absorbed\n\r&zsurround you in a ghostly veil feeding you their rage.", ch);
      act(AT_GREY,"&P$n unleashes a high pitched scream, almost tearing a hole\n\r&pin time itself, as the souls of those $e has absorbed\n\r&zsurround $m in a ghostly veil feeding $m their rage.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "soulcry" );
      waitstate( ch, "soulcry" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillregeneration( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;

     if (xIS_SET(ch->skill, SKILL_REGENERATION))
     {
      xREMOVE_BIT(ch->skill, SKILL_REGENERATION);
      update_skill( ch );
      send_to_char( "\n\r&WYou release your concentration, causing your cell division to slow.\n\r", ch);
      act(AT_GREY,"&W$n releases their concentration as they relax their ki.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_regen) )
     {
      xSET_BIT(ch->skill, SKILL_REGENERATION);
      update_skill( ch );
      learn_from_success( ch, gsn_regen );
      send_to_char( "\n\r&cYou begin focusing your energy towards the location of\n\ryour gravest wounds forcing rapid cell multiplication.", ch);
      act(AT_GREY,"&c$n begins concentrating intently, focusing $s energy towards their wounds.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "regenerate" );
      waitstate( ch, "regenerate" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skillkyodaika( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;

     if (xIS_SET(ch->skill, SKILL_KYODAIKA))
     {
      xREMOVE_BIT(ch->skill, SKILL_KYODAIKA);
      update_skill( ch );
      send_to_char( "\n\r&WYou expel all the stored ki within your body forcing\n\ryou to shrink down to normal size.\n\r", ch);
      act(AT_GREY,"&W$n flares an almighty aura as $e\n\rbegins to shrink down to normal size.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_kyodaika) )
     {
      xSET_BIT(ch->skill, SKILL_KYODAIKA);
      update_skill( ch );
      learn_from_success( ch, gsn_kyodaika );
      send_to_char( "\n\r&gYou begin feeding your ki to your entire body eventually forcing\n\ryour limbs and body to grow to an impressive height and bulk.", ch);
      act(AT_GREY,"&g$n focuses intently as $s body begins to\n\rgrow to an impressive height and bulk.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "kyodaika" );
      waitstate( ch, "kyodaika" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}

void do_skilltimefreeze( CHAR_DATA *ch, char *argument )
{
     if (IS_NPC(ch))
     return;

     if (xIS_SET(ch->skill, SKILL_TIMEFREEZE))
     {
      xREMOVE_BIT(ch->skill, SKILL_TIMEFREEZE);
      update_skill( ch );

      send_to_char( "\n\r&WYou finally give in and release your breath as time speeds\n\rup once again returning you to your normal speed of thought.\n\r", ch);
      act(AT_GREY,"&W$n finally gives in and releases $s breath as time\n\rresumes it's pace gradually returning $s speed to normal.", ch, NULL, NULL, TO_CANSEE);
     }
     else if ( can_use_skill2(ch, number_percent(), gsn_timefreeze) )
     {
      xSET_BIT(ch->skill, SKILL_TIMEFREEZE);
      update_skill( ch );
      learn_from_success( ch, gsn_timefreeze );
      send_to_char( "\n\r&BYou calm yourself before drawing in and holding a deep breath as time\n\r&bstarts to pass by you at a slower rate, dramatically increasing your speed.\n\r", ch);
      act(AT_GREY,"&B$n calms $mself and draws in and holds a deep breath, as time\n\r&bseems to crawl past $m, dramatically increasing their speed.", ch, NULL, NULL, TO_CANSEE);
      consume_ki( ch, "timefreeze" );
      waitstate( ch, "timefreeze" );
     }
     else
     {
      send_to_char( "You failed.\n\r", ch);
      WAIT_STATE(ch, 5);
     }
     return;
}




