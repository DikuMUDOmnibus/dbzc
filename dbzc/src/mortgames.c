/*
 * This is a new automated arena for Smaug 1.4.
 * You can do anything you want with this code, I hope it will be
 * compatible with the DOS version.
 *
 * INSTALLATION:
 * Add to mud.h
 * in pc_data ...
 * char *     betted_on;
 * int 	      bet_amt;
 * down at the bottom of mud.h with all the rest of this stuff ...
 * #define GET_BETTED_ON(ch)    ((ch)->betted_on)
 * #define GET_BET_AMT(ch) ((ch)->bet_amt)
 *
 * change around the Makefile to include this file,
 * You also have to set the room flags in the limbo.are for the arena.
 * The arena bit is 67108864 (It's not included in the help roomflags)
 * This snippet is based on the ROM arena snippet by Kevin Hoogheem
 * It was ported to SMAUG1.4 by LrdElder
 * If you have any cool additions or questions just e-mail me at
 * tdison@swetland.net - LrdElder 10/24/98
 */
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Arena Defines */

#define ARENA_PREP_START  42   /* vnum of first prep room */
#define ARENA_PREP_END    43   /* vnum of last prep room */
#define ARENA_START number_range( 29, 37)    /* vnum of first real arena room*/
#define ARENA_END   41   /* vnum of last real arena room*/
#define HALL_FAME_FILE  SYSTEM_DIR "halloffame.lst"
#define CTF_PREP_START    11502


struct hall_of_fame_element 
{
   char name[MAX_INPUT_LENGTH+1];
   time_t date;
   int award;
   struct  hall_of_fame_element *next;
};
/* KOTH Integers */
void silent_end_koth();
int ppl_waiting_koth = 0;
int ppl_in_koth = 0;
int start_time_koth;
int game_length_koth;
int time_to_start_koth;
int time_left_in_koth;
int koth_pot;
int in_start_koth = 0;
void start_koth( void );
void start_game_koth( void );
void do_koth_game( void );
void find_game_winner2( void );
void show_jack_pot_koth();


void sportschan(char *);
void start_arena();
void show_jack_pot();
void do_game();
int num_in_arena();
void find_game_winner();
void do_end_game();
void do_end_game2();
void start_game();
void silent_end();
void write_fame_list(void);
void write_one_fame_node(FILE * fp, struct hall_of_fame_element * node);
void load_hall_of_fame(void);
void find_bet_winners(CHAR_DATA *winner);
void lost_arena(CHAR_DATA *ch);


struct hall_of_fame_element *fame_list = NULL;

/* Arena Integers */
int ppl_challenged = 0;
int ppl_in_arena = 0;
int in_start_arena = 0;
int start_time;
int game_length;
int lo_lim;
int hi_lim;
int time_to_start;
int time_left_in_game;
int arena_pot;
int bet_pot;
int barena = 0;



extern int parsebet (const int currentbet, char *s);
extern int advatoi (char *s);

void do_bet(CHAR_DATA *ch, char *argument)
 {
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char buf1[MAX_INPUT_LENGTH];
   int newbet;
       
   argument = one_argument(argument,arg);
   one_argument(argument,buf1);
           
   if (IS_NPC(ch))
   {
      send_to_char("Mobs cant bet on the arena.\r\n",ch);
      return;
   }
                    
   if(arg[0]=='\0')
   {
      send_to_char("Usage: bet <player> <amt>\r\n",ch);
      return;
   }
   else if(!in_start_arena && !ppl_challenged)
   {
      send_to_char("Sorry the arena is closed, wait until it opens up to bet.\r\n", ch);
      return;
   }
   else if(ppl_in_arena)
   {
      send_to_char("Sorry Arena has already started, no more bets.\r\n", ch);
      return;
   }
  
   else if (!(ch->betted_on = get_char_world(ch, arg)))
      send_to_char("No such person exists in the MUD.", ch);
   else if (ch->betted_on == ch)
      send_to_char("That doesn't make much sense, does it?\r\n", ch);
   else if(!(IS_SET(ch->betted_on->in_room->room_flags, ROOM_STADIUM)))
      send_to_char("Sorry that person is not in the arena.\r\n", ch);
   else
      {
         if(GET_BET_AMT(ch) > 0)
         {
           send_to_char("Sorry you have already bet.\r\n", ch);
           return;
         }
         GET_BETTED_ON(ch) = ch->betted_on;
         newbet=parsebet(bet_pot,buf1); 
         if(newbet == 0)
         {
            send_to_char("Bet some gold why dont you!\r\n", ch);
            return;
         }
         if (newbet > ch->gold)
         {
            send_to_char("You don't have that much money!\n\r",ch);
            return;
         }
         if(newbet > 50000)
         {
            send_to_char("Sorry the house will not accept that much.\r\n", ch);
            return;
         }
	 if(newbet < 1)
	 {
            send_to_char("You need to bet atleast 1 gold.\r\n", ch);
            return;
         }
       
         ch->gold -= newbet;
         arena_pot += (newbet / 2);
         bet_pot += (newbet / 2);
         GET_BET_AMT(ch) = newbet;
         sprintf(buf, "You place %d gold coins on %s.\r\n", newbet, ch->betted_on->name);
         send_to_char(buf, ch);
         sprintf(buf,"&C%s&c has placed &Y%d&c gold coins on &C%s&c.", ch->name,
         newbet, ch->betted_on->name);
         sportschan(buf);
  }
}

