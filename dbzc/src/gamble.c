/*********************************************************
* Gamble.c has been composed by many snippets that can   *
* be found around the web. None of this is the original  *
* code of Seventeen nor Vegeta, only do_gamble which made*
* gambling alot easier than it was.                      *
*********************************************************/
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
#include "mud.h"

/* 
 * DATA DEFINITIONS
 */



typedef enum
{
	ROULETTE_IDLE, 		/* Roulette is idle. No bets recieved. */
	ROULETTE_BETTING,	/* Someone has bet.. */
	ROULETTE_END_BETTING, 	/* Players have time till
				   next update to bet. In next update the
				   ball will be rolled */
	ROULETTE_ROLLING	/* Ball is rolling. In next update, the rolling
				   will be over and bets will be resolved;
				   roulette will go back to idle */
} roulete_states;

/* bet types */
typedef enum 
{
	BET_NONE, 
	/* from 1 to 36 the bet_type is the number.. */
	BET_RED = 37, BET_BLACK, BET_ODD, BET_EVEN, BET_1STCOL, BET_2NDCOL,
	BET_3RDCOL, BET_1STDOZEN, BET_2NDDOZEN, BET_3RDDOZEN
} BET_TYPE;

/* keywords for betting in each of the non-numeric types */
char * const bet_names[] = 
{
	"red", "black", "odd", "even", "col1", "col2", "col3",
	"doz1", "doz2", "doz3", 
	NULL /* null acts as array terminator, dont remove */
};

/* descriptive names for non numeric bet types */
char * const desc_bet_names[] =
{
	"red", "black", "odds", "evens", "first column", "second column",
	"third column", "first dozen", "second dozen", "third dozen"
};

typedef struct bet_data BET_DATA;
struct bet_data
{
	BET_DATA	* 	next_bet;
	BET_DATA	*	prev_bet;

	char		*	player_name;	/* Name of the gambler */
	int			amount;		/* Amount of money in the bet */
	sh_int			bet_type;	/* On what did they bet? */
};

/* 
 *  MODULE DATA
 */

sh_int			roulette_state = ROULETTE_IDLE;
BET_DATA	*	first_bet = NULL;
BET_DATA	*	last_bet = NULL;
char		*	roulette_extra_descr;
extern sh_int		top_ed ; /* from db.c */

//Constant multipliers types of winnings
#define matchingthreebars     5
#define matchingthreenobar    3
#define wildthree             2
#define twocherries           1

//Constant Defines for max and min slot bets
#define maxsbet		      1000000
#define minsbet               100

//Constant values for the slots
#define slot_cherries         1
#define slot_orange           2
#define slot_plum             3
#define slot_bar              4
#define slot_lemon            5

#define MAX_BET 1000000      /* Maxim bet */
#define MIN_BET 500       /* Minimum bet */
#define REWARD  2*gold_bet      /* Reward for the winner */

int gold_bet;


/*
 *  LOCAL FUNCTIONS
 */
//All the the headers for functions used in the program
int      genslots      (void);
void     calcslots     (CHAR_DATA *ch, int amount2);
void     printslots    (CHAR_DATA *ch, int slot1, int slot2, int slot3);
void     printwinnings (CHAR_DATA *ch, int amount2, int winningsmult);
void     checkslots    (CHAR_DATA *ch, int amount2, int slot1, int slot2, int slot3);
void     playslots_help(CHAR_DATA *ch);
void	 do_playslots  (CHAR_DATA *ch, char *argument );
OBJ_DATA *get_roulette_obj();
CHAR_DATA *get_croupier( ROOM_INDEX_DATA *room );
EXTRA_DESCR_DATA *get_roulette_extra( OBJ_DATA * roulette );
EXTRA_DESCR_DATA *get_extra_from_list( EXTRA_DESCR_DATA *extra_list, char *name );
void clean_bets();
void resolve_bets(CHAR_DATA *croupier, int number );
bool has_won( sh_int type, sh_int number );
int calc_gain( sh_int type, int amount );
char *get_bet_name( sh_int type );
sh_int get_bet_type( char *str );
int advatoi ( char *s ); /*	from bet.h (included in act_obj.c).
				used in auctions.
				god knows why it's in an *.h .. */

