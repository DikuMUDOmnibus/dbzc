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
 * 		Commands for personal player settings/statictics	    *
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


/*
 *  Locals
 */

void do_gold(CHAR_DATA * ch, char *argument)
{
   set_char_color( AT_GOLD, ch );
   ch_printf( ch,  "You have %s gold pieces.\n\r", num_punct(ch->gold) );
   return;
}
char *drawlife(int min, int max){
   static char buf[MAX_STRING_LENGTH];
   int per;
   per = ((min * 100) / max);
   if (per < 0) per = 0;
   if (per > 100) per = 100;
                   if (per == 100)  sprintf(buf, "&G&W[&R|||||&Y||||&G||||&W]");
   else if (per >= 90 && per < 100) sprintf(buf, "&G&W[&R|||||&Y||||&G|||&G-&W]");
   else if (per >= 80 && per < 90)  sprintf(buf, "&G&W[&R|||||&Y||||&G||&G--&W]");
   else if (per >= 70 && per < 80)  sprintf(buf, "&G&W[&R|||||&Y||||&G|&G---&W]");
   else if (per >= 60 && per < 70)  sprintf(buf, "&G&W[&R|||||&Y|||&G&G-----&W]");
   else if (per >= 50 && per < 60)  sprintf(buf, "&G&W[&R|||||&Y||&G&G------&W]");
   else if (per >= 40 && per < 50)  sprintf(buf, "&G&W[&R|||||&Y|&G&G-------&W]");
   else if (per >= 30 && per < 40)  sprintf(buf, "&G&W[&R|||||&Y&G&G--------&W]");
   else if (per >= 30 && per < 40)  sprintf(buf, "&G&W[&R||||&Y&G&G---------&W]");
   else if (per >= 20 && per < 40)  sprintf(buf, "&G&W[&R|||&Y&G&G----------&W]");
   else if (per >= 10 && per < 40)  sprintf(buf, "&G&W[&R||&Y&G&G-----------&W]");
   else if (per >= 0 &&  per < 10)  sprintf(buf, "&G&W[&R&Y&G&G-------------&W]");
   //else sprintf(buf, "&G&W[&R&W]");
   return buf;
}


void do_dbzdlscore( CHAR_DATA *ch, char *argument)
{
   char buf[MSL];
    if (get_curr_armor(ch) <= 1)
        sprintf(buf, "the rags of a beggar");
    else if (get_curr_armor(ch) <= 30)
        sprintf(buf, "improper for adventure");
    else if (get_curr_armor(ch) <= 75)
        sprintf(buf, "shabby and threadbare");
    else if (get_curr_armor(ch) <= 100)
        sprintf(buf, "of poor quality");
    else if (get_curr_armor(ch) <= 125)
        sprintf(buf, "scant protection");
    else if (get_curr_armor(ch) <= 150)
        sprintf(buf, "that of a knave");
    else if (get_curr_armor(ch) <= 175)
        sprintf(buf, "moderately crafted");
    else if (get_curr_armor(ch) <= 200)
        sprintf(buf, "well crafted");
    else if (get_curr_armor(ch) <= 225)
        sprintf(buf, "the envy of squires");
    else if (get_curr_armor(ch) <= 250)
        sprintf(buf, "excellently crafted");
    else if (get_curr_armor(ch) <= 275)
        sprintf(buf, "the envy of knights");
    else if (get_curr_armor(ch) <= 300)
        sprintf(buf, "the envy of barons");
    else if (get_curr_armor(ch) <= 325)
        sprintf(buf, "the envy of dukes");
    else if (get_curr_armor(ch) <= 350)
        sprintf(buf, "the envy of emperors");
    else
        sprintf(buf, "that of an avatar");
   send_to_char("\n\r", ch);
   send_to_char("&z|+================================================================================+|\n\r", ch);
   send_to_char("&z||                                                                                ||\n\r", ch);
   ch_printf(ch,"&z||   &YName&O:&W %-15.15s   &YWeight&O:&W %s               &YAlign&O:&W %5d   &z||\n\r", ch->name, drawlife(ch->carry_weight, can_carry_w(ch)), ch->alignment);
   ch_printf(ch,"&z||   &YRace&O:&W %-15.15s    &YItems&O:&W %s                 &YAge&O:&W %5d   &z||\n\r", race_table[ch->race]->race_name, drawlife(ch->carry_number, can_carry_n(ch)), get_age(ch));
   ch_printf(ch,"&z||   &YClan&O:&W %-15.15s    &YStyle&O:&W %-14.14s                &YWimpy&O:&W %5d   &z||\n\r",ch->pcdata->clan ? ch->pcdata->clan->name : "None", ch->style == STYLE_AGGRESSIVE ? "Aggressive" : "Defensive", ch->wimpy);
   ch_printf(ch,"&z||                                                                                &z||\n\r", ch);
   ch_printf(ch,"&z||    &YSTR&O:&W %2.2d(%2.2d)              &YGold&O:&W %-14d                            &z||\n\r", ch->perm_str, get_curr_str(ch), ch->gold);
   ch_printf(ch,"&z||    &YINT&O:&W %2.2d(%2.2d)             &YBless&O:&W %-14d                &YTrain&O:&W %5d   &z||\n\r", ch->perm_int, get_curr_int(ch), ch->pcdata->bless, ch->stat);
   ch_printf(ch,"&z||    &YWIS&O:&W %2.2d(%2.2d)              &YLife&O:&W %-3d &Yof&W %-3d                   &YMkills&O:&W %5d   &z||\n\r", ch->perm_wis, get_curr_wis(ch), ch->hit, ch->max_hit, ch->pcdata->mkills);
   ch_printf(ch,"&z||    &YDEX&O:&W %2.2d(%2.2d)                &YKi&O:&W %-7d &Yof&W %-7d          &YMdeaths&O:&W %5d   &z||\n\r", ch->perm_dex, get_curr_dex(ch), ch->mana, ch->max_mana, ch->pcdata->mdeaths);
   ch_printf(ch,"&z||    &YCON&O:&W %2.2d(%2.2d)                &YPL&O:&W %-10lld &Yof&W %-9ld                      &z||\n\r", ch->perm_con, get_curr_con(ch), get_curr_pl2(ch), ch->exp);
   if (IS_BIODROID(ch))
   ch_printf(ch,"&z||    &YCHA&O:&W %2.2d(%2.2d)             &YArmor&O:&W %-17.17s           &YBiomass&O:&W %5d   &z||\n\r", ch->perm_cha, get_curr_cha(ch), buf, ch->biomass);
   else                                                                                                   
   ch_printf(ch,"&z||    &YCHA&O:&W %2.2d(%2.2d)             &YArmor&O:&W %-17.17s                            &z||\n\r", ch->perm_cha, get_curr_cha(ch), buf);
   ch_printf(ch,"&z||    &YLCK&O:&W %2.2d(%2.2d)                                                                 &z||\n\r", ch->perm_lck, get_curr_lck(ch));
   send_to_char("&z||                                                                                ||\n\r", ch);
   send_to_char("&z|+================================================================================+|\n\r", ch);
 }