void do_arena(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];
 ROOM_INDEX_DATA *location;

 
 if (IS_NPC(ch))
 {
   send_to_char("Mobs cant play in the arena.\r\n",ch);
   return;
 }

 if(!in_start_arena)
 {
   send_to_char("The stadium is closed right now.\r\n", ch);
   return;
 }
 if (IS_SET( ch->in_room->area->flags, AFLAG_NOARENA ))
 {
  send_to_char("You are not allowed to do that.\n\r", ch);
  return;
 }
 if ( get_timer( ch, TIMER_RECENTFIGHT) > 0 || get_timer( ch, TIMER_PROPAGANDA ) > 0)
 {
  send_to_char("You are not allowed to do that.\n\r", ch);
  return;
 }

 if(ch->level < lo_lim)
 {
   sprintf(buf, "Sorry but you must be at least level %d to enter this arena.\r\n", lo_lim);
   send_to_char(buf, ch);
   return;	
 }
 
 if( ch->level > hi_lim)
 {
    send_to_char("This arena is for lower level characters.\n\r", ch);
    return;
 } 
 
 if(IS_SET(ch->in_room->room_flags, ROOM_STADIUM))
 { 
    send_to_char("You are in the arena already\r\n",ch);
    return;
 }	
 else
 {

    if ( xIS_SET( ch->act, PLR_ONMAP) )
    {
       xREMOVE_BIT(ch->act, PLR_ONMAP);
       xSET_BIT(ch->act, PLR_WASONMAP);
       ch->wasx = ch->x;
       ch->wasy = ch->y;
       ch->wasmap = ch->map;
       ch->x = ch->y = ch->map = -1;
    }

    location = ch->in_room;
    ch->pcdata->roomarena = location;
    act(AT_RED, "$n has been whisked away to the stadium", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(ARENA_PREP_START)); 
    act(AT_WHITE,"$n is dropped from the sky.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You have been taken to the stadium\r\n",ch);
    do_look(ch, "auto");
    sprintf(buf, "&C%s&c has joined the fight in the &YStadium&c!", ch->name);
    sportschan(buf);
    send_to_char(buf, ch);
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move; 
     return;
  }
}


void do_spectate(CHAR_DATA *ch, char *argument)
{ 
 char buf[MAX_INPUT_LENGTH];
 ROOM_INDEX_DATA *location;
 if (IS_NPC(ch))
 {
   send_to_char("Mobs cant spectate.\r\n",ch);
   return;
 }

if(!in_start_arena && !ppl_challenged)
 {
   send_to_char("\n\r&zThe stadium is closed right now...no spectating.\r\n", ch);
   return;
 }
 if(IS_SET(ch->in_room->room_flags, ROOM_STADIUM))
 {
    send_to_char("&zYou are in the arena already.\r\n",ch);
    return;
 }
 else
 {

    if ( xIS_SET( ch->act, PLR_ONMAP) )
    {
       xREMOVE_BIT(ch->act, PLR_ONMAP);
       xSET_BIT(ch->act, PLR_WASONMAP);
       ch->wasx = ch->x;
       ch->wasy = ch->y;
       ch->wasmap = ch->map;
       ch->x = ch->y = ch->map = -1;
    }
    location = ch->in_room;
    ch->pcdata->roomarena = location;
    act(AT_RED, "&z$n has been whisked away to the stadium to spectate", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(ARENA_PREP_START));
    act(AT_WHITE,"&z$n walks into spectate.", ch, NULL, NULL, TO_ROOM);
    send_to_char("&zYou have been taken to the stadium.\r\n",ch);
    if (xIS_SET(ch->act, PLR_ONMAP))
    {
                xREMOVE_BIT(ch->act, PLR_ONMAP);
                xSET_BIT(ch->act, PLR_WASONMAP);
                ch->wasx = ch->x;
                ch->wasy = ch->y;
                ch->wasmap = ch->map;
                ch->x = ch->y = ch->map = -1;
    }

    do_look(ch, "auto");
    sprintf(buf, "&z%s has decided to spectate.", ch->name);
    xSET_BIT(ch->act, PLR_SPEC);
    send_to_char(buf, ch);
    return;
  }
}