void do_gamble( CHAR_DATA *ch, char *argument )
{
   char arg1[MIL];
   char arg2[MIL];
   char arg3[MIL];
   char buf[MIL];

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if ( arg1[0] == '\0' )
   {
      do_help( ch, "_helpgamble_" );
      return;
   }
   if ( !str_cmp( arg1, "arena" ) )
   {
     if ( arg2[0] == '\0' || arg3[0] == '\0' )
     {
        do_gamble( ch, "" );
        return;
     }
     sprintf( buf, "%s %s", arg2, arg3 );
     do_bet( ch, buf );
     WAIT_STATE( ch, 10 );
     return;
   }
   else if ( !str_cmp( arg1, "chance" ) )
   {
     if ( arg2[0] == '\0' )
     {
        do_gamble( ch, "" );
        return;
     }
     do_betc( ch, arg2 );
     return;
   }
   else if ( !str_cmp( arg1, "uncover" ) )
   {
     if ( arg2[0] == '\0' )
     {
        do_gamble( ch, "" );
        return;
     }
     do_uncover( ch, arg2 );
     WAIT_STATE( ch, 10 );
     return;
   }
   else if ( !str_cmp( arg1, "roulette" ) )
   {
     if ( arg2[0] == '\0' || arg3[0] == '\0')
     {
        do_gamble( ch, "" );
        return;
     }
     sprintf( buf, "%s %s", arg2, arg3 );
     do_betr( ch, buf );
     WAIT_STATE( ch, 10 );
     return;
   }
   else if ( !str_cmp( arg1, "slot" ) )
   {
     if ( arg2[0] == '\0')
     {
        do_gamble( ch, "" );
        return;
     }
     do_playslots( ch, arg2 );
     WAIT_STATE( ch, 10 );
     return;
   }
   else
   {
     do_gamble( ch, "" );
     return;
   }
   return;
}  


