/******************************************************************
 *                [D]ragon [B]all [Z] [C]hronicles                *
 *----------------------------------------------------------------*
 * DBZB, DBZDL, DBZC versions done by: Seventeen and Vegeta       *
 *----------------------------------------------------------------*
 * All modifications made by Seventeen and Vegeta are sole        *
 * property of them. Credit must be given if any part is ripped   *
 * or used from this version or any other version of DBZC.        *
 *****************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include "mud.h"

void  update_shuttles	args( ( void ) );
extern int in_start_arena;
extern int time_to_start;
bool can_userace( CHAR_DATA *ch, int gsn );
bool koth_progress;
POLL_DATA *first_vote;
POLL_DATA *last_vote;

void do_mudstatus( CHAR_DATA *ch, char *argument )
{
   send_to_char( "\n\r\n\r&z                   &RMUD Status", ch);
   send_to_char( "\n\r&z+----------------------------+----------------------+", ch);
   ch_printf(ch, "\n\r&z| &cName                       &z| &cStatus&z               |" );
   send_to_char( "\n\r&z+----------------------------+----------------------+", ch);
   ch_printf(ch, "\n\r&z|                            |                      |" );
   ch_printf(ch, "\n\r&z| &YEarth Dragonballs are      &z| %-20s   &z|", dbtimer >= 60 ? "&OSpheres" : "&WStones" );
   ch_printf(ch, "\n\r&z| &YNamek Dragonballs are      &z| %-20s   &z|", dbtimer3 >= 80 ? "&OSpheres" : "&WStones" );
   ch_printf(ch, "\n\r&z| &YUniverse Dragonballs are   &z| %-20s   &z|", dbtimer2 >= 125 ? "&OSpheres" : "&WStones" );
   ch_printf(ch, "\n\r&z| &YStadium is                 &z| %-20s   &z|", in_start_arena == 0 ? "&RClosed" : time_to_start > 0 ? "&YOpen" : "&GClosed [Fighting]" );
   ch_printf(ch, "\n\r&z|                            |                      |" );
   send_to_char( "\n\r&z+----------------------------+----------------------+", ch);
   ch_printf(ch, "\n\r&z| &cStatistics                 &z| &cStatus&z               |" );
   send_to_char( "\n\r&z+----------------------------+----------------------+", ch);
   ch_printf(ch, "\n\r&z|                            |                      |" );

   ch_printf(ch, "\n\r&z| &YSaiyan                     &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_SAIYAN]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YHalfbreed                  &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_HALFBREED]*100)/sysdata.pfile_count  );
   ch_printf(ch, "\n\r&z| &YHuman                      &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_HUMAN]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YAndroid                    &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_ANDROID]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YNamek                      &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_NAMEK]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YIcer                       &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_ICER]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YBiodroid                   &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_BIODROID]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YMutant                     &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_MUTANT]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YBuu                        &z| &W%3d&w%%                 &z|", (sysdata.race_count[RACE_BUU]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z|                            |                      |" );

   ch_printf(ch, "\n\r&z| &YMale                       &z| &W%3d&w%%                 &z|", (sysdata.sex_count[1]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YFemale                     &z| &W%3d&w%%                 &z|", (sysdata.sex_count[2]*100)/sysdata.pfile_count );
   ch_printf(ch, "\n\r&z| &YNeutral                    &z| &W%3d&w%%                 &z|", (sysdata.sex_count[0]*100)/sysdata.pfile_count );


   ch_printf(ch, "\n\r&z|                            |                      |" );
   send_to_char( "\n\r&z+----------------------------+----------------------+\n\r", ch);

}

void update_skill( CHAR_DATA *ch )
{
  sh_int lvl;
  de_equip_char(ch);
  ch->pdef2 = ch->patk2 = ch->edef2 = ch->eatk2 = ch->evasion = ch->critical = ch->accuracy = 0;
  re_equip_char( ch );
  if (xIS_SET(ch->skill, SKILL_ENRAGE) )
  {
     lvl = ch->pcdata->skill_level2[gsn_enrage];
     ch->patk2 += 25 + lvl;
     ch->eatk2 += 25 + lvl;
     ch->pdef2 -= 25 + lvl;
     ch->edef2 -= 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_BERSERK) )
  {
     lvl = ch->pcdata->skill_level2[gsn_berserk];
     ch->patk2 += 25 + lvl;
     ch->eatk2 += 25 + lvl;
     ch->pdef2 -= 25 + lvl;
     ch->edef2 -= 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_OVERDRIVE) )
  {
     lvl = ch->pcdata->skill_level2[gsn_overdrive];
     ch->patk2 += 25 + lvl;
     ch->eatk2 += 25 + lvl;
     ch->pdef2 -= 25 + lvl;
     ch->edef2 -= 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_KYODAIKA) )
  {
     lvl = ch->pcdata->skill_level2[gsn_kyodaika];
     ch->patk2 += 25 + lvl;
     ch->pdef2 -= 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_SOULCRY) )
  {
     lvl = ch->pcdata->skill_level2[gsn_soulcry];
     ch->patk2 += 25 + lvl;
     ch->pdef2 -= 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_TIMEFREEZE) )
  {
     lvl = ch->pcdata->skill_level2[gsn_timefreeze];
     ch->evasion += 5 + lvl;
     ch->accuracy += 15 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_VSTANCE) )
  {
     lvl = ch->pcdata->skill_level[gsn_vstance];
     ch->critical += 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_ACCURACY) )
  {
     lvl = ch->pcdata->skill_level[gsn_accuracy];
     ch->accuracy += 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_ZANZOKEN) )
  {
     lvl = ch->pcdata->skill_level[gsn_zanzoken];
     ch->evasion += 25 + lvl;
  }
  if (xIS_SET(ch->skill, SKILL_PBARRIER) )
  {
     lvl = ch->pcdata->skill_level[gsn_pbarrier];
     ch->pdef2 += 25 + lvl;
     ch->edef2 += 25 + lvl;
  }    
}


int calculate_rep( CHAR_DATA *ch )
{
   int rep=0;


   rep += ch->pcdata->pkills;
   rep -= (ch->pcdata->pdeaths * 2 );
   rep += sqrt(sqrt(ch->pcdata->mkills));
   rep -= sqrt(sqrt(ch->pcdata->mdeaths));
   rep += ch->pcdata->bless * 2;

  return rep;
}



void do_aura ( CHAR_DATA *ch, char *argument )
{
  int i, col=0;
  char arg[MSL];
  if( IS_NPC( ch ) )
    return;

  if( argument[0] == '\0' )
  {
    send_to_char( "Usage:\n\r", ch );
    send_to_char( "  Aura show - shows list of colors and your set color\n\r", ch );
    send_to_char( "  Aura <color> - sets aura to color chosen\n\r", ch );
    send_to_char( "  *note: you can only set your aura color once\n\r", ch );
    return;
  }
  argument = one_argument( argument, arg );
  if( !str_cmp( arg, "show" ) )
  {  
	ch_printf( ch, "\n\r&WYour Aura&z: %s%s", get_aura_color(ch->auracolor), get_aura_name(ch->auracolor));
	ch_printf( ch, "\n\r&WYour Powerup Aura&z: %s%s", get_aura_color(ch->aurapowerup), get_aura_name(ch->aurapowerup));
        ch_printf( ch, "\n\r&WAvailable Colors&z:\n\r------------------------------------------------\n\r");
        for ( i = 0; i < MAX_AURA; i++ )
        {
          col++;
          ch_printf( ch, "  %s%12.12s  ", get_aura_color(i), get_aura_name(i));
          if ( col >= 3 )
          {
              ch_printf(ch, "\n\r");
              col = 0;
           }

        }
        ch_printf( ch, "\n\r&z------------------------------------------------\n\r");
        return;
  }
  if ( ch->auracolor > 0 && !IS_IMMORTAL(ch))
  {
    send_to_char( "You can't change your aura.\n\r", ch);
    return;
  }
  if ( !str_cmp( arg, "pink" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &PPink\n\r", ch);
    ch->auracolor = AURA_PINK;
    return;
  }
  if ( !str_cmp( arg, "green" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &GGreen\n\r", ch);
    ch->auracolor = AURA_GREEN;
    return;
  }

  if ( !str_cmp( arg, "white" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &WWhite\n\r", ch);
    ch->auracolor = AURA_WHITE;
    return;
  }
  if ( !str_cmp( arg, "darkred" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &rDark Red\n\r", ch);
    ch->auracolor = AURA_DRED;
    return;
  }
  if ( !str_cmp( arg, "darkgreen" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &gDark Green\n\r", ch);
    ch->auracolor = AURA_DGREEN;
    return;
  }
  if ( !str_cmp( arg, "orange" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &OOrange\n\r", ch);
    ch->auracolor = AURA_BROWN;
    return;
  }
  if ( !str_cmp( arg, "black" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &zBlack\n\r", ch);
    ch->auracolor = AURA_BLACK;
    return;
  }
  if ( !str_cmp( arg, "red" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &RRed\n\r", ch);
    ch->auracolor = AURA_RED;
    return;
  }
  if ( !str_cmp( arg, "darkblue" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &bDark Blue\n\r", ch);
    ch->auracolor = AURA_DBLUE;
    return;
  }
  if ( !str_cmp( arg, "purple" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &pPurple\n\r", ch);
    ch->auracolor = AURA_PURPLE;
    return;
  }
  if ( !str_cmp( arg, "cyan" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &cCyan\n\r", ch);
    ch->auracolor = AURA_CYAN;
    return;
  }
  if ( !str_cmp( arg, "yellow" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &YYellow\n\r", ch);
    ch->auracolor = AURA_YELLOW;
    return;
  }
  if ( !str_cmp( arg, "blue" ) )
  {
    send_to_char( "&WSetting your aura color to&z: &CBlue\n\r", ch);
    ch->auracolor = AURA_BLUE;
    return;
  }
  send_to_char( "Invalid Aura Color\n\r", ch );
  return;

}

void do_checkradar( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *radar;
  OBJ_DATA *obj;
  bool found = FALSE;
  int db;
  char buf[MSL];
  if ( ( radar = get_eq_char( ch, WEAR_HOLD)) == NULL )
  {
    send_to_char("You must be holding the dragonradar.\n\r", ch);
    return;
  }
  if ( radar->item_type != ITEM_RADAR )
  {
    send_to_char("That isn't a dragonradar\n\r", ch);
    return;
  }

  if (!xIS_SET(ch->act, PLR_ONMAP))
  {
    send_to_char("You can only use this outside.\n\r", ch);
    return;
  }

  send_to_char("\n\rYou click on the radar button as you watch:\n\r\n\r",ch);

  for ( db = 253; db <= 276; db++ )
  {
   if ( (obj = get_db( db )) == NULL )
   continue;
   if ( !IS_OBJ_STAT( obj, ITEM_ONMAP ) )
   continue;
   if ( obj->item_type != ITEM_DRAGONBALL && obj->item_type != ITEM_DRAGONBALL2)
   continue;
   if ( obj->carried_by != NULL )
   continue;
   if ( getsamemap(ch->map) != getsamemap(obj->map))
   continue;
   if ( obj->x == -1 || obj->y == -1 || obj->map == -1)
   continue;

/*   if ( obj->x == ch->x && obj->y == ch->y && obj->map == ch->map )
   continue;*/
  
   found = TRUE;

                 if ( obj->y < ch->y && obj->x == ch->x)
                   sprintf( buf, "north");
                 else if ( obj->y > ch->y && obj->x == ch->x)
                   sprintf( buf, "south");
                 else if ( obj->x > ch->x && obj->y == ch->y)
                   sprintf( buf, "east" );
                 else if ( obj->x < ch->x && obj->y == ch->y)
                   sprintf( buf, "west" );
                 else if ( obj->y < ch->y && obj->x > ch->x )
                   sprintf( buf, "northeast" );
                 else if ( obj->y < ch->y && obj->x < ch->x )
                   sprintf( buf, "northwest" );
                 else if ( obj->y > ch->y && obj->x > ch->x )
                   sprintf( buf, "southeast" );
                 else if ( obj->y > ch->y && obj->x < ch->x )
                   sprintf( buf, "southwest" );

                 if ( obj->x == ch->x && obj->y == ch->y )
                  ch_printf( ch, "&O%s &W- &RLocated here.\n\r", obj->short_descr );
                 else
                  ch_printf( ch, "&O%s &W- &RLocated %s of this point.\n\r", obj->short_descr, buf);


  }

  if (!found)
  send_to_char("None.\n\r", ch);

}