void do_newscore(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH], buf2[MSL];
    CLAN_DATA *clan = NULL;
    if (IS_NPC(ch))
    {
	do_oldscore(ch, argument);
	return;
    }
    set_char_color(AT_SCORE, ch);
    send_to_char( "\n\r\n\r", ch);
       sprintf( buf2, "&w-&W<&z( &Y%s&c%s &z)&W>&w-", ch->name, ch->pcdata->title );
       ch_printf( ch, "  %s", centertext( buf2, 75 ));

    if ( get_trust( ch ) != ch->level )
	pager_printf( ch, "  You are trusted at level %d.\n\r", get_trust( ch ) );
  if ( xIS_SET(ch->pcdata->flags, PCFLAG_STAT) )
  {
    send_to_char("\n\r &W=========&RStats&W===============================================================\n\r\n\r", ch);
    if (ch->level > 49)
    {
    ch_printf(ch, "  &zLevel&O: &w%-3d          &zRace&O: &w%-10.10s      &zPlayed&O: &w%d hours\n\r",
	ch->level, capitalize(get_race(ch)), (get_age(ch) - 4) * 2);
    }
    else
    {
    ch_printf(ch,"  &zBless&O: &w%-6d       &zRace&O: &w%-10.10s      &zPlayed&O:&w %d hours\n\r",
                 ch->pcdata->bless, capitalize(get_race(ch)), (get_age(ch) - 4)* 2);
    }
    ch_printf(ch,"  &zYears&O: &w%-6d      &zClass&O: &w%-11.11s     &zLog In&O: &w%s\r",
                 get_age(ch), capitalize(get_class(ch)), ctime(&(ch->logon)) );

    if (ch->alignment > 900)
        sprintf(buf, "devout");
    else if (ch->alignment > 700)
        sprintf(buf, "noble");
    else if (ch->alignment > 350)
        sprintf(buf, "honorable");
    else if (ch->alignment > 100)
        sprintf(buf, "worthy");
    else if (ch->alignment > -100)
        sprintf(buf, "neutral");
    else if (ch->alignment > -350)
        sprintf(buf, "base");
    else if (ch->alignment > -700)
        sprintf(buf, "evil");
    else if (ch->alignment > -900)
        sprintf(buf, "ignoble");
    else
        sprintf(buf, "fiendish");

	ch_printf(ch, "    &zSTR&O: &W%2.2d&z(&w%2.2d&z)      Align&O: &w%-15.15s  &zSaved&O:&w %s\r",
		get_curr_str(ch), ch->perm_str, buf, ch->save_time ? ctime(&(ch->save_time)) : "no\n" );

        ch_printf(ch, "    &zINT&O: &W%2.2d&z(&w%2.2d&z)      Pos'n&O: &w%-15.15s     &zTime&O:&w %s\r",
                get_curr_int(ch), ch->perm_int, ch->position == POS_DEAD ? "&wdead" :
          ch->position == POS_MORTAL  ? "&wmortal"  :
          ch->position == POS_INCAP  ? "&wincapacitated"  :
          ch->position == POS_STUNNED  ? "&wstunned"  :
          ch->position == POS_SLEEPING  ? "&wsleeping"  :
          ch->position == POS_RESTING  ? "&wresting"  :
          ch->position == POS_STANDING  ? "&wstanding"  :
          ch->position == POS_FIGHTING  ? "&wfighting"  :
          ch->position == POS_MOUNTED ? "&wmounted"  :
          ch->position == POS_SITTING  ? "&wsitting"  : "&wstanding", ctime(&current_time) );

        sprintf(buf, "&G%d&z/&g%d", ch->hit, ch->max_hit );
	ch_printf(ch, "    &zWIS&O: &W%2.2d&z(&w%2.2d&z)         &zLF&O: &w%-15.15s        &zItems&O:  &w%d &z(&Wmax %d&z)\n\r",
		get_curr_wis(ch), ch->perm_wis,  buf, ch->carry_number, can_carry_n(ch));
        sprintf(buf, "&B%ld&z/&b%ld", ch->mana, ch->max_mana );
	ch_printf(ch, "    &zDEX&O: &W%2.2d&z(&w%2.2d&z)         &zKi&O: &w%-20.20s  &zWeight&O:  &w%d &z(&Wmax %d&z)\n\r",
		get_curr_dex(ch), ch->perm_dex, buf, ch->carry_weight, can_carry_w(ch));
        ch_printf(ch, "    &zCON&O: &W%2.2d&z(&w%2.2d&z)         &zPL&O: &R%-11ld     &zMkills&O: &w%-5d\n\r",
	get_curr_con(ch), ch->perm_con, ch->exp, ch->pcdata->mkills );


    /*
     * Fighting style support -haus
     */
        ch_printf(ch, "    &zCHA&O: &W%2.2d&z(&w%2.2d&z)       &zGold&O: &Y%-13lld  &zMDeaths&O: &w%-5d\n\r",
 	get_curr_cha(ch), ch->perm_cha, ch->gold, ch->pcdata->mdeaths);
        ch_printf(ch, "    &zLCK&O: &W%2.2d&z(&w%2.2d&z)     &zToggle&O: &w%-8.8s\n\r",
		get_curr_lck(ch), ch->perm_lck, xIS_SET(ch->act, PLR_TOGGLE) ? "Spar" : "Kill");

    if (ch->pcdata->release_date)
    ch_printf(ch, "  &YYou will be released from hell at &W%24.24s&Y.\n\r", ctime(&ch->pcdata->release_date));
    if (ch->pcdata->silenced_date)
    ch_printf(ch, "  &YYou will be unsilenced &W%24.24s&Y.\n\r", ctime(&ch->pcdata->silenced_date));
    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
        ch_printf(ch, "  &WYou are bestowed with the command(s)&O: &Y%s\n\r",
                ch->pcdata->bestowments );
   }
   if ( xIS_SET(ch->pcdata->flags, PCFLAG_COMBAT) )
   {
    send_to_char("\n\r &W=========&RCombat&W==============================================================\n\r\n\r", ch);

    ch_printf(ch, "         &zP.Atk&O: &w%-5d              &zE.Atk&O: &w%-5d            &zEvasion&O: &w%-5d\n\r", get_curr_patk(ch), get_curr_eatk(ch), get_curr_evasion(ch));
    ch_printf(ch, "         &zP.Def&O: &w%-5d              &zE.Def&O: &w%-5d           &zAccuracy&O: &w%-5d\n\r", get_curr_pdef(ch), get_curr_edef(ch), get_curr_accuracy(ch));
    ch_printf(ch, "      &zCritical&O: &w%-5d              &zStyle&O: &w%-5s              &zWimpy&O: &w%-5d\n\r", get_curr_critical(ch), ch->style == STYLE_AGGRESSIVE ? "Agg" : "Def", ch->wimpy);
   }
   if ( xIS_SET(ch->pcdata->flags, PCFLAG_INFO) )
   {
    send_to_char("\n\r &W=========&RInfo&W================================================================\n\r\n\r", ch);

    ch_printf(ch, "         &zTrain&O: &w%-5d             &zSlevel&O: &w%-5d               &zAura&O: %s%s\n\r", ch->stat, ch->skill_lvl, get_aura_color(ch->auracolor), capitalize(get_aura_name(ch->auracolor)) );
    ch_printf(ch, "       &zKi Gain&O: &w%-5d          &zStat Gain&O: &w%-5d        &zSlevel Gain&O: &w%-5d\n\r", ch->skigain, ch->statgain, ch->skillgain );
    if ( IS_MUTANT(ch))
    {
    ch_printf(ch, "     &zPrimSkill&O:&w %-12.12s    &zSecSkill&O:&w %-15s\n\r",
        (xIS_SET(ch->act, PLR_LS))? "LargeSize" : (xIS_SET(ch->act, PLR_SS))? "SmallSize":
        (xIS_SET(ch->act, PLR_HK))? "HighKi" :
        (xIS_SET(ch->act, PLR_SK))? "StoneSkin" : (xIS_SET(ch->act, PLR_MB))? "MegaBrain" : "None",
        (xIS_SET(ch->act, PLR_AS))? "AugmentedStrength" : (xIS_SET(ch->act, PLR_IS))? "ImprovedSpeed" :
        (xIS_SET(ch->act, PLR_MS))? "MetallicSkin" : (xIS_SET(ch->act, PLR_EK))? "ExtensiveKnowledge":
        (xIS_SET(ch->act, PLR_KC))? "KiControl" : "None" );
    }
    if ( IS_BIODROID(ch))
    {
        ch_printf(ch, "         &zStage&O: &R%-12.12s     &zPrimAtt&O: &w%-12.12s     &zBiomass&O: &R%d\n\r",
                (xIS_SET(ch->powerup, PU_LARVAE)) ? "Larvae" :
                (xIS_SET(ch->powerup, PU_ADULT)) ? "Adult" :
                (xIS_SET(ch->powerup, PU_SEMIP)) ? "Semi-Perfect" :
                (xIS_SET(ch->powerup, PU_PERFECT)) ? "Perfect" : "None",
                (xIS_SET(ch->act, PLR_TRIBEAM))? "Tribeam" :
                (xIS_SET(ch->act, PLR_MASENKO))? "Masenko" : (xIS_SET(ch->act, PLR_SCATTER))? "Scattershot" :
                (xIS_SET(ch->act, PLR_MT))? "Mental Throw" : "None", ch->biomass );
    }
   }
    ch_printf(ch, "         &zQuest&O: &w%.1f\n\r", ch->quest );
   if ( xIS_SET(ch->pcdata->flags, PCFLAG_PKILL) )
   { 
        send_to_char("\n\r &W=========&RPKill Data&W==========================================================\n\r\n\r", ch);
        ch_printf(ch,"        &zPkills&O: &w%-7d&z   Illegal Pkills&O: &w%-5d&z            Pdeaths&O: &w%-5d\n\r",
		ch->pcdata->pkills, ch->pcdata->illegal_pk, ch->pcdata->pdeaths );
   }
    clan = ch->pcdata->clan;
    if ( clan && xIS_SET(ch->pcdata->flags, PCFLAG_CLAN) )
    {
        send_to_char("\n\r &W=========&RClan Data&W==========================================================\n\r\n\r", ch);
        ch_printf(ch, "          &zName&O: &w%s\n\r", clan->name );
        ch_printf(ch, "        &zLeader&O: &w%-8.8s         &zNumber1&O: &w%-8.8s         &zNumber2&O: &w%-8.8s\n\r", clan->leader, clan->number1, clan->number2 );
        ch_printf(ch, "         &zLevel&O: &w%-5d            &zMembers&O: &w%-5d               &zGold&O: &Y%lld\n\r", clan->level, clan->members, clan->gold );
	ch_printf(ch, "        &zPkills&O: &w%-5d            &zPdeaths&O: &w%-5d\n\r",
                (clan->pkills[6]+clan->pkills[0]+clan->pkills[1]+
                 clan->pkills[2]+clan->pkills[3]+clan->pkills[4]+
                 clan->pkills[5]),
                (clan->pdeaths[6]+clan->pdeaths[0] + clan->pdeaths[1] +
                 clan->pdeaths[2] + clan->pdeaths[3] +clan->pdeaths[4] +
                 clan->pdeaths[5] ) );
    }