void do_betr( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	int amount;
	BET_TYPE bet_type;
	OBJ_DATA *roulette;
	ROOM_INDEX_DATA *room;
	CHAR_DATA *croupier;
	BET_DATA *bet;
	EXTRA_DESCR_DATA *extra;

	if( (roulette = get_roulette_obj()) == NULL
	||  (room = roulette->in_room) == NULL 
	||  (croupier = get_croupier(room)) == NULL )
	{
		send_to_char("Sorry but the roulette hasn't been set up properly yet.\n\r", ch );
		return;
	}

	if( !str_cmp( argument, "stop") && IS_IMMORTAL(ch) )
	{
		if( roulette_state != ROULETTE_IDLE )
		{
			act( AT_SAY, "$n says '$N wants us to finish this right now, so...", croupier, NULL, ch, TO_ROOM );
			roulette_state = ROULETTE_ROLLING;
			bet_update();
			send_to_char( "Ok. The roulette round is now over", ch );
		}
		else
			send_to_char( "The roulette is idle right now. No need to do that.\n\r", ch );
		return;
	}

	if( ch->in_room != room )
	{
		send_to_char("You can't bet here. Go to the roulette!\n\r", ch );
		return;
	}

	if( IS_NPC(ch) )
	{
		send_to_char( "Sorry, mobs can't bet.\n\r", ch );
		return;
	}
	if( !can_see( croupier, ch, FALSE ) )
	{
		send_to_char( "You can't bet while being invisible.\n\r", ch );
		return;
	}

	if( roulette_state == ROULETTE_ROLLING )
	{
		sprintf(buf, "%s You can't bet now! Wait till it stops rolling, please.", ch->name );
		do_tell(croupier, buf );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	/* munch optional words */
	if( !str_cmp( arg2, "coins") 
	||  !str_cmp( arg2, "coin") 
	||  !str_cmp( arg2, "gold") )
		argument = one_argument( argument, arg2 );
	if( !str_cmp( arg2, "on" ) )
		argument = one_argument( argument, arg2 );

	if( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Syntax: gamble roulette <amount> <what>\n\r", ch );
		return;
	}

	amount = advatoi( arg1 );
	if( amount <= 0 )
	{
		send_to_char( "Illegal amount.\n\r", ch );
		return;
	}
	if( ch->gold < amount )
	{
		send_to_char( "You don't have that much money.\n\r", ch );
		return;
	}

	bet_type = get_bet_type( arg2 );
	if( bet_type == BET_NONE )
	{
		send_to_char( "Illegal bet. Legal bets are numbers 1-36 and either \"red\" or \"black\".\n\r", ch );
		return;
	}

	extra = get_roulette_extra( roulette );
	if( roulette_state == ROULETTE_IDLE )
	{
		if( extra != NULL )
		{
			roulette_extra_descr = extra->description;
			sprintf(buf, "%s\n\rOn the table you see:\n\r", roulette_extra_descr );
			extra->description = STRALLOC( buf );
		}

		roulette_state = ROULETTE_BETTING;
	}
	if( extra != NULL )
	{
		sprintf(buf, "%s%s has %d coins on %s.\n\r", 
			extra->description,
			ch->name, 
			amount, 
			get_bet_name( bet_type) );
		STRFREE( extra->description );
		extra->description = STRALLOC( buf );
	}

	ch->gold -= amount;

	CREATE( bet, BET_DATA, 1 );
	bet->player_name	= QUICKLINK( ch->name );
	bet->amount		= amount;
	bet->bet_type		= bet_type;
	LINK( bet, first_bet, last_bet, next_bet, prev_bet );

	act( AT_GOLD, "$n places a bet in the roulette table.", ch, NULL, NULL, TO_ROOM );
	sprintf(buf, "%s Your bet of %d coins on %s has been placed, %s.",
		ch->name, amount, get_bet_name( bet_type), ch->name );
	do_tell( croupier, buf );
}

/* look for an extra with the same keyword as the
   roulette's object name.  */
EXTRA_DESCR_DATA *get_roulette_extra( OBJ_DATA * roulette )
{
	EXTRA_DESCR_DATA *extra, *new_extra;

	extra = get_extra_from_list( roulette->first_extradesc, "roulette" );
	if( extra == NULL )
	{
		extra = get_extra_from_list( roulette->pIndexData->first_extradesc, "roulette" );
		if( extra != NULL )
		{
			CREATE(new_extra, EXTRA_DESCR_DATA, 1);
			new_extra->keyword = STRALLOC( "roulette" );
			new_extra->description = QUICKLINK( extra->description );
			LINK( new_extra, roulette->first_extradesc, roulette->last_extradesc, next, prev );
			top_ed ++;
			return new_extra;
		}
	}
	return extra;
}

EXTRA_DESCR_DATA *get_extra_from_list( EXTRA_DESCR_DATA *extra, char *name )
{
	for ( ; extra ; extra = extra->next )
	{
		if( nifty_is_name( name, extra->keyword) )
			return extra;
	}
	return NULL;
}

/* return a descriptive name for that type of bet .. static string */
char *get_bet_name( sh_int type )
{
	static char buf[ MAX_STRING_LENGTH ];

	if( type <= 0 )
	{
		bug( "get_bet_name: invalid type passed" );
		return "(invalid bet)";
	}

	if( type > 0 && type < 37 )
	{
		sprintf( buf, "%d", type );
		return buf;
	}
	else
		return desc_bet_names[ type - BET_RED ];
}

/* return the type of bet for a given keyword string */
sh_int get_bet_type( char *s )
{
	sh_int number, i;
	

	number = atoi(s);
	if( number > 0 && number < 37 )
		return number;
	else
	{
		for( i=0 ; bet_names[i] ; i++ )
		{
			if( !str_cmp(bet_names[i], s) )
				return i + BET_RED;
		}
		return BET_NONE;
	}
}

void bet_update(void)
{
	OBJ_DATA *roulette;
	ROOM_INDEX_DATA *room;
	CHAR_DATA *croupier;
	char buf [MAX_STRING_LENGTH];
	sh_int result;
	EXTRA_DESCR_DATA *extra;

	if( (roulette = get_roulette_obj()) == NULL 
	||  (room = roulette->in_room) == NULL
	||  (croupier = get_croupier(room)) == NULL )
		return;

	switch( roulette_state )
	{
	   case ROULETTE_IDLE:
		do_say( croupier, "Place your bets... Place your bets..." );
		act( AT_ACTION, "$n is waiting for someone to bet...", croupier, NULL, NULL, TO_ROOM );
		return;
	   case ROULETTE_BETTING:
		do_say( croupier, "That's right! Keep betting, keep betting please...");
 		roulette_state = ROULETTE_END_BETTING;
		return;
	   case ROULETTE_END_BETTING:
		act( AT_ACTION, "With a movement of $s hand, $n starts spinning the roulette. After a second, $e throws the ball in.", 
			croupier, NULL, NULL, TO_ROOM );
		do_say( croupier, "No more bets! The ball is rolling! Woohoo!" );
		roulette_state = ROULETTE_ROLLING;
		return;
	   case ROULETTE_ROLLING:
		/* The big moment! */
		act( AT_ACTION, "The roulette stops spinning.", croupier, NULL, NULL, TO_ROOM );
		result = (sh_int) number_range( 0, 36 );
		if( result == 0 )
			sprintf( buf, "The roulette has stopped! It's the zero! Everyone loses. I love these moments, he he he...");
		else
			sprintf( buf, "The roulette has stopped! It's the %d!", result );
		do_say( croupier, buf );
		if ( result != 0 )
			resolve_bets( croupier, result );
		clean_bets();
		roulette_state = ROULETTE_IDLE;

		extra = get_roulette_extra( roulette );
		if( extra != NULL && roulette_extra_descr != NULL )
		{
			STRFREE( extra->description );
			extra->description = roulette_extra_descr;
		}
		return;
	}
}

/* wipe all bets */
void clean_bets()
{
	BET_DATA *bet, *next_bet;

	for( bet = first_bet ; bet ; bet = next_bet )
	{
		next_bet = bet->next_bet;

		STRFREE( bet->player_name );
		DISPOSE( bet );
	}

	first_bet = NULL;
	last_bet = NULL;
}

/* given the result number, resolve each of the outstanding bets, giving
   money to the winners, and informing the losers */
void resolve_bets(CHAR_DATA *croupier, int number)
{
	BET_DATA *bet;
	char buf[ MAX_STRING_LENGTH ];
	int gain;
	bool is_in_room;
	CHAR_DATA *ch;
	int bet_count;

	for( bet = first_bet, bet_count = 0 ; bet ; bet = bet->next_bet, bet_count++ )
	{
		is_in_room = TRUE;
		ch = get_char_room( croupier, bet->player_name );
		if( !ch )
		{
			ch = get_char_world( croupier, bet->player_name );
			is_in_room = FALSE;
		}
		if( !ch )	/* the player left the game... lost his money */
			continue;

		if( has_won(bet->bet_type, number) )
		{
			if( is_in_room )
			{
				act( AT_SAY, "$n says '$N has won!'", croupier, NULL, ch, TO_ROOM );
				gain = calc_gain( bet->bet_type, bet->amount );
				sprintf(buf, "%s You won on %s. Here's your %d coins",
					bet->player_name,
					get_bet_name( bet->bet_type),
					gain);
				do_tell( croupier, buf );
				ch->gold += gain;
				act( AT_GOLD, "$n gives you the money.", croupier, NULL, ch, TO_VICT );
			}
			else 
			{
				act( AT_SAY, "$n says '$N has won! But since $E left, I keep the money.'", croupier, NULL, ch, TO_ROOM );
			}
		}
		else
		{
			if( is_in_room )
			{
				sprintf(buf, "%s Sorry, you have lost your %d coins on %s", 
					bet->player_name,
					bet->amount,
					get_bet_name( bet->bet_type) );
				do_tell( croupier, buf );
			}
		}
	}
}

int calc_gain( sh_int type, int amount )
{
	if( type > 0 && type < 37 )
		return amount * 15;
	else
	{
		switch( type )
		{
			case BET_EVEN:
			case BET_ODD:
			case BET_RED:
			case BET_BLACK:
				return amount * 1.3;
			case BET_1STCOL:
			case BET_2NDCOL:
			case BET_3RDCOL:
			case BET_1STDOZEN:
			case BET_2NDDOZEN:
			case BET_3RDDOZEN:
				return amount * 2.3;
			default:
				bug( "calc_gain: unknown bet type ");
				return 0;
		}
	}
}

/* return true if the bet_type has won for that given number */
bool has_won( sh_int type, sh_int number )
{
	if ( type > 0 && type < 37 )
	{
		return (type == number);
	}
	else
	{
		switch( type )
		{
			case BET_EVEN:
				return ((number % 2) == 0);
			case BET_ODD:
				return ((number % 2) != 0);
			case BET_BLACK:
                          if (number == 2 || number == 4 || number == 6 || number == 8 || number == 10 || number == 12 || number == 14 || number == 16 || number == 18 || number == 20 || number == 22 || number == 24 || number == 26 || number == 28 || number == 30 || number == 32 || number == 34 || number == 36 )
                          return TRUE;
			case BET_RED:
                          if (number == 1 || number == 3 || number == 5 || number == 7 || number == 9 || number == 11 || number == 13 || number == 15 || number == 17 || number == 19 || number == 21 || number == 23 || number == 25 || number == 27 || number == 29 || number == 31 || number == 33 || number == 35 || number == 37 )
                          return TRUE;
			case BET_1STCOL:
			case BET_2NDCOL:
			case BET_3RDCOL:
				return FALSE;
			case BET_1STDOZEN:
				return (number >= 1 && number <= 12);
			case BET_2NDDOZEN:
				return (number >= 13 && number <= 24);
			case BET_3RDDOZEN:
				return (number >= 25 && number <= 36);
				return FALSE;
			default:
				bug( "has_won; unknown bet type" );
				return FALSE;
		}
	}
}

OBJ_DATA *get_roulette_obj()
{
	OBJ_DATA *obj;

	for( obj = first_object ; obj ; obj = obj->next )
	{
		if( obj->pIndexData->vnum == OBJ_VNUM_ROULETTE )
			return obj;
	}
	return NULL;
}

CHAR_DATA *get_croupier( ROOM_INDEX_DATA *room )
{
	CHAR_DATA *ch;

	if( !room )
	{
		bug( "get_croupier: null room ");
		return NULL;
	}

	for( ch = room->first_person ; ch ; ch = ch->next_in_room )
	{
		if( IS_NPC(ch) )
			return ch;
	}
	return NULL;
}

void do_betc( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gambler;
  CHAR_DATA *compet;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  
 argument=one_argument(argument,arg);
 gold_bet = atoi(arg);

 
  for (gambler = ch->in_room->first_person; gambler != NULL; gambler = gambler->next_in_room)
  {
   if (!IS_NPC(gambler))continue;
   if( xIS_SET(gambler->act, ACT_CHANCE)) break;
  }
  if (gambler == NULL || !xIS_SET(gambler->act, ACT_CHANCE))
  {
     send_to_char ("You can't do that here!\n\r",ch);
     return;
  }
  if ( arg[0] == '\0')             /* You must bet something */
  {
   send_to_char("You must bet something.\n\r",ch);
   return;
  } 
  if (gold_bet < MIN_BET || gold_bet > MAX_BET)
  {
   sprintf(buf,"You must bet a reasonable amount. Min: %d, max: %d \n\r",MIN_BET, MAX_BET);
   send_to_char(buf,ch);
   return;
  }
 /* Checks mob is not fighting */
  if (gambler->position == POS_FIGHTING)
  {
    send_to_char ("You must wait he stops fighting.\n\r",ch);
    return;
  }

  /* Cheks there is not another player doing a bet */
  for (compet = ch->in_room->first_person; compet != NULL; compet = compet->next_in_room)
  {
    if( xIS_SET(compet->act, PLR_GAMBLER))
    {  
      act(AT_PLAIN,"You must wait $N finishes a pending bet.",ch,NULL,gambler,TO_CHAR);
      return;
    }
   }

 /* Checks player has enough money */
  if (ch->gold < gold_bet)
  {
   sprintf(buf,"You must have %d gold coins, at least, to do bets.\n\r",gold_bet );
   send_to_char(buf,ch);
   return;
  }
  else
  {
   xSET_BIT(ch->act, PLR_GAMBLER); /* Set gambler bit in player */
   ch->gold -= gold_bet;         /* He takes away bet amount from your gold*/
   sprintf(buf,"\n\r%s puts a small piece of amber into a cup and begin to interchange them.\n\rHe does so quickly than you can't follow his movements. When he finishes, it\n\rseems cups are in the same place. But, where is now the amber?. %s try\n\rto uncover one of the cups!",gambler->name,ch->name);
   act(AT_PLAIN,buf,ch,NULL,NULL,TO_CHAR);
   act(AT_PLAIN,buf,ch,NULL,NULL,TO_ROOM);
   return;
  }
}

void do_uncover(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gambler;
  int result;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH]; 
  
  argument=one_argument(argument,arg);
  result = number_range(1,3);           /* Chooses a random cup */           
  

  for (gambler = ch->in_room->first_person; gambler != NULL; gambler = gambler->next_in_room)
  {
   if (!IS_NPC(gambler))continue;
   if( xIS_SET(gambler->act, ACT_CHANCE)) break;
  }
  if (gambler == NULL || !xIS_SET(gambler->act, ACT_CHANCE))
  {
     send_to_char ("You can't do that here!\n\r",ch);
     return;
  }

  /* Checks bet has been done */                 
  if (!xIS_SET(ch->act, PLR_GAMBLER))
  {
   send_to_char("You must cbet first if you want to play.\n\r",ch);
   return;
  }

  /* Checks mob is not fighting*/              
  if (gambler->position == POS_FIGHTING)
  {
   send_to_char ("You must wait he stops fighting.\n\r",ch);
   return;
  }

  if(!str_cmp(arg,"red") || !str_cmp(arg,"green") || !str_cmp(arg,"blue"))
  {
     sprintf(buf,"\n\r Well, %s bets %d gold coins and says amber is into %s cup.\n\r", ch->name, gold_bet, arg);
     act(AT_PLAIN, buf, ch, NULL, NULL, TO_CHAR);
     act(AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM);
 /* Compare choosen cup with random choice */                 
   if((!str_cmp(arg,"red") && result==1) || (!str_cmp(arg,"green") && result==2) || (!str_cmp(arg,"blue") && result==3))
     {
      sprintf(buf,"\n\r %s uncovers the indicated cup and shouts: price for the %s.\n\r%s has just won %d gold coins.", gambler->name,ch->sex==SEX_FEMALE?"lady" : "gentleman", ch->name, REWARD);
      act(AT_YELLOW,buf,ch,NULL,NULL,TO_CHAR);
      act(AT_YELLOW,buf,ch,NULL,NULL,TO_ROOM); 
      ch->gold += REWARD;      /* Pay winner bet */                   
     }
     else              
     {
      sprintf(buf,"\n\r With a smile %s uncovers the indicated cup and says: Oh, bad luck.\n\rI'm sorry, %s, it wasn't there, precisely.",gambler->name,ch->name);
      act(AT_YELLOW,buf,ch,NULL,NULL,TO_CHAR);
      act(AT_YELLOW,buf,ch,NULL,NULL,TO_ROOM);
     }
   xREMOVE_BIT(ch->act, PLR_GAMBLER);   /* Removes gambler bit in player */                 
  }
  else          
  { 
   act(AT_PLAIN,"$n, you must uncover one of the cups.",ch,NULL,gambler,TO_CHAR);
   return;
  }
}