void do_timers( CHAR_DATA *ch, char *argument )
{
   send_to_char("\n\r&RCurrent MUD Timers&r:\n\r\n\r", ch);
   ch_printf(ch,"&YDragonball Timer &O:  &C%d\n\r", dbtimer);
   ch_printf(ch,"&YDragonball Timer2&O:  &C%d\n\r", dbtimer2);
   ch_printf(ch,"&YDragonball Timer3&O:  &C%d\n\r", dbtimer3);
   ch_printf(ch,"&YIncome Timer     &O:  &C%d\n\r", income);
   ch_printf(ch,"&YMoonphase        &O:  &C%d\n\r", moonphase);
   ch_printf(ch,"&YKi Timer         &O:  &C%d\n\r", kitimer);

 return;
}
void do_clear( CHAR_DATA *ch, char *argument )
{
  if (!IS_NPC(ch))
  ch->pcdata->rank = "";
}
void do_monkey( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;
     if (!CAN_OOZARU(ch))
     return;
     if ( xIS_SET(ch->act, PLR_FUSED) )
     return;
     if ( !xIS_SET(ch->act, PLR_TAIL))
     return;
     if (!IS_AWAKE(ch))
     return;
     if ( xIS_SET(ch->powerup, PU_SSJ4) )
     return;
     xSET_BIT(ch->powerup, PU_OOZARU);
     send_to_char( "\n\r&YYou watch the moon as your heart begins to pump faster...\n\rYou feel the extreme rage inside trying to get out.\n\rA few seconds later you turn into a HUGE Ape... An OOZARU!!!\n\r", ch );
     act(AT_YELLOW,"\n\r$n watches the moon as $s heart begins to pump faster...\n\r$n feels the extreme rage inside of $m trying to get out.\n\rA few seconds later $e turns into a HUGE Ape, An OOZARU!!!\n\r", ch, NULL, NULL, TO_CANSEE);

     if (xIS_SET(ch->powerup, PU_SUPPRESS))
     do_suppress( ch, "0");
     ch->mod = 9.5;
     ch->height_mod = ch->height * 5;
     ch->weight_mod = ch->weight * 5;
     ch->mod_str = 5;
     ch->mod_dex = -3;
     ch->mod_int = -3;
     ch->mod_lck = 5;
     ch->mod_con = 0;
     ch->mod_wis = 0;
     ch->mod_cha = 0;
     ch->kimod = 1;
     ch->hit = ch->max_hit;
     ch->mana = ch->max_mana;
     xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
     xREMOVE_BIT(ch->powerup, PU_SSJ1);
     xREMOVE_BIT(ch->powerup, PU_SSJ2);
     xREMOVE_BIT(ch->powerup, PU_SSJ3);
}

void do_looksky( CHAR_DATA *ch, char *argument )
{

       send_to_char( "\n\rAs you look up into the sky, you see:\n\r", ch);
       send_to_char( "&z------------------------------------------------------------------\n\r", ch);
       if ( time_info.hour >= 6 && time_info.hour < 20 )
       {
         send_to_char( "&Y        _..._\n\r",ch);
         send_to_char( "&Y      .:::::::.\n\r",ch);
         send_to_char( "&Y     :::::::::::\n\r",ch);
         send_to_char( "&Y     :::::::::::\n\r",ch);
         send_to_char( "&Y     `:::::::::'\n\r",ch);
         send_to_char( "&Y       `-:::-'\n\r",ch);
       }
       else
       {
         switch(moonphase)
         {
           case 1:
           {
            send_to_char( "&w        _..._\n\r",ch);
            send_to_char( "&w      .'     `.\n\r",ch);
            send_to_char( "&w     :         :\n\r",ch);
            send_to_char( "&w     :         :\n\r",ch);
            send_to_char( "&w     `.       .'\n\r",ch);
            send_to_char( "&w       `-...-'\n\r",ch);
            break;             
           }
           case 2:
           {
            send_to_char( "&w        _..._\n\r",ch);
            send_to_char( "&w      .'   `::.\n\r",ch);
            send_to_char( "&w     :       :::\n\r",ch);
            send_to_char( "&w     :       :::\n\r",ch);
            send_to_char( "&w     `.     .::'\n\r",ch);
            send_to_char( "&w       `-..:-'\n\r",ch);
            break;
           }
           case 3:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .'  ::::.\n\r", ch);
            send_to_char( "&w     :    ::::::\n\r", ch);
            send_to_char( "&w     :    ::::::\n\r", ch);
            send_to_char( "&w     `.   :::::'\n\r", ch);
            send_to_char( "&w       `-.::-'\n\r", ch);
            break;
           }
           case 4:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .' '::::.\n\r", ch);
            send_to_char( "&w     :  ::::::::\n\r", ch);
            send_to_char( "&w     :  ::::::::\n\r", ch);
            send_to_char( "&w     `. `::::::'\n\r", ch);
            send_to_char( "&w       `-:::-'\n\r", ch);
            break;
           }
           case 5:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .:::::::.\n\r", ch);
            send_to_char( "&w     :::::::::::\n\r", ch);
            send_to_char( "&w     :::::::::::\n\r", ch);
            send_to_char( "&w     `:::::::::'\n\r", ch);
            send_to_char( "&w       `-:::-'\n\r", ch);
            if (!xIS_SET(ch->powerup, PU_OOZARU))
            do_monkey(ch, "");
            break;
           }
           case 6:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .:::::``.\n\r", ch);
            send_to_char( "&w     ::::::::  :\n\r", ch);
            send_to_char( "&w     ::::::::  :\n\r", ch);
            send_to_char( "&w     `::::::  .'\n\r", ch);
            send_to_char( "&w       `-::.-'\n\r", ch);
            break;
           }
           case 7:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .::::  `.\n\r", ch);
            send_to_char( "&w     ::::::    :\n\r", ch);
            send_to_char( "&w     ::::::    :\n\r", ch);
            send_to_char( "&w     `:::::   .'\n\r", ch);
            send_to_char( "&w       `-::.-'\n\r", ch);
            break;
           }
           case 0:
           {
            send_to_char( "&w        _..._\n\r", ch);
            send_to_char( "&w      .::'   `.\n\r", ch);
            send_to_char( "&w     :::       :\n\r", ch);
            send_to_char( "&w     :::       :\n\r", ch);
            send_to_char( "&w     `:::     .'\n\r", ch);
            send_to_char( "&w       `::..-'\n\r", ch);
            break;
           }
           default:
           break;
         }
    }          
       send_to_char( "\n\r&z------------------------------------------------------------------\n\r", ch);
    if (IS_IMMORTAL(ch))
    ch_printf(ch, "%d moonphase", moonphase);
}