void start_arena()
{
  char buf1[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
//  DESCRIPTOR_DATA *d;
  
  
 if (!(ppl_challenged))
 {   
    if(time_to_start == 0)
    {
      in_start_arena = 0;
      show_jack_pot();
      ppl_in_arena = 1;    /* start the blood shed */
      time_left_in_game = game_length;
      start_game();
    }
    else
    {
      if(time_to_start >1)
      {
         sprintf(buf1, "&cThe &YStadium &chas opened its doors to new Fighters&z::");
         sprintf(buf1, "%s\n\r&r|&RGAME&r| &z::&cType &YArena &cto enter, &Y%d&c hour(s) to start", buf1, time_to_start);
         sprintf(buf, "&Y%d&c people have accepted the Challenge", num_in_arena());
         sportschan (buf);
         sportschan (buf1);
      }
      else
      {
         sprintf(buf1, "&cThe &YStadium&c has opened its doors to new Fighters&z::");
         sprintf(buf1, "%s\n\r&r|&RGAME&r| &z::&cType &YArena &cto enter. &Y%d&c hour(s) to start", buf1,time_to_start);
         sprintf(buf, "&Y%d&c people have accepted the Challenge", num_in_arena());
         sportschan (buf);
         sportschan (buf1);
      }
    }
      /* echo_to_all(AT_WHITE, buf1, ECHOTAR_ALL); */
      time_to_start--;
  }  
  else
  if (!(ppl_in_arena))
  {
    if(time_to_start == 0)
    {
      ppl_challenged = 0;
      game_length = 50;
      show_jack_pot();
      ppl_in_arena = 1;    /* start the blood shed */
      time_left_in_game = 50;
      start_game();
    }
    else 
    {
      if(time_to_start >1)
      {
        sprintf(buf1, "&cThe duel will start in &Y%d&c hours",
                        time_to_start);
      }
      else
      {
        sprintf(buf1, "The duel will start in &Y1&c hour");
      }
      sportschan(buf1);
      time_to_start--;
    }
  }
}                      
                                                                                                                                                                                                                                                                                      
void start_game()
{
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;
    
  for (d = first_descriptor; d; d = d->next)
  {
  if( d->connected == CON_PLAYING )
  {
    i = d->character;
    if (IS_SET(i->in_room->room_flags, ROOM_STADIUM))
    {
       send_to_char("\r\nThe floor falls out from bellow, droping you in the arena\r\n", i);
       char_from_room(i);
       char_to_room(i, get_room_index( ARENA_START));
       do_look(i,"auto");
    }
  }
 }
  do_game();
}
void do_game()
{
  char buf[MAX_INPUT_LENGTH];
  
  if(num_in_arena() == 1)
  {
    ppl_in_arena = 0;
    ppl_challenged = 0;
    find_game_winner();
  }
  else if(time_left_in_game == 0)
  {
    do_end_game();
  }
  else if(num_in_arena() == 0)
  {
    ppl_in_arena = 0;
    ppl_challenged = 0;
    silent_end();
  }
  else if(time_left_in_game % 5)
  {
     if ( time_left_in_game > 100 )
     sprintf(buf, "&cThere are still &Y%d&c fighters left.",  num_in_arena());
     else      
     sprintf(buf, "&cWith &Y%d&c hours left in the game there are &Y%d&c Players left",
             time_left_in_game, num_in_arena());
     sportschan(buf);
  }
  else if(time_left_in_game == 1)
  {
    sprintf(buf, "&cWith &Y1&c hour left in the game there are &Y%d&c Players left",
                  num_in_arena());
    sportschan(buf);
  }
  else if(time_left_in_game <= 4)
  {
    sprintf(buf, "&cWith &Y%d&c hours left in the game there are &Y%d&c Players lef.",
            time_left_in_game, num_in_arena());
   sportschan(buf);
  }
  time_left_in_game--;
}

void find_game_winner()
{
  char buf[MAX_INPUT_LENGTH];
//  char buf2[MAX_INPUT_LENGTH];
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;    
  ROOM_INDEX_DATA *location;
  struct hall_of_fame_element *fame_node;
          
  for (d = first_descriptor; d; d = d->next)
      {
       if( d->connected == CON_PLAYING )
        {
        i = d->original ? d->original : d->character;
        if (IS_SET(i->in_room->room_flags,ROOM_STADIUM) && xIS_SET(i->act, PLR_SPEC))
        {
          do_stopspec(i, "");
          continue;
        }
        if (IS_SET(i->in_room->room_flags,ROOM_STADIUM)
            && (i->level < LEVEL_IMMORTAL) && !xIS_SET(i->act, PLR_SPEC))
        {
          i->hit = i->max_hit;
          i->mana = i->max_mana;
          i->move = i->max_move;
          i->challenged=NULL;
//          i->armor = i->pcdata->oldac;
          char_from_room(i);
          location = i->pcdata->roomarena;
          char_to_room(i,location);
	  if ( xIS_SET(i->act, PLR_WASONMAP) )
	  {
 		xREMOVE_BIT(i->act, PLR_WASONMAP);
		xSET_BIT(i->act, PLR_ONMAP);
		i->x = i->wasx;
		i->y = i->wasy;
		i->map = i->wasmap;
		i->wasx = i->wasy = i->wasmap = -1;
          }
          do_look(i, "auto");
          act(AT_YELLOW,"$n falls from the sky.", i, NULL, NULL, TO_ROOM);
          if(time_left_in_game == 1)
          {
             sprintf(buf, "&cAfter &Y1&c hour of battle &C%s&c is declared the winner",i->name);
             sportschan(buf);
          }
          else
          {
             sprintf(buf, "After&Y %d &chours of battle &C%s&c is declared the winner",
                   game_length - time_left_in_game, i->name);
             sportschan(buf);
          }
          i->gold += arena_pot/2;
          sprintf(buf, "&cYou have been awarded &Y%d&c gold coins for winning the fight\r\n",
                        (arena_pot/2));
          send_to_char(buf, i);
          CREATE(fame_node, struct hall_of_fame_element, 1);
          strncpy(fame_node->name, i->name, MAX_INPUT_LENGTH);
          fame_node->name[MAX_INPUT_LENGTH] = '\0';
          fame_node->date = time(0);
          fame_node->award = (arena_pot/2);
          fame_node->next = fame_list;
          fame_list = fame_node;
          write_fame_list();
          find_bet_winners(i);
          ppl_in_arena = 0;
          ppl_challenged = 0;
        }
     }
}
}                                                                                                                                                                                                                                                                                                                     
void show_jack_pot()
{
  char buf1[MAX_INPUT_LENGTH];
  
  sprintf(buf1, "&cLets get ready to &YRUMBLE&c!!!!!!!!\r\n");
  sprintf(buf1, "%s&r|&RGAME&r| &z::&cThe JackPot for this &YArena is &Y%d&c gold coin&z::\r\n",  buf1, arena_pot);
  sprintf(buf1, "%s&r|&RGAME&r| &z::&Y%d&c gold coins have been bet on this &YArena&z",buf1, bet_pot);
  sportschan(buf1);
                    
}

void silent_end()
{
  char buf[MAX_INPUT_LENGTH];
  ppl_in_arena = 0;
  ppl_challenged = 0;
  in_start_arena = 0;
  start_time = 0;
  game_length = 0;
  time_to_start = 0;
  time_left_in_game = 0;
  arena_pot = 0;
  bet_pot = 0;
  sprintf(buf, "It looks like no one was brave enough to enter the &YArena&z");
  sportschan(buf);
}
       
void do_end_game()
{
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *location;      
  for (d = first_descriptor; d; d = d->next)
  {
     if( d->connected == CON_PLAYING )

     {
       i = d->character;
       if (IS_SET(i->in_room->room_flags, ROOM_STADIUM))
       {
          i->hit = i->max_hit;
          i->mana = i->max_mana;
          i->move = i->max_move;
          i->challenged = NULL;
//          i->armor = i->pcdata->oldac;
          stop_fighting(i,TRUE);
          location = i->pcdata->roomarena;
          char_from_room(i);
          char_to_room(i, location);
         if ( xIS_SET( i->act, PLR_WASONMAP) )
         {
             xREMOVE_BIT(i->act, PLR_WASONMAP);
             xSET_BIT(i->act, PLR_ONMAP);
             i->x = i->wasx;
             i->y = i->wasy;
             i->map = i->wasmap;
             i->wasx = i->wasy = i->wasmap = -1;
         }

          do_look(i,"auto");
          act(AT_TELL,"$n falls from the sky.", i, NULL, NULL, TO_ROOM);
       }
     }
     }
     sprintf(buf, "After &Y%d&c hours of battle the &CMatch&c is a &YDraw&z",game_length);
     sportschan(buf);
     time_left_in_game = 0;
     ppl_in_arena=0;
     ppl_challenged = 0;
}                                                                                                                                                                  

int num_in_koth()
{
        DESCRIPTOR_DATA *d;
        int num = 0;

        for (d = first_descriptor; d; d = d->next)
        {
                if( d->connected == CON_PLAYING )
                {
                        if ((IS_SET(d->character->in_room->room_flags2,ROOM_KOTH) || IS_SET(d->character->in_room->room_flags2, ROOM_KOTH2)) && !xIS_SET(d->character->act, PLR_SPEC))
                        {
                                if(d->character->level < LEVEL_IMMORTAL && !xIS_SET(d->character->act, PLR_SEMI))
                                        num++;
                        }
                }
        }
        return num;
}


int num_in_arena()
{
	DESCRIPTOR_DATA *d;
	int num = 0;
      
	for (d = first_descriptor; d; d = d->next)
	{
		if( d->connected == CON_PLAYING )
		{
			if (IS_SET(d->character->in_room->room_flags,ROOM_STADIUM) && !xIS_SET(d->character->act, PLR_SPEC))
			{
				if(d->character->level < LEVEL_IMMORTAL && !xIS_SET(d->character->act, PLR_SEMI))
					num++;
			}
		}
	}
	return num;
}
                                                                                                        
void sportschan(char *argument)
{
  char buf1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *i;
        
  sprintf(buf1, "&r|&RGAME&r| &z::&c%s&z::\r\n", argument);

  for (i = first_descriptor; i; i = i->next)
  {
    if (!i->connected && i->character)
    {
      if(IS_SET( i->character->deaf, CHANNEL_SPORTS))
	continue;

      send_to_char(buf1, i->character);
    }
  }
}



void infochan(char *argument)
{
  char buf1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *i;

  sprintf(buf1, "&b|&BINFO&b|&z: &Y%s&w\r\n", argument);

  for (i = first_descriptor; i; i = i->next)
  {
    if (!i->connected && i->character)
    {
      if(IS_SET( i->character->deaf, CHANNEL_INFO))
        continue;
      send_to_char(buf1, i->character);
    }
  }
}


                                                               
void do_awho(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *tch;
  char buf[MAX_INPUT_LENGTH];
  char buf2[MAX_INPUT_LENGTH];
  int phealth;
  int num=num_in_arena();
          
  if(num==0)
  {
     send_to_char("There is noone in the arena right now.\r\n", ch);
     return;
  }
                            
  sprintf(buf,"\n\r&W  Players inside the &BStadium Arena&W\r\n");
  sprintf(buf,"%s-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-", 
buf);
  
sprintf(buf,"%s&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-\r\n", 
buf);
  sprintf(buf,"%sGame Length = &R%-3d   &WTime To Start &R%-3d\r\n", buf, game_length, time_to_start);
  sprintf(buf,"%s&Wlevel Limits &R%d &Wto &R%d\r\n", buf, lo_lim, hi_lim);
  sprintf(buf,"%s         &WJackpot = &R%d\r\n", buf, arena_pot);
  
sprintf(buf,"%s&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B", 
buf);
  
sprintf(buf,"%s-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B-&W-&B\r\n", 
buf);
  send_to_char(buf, ch);
  for (d = first_descriptor; d; d = d->next)
  {
     if( d->connected == CON_PLAYING )
        {
          tch = d->character;
	  phealth = (100 * tch->hit) / tch->max_hit;
          if (IS_SET(tch->in_room->room_flags, ROOM_STADIUM)
              && (tch->level < LEVEL_IMMORTAL))
          {
             sprintf(buf2, "&W%12.12s         %d%%\n\r", tch->name, phealth);
             send_to_char(buf2,ch);
          }
        }
       }
   return;	
}

void do_ahall(CHAR_DATA *ch, char *argument)
{
  char site[MAX_INPUT_LENGTH], format[MAX_INPUT_LENGTH], *timestr;
  char format2[MAX_INPUT_LENGTH];
  struct hall_of_fame_element *fame_node;
      
  char buf[MAX_INPUT_LENGTH];
  char buf2[MAX_INPUT_LENGTH];
          
  if (!fame_list)
  {
     send_to_char("No-one is in the Hall of Fame.\r\n", ch);
     return;
  }
                                  
     sprintf(buf2,  "&B|---------------------------------------|\r\n");
     strcat(buf2, "|    &WPast Winners of The DBZ:C Arena&B    |\r\n");
     strcat(buf2, "|---------------------------------------|\r\n\r\n"); 

     send_to_char(buf2, ch);
     strcpy(format, "%-25.25s  %-10.10s  %-16.16s\r\n");
     sprintf(buf, format,
        "&RName",
        "&RDate",
        "&RAward Amt");
     send_to_char(buf, ch);
     sprintf(buf, format,
        "&B---------------------------------",
        "&B---------------------------------",
        "&B---------------------------------");
 
     send_to_char(buf, ch);
     strcpy(format2, "&W%-25.25s  &R%-10.10s  &Y%-16d\r\n");
     for (fame_node = fame_list; fame_node; fame_node = fame_node->next)
     {
        if (fame_node->date)
        {
           timestr = asctime(localtime(&(fame_node->date)));
           *(timestr + 10) = 0;
           strcpy(site, timestr);
        }
     else
        strcpy(site, "Unknown");
     sprintf(buf, format2, fame_node->name, site, fame_node->award);
     send_to_char(buf, ch);
     }
     return;
 }

void load_hall_of_fame(void)
{
  FILE *fl;
  int date, award;
  char name[MAX_INPUT_LENGTH + 1];
  struct hall_of_fame_element *next_node;
        
  fame_list = 0;
          
  if (!(fl = fopen(HALL_FAME_FILE, "r")))
  {
    perror("Unable to open hall of fame file");
    return;
  }
  while (fscanf(fl, "%s %d %d", name, &date, &award) == 3)
  {
    CREATE(next_node, struct hall_of_fame_element, 1);
    strncpy(next_node->name, name, MAX_INPUT_LENGTH);
    next_node->date = date;
    next_node->award = award;
    next_node->next = fame_list;
    fame_list = next_node;
  }
  
  fclose(fl);
  return;
}
                                                        
void write_fame_list(void)
{
  FILE *fl;
  
  if (!(fl = fopen(HALL_FAME_FILE, "w")))
  {
     bug("Error writing _hall_of_fame_list", 0);
     return;
  }
  write_one_fame_node(fl, fame_list);/* recursively write from end to start */
  fclose(fl);
                    
   return;
}
                    
void write_one_fame_node(FILE * fp, struct hall_of_fame_element * node)
{
  if (node)
  {
    write_one_fame_node(fp, node->next);
    fprintf(fp, "%s %ld %d\n",node->name,(long) node->date, node->award);
  }
}
                             
void find_bet_winners(CHAR_DATA *winner)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *wch;
    
  char buf1[MAX_INPUT_LENGTH];
      
  for (d = first_descriptor; d; d = d->next)
  {
    if( d->connected == CON_PLAYING )
    {
       wch = d->original ? d->original : d->character;
       if ((!IS_NPC(wch)) && (GET_BET_AMT(wch) > 0) && (GET_BETTED_ON(wch) == winner))
       {
          sprintf(buf1, "You have won %d coins on your bet.\r\n",(GET_BET_AMT(wch))*2);
          send_to_char(buf1, wch);
          wch->gold += GET_BET_AMT(wch)*2;
          GET_BETTED_ON(wch) = NULL;
          GET_BET_AMT(wch) = 0;
       }	  
    }
}
}
void do_challenge(CHAR_DATA *ch, char *argument)
{
 CHAR_DATA *victim;
 char buf[MAX_INPUT_LENGTH];
 ROOM_INDEX_DATA *location;     
   
 if ( ( victim = get_char_world( ch, argument ) ) == NULL)
 {
    send_to_char("&WThat character is not of these realms!\n\r",ch);  
    return;
 }

 if ((ch->level > LEVEL_IMMORTAL) || (victim->level > LEVEL_IMMORTAL))
 {
    send_to_char("Sorry, Immortal's are not allowed to participate in the arena.\n\r",ch);
    return;
 }
         
 if (IS_NPC(victim) || IS_NPC(ch))
 {
    send_to_char("&WYou cannot challenge mobiles!\n\r",ch);
    return;
 }
 
 if (victim->name == ch->name)
 {
   send_to_char("&WYou cannot challenge yourself!",ch);
   return;
 }
 
 if (victim->level<5)
 {
   send_to_char("&WThat character is too young.\n\r",ch);
   return;
 }
 
 if (get_timer(victim,TIMER_PKILLED)>0)
 {
   send_to_char("&WThat player has died within the last 5 minutes and cannot be challenged!\n\r",ch);
   return;
 }
 
 if (get_timer(ch,TIMER_PKILLED)>0)
 {
   send_to_char("&WYou have died within the last 5 minutes and cannot challenge anyone.\n\r",ch);
   return;
 }        
 
 if (num_in_arena()>0)
 {
    send_to_char("&WSomeone is already in the arena!\n\r",ch);
    return;
 }
 if(ch->pcdata->release_date != 0)
 {
   send_to_char("&RYou're in hell. Dink.\n\r", ch);
   return;
 }
 if(victim->pcdata->release_date != 0)
 {
   send_to_char("&RYou cannot challenge someone in hell.\n\r", ch);
    return;
 }
 if (victim->exp < 5000000)
 {
   send_to_char("&RThey aint strong enough.\n\r", ch);
    return;
 }
 if (ch->exp < 5000000)
 {
   send_to_char("&RYou aint strong enough.\n\r", ch);
    return;
 }
 if (victim->challenged == ch)
 {
   send_to_char("&RYou already challenged them.\n\r", ch);
    return;
 }

 sprintf(buf,"&C%s &chas challenged you to a &YDuel&c!\n\r",ch->name);
 send_to_char(buf,victim);
 send_to_char("&cPlease either &CAaccept&c or &CAdecline &cthe &YChallenge&z\n\r\n\r",victim);
 sprintf(buf,"&C%s &chas challenged &C%s&c to a &YDuel&c!",ch->name,victim->name);
 location = ch->in_room;
 ch->pcdata->roomarena = location;
 sportschan(buf);
 victim->challenged = ch;
}