/* if (ch->level > 6) 
 { 

           send_to_pager("\n\r &W==========&RImmortal Data&W=====================================================\n\r\n\r", ch);
           pager_printf(ch, "  &zEnter&O:&w %s %s\n\r", ch->name, (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "appears in a swirling mist.");
	   pager_printf(ch, "   &zExit&O:&w %s %s\n\r", ch->name, (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "leaves in a swirling mist.");
        
          if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
                  ch_printf(ch, "  &zYou are bestowed with the command(s)&O: &w%s\n\r", ch->pcdata->bestowments );
   
} */ 
    send_to_char("\n\r &W=============================================================================\n\r\n\r", ch);
 send_to_pager("\n\r", ch);
    return;
}

/*
void do_newscore(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	do_oldscore(ch, argument);
	return;
    }
    set_char_color(AT_SCORE, ch);

    ch_printf(ch, "\n\r&C%s%s.\n\r", ch->name, ch->pcdata->title);
    if ( get_trust( ch ) != ch->level )
	pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
    
    send_to_char("&W----------------------------------------------------------------------------\n\r", ch);
    if (ch->level > 49)
    {
    ch_printf(ch, "&CLevel: &W%-3d         &CRace : &W%-10.10s      &CPlayed: &W%d &Chours\n\r",
	ch->level, capitalize(get_race(ch)), (get_age(ch) - 4) * 2);
    ch_printf(ch, "&CYears: &W%-6d      &CClass: &W%-11.11s     &CLog In: %s\r",
		get_age(ch), capitalize(get_class(ch)), ctime(&(ch->logon)) );
    }
    else
    {
    ch_printf(ch,"&CBless: &W%-6d      &CRace : &W%-10.10s      &CPlayed: &W%d &Chours\n\r",
                 ch->pcdata->bless, capitalize(get_race(ch)), (get_age(ch) - 4)* 2);
    ch_printf(ch,"&CYears: &W%-6d      &CClass: &W%-11.11s     &CLog In: %s\r",
                 get_age(ch), capitalize(get_class(ch)), ctime(&(ch->logon)) );
    }
    if (IS_BIODROID(ch))
    {
	ch_printf(ch, "&CSTR  : &W%2.2d&C(&w%2.2d&C)      Biomass: &R%-4d&C          Saved:  %s\r",
		get_curr_str(ch), ch->perm_str, ch->biomass, ch->save_time ? ctime(&(ch->save_time)) : "no\n" );
    }
    else
    {
        ch_printf(ch, "&CSTR  : &W%2.2d&C(&w%2.2d&C)                             Saved:  %s\r",
                get_curr_str(ch), ch->perm_str, ch->save_time ? ctime(&(ch->save_time)) : "no\n" );
    }
        ch_printf(ch, "&CINT  : &W%2.2d&C(&w%2.2d&C)                             Time:   %s\r",
                get_curr_int(ch), ch->perm_int, ctime(&current_time) );

    if (get_curr_armor(ch) <= 1)
	sprintf(buf, "the rags of a beggar");
    else if (get_curr_armor(ch) <= 30)
	sprintf(buf, "improper for adventure");
    else if (get_curr_armor(ch) <= 75)
	sprintf(buf, "shabby and threadbare");
    else if (get_curr_armor(ch) <= 100)
	sprintf(buf, "of poor quality");
    else if (get_curr_armor(ch) <= 125)
	sprintf(buf, "scant protection");
    else if (get_curr_armor(ch) <= 150)
	sprintf(buf, "that of a knave");
    else if (get_curr_armor(ch) <= 175)
	sprintf(buf, "moderately crafted");
    else if (get_curr_armor(ch) <= 200)
	sprintf(buf, "well crafted");
    else if (get_curr_armor(ch) <= 225)
	sprintf(buf, "the envy of squires");
    else if (get_curr_armor(ch) <= 250)
	sprintf(buf, "excellently crafted");
    else if (get_curr_armor(ch) <= 275)
	sprintf(buf, "the envy of knights");
    else if (get_curr_armor(ch) <= 300)
	sprintf(buf, "the envy of barons");
    else if (get_curr_armor(ch) <= 325)
	sprintf(buf, "the envy of dukes");
    else if (get_curr_armor(ch) <= 350)
	sprintf(buf, "the envy of emperors");
    else
	sprintf(buf, "that of an avatar");
    if (ch->level > 49)
	ch_printf(ch, "&CWIS  : &W%2.2d&C(&w%2.2d&C)      Armor: &W%-d; %s\n\r",
		get_curr_wis(ch), ch->perm_wis, get_curr_armor(ch), buf);
    else
	ch_printf(ch, "&CWIS  : &W%2.2d&C(&w%2.2d&C)      Armor: &W%s \n\r",
		get_curr_wis(ch), ch->perm_wis, buf);

    if (ch->alignment > 900)
	sprintf(buf, "devout");
    else if (ch->alignment > 700)
	sprintf(buf, "noble");
    else if (ch->alignment > 350)
	sprintf(buf, "honorable");
    else if (ch->alignment > 100)
	sprintf(buf, "worthy");
    else if (ch->alignment > -100)
	sprintf(buf, "neutral");
    else if (ch->alignment > -350)
	sprintf(buf, "base");
    else if (ch->alignment > -700)
	sprintf(buf, "evil");
    else if (ch->alignment > -900)
	sprintf(buf, "ignoble");
    else
	sprintf(buf, "fiendish");
    if (ch->level < 10)
	ch_printf(ch, "&CDEX  : &W%2.2d&C(&w%2.2d&C)      Align: &W%-20.20s   &CItems:  &W%d &w(max %d)\n\r",
		get_curr_dex(ch), ch->perm_dex, buf, ch->carry_number, can_carry_n(ch));
    else
	ch_printf(ch, "&CDEX  : &W%2.2d&C(&w%2.2d&C)      Align: &W%4d; %-14.14s  &CItems:  &W%d &w(max %d)\n\r",
		get_curr_dex(ch), ch->perm_dex, ch->alignment, buf, ch->carry_number, can_carry_n(ch));

    ch_printf(ch, "&CCON  : &W%2.2d&C(&w%2.2d&C)      Pos'n: &W%-21.21s   &CWeight:  &W%d &w(max %d)\n\r",
	get_curr_con(ch), ch->perm_con, 
          ch->position == POS_DEAD ? "&Wdead" :
          ch->position == POS_MORTAL  ? "&Wmortal"  :
          ch->position == POS_INCAP  ? "&Wincapacitated"  :
          ch->position == POS_STUNNED  ? "&Wstunned"  :
          ch->position == POS_SLEEPING  ? "&Wsleeping"  :
          ch->position == POS_RESTING  ? "&Wresting"  :
          ch->position == POS_STANDING  ? "&Wstanding"  :
          ch->position == POS_FIGHTING  ? "&Wfighting"  :
          ch->position == POS_MOUNTED ? "&Wmounted"  :
          ch->position == POS_SITTING  ? "&Wsitting"  : "&Wstanding", ch->carry_weight, can_carry_w(ch));


    ch_printf(ch, "&CCHA  : &W%2.2d&C(&w%2.2d&C)      Wimpy: &Y%-5d",
	get_curr_cha(ch), ch->perm_cha, ch->wimpy);
    if(IS_MUTANT(ch))
    {
    ch_printf(ch, "\n\r&CLCK  : &W%2.2d&C(&w%2.2d&C)      &CPrimSkill:&W %-9s       &CSecSkill:&W %-10s\n\r",
        get_curr_lck(ch), ch->perm_lck, (xIS_SET(ch->act, PLR_LS))? "Large Size" : (xIS_SET(ch->act, PLR_SS))? "Small Size":
        (xIS_SET(ch->act, PLR_HK))? "High Ki" : (xIS_SET(ch->act, PLR_SK))? "Stone Skin" : (xIS_SET(ch->act, PLR_MB))? "MegaBrain" : "None",
        (xIS_SET(ch->act, PLR_AS))? "Augmented Strength" : (xIS_SET(ch->act, PLR_IS))? "Improved Speed" :
        (xIS_SET(ch->act, PLR_MS))? "Metallic Skin" : (xIS_SET(ch->act, PLR_EK))? "Extensive Knowledge":
        (xIS_SET(ch->act, PLR_KC))? "Ki Control" : "None" );
    }
    else if(IS_BIODROID(ch))
    {
        ch_printf(ch, "\n\r&CLCK  : &W%2.2d&C(&w%2.2d&C)      &CPrimAtt:&W %s\n\r",
                get_curr_lck(ch), ch->perm_lck, (xIS_SET(ch->act, PLR_TRIBEAM))? "Tribeam" :
                (xIS_SET(ch->act, PLR_MASENKO))? "Masenko" : (xIS_SET(ch->act, PLR_SCATTER))? "Scattershot" :
                (xIS_SET(ch->act, PLR_MT))? "Mental Throw" : "None" );
    }
    else
        ch_printf(ch, "\n\r&CLCK  : &W%2.2d&C(&w%2.2d&C)\n\r",
		get_curr_lck(ch), ch->perm_lck );

    ch_printf(ch, "                    &CLF  : &G%-5d &Cof &g%5d\n\r", ch->hit, ch->max_hit);

	ch_printf(ch, "&CStyle: &W%-10s      &CKi  : &B%-5d &Cof &b%5d    &CMKills:  &W%5d\n\r",           
          ch->style == STYLE_AGGRESSIVE ? "Agg" : "Def", ch->mana, ch->max_mana, ch->pcdata->mkills);

    ch_printf(ch, "&CGold : &Y%-13lld", ch->gold );   
    ch_printf(ch, "   &CPL  : &R%-11ld       &CMdeaths: &W%5d\n\r",ch->exp, ch->pcdata->mdeaths);
    if (ch->pcdata->release_date)
    ch_printf(ch, "&YYou will be released from hell at %24.24s.\n\r", ctime(&ch->pcdata->release_date));
    if (ch->pcdata->silenced_date)
    ch_printf(ch, "&YYou will be unsilenced %24.24s.\n\r", ctime(&ch->pcdata->silenced_date));

send_to_char("&W----------------------------------------------------------------------------\n\r", ch);


    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
	ch_printf(ch, "&CYou are bestowed with the command(s): &Y%s\n\r", 
		ch->pcdata->bestowments );

    if ( CAN_PKILL( ch ) )
    {
	ch_printf(ch, "&CPKILL DATA:  Pkills (&W%d&C)     Illegal Pkills (&W%d&C)     Pdeaths (&W%d&C)\n\r",
		ch->pcdata->pkills, ch->pcdata->illegal_pk, ch->pcdata->pdeaths );
    }
    if (ch->pcdata->clan )
    {

        ch_printf( ch, "&CCLAN STATS: &W%s", ch->pcdata->clan->name);
    }
 if (ch->level > 6) 
 { 
           send_to_pager("\n\r&W---------&CImmortal Data&W------------------------------------------------------\n\r", ch);
           pager_printf(ch, "&CEnter:&W %s %s\n\r", ch->name, (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "appears in a swirling mist.");
	   pager_printf(ch, "&CExit :&W %s %s\n\r", ch->name, (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "leaves in a swirling mist.");
        
          if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
                  ch_printf(ch, "&CYou are bestowed with the command(s): &W%s\n\r", ch->pcdata->bestowments );
   
}  
 send_to_pager("\n\r", ch);
    return;
}
*/