void reset_sector( void );
void do_changes (CHAR_DATA *ch, char *argument );
void do_dragonball( CHAR_DATA *ch, char *argument )
{
   int value;
   char arg[MSL];
   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char("\n\r&wSyntax: Handle <field> <value>\n\r", ch);
      send_to_char("  Field being one of:\n\r\n\r", ch);
      send_to_char("  db, db2, db3, kitimer, mobileupdate, search_boost, reset_sector,\n\r", ch);
      send_to_char("  add_income, scatter, start_ctf, end_ctf, start_koth, end_koth, find_fighters, moonphase,\n\r", ch);
      send_to_char("  space_update, moon_update, char_update, db_update, time_update,\n\r", ch);
      send_to_char("  shuttle_update, webclan, webwho, webwizlist, webchanges\n\r", ch);

      return;
   }
   if (!str_cmp(arg, "db"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     dbtimer = value;
     ch_printf( ch, "\n\r&wSetting Timer for Earth Dragonballs to: %d\n\r", dbtimer);
     return;
   }
   if (!str_cmp(arg, "kitimer"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     kitimer = value;
     ch_printf( ch, "\n\r&wSetting Timer for Ki to: %d\n\r", kitimer);
     return;
   }

   if (!str_cmp(arg, "moonphase"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     moonphase = value;
     ch_printf( ch, "\n\r&wSetting Timer for Moonphase to: %d\n\r", moonphase);
     return;
   }
   if (!str_cmp( arg,"webwizlist" ) )
   {
     do_makewizlist(ch, "");
     return;
   }
   if (!str_cmp( arg,"webchanges" ) )
   {
     do_changes(NULL, "www");
     send_to_char("Updating Webchanges.\n\r", ch);
     return;
   }

   if (!str_cmp( arg,"webwho" ) )
   {
     do_who(NULL, "www");
     send_to_char("Updating webwho.\n\r", ch);
     return;
   }
   if (!str_cmp( arg,"webclans" ) )
   {
     do_clans(NULL, "www");
     send_to_char("Updating webclans.\n\r", ch);
     return;
   }

   if (!str_cmp(arg, "find_fighters"))
   {
     value = find_fighters();
     ch_printf( ch, "\n\rCurrently Maxed Fighters in MUD:  %d&w\n\r", value);
     return;
   }
   if (!str_cmp(arg, "space_update"))
   {
     space_update();
     ch_printf( ch, "\n\rUpdating Space...&w\n\r");
     return;
   }
   if (!str_cmp(arg, "shuttle_update"))
   {
     update_shuttles();
     ch_printf( ch, "\n\rUpdating Shuttles...&w\n\r");
     return;
   }

   if (!str_cmp(arg, "moon_update"))
   {
     moon_update();
     ch_printf( ch, "\n\rUpdating Moon Phase...&w\n\r");
     return;
   }
   if (!str_cmp(arg, "char_update"))
   {
     char_update();
     ch_printf( ch, "\n\rUpdating In Mud Characters...&w\n\r");
     return;
   }
   if (!str_cmp(arg, "time_update"))
   {
     time_update();
     ch_printf( ch, "\n\rUpdating Time...&w\n\r");
     return;
   }

   if (!str_cmp(arg, "start_ctf"))
   {
     ctf = TRUE;
     redscore = bluescore = 0;
     command( ch, do_echo, "\n\rCTF has started.\n\r&BBlue Score&O: &C%d\n\r&RRed Score &O: &C%d", bluescore, redscore);
     return;
   }
   if ( !str_cmp( arg, "start_koth" ) )
   {
     koth_progress = TRUE;
     do_echo( ch, "\n\r&r|&RGAME&r|&z: ::&CKOTH&c has started, type &Ckoth&c if you want to join.&z::" );
   }
   if ( !str_cmp( arg, "end_koth" ) )
   {
     koth_progress = FALSE;
     do_echo( ch, "\n\r&r|&RGAME&r|&z: ::&CKOTH&c entrance has closed, no one else will be allowed to join.&z::");
   }

   if (!str_cmp(arg, "end_ctf"))
   {
     ctf = FALSE;
     command( ch, do_echo, "\n\rCTF has end.\n\r&BFinal Blue Score&O: &C%d\n\r&RFinal Red Score &O: &C%d", bluescore, redscore);
     redscore = bluescore = 0;
     return;
   }
   if (!str_cmp(arg, "db2"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     dbtimer2 = value;
     ch_printf( ch, "\n\r&wSetting Timer for Black Dragonballs to: %d\n\r", dbtimer2);
     return;
   }
   if (!str_cmp(arg, "db3"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     dbtimer3 = value;
     ch_printf( ch, "\n\r&wSetting Timer for Namek Dragonballs to: %d\n\r", dbtimer3);
     return;
   }

   if (!str_cmp( arg, "mobileupdate"))
   {
     ch_printf( ch, "\n\r&wActivating Mobile Update\n\r");
     mobile_update();
     return;
   }
   if (!str_cmp( arg, "search_boost"))
   {
     search_boost();
     send_to_char("\n\r&wSearching for boost\n\r", ch);
     return;

   }
   if (!str_cmp(arg, "reset_sector"))
   {
     send_to_char("\n\r&wResetting Sectors...\n\r", ch);
     reset_sector();
     return;

   }
   if (!str_cmp(arg, "income"))
   {
     value = atoi( argument );
     if ( value < 0 || value > 100000 )
     {
       send_to_char( "Invalid Value.\n\r", ch);
       return;
     }
     income = value;
     ch_printf( ch, "\n\r&wSetting Income Timer to: %d\n\r", income);
     return;
   }
   if (!str_cmp(arg, "scatter"))
   {
     db_update();
     ch_printf( ch, "\n\r&wRunning DB Scatter Proccess...\n\r");
     return;
   }
}

char *itoa(int foo)
{
  static char bar[256];

  sprintf(bar,"%d",foo);
  return(bar);
}
void add_letter( char *string, char letter)
{
  char buf[MIL];

  sprintf(buf, "%c", letter);
  strcat(string, buf);
  return;
}
void do_rps( CHAR_DATA *ch, char *argument)
{
  sh_int rps;
  rps = number_range( 0, 5);

  if (rps == 0 || rps == 2)
  {
   send_to_char("\n\r&PYou throw rock.\n\r", ch);
   act( AT_WHITE, "&P$n throws rock.\n\r", ch, NULL, NULL, TO_ROOM);
   return;
  }
  if (rps == 3 || rps == 5)
  {
   send_to_char("\n\r&PYou throw scissors.\n\r", ch);
   act( AT_WHITE, "&P$n throws scissors.\n\r", ch, NULL, NULL, TO_ROOM);
   return;
  }
  if (rps == 1 || rps == 4)
  {
   send_to_char("\n\r&PYou throw paper.\n\r", ch);
   act( AT_WHITE, "&P$n throws paper.\n\r", ch, NULL, NULL, TO_ROOM);
   return;
  }
 return;
}

void do_cointoss( CHAR_DATA *ch, char *argument)
{
  sh_int toss;

  toss = number_range( 0, 1);

  if ( toss == 0 )
  {
   send_to_char("\n\r&YYou toss a &Wcoin&Y high into the air, and you catch it flat in your hand as it falls.\n\r", ch);
   send_to_char("&YThe coin lands on &BHEADS&Y!\n\r", ch);
   act( AT_WHITE, "&Y$n tosses a &Wcoin&Y high into the air, and $e catches it flat in $s hand as it falls.\n\r", ch, NULL, NULL, TO_ROOM);
   act( AT_WHITE, "&YThe coin lands on &BHEADS&Y!.\n\r", ch, NULL, NULL, TO_ROOM);
   return;
  }
  if ( toss == 1 )
  {
   send_to_char("\n\r&YYou toss a &Wcoin&Y high into the air, and you catch it flat in your hand as it falls.\n\r", ch);
   send_to_char("&YThe coin lands on &RTAILS&Y!\n\r", ch);
   act( AT_WHITE, "&Y$n tosses a &Wcoin&Y high into the air, and $e catches it flat in $s hand as it falls.\n\r", ch,NULL, NULL, TO_ROOM);
   act( AT_WHITE, "&YThe coin lands on &RTAILS&Y!.\n\r", ch, NULL, NULL, TO_ROOM);
   return;
  }
}

void do_plist( CHAR_DATA *ch, char *argument )
{


      ch_printf(ch,"\n\r&z+-----------------------------------------------------------------+");
      ch_printf(ch,"\n\r&z|&Y                          POWERUPS LIST                          &z|");
      ch_printf(ch,"\n\r&z+-----------------------------------------------------------------+");
      ch_printf(ch,"\n\r&z|  &RSkill                 Learned       Requirement       Mod      &z|");
      ch_printf(ch,"\n\r&z+-----------------------------------------------------------------+");
      ch_printf(ch,"\n\r&z|                                                                 |");
   if (IS_BIODROID(ch))
   {
      ch_printf(ch,"\n\r&z|&c  perfect form                        &W5001 &zBM           &Y10&Ox      &z|");
      ch_printf(ch,"\n\r&z|&c  semi-perfect form                   &W1001 &zBM           &Y9&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  adult form                          &W101  &zBM           &Y4&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  larvae form                         &W0    &zBM           &Y1&Ox       &z|");
  }
  else if (IS_HALFBREED(ch))
  {
      ch_printf(ch,"\n\r&z|&c  oozaru                              &wMoon              &Y9.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  mystic ssj                          &G70,000,000 &gPL     &Y9.7&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  ussj                                &G65,000,000 &gPL     &Y8.6&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  mystic                &w%3d&W%%          &G20,000,000 &gPL     &Y8.9&Ox     &z|", ch->pcdata->learned2[gsn_mystic]);
      ch_printf(ch,"\n\r&z|&c  ssj2                                &G10,000,000 &gPL     &Y8.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  ssj1                                &G5,000,000 &gPL      &Y4&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  kaioken               &w%3d&W%%          &G4,000,000 &gPL      &Y#x       &z|", ch->pcdata->learned2[gsn_kaioken]);
  }
  else if (IS_HUMAN(ch))
  {
      ch_printf(ch,"\n\r&z|&c  superhuman                          &G70,000,000 &gPL     &Y9.6&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  turbo                               &G2,500,000 &gPL      &Y8.5&Ox     &z|");
  }
  else if (IS_NAMEK(ch))
  {
      ch_printf(ch,"\n\r&z|&c  ultra namek                         &G70,000,000 &gPL     &Y9.55&Ox    &z|");
      ch_printf(ch,"\n\r&z|&c  super namek                         &G4,500,000 &gPL      &Y8.9&Ox     &z|");
  }
  else if (IS_ICER(ch))
  {
      ch_printf(ch,"\n\r&z|&c  transform metal                     &G70,000,000 &gPL     &Y9.4&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  transform 5                         &G30,000,000 &gPL     &Y9&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  transform 4                         &G15,000,000 &gPL     &Y6&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  transform 3                         &G4,000,000 &gPL      &Y4&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  transform 2                         &G0 &gPL              &Y2&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  transform 1                         &G0 &gPL              &Y1&Ox       &z|");
  }
  else if (IS_MUTANT(ch))
  {
      ch_printf(ch,"\n\r&z|&c  mutate monster                      &G70,000,000 &gPL     &Y9.4&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  mutate 4                            &G20,000,000 &gPL     &Y8.6&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  mutate 3                            &G4,000,000 &gPL      &Y6&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  mutate 2                            &G1,500,000 &gPL      &Y2&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  mutate 1                            &G0 &gPL              &Y1&Ox       &z|");
  }
  else if (IS_BUU(ch))
  {
      ch_printf(ch,"\n\r&z| &GGood Buu&z                                                        |");
      ch_printf(ch,"\n\r&z|&c  fat form                            &P100&p%%&W Kili        &Y8.6&Ox      &z|" );
      ch_printf(ch,"\n\r&z|&c  ultimate form                       &P100&p%%&W Kili        &Y9.4&Ox      &z|" );
      ch_printf(ch,"\n\r&z|                                                                 |");
      ch_printf(ch,"\n\r&z| &REvil Buu&z                                                        |");
      ch_printf(ch,"\n\r&z|&c  skinny form                         &P100&p%%&W Kili        &Y8.8&Ox      &z|" );
      ch_printf(ch,"\n\r&z|&c  ultimate form                       &P100&p%%&W Kili        &Y9.7&Ox      &z|" );
  }
  else if (IS_ANDROID(ch))
  {
      ch_printf(ch,"\n\r&z|&c  M5 chip                             &G50,000,000 &gPL     &Y9.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  M4 chip                             &G20,000,000 &gPL     &Y8.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  M3 chip                             &G15,000,000 &gPL     &Y6&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  M2 chip                             &G5,000,000 &gPL      &Y4&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  M1 chip                             &G1,000,000 &gPL      &Y2&Ox       &z|");
  }
  else if (IS_SAIYAN(ch))
  {
      ch_printf(ch,"\n\r&z|&c  oozaru                              &wMoon              &Y9.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  ssj4                                &G95,000,000 &gPL     &Y10&Ox      &z|");
      ch_printf(ch,"\n\r&z|&c  ssj3                                &G25,000,000 &gPL     &Y9&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  ussj                                &G65,000,000 &gPL     &Y8.6&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  ssj2                                &G10,000,000 &gPL     &Y8.5&Ox     &z|");
      ch_printf(ch,"\n\r&z|&c  ssj1                                &G5,000,000 &gPL      &Y4&Ox       &z|");
      ch_printf(ch,"\n\r&z|&c  kaioken               &W%3d&z%%          &G4,000,000 &gPL      &Y#x       &z|", ch->pcdata->learned2[gsn_kaioken]);
  }                                                  

      ch_printf(ch,"\n\r&z|                                                                 |");
      ch_printf(ch,"\n\r&z+-----------------------------------------------------------------+");
  return;

}

void do_skills( CHAR_DATA *ch, char *argument)
{
    do_alist(ch, "");
    do_newslist(ch, "");
}
/*
void do_skills( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:nuke  <character name> \n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not logged on.\n\r", ch );
	return;
    }

   if (IS_NPC(victim))
    {
     send_to_char( "This command only works on players.\n\r", ch);
      return;
    }

    sprintf( strsave, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), capitalize( victim->name ) );
    stop_fighting(victim,TRUE);
    sprintf( buf,"%s has been nuked by %s",victim->name,ch->name);
    do_quit(victim,"");
    unlink(strsave);
    return;
}
*/
void do_newslist( CHAR_DATA *ch, char *argument)
{
   sh_int sn;
   if ( IS_NPC(ch))
   {
      send_to_char( "Huh?\n\r", ch);
      return;
   }
     ch_printf(ch, "\n\r&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|                           &YSKILL LIST                          &z|\n\r");
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|  &RSkill                 Learned       Lvl       Powerlevel     &z|\n\r" );
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|                                                               |\n\r");
     for ( sn = 0; sn < top_sskill; sn++ )
     {
         if ( sskill_table[sn]->value < 0 )
            ch_printf(ch, "&z|&c  %-20.20s  &W%3d&z%%                    &W%-12.12s   &z|\n\r",sskill_table[sn]->name, ch->pcdata->learned[sn], sskill_table[sn]->pl ? num_punct(sskill_table[sn]->pl) : "0" );
        else
            ch_printf(ch, "&z|&c  %-20.20s  &W%3d&z%%         &W%3d        &W%-12.12s   &z|\n\r",sskill_table[sn]->name, ch->pcdata->learned[sn], ch->pcdata->skill_level[sn], sskill_table[sn]->pl ? num_punct(sskill_table[sn]->pl) : "0" );

     }
     ch_printf(ch, "&z|                                                               |\n\r");
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
      ch_printf(ch, "\n\r&zSee also: score, alist, plist, practice and status\n\r");
}

void do_alist( CHAR_DATA *ch, char *argument)
{
   sh_int sn;
   if ( IS_NPC(ch))
   {
      send_to_char( "Huh?\n\r", ch);
      return;
   }
     ch_printf(ch, "\n\r&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|                          &YABILITY LIST&z                         |\n\r");
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|  &RSkill                 Learned       Lvl       Powerlevel     &z|\n\r" );
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
     ch_printf(ch, "&z|                                                               |\n\r");
     for ( sn = 0; sn < top_askill; sn++ )
     {
       if ( can_userace( ch, sn ) == FALSE )
       continue;
         if ( askill_table[sn]->value < 0 )
            ch_printf(ch, "&z|&c  %-20.20s  &W%3d&z%%                    &W%-12.12s   &z|\n\r",askill_table[sn]->name, ch->pcdata->learned2[sn], askill_table[sn]->pl ? num_punct(askill_table[sn]->pl) : "0" );
        else
            ch_printf(ch, "&z|&c  %-20.20s  &W%3d&z%%         &W%3d        &W%-12.12s   &z|\n\r",askill_table[sn]->name, ch->pcdata->learned2[sn], ch->pcdata->skill_level2[sn], askill_table[sn]->pl ? num_punct(askill_table[sn]->pl) : "0" );

     }
     ch_printf(ch, "&z|                                                               |\n\r");
     ch_printf(ch, "&z+---------------------------------------------------------------+\n\r");
      ch_printf(ch, "\n\r&zSee also: score, alist, plist, practice and status\n\r");
}

void do_newpractice( CHAR_DATA *ch, char *argument )
{
   int sn, col, col2;
   col = col2 = 0;
   if ( IS_NPC(ch))
   {
      send_to_char( "Huh?\n\r", ch);
      return;
   }
   if ( argument[0] == '\0' )
   {
     ch_printf( ch,"\n\r&b========================================&z[&G &YSkills &z]&b=======================================&w\n\r\n\r");
     for ( sn = 0; sn < top_sskill; sn++ )
     {
      if ( sskill_table[sn]->name )
      {
       col++;
       ch_printf(ch, " &c%20.20s &z[&Y%3d&O%%&z]", sskill_table[sn]->name, ch->pcdata->learned[sn] );
      }
      if ( col >= 3 )
      {
       ch_printf(ch, "\n\r");
       col = 0;
       }
     }
     ch_printf( ch,"\n\r\n\r&b======================================&z[&G &YAbilities&z ]&b======================================&w\n\r\n\r");
     for ( sn = 0; sn < top_askill; sn++ )
     {
      if ( col2 >= 3 )
      {
       ch_printf(ch, "\n\r");
       col2 = 0;
      }

      if ( askill_table[sn]->name )
      {
       if ( can_userace( ch, sn ) == FALSE )
       continue;
       col2++;
       ch_printf(ch, " &c%20.20s &z[&Y%3d&O%%&z]", askill_table[sn]->name, ch->pcdata->learned2[sn] );
      }
     }
     ch_printf( ch,"\n\r\n\r&b======================================================================================\n\r");
     ch_printf( ch,"&zYou have &W%d &zattribute training sessions left.\n\r", ch->stat);
     ch_printf( ch,"&zYou have &W%d&z skill leveling training sessions left.\n\r", ch->skill_lvl);
   }
   else
   {
     do_learn(ch, argument);
     return;
   }

}
void do_learn( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *mob=NULL;
  char buf[MSL];
  int sn, value, gsn;
  bool can=FALSE;
  struct skill_type *skill;
  bool ability=FALSE; 
  if ( IS_NPC(ch))
  {
      send_to_char( "Huh?\n\r", ch);
      return;
  }


  if ( ( sn = sskill_lookup( ch, argument )) >= 0 )
  {
     ability = FALSE;
     skill = sskill_table[sn];
  }
  else if ( ( sn = askill_lookup( ch, argument ) ) >= 0 )
  {
     ability = TRUE;
     skill = askill_table[sn];
  }
  else
  {
     send_to_char( "Unknown Skill.\n\r", ch );
     return;
  }
  for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
     if ( IS_NPC(mob) && ( xIS_SET(mob->act, ACT_PRACTICE) || xIS_SET(mob->act, ACT_ABILITY)))
        break;

  if ( !mob )
  {
     send_to_char( "You can't do that here.\n\r", ch );
     return;
  }
  if ( ability )
  {
       if ( can_userace( ch, sn ) == FALSE )
         can = FALSE;
       else
       	can=TRUE;
       if ( !can )
       {
         send_to_char("You cannot learn this skill.\n\r", ch);
         return;
       }
  }
  /*
   * Skill requires a special teacher
  */
  if ( ability )
  gsn = ch->pcdata->learned2[sn];
  else
  gsn = ch->pcdata->learned[sn];

  if ( (!skill->teachers || skill->teachers[0] == '\0') && xIS_SET(mob->act, ACT_ABILITY ) )
  {
        act( AT_TELL, "&W$n says 'I don't know how to do that.'",
            mob, NULL, ch, TO_VICT );
        return;
  }

  if ( skill->teachers && skill->teachers[0] != '\0' )
  {
     sprintf( buf, "%d", mob->pIndexData->vnum );
     if ( !is_name( buf, skill->teachers ) )
     {
	act( AT_TELL, "&W$n says 'I don't know how to do that.'",
	    mob, NULL, ch, TO_VICT );
	return;
     }
  }
  if ( skill->pl && ch->exp < skill->pl )
  {
    act( AT_TELL, "&W$n says 'I don't think you're ready to learn that yet.'",
	mob, NULL, ch, TO_VICT );
//    send_to_char( "You are not strong enough.\n\r", ch);
    return;
  } 
  if ( gsn > 0 )
  {
    act( AT_TELL, "&W$n says 'You have to practice this on your own now.'",
        mob, NULL, ch, TO_VICT );

//    send_to_char( "You have to train this on your own.\n\r", ch);
    return;
  }
  if ( skill->range > 0 )
  {
    send_to_char( "You can't learn this here.\n\r", ch);
    return;
  }

  value = 5;
  value += get_curr_int(ch)*2; 
  value = URANGE( 0, value, 40 );
  if ( ability )
  ch->pcdata->learned2[sn] = value;
  else
  ch->pcdata->learned[sn] = value;
  if ( sn == gsn_be )
  ch->pcdata->learned[sn] = 1;
  ch_printf(ch,"You practice %s.\n\r", skill->name);
  act( AT_WHITE, "$n practices $T.", ch, NULL, skill->name, TO_ROOM );

}


void do_status( CHAR_DATA *ch, char *argument )
{
  ch_printf( ch, "\n\r&z____________________________________________\n\r");
  ch_printf( ch, "\n\r&RGains Stat&z:\n\r\n\r");
  ch_printf( ch, "&YMax Skill Level Gain is&O:&R %d\n\r", calculatelevel(ch));
  ch_printf( ch, "&YYour current Skill Level Gain is&O:&R %d\n\r", ch->skillgain );
  ch_printf( ch, "&YYour current Ki Gain is&O:&R %d\n\r", ch->skigain );
  ch_printf( ch, "&YYour current Stat Gain is&O: &R%d\n\r&z____________________________________________", ch->statgain);
  ch_printf( ch, "\n\r\n\r&GGravity Stat&z:\n\r\n\r");
  ch_printf( ch, "&YGravity is set to&O:&G %d\n\r", ch->gravset);
  ch_printf( ch, "&YYou have had &G%d&Y good training sequences.\n\r", ch->gravsucc);
  ch_printf( ch, "&YYou have totally trained &G%d&Y in this session.\n\r", ch->gravtotal);    
  ch_printf( ch, "\n\r&z____________________________________________\n\r");
  if (IS_IMMORTAL(ch))
  {
  ch_printf( ch, "\n\r&RImmortal Info&z:\n\r");
  ch_printf( ch, "&r_____________________________\n\r");
  ch_printf( ch, "&YBiomass&O :  &C%d\n\r", ch->biomass);
  ch_printf( ch, "&YRage&O    :  &C%d\n\r", ch->rage   );
  ch_printf( ch, "&YBarrier&O :  &C%lld\n\r", ch->barrier);
  ch_printf( ch, "&YSuppress&O:  &C%lld\n\r", ch->suppress);
  ch_printf( ch, "&YCharge  &O:  &C%lld\n\r", ch->charge);
  ch_printf( ch, "&YSkillevel&O: &C%d\n\r", calculatelevel(ch));
  ch_printf( ch, "\n\r&RTest:\n\r");
  ch_printf( ch, "Cloak Timer: %d\n\r", get_timer(ch, TIMER_CLOAK));

  ch_printf( ch, "&YRank    &O:  %s\n\r", get_transformation(ch));
  ch_printf( ch, "&YFusionname&O: %s\n\r", ch->fusionname);
  ch_printf( ch, "&YAtkDef&O: %d %d %d %d\n\r", ch->patk, ch->eatk, ch->pdef, ch->edef );
  }
}

void do_semimortal( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch))
    return;

    if ( ch->level < 49)
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( !xIS_SET(ch->act, PLR_SEMI))
    {
     act( AT_YELLOW, "$n is now set as Semi-Mortal.", ch, NULL, NULL, TO_ROOM );
     send_to_char( "&YYou are now set as Semi-Mortal\n\r", ch );
     xSET_BIT(ch->act, PLR_SEMI);
     return;
    }
    else
    {
     act( AT_YELLOW, "$n is now set as Immortal.", ch, NULL, NULL, TO_ROOM );
     send_to_char( "&YYou are now set as Immortal.\n\r", ch );
     xREMOVE_BIT(ch->act, PLR_SEMI);
     return;
    }
}

void  do_growtail( CHAR_DATA *ch, char *argument )
{
 if (CAN_TAIL(ch))
 {
  if (!xIS_SET(ch->act, PLR_TAIL))
  {
   xSET_BIT(ch->act, PLR_TAIL);
   send_to_char( "You feel that your tail has grown back.\n\r", ch);
  }
 }
}

void do_cuttail( CHAR_DATA *ch, char *argument)
{
  
   if (!xIS_SET(ch->act, PLR_TAIL))
   {
    send_to_char("Cut which tail?\n\r", ch );
    return;
   }
   if (IS_ICER(ch))
   {
    send_to_char("Icers don't need to cut off their tails.", ch );
    return;
   }
   if (xIS_SET(ch->act, PLR_SPEC) )
       {
         send_to_char("&RSpectators cant do that.\n\r", ch);
         return;
       }
   if (ch->position == POS_RESTING || ch->position == POS_SITTING )
   {
    send_to_char("Stand up first.\n\r", ch);
    return;
   }
   if (!IS_AWAKE(ch))
   {
    send_to_char("Cut your tail while sleeping? Nice.\n\r", ch);
    return;
   }

   send_to_char("\n\rYou look at your tail, as you grab it and you pull it off.\n\r", ch);
   act(AT_RED,"\n\r$n looks down at there tail as they grab it and tear it off.", ch, NULL, NULL, TO_CANSEE);
   xREMOVE_BIT(ch->act, PLR_TAIL);
   if (xIS_SET(ch->powerup, PU_OOZARU))
   {
    send_to_char("As you cut your tail, you lose your power of Oozaru!", ch);
    act(AT_RED,"As $n finishes ripping there tail off they lose there Oozaru transformation\n\r", ch, NULL, NULL, TO_CANSEE);
    ch->mod = 1;
    ch->height_mod = 0;
    ch->weight_mod = 0;
    ch->mod_str = 0;
    ch->mod_dex = 0;
    ch->mod_lck = 0;
    ch->mod_int = 0;
    xREMOVE_BIT(ch->powerup, PU_OOZARU);
    xREMOVE_BIT(ch->powerup, PU_GOLDEN);
    return;
   }
}

void do_gravset( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];  
   bool check=FALSE;
   int setgrav;

   argument = one_argument( argument, arg1 );

   setgrav = atoi(arg1);

   if (!IS_AWAKE(ch))
   {
    send_to_char("You aren't awake...\n\r", ch);
    return;
   }
   if (ch->position == POS_RESTING || ch->position == POS_SITTING )
   {
    send_to_char("Stand up first.\n\r", ch);
    return;
   }

   if ( arg1[0] == '\0' )
   {
    send_to_char( "Set the Gravity to which level?\n\r", ch );
    return;
   }
   if (!IS_SET(ch->in_room->room_flags2, ROOM_GRAVITY) && !IS_SET(ch->in_room->room_flags2, ROOM_UGRAVITY))
   {
    send_to_char( "You aren't in a gravity room.\n\r", ch );
    return;
   }
   if ( !is_number(arg1))
   {
    send_to_char( "It must be a number.\n\r", ch);
    return;
   }
   if ( setgrav > 3500 )
   {
    send_to_char("The highest gravity at the moment is 3500.", ch);
    check = TRUE;
    return;
   }
   if (setgrav != ch->gravset) 
   {
    ch->gravsucc = 0;
    ch->gravset = setgrav;
   }
    ch->gravset = setgrav;
    ch_printf(ch, "Gravity Set to %d.\n\r", ch->gravset);
    return;
}

void do_ojunk( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj, *obj_next;
   bool found = FALSE;

   if( !argument || argument[0] == '\0' )
   {
    send_to_char( "Junk what?\n\r", ch );
    return;
   }
   if (ch->position == POS_RESTING || ch->position == POS_SITTING )
   {
    send_to_char("Stand up first.\n\r", ch);
    return;
   }
   if (!IS_AWAKE(ch))
   {
    send_to_char("You have to be awake to do that.\n\r", ch);
    return;
   }

   for( obj = ch->first_carrying; obj; obj = obj_next )
   {
    obj_next = obj->next_content;
    if ( ( nifty_is_name( argument, obj->name ) ) && can_see_obj( ch, obj ) && obj->wear_loc == WEAR_NONE )
    {
       found = TRUE;
       break;
    }
   }
   if( found )
   {
    if( IS_OBJ_STAT( obj, ITEM_NOJUNK ))
    {
     send_to_char( "You can't junk that.\n\r", ch );
     return;
    }
    if( !can_drop_obj( ch, obj ) && ch->level < LEVEL_IMMORTAL )
    { 
     send_to_char( "You cannot junk that.\n\r", ch );
     return;
    }
    separate_obj( obj );
    obj_from_char( obj );
    extract_obj( obj );
  /* act( AT_ACTION, "", ch, obj, NULL, TO_ROOM );
    act( AT_ACTION, "", ch, obj, NULL, TO_CHAR );*/
   }
   return;
}

/*
 * Zone Echo v2.0 -Nopey
 * noplex@crimsonblade.org
 */
void do_zecho(CHAR_DATA *ch, char *argument)
{
 if(!IS_IMMORTAL(ch) || IS_NPC(ch))
 {
  send_to_char("Huh?\n\r", ch);
  return;
 }

 if(argument[0] == '\0')
 {
  send_to_char("Zecho what?\n\r", ch);
  return;
 }

 /* let's try this once more... */
 {
  AREA_DATA *pArea = ch->in_room->area;
  DESCRIPTOR_DATA *d = NULL;

  for(d = first_descriptor; d; d = d->next)
   if(d->character && d->character->in_room->area == pArea)
    pager_printf_color(d->character, "%s&g\n\r", argument);
 }
 return;
}


void do_spar( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
   if (ch->position == POS_RESTING || ch->position == POS_SITTING )
   {
    send_to_char("Stand up first.\n\r", ch);
    return;
   }
   if (!IS_AWAKE(ch))
   {
    send_to_char("Wake up first.\n\r", ch);
    return;
   }

    if (!IS_SET(ch->in_room->room_flags2, ROOM_SPAR))
    {
	send_to_char( "You aren't in a sparring chamber.\n\r", ch );
	return;
    }    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Spar with whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "Spar with yourself? Interesting concept.\n\r", ch );
	return;
    }
    if (ch->position == POS_SITTING || ch->position == POS_RESTING)
    {
        send_to_char( "You stand up before sparring.\n\r", ch );
        return;
    }
    if (victim->position == POS_SITTING || victim->position == POS_RESTING)
    {
        send_to_char( "That person aint standing up.\n\r", ch );
        return;
    }
    if ( ch->position == POS_FIGHTING && xIS_SET(ch->act, PLR_SPAR))
    {
	send_to_char( "You are already sparring!\n\r", ch );
	return;
    }
    if (xIS_SET(victim->act, PLR_SPAR))
    {
      ch_printf(ch, "%s is already sparring someone.\n\r", victim->name );
      return;
    }
    if ( !victim->desc )
    {
      ch_printf(ch, "%s is link-dead.\n\r", victim->name );
      return;
    }
    if (victim->hit <= 0)
    {
	send_to_char( "They aren't ready for that.\n\r", ch );
	return;
    }
    if (!IS_NPC (victim))
    {
     sprintf( log_buf, "%s: sparring %s.\n\r", ch->name, victim->name );
     log_string_plus( log_buf, LOG_NORMAL, ch->level );
    }
    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    sprintf( buf, "You bow before %s as you begin sparring.\n\r", victim->name );
    send_to_char( buf, ch);
    sprintf( buf, "%s bows before you, as you prepare for sparring.\n\r", victim->name );
    send_to_char( buf, victim);
    xSET_BIT(ch->act, PLR_SPAR);
    xSET_BIT(victim->act, PLR_SPAR);
    damage( ch, victim, number_range( 0, 4 ), TYPE_HIT );
    damage( ch, victim, number_range( 0, 4 ), TYPE_HIT );
    damage( victim, ch, number_range( 0, 4 ), TYPE_HIT );
    damage( victim, ch, number_range( 0, 4 ), TYPE_HIT );

    return;
}