void do_aaccept(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;     
  if (num_in_arena()>0)
  {
   send_to_char("Please wait until the current arena is closed before you accept.\n\r",ch);
   return;
  }

  if (!(ch->challenged))
  {
    send_to_char("You have not been challenged!\n\r",ch);
    return;
  }
  else
  {  

    CHAR_DATA *dch;
    dch = ch->challenged;
    if(!dch || dch == NULL)
    {
	send_to_char("Your challenger has left!\n\r", ch);
	ch->challenged = NULL;
	return;
    }
    if (IS_SET( ch->in_room->area->flags, AFLAG_NOARENA ))
    {
     send_to_char("You are not allowed to do that.\n\r", ch);
     return;
    }
    location = ch->in_room;
    ch->pcdata->roomarena = location;
    sprintf(buf,"&C%s&c has accepted &C%s's &YChallenge&c!",ch->name,dch->name);
    sportschan(buf);
    ch->challenged = NULL;
    char_from_room(ch);
    char_to_room(ch, get_room_index(ARENA_PREP_END));
    do_look(ch,"auto");
    char_from_room(dch);
    char_to_room(dch, get_room_index(ARENA_PREP_START));
    do_look(dch,"auto");
    ppl_in_arena = 0;
    ppl_challenged = 1;
    time_to_start = 2;
    time_left_in_game =0;
    arena_pot =0;
    bet_pot = 0;

    start_arena();
    return;
   }
}