void playslots_help (CHAR_DATA *ch)
{
   send_to_char( "&WSyntax:\n\r", ch);
   send_to_char( "&wplayslots <amount to bet> \n\r"
      "   &WWinning Combos		             Payoff \n\r"
	  "   &W--------------                   ------ \n\r"
	  "   &w2 cherries                       &C1x    \n\r"
	  "   Wild 3 (3 of a kind with 1 bar)    &C2x    \n\r"
	  "   3 of a kind (not using a bar)      &C3x    \n\r"
	  "   bar bar bar                        &C5x    \n\r", ch );
}

int genslots(void)
{
	int slotval;

	slotval = number_range(1, 100);
 
        if (slotval >= 90)
             return 4;
       
        else if ( slotval >= 75 )
             return 1;
 
        else if ( slotval >= 50 )
             return 5; 

        else if ( slotval >= 25 )
             return 2;
 
        else 
             return 3;
	
}

void calcslots(CHAR_DATA *ch, int amount) //This function makes and sets the values of the slots
{
	int slot1=0, slot2=0, slot3 =0;

	//Set up to output a value of 1 to 5
	slot1 = genslots(); 
	slot2 = genslots();
	slot3 = genslots();

	printslots( ch, slot1, slot2, slot3); //Sending them to the printing function
        checkslots( ch, amount,  slot1, slot2, slot3); //Figuring out if you are the winner
	return;
}

