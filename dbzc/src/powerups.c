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

void add_mutate( CHAR_DATA *ch, int stage )
{
  switch( stage ) 
  {
   case 1:
         clear_mods( ch ); /* Clear all the mods */
         clear_mutateflags( ch ); /* Clear all Mutate Flags */
         xSET_BIT(ch->powerup, PU_MUTATE1);
         /* Primary Stats for Mutate 1 */
         if ( xIS_SET(ch->act, PLR_LS) )
         {
           ch->mod_str = 3;
           ch->mod_dex = -1;
         }
         else if ( xIS_SET(ch->act, PLR_SS) )
         {
           ch->mod_str = -1;
           ch->mod_dex = 3;
         }
         else if ( xIS_SET(ch->act, PLR_HK) )
           ch->mod_con = 3;
         else if ( xIS_SET(ch->act, PLR_MB) )
           ch->mod_wis = 3;

         /* Secondary Stats for Mutate 1 */
         if( xIS_SET(ch->act, PLR_EK))
           ch->mod_int += 1;
         else if( xIS_SET(ch->act, PLR_IS))
           ch->mod_dex += 1;
         else if( xIS_SET(ch->act, PLR_AS))
           ch->mod_str += 1;

     break;  
   case 2:
         clear_mods( ch ); /* Clear all the mods */
         clear_mutateflags( ch ); /* Clear all Mutate Flags */
         ch->mod = 2;
         xSET_BIT(ch->powerup, PU_MUTATE2);
         /* Primary Stats for Mutate 2 */
         if ( xIS_SET(ch->act, PLR_LS) )
         {
           ch->mod_str = 3;
           ch->mod_dex = 0;
         }
         else if ( xIS_SET(ch->act, PLR_SS) )
         {
           ch->mod_str = 0;
           ch->mod_dex = 3;
         }
         else if ( xIS_SET(ch->act, PLR_HK) )
         {
           ch->mod_con = 3;
           ch->mod_dex = 1;
         }
         else if ( xIS_SET(ch->act, PLR_MB) )
         {
           ch->mod_int = 1;
           ch->mod_wis = 3;
         }
         /* Secondary Stats for Mutate 2 */
         if( xIS_SET(ch->act, PLR_EK))
           ch->mod_int += 1;
         else if( xIS_SET(ch->act, PLR_IS))
           ch->mod_dex += 1;
         else if( xIS_SET(ch->act, PLR_AS))
           ch->mod_str += 1;

     break;
   case 3:
         clear_mods( ch ); /* Clear all the mods */
         clear_mutateflags( ch ); /* Clear all Mutate Flags */
         xSET_BIT(ch->powerup, PU_MUTATE3);
         ch->mod = 6;
         /* Primary Stats for Mutate 3 */
         if ( xIS_SET(ch->act, PLR_LS) )
         {
           ch->mod_str = 3;
           ch->mod_dex = -2;
         }
         else if ( xIS_SET(ch->act, PLR_SS) )
         {
           ch->mod_str = -2;
           ch->mod_dex = 3;
         }
         else if ( xIS_SET(ch->act, PLR_HK) )
         {
           ch->mod_con = 3;
           ch->mod_dex = 3;
         }
         else if ( xIS_SET(ch->act, PLR_MB) )
         {
           ch->mod_int = 3;
           ch->mod_wis = 3;
         }
         /* Secondary Stats for Mutate 3 */
         if( xIS_SET(ch->act, PLR_EK))
           ch->mod_int += 1;
         else if( xIS_SET(ch->act, PLR_IS))
           ch->mod_dex += 1;
         else if( xIS_SET(ch->act, PLR_AS))
           ch->mod_str += 1;

     break;
   case 4:
         clear_mods( ch ); /* Clear all the mods */
         clear_mutateflags( ch ); /* Clear all Mutate Flags */
         xSET_BIT(ch->powerup, PU_MUTATE4);
         ch->mod = 8.6;
         /* Primary Stats for Mutate 3 */
         if ( xIS_SET(ch->act, PLR_LS) )
         {
           ch->mod_str = 5;
           ch->mod_dex = -3;
         }
         else if ( xIS_SET(ch->act, PLR_SS) )
         {
           ch->mod_str = -3;
           ch->mod_dex = 5;
         }
         else if ( xIS_SET(ch->act, PLR_HK) )
         {
           ch->mod_str = 1;
           ch->mod_con = 3;
           ch->mod_dex = 1;
         }
         else if ( xIS_SET(ch->act, PLR_MB) )
         {
           ch->mod_str = 1;
           ch->mod_int = 1;
           ch->mod_wis = 3;
         }
         /* Secondary Stats for Mutate 3 */
         if( xIS_SET(ch->act, PLR_EK))
           ch->mod_int += 1;
         else if( xIS_SET(ch->act, PLR_IS))
           ch->mod_dex += 1;
         else if( xIS_SET(ch->act, PLR_AS))
           ch->mod_str += 1;

     break;
   case 5:
         clear_mods( ch );
         clear_mutateflags( ch );
         add_mutate( ch, 1 );
         ch->mod = 9.5;
         ch->kimod = 1.2;
         xREMOVE_BIT(ch->powerup, PU_MUTATE1);
         xSET_BIT(ch->powerup, PU_MONSTER);
     break;
   }        
   return;   
}

void do_ultranamek( CHAR_DATA *ch, char *argument )
{
     if (!IS_NAMEK(ch))
     {
        send_to_char("Huh?\n\r", ch);
        return;
     }
     if ( IS_NPC(ch) || is_fused(ch) || check_spec(ch))
     return;
     if (IS_POWERUP(ch) || xIS_SET(ch->powerup, PU_SNAMEK))
     {
        send_to_char("Your already using another technique...\n\r", ch);
        return;
     }
     if (ch->exp < 70000000)
     {
        send_to_char("You're no Ultra Namek...\n\r", ch);
        return;
     }
     if (xIS_SET(ch->powerup, PU_SUPPRESS))
     do_suppress( ch, "0");
     clear_mods( ch );
     if (xIS_SET(ch->powerup, PU_ULTRA))
     {
      xREMOVE_BIT(ch->powerup, PU_ULTRA);
      send_to_char( "\n\r&GYou close your eyes and you take a deep breath, as your muscles begin to dissappear.\n\r&gYou open your eyes as you return to your normal self.\n\r", ch);
      act(AT_GREY,"&W$n closes $s eyes and takes a deep breath, as $s muscles begin to dissappear.\n\r&g$n open $s eyes as $s body and power returns to normal.", ch, NULL, NULL, TO_CANSEE);
     }
     else
     {
      xSET_BIT(ch->powerup, PU_ULTRA);
      send_to_char( "\n\r&wYou close your eyes as you concentrate your power within you.\n\r&GYour muscles begin to grow to a higher size as your power increases.\n\r&gYou have turned now into an ULTRA NAMEK!!!\n\r", ch );
      act(AT_GREY,"&w$n closes $s eyes as $e concentrates $s power within $m.\n\r&G$n's muscles begin to grow to a higher size as $s power increases.\n\r&g$n has turned into an ULTRA NAMEK!!!\n\r", ch, NULL, NULL, TO_CANSEE);
      ch->mod = 9.55;
      ch->kimod = 1.2;
     }
     return;
}

void do_snamek( CHAR_DATA *ch, char *argument )
{
     if (!IS_NAMEK(ch))
     {
      send_to_char("Huh?\n\r", ch);
      return;
     }
     if (IS_NPC(ch) || is_fused(ch) || check_spec(ch))
     return;
     if (IS_POWERUP(ch) || xIS_SET(ch->powerup, PU_ULTRA))
     {
      send_to_char("Your already using another technique...\n\r", ch);
      return;
     }
     if (ch->exp < 3500000)
     {
      send_to_char("Your no Super Namek...\n\r", ch);
      return;
     }
     if (xIS_SET(ch->powerup, PU_SUPPRESS))
     do_suppress( ch, "0");
     clear_mods( ch );
     if (xIS_SET(ch->powerup, PU_SNAMEK))
     {
      xREMOVE_BIT(ch->powerup, PU_SNAMEK);
      send_to_char( "\n\rYou concentrate and your body and muscles begin to dissappear...\n\rYour enourmous power fades as you return to your normal form.\n\r", ch );
      act(AT_GREY,"$n concentrates and $s body and muscles begin to dissappear...\n\r$s enourmous power fades as $e return to $s normal form.", ch, NULL, NULL, TO_CANSEE);
     }
     else
     {
      xSET_BIT(ch->powerup, PU_SNAMEK);
      send_to_char( "\n\rYou scream as your muscles and your body grow...\n\rYou raidate with extreme power as you become a Super Namek!!!\n\r", ch );
      act(AT_GREY,"$n screams as $s muscles and $s body grows...\n\r$e raidate with extreme power as $e becomes a Super Namek!!!", ch, NULL, NULL, TO_CANSEE);
      ch->mod = 8.9;
      ch->mod_str += -1;
      ch->mod_int += 3;
      ch->mod_dex += 3;     
     }
     return;
}