void do_adecline(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];
 
 if (ch->challenged)
 {
   sprintf(buf,"&C%s &chas &YDECLINED &C%s's &cChallenge! WHAT A WUSS!!!",ch->name,ch->challenged->name);
   sportschan(buf);
   ch->challenged=NULL;
   return;
 }
 else 
 {
   send_to_char("You have not been challenged!\n\r",ch);
   return;
 }
}                                                                                                                                                                                                 

void lost_arena(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];

   sprintf(buf, "&C%s&c has been defeated in the &YArena&c!", ch->name );
   sportschan(buf);

   return;
}

bool koth_progress;
void do_koth(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];


 if (IS_NPC(ch))
 {
   send_to_char("Mobs cant play in the King of the Hill.\r\n",ch);
   return;
 }
 if(!in_start_koth)
 {
   send_to_char("The stadium is closed right now.\r\n", ch);
   return;
 }
 if (IS_SET( ch->in_room->area->flags, AFLAG_NOARENA ))
 {
  send_to_char("You are not allowed to do that in here.\n\r", ch);
  return;
 }
 if ( get_timer( ch, TIMER_RECENTFIGHT) > 0 || get_timer( ch, TIMER_PROPAGANDA ) > 0)
 {
  send_to_char("You are not allowed to do that.\n\r", ch);
  return;
 }

 if( ch->exp < 75000000)
 {
   send_to_char( "Sorry but you must be at least 75,000,000 PL to enter the Hill.\r\n", ch);
   return;
 }
 if(IS_SET(ch->in_room->room_flags2, ROOM_KOTH) || IS_SET(ch->in_room->room_flags2, ROOM_KOTH2))
 {
    send_to_char("You are in the Hill already\r\n",ch);
    return;
 }
 if ( xIS_SET( ch->act, PLR_ONMAP) )
 {
       xREMOVE_BIT(ch->act, PLR_ONMAP);
       ch->x = ch->y = ch->map = -1;
 }

    act(AT_RED, "$n has been whisked away to the King of the Hill", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(KOTH_PREP_START));
    act(AT_WHITE,"$n is dropped from the sky.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You have been taken to the King of the Hill\r\n",ch);
    do_look(ch, "auto");
    sprintf(buf, "&C%s&c has joined the fight in the &YHill&c!", ch->name);
    sportschan(buf);
    send_to_char(buf, ch);
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;
     return;
}