void do_togglechat( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];
   CHAR_DATA         *victim;
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    send_to_char( "Togglechat who?\n\r", ch );
    return;
   }
   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
    send_to_char( "They aren't here.\n\r", ch );
    return;
   }
   if (xIS_SET(victim->act, PLR_CHAT))
   {
    xREMOVE_BIT(victim->act, PLR_CHAT);
    ch_printf(ch, "%s can't use Roleplaying channels now.\n\r", victim->name);
    ch_printf(victim, "You can't use Roleplaying channels now.\n\r");
   }
   else if (!xIS_SET(victim->act, PLR_CHAT))
   {
    xSET_BIT(victim->act, PLR_CHAT);
    ch_printf(ch, "%s can use Roleplaying channels now.\n\r", victim->name);
    ch_printf(victim, "You can use Roleplaying channels now.\n\r");
   }
}

void do_outlaw( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];
   char 	     buf[MAX_INPUT_LENGTH];
   CHAR_DATA         *victim;
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    send_to_char( "Outlaw who?\n\r", ch );
    return;
   }
   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
    send_to_char( "They aren't here.\n\r", ch );
    return;
   }
   if (xIS_SET(victim->act, PLR_OUTLAW))
   {
    xREMOVE_BIT(victim->act, PLR_OUTLAW);
    sprintf(buf,"&R%s is no longer Outlaw!", victim->name);
    ch_printf(victim, "You are no longer an Outlaw!\n\r");
    do_echo(ch, buf);
   }
   else if (!xIS_SET(victim->act, PLR_OUTLAW))
   {
    xSET_BIT(victim->act, PLR_OUTLAW);
    sprintf(buf, "%s now an Outlaw!", victim->name);
    ch_printf(victim, "You are now an Outlaw!\n\r");
    do_echo(ch, buf);
   }
}