void do_superhuman( CHAR_DATA *ch, char *argument )
{
     if (!IS_HUMAN(ch))
     {
        send_to_char("Huh?\n\r", ch);
        return;
     }
     if (is_fused(ch) || IS_NPC(ch) || check_spec(ch))
     return;
     if (IS_POWERUP(ch))
     {
        send_to_char("Your already using another technique...\n\r", ch);
        return;
     }
     if (ch->exp < 70000000)
     {
      send_to_char("Your no Super Human...\n\r", ch);
      return;
     }
     if (xIS_SET(ch->powerup, PU_SUPPRESS))
     do_suppress( ch, "0");
     clear_mods( ch );
     if (xIS_SET(ch->powerup, PU_SUPERHUMAN))
     {
      xREMOVE_BIT(ch->powerup, PU_SUPERHUMAN);
      send_to_char( "\n\r&WYou close your eyes as your muscles begin to dissappear...\n\r&zAs you open your eyes once again, your power has diminished to what it was before.\n\r", ch);
      act(AT_GREY,"&W$n closes $s eyes as $s muscles begin to dissappear...\n\r&zAs $n opens $s eyes once again, $s power has diminished to what it was before.", ch, NULL, NULL, TO_CANSEE);
     }
     else
     {
      xSET_BIT(ch->powerup, PU_SUPERHUMAN);
      send_to_char( "\n\r&wYou close your eyes as you search within you your hidden power.\n\r&WYour muscles begin to grow in size and your power raises as you feel your power being tapped.\n\r&zCrackles of energy explode from your body as you unlock your true potential!\n\r", ch );
      act(AT_GREY,"$n closes $s eyes as $e searches within $m for $s hidden power.\n\r&W$n's muscles begin to grow in size and power as you notice that $n is somewhat tapping $s power.\n\r&zCrackles of energy explode from $n's body as $n unlocks $s true potential!", ch, NULL, NULL, TO_CANSEE);
      ch->mod = 9.6;
      ch->kimod = 1.2;
     }
     return;
}