char *
get_class(CHAR_DATA *ch)
{
    if ( IS_NPC(ch) && ch->class < MAX_NPC_CLASS && ch->class >= 0)
    	return ( npc_class[ch->class] );
    else if ( !IS_NPC(ch) && ch->class < MAX_PC_CLASS && ch->class >= 0 )
        return class_table[ch->class]->who_name;
    return ("Unknown");
}


char *
get_race( CHAR_DATA *ch)
{
    if(  ch->race < MAX_PC_RACE  && ch->race >= 0)
        return (race_table[ch->race]->race_name);
    if ( ch->race < MAX_NPC_RACE && ch->race >= 0)
	return ( npc_race[ch->race] );
    return ("Unknown");
}

void do_oldscore( CHAR_DATA *ch, char *argument )
{

    set_char_color( AT_SCORE, ch );
    pager_printf( ch,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level,
	get_age(ch),
	(get_age(ch) - 17) * 2 );

    if ( get_trust( ch ) != ch->level )
	pager_printf( ch, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );

    if (  IS_NPC(ch) && xIS_SET(ch->act, ACT_MOBINVIS) )
      pager_printf( ch, "You are mobinvis at level %d.\n\r",
            ch->mobinvis);


    pager_printf( ch,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );

    pager_printf( ch,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch),
	get_curr_cha(ch),
	get_curr_lck(ch) );

    pager_printf(ch,
	     "You have scored %s exp, and have ",num_punct(ch->exp));
    pager_printf(ch,
	     "%s gold coins.\n\r", num_punct(ch->gold) );



    pager_printf( ch,
	"Autoexit: %s   Autoloot: %s   Autosac: %s   Autogold: %s\n\r",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no",
  	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOGOLD)) ? "yes" : "no" );

    pager_printf( ch, "Wimpy set to %d hit points.\n\r", ch->wimpy );

    if ( !IS_NPC(ch) ) {
       if ( ch->drunk   > 10 )
	   send_to_pager( "You are drunk.\n\r",   ch );
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_pager( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_pager( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_pager( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_pager( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_pager( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_pager( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_pager( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_pager( "You are fighting.\n\r",		ch );
	break;
    case POS_MOUNTED:
	send_to_pager( "Mounted.\n\r",			ch );
	break;
    case POS_SHOVE:
	send_to_pager( "Being shoved.\n\r",		ch );
	break;
    case POS_DRAG:
	send_to_pager( "Being dragged.\n\r",		ch );
	break;
    }

    if ( ch->level >= 25 )
	pager_printf( ch, "AC: %d.  ", get_curr_armor(ch) );

    send_to_pager( "You are ", ch );
	 if ( GET_AC(ch) >=  101 ) send_to_pager( "WORSE than naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_pager( "naked.\n\r",            ch );
    else if ( GET_AC(ch) >=   60 ) send_to_pager( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_pager( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_pager( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_pager( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >= - 20 ) send_to_pager( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= - 40 ) send_to_pager( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= - 60 ) send_to_pager( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= - 80 ) send_to_pager( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -100 ) send_to_pager( "divinely armored.\n\r", 
ch );
    else                           send_to_pager( "invincible!\n\r",       ch );

    if ( ch->level >= 15
    ||   IS_PKILL( ch ) )
	pager_printf( ch, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );

    if ( ch->level >= 10 )
	pager_printf( ch, "Alignment: %d.  ", ch->alignment );

    send_to_pager( "You are ", ch );
	 if ( ch->alignment >  900 ) send_to_pager( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_pager( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_pager( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_pager( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_pager( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_pager( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_pager( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_pager( "demonic.\n\r", ch );
    else                             send_to_pager( "satanic.\n\r", ch );


    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
	pager_printf( ch, "\n\rWizInvis level: %d   WizInvis is %s\n\r",
			ch->pcdata->wizinvis,
			xIS_SET(ch->act, PLR_WIZINVIS) ? "ON" : "OFF" );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  pager_printf( ch, "Room Range: %d - %d\n\r", ch->pcdata->r_range_lo,
					 	   ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  pager_printf( ch, "Obj Range : %d - %d\n\r", ch->pcdata->o_range_lo,
	  					   ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  pager_printf( ch, "Mob Range : %d - %d\n\r", ch->pcdata->m_range_lo,
	  					   ch->pcdata->m_range_hi	);
    }

    return;
}

/*								-Thoric
 * Display your current exp, level, and surrounding level exp requirements
 */
void do_level( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int x, lowlvl, hilvl;

    if ( ch->level == 1 )
      lowlvl = 1;
    else
      lowlvl = UMAX( 2, ch->level - 5 );
    hilvl = URANGE( ch->level, ch->level + 5, MAX_LEVEL );
    set_char_color( AT_SCORE, ch );
    ch_printf( ch, "\n\rExperience required, levels %d to %d:\n\r______________________________________________\n\r\n\r", lowlvl, hilvl );
    sprintf( buf, " exp  (Current: %12s)", num_punct(ch->exp) );
    sprintf( buf2," exp  (Needed:  %12s)", num_punct( exp_level(ch, ch->level+1) - ch->exp) );
    for ( x = lowlvl; x <= hilvl; x++ )
	ch_printf( ch, " (%2d) %12s%s\n\r", x, num_punct( exp_level( ch, x ) ),
		(x == ch->level) ? buf : (x == ch->level+1) ? buf2 : " exp" );
    send_to_char( "______________________________________________\n\r", ch );
}



void do_inventory( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "\n\rYou are carrying:\n\r&R", ch );
    send_to_char( "&z--------------------------------------&R\n\r", ch);
    show_list_to_char( ch->first_carrying, ch, TRUE, TRUE );
    send_to_char( "&z--------------------------------------&R\n\r", ch);
    return;
}


char *get_weapon_name( const char *str, sh_int number );

void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    set_char_color( AT_RED, ch );
    send_to_char( "\n\rYou are using:\n\r", ch );
    found = FALSE;
    send_to_char( "&z--------------------------------------&R\n\r", ch);
    set_char_color( AT_OBJECT, ch );
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	   if ( obj->wear_loc == iWear )
	   {
            if ( obj->item_type == ITEM_WEAPON && (obj->value[0] == W_DSWORD || obj->value[0] == W_DDAGGER) &&obj->wear_loc == WEAR_WIELD )
            {
               send_to_char( where_name[WEAR_WIELD_L], ch );
               send_to_char(  get_weapon_name( obj->short_descr, 2 ), ch );
               send_to_char( "\n\r", ch );
               send_to_char( where_name[WEAR_WIELD_R], ch );
               send_to_char( get_weapon_name( obj->short_descr, 1 ), ch );
               send_to_char( "\n\r", ch );
	       found = TRUE;
            }
            else
            {
                    send_to_char( where_name[iWear], ch );

		if ( can_see_obj( ch, obj ) )
		{
		    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
		    send_to_char( "\n\r", ch );
		}
		else
		    send_to_char( "something.\n\r", ch );
		found = TRUE;
            }
	   }
    }

    if ( !found )
	send_to_char( "&RNothing.\n\r", ch );
    send_to_char( "&z--------------------------------------&R\n\r", ch);

    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
	strcpy( buf, title );

    if ( ch->pcdata->title )
      STRFREE( ch->pcdata->title );
    ch->pcdata->title = STRALLOC( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    set_char_color( AT_SCORE, ch );
    if ( ch->exp < 45000000 )
    {
	send_to_char( "Sorry... you must have 45  million powerlevel or more to set your title...\n\r", ch );
	return;
    }
    if ( xIS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
	set_char_color( AT_IMMORT, ch );
        send_to_char( "The Gods prohibit you from changing your title.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}


/*void do_homepage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( ch->level < 5 )
    {
	send_to_char( "Sorry... you must be at least level 5 to do that.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->homepage )
	  ch->pcdata->homepage = str_dup( "" );
	ch_printf( ch, "Your homepage is: %s\n\r",
		show_tilde( ch->pcdata->homepage ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->homepage )
	  DISPOSE(ch->pcdata->homepage);
	ch->pcdata->homepage = str_dup("");
	send_to_char( "Homepage cleared.\n\r", ch );
	return;
    }

    if ( strstr( argument, "://" ) )
	strcpy( buf, argument );
    else
	sprintf( buf, "http://%s", argument );
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->homepage )
      DISPOSE(ch->pcdata->homepage);
    ch->pcdata->homepage = str_dup(buf);
    send_to_char( "Homepage set.\n\r", ch );
}*/



/*
 * Set your personal description				-Thoric
 */
void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Monsters are too dumb to do that!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_description: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_description: illegal substate", 0 );
	   return;

	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_DESC;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->description );
	   return;

	case SUB_PERSONAL_DESC:
	   STRFREE( ch->description );
	   ch->description = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}

/* Ripped off do_description for whois bio's -- Scryn*/
void do_bio( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs cannot set a bio.\n\r", ch );
	return;	  
    }
    if ( ch->level < 5 )
    {
	set_char_color( AT_SCORE, ch );
	send_to_char( "You must be at least level five to write your bio...\n\r", ch );
	return;
    }
    if ( !ch->desc )
    {
	bug( "do_bio: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_bio: illegal substate", 0 );
	   return;
	  	   
	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_BIO;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->pcdata->bio );
	   return;

	case SUB_PERSONAL_BIO:
	   STRFREE( ch->pcdata->bio );
	   ch->pcdata->bio = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}



/*
 * New stat and statreport command coded by Morphina
 * Bug fixes by Shaddai
 */

void do_statreport( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }
    if (!IS_NPC(ch))
    {
      ch_printf( ch, "You report: %d/%d Life %d/%d Ki  %d PL.\n\r",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 		ch->exp   );
      sprintf( buf, "$n reports: %d/%d Life %ld/%ld Ki %ld PL.",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 	        ch->exp   );
      act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
    }

    ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );
    sprintf( buf, "$n's base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );
    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    sprintf( buf, "$n's current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
    return;
}

void do_stat( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }

      ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 		ch->move, ch->max_move, ch->exp   );

      ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );

      ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    return;
}


void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) && ch->fighting )
	return;


      ch_printf( ch,
	"You report: %d/%d Life %d/%d Ki  %s Powerlevel.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	num_punct(get_curr_pl2(ch))   );

      sprintf( buf, "$n reports: %d/%d Life %ld/%ld Ki %s Powerlevel.",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	num_punct(get_curr_pl2(ch)));
    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_fprompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  set_char_color( AT_GREY, ch );

  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg || !str_cmp( arg, "display" ) )
  {
    send_to_char( "Your current fighting prompt string:\n\r", ch );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->fprompt, "" ) ? "(default prompt)"
				 				: ch->pcdata->fprompt );
    set_char_color( AT_GREY, ch );
    send_to_char( "Type 'help prompt' for information on changing your prompt.\n\r", ch );
    return;
  }
  send_to_char( "Replacing old prompt of:\n\r", ch );
  set_char_color( AT_WHITE, ch );
  ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->fprompt, "" ) ? "(default prompt)"
							      : ch->pcdata->fprompt );
  if (ch->pcdata->fprompt)
    STRFREE(ch->pcdata->fprompt);
  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->fprompt = STRALLOC("");
  else
    ch->pcdata->fprompt = STRALLOC(argument);
  return;
}

void do_prompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  set_char_color( AT_GREY, ch );

  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg || !str_cmp( arg, "display" ) )
  {
    send_to_char( "Your current prompt string:\n\r", ch );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)"
				 				: ch->pcdata->prompt );
    set_char_color( AT_GREY, ch );
    send_to_char( "Type 'help prompt' for information on changing your prompt.\n\r", ch );
    return;
  }
  send_to_char( "Replacing old prompt of:\n\r", ch );
  set_char_color( AT_WHITE, ch );
  ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)"
							      : ch->pcdata->prompt );
  if (ch->pcdata->prompt)
    STRFREE(ch->pcdata->prompt);
  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->prompt = STRALLOC("");
  else
    ch->pcdata->prompt = STRALLOC(argument);
  return;
}