void do_bless( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];
   CHAR_DATA         *victim;

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    send_to_char( "Bless who?\n\r", ch );
    return;
   }
   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
    send_to_char( "They aren't here.\n\r", ch );
    return;
   }
   if ( IS_IMMORTAL(victim))
   {
    send_to_char( "Whats the point?\n\r", ch );
    return;
   }

    ch_printf(ch, "You have blessed %s!\n\r", victim->name);
    ch_printf(victim, "&WYou glow up brightly and a wave of power engulfs your surroundings\n\r&BYou have been blessed by the Gods!\n\r");
    act( AT_WHITE, "&W$N glows up brightly and a wave of power engulfs $s surroundings\n\r&B$N has been blessed by the Gods!", ch, NULL, victim, TO_NOTVICT );

    victim->pcdata->bless += 1;
}

void do_unbless( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];
   CHAR_DATA         *victim;
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
    send_to_char( "UnBless who?\n\r", ch );
    return;
   }
   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
    send_to_char( "They aren't here.\n\r", ch );
    return;
   }
   if ( IS_IMMORTAL(victim))
   {
    send_to_char( "Whats the point?\n\r", ch );
    return;
   }

    ch_printf(ch, "You have unblessed %s!\n\r", victim->name);
    ch_printf(victim, "&zYou are engulfed by a black and red aura as you feel different.\n\r&RYou have been cursed by the Gods!");
    act( AT_WHITE, "&W$N is engulfed by a black and red aura.\n\r&B$N has been cursed by the Gods!", ch, NULL, victim, TO_NOTVICT );
    victim->pcdata->bless -= 1;
}

void do_connect( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA * first_ob;
  OBJ_DATA * second_ob;
  OBJ_DATA * new_ob;

  char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Syntax: Connect <firstobj> <secondobj>.\n\r", ch );
    return;
  }

  if ( ( first_ob =  get_obj_carry( ch, arg1 ) )  == NULL )
  {
    sprintf( buf, "You must be holding both parts!!\n\r" );
    send_to_char( buf, ch );
    return;
  }

  if ( ( second_ob =  get_obj_carry( ch, arg2 ) )  == NULL )
  {
    sprintf( buf, "You must be holding both parts!!\n\r" );
    send_to_char( buf, ch );
    return;
  }

  if ( first_ob->item_type != ITEM_PIECE || second_ob->item_type !=ITEM_PIECE )
  {
    send_to_char( "Both items must be pieces of another item!\n\r", ch );
    return;
  }

  /* check to see if the pieces connect */

  if (  ( first_ob->value[0] == second_ob->pIndexData->vnum )
  && ( second_ob->value[0] == first_ob->pIndexData->vnum )
  && ( first_ob->value[1] == second_ob->value[1]  )
  && (first_ob->pIndexData->vnum != second_ob->pIndexData->vnum) )
 /* good connection  */
  {
    new_ob = create_object( get_obj_index( first_ob->value[2] ), ch->level );
    extract_obj( first_ob );
    extract_obj( second_ob );
    obj_to_char( new_ob , ch );
    act( AT_ACTION, "$n jiggles some pieces together...\r\n ...suddenly they snap in place, creating $p!", ch, new_ob,NULL,  TO_ROOM );
    act( AT_ACTION, "You jiggle the pieces together...\r\n ...suddenly they snap into place, creating $p!", ch, new_ob, NULL, TO_CHAR);

  }
  else
  {
    act( AT_ACTION, "$n jiggles some pieces together, but can't seem to make them connect.", ch, NULL, NULL, TO_ROOM);
    act( AT_ACTION, "You try to fit them together every which way, but they just don't want to fit together.", ch, NULL, NULL, TO_CHAR);

    return;
  }

  return;
}

void do_deadly( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;

     if (xIS_SET(ch->pcdata->flags, PCFLAG_DEADLY))
     {
        send_to_char( "You are already a Pkill Player.\n\r", ch );
     }
     else
     {
        xSET_BIT(ch->pcdata->flags, PCFLAG_DEADLY);
        send_to_char( "You are now a PKILL Player.\n\r", ch );
        act(AT_GREY,"$n is now a PKILL Player.", ch, NULL, NULL, TO_CANSEE);
        return;
     }

}

/* This is only required if you do not already have it, you need it for both loop and cycle */
char *number_sign( char *txt, int num )
{
  static char newstring[MAX_INPUT_LENGTH];
  int i;

  if ( !txt)
    return "";

  newstring[0] = '\0';
  for ( i = 0; txt[i] != '\0'; i++ )
  {
     if (txt[i] == '#')
       strcat(newstring, itoa(num));
     else
      add_letter(newstring, txt[i]);
  }
  return newstring;
}


void do_cycle( CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    int start, stop, x;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax: loop <start> <end> <command>   \n\r", ch );
	return;
    }
    argument = one_argument( argument, arg );

    if ( (start = atoi(arg)) < 0)
    {
      if (!is_number(arg))
      return;

	send_to_char( "You must start on a zero or higher.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ((stop= atoi(arg)) <= start)
    {
      if (!is_number(arg))
      return;
	ch_printf( ch, "You must stop at a point higher than you've started. %d is %s than %d.\n\r", stop, stop == start ? "equal to" : "less than", start );
	return;
    }

    for ( x = start; x <= stop; x++ )
     interpret( ch, number_sign(argument, x) );
}

void do_notepeek( CHAR_DATA *ch, char *argument)
{ 
  CHAR_DATA *victim;

  if ((victim=get_char_world( ch, argument)) == NULL)
  {
        ch_printf( ch, "No such player as %s.\n\r", argument);
        return;
  }
  if (IS_NPC(victim) || victim->editor == NULL)
  {
        ch_printf( ch, "They are not writing anything.\n\r");
        return;
  }
  ch_printf( ch, "%s's buffer says:\n\r%s\n\r", victim->name, copy_buffer(victim));
  return;
} 

void do_height( CHAR_DATA *ch, char *argument)
{
    int value;
    
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Height <How fuck tall are you?>\n\r", ch );
        return;
    }
    if ( ch->height > 0 )
    {
        send_to_char( "You already have a height.\n\r", ch );
        return;
    }
    value = atoi(argument);
     if ( value < 30 || value > 70 )
     {
         send_to_char( "Height value is from 30 to 70", ch );
         return;
     }
      ch->height = value;
      return;
}
void do_weight( CHAR_DATA *ch, char *argument)
{
    int value;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Weight <How much you weight?>\n\r", ch );
        return;
    }
    if ( ch->weight > 0 )
    {
        send_to_char( "You already have a weight.\n\r", ch );
        return;
    }
    value = atoi(argument);
     if ( value < 70 || value > 300 )
     {
         send_to_char( "Height value is from 70 to 300", ch );
         return;
     }
      ch->weight = value;
      return;
}