void do_mystic( CHAR_DATA *ch, char *argument )
{
   if ( IS_NPC(ch) || is_fused(ch) || check_spec(ch))
   return;
   if ( xIS_SET(ch->powerup, PU_OOZARU))
   {
      send_to_char("You cannot control the power of Oozaru in Mystic.\n\r", ch);
      return;
   }
   if (IS_POWERUP(ch)
   || xIS_SET(ch->powerup, PU_SSJ1)
   || xIS_SET(ch->powerup, PU_SSJ2)
   || xIS_SET(ch->powerup, PU_SSJ3)
   || xIS_SET(ch->powerup, PU_KAIOKEN)
   || xIS_SET(ch->powerup, PU_USSJ))
   {
      send_to_char("Your already using another technique...\n\r", ch);
      return;
   }
   if (!xIS_SET(ch->powerup, PU_MYSTIC))
   {
     if (can_use_skill2(ch, number_percent(), gsn_mystic ))
     {

       xSET_BIT(ch->powerup, PU_MYSTIC);
       clear_mods( ch );

        if ( !str_cmp( argument, "ssj" ) && ch->exp > 70000000)
        {
           ch->mod = 9.7;
           ch->kimod = 1.2;
           send_to_char( "\n\r&YYou scream as a blast of energy explodes away from you.\n\r&RYou concentrate trying to search all the power within your soul.\n\rYour hair turns yellow as your eyes turn green, a yellow aura soon surrounds you completely.\n\r&WYou have unleashed the power of Mystic SSJ.\n\r", ch);
           act( AT_RED, "&Y$n screams as a blast of energy explodes away from $m.\n\r&R$n concentrates trying to search all the power within $s soul.\n\r&W$n's hair turns yellow as $s eyes turns green and a yellow aura soon surrounds $m completely.\n\r&z$n has now unleashed the power of a Mystic SSJ.\n\r", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
           ch->mod = 8.9;
           ch->kimod = 1;
           ch->mod_str = -1;
           ch->mod_dex = 3;
           send_to_char( "\n\r&RYou close your eyes as you concentrate on harvesting the energy inside of you.\n\r&WYou open your eyes as white flames surrounds you comepletely.\n\r&YYou have now transformed into a Mystic Fighter.\n\r", ch);
           act( AT_RED, "$n closes $s eyes as $e concentrates on harvesting the energy inside of $m.\n\r&W$n opens $s eyes as white flames explodes and surrounds $s body completely.", ch, NULL, NULL, TO_ROOM );
        }
        learn_from_success2( ch, gsn_mystic );             
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0");
 
    }
    else
    {
       send_to_char( "\n\r&RYou close your eyes as you concentrate on harvesting the energy inside of you.\n\r&WYou open your eyes but you are unable to unleash the power...\n\r", ch);
       act( AT_RED, "$n closes $s eyes as $e concentrates on harvesting the energy inside of $m.\n\r&W$n opens $s eyes as $e is unable to unleash the power...", ch, NULL, NULL, TO_ROOM );
       learn_from_failure2( ch, gsn_mystic );
    }
   }
   else if (xIS_SET(ch->powerup, PU_MYSTIC ))
   {
      xREMOVE_BIT(ch->powerup, PU_MYSTIC);
      send_to_char( "\n\r&WYou concentrate as the white flames begin to fade away and they dissappear...\n\r", ch );
      act(AT_GREY,"$n concentrates as the white flames begin to fade away and they dissappear...", ch, NULL, NULL, TO_CANSEE);
      clear_mods( ch );
      return;
   }
     
}

void do_oozaru( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    argument = one_argument( argument, arg1 );

     if ( !str_cmp( arg1, "golden" ) )
     {
        if (xIS_SET(ch->powerup, PU_GOLDEN))
        {
          send_to_char("You are already in this form!\n\r",ch);
          return;
        }
        if (xIS_SET(ch->powerup, PU_OOZARU))
        {
          if (ch->exp < 95000000)
          return;
          if (ch->rage < 1000)
          return;
          send_to_char( "\n\r&YYou growl as you begin to control the awesome power of the Oozaru!!!\n\rYour fur turns golden as your power rages through out your\n\rgiant body, as you turn into a GOLDEN OOZARU!!!\n\r", ch );
          act(AT_YELLOW,"\n\r$n growls as $e begins to control the awesome power of the Oozaru!!!\n\r$n's fur turns golden as $s power rages through out $s\n\rgiant body, as $n turns into a GOLDEN OOZARU!!!\n\r", ch, NULL, NULL, TO_CANSEE);
          clear_mods( ch );
          ch->mod = 9.8;
          ch->mod_str = 4;
          ch->mod_dex = -1;
          ch->mod_int = -1;
          ch->mod_lck = 2;
          xSET_BIT(ch->powerup, PU_GOLDEN);
          if (xIS_SET(ch->powerup, PU_SUPPRESS))
          do_suppress( ch, "0");
          ch->hit = ch->max_hit;
          ch->mana = ch->max_mana;
          xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
          xREMOVE_BIT(ch->powerup, PU_SSJ1);
          xREMOVE_BIT(ch->powerup, PU_SSJ2);
          xREMOVE_BIT(ch->powerup, PU_SSJ3);
          xREMOVE_BIT(ch->powerup, PU_MYSTIC);
          xREMOVE_BIT(ch->powerup, PU_USSJ);
        }                          
     }
     else
     {
          send_to_char( "Huh?\n\r", ch);
          return;
     }
}

void do_mutate( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];  
   char              buf[MAX_INPUT_LENGTH];  

   argument = one_argument( argument, arg1 );

     if ( IS_NPC(ch) )
     return;
     if (!IS_MUTANT(ch))
     {
      send_to_char("Huh?\n\r", ch);
      return;
     }
     if (is_fused(ch))
     return;
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
       send_to_char("&RSpectators cant do that.\n\r", ch);
       return;
     } 
     if (IS_POWERUP(ch))
     {
      send_to_char("Your already using another technique...\n\r", ch);
      return;
     }
     if ( arg1[0] == '\0' )
     {
      send_to_char( "Mutate into which form?\n\r", ch );
      return;
     }
     if ( !str_cmp( arg1, "1" ) )
     {
       if (xIS_SET(ch->powerup, PU_MUTATE1))
       {
        ch_printf(ch, "&YYour already at that stage.\n\r");
        return;
       }
       ch->height_mod = 0;
       ch->weight_mod = 0;
       send_to_char("\n&YYou shrink and mutate to your normal form.\nYour muscles and your body has returned to the way they were.\n\r", ch);
       sprintf( buf , "%s shrinks and mutates to %s normal form\n\r%s muscles and %s body have returned to the way they were." , ch->name,
       ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its", ch->sex == 1 ? "His" : ch->sex == 2 ? "Her" : "Its", ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its" );
       act( AT_YELLOW, buf , ch, NULL, NULL, TO_CANSEE );
       add_mutate( ch, 1 );
       return;
      } 
      else if ( !str_cmp( arg1, "2" ) )
      {
       if (xIS_SET(ch->powerup, PU_MUTATE2))
       {
         send_to_char("&YYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 1499999)
       {
         send_to_char("&YYou are too young to mutate into that stage.\n\r", ch);
         return;
       }
       ch->height_mod = 12;
       ch->weight_mod = 31;
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       send_to_char("\n\r&YYou growl as you unleash all of your power and you mutate into your second form!\n\rYou scream in pain as your body undergoes changes. You grow in size as your mutation finishes.\n\r", ch);
       act(AT_YELLOW,"$n growls as $e unleashes all of $s power and $e mutates into $s second form!\n\r$n screams in pain as $s body undergoes changes.", ch, NULL, NULL, TO_CANSEE);
       add_mutate( ch, 2 );
      }
      else if ( !str_cmp( arg1, "3" ) )
      {
       if (xIS_SET(ch->powerup, PU_MUTATE3))
       {
         send_to_char("&YYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 3999999)
       {
         send_to_char("&YYou are too young to mutate into that stage.\n\r", ch);
         return;
       }
       send_to_char("\n\r&YYou grow in size as your body undergoes its mutation into its third form.\n\rYou feel all the power that now runs through your veins as you.\n\r", ch);
       act(AT_YELLOW,"$n grows in size as $s body undergoes its mutation into its third form.\n\r$n stands there flexing $s new built muscles.", ch, NULL, NULL, TO_CANSEE);
       ch->height_mod = 12;
       ch->weight_mod = 31;
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       add_mutate( ch, 3 );
      }
/*Mutate 4*/
      else if ( !str_cmp( arg1, "4" ) )
      {
       if (xIS_SET(ch->powerup, PU_MUTATE4))
       {
         send_to_char("&YYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 14999999)
       {
         send_to_char("&YYou are too young to mutate into that stage.\n\r", ch);
         return;
       }
       ch->height_mod = 12;
       ch->weight_mod = 31;
       send_to_char("\n\r&YYou close your eyes as you try to harvest the ultimate power within you.\n\rYou begin to grow in size and that power now comes alive and begins to be unleashed.\n\rYou open your eyes which are now turned white as you feel amazingly new power that you have achieve.\n\r",ch);
       act(AT_YELLOW,"$n closes $s eyes as $e tries to harvest the ultimate power within $m\n\r$n begins to grow in size and that now comes alive and begins to be unleashed.\n\r$n opens $s eyes which are now turned into white, and you can feel the amazingly new power that $n has achieved.", ch, NULL, NULL, TO_CANSEE);
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       add_mutate( ch, 4 );
     }
      else if ( !str_cmp( arg1, "monster" ) )
      {
       if (xIS_SET(ch->powerup, PU_MONSTER))
       {
         send_to_char("&YYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp < 70000000)
       {
         send_to_char("&YYou are too young to mutate into that stage.\n\r", ch);
         return;
       }
       ch->height_mod = 20;
       ch->weight_mod = 60;
       send_to_char("\n\r&wYou feel the muscles grow to a point that your body cannot contain.\n\r&WYou scream as your back emits a gut wrenching crack, and a fiery pain shoots throughout your body.\n\r&zSlowly you calm down, as your piercing monstrous &rred&z eyes glare around.\n\r",ch);
       act(AT_WHITE,"&w$n growls as the muscles on $s back grow to a point that $s body cannot handle it.\n\r&W$n screams as $s back emits a gut wrenching crack, and a fiery pain shoots throughout $s body.\n\r&wSlowly $n calms down, as $s pierciend monstrous &rred&z eyes glares around.", ch, NULL, NULL, TO_CANSEE);
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       add_mutate( ch, 5 );
      }

}


void do_powerup( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];  
   char              buf[MAX_INPUT_LENGTH];  

   argument = one_argument( argument, arg1 );
     if ( IS_NPC(ch) )
     return;
     if (is_fused(ch))
     return;    
    if (xIS_SET(ch->act, PLR_SPEC) )
     {
     send_to_char("&RSpectators cant do that.\n\r", ch);
     return;
     }
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

    if (ch->mod >= 2 && !xIS_SET(ch->powerup, PU_TURBO))
    {
      send_to_char("\n\rYou are already using another powerup technique.", ch);
      return;
    }
    if ( arg1[0] == '\0' )
    {
      if (xIS_SET(ch->powerup, PU_POWER7))
      {
      send_to_char("Your already powered up to your maximum.", ch);
      return;
      }       
      if (xIS_SET(ch->powerup, PU_SUPPRESS))
      do_suppress( ch, "0");
      else if (xIS_SET(ch->powerup, PU_POWER6) && (ch->exp >= 2000000)  && !xIS_SET(ch->powerup, PU_POWER7))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER6);
      xSET_BIT(ch->powerup, PU_POWER7);
      ch->mod = 1.7;
      send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
      sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
      ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      return;      
      }      
      else if (xIS_SET(ch->powerup, PU_POWER5) && (ch->exp >= 1000000) && !xIS_SET(ch->powerup, PU_POWER6))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER5);
      xSET_BIT(ch->powerup, PU_POWER6);
      ch->mod = 1.6;
      send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles,\n\rthen it explodes as you powerup for the sixth time!\n\r", ch );
      sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
      ch->name, ch->name);
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      return;      
      }
      else if (xIS_SET(ch->powerup, PU_POWER4) && (ch->exp >= 750000) && !xIS_SET(ch->powerup, PU_POWER5))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER4);
      xSET_BIT(ch->powerup, PU_POWER5);
      ch->mod = 1.5;
      send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
      sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
      ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      return;      
      }
      else if (xIS_SET(ch->powerup, PU_POWER3) && (ch->exp >= 500000) && !xIS_SET(ch->powerup, PU_POWER4))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER3);
      xSET_BIT(ch->powerup, PU_POWER4);
      ch->mod = 1.4;
      send_to_char("\n\r&BThe ground begins to tremble as you powerup for the fourth time.\n\r", ch );
      sprintf( buf , "The ground begins to tremble as %s powersup for the fourth time.\n\r",
      ch->name);
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      return;      
      }
      else if (xIS_SET(ch->powerup, PU_POWER2) && (ch->exp >= 250000) && !xIS_SET(ch->powerup, PU_POWER3))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER2);
      xSET_BIT(ch->powerup, PU_POWER3);
      ch->mod = 1.3;
      send_to_char("\n\r&BYour clothing now begins to flutter as you push your powerlevel into the next level.\n\r", ch );
      sprintf( buf , "%s's clothing now begins to flutter as %s pushes %s powerlevel into the next level.\n\r",
      ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      return;      
      }
      else if (xIS_SET(ch->powerup, PU_POWER1) && (ch->exp >= 50000) && !xIS_SET(ch->powerup, PU_POWER2))
      {
      xREMOVE_BIT(ch->powerup, PU_POWER1);
      xSET_BIT(ch->powerup, PU_POWER2);
      ch->mod = 1.2;
      send_to_char("\n\r&BA few rocks begin to hover above the ground as you powerup for the second time.\n\r", ch );
      sprintf( buf , "A few rocks begin to hover above the ground as %s powers up for the second time.\n\r",
      ch->name);
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
      }
      else if (NO_POWERUP(ch) && !xIS_SET(ch->powerup, PU_POWER1) && !xIS_SET(ch->powerup, PU_POWER2) && !xIS_SET(ch->powerup, PU_POWER3) && !xIS_SET(ch->powerup, PU_POWER4) && !xIS_SET(ch->powerup, PU_POWER5) && !xIS_SET(ch->powerup, PU_POWER6) && !xIS_SET(ch->powerup, PU_POWER7))
      {
      xSET_BIT(ch->powerup, PU_POWER1);
      ch->mod = 1.1;
      send_to_char("\n\r&BYou begin to be surrounded by a white aura as you powerup for the first time.\n\r", ch );
      sprintf( buf , "%s begins to be surrounded by a white aura as %s powers up for the first time.\n\r",
      ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it");
      act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );  
      return;
      }
     }
     if ( !str_cmp( arg1, "max" ) )
     {
     
      if (xIS_SET(ch->powerup, PU_POWER7))
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER6) && ch->exp < 2000000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER5) && ch->exp < 1000000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER4) && ch->exp < 750000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER3) && ch->exp < 500000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER2) && ch->exp < 250000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_POWER1) && ch->exp < 50000 )
      {
       send_to_char("Your already poweredup to your maximum.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_SUPPRESS))
      do_suppress( ch, "0");
      if ((IS_HUMAN(ch)) && (ch->exp >= 2500000) && (!xIS_SET(ch->powerup, PU_TURBO)) )
      {
       send_to_char("\n\r&WYou are engulfed by a white aura as you powerup.", ch);
       act( AT_WHITE, "\n\r$n is engulfed by a white aura as $e powers up.", ch, NULL, NULL, TO_CANSEE);
       xSET_BIT(ch->powerup, PU_TURBO);
       ch->mod      = 8.5;
       ch->mod_str -= 1;
       ch->mod_dex += 3;
       ch->mod_int += 1;
      }
      if (NO_POWERUP(ch))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles,\n\rthen it explodes as you powerup for the sixth time!\n\r", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 750000)
        {
         xSET_BIT(ch->powerup, PU_POWER5);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.5;}
         send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
         sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 500000)
        {
         xSET_BIT(ch->powerup, PU_POWER4);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.4;}
         send_to_char("\n\r&BThe ground begins to tremble as you powerup for the fourth time.\n\r", ch );
         sprintf( buf , "The ground begins to tremble as %s powersup for the fourth time.\n\r",
         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 250000)
        {
         xSET_BIT(ch->powerup, PU_POWER3);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.3;}
         send_to_char("\n\r&BYour clothing now begins to flutter as you push your powerlevel into the next level.\n\r", ch );
         sprintf( buf , "%s's clothing now begins to flutter as %s pushes %s powerlevel into the next level.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "hers" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 50000)
        {
         xSET_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.2;}
         send_to_char("\n\r&BA few rocks begin to hover above the ground as you powerup for the second time.\n\r", ch );
         sprintf( buf , "A few rocks begin to hover above the ground as %s powers up for the second time.\n\r",
         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }       
        if (ch->exp >= 500 )
        {
         xSET_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.1;}
         send_to_char("\n\r&BYou begin to be surrounded by a white aura as you powerup for the first time.\n\r", ch );
         sprintf( buf , "%s begins to be surrounded by a white aura as %s powers up for the first time.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }       
        return;
      }
      if (xIS_SET(ch->powerup, PU_POWER1))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles,\n\rthen it explodes as you powerup for the sixth time!", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 750000)
        {
         xSET_BIT(ch->powerup, PU_POWER5);
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
         sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.5;}
         return;
        }
        if (ch->exp >= 500000)
        {
         xSET_BIT(ch->powerup, PU_POWER4);
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.4;}
         send_to_char("\n\r&BThe ground begins to tremble as you powerup for the fourth time.\n\r", ch );
         sprintf( buf , "The ground begins to tremble as %s powersup for the fourth time.\n\r",
         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 250000)
        {
         xSET_BIT(ch->powerup, PU_POWER3);
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.3;}
         send_to_char("\n\r&BYour clothing now begins to flutter as you push your powerlevel into the next level.\n\r", ch );
         sprintf( buf , "%s's clothing now begins to flutter as %s pushes %s powerlevel into the next level.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", ch->sex == 1 ? "his" : ch->sex == 2 ? "hers" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 50000)
        {
         xSET_BIT(ch->powerup, PU_POWER2);
         xREMOVE_BIT(ch->powerup, PU_POWER1);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.2;}
         send_to_char("\n\r&BA few rocks begin to hover above the ground as you powerup for the second time.\n\r", ch );
         sprintf( buf , "A few rocks begin to hover above the ground as %s powers up for the second time.\n\r",
         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }       
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER2))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         xREMOVE_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles,\n\rthen it explodes as you powerup for the sixth time!\n\r", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 750000)
        {
         xSET_BIT(ch->powerup, PU_POWER5);
         xREMOVE_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.5;}
         send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
         sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 500000)
        {
         xSET_BIT(ch->powerup, PU_POWER4);
         xREMOVE_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.4;}
         send_to_char("\n\r&BThe ground begins to tremble as you powerup for the fourth time.\n\r", ch );
         sprintf( buf , "The ground begins to tremble as %s powersup for the fourth time.\n\r",
         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 250000)
        {
         xSET_BIT(ch->powerup, PU_POWER3);
         xREMOVE_BIT(ch->powerup, PU_POWER2);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.3;}
         send_to_char("\n\r&BYour clothing now begins to flutter as you push your powerlevel into the next level.\n\r", ch );
         sprintf( buf , "%s's clothing now begins to flutter as %s pushes %s powerlevel into the next level.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "hers" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER3))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER3);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         xREMOVE_BIT(ch->powerup, PU_POWER3);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles,\n\rthen it explodes as you powerup for the sixth time!", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 750000)
        {
         xSET_BIT(ch->powerup, PU_POWER5);
         xREMOVE_BIT(ch->powerup, PU_POWER3);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.5;}
         send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
         sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 500000)
        {
         xSET_BIT(ch->powerup, PU_POWER4);
         xREMOVE_BIT(ch->powerup, PU_POWER3);
         send_to_char("\n\r&BThe ground begins to tremble as you powerup for the fourth time.\n\r", ch );
         sprintf( buf , "The ground begins to tremble as %s powersup for the fourth time.\n\r",         ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.4;}
         return;
        }
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER4))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER4);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         xREMOVE_BIT(ch->powerup, PU_POWER4);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles, then it explodes as you powerup for the sixth time!\n\r", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        if (ch->exp >= 750000)
        {
         xSET_BIT(ch->powerup, PU_POWER5);
         xREMOVE_BIT(ch->powerup, PU_POWER4);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.5;}
         send_to_char("\n\r&BYour aura now flashes brightly as you continue to raise your powerlevel further.\n\r", ch );
         sprintf( buf , "%s's aura now flashes brightly as %s continues to raise %s powerlevel further.\n\r",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER5))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER5);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         return;
        }
        if (ch->exp >= 1000000)
        {
         xSET_BIT(ch->powerup, PU_POWER6);
         xREMOVE_BIT(ch->powerup, PU_POWER5);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.6;}
         send_to_char("\n\r&BDust around you begins to accumulate surrounding you in circles, then it explodes as you powerup for the sixth time!\n\r", ch );
         sprintf( buf , "The dust around you begins to accumulate surrounding %s in circles, then it explodes as %s powerups for the sixth time!",
         ch->name, ch->name);
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER6))
      {
        if (ch->exp >= 2000000)
        {
         xSET_BIT(ch->powerup, PU_POWER7);         
         xREMOVE_BIT(ch->powerup, PU_POWER6);
         if (!xIS_SET(ch->powerup, PU_TURBO)){
         ch->mod = 1.7;}
         send_to_char("\n\r&BYour aura now grows in size as you begin to hover above the ground slightly.\n\rYou scream as you let loose all of your power that was still inside!\n\r", ch );
         sprintf( buf , "%s's aura now grows in size as %s begins to hover above the ground slightly.\n\r%s screams as %s lets loose all of %s power that was still inside!",
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it", 
         ch->name, ch->sex == 1 ? "he" : ch->sex == 2 ? "she" : "it",
         ch->sex == 1 ? "his" : ch->sex == 2 ? "her" : "its");
         act( AT_BLUE, buf , ch, NULL, NULL, TO_CANSEE );
         return;
        }
        return;
      } 
      if (xIS_SET(ch->powerup, PU_POWER7))
      {
       send_to_char("\n\rYour already at maximum.\n\r", ch);
      } 
  }
}