void do_ctf(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];


 if (IS_NPC(ch))
 {
   send_to_char("Mobs cant play in the CTF.\r\n",ch);
   return;
 }
 if( ctf == FALSE)
 {
   send_to_char("The CTF Arena is closed right now.\r\n", ch);
   return;
 }
 if (IS_SET( ch->in_room->area->flags, AFLAG_NOARENA ))
 {
  send_to_char("You are not allowed to do that in here.\n\r", ch);
  return;
 }
 if ( get_timer( ch, TIMER_RECENTFIGHT) > 0 || get_timer( ch, TIMER_PROPAGANDA ) > 0)
 {
  send_to_char("You are not allowed to do that.\n\r", ch);
  return;
 }

 if( ch->exp < 75000000)
 {
   send_to_char( "Sorry but you must be at least 75,000,000 PL to enter the CTF Arena.\r\n", ch);
   return;
 }
 if(IS_SET(ch->in_room->room_flags2, ROOM_KOTH) || IS_SET(ch->in_room->room_flags2, ROOM_KOTH2))
 {
    send_to_char("You are in the Hill at the moment!\r\n",ch);
    return;
 }
 if (IS_SET(ch->in_room->room_flags2, ROOM_CTF) )
 {
    send_to_char("You are in the CTF Arena already!\r\n",ch);
    return;
 }
 if ( xIS_SET( ch->act, PLR_ONMAP) )
 {
       xREMOVE_BIT(ch->act, PLR_ONMAP);
       ch->x = ch->y = ch->map = -1;
 }

    act(AT_RED, "$n has been whisked away to the Capture the Flag Arena", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(CTF_PREP_START));
    act(AT_WHITE,"$n is dropped from the sky.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You have been taken to the CTF Arena\r\n",ch);
    do_look(ch, "auto");
    sprintf(buf, "&C%s&c has joined the fight in the &YCTF Arena&c!", ch->name);
    sportschan(buf);
    send_to_char(buf, ch);
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;
     return;
}