void do_showmap( CHAR_DATA *ch, char *argument )
{
    char player[MAX_STRING_LENGTH];
    char buf[MSL], buf1[MSL], buf2[MSL], buf3[MSL], buf4[MSL], buf5[MSL];
    char line0[MSL], line1[MSL], line2[MSL], line3[MSL], line4[MSL], line5[MSL], line6[MSL], line7[MSL];
    char line8[MSL], line9[MSL];
    EXIT_DATA *pexit;
    bool found, nfound, efound, sfound, wfound, ufound, dfound, nefound, nwfound, sefound, swfound, fAuto;
    int count;
    fAuto  = !str_cmp( argument, "auto" );

    found = nfound = efound = sfound = wfound = ufound = dfound = nefound = nwfound = sefound = swfound = FALSE;
    count = 0;
    buf5[0] =  line0[0] = line1[0] = line2[0] = line3[0] = line4[0]
        = line5[0] = line6[0] = line7[0] = line8[0] = line9[0] = '\0';


    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
    {
        if ( pexit->to_room
        &&  (!IS_SET(pexit->exit_info, EX_HIDDEN) && !IS_SET(pexit->exit_info, EX_SECRET) && !IS_SET(pexit->exit_info,EX_CLOSED)  ))
        {
            found = TRUE;
                count++;
                buf5[0] = '\0';

            if ( !fAuto )
            {
                if ( IS_SET(pexit->exit_info, EX_CLOSED) )
                {
                    sprintf( buf5 + strlen(buf5), "&G&W%-9s &c- (closed)",
                    capitalize( dir_name[pexit->vdir] ) );
                }
                else if ( IS_SET(pexit->exit_info, EX_WINDOW) )
                {
                    sprintf( buf5 + strlen(buf5), "&G&W%-9s &c- (window)",
                    capitalize( dir_name[pexit->vdir] ) );
                }
                else if ( IS_SET(pexit->exit_info, EX_xAUTO) )
                {
                   sprintf( buf5 + strlen(buf5), "&G&W%-9s &c- &G&W%s",
                    capitalize( pexit->keyword ),
                    room_is_dark( pexit->to_room )
                        ?  "Too dark to tell"
                        : pexit->to_room->name );
                }
                else
                    sprintf( buf5 + strlen(buf5), "&G&W%-9s &c- &G&W%s",
                    capitalize( dir_name[pexit->vdir] ),
                    room_is_dark( pexit->to_room )
                        ?  "Too dark to tell"
                        : pexit->to_room->name );
            }
            else
            {
                sprintf( buf2 + strlen(buf2), "&c&W %s",
                    capitalize( dir_name[pexit->vdir] ) );
            }
                if(pexit->vdir == 0)    nfound=TRUE;
                if(pexit->vdir == 1)    efound=TRUE;
                if(pexit->vdir == 2)    sfound=TRUE;
                if(pexit->vdir == 3)    wfound=TRUE;
                if(pexit->vdir == 4)    ufound=TRUE;
                if(pexit->vdir == 5)    dfound=TRUE;
                if(pexit->vdir == 6)    nefound=TRUE;
                if(pexit->vdir == 7)    nwfound=TRUE;
                if(pexit->vdir == 8)    sefound=TRUE;
                if(pexit->vdir == 9)    swfound=TRUE;
                if(count==1) strcpy(line0, buf5);
                if(count==2) strcpy(line1, buf5);
                if(count==3) strcpy(line2, buf5);
                if(count==4) strcpy(line3, buf5);
                if(count==5) strcpy(line4, buf5);
                if(count==6) strcpy(line5, buf5);
                if(count==7) strcpy(line6, buf5);
                if(count==8) strcpy(line7, buf5);
                if(count==9) strcpy(line8, buf5);
                if(count==10) strcpy(line9, buf5);
        }
    }

    if ( dfound && ufound )
    sprintf( player, "&CB");
    else if ( dfound )
    sprintf( player, "&BD");
    else if ( ufound )
    sprintf( player, "&GU");
    else
    sprintf( player, "&RX");
    sprintf(buf,"&W+&z-----&W+  %s\n\r", line0[0] == '\0' ? "" : line0 );
    sprintf(buf1,"&z|%s%s%s&z|  %s\n\r", nwfound == TRUE ? " &Y\\" : "  ", nfound == TRUE ? "&Y|" : " ", nefound == TRUE ?  "&Y/ " : "  ", line1[0] == '\0' ? "" : line1 );
    sprintf(buf2,"&z|%s%s%s&z|  %s\n\r", wfound == TRUE ? "&Y--" : "  ", player, efound == TRUE ? "&Y--" : "  ", line2[0] == '\0' ? "" : line2 );
    sprintf(buf3,"&z|%s%s%s&z|  %s\n\r",  swfound == TRUE ? " &Y/" : "  ", sfound == TRUE ? "&Y|" : " ", sefound == TRUE ? "&Y\\ " : "  ", line3[0] == '\0' ? "" : line3 );
    sprintf(buf4,"&W+&z-----&W+  %s&w\n\r",line4[0] == '\0' ? "" : line4 );
    send_to_char( buf, ch);
    send_to_char( buf1, ch);
    send_to_char( buf2, ch);
    send_to_char( buf3, ch);
    send_to_char( buf4, ch);
    if(line5[0] != '\0') ch_printf(ch,"         %s&w\n\r",line5[0] == '\0' ? "" : line5);
    if(line6[0] != '\0') ch_printf(ch,"         %s&w\n\r",line6[0] == '\0' ? "" : line6);
    if(line7[0] != '\0') ch_printf(ch,"         %s&w\n\r",line7[0] == '\0' ? "" : line7);
    if(line8[0] != '\0') ch_printf(ch,"         %s&w\n\r",line8[0] == '\0' ? "" : line8);
    if(line9[0] != '\0') ch_printf(ch,"         %s&w\n\r",line9[0] == '\0' ? "" : line9);


    return;
}


void do_stopspec ( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *loc;
     if (!xIS_SET(ch->act, PLR_SPEC) )
     {
        send_to_char( "\n\r&zYou are not spectating.\n\r", ch );
     }
     else
     {
        xREMOVE_BIT(ch->act, PLR_SPEC);
        send_to_char( "&RYou have stoped spectating.\n\r", ch );
        act(AT_RED,"$n has stoped spectating.", ch, NULL, NULL, TO_CANSEE);
        char_from_room( ch );
        if ( ( loc = ch->pcdata->roomarena )  != NULL )
        char_to_room( ch, loc );
        else
        char_to_room( ch, get_room_index( 251 ) );
	if (xIS_SET(ch->act, PLR_WASONMAP))
	{
		xREMOVE_BIT(ch->act, PLR_WASONMAP);
		xSET_BIT(ch->act, PLR_ONMAP);	
 		ch->x = ch->wasx;
		ch->y = ch->wasy;
		ch->map = ch->wasmap;
		ch->wasx = ch->wasy = ch->wasmap = -1;
	}
        do_look(ch, "auto");
        return;
     }
}

void do_igive( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf  [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );
    
    if ( !IS_NPC(ch) )
    {        
          send_to_char( "Huh?\n\r", ch );
          return; 
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
	 return;

    if ( ms_find_obj(ch) )
	return;

    if ( is_number( arg1 ) )
    {
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) ) )
	    return;

	argument = one_argument( argument, arg2 );
	if ( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
	    argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	    return;
	

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	    return;
	
	if ( ch->gold < amount )
             return;
	

	ch->gold     -= amount;
	victim->gold += amount;
        strcpy(buf, "$n gives you ");
        strcat(buf, arg1 );
        strcat(buf, (amount > 1) ? " coins." : " coin.");

	mprog_bribe_trigger( victim, ch, amount );
	if ( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch) )
	    save_char_obj(ch);
	if ( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim) )
	    save_char_obj(victim);
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	return;

    if ( obj->wear_loc != WEAR_NONE )
	return;

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	return;
    
    if ( !can_drop_obj( ch, obj ) )
	return;

    if ( victim->carry_number + (get_obj_number(obj)/obj->count) > can_carry_n( victim ) )
	return;

    if ( victim->carry_weight + (get_obj_weight(obj)/obj->count) > can_carry_w( victim ) )
	return;

    if ( !can_see_obj( victim, obj ) )
	return;

    if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto( victim ) )
	return;
    
    separate_obj(obj);
    obj_from_char(obj);
    obj = obj_to_char(obj, victim);
    mprog_give_trigger(victim, ch, obj);
    if ( IS_SET(sysdata.save_flags, SV_GIVE) && !char_died(ch) )
	save_char_obj(ch);
    if ( IS_SET(sysdata.save_flags, SV_RECEIVE) && !char_died(victim) )
	save_char_obj(victim);
    return;
}

void do_calculator( CHAR_DATA *ch, char *argument)
{
  char arg[MIL];
  char arg1[MIL];
  int value, result;
  
  argument = one_argument( argument, arg);
  argument = one_argument( argument, arg1);

  if (IS_NPC(ch))
  return;
  value = atoi(arg1);
  if ( value > 100000 )
  {
   send_to_char( "Unreal Number.\n\r", ch);
   return;
  }
  ch_printf( ch, "SquareRoot for %d, is %f\n\r", atoi(arg1), sqrt(atoi(arg1)));
  if ( !str_cmp( arg, "train" ))
  {
    if ( value == 0)
    value = ch->statgain;

    result = value * value * 12300;
    ch_printf(ch, "\n\rResult to the Calculation of Train Number %s is: %d\n\r", num_punct(value), result);
    return;
  }
  else if ( !str_cmp( arg, "energy" ))
  {
    if ( value == 0)
    value = ch->skigain;

    result = value * value * 1000;
    ch_printf(ch, "\n\rResult to the Calculation of Energy Number %s is: %d\n\r", num_punct(value), result);
    return;
  }
  else
  {
    send_to_char("Syntax: Calculator [Train|Practice|Energy] <Number Expected>\n\r", ch);
    return;
  }

 return;
}