void do_powerdown( CHAR_DATA *ch, char *argument )
{
   if (xIS_SET(ch->act, PLR_SPEC) )
     {
     send_to_char("&RSpectators cant do that.\n\r", ch);
     return;
     }
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
   if (!xIS_SET(ch->powerup, PU_POWER1) && !xIS_SET(ch->powerup, PU_POWER2) && !xIS_SET(ch->powerup, PU_POWER3) && !xIS_SET(ch->powerup, PU_POWER4) && !xIS_SET(ch->powerup, PU_POWER5) && !xIS_SET(ch->powerup, PU_POWER6) && !xIS_SET(ch->powerup, PU_POWER7))
   {
    send_to_char( "\n\rYou are not powered up.\n\r", ch);
    return;
   }
    send_to_char( "\n\r&BYou take a deep breath as you release all your stored energy.\n\r", ch);
    act( AT_BLUE, "$n takes a deep breath as $e releases all his stored energy.", ch, NULL, NULL, TO_ROOM );
    xREMOVE_BIT(ch->powerup, PU_POWER1);
    xREMOVE_BIT(ch->powerup, PU_POWER2);     
    xREMOVE_BIT(ch->powerup, PU_POWER3);
    xREMOVE_BIT(ch->powerup, PU_POWER4);
    xREMOVE_BIT(ch->powerup, PU_POWER5);
    xREMOVE_BIT(ch->powerup, PU_POWER6);
    xREMOVE_BIT(ch->powerup, PU_POWER7);
    ch->mod = 1;
    ch->kimod = 1;

   if (xIS_SET(ch->powerup, PU_TURBO))
   {
    send_to_char( "&WThe white aura which engulfed you now fades away.\n\r", ch);
    act( AT_WHITE, "The white aura that surrounded $n  now fades away.\n\r", ch, NULL, NULL, TO_CANSEE);    
    ch->mod_str = 0;
    ch->mod_int = 0;
    ch->mod_lck = 0;
    ch->mod_dex = 0;
    xREMOVE_BIT(ch->powerup, PU_TURBO);
   }
}