//Below code prints out the nice strings for the values
void printslots(CHAR_DATA *ch, int slot1, int slot2, int slot3)
{ 
    char buf[MSL];
	char slots1[MSL], slots2[MSL], slots3[MSL]; //defining slot strings to be modified
        
        strcpy( slots1, "");
        strcpy( slots2, "");
        strcpy( slots3, "");
//Slot 1 checks for type
	if (slot1 == slot_cherries) //if the rand number is equal to cheries num (1) 
		 strcpy(slots1, "&Rcherries");    //copy cherries to string

	else if (slot1 == slot_orange)       //repeat comment above but change type and num value
		strcpy(slots1, "&Oorange");

	else if (slot1 == slot_plum)
		strcpy(slots1, "&Pplum");

	else if (slot1 == slot_bar)
		strcpy(slots1, "&zbar");

	else
		strcpy(slots1, "&Ylemon");
	
//slot 2 checks for type
        if (slot2 == slot_cherries) //if the rand number is equal to cheries num (1)
                strcpy(slots2, "&Rcherries");    //copy cherries to string

        else if (slot2 == slot_orange)       //repeat comment above but change type and num value
                strcpy(slots2, "&Oorange");

        else if (slot2 == slot_plum)
                strcpy(slots2,"&Pplum");

        else if (slot2 == slot_bar)
                strcpy(slots2,"&zbar");

        else
                strcpy(slots2,"&Ylemon");

//slot 3 checks for type
        if (slot3 == slot_cherries) //if the rand number is equal to cheries num (1)
                strcpy(slots3,"&Rcherries");    //copy cherries to string

        else if (slot3 == slot_orange)       //repeat comment above but change type and num value
                strcpy(slots3,"&Oorange");

        else if (slot3 == slot_plum)
                strcpy(slots3,"&Pplum");

        else if (slot3 == slot_bar)
                strcpy(slots3,"&zbar");

        else
                strcpy(slots3,"&Ylemon");
	
	send_to_char("\n\r&c*********************************************************&w&W\n\r", ch);
	sprintf(buf, "   &c***&w&W  %10.10s   &c***&w&W  %10.10s   &c***&w&W  %10.10s   &c***&w&W       \n\r", slots1, slots2, slots3);
        send_to_char(buf, ch);
	send_to_char("&c*********************************************************&w&W\n\r", ch);	
	slot1 = slot2 = slot3 = 0;
	return;
}