void do_summon( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj, *obj_next;
  MOB_INDEX_DATA *pMobIndex;
  CHAR_DATA *victim;
  bool ball1, ball2, ball3, ball4, ball5, ball6, ball7;
  int vnum=0;
  ball1 = ball2 = ball3 = ball4 = ball5 = ball6 = ball7 = FALSE;
/*  if ( xIS_SET(ch->act, PLR_ONMAP) )
  {
     send_to_char( "Sorry, you can't do this outside.\n\r", ch );
     return;
  }  */
  if (!str_cmp( argument, "shenlong" ) )
  {
    vnum = DRAGON_EARTH;
    if ( !xIS_SET(ch->act, PLR_ONMAP) )
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->vnum == 260)
        ball1 = TRUE;
        if (obj->pIndexData->vnum == 261)
        ball2 = TRUE;
        if (obj->pIndexData->vnum == 262)
        ball3 = TRUE;     
        if (obj->pIndexData->vnum == 263)
        ball4 = TRUE;
        if (obj->pIndexData->vnum == 264)
        ball5 = TRUE;
        if (obj->pIndexData->vnum == 265)
        ball6 = TRUE;
        if (obj->pIndexData->vnum == 266)
        ball7 = TRUE;
      }
    }
    else
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if ( IS_OBJ_FLAG(obj, ITEM_ONMAP) && (obj->x == ch->x) && (obj->y == ch->y) && (obj->map == ch->map) )
        {
           if (obj->pIndexData->vnum == 260)
           ball1 = TRUE;
           if (obj->pIndexData->vnum == 261)
           ball2 = TRUE;
           if (obj->pIndexData->vnum == 262)
           ball3 = TRUE;
           if (obj->pIndexData->vnum == 263)
           ball4 = TRUE;
           if (obj->pIndexData->vnum == 264)
           ball5 = TRUE;
           if (obj->pIndexData->vnum == 265)
           ball6 = TRUE;
           if (obj->pIndexData->vnum == 266)
           ball7 = TRUE;
        }
        continue;
      }
    }         
  }
  else if (!str_cmp( argument, "porunga" ) )
  {
    vnum = DRAGON_NAMEK;
    if ( !xIS_SET(ch->act, PLR_ONMAP) )
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->vnum == 253)
        ball1 = TRUE;
        if (obj->pIndexData->vnum == 254)
        ball2 = TRUE;
        if (obj->pIndexData->vnum == 255)
        ball3 = TRUE;
        if (obj->pIndexData->vnum == 256)
        ball4 = TRUE;
        if (obj->pIndexData->vnum == 257)
        ball5 = TRUE;
        if (obj->pIndexData->vnum == 258)
        ball6 = TRUE;
        if (obj->pIndexData->vnum == 259)
        ball7 = TRUE;
      }
    }
    else
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if ( IS_OBJ_FLAG(obj, ITEM_ONMAP) && (obj->x == ch->x) && (obj->y == ch->y) && (obj->map == ch->map) )
        {
           if (obj->pIndexData->vnum == 253)
           ball1 = TRUE;
           if (obj->pIndexData->vnum == 254)
           ball2 = TRUE;
           if (obj->pIndexData->vnum == 255)
           ball3 = TRUE;
           if (obj->pIndexData->vnum == 256)
           ball4 = TRUE;
           if (obj->pIndexData->vnum == 257)
           ball5 = TRUE;
           if (obj->pIndexData->vnum == 258)
           ball6 = TRUE;
           if (obj->pIndexData->vnum == 259)
           ball7 = TRUE;
        }
        continue;
      }
    }
  }
  else if ( !str_cmp( argument, "evil_shenlong" ))
  {
    vnum = DRAGON_UNIVERSE;
    if ( !xIS_SET(ch->act, PLR_ONMAP) )
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->vnum == 270)
        ball1 = TRUE;
        if (obj->pIndexData->vnum == 271)
        ball2 = TRUE;
        if (obj->pIndexData->vnum == 272)
        ball3 = TRUE;
        if (obj->pIndexData->vnum == 273)
        ball4 = TRUE;
        if (obj->pIndexData->vnum == 274)
        ball5 = TRUE;
        if (obj->pIndexData->vnum == 275)
        ball6 = TRUE;
        if (obj->pIndexData->vnum == 276)
        ball7 = TRUE;
      }
    }
    else
    {
      for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
      {
        if ( IS_OBJ_FLAG(obj, ITEM_ONMAP) && (obj->x == ch->x) && (obj->y == ch->y) && (obj->map == ch->map) )
        {
           if (obj->pIndexData->vnum == 270)
           ball1 = TRUE;
           if (obj->pIndexData->vnum == 271)
           ball2 = TRUE;
           if (obj->pIndexData->vnum == 272)
           ball3 = TRUE;
           if (obj->pIndexData->vnum == 273)
           ball4 = TRUE;
           if (obj->pIndexData->vnum == 274)
           ball5 = TRUE;
           if (obj->pIndexData->vnum == 275)
           ball6 = TRUE;
           if (obj->pIndexData->vnum == 276)
           ball7 = TRUE;
        }
        continue;
      }
    }

  }
  else
  {
    send_to_char( "Syntax: Summon <Shenlong|Evil_Shenlong|Porunga>\n\r", ch);
    return;
  }
    if (!ball1 && !ball2 && !ball3 && !ball4 && !ball5 && !ball6 && !ball7 )
    {
       send_to_char( "&RAll of the Dragonballs are missing.\n\r", ch);
       return;
    }
    if (!ball1 || !ball2 || !ball3 || !ball4 || !ball5 || !ball6 || !ball7 )
    {
       send_to_char( "&RSome of the dragonballs are missing.\n\r", ch);
       return;
    }
    else
    {
      for ( obj = ch->in_room->first_content; obj; obj = obj_next )
      {
          obj_next = obj->next_content;
          if ( !str_cmp(argument, "porunga") && obj->pIndexData->vnum >= 253 && obj->pIndexData->vnum <= 259)
          extract_obj( obj );
          if ( !str_cmp(argument, "shenlong") && obj->pIndexData->vnum >= 260 && obj->pIndexData->vnum <= 266)
          extract_obj( obj );
          if ( !str_cmp(argument, "evil_shenlong") && obj->pIndexData->vnum >= 270 && obj->pIndexData->vnum <= 276)
          extract_obj( obj );
          
      }
      if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
      {
          send_to_char( "&RSomething went wrong, contact an Immortal.\n\r", ch );
          return;
      }
      victim = create_mobile( pMobIndex );
      char_to_room( victim, ch->in_room );
      if ( xIS_SET(ch->act, PLR_ONMAP))
      {
        xSET_BIT(victim->act, ACT_ONMAP);
        victim->map = ch->map;
        victim->x = ch->x;
        victim->y = ch->y;
      }
      if ( !str_cmp( argument, "shenlong" ))
      {
       send_to_char( "\n\r&zThe sky turns dark as you raise both of your arms.\n\r", ch);
       send_to_char( "&WYou are blinded by a flashing light, as you open your eyes, a &Ggreen&W dragon is standing there.\n\r", ch);
       send_to_char( "&RYou summon the mighty Dragon Shenlong.\n\r", ch);
       act( AT_WHITE, "&zThe sky turns dark as $n raises both of $s arms.", ch, NULL, NULL, TO_ROOM ); 
       act( AT_WHITE, "&WYou are blinded by a flashing light, as you open your eyes, a &Ggreen &Wdragon is standing there.", ch, NULL, NULL, TO_ROOM );
       act( AT_WHITE, "&R$n has summoned the mighty Dragon Shenlong.", ch, NULL, NULL, TO_ROOM );
       command( ch, do_echo, "%s has summoned Shenlong at %s!", ch->name, ch->in_room->name);
       dbtimer = 1;
      }
      else if ( !str_cmp( argument, "evil_shenlong" ))
      {
       send_to_char( "\n\r&zThe sky turns dark as you raise both of your arms.\n\r", ch);
       send_to_char( "&WYou are blinded by a flashing light, as you open your eyes, a &Rred&W dragon is standing there.\n\r", ch);
       send_to_char( "&RYou summon the almighty Dragon Evil Shenlong.\n\r", ch);
       act( AT_WHITE, "&zThe sky turns dark as $n raises both of $s arms.", ch, NULL, NULL, TO_ROOM );
       act( AT_WHITE, "&WYou are blinded by a flashing light, as you open your eyes, a &rred &Wdragon is standing there.", ch, NULL, NULL, TO_ROOM );
       act( AT_WHITE, "&R$n has summoned the almighty Dragon Evil Shenlong.", ch, NULL, NULL, TO_ROOM );
       command( ch, do_echo, "%s has summoned the &w&G&REvil&Y Shenlong at %s!", ch->name, ch->in_room->name);
       dbtimer2 = 1;
      }
      else if ( !str_cmp( argument, "porunga" ))
      {
       send_to_char( "\n\r&zThe sky turns dark as you raise both of your arms.\n\r", ch);
       send_to_char( "&WYou are blinded by a flashing light, as you open your eyes, a &Bmuscular&W dragon is standingthere.\n\r", ch);
       send_to_char( "&RYou summon the almighty Porunga.\n\r", ch);
       act( AT_WHITE, "&zThe sky turns dark as $n raises both of $s arms.", ch, NULL, NULL, TO_ROOM );
       act( AT_WHITE, "&WYou are blinded by a flashing light, as you open your eyes, a &Bmuscular &Wdragon is standingthere.", ch, NULL, NULL, TO_ROOM );
       act( AT_WHITE, "&R$n has summoned the almighty Porunga.", ch, NULL, NULL, TO_ROOM );
       command( ch, do_echo, "%s has summoned the &w&GPorunga&Y at %s!", ch->name, ch->in_room->name);
       dbtimer3 = 1;
      }
      return;
   }
}
  
void do_activate( CHAR_DATA *ch, char *argument )
{
 OBJ_DATA *capsule;
 OBJ_DATA *obj;
 capsule = get_eq_char( ch, WEAR_HOLD );

  if ( !capsule || capsule->item_type != ITEM_CAPSULE )
  {
    ch_printf( ch, "You don't seem to be holding anything to activate..\n\r" );
    return;
  }

  switch(capsule->value[2])
  {
   case 1:
    obj_from_char (capsule);
    extract_obj (capsule);
    obj = create_object( get_obj_index(OBJ_VNUM_SENSU), 5 );
    obj->count = 10;
    send_to_char( "&YYou click the capsule and throw it to the ground in a puff of smoke many sensus appear", ch);
    act( AT_PLAIN, "$n clicks a capsule and throws it down, in a puff of smoke many sensus appear.", ch, NULL, argument, TO_ROOM );
    obj_to_room(obj, ch->in_room, ch);
   break;
   case 2:
    obj_from_char (capsule);
    extract_obj (capsule); 
    obj = create_object( get_obj_index(OBJ_VNUM_SENSU), 5 );
    obj->count = 20;
    send_to_char( "&YYou click the capsule and throw it to the ground in a puff of smoke many sensus appear", ch);
    act( AT_PLAIN, "$n clicks a capsule and throws it down, in a puff of smoke sensus appear", ch, NULL, argument, TO_ROOM );
    obj_to_room(obj, ch->in_room, ch);
   break;
   case 3:
    obj_from_char (capsule);
    extract_obj (capsule);
    send_to_char ( "You click the capsule and throw it down, and dissapear in a puff of smoke.", ch);
    act( AT_PLAIN, "$n clicks a capsule and throws it down, in a puff of smoke they dissapear", ch, NULL, argument, TO_ROOM );
    char_from_room(ch);
    char_to_room( ch, get_room_index( ROOM_VNUM_JPA ) );
   break;
   default:
         send_to_char( "&RTHERE IS SOMETHING WRONG WITH YOUR CAPSULE\n\r", ch);
   break;
  }
  return;
}