void do_chaos(CHAR_DATA *ch, char *argument)
{
  char  arg1[MAX_INPUT_LENGTH];
  char arg2[MIL];
  char arg3[MIL];
  char  buf[MAX_INPUT_LENGTH];
          
  int start_time;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
  if(arg1[0] == '\0')  
  {
    send_to_char("\n\rUsage: chaos arena start_delay", ch);
    send_to_char("\n\r             koth  start_delay game_length", ch); 
    send_to_char("\n\r             raisepot  [how much]", ch );
    send_to_char("\n\r             raisepot2 [how much]", ch );
    return;
  }
  if (!str_cmp(arg1, "raisepot") )
  {
    if(arg2[0] == '\0')
    {
      send_to_char("Usage: chaos raisepot <how much>", ch);
      return;
    }
    arena_pot = atoi(arg2);
    sprintf(buf, "%s has set the Arena Pot to: %d", ch->name, atoi(arg2));
    sportschan(buf);
    return;
  }
  if (!str_cmp(arg1, "raisepot2") )
  {
    if(arg2[0] == '\0')
    {
      send_to_char("Usage: chaos raisepot2 <how much>", ch);
      return;
    }
    koth_pot = atoi(arg2);
    sprintf(buf, "%s has set the KOTH Pot to: %d", ch->name, atoi(arg2));
    sportschan(buf);
    return;
  }
 /*Usage: chaos lo hi start_delay cost/lev length*/
  if ( !str_cmp( arg1, "arena" ) )
  {        
    lo_lim = 5;
    hi_lim = 30;
    start_time = atoi(arg2);
    game_length = 999;
    send_to_char(buf,ch);
    if ( start_time <= 0)
    {
      send_to_char("Lets at least give them a chance to enter!\r\n", ch);
      start_time = 0;
      game_length = 0;
      return;
    }
    ppl_in_arena = 0;
    in_start_arena = 1;
    time_to_start = start_time;
    time_left_in_game = 0;
    arena_pot = 500000;
    bet_pot = 0;
    barena = 1;
    start_arena();
  }                                 
  if ( !str_cmp( arg1, "koth" ) )
  {        
    start_time_koth = atoi(arg2);
    game_length_koth = atoi(arg3);
    send_to_char(buf,ch);
    if ( start_time_koth <= 0)
    {
      game_length_koth = 0;
      start_time_koth = 0;
      send_to_char("Lets at least give them a chance to enter!\r\n", ch);
      return;
    }
    if ( game_length_koth <= 0 )
    {
      send_to_char( "The game must last atleast a minute!\n\r", ch);
      return;
    }
    ppl_in_koth = 0;
    in_start_koth = 1;
    time_to_start_koth = start_time_koth;
    time_left_in_koth = 0;
    koth_pot = 500000;
    bet_pot = 0;
    start_koth();
  }
}