void clear_ssjflags( CHAR_DATA *ch )
{
      xREMOVE_BIT(ch->powerup, PU_SSJ1); xREMOVE_BIT(ch->powerup, PU_SSJ2); xREMOVE_BIT(ch->powerup, PU_GOLDEN);
      xREMOVE_BIT(ch->powerup, PU_SSJ3); xREMOVE_BIT(ch->powerup, PU_SSJ4); xREMOVE_BIT(ch->powerup, PU_OOZARU);
      ch->mod = ch->kimod = 1;
      ch->mod_str = ch->mod_dex = ch->mod_lck = ch->mod_wis = ch->mod_int = ch->mod_con = 0;
}
void do_ssj( CHAR_DATA *ch, char *argument )
{
     char              arg1[MAX_INPUT_LENGTH];    
     argument = one_argument( argument, arg1 );

     if ( IS_NPC(ch) )
     return;
     if (is_fused(ch))
     return;
     if (!IS_MONKEY(ch))
     {
      send_to_char("You're a very confused individual...\n\r", ch);
      return;
     }
    
     if ( xIS_SET(ch->powerup, PU_OOZARU) && !xIS_SET(ch->powerup, PU_GOLDEN) && (!str_cmp( arg1, "1" ) || !str_cmp( arg1, "2" ) || !str_cmp( arg1, "3" )  || arg1[0] == '\0' ))
     {
      send_to_char("You cannot control the power of Oozaru in SSJ.\n\r", ch);
      return;
     }
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
     send_to_char("&RSpectators cant do that.\n\r", ch);
     return;
     }
     if ( xIS_SET(ch->powerup, PU_KAIOKEN ) || xIS_SET(ch->powerup, PU_MYSTIC) || xIS_SET(ch->powerup, PU_USSJ) || IS_POWERUP(ch))
     {
      send_to_char("You can't do this while using another technique.\n\r", ch);
      return;
     }
  
     if ( arg1[0] == '\0' && (xIS_SET(ch->powerup, PU_SSJ1) || xIS_SET(ch->powerup, PU_SSJ4) || xIS_SET(ch->powerup, PU_SSJ2) || xIS_SET(ch->powerup, PU_SSJ3) ) )
     {
      send_to_char("&YYour hair falls and your golden aura fades...\n\r", ch);
      act(AT_YELLOW,"$n's hair falls and $s golden aura fades...", ch, NULL, NULL, TO_CANSEE);
      clear_ssjflags( ch );
      return;
     }
     else if ( arg1[0] == '\0' )
     {
      send_to_char("&YWhat level would you like to attempt?\n\r", ch);
      return;
     }
     if ( !is_number(arg1) )
     {
      send_to_char( "Which SSJ level would you like to attempt?\n\r", ch);
      return;
     }
     if ( !str_cmp( arg1, "1" ) )
     {
      if (ch->exp <= 5000000)
      {
       send_to_char("&YYour aura flashes gold to no avail..\n\r", ch);
       return;
      }      
      if (ch-> rage < 250)
      {
       send_to_char ("You, a Super Saiyan!? Yeah right!\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_SSJ1) )
      {
       send_to_char("&YYour already at that level!\n\r", ch);
       return;
      }
      else
      {
       send_to_char("\n\r&YYour hair rises up and a golden aura engulfs you\n\r&RYou turn into a mighty Super Saiya-Jin again!\n\r", ch);
       act(AT_YELLOW,"$n's hair rises up and a golden aura engulfs $m\n\r&R$n turns into a mighty Super Saiya-Jin again!", ch, NULL, NULL, TO_CANSEE);
       clear_ssjflags( ch );
       xSET_BIT(ch->powerup, PU_SSJ1);
       ch->mod = 4;
       ch->mod_str = 4;
       ch->mod_int = -1;
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       return;
       }          
      } 
      else if ( !str_cmp( arg1, "2" ) )
       {
       if (ch->exp <= 10000000)
       {
         send_to_char("&YYour aura flashes gold to no avail..\n\r", ch);
         return;
       }
       if (ch->rage < 450)
       {
         send_to_char ("You a Super Saiyan 2!? Yeah right!\n\r", ch);
         return;
       }      
       if (xIS_SET(ch->powerup, PU_SSJ2) )
       {
         send_to_char("&YYour already at that level!.\n\r", ch);
         return;
       }
       else
       {
        send_to_char("\n\r&YYou glow with a golden aura and crackle with electricty.\n\r&RYou turn into a powerful Super Saiya-Jin level 2 again!\n\r", ch);
        act(AT_YELLOW,"$n glows with a golden aura and crackle with electricity.\n\r&R$n turns into a powerful Super Saiya-Jin level 2 again!", ch, NULL, NULL, TO_CANSEE);
        clear_ssjflags( ch );
        xSET_BIT(ch->powerup, PU_SSJ2);
        ch->mod = 8.3;
        ch->mod_str = 3;
        ch->mod_dex = 3;
        ch->mod_int = -2;
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0");
        return;
       }
      }
      else if ( !str_cmp( arg1, "3" ) )
      {
       if (!IS_SAIYAN(ch))
       {
        send_to_char("Huh?\n\r", ch);
        return;
       }
       if (ch->exp <= 25000000)
       {
         send_to_char("&YYour aura flashes gold to no avail..\n\r", ch);
         return;
       }
       if (ch->rage < 650)
       {
         send_to_char ("You a Super Saiyan 3!? Yeah right!\n\r", ch);
         return;
       }      
       if (xIS_SET(ch->powerup, PU_SSJ3) )
       {
         send_to_char("&YYour already at that level!\n\r", ch);
         return;
       }
       else
       {
        send_to_char("\n\r&YYou glow with a bright golden aura and your hair flows down your back.\n\r&YYour eyebrows vanish as you make the ultimate transformation\n\r&RYou turn into a vicious Super Saiya-Jin level 3 again!\n\r", ch);
        act(AT_YELLOW,"$n glows with a bright golden aura and $s hair flows down $s back.\n\r$n's eyebrows vanish as $e makes the ultimate transformation.\n\r&R$n turns into a vicious Super Saiya-Jin level 3 again!", ch, NULL, NULL, TO_CANSEE);
       }
        clear_ssjflags( ch );
        xSET_BIT(ch->powerup, PU_SSJ3);
        ch->mod = 9;
        ch->kimod = 1;
        ch->mod_str = 3;
        ch->mod_dex = 3;
        ch->mod_int = -3;
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0"); 
        return;       
      }
     else if ( !str_cmp( arg1, "4" )  )
     {
      if (!IS_SAIYAN(ch))
      {
        send_to_char("Huh?\n\r", ch);
        return;
      }
      if (!xIS_SET(ch->act, PLR_SSJ4) && !xIS_SET(ch->powerup, PU_GOLDEN))
      {
        send_to_char("&YYou cannot achieve this kind of power.\n\r", ch);
        return;
      }
      if (ch->rage < 2000 && !xIS_SET(ch->act, PLR_SSJ4))
      {
        send_to_char("&YYou cannot control this kind of power..\n\r", ch);
        return;
      }   
      if (ch->exp <= 95000000)
      {
        send_to_char("&YYour aura flashes gold to no avail..\n\r", ch);
        return;
      }
      if (ch->rage < 2000 )
      {
        send_to_char ("You a Super Saiyan 4!? Yeah right!\n\r", ch);
        return;
      }      
      if (xIS_SET(ch->powerup, PU_SSJ4) )
      {
        send_to_char("&YYour already at that level!\n\r", ch);
        return;
      }
      else
      {
        send_to_char("\n\r&YYour hair turns ash black and your skin gets covered with red fur.\n\r&YYou growl and look around, nobody can stop you now. \n\r&RYou turn into a vicious Super Saiya-Jin level 4 again!\n\r", ch);
        act(AT_YELLOW,"$n's muscles bulge enourmously and $s hair turns ash black.\n\r&R$n lets loose a loud growl as $s entire body is covered in red fur.\n\r&W$n scowls wildly, and glares at you with $s ferocious red eyes.\n\r&RWithin a flash, the transformation is over, and $e stands here, in all $s\n\rglory, emenating tremendous power.", ch, NULL,NULL, TO_CANSEE);
      }
        clear_ssjflags( ch );
        xSET_BIT(ch->powerup, PU_SSJ4);
	xSET_BIT(ch->act, PLR_SSJ4);
        ch->mod = 10;
	ch->kimod = 1.2;
	ch->height_mod = 0;
        ch->weight_mod = 0;
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0");
        return;       
      }
}

void clear_icerflags( CHAR_DATA *ch )
{
       xREMOVE_BIT(ch->powerup, PU_TRANSFORM2);xREMOVE_BIT(ch->powerup, PU_TRANSFORM3);
       xREMOVE_BIT(ch->powerup, PU_TRANSFORM4);xREMOVE_BIT(ch->powerup, PU_TRANSFORM5);
       xREMOVE_BIT(ch->powerup, PU_TRANSFORM1);xREMOVE_BIT(ch->powerup, PU_METAL);
       ch->height_mod = ch->weight_mod = 0;
       ch->mod = ch->kimod = 1;
       ch->mod_str = ch->mod_dex = ch->mod_lck = ch->mod_wis = ch->mod_int = ch->mod_con = 0;
}

void do_transform( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];  
   int 		     transform;
   argument = one_argument( argument, arg1 );

     if ( IS_NPC(ch) )
     return;
     if (!IS_ICER(ch))
     {
      send_to_char("Huh?\n\r", ch);
      return;
     }
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
      send_to_char("&RSpectators cant do that.\n\r", ch);
      return;
     }
     if ( arg1[0] == '\0' )
     {
      send_to_char( "Transform into which stage?\n\r", ch );
      return;
     }
     if ( !str_cmp( arg1, "metal" ))
     {
       if (ch->exp < 70000000 )
       {
          send_to_char("You aren't strong enough to transform into this.\n\r", ch);
          return;
       }
       if (xIS_SET(ch->powerup, PU_METAL))
       {
          send_to_char("You are already transformed into your metal form.\n\r", ch);
          return;
       }
       send_to_char( "\n\r&WYou close your eyes as you start to change into your ultimate transformation.\n\r&zYour body begins to be covered by a hardened metal.\n\r&BYou open your eyes as you are now covered by a cold metallic armor.\n\r", ch);
       act( AT_GREY, "&W$n closes $s eyes as $e starts to change into $s ultimate transformation\n\r&z$n's body begins to be covered by a hardened metal.\n\r&B$n opens $s eyes as $e is now covered by a cold metallic armor.\n\r", ch, NULL, NULL, TO_CANSEE );
       clear_icerflags( ch );
       ;xSET_BIT(ch->powerup, PU_METAL);
       ch->height_mod = 2;
       ch->weight_mod = 15;
       ch->mod = 9.4;
       ch->kimod = 1.2;
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");

      return;
     }
     if ( !is_number(arg1))
     {
      send_to_char( "It must be a number.\n\r", ch);
      return;
     }
     transform = atoi(arg1);
     switch( transform )
     {
      case 1:
       if (xIS_SET(ch->powerup, PU_TRANSFORM1))
       {
        ch_printf(ch, "&WYour already at that stage.\n\r");
        return;
       }
       else if (xIS_SET(ch->powerup, PU_TRANSFORM2) || xIS_SET(ch->powerup, PU_TRANSFORM3) || xIS_SET(ch->powerup, PU_TRANSFORM4) || xIS_SET(ch->powerup, PU_TRANSFORM5) || xIS_SET(ch->powerup, PU_METAL))
       {
         send_to_char("\n&WYou shrink and you transform to your normal form.\n\rYour body and your muscles have shrunk to the way they were.\n\r", ch);
         act( AT_GREY, "&W$n shrinks and transforms to $s normal form.\n\r$n muscles and $s body are not so big and powerful as before.", ch, NULL, NULL, TO_CANSEE );
         clear_icerflags( ch );
         xSET_BIT(ch->powerup, PU_TRANSFORM1);
         if (xIS_SET(ch->powerup, PU_SUPPRESS))
         do_suppress( ch, "0");
         return;
       }
      break;
      case 2:
       if (xIS_SET(ch->powerup, PU_TRANSFORM2))
       {
         send_to_char("&WYour already at that stage.\n\r", ch);
         return;
       }
       if (xIS_SET(ch->powerup, PU_TRANSFORM1) || xIS_SET(ch->powerup, PU_TRANSFORM3) || xIS_SET(ch->powerup, PU_TRANSFORM4) || xIS_SET(ch->powerup, PU_TRANSFORM5) || xIS_SET(ch->powerup, PU_METAL))
       {
         send_to_char("\n\r&WYou growl as you unleash all of your power and you transform into your second form!\n\rYou scream in pain as your body undergoes changes. You grow in size as your mutation finishes.\n\r", ch);
         act(AT_GREY,"$n growls as $e unleashes all of $s power and $e transforms into $s second form!\n\r$n screams in pain as $s body undergoes changes.", ch, NULL, NULL, TO_CANSEE);
         clear_icerflags( ch );
         xSET_BIT(ch->powerup, PU_TRANSFORM2);
         ch->height_mod = ch->height;
         ch->weight_mod = ch->weight;
         ch->mod = 2;
         if (xIS_SET(ch->powerup, PU_SUPPRESS))
         do_suppress( ch, "0");
         return;
       }
      break;
      case 3:
       if (xIS_SET(ch->powerup, PU_TRANSFORM3))
       {
         send_to_char("&WYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 3999999)
       {
         send_to_char("&WYou are too young to transform into that stage.\n\r", ch);
         return;
       }
       if (xIS_SET(ch->powerup, PU_TRANSFORM1) || xIS_SET(ch->powerup, PU_TRANSFORM2) || xIS_SET(ch->powerup, PU_TRANSFORM4) || xIS_SET(ch->powerup, PU_TRANSFORM5) || xIS_SET(ch->powerup, PU_METAL))
       {
         send_to_char("\n\r&WYou grow in size as your body undergoes its mutation into its third form.\n\rYou feel all the power that now runs through your veins as you.\n\r", ch);
         act(AT_GREY,"$n grows in size as $s body undergoes its mutation into its third form.\n\r$n stands there flexing $s new built muscles.", ch, NULL, NULL, TO_CANSEE);
         clear_icerflags( ch );
         xSET_BIT(ch->powerup, PU_TRANSFORM3);
         ch->height_mod = 12;
         ch->weight_mod = 32;
         ch->mod = 4;
         ch->mod_str = 3;ch->mod_dex = 2;
         ch->mod_int = 2;
         if (xIS_SET(ch->powerup, PU_SUPPRESS))
         do_suppress( ch, "0");
         return;
       }
      break;
      case 4:
       if (xIS_SET(ch->powerup, PU_TRANSFORM4))
       {
         send_to_char("&WYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 14999999)
       {
         send_to_char("&WYou are too young to transform into that stage.\n\r", ch);
         return;
       }
       if (xIS_SET(ch->powerup, PU_TRANSFORM1) || xIS_SET(ch->powerup, PU_TRANSFORM2) || xIS_SET(ch->powerup, PU_TRANSFORM3) || xIS_SET(ch->powerup, PU_TRANSFORM5) || xIS_SET(ch->powerup, PU_METAL))
       {
         send_to_char("\n\r&WYou close your eyes as you try to harvest the ultimate power within you.\n\rYou begin to grow in size and that power now comes alive and begins to be unleashed.\n\rYou open your eyes which are now turned white as you feel amazingly new power that you have achieve.\n\r", ch);
         act(AT_GREY,"$n closes $s eyes as $e tries to harvest the ultimate power within $m\n\r$n begins to grow in size and that now comes alive and begins to be unleashed.\n\r$n opens $s eyes which are now turned into white, and you can feel the amazingly new power that $n has achieved.", ch, NULL, NULL, TO_CANSEE);
         clear_icerflags( ch );
         xSET_BIT(ch->powerup, PU_TRANSFORM4);
         ch->height_mod = -19;
         ch->weight_mod = -50;
         ch->mod = 6;
         ch->mod_str = 3;ch->mod_dex = 2;
         if (xIS_SET(ch->powerup, PU_SUPPRESS))
         do_suppress( ch, "0");
         return;
       }
      break;
      case 5:
       if (xIS_SET(ch->powerup, PU_TRANSFORM5))
       {
         send_to_char("&WYour already at that stage.\n\r", ch);
         return;
       }
       if (ch->exp <= 29999999)
       {
         send_to_char("&WYou are too young to transform into that stage.\n\r", ch);
         return;
       }
       if (xIS_SET(ch->powerup, PU_TRANSFORM1) || xIS_SET(ch->powerup, PU_TRANSFORM2) || xIS_SET(ch->powerup, PU_TRANSFORM3) || xIS_SET(ch->powerup, PU_TRANSFORM4) || xIS_SET(ch->powerup, PU_METAL))
       {
         send_to_char("\n\r&WYou begin to shrink into a midget.\n\rYou are now fast and accurate in all your attacks, your skin turns smooth and shiny.\n\r", ch);
         act(AT_GREY,"$n begins to shrink into a midget.\n\r$n is now fast and accurate in all $s attacks. $n's skin turns smooth and shiny.", ch, NULL, NULL, TO_CANSEE);
         clear_icerflags( ch );
         xSET_BIT(ch->powerup, PU_TRANSFORM5);
         ch->mod = 9;
         ch->height_mod = -19;
         ch->weight_mod = -50;
         ch->mod_dex = 4; ch->mod_int = 4;
         ch->mod_str = 1; ch->mod_lck = -2;
         if (xIS_SET(ch->powerup, PU_SUPPRESS))
         do_suppress( ch, "0");
         return;
       }
       break;
       default:
       break;
     }
       
}


void do_kaioken( CHAR_DATA *ch, char *argument )
{
   char              arg1[MAX_INPUT_LENGTH];  
   int               pl;
   sh_int	     maxlvl, lvl;
   argument = one_argument( argument, arg1 );

     maxlvl = lvl = 0;
     pl = (atoi(arg1) * 100000) + 4000000;

     if ( IS_NPC(ch) )
     return;
     if (is_fused(ch))
     return;
     lvl = ch->pcdata->skill_level2[gsn_kaioken];
     maxlvl = URANGE( 2, lvl*1.34, 20 );
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
     send_to_char("&RSpectators cant do that.\n\r", ch);
     return;
     }
     if (xIS_SET(ch->powerup, PU_MYSTIC) || xIS_SET(ch->powerup, PU_SSJ1) || xIS_SET(ch->powerup, PU_SSJ2) || xIS_SET(ch->powerup, PU_SSJ3) || xIS_SET(ch->powerup, PU_SSJ4) || xIS_SET(ch->powerup, PU_USSJ) || IS_POWERUP(ch))
     {
      send_to_char("You are already another powerup technique.\n\r", ch);
      return;
     }
     if (xIS_SET(ch->powerup, PU_OOZARU))
     {
      send_to_char("Monkeys don't use Kaioken.\n\r", ch);
      return;
     }
     if ( arg1[0] == '\0' && !xIS_SET(ch->powerup, PU_KAIOKEN))
     {
      send_to_char( "Kaioken at level what?\n\r", ch );
      return;
     }
     if ( arg1[0] == '\0' && xIS_SET(ch->powerup, PU_KAIOKEN))
     {
      send_to_char( "\n\r&RYour red glowing and flaming aura dissappears as you descend from Kaioken.\n\r", ch );
      act( AT_RED, "$n's red glowing and flaming aura dissappears as $e descends from Kaioken.\n\r", ch, NULL, NULL, TO_CANSEE );        
      clear_mods( ch );
      xREMOVE_BIT(ch->powerup, PU_KAIOKEN);
      return;
     }
     if ( !is_number(arg1))
     {
      send_to_char( "It must be a number.\n\r", ch);
      return;
     }
     if (atoi(arg1) > maxlvl )
     {
      send_to_char( "Your skill level is not high enough..\n\r", ch);
      return;
     }
     if ( ch->exp < pl )
     {
      send_to_char("\n&RYou explode as your body tears in tiny pieces from using a high level of kaioken.\n\r\n\r", ch);
      act( AT_RED, "$n explodes as $s body tears into tiny pieces from using a high level of kaioken", ch, NULL, NULL, TO_CANSEE );        
      raw_kill( ch, ch);
      clear_mods( ch );
      return;
     }
     if (can_use_skill2(ch, number_percent(), gsn_kaioken ))
     {
      send_to_char("\n\r&RYou slowly glow up with a bright red and white aura.\n\r", ch);
      act( AT_RED, "$n slowly glows up with a bright red and white aura.\n\r", ch, NULL, NULL, TO_CANSEE );
      if ( !str_cmp( arg1, "1" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES ONE!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES ONE!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 1.45;
      } 
      if ( !str_cmp( arg1, "2" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES TWO!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES TWO!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 1.9;
      }     
      if ( !str_cmp( arg1, "3" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES THREE!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES THREE!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 2.35;
      }     
      if ( !str_cmp( arg1, "4" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES FOUR!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES FOUR!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 2.8;
      }
      if ( !str_cmp( arg1, "5" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES FIVE!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES FIVE!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 3.25;
      }
      if ( !str_cmp( arg1, "6" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES SIX!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES SIX!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 3.7;
      }
      if ( !str_cmp( arg1, "7" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES SEVEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES SEVEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 4.15;
      }
      if ( !str_cmp( arg1, "8" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES EIGHT!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES EIGHT!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 4.6;
      }
      if ( !str_cmp( arg1, "9" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES NINE!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES NINE!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 5.05;
      }
      if ( !str_cmp( arg1, "10" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES TEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES TEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 5.5;
      }
      if ( !str_cmp( arg1, "11" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES ELEVEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES ELEVEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 5.95;
      }
      if ( !str_cmp( arg1, "12" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES TWELVE!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES TWELVE!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 6.4;
      }
      if ( !str_cmp( arg1, "13" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES THIRTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES THIRTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 6.85;
      }
      if ( !str_cmp( arg1, "14" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES FOURTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES FOURTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 7.3;
      }
      if ( !str_cmp( arg1, "15" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES FIFTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES FIFTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 7.75;
      }
      if ( !str_cmp( arg1, "16" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES SIXTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES SIXTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 8.2;
      }
      if ( !str_cmp( arg1, "17" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES SEVENTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES SEVENTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 8.5;
      }
      if ( !str_cmp( arg1, "18" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES EIGHTEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES EIGHTEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 8.7;
      }
      if ( !str_cmp( arg1, "19" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES NINETEEN!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES NINETEEN!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 9;
      }
      if ( !str_cmp( arg1, "20" ))
      {
      send_to_char("&RYou yell KAAAIIIOOO-KEEEENN TIMES TWENTY!!\n\r", ch);
      act( AT_RED, "$n yells KAAAIIIOOO-KEEEENN TIMES TWENTY!!", ch, NULL, NULL, TO_CANSEE );
      ch->mod = 9.1;
      }

      send_to_char("\n\r&RYou erupt in a red and white glowing flaming aura!\n\r", ch);
      act( AT_RED, "&R$n erupts in a red and white glowing flaming aura!", ch, NULL, NULL, TO_CANSEE );
      xSET_BIT(ch->powerup, PU_KAIOKEN);
      ch->mana -= 200;
      learn_from_success2(ch, gsn_kaioken);
      ch->mod_str = 3;
      ch->mod_dex = 4;
      if (xIS_SET(ch->powerup, PU_SUPPRESS))
      do_suppress( ch, "0");
      consume_ki( ch, "kaioken" );
      waitstate( ch, "kaioken" );
     } 
     else
     {
      send_to_char("\n\r&RYour aura flashes in red as you try to use the Kaioken technique.\n\r", ch);
      act( AT_RED, "$n's aura flashes in red as $e tries to use the Kaioken technique.", ch, NULL, NULL, TO_CANSEE );        
      ch->mod = 1;
      ch->mana -= 150;           
      ch->mod_str = 0;
      ch->mod_dex = 0;
      learn_from_failure2(ch, gsn_kaioken);
      return;
     }
}

void clear_stageflags ( CHAR_DATA *ch )
{
   xREMOVE_BIT(ch->powerup, PU_SEMIP);
   xREMOVE_BIT(ch->powerup, PU_PERFECT);
   xREMOVE_BIT(ch->powerup, PU_ADULT);
   xREMOVE_BIT(ch->powerup, PU_LARVAE);
   ch->mod = ch->kimod = 1;
   ch->mod_str = ch->mod_dex = ch->mod_lck = ch->mod_wis = ch->mod_int = ch->mod_con = 0;
}

void do_stage( CHAR_DATA *ch, char *argument )
{
     char              arg1[MAX_INPUT_LENGTH];    
     argument = one_argument( argument, arg1 );

     if ( IS_NPC(ch) )
     return;
     if (is_fused(ch))
     return;
     if (!IS_BIODROID(ch))
     {
      send_to_char("Huh?\n\r", ch);
      return;
     }
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
     send_to_char("&RSpectators cant do that.\n\r", ch);
     return;
     }
     if ( IS_POWERUP(ch))
     {
      send_to_char("Your powered up with another technique.\n\r", ch);
      return;
     }     
     if ( arg1[0] == '\0' )
     {
      send_to_char("&RWhat stage would you like to evolve into?\n\r", ch);
      return;
     }
     if ( !str_cmp( arg1, "larvae" ) || !str_cmp( arg1, "1" ) )
     {
      if (xIS_SET(ch->powerup, PU_LARVAE) )
      {
       send_to_char("&RYour already in that stage.\n\r", ch);
       return;
      }
      else
      {
       send_to_char("\n\r&RYou sink your head and limbs back into your body and slump to the\n\rground. You coat your body over as you go back into Larvae Stage.\n\r", ch);
       act(AT_RED,"$n sinks $s head and limbs back into $s body and slumps to the\n\rground. $n coats $s body as $e goes back into Larvae Stage.", ch, NULL, NULL, TO_CANSEE);
       clear_stageflags( ch );
       xSET_BIT(ch->powerup, PU_LARVAE );
       return;
       }          
     } 
     else if ( !str_cmp( arg1, "adult" ) || !str_cmp( arg1, "2" ) )
     {
      if (ch->biomass < 100)
      {
       send_to_char ("&RYou don't have enough biomass to transform.\n\r", ch);
       return;
      }
      if (xIS_SET(ch->powerup, PU_ADULT) )
      {
       send_to_char("&RYour already in that stage.\n\r", ch);
       return;
      }
      else
      {
       send_to_char("\n\r&RYou close your eyes as your body decreases in size and your tail\n\rgrows out around your body. You make a hissing noise as you enter\n\rAdult Stage.\n\r", ch);
       act(AT_RED,"$n closes $s eyes as $s body decreases in size and $s tail\n\rgrows out around $s body. $n makes a hissing noise as $e enters\n\rAdult Stage.", ch, NULL, NULL, TO_CANSEE);
       clear_stageflags( ch );
       xSET_BIT(ch->powerup, PU_ADULT );
       ch->mod = 4;
       ch->mod_str = ch->mod_dex = ch->mod_int = 2;
       if (xIS_SET(ch->powerup, PU_SUPPRESS))
       do_suppress( ch, "0");
       return;
       }          
      } 

      else if ( !str_cmp( arg1, "semi" ) || !str_cmp( arg1, "semiperfect" ) || !str_cmp( arg1, "3" ))
      {
       if (ch->biomass < 1000)
       {
        send_to_char ("&RYou don't have enough biomass to transform.\n\r", ch);
        return;
       }
       if (xIS_SET(ch->powerup, PU_SEMIP) )
       {
         send_to_char("&RYour already in that stage.\n\r", ch);
         return;
       }
       else
       {
        send_to_char("\n\r&RYou stand still pushing back your power limit as your body begins\n\rto grow large and bulky. Your tail fattens out and slaps the\n\rground as you transform into Semi-Perfect Stage.\n\r", ch);
        act(AT_RED,"$n stands still pushing $s power limit as $s body begins \n\rto grow large and bulky. $n's tail fattens out and slaps the\n\rground as $e transforms into Semi-Perfect Stage.", ch, NULL, NULL, TO_CANSEE);
        clear_stageflags( ch );
        xSET_BIT(ch->powerup, PU_SEMIP);
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0");
        ch->mod = 9;
        ch->mod_str = ch->mod_dex = ch->mod_int = 3;
        ch->mod_con = ch->mod_lck = -3;
        return;
       }
      }
      else if ( !str_cmp( arg1, "perfect" ) || !str_cmp( arg1, "4" ))
      {
       if (ch->biomass < 5000)
       {
        send_to_char ("&RYou don't have enough biomass to transform.\n\r", ch);
        return;
       }
       if (xIS_SET(ch->powerup, PU_PERFECT) )
       {
         send_to_char("&RYour already in that stage.\n\r", ch);
         return;
       }
       else
       {
        send_to_char("\n\r&RYou grin and throw your hands into the air as the ground cracks\n\runderneath your feet. Your body trims in size and becomes sleek\n\rand mobile. You stand back as your body take a few seconds to\n\radjust to Perfection.\n\r", ch);
        act(AT_RED,"$n grins and throws $s hands into the air as the ground cracks\n\runderneath $s feet. $n's body trims in size and becomes sleek\n\rand mobile. $n stands back as $s body takes a few seconds\n\rto adjust to Perfection.", ch, NULL, NULL, TO_CANSEE);
        clear_stageflags( ch );
        xSET_BIT(ch->powerup, PU_PERFECT);
        ch->mod = 10;
        ch->kimod = 1.2;
        if (xIS_SET(ch->powerup, PU_SUPPRESS))
        do_suppress( ch, "0");
        return;       
       } 
      }
      else
      {
       send_to_char("What stage would you like to transform? (Larvae|Adult|Semi|Perfect)\n\r", ch);
       return;
      }
}
void clear_androidmod( CHAR_DATA *ch )
{
     ch->mod = ch->kimod = 1;
     ch->mod_str = ch->mod_int = ch->mod_dex = ch->mod_lck = 0;
     xREMOVE_BIT(ch->powerup, PU_POWER1); xREMOVE_BIT(ch->powerup, PU_POWER2);
     xREMOVE_BIT(ch->powerup, PU_POWER3); xREMOVE_BIT(ch->powerup, PU_POWER4);
     xREMOVE_BIT(ch->powerup, PU_POWER5); xREMOVE_BIT(ch->powerup, PU_POWER6);
     xREMOVE_BIT(ch->powerup, PU_POWER7);
}
void do_androidmod( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * chip;

    if ( !str_cmp( argument, "remove" ) )
    {
     clear_mods( ch );
     return;
    }
     if (is_fused(ch))
     return;
    if ( ( chip = get_eq_char( ch, WEAR_SLOT1 ) ) == NULL )
    return;

    if ( chip->pIndexData->vnum == OBJ_VNUM_CHIP_M5 )
    {
     clear_androidmod( ch );
     ch->mod = 9.5; ch->kimod = 1.2;
    }
    else if ( chip->pIndexData->vnum == OBJ_VNUM_CHIP_M4 )
    { 
     clear_androidmod( ch );
     ch->mod = 8.5; 
     ch->mod_str = 2;  ch->mod_int = 2;
     ch->mod_dex = 2; 
     ch->mod_con = -2;
    }
    else if ( chip->pIndexData->vnum == OBJ_VNUM_CHIP_M3 )
    {
     clear_androidmod( ch );
     ch->mod = 6; 
     ch->mod_int = ch->mod_dex = 2;
    }
    else if ( chip->pIndexData->vnum == OBJ_VNUM_CHIP_M2 )
    {
     clear_androidmod( ch );
     ch->mod = 4;
     ch->mod_str = ch->mod_int = ch->mod_dex = ch->mod_lck = 1;
    }
    else if ( chip->pIndexData->vnum == OBJ_VNUM_CHIP_M1 )
    {
     clear_androidmod( ch );
     ch->mod = ch->mod_str = 2;
    }
    else if ( chip->pIndexData->vnum == 144 )
    {
     if (ch->level == 65)
     {
      clear_androidmod( ch );
      ch->mod = 11;
      ch->mod_str = ch->mod_int = ch->mod_dex = ch->mod_lck = 5;
     }
    }
    return;
   
} 

void do_ussj( CHAR_DATA *ch, char *argument )
{
   if ( IS_NPC(ch) )
   return;
   if (IS_HALFBREED(ch) || IS_SAIYAN(ch))
   {
     if (xIS_SET(ch->act, PLR_SPEC) )
     {
       send_to_char("&RSpectators cant do that.\n\r", ch);
       return;
     }
      if (IS_POWERUP(ch) || xIS_SET(ch->powerup, PU_MYSTIC) || xIS_SET(ch->powerup, PU_KAIOKEN) || xIS_SET(ch->powerup, PU_SSJ1) || xIS_SET(ch->powerup, PU_SSJ2) || xIS_SET(ch->powerup, PU_SSJ3) || xIS_SET(ch->powerup, PU_SSJ4) || xIS_SET(ch->powerup, PU_OOZARU))
     {
      send_to_char("Your already using another technique...\n\r", ch);
      return;
     }
     if (ch->exp < 65000000)
     {
      send_to_char("You aren't strong enough...\n\r", ch);
      return;
     }
     if (ch->mana <= 5000 )
     {
      send_to_char("You need more energy to use this technique.\n\r", ch);
      return;
     }
     if (ch->rage < 250)
     {
       send_to_char("&wYou don't have enough rage to do that.\n\r", ch);
       return;
     }
     if (xIS_SET(ch->powerup, PU_SUPPRESS))
     do_suppress( ch, "0");
     if (xIS_SET(ch->powerup, PU_USSJ))
     {
      xREMOVE_BIT(ch->powerup, PU_USSJ);
      send_to_char( "\n\r&YAs you powerdown, your muscles fade to their usual size\n\r&WYour eyes turned back to their normal color.\n\r", ch );
      act(AT_GREY,"$n releases all $s power and $s muscles fade to their usual size\n\r&W$n's eyes have turned back to their normal color as $e powers down from Ultra Super Saiyan.", ch, NULL, NULL, TO_CANSEE);
      clear_mods( ch );
     }
     else
     {
      xSET_BIT(ch->powerup, PU_USSJ);
      send_to_char( "\n\r&YYou scream as your muscles bulk up more than their usual size.\n\r&WYour eyes turn white as your power flows enourmously inside your body.\n\r&RYou have turned into an Ultra Super Saiyan!\n\r", ch );
      act(AT_GREY,"$n screams as $s muscles bulk up more than their usual size\n\r&W$n's eyes turn white as $s power flows enourmously inside $s body.\n\r&R$n has turned into an Ultra Super saiyan!", ch, NULL, NULL, TO_CANSEE);
      ch->mod = 8.6;
      ch->mod_str = 3;
      ch->mod_dex = -3;
      ch->kimod = 1;
      ch->mana -= 5000;
      return;
     }
   }
   else
   {
     send_to_char( "Huh?\n\r", ch);
     return;
   }
}

void do_steam( CHAR_DATA *ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if (!IS_BUU(ch) || !xIS_SET(ch->act, PLR_ULTIMATE))
        {
      		send_to_char("Huh?\n\r", ch);
      		return;
     	}
	
	if(IS_EVIL(ch))
	{
                send_to_char("Huh?\n\r", ch);
                return;
        }


	if(xIS_SET(ch->powerup, PU_STEAM))
	{
		act(AT_WHITE, "Your rage decreases and the steam around you fades away.", ch, NULL, NULL, TO_CHAR);
		act(AT_WHITE, "$n's rage decreases and the steam around $m fades away.", ch, NULL, NULL, TO_ROOM);
		xREMOVE_BIT(ch->powerup, PU_STEAM);
		ch->mod = 8.6;
		if (xIS_SET(ch->powerup, PU_SUPPRESS))
		do_suppress( ch, "0");
		return;
	}
	else
	{
		act(AT_WHITE, "You feel the rage build up inside of you and then steam shoots out of your head!",
			ch, NULL, NULL, TO_CHAR);
		act(AT_WHITE, "$n looks mad! $n's body seems to glow with rage and steam begins to shoot out of $s head!",
			ch, NULL, NULL, TO_ROOM);
		xSET_BIT(ch->powerup, PU_STEAM);
		ch->mod = 9.4;
                if (xIS_SET(ch->powerup, PU_SUPPRESS))
			do_suppress( ch, "0");
		return;
	}
}