char *drawmeter(float per)
{
   static char buf[MAX_STRING_LENGTH];


   if (per < 0) per = 0;
   if (per > 100) per = 100;
                   if (per == 100)  sprintf(buf, "&z[{p       {x&R100.0%%{p       {x&z]");
   else if (per >= 95 && per < 100 ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p      {x &z]", per);
   else if (per >= 90 && per < 95  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p     {x  &z]", per);
   else if (per >= 85 && per < 90  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p    {x   &z]", per);
   else if (per >= 80 && per < 85  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p   {x    &z]", per);
   else if (per >= 75 && per < 80  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p  {x     &z]", per);
   else if (per >= 70 && per < 75  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{p {x      &z]", per);
   else if (per >= 65 && per < 70  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{x       {x&z]", per);
   else if (per >= 60 && per < 65  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{x       {x&z]", per);
   else if (per >= 55 && per < 60  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{x       {x&z]", per);
   else if (per >= 35 && per < 70  ) sprintf(buf, "&z[{p       {x&R%0.2f%%{x       {x&z]", per);
   else if (per >= 30 && per < 35  ) sprintf(buf, "&z[{p      {x &R%0.2f%%{x       {x&z]", per);
   else if (per >= 25 && per < 30  ) sprintf(buf, "&z[{p     {x  &R%0.2f%%{x       {x&z]", per);
   else if (per >= 20 && per < 25  ) sprintf(buf, "&z[{p    {x   &R%0.2f%%{x       {x&z]", per);
   else if (per >= 15 && per < 20  ) sprintf(buf, "&z[{p   {x    &R%0.2f%%{x       {x&z]", per);
   else if (per >= 10 && per < 15  ) sprintf(buf, "&z[{p  {x     &R%0.2f%%{x       {x&z]", per);
   else if (per >= 5 && per < 10   ) sprintf(buf, "&z[{p {x      &R%0.2f%%{x       {x&z]", per);
   else     sprintf(buf, "&z[       &R%0.2f%%       &z]", per);

   return buf;
}

void do_kilimeter(CHAR_DATA *ch, char *argument)
{
	float percent, nextform, kili;
	
	if(IS_NPC(ch))
		return;

	if(!xIS_SET(ch->act, PLR_SKINNY) && !xIS_SET(ch->act, PLR_FAT))
		nextform = 5000000;
	else
		nextform = 100000000;

	percent = ch->pcdata->kili;
	percent = percent/nextform * 100;
	kili = percent;
        ch_printf(ch, "\n\r&WKilimeter&w: %s\n\r", drawmeter(kili));
	kili = floor(kili);
	if(kili < 12)
		kili = 0;
	else if(kili < 25)
		kili = 12;
	else if(kili < 36)
		kili = 25;
	else if(kili < 50)
		kili = 36;
	else if(kili < 62)
		kili = 50;
	else if(kili < 75)
		kili = 62;
	else if(kili < 87)
		kili = 75;
	else if(kili < 100)
		kili = 87;
	else
		kili = 100;
/*	send_to_char("\n\r&W    _______\r\n", ch);
	switch(kili)
	{
		case 0:
			send_to_char("&W   /   |   \\ \r\n", ch);
			send_to_char("&W  /    |    \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);
		break;
		case 100:
			send_to_char("&W   /   |   \\ \r\n", ch);
			send_to_char("&W  /    |    \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);
		break;
		case 12:
			send_to_char("&W   /    /  \\ \r\n", ch);
			send_to_char("&W  /    /    \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);
		break;
		case 25:
			send_to_char("&W   /       \\ \r\n", ch);
			send_to_char("&W  /     ___ \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);
		break;
		case 36:
			send_to_char("&W   /       \\ \r\n", ch);
			send_to_char("&W  /         \\ \r\n", ch);
			send_to_char("&W  \\    \\    /   \r\n", ch);
			send_to_char("&W   \\    \\  /    \r\n", ch);			
		break;
		case 50:
			send_to_char("&W   /       \\ \r\n", ch);
			send_to_char("&W  /         \\ \r\n", ch);
			send_to_char("&W  \\    |    /   \r\n", ch);
			send_to_char("&W   \\   |   /    \r\n", ch);			
		break;
		case 62:
			send_to_char("&W   /       \\ \r\n", ch);
			send_to_char("&W  /         \\ \r\n", ch);
			send_to_char("&W  \\  /      /   \r\n", ch);
			send_to_char("&W   \\/      /    \r\n", ch);			
		break;
		case 75:
			send_to_char("&W   /       \\ \r\n", ch);
			send_to_char("&W  /  ___    \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);			
		break;
		case 87:
			send_to_char("&W   / \\     \\ \r\n", ch);
			send_to_char("&W  /   \\     \\ \r\n", ch);
			send_to_char("&W  \\         /   \r\n", ch);
			send_to_char("&W   \\       /    \r\n", ch);			
		break;
	}
	send_to_char("&W    -------\r\n", ch);
*/
	ch_printf(ch, "&RYou are currently working towards:&W %s\r\n", (!xIS_SET(ch->act, PLR_SKINNY) && !xIS_SET(ch->act, PLR_FAT))? "Alignment Form" : 
			"Ultimate Power");
		
	  			
}


		

void do_study( CHAR_DATA *ch, char *argument )
{
    int sn, per, lvl, temp;
    bool ability=FALSE;
    SKILLTYPE *skill = NULL;
    temp = 0;
    if ( IS_NPC(ch))
    return;
    if ( !IN_LIBRARY(ch) )
    {
       	send_to_char( "You must be inside a library to be able to study.\n\r", ch );
       	return;
    }
    if ( ch->skill_lvl <= 0 )
    {
       	send_to_char( "You have no skill leveling training sessions left!\n\r", ch );
       	return;
    }     
    if ( ( sn = sskill_lookup( ch, argument ) ) >= 0 )
    {
          ability = FALSE;
          skill = sskill_table[sn];
    }
    else if ( ( sn = askill_lookup( ch, argument ) ) >= 0 )
    {
          skill = askill_table[sn];
          ability = TRUE;
    }
    else
    {
       	send_to_char( "No such skill.\n\r", ch );
       	return;
    }
    if ( !skill )
    {
    	send_to_char( "No such Skill.\n\r", ch );
        return;
    }
    if ( skill->value < 0 )
    {
    	send_to_char( "You can't level up that skill.\n\r", ch );
        return;
    }
    if ( ability )
    {
      if ( ch->pcdata->skill_level2[sn] >= 15 )
      {
     	send_to_char( "You have reached the limits of this skill.\n\r", ch );
        return;
      }
    }
    else
    {
      if ( ch->pcdata->skill_level[sn] >= 15 )
      {
     	send_to_char( "You have reached the limits of this skill.\n\r", ch );
        return;
      }
    }
    if ( ability )
    per = ch->pcdata->learned2[sn];
    else
    per = ch->pcdata->learned[sn];

    if ( per <= 0 )
    {
    	send_to_char( "You don't know anything about that skill.\n\r", ch );
        return;
    }
    else if ( per < 45 )
    {
    	send_to_char( "You need to be more adept at this skill before studying it more.\n\r", ch );
        return;
    }
    ch->skill_lvl--;
    if ( ability )
    {
        
        lvl = ch->pcdata->skill_level2[sn]+1;
        temp = ch->pcdata->skill_level2[sn];
        ch->pcdata->skill_level2[sn] += 1;  
        if (  can_userace(ch, sn) == FALSE )
        {
                ch_printf(ch, "\n\rYou can't continue to upgrade %s.\n\r", capitalize(skill->name) );
                ch->pcdata->skill_level2[sn] = temp;
		return;
        }
        ch->pcdata->skill_level2[sn] = temp;
        ch_printf(ch, "\n\r&GYou pick up a book and you begin to study the art of %s.\n\r", capitalize(askill_table[sn]->name));
        ch_printf(ch,"&Y%s has increased to level %d!\n\r", capitalize(askill_table[sn]->name), lvl);
       	ch->pcdata->skill_level2[sn] += 1 ;
        ch->pcdata->learned2[sn] = URANGE( 15, ch->pcdata->learned2[sn]-number_range(1, 6), 100);
        if ( ch->pcdata->skill_level2[sn] == 15 )
                ch_printf(ch, "&YYou have reached the maximum level of %s!\n\r", capitalize(askill_table[sn]->name) );

    }
    else
    {
        lvl = ch->pcdata->skill_level[sn]+1;
        ch_printf(ch, "\n\r&GYou pick up a book and you begin to study the art of %s.\n\r", capitalize(sskill_table[sn]->name));
        ch_printf(ch,"&Y%s has increased to level %d!\n\r", capitalize(sskill_table[sn]->name), lvl);
       	ch->pcdata->skill_level[sn] += 1;
        ch->pcdata->learned[sn] = URANGE( 15, ch->pcdata->learned[sn]-number_range(1, 6), 100);
        if ( ch->pcdata->skill_level[sn] == 15 )
                ch_printf(ch, "&YYou have reached the maximum level of %s!\n\r", capitalize(sskill_table[sn]->name) );

    }
}


void do_skillinfo( CHAR_DATA *ch, char *argument )
{
   int sn;
   SKILLTYPE *skill;
   char buf[MSL];
   bool ability=FALSE;

   	if ( IS_NPC(ch))
	return;

        if ( ( sn = sskill_lookup( ch, argument ) ) >= 0 )
            skill = sskill_table[sn];
        else
        if ( ( sn = askill_lookup( ch, argument ) ) >= 0 )
        {
            skill = askill_table[sn];
	    ability = TRUE;
	}	
        else
        {
            send_to_char( "No such skill or ability.\n\r", ch );
            return;
        }
        if ( !skill )
        {
            send_to_char( "Not created yet.\n\r", ch );
            return;
        }
   	send_to_char( "\n\r&z+-------------------------------------------------------+", ch);
        send_to_char( "\n\r&z|                   &RSkill Stats &z                        |", ch);
   	send_to_char( "\n\r&z+-------------------------------------------------------+\n\r", ch);
      if ( skill->value >= 0 )
        ch_printf( ch,"  &CName&c:&W    %s\n\r", skill->name );
        ch_printf( ch,"  &CSyntax&c:&W  %s\n\r", skill->keyword );
        if ( skill->min_mana < 0 )
   		sprintf( buf, "  1/%d", convert_pos( skill->min_mana ) );
        else
                sprintf( buf, "  %d", skill->min_mana );

        ch_printf( ch,"  &CEnergy&c:&W  %s\n\r",  buf );
      if ( ability )
        ch_printf( ch,"  &CRaces&c:&W   %s\n\r", ext_flag_string_capitalize(&skill->raceskill, raceskills_name));
        send_to_char( "\n\r&z+-------------------------------------------------------+\n\r", ch);

      return;


}

void do_cskills( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC(ch))
  {
      send_to_char( "Huh?\n\r", ch);
	return;      
  }
  send_to_char( "\n\r&zCurrent Skills&W:\n\r", ch);
  send_to_char( "&z-------------------------------\n\r\n\r", ch);
  if ( xIS_EMPTY( ch->skill ) )
  {
    send_to_char( "&w-&YNone\n\r", ch );
  }
  else
  {
    if (xIS_SET(ch->skill, SKILL_ENRAGE) )
      send_to_char( "&w-&RE&rnrage\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_BERSERK) )
      send_to_char( "&w-&YB&Oerserk\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_OVERDRIVE) )
      send_to_char( "&w-&GO&Bverdrive\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_BBARRIER) )
      send_to_char( "&w-&BB&blue &BB&barrier &BC&bhip\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_KYODAIKA) )
      send_to_char( "&w-&gK&cyodaika\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_SOULCRY) )
      send_to_char( "&w-&pS&zoul &pC&zry\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_TIMEFREEZE) )
      send_to_char( "&w-&bT&Oime &bF&Oreeze\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_PBARRIER) )
      send_to_char( "&w-&PP&Wink &PB&Warrier\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_REGENERATION) )
      send_to_char( "&w-&CR&cegeneration\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_ZANZOKEN) )
      send_to_char( "&w-&WZ&zanzoken\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_VSTANCE) )
      send_to_char( "&w-&PV&picious &PS&ptance\n\r", ch);
    if (xIS_SET(ch->skill, SKILL_ACCURACY) )
      send_to_char( "&w-&GA&gccuracy\n\r", ch);
  }
  send_to_char( "\n\r&z-------------------------------\n\r", ch);
}

int pollyes;
int pollno;
bool poll_in_progress;
char *pollarg;

void save_votes(void)
{
  POLL_DATA *vote;
  FILE *fp;

  unlink( VOTE_LIST );

    if ( (fp=fopen( VOTE_LIST, "w" )) == NULL )
    {
        bug( "Cannot open vote.lst for writting", 0 );
        perror( VOTE_LIST );
        return;
    }

  for (vote = first_vote; vote; vote = vote->next)
    fprintf(fp, "%s~\n", vote->ip);
  fprintf(fp, "$~\n");
  fclose(fp);

  return;
}

void load_votes( void )
{
  POLL_DATA *vote;
  FILE *fp;

  if ( !(fp = fopen( VOTE_LIST, "r" )) )
    return;

  for ( ; ; )
  {
    if ( feof( fp ) )
    {
      bug( "Load_votes: no $ found." );
      fclose(fp);
      return;
    }
    CREATE(vote, POLL_DATA, 1);
    LINK( vote, first_vote, last_vote, next, prev );

    vote->ip = fread_string_nohash(fp);
    if (*vote->ip == '$')
      break;
  }
  DISPOSE(vote->ip);
  DISPOSE(vote);
  fclose(fp);
  return;
}
void do_poll( CHAR_DATA *ch, char *argument )
{
   char arg[MSL];
   
   argument = one_argument( argument, arg );
   if ( ch->level < 65 )
   {
      ch_printf (ch,"\n\r&YPoll&O: &C%s", pollarg ? pollarg : "Null");
      send_to_char ("\n\r&z----------------------------", ch );
      ch_printf(ch, "\n\r&GYes&g: &W%d", pollyes );
      ch_printf(ch, "\n\r&RNo&g:  &W%d", pollno  );
      send_to_char ("\n\r&z----------------------------", ch );
      ch_printf(ch, "\n\r&WStatus&O: %s", poll_in_progress == FALSE ? "&RClosed" : "&GOpen" );
      send_to_char( "\n\r", ch);
      return;
   }
   if ( arg[0] == '\0' )
   {
      send_to_char("\n\rSyntax: poll new [argument]\n\r", ch);
      send_to_char("             close\n\r", ch);
      send_to_char("             score\n\r", ch);
      return;
   }
   if ( !str_cmp( arg, "score" ) )
   {
      ch_printf (ch,"\n\r&YPoll&O: &C%s", pollarg ? pollarg : "Null");
      send_to_char ("\n\r&z----------------------------", ch );
      ch_printf(ch, "\n\r&GYes&g: &W%d", pollyes );
      ch_printf(ch, "\n\r&RNo&g:  &W%d", pollno );
      send_to_char ("\n\r&z----------------------------", ch );
      ch_printf(ch, "\n\r&WStatus&O: %s", poll_in_progress == FALSE ? "&RClosed" : "&GOpen" );
      send_to_char( "\n\r", ch);
      return;
   }
   if ( !str_cmp( arg, "close" ) )
   {
      if ( poll_in_progress == FALSE )
      {
         send_to_char( "&RIts already closed.\n\r", ch);
         return;
      }
      send_to_char( "&RYou have closed the poll.\n\r", ch);
      command ( ch, do_echo, "&B|&RPOLL&B| &z::&G%s has closed the poll that was currently in progress.&z::", ch->name );
      poll_in_progress = FALSE;
   }
   if ( !str_cmp( arg, "new" ) )
   {
      if ( poll_in_progress == TRUE )
      {
         send_to_char( "&RThere is another poll in progress.\n\r", ch);
         return;
      }
      if ( argument[0] == '\0' )
      {
         send_to_char("&RYou need an argument for the poll.\n\r", ch);
         return;
      }
      pollno = pollyes = 0;
      pollarg = str_dup( argument );
      ch_printf( ch, "&CYou have opened a poll on&c: &Y%s\n\r", pollarg );
      poll_in_progress = TRUE;
      command ( ch, do_echo, "&B|&RPOLL&B| &z::&G%s has opened a poll on&g: &W%s.&z::", ch->name, pollarg );

   }   

}


void do_vote( CHAR_DATA *ch, char *argument )
{
   POLL_DATA *vote;

 
   if ( argument[0] == '\0' )
   {
      send_to_char("&YIs your vote 'yes' or 'no'?.\n\r", ch);
     return;
   }
 
   if ( poll_in_progress == FALSE )
   {
     send_to_char( "There is no poll in progress.\n\r.", ch);
     return;
   }

   if ( str_cmp( argument, "no" ) && str_cmp( argument, "yes") )
   {
      send_to_char("&YIs your vote 'yes' or 'no'?.\n\r", ch);
      return;

   }
   for (vote = first_vote; vote; vote = vote->next)
   {
      if ( vote->ip == ch->desc->host )
      {
        send_to_char("&RYou have already voted.\n\r", ch);
        return;
      }
      continue;
   }
   if ( !str_cmp( argument, "yes" ) )
   {
      pollyes++;
      send_to_char("&YYou have casted in your vote.\n\r", ch);
   }
   else if ( !str_cmp( argument, "no" ) )
   {
      pollno++;
      send_to_char("&YYou have casted in your vote.\n\r", ch);
   }
   else
   return;
   CREATE(vote, POLL_DATA, 1);
   LINK( vote, first_vote, last_vote, next, prev );
   vote->ip = STRALLOC(ch->desc->host);
   save_votes();

   command ( supermob, do_echo, "&B|&RPOLL&B| &z::&G%s has casted in their vote.&z::", ch->name );
}