void start_koth()
{
  char buf1[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
    if(time_to_start_koth <= 0)
    {
      in_start_koth = 0;
       show_jack_pot_koth();
      ppl_in_koth = 1;    /* start the blood shed */
      time_left_in_koth = game_length_koth;
      start_game_koth();
    }
    else
    {
      if(time_to_start_koth >1)
      {
         sprintf(buf1, "&cThe &YHill &chas opened its doors to new Fighters&z::");
         sprintf(buf1, "%s\n\r&r|&RGAME&r| &z::&cType &YKOTH &cto enter, &Y%d&c hour(s) to start", buf1, time_to_start_koth);
         sprintf(buf, "&Y%d&c people have accepted the challenge at the Hill", num_in_koth());
         sportschan (buf);
         sportschan (buf1);
      }
      else
      {
         sprintf(buf1, "&cThe &YHill&c has opened its doors to new Fighters&z::");
         sprintf(buf1, "%s\n\r&r|&RGAME&r| &z::&cType &YKOTH &cto enter. &Y%d&c hour(s) to start", buf1,time_to_start_koth);
         sprintf(buf, "&Y%d&c people have accepted the challenge at the Hill", num_in_koth());
         sportschan (buf);
         sportschan (buf1);
      }
    }
      /* echo_to_all(AT_WHITE, buf1, ECHOTAR_ALL); */
      time_to_start_koth--;
}


void start_game_koth()
{
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;
    
  for (d = first_descriptor; d; d = d->next)
  {
  if( d->connected == CON_PLAYING )
  {
    i = d->character;
    if (IS_SET(i->in_room->room_flags2, ROOM_KOTH) || IS_SET(i->in_room->room_flags2, ROOM_KOTH2))
    {
       send_to_char("\r\nThe floor falls out from bellow, droping you in the base of the &YHill&W!\r\n", i);
       char_from_room(i);
       char_to_room(i, get_room_index( KOTH_START ));
       do_look(i,"auto");
    }
  }
 }
  do_koth_game();
}
void do_koth_game()
{
  char buf[MAX_INPUT_LENGTH];
  
  if(num_in_koth() == 1)
  {
    ppl_in_koth = 0;
    find_game_winner2();
  }
  else if(time_left_in_koth == 0)
  {
    do_end_game2();
  }
  else if(num_in_koth() == 0)
  {
    ppl_in_koth = 0;
    silent_end_koth();
  }
  else if (time_left_in_koth <= 0)
  {
     sprintf(buf, "&cThere are still &Y%d&c fighters left.",  num_in_koth());
     sportschan(buf);
  }
  else if(time_left_in_koth % 5)
  {
     if ( time_left_in_game > 100 )
     sprintf(buf, "&cThere are still &Y%d&c fighters left.",  num_in_koth());
     else      
     sprintf(buf, "&cWith &Y%d&c hours left in the game there are &Y%d&c Players left",
             time_left_in_koth, num_in_koth());
     sportschan(buf);
  }
  else if(time_left_in_koth == 1)
  {
    sprintf(buf, "&cWith &Y1&c hour left in the game there are &Y%d&c Players left",
                  num_in_koth());
    sportschan(buf);
  }
  time_left_in_koth--;
}

void find_game_winner2()
{
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;    
          
  for (d = first_descriptor; d; d = d->next)
      {
       if( d->connected == CON_PLAYING )
        {
        i = d->original ? d->original : d->character;
        if (IS_SET(i->in_room->room_flags2,ROOM_KOTH)
            && (i->level < LEVEL_IMMORTAL) && !xIS_SET(i->act, PLR_SPEC))
        {
          i->hit = i->max_hit;
          i->mana = i->max_mana;
          i->move = i->max_move;
          char_from_room(i);
          char_to_room(i,get_room_index(KOTH_LOSER_ROOM));
          do_look(i, "auto");
          act(AT_YELLOW,"$n falls from the sky.", i, NULL, NULL, TO_ROOM);
          i->gold += koth_pot/2;
          sprintf(buf, "&cYou have been awarded &Y%d&c gold coins for being crowned &YKing of the Hill&c!\r\n",
                        (arena_pot/2));
          send_to_char(buf, i);
          ppl_in_koth = 0;
        }
     }
}
}                                                                                                                                                                                                                                                                                                                     

void show_jack_pot_koth()
{
  char buf1[MAX_INPUT_LENGTH];
  
  sprintf(buf1, "&cLets get ready to &YRUMBLE&c!!!!!!!!\r\n");
  sprintf(buf1, "%s&r|&RGAME&r| &z::&cThe JackPot for this &YKOTH is &Y%d&c gold coin&z::\r\n",  buf1, koth_pot);
  sportschan(buf1);
                    
}

void silent_end_koth()
{
  char buf[MAX_INPUT_LENGTH];
  ppl_in_koth = 0;
  in_start_koth = 0;
  start_time_koth = 0;
  game_length_koth = 0;
  time_to_start_koth = 0;
  time_left_in_koth = 0;
  koth_pot = 0;
  sprintf(buf, "It looks like no one was brave enough to enter the &YKOTH&z");
  sportschan(buf);
}
       
void do_end_game2()
{
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *i;
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *location;      
  for (d = first_descriptor; d; d = d->next)
  {
     if( d->connected == CON_PLAYING )

     {
       i = d->character;
       if (IS_SET(i->in_room->room_flags2, ROOM_KOTH) || IS_SET(i->in_room->room_flags2, ROOM_KOTH2))
       {
          if ( i->in_room->vnum == KOTH_HEALING_ROOM )
          {
            i->hit = i->max_hit;
            i->mana = i->max_mana;
            i->move = i->max_move;
            stop_fighting(i,TRUE);
            char_from_room(i);
            char_to_room(i, get_room_index(KOTH_LOSER_ROOM));
            do_look(i,"auto");
            act(AT_TELL,"$n falls from the sky.", i, NULL, NULL, TO_ROOM);
          }
       }
     }
  }
     sprintf(buf, "After &Y%d&c hours of battle the &CBattle in the Hill&c is coming to an end...",game_length_koth);
     sportschan(buf);
     time_left_in_koth = 0;
}