//Below function says if you are a winner or not
//Then adds winnings or takes away the cost per turn
void printwinnings(CHAR_DATA *ch, int amount, int winningsmult)
{
	char buf[MSL];

	if (winningsmult > 0)
	{
		sprintf(buf, "&BWINNER, &WYou won &Y%d &Wgold coin(s)&w&W\n\r", (winningsmult * amount) );
		send_to_char(buf, ch);
		ch->gold += (winningsmult * amount);
        }
	else 
	{
		sprintf(buf, "&RNo luck. &WYou lose &Y%d &Wgold coin(s)\n\r", amount);
		send_to_char(buf, ch);
	}
	return;
}

void checkslots(CHAR_DATA *ch, int amount, int slot1, int slot2, int slot3)
{
	bool oneandtwo=FALSE, oneandthree=FALSE, twoandthree=FALSE;
	bool allthree=FALSE;
        int winningsmult =0;

	      /*1 = "cherries";
		2 = "orange";
		3 = "plum";
	        4 = "bar";
		5 = "lemon";*/

	//This sections checks to see which ones match
	if ( slot1 == slot2)
		oneandtwo = TRUE;

	if( slot1 == slot3)
		oneandthree = TRUE;

	if( slot2 == slot3)
		twoandthree = TRUE;

//Find out if they all match
	if ( (oneandtwo == TRUE) && (oneandthree == TRUE) && (twoandthree == TRUE) )
		allthree = TRUE;

//If they all match and are all bars
    if ( (allthree == TRUE) && (slot1 == slot_bar) )
		winningsmult = matchingthreebars;

//If they all match but aren't bars
	else if ( (allthree == TRUE) && (slot1 != slot_bar) )
		winningsmult = matchingthreenobar;

//This section checks to see if two match and one of the matching is a bar
	else if ( (oneandtwo == TRUE) && (slot1 == slot_bar) )
		winningsmult = wildthree;
	
	else if ( (oneandthree == TRUE) && (slot1 == slot_bar) )
		winningsmult = wildthree;
		
	else if ( (twoandthree == TRUE) && (slot2 == slot_bar) )
		winningsmult = wildthree;

//This section checks to see if two match and the third is a bar (doesn't work for 2 matching bars)
	else if ( (oneandtwo == TRUE) && (slot3 == slot_bar) )
		winningsmult = wildthree;
	
	else if ( (oneandthree == TRUE) && (slot2 == slot_bar) )
		winningsmult = wildthree;
		
	else if ( (twoandthree == TRUE) && (slot1 == slot_bar) )
		winningsmult = wildthree;

	//This section checks to see if two match and they are type cherries
	else if ( (oneandtwo == TRUE) && (slot1 == slot_cherries) )
		winningsmult = twocherries;
	
	else if ( (oneandthree == TRUE) && (slot1 == slot_cherries) )
		winningsmult = twocherries;
		
	else if ( (twoandthree == TRUE) && (slot2 == slot_cherries) )
		winningsmult = twocherries;

	printwinnings(ch, amount, winningsmult); //Send the winningsmult to the printing function
	return;
}

 

void do_playslots (CHAR_DATA *ch, char *argument)
{
	int amount;
        char errbuf[MSL];

	if(IS_NPC(ch))
	return;

	if(argument == NULL)
	{
		playslots_help(ch);
		return;
	}

	if( !IS_SET( ch->in_room->room_flags2, ROOM_SLOTS) )
	{
		send_to_char("&WThere are no slot machines in your vicinity\n\r", ch);
		return;
	}

	amount = atoi(argument);

	if(amount < minsbet)
	{
		sprintf(errbuf, "&WYou must insert at least &Y%d &Wgold coin(s)!\n\r", minsbet);
		send_to_char(errbuf, ch);
		return;
	}

    if(ch->gold < atoi(argument))
    {
        send_to_char("You don't have that much money\n\r", ch);
        return;
    }

	if(amount > maxsbet)
	{
        sprintf(errbuf, "&WWhoah....the max bet is &Y%d &Wgold coin(s)!\n\r", maxsbet);
        send_to_char(errbuf, ch);
        return;
	}

	ch->gold -= amount;
	calcslots(ch, amount);
	return;
}