void do_style( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];


    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	ch_printf(ch, "&wAdopt which fighting style?  (current:  %s&w)\n\r",
          ch->style == STYLE_AGGRESSIVE ? "&Raggressive" :
          ch->style == STYLE_DEFENSIVE  ? "&Ydefensive"  :
                                          "&Ydefensive"     );
	return;
    }
         else if( !str_prefix(arg, "defensive"))
         {
          act( AT_ACTION, "$n moves into a defensive posture.", ch, NULL, NULL, TO_ROOM );
          ch->style = STYLE_DEFENSIVE;
          send_to_char( "You adopt a defensive fighting style.\n\r",ch);
         return;
         }
         else if( !str_prefix(arg,"aggressive"))
         {
	  act( AT_ACTION, "$n assumes an aggressive stance.", ch, NULL, NULL, TO_ROOM );
          ch->style = STYLE_AGGRESSIVE;
          send_to_char( "You adopt an aggressive fighting style.\n\r", ch);
         return;
         }
          else 
          ch_printf(ch,"&wAdopt which fighting style?\n\r");
}

/*void do_finger( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;

  victim = get_char_world(ch, argument);

  buf[0] = '\0';
  if(IS_NPC(ch))
     return;


  if ( argument[0] == '\0' )
  {
      send_to_char("Finger whom?\n\r", ch );
      return;
  }
  if ( !IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
  {
    send_to_char("You can't finger an Immortal.\n\r", ch);
    return;
  }
    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        read_finger( ch, argument );
        return;
    }

  if ( !can_see( ch, victim, FALSE ) )
  {
      send_to_char("They aren't here.\n\r", ch );
      return;
  }

  if  ( IS_NPC( victim ) )
  {
      send_to_char("Not on NPC's.\n\r", ch );
      return;
  } 

  send_to_char("\n\r          &RF&zinger &RI&znfo&w\n\r", ch);
  send_to_char("          &W============\n\r", ch);
  ch_printf(ch, "&zName&w:  &W%s\n\r", victim->name);
  ch_printf(ch, "&zAge&w:   &W%d\n\r", get_age( victim ));
  ch_printf(ch, "&zLevel&w: &W%d\n\r", victim->level );
  ch_printf(ch, "&zSex&w:   &W%s\n\r",
                victim->sex == SEX_MALE   ? "Male"   :
                victim->sex == SEX_FEMALE ? "Female" : "Neutral" );
  ch_printf(ch, "&zClass&w: &W%s\n\r", capitalize(npc_class[victim->class]));
  ch_printf(ch, "&zRace&w:  &W%s\n\r", capitalize(npc_race[victim->race]) );
  ch_printf(ch, "&zTitle&w:&W%s\n\r", victim->pcdata->title );
  ch_printf(ch, "&zLast&w:  &W%s\n\r", (char *) ctime( &ch->logon ) );
  return;

}*/

