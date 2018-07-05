
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
 *			     Spell handling module			    *
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
#ifdef sun
  #include <strings.h>
#endif
#include <time.h>
#include "mud.h"


/*
 * Local functions.
 */

/*
 * saving throw check						-Thoric
 */

void	say_spell	args( ( CHAR_DATA *ch, int sn ) );
/*
CHAR_DATA *make_poly_mob args( (CHAR_DATA *ch, int vnum) );
*/
bool can_charm ( CHAR_DATA *ch );

/*
 * Lookup a skill by name, only stopping at skills the player has.
 */


/*
 * Lookup a personal skill
 * Unused for now.  In place to allow a player to have a custom spell/skill.
 * When this is put in make sure you put in cleanup code if you do any
 * sort of allocating memory in free_char --Shaddai
 */
int personal_lookup( CHAR_DATA *ch, const char *name )
{
    int sn;

    if ( !ch->pcdata )
	return -1;
    for ( sn = 0; sn < MAX_PERSONAL; sn++ )
    {
	if ( !ch->pcdata->special_skills[sn]
	||   !ch->pcdata->special_skills[sn]->name )
	    return -1;
	if ( LOWER(name[0]) == LOWER(ch->pcdata->special_skills[sn]->name[0])
	&&  !str_prefix( name, ch->pcdata->special_skills[sn]->name ) )
	    return sn;
    }
    return -1;
}

int sskill_lookup( CHAR_DATA *ch, const char *name )
{
    int sn;

      for ( sn = 0; sn < top_sskill; sn++ )
      {
          if ( !sskill_table[sn] || !sskill_table[sn]->name )
              return -1;
          

                 if ( LOWER(name[0]) == LOWER(sskill_table[sn]->name[0])
                 &&  !str_prefix( name, sskill_table[sn]->name ) )
                   return sn;
                 if ( sskill_table[sn]->keyword && LOWER(name[0]) == LOWER(sskill_table[sn]->keyword[0])
                 &&  !str_prefix( name, sskill_table[sn]->keyword ) )
                   return sn;
     }
     return -1;
}

int askill_lookup( CHAR_DATA *ch, const char *name )
{
    int sn;
      for ( sn = 0; sn < top_askill; sn++ )
      {
          if ( !askill_table[sn] || !askill_table[sn]->name)
          return -1;

                 if ( LOWER(name[0]) == LOWER(askill_table[sn]->name[0])
                 &&  !str_prefix( name, askill_table[sn]->name ) )
                   return sn;
                 if ( askill_table[sn]->keyword && LOWER(name[0]) == LOWER(askill_table[sn]->keyword[0])
                 &&  !str_prefix( name, askill_table[sn]->keyword ) )
                   return sn;
	    
      
    }
    return -1;
}




/*
 * Return a skilltype pointer based on sn			-Thoric
 * Returns NULL if bad, unused or personal sn.
 */
SKILLTYPE *get_skilltype( int sn )
{
    if ( sn >= TYPE_PERSONAL )
	return NULL;
    if ( sn >= TYPE_HIT )
	return NULL;
    return IS_VALID_ASN(sn) ? askill_table[sn] : NULL;
}


/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
    char buf  [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    char *pName;
    int iSyl;
    int length;
    SKILLTYPE *skill = get_skilltype( sn );

    struct syl_type
    {
	char *	old;
	char *	new;
    };

    static const struct syl_type syl_table[] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
//	{ "polymorph",  "iaddahs"	},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill->name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill->name );

    for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
	if ( rch != ch )
	act( AT_MAGIC, ch->class==rch->class ? buf : buf2,
	     ch, NULL, rch, TO_VICT );
    }

    return;
}


/*
 * Make adjustments to saving throw based in RIS		-Thoric
 */
int ris_save( CHAR_DATA *ch, int chance, int ris )
{
   sh_int modifier;

   modifier = 10;
   if ( IS_SET(ch->immune, ris ) )
	modifier -= 10;
   if ( IS_SET(ch->resistant, ris ) )
	modifier -= 2;
   if ( IS_SET(ch->susceptible, ris ) )
   {
	if ( IS_NPC( ch )
	&&   IS_SET( ch->immune, ris ) )
	  modifier += 0;
	else
	  modifier += 2;
   }
   if ( modifier <= 0 )
	return 1000;
   if ( modifier == 10 )
	return chance;
   return (chance * modifier) / 10;
}


/*								    -Thoric
 * Fancy dice expression parsing complete with order of operations,
 * simple exponent support, dice support as well as a few extra
 * variables: L = level, H = hp, M = mana, V = move, S = str, X = dex
 *            I = int, W = wis, C = con, A = cha, U = luck, A = age
 *
 * Used for spell dice parsing, ie: 3d8+L-6
 *
 */
int rd_parse(CHAR_DATA *ch, int level, char *exp)
{
    int x, lop = 0, gop = 0, eop = 0;
    char operation;
    char *sexp[2];
    int total = 0, len = 0;

    /* take care of nulls coming in */
    if (!exp || !strlen(exp))
	return 0;

    /* get rid of brackets if they surround the entire expresion */
    if ( (*exp == '(') && !index(exp+1,'(') && exp[strlen(exp)-1] == ')' )
    {
	exp[strlen(exp)-1] = '\0';
	exp++;
    }

    /* check if the expresion is just a number */
    len = strlen(exp);
    if ( len == 1 && isalpha(exp[0]) )
    {
	switch(exp[0])
	{
	    case 'L': case 'l':	return level;
	    case 'H': case 'h':	return ch->hit;
	    case 'M': case 'm':	return ch->mana;
	    case 'V': case 'v':	return ch->move;
	    case 'S': case 's':	return get_curr_str(ch);
	    case 'I': case 'i':	return get_curr_int(ch);
	    case 'W': case 'w':	return get_curr_wis(ch);
	    case 'X': case 'x':	return get_curr_dex(ch);
	    case 'C': case 'c':	return get_curr_con(ch);
	    case 'A': case 'a':	return get_curr_cha(ch);
	    case 'U': case 'u':	return get_curr_lck(ch);
	    case 'Y': case 'y':	return get_age(ch);
	}
    }

    for (x = 0; x < len; ++x)
	if (!isdigit(exp[x]) && !isspace(exp[x]))
	    break;
    if (x == len)
	return atoi(exp);
  
    /* break it into 2 parts */
    for (x = 0; x < strlen(exp); ++x)
	switch(exp[x])
	{
	    case '^':
	      if (!total)
		eop = x;
	      break;
	    case '-': case '+':
	      if (!total) 
		lop = x;
	      break;
	    case '*': case '/': case '%': case 'd': case 'D':
	    case '<': case '>': case '{': case '}': case '=':
	      if (!total) 
		gop =  x;
	      break;
	    case '(':
	      ++total;
	      break;
	    case ')':
	      --total;
	      break;
	}
    if (lop)
	x = lop;
    else
    if (gop)
	x = gop;
    else
	x = eop;
    operation = exp[x];
    exp[x] = '\0';
    sexp[0] = exp;
    sexp[1] = (char *)(exp+x+1);

    /* work it out */
    total = rd_parse(ch, level, sexp[0]);
    switch(operation)
    {
	case '-':		total -= rd_parse(ch, level, sexp[1]);	break;
	case '+':		total += rd_parse(ch, level, sexp[1]);	break;
	case '*':		total *= rd_parse(ch, level, sexp[1]);	break;
	case '/':		total /= rd_parse(ch, level, sexp[1]);	break;
	case '%':		total %= rd_parse(ch, level, sexp[1]);	break;
	case 'd': case 'D':	total = dice( total, rd_parse(ch, level, sexp[1]) );	break;
	case '<':		total = (total < rd_parse(ch, level, sexp[1]));		break;
	case '>':		total = (total > rd_parse(ch, level, sexp[1]));		break;
	case '=':		total = (total == rd_parse(ch, level, sexp[1]));	break;
	case '{':		total = UMIN( total, rd_parse(ch, level, sexp[1]) );	break;
	case '}':		total = UMAX( total, rd_parse(ch, level, sexp[1]) );	break;

	case '^':
	{
	    int y = rd_parse(ch, level, sexp[1]), z = total;

	    for (x = 1; x < y; ++x, z *= total);
	    total = z;
	    break;
	}
    }
    return total;
}

/* wrapper function so as not to destroy exp */
int dice_parse(CHAR_DATA *ch, int level, char *exp)
{
    char buf[MAX_INPUT_LENGTH];

    strcpy( buf, exp );
    return rd_parse(ch, level, buf);
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_poison_death( int level, CHAR_DATA *victim )
{
    int save;

    save = 50 + ( victim->level - level - victim->saving_poison_death ) * 5;
    save = URANGE( 5, save, 95 );
    return chance( victim, save );
}
bool saves_wands( int level, CHAR_DATA *victim )
{
    int save;

    save = 50 + ( victim->level - level - victim->saving_wand ) * 5;
    save = URANGE( 5, save, 95 );
    return chance( victim, save );
}
bool saves_para_petri( int level, CHAR_DATA *victim )
{
    int save;

    save = 50 + ( victim->level - level - victim->saving_para_petri ) * 5;
    save = URANGE( 5, save, 95 );
    return chance( victim, save );
}
bool saves_breath( int level, CHAR_DATA *victim )
{
    int save;

    save = 50 + ( victim->level - level - victim->saving_breath ) * 5;
    save = URANGE( 5, save, 95 );
    return chance( victim, save );
}
bool saves_spell_staff( int level, CHAR_DATA *victim )
{
    int save;

    if ( IS_NPC( victim ) && level > 10 )
      level -= 5;
    save = 50 + ( victim->level - level - victim->saving_spell_staff ) * 5;
    save = URANGE( 5, save, 95 );
    return chance( victim, save );
}


/*
 * Process the spell's required components, if any		-Thoric
 * -----------------------------------------------
 * T###		check for item of type ###
 * V#####	check for item of vnum #####
 * Kword	check for item with keyword 'word'
 * G#####	check if player has ##### amount of gold
 * H####	check if player has #### amount of hitpoints
 *
 * Special operators:
 * ! spell fails if player has this
 * + don't consume this component
 * @ decrease component's value[0], and extract if it reaches 0
 * # decrease component's value[1], and extract if it reaches 0
 * $ decrease component's value[2], and extract if it reaches 0
 * % decrease component's value[3], and extract if it reaches 0
 * ^ decrease component's value[4], and extract if it reaches 0
 * & decrease component's value[5], and extract if it reaches 0
 */
bool process_spell_components( CHAR_DATA *ch, int sn )
{
     SKILLTYPE *skill	= get_skilltype(sn);
     char *comp		= skill->components;
     char *check;
     char arg[MAX_INPUT_LENGTH];
     bool consume, fail, found;
     int  val, value;
     OBJ_DATA *obj;

     /* if no components necessary, then everything is cool */
     if ( !comp || comp[0] == '\0' )
	return TRUE;

     while ( comp[0] != '\0' )
     {
	comp = one_argument( comp, arg );
	consume = TRUE;
	fail = found = FALSE;
	val = -1;
	switch( arg[1] )
	{
	    default:	check = arg+1;				break;
	    case '!':	check = arg+2;	fail = TRUE;		break;
	    case '+':	check = arg+2;	consume = FALSE;	break;
	    case '@':	check = arg+2;	val = 0;		break;
	    case '#':	check = arg+2;	val = 1;		break;
	    case '$':	check = arg+2;	val = 2;		break;
	    case '%':	check = arg+2;	val = 3;		break;
	    case '^':	check = arg+2;	val = 4;		break;
	    case '&':	check = arg+2;	val = 5;		break;
	    /*   reserve '*', '(' and ')' for v6, v7 and v8   */
	}
	value = atoi(check);
	obj = NULL;
	switch( UPPER(arg[0]) )
	{
	    case 'T':
		for ( obj = ch->first_carrying; obj; obj = obj->next_content )
		   if ( obj->item_type == value )
		   {
			if ( fail )
			{
			  send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
			  return FALSE;
			}
			found = TRUE;
			break;
		   }
		break;
	    case 'V':
		for ( obj = ch->first_carrying; obj; obj = obj->next_content )
		   if ( obj->pIndexData->vnum == value )
		   {
			if ( fail )
			{
			  send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
			  return FALSE;
			}
			found = TRUE;
			break;
		   }
		break;
	    case 'K':
		for ( obj = ch->first_carrying; obj; obj = obj->next_content )
		   if ( nifty_is_name( check, obj->name ) )
		   {
			if ( fail )
			{
			  send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
			  return FALSE;
			}
			found = TRUE;
			break;
		   }
		break;
	    case 'G':
	    	if ( ch->gold >= value )
            {
		  if ( fail )
		  {
		    send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
		    return FALSE;
		  }
		  else
		  {
		    if ( consume )
		    {
			set_char_color( AT_GOLD, ch );
			send_to_char( "You feel a little lighter...\n\r", ch );
		 	ch->gold -= value;
		    }
		    continue;
		  }
            }
	    	break;
	    case 'H':
		if ( ch->hit >= value )
            {
		  if ( fail )
		  {
		    send_to_char( "Something disrupts the casting of this spell...\n\r", ch );
		    return FALSE;
		  }
		  else
		  {
		    if ( consume )
		    {
			set_char_color( AT_BLOOD, ch );
			send_to_char( "You feel a little weaker...\n\r", ch );
			ch->hit -= value;
			update_pos( ch );
		    }
		    continue;
		  }
            }
		break;
	}
	/* having this component would make the spell fail... if we get
	   here, then the caster didn't have that component */
	if ( fail )
	    continue;
	if ( !found )
	{
	    send_to_char( "Something is missing...\n\r", ch );
	    return FALSE;
	}
	if ( obj )
	{
	    if ( val >=0 && val < 6 )
	    {
		separate_obj(obj);
		if ( obj->value[val] <= 0 )
		{
		    act( AT_MAGIC, "$p disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
		    act( AT_MAGIC, "$p disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
		    extract_obj( obj );
		    return FALSE;
		}
		else
		if ( --obj->value[val] == 0 )
		{
		    act( AT_MAGIC, "$p glows briefly, then disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
		    act( AT_MAGIC, "$p glows briefly, then disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
		    extract_obj( obj );
		}
		else
		    act( AT_MAGIC, "$p glows briefly and a whisp of smoke rises from it.", ch, obj, NULL, TO_CHAR );
	    }
	    else
	    if ( consume )
	    {
		separate_obj(obj);
		act( AT_MAGIC, "$p glows brightly, then disappears in a puff of smoke!", ch, obj, NULL, TO_CHAR );
		act( AT_MAGIC, "$p glows brightly, then disappears in a puff of smoke!", ch, obj, NULL, TO_ROOM );
		extract_obj( obj );
	    }
	    else
	    {
		int count = obj->count;

		obj->count = 1;
		act( AT_MAGIC, "$p glows briefly.", ch, obj, NULL, TO_CHAR );
		obj->count = count;
	    }
	}
     }
     return TRUE;
}




int pAbort;

/*
 * Locate targets.
 */
/* Turn off annoying message and just abort if needed */
bool silence_locate_targets;

void *locate_targets( CHAR_DATA *ch, char *arg, int sn,
		      CHAR_DATA **victim, OBJ_DATA **obj )
{
    SKILLTYPE *skill = get_skilltype( sn );
    void *vo	= NULL;

    *victim	= NULL;
    *obj	= NULL;
      
    switch ( skill->target )
    {
	default:
	  bug( "Do_cast: bad target for sn %d.", sn );
	  return &pAbort;

	case TAR_IGNORE:
	  break;

	case TAR_CHAR_OFFENSIVE:
	  {
	    if ( arg[0] == '\0' )
	    {
		if ( ( *victim = who_fighting( ch ) ) == NULL )
		{
		    if (!silence_locate_targets)
			send_to_char( "Cast the spell on whom?\n\r", ch );
		    return &pAbort;
		}
	    }
	    else
	    {
		if ( ( *victim = get_char_room( ch, arg ) ) == NULL )
		{
		    if (!silence_locate_targets)
			send_to_char( "They aren't here.\n\r", ch );
		    return &pAbort;
		}
	    }
	  }

	  if ( is_safe(ch, *victim, TRUE) )
		return &pAbort;

	  if ( ch == *victim )
	  {
	        if ( SPELL_FLAG(get_skilltype(sn), SF_NOSELF))
	        {
		  if (!silence_locate_targets)
		    send_to_char( "You can't cast this on yourself!\n\r", ch);
		  return &pAbort;
		}
		if (!silence_locate_targets)
		  send_to_char( "Cast this on yourself?  Okay...\n\r", ch );
		/*
		send_to_char( "You can't do that to yourself.\n\r", ch );
		return &pAbort;
		*/
	  }

	  if ( !IS_NPC(ch) )
	  {
		if ( !IS_NPC(*victim) )
		{
		    if ( get_timer( ch, TIMER_PKILLED ) > 0 )
		    {
			if (!silence_locate_targets)
			    send_to_char( "You have been killed in the last 5 minutes.\n\r", ch);
			return &pAbort;
		    }

		    if ( get_timer( *victim, TIMER_PKILLED ) > 0 )
		    {
			if (!silence_locate_targets)
			    send_to_char( "This player has been killed in the last 5 minutes.\n\r", ch );
			return &pAbort;
		    }	
		    if ( *victim != ch)
		    {
			if (!silence_locate_targets)
		            send_to_char( "You really shouldn't do this to another player...\n\r", ch );
		        else if (who_fighting(*victim) != ch)
		        {
		            /* Only auto-attack those that are hitting you. */
		            return &pAbort;
		        }
		    }
		}

	  }

	  check_illegal_pk( ch, *victim );
	  vo = (void *) *victim;
	  break;

	case TAR_CHAR_DEFENSIVE:
	  {
	    if ( arg[0] == '\0' )
		*victim = ch;
	    else
	    {
		if ( ( *victim = get_char_room( ch, arg ) ) == NULL )
		{
		    if (!silence_locate_targets)
			send_to_char( "They aren't here.\n\r", ch );
		    return &pAbort;
		}
	    }
	  }


	  if ( ch == *victim && 
			SPELL_FLAG(get_skilltype(sn), SF_NOSELF))
	  {
	    if (!silence_locate_targets)
	      send_to_char( "You can't cast this on yourself!\n\r", ch);
	    return &pAbort;
	  }

	  vo = (void *) *victim;
	  break;

	case TAR_CHAR_SELF:
	  if ( arg[0] != '\0' && !nifty_is_name( arg, ch->name ) )
	  {
		if (!silence_locate_targets)
		    send_to_char( "You cannot cast this spell on another.\n\r", ch );
		return &pAbort;
	  }

	  vo = (void *) ch;
	  break;

	case TAR_OBJ_INV:
	  {
	    if ( arg[0] == '\0' )
	    {
		if (!silence_locate_targets)
		    send_to_char( "What should the spell be cast upon?\n\r", ch );
		return &pAbort;
	    }

	    if ( ( *obj = get_obj_carry( ch, arg ) ) == NULL )
	    {
		if (!silence_locate_targets)
		    send_to_char( "You are not carrying that.\n\r", ch );
		return &pAbort;
	    }
	  }

	  vo = (void *) *obj;
	  break;
    }

    return vo;
}



/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;
char *ranged_target_name = NULL;





/*
 * Spell functions.
 */


ch_ret spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return rNONE;
}

/* don't remove, may look redundant, but is important */
ch_ret spell_notfound( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return rNONE;
}


/*
 *   Haus' Spell Additions
 *
 */

/* to do: portal           (like mpcreatepassage)
 *        enchant armour?  (say -1/-2/-3 ac )
 *        sharpness        (makes weapon of caster's level)
 *        repair           (repairs armor)
 *        blood burn       (offensive)  * name: net book of spells *
 *        spirit scream    (offensive)  * name: net book of spells *
 *        something about saltpeter or brimstone
 */

/*
 * saving throw check						-Thoric
 */
bool check_save( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim )
{
    SKILLTYPE *skill = get_skilltype(sn);
    bool saved = FALSE;

    if ( SPELL_FLAG(skill, SF_PKSENSITIVE)
    &&  !IS_NPC(ch) && !IS_NPC(victim) )
	level /= 2;

    if ( skill->saves )
	switch( skill->saves )
	{
	  case SS_POISON_DEATH:
	    saved = saves_poison_death(level, victim);	break;
	  case SS_ROD_WANDS:
	    saved = saves_wands(level, victim);		break;
	  case SS_PARA_PETRI:
	    saved = saves_para_petri(level, victim);	break;
	  case SS_BREATH:
	    saved = saves_breath(level, victim);	break;
	  case SS_SPELL_STAFF:
	    saved = saves_spell_staff(level, victim);	break;
 	}
    return saved;
}

/*
 * Generic offensive spell damage attack			-Thoric
 */
ch_ret spell_attack( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    SKILLTYPE *skill = get_skilltype(sn);
    bool saved = check_save( sn, level, ch, victim );
    int dam;
    ch_ret retcode = rNONE;

    if ( skill->dice )
	dam = UMAX( 0, dice_parse( ch, level, skill->dice ) );
    else
	dam = dice( 1, level/2 );
    if ( saved )
    {
	switch( SPELL_SAVE(skill) )
	{
	    case SE_3QTRDAM:	dam = (dam * 3) / 4;	break;
	    case SE_HALFDAM:	dam >>= 1;		break;
	    case SE_QUARTERDAM:	dam >>= 2;		break;
	    case SE_EIGHTHDAM:	dam >>= 3;		break;

	    case SE_ABSORB:	/* victim absorbs spell for hp's */
		act( AT_MAGIC, "$N absorbs your $t!", ch, skill->noun_damage, victim, TO_CHAR );
		act( AT_MAGIC, "You absorb $N's $t!", victim, skill->noun_damage, ch, TO_CHAR );
		act( AT_MAGIC, "$N absorbs $n's $t!", ch, skill->noun_damage, victim, TO_NOTVICT );
		victim->hit = URANGE( 0, victim->hit + dam, victim->max_hit );
		update_pos( victim );
		if ( (dam > 0 && ch->fighting && ch->fighting->who == victim)
		||   (dam > 0 && victim->fighting && victim->fighting->who == ch) )
		{
		    int xp = ch->fighting ? ch->fighting->xp : victim->fighting->xp;
		    int xp_gain = (int) (xp * dam * 2) / victim->max_hit;

		    gain_exp( ch, 0 - xp_gain );
		}
		return retcode;

	}
    }
    retcode = damage( ch, victim, dam, sn );
    return retcode;
}

/*
 * Generic area attack						-Thoric
 */
ch_ret spell_area_attack( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch, *vch_next;
    SKILLTYPE *skill = get_skilltype(sn);
    bool saved;
    bool affects;
    int dam;
    bool ch_died = FALSE;
    ch_ret retcode = rNONE;


    affects = (skill->affects ? TRUE : FALSE);
    if ( skill->hit_char && skill->hit_char[0] != '\0' )
	act( AT_MAGIC, skill->hit_char, ch, NULL, NULL, TO_CHAR );
    if ( skill->hit_room && skill->hit_room[0] != '\0' )
	act( AT_MAGIC, skill->hit_room, ch, NULL, NULL, TO_ROOM );

    for ( vch = ch->in_room->first_person; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( !IS_NPC(vch) && xIS_SET(vch->act, PLR_WIZINVIS)
	&&    vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
	   continue;

        if ( vch == ch )
	   continue;

        if ( is_safe( ch, vch, FALSE) )
	   continue;

	if ( !IS_NPC(ch) && !IS_NPC(vch) && !in_arena(ch) && ( !IS_PKILL( ch ) 
		|| !IS_PKILL( vch ) ) )
	   continue;

	saved = check_save( sn, level, ch, vch );
	if ( skill->dice )
	    dam = dice_parse(ch, level, skill->dice);
	else
	    dam = dice( 1, level/2 );
	if ( saved )
	{
	    switch( SPELL_SAVE(skill) )
	    {
		case SE_3QTRDAM:	dam = (dam * 3) / 4;	break;
		case SE_HALFDAM:	dam >>= 1;		break;
		case SE_QUARTERDAM:	dam >>= 2;		break;
		case SE_EIGHTHDAM:	dam >>= 3;		break;

		case SE_ABSORB:	/* victim absorbs spell for hp's */
		    act( AT_MAGIC, "$N absorbs your $t!", ch, skill->noun_damage, vch, TO_CHAR );
		    act( AT_MAGIC, "You absorb $N's $t!", vch, skill->noun_damage, ch, TO_CHAR );
		    act( AT_MAGIC, "$N absorbs $n's $t!", ch, skill->noun_damage, vch, TO_NOTVICT );
		    vch->hit = URANGE( 0, vch->hit + dam, vch->max_hit );
		    update_pos( vch );
		    if ( (dam > 0 && ch->fighting && ch->fighting->who == vch)
		    ||   (dam > 0 && vch->fighting && vch->fighting->who == ch) )
		    {
			int xp = ch->fighting ? ch->fighting->xp : vch->fighting->xp;
			int xp_gain = (int) (xp * dam * 2) / vch->max_hit;

			gain_exp( ch, 0 - xp_gain );
		     }
		     continue;

		case SE_REFLECT:	/* reflect the spell to the caster */
	    	    retcode = spell_attack( sn, level, vch, ch );
		    if ( char_died(ch) )
		    {
			ch_died = TRUE;
			break;
		    }
		    continue;
	    }
	}
	retcode = damage( ch, vch, dam, sn );
	if ( retcode == rCHAR_DIED || char_died(ch) )
	{
	    ch_died = TRUE;
	    break;
	}
    }
    return retcode;
}




ch_ret ranged_attack( CHAR_DATA *, char *, OBJ_DATA *, OBJ_DATA *, sh_int, sh_int );

/*
 * Generic handler for new "SMAUG" spells			-Thoric
 */
ch_ret spell_smaug( int sn, int level, CHAR_DATA *ch, void *vo )
{
    struct skill_type *skill = get_skilltype(sn);

    /* Put this check in to prevent crashes from this getting a bad skill */

    if ( !skill )
    {
    	bug ( "spell_smaug: Called with a null skill for sn %d", sn );
	return rERROR;
    }

    return rNONE;
}



/* Haus' new, new mage spells follow */

/*
 *  4 Energy Spells
 */
ch_ret spell_ethereal_fist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(35, level);
    dam         = 1.3*(level*number_range( 1, 6 ))-31;
    dam         = UMAX(0,dam);

    if ( saves_spell_staff( level, victim ) )
	dam/=4;

    return damage( ch, victim, dam, sn );
}


ch_ret spell_spectral_furor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level	= UMAX(0, level);
    level	= UMIN(16, level);
    dam		= 1.3*(level*number_range( 1, 7 )+7);
    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_hand_of_chaos( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(18, level);
    dam         = 1.3*(level*number_range( 1, 7 )+9);

    if ( saves_spell_staff( level, victim ) )
	dam/=4;
    return damage( ch, victim, dam, sn );
}


ch_ret spell_disruption( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level	= UMAX(0, level);
    level	= UMIN(14, level);
    dam		= 1.3*(level*number_range( 1, 6 )+8);

    if ( saves_spell_staff( level, victim ) )
	dam = 0;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_sonic_resonance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(23, level);
    dam         = 1.3*(level*number_range( 1, 8 ));

    if ( saves_spell_staff( level, victim ) )
	dam = dam*3/4;
    return damage( ch, victim, dam, sn );
}

/*
 * 3 Mentalstate spells
 */
ch_ret spell_mind_wrack( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    /* decrement mentalstate by up to 50 */

    level	= UMAX(0, level);
    dam		= number_range( 0, 0 );
    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_mind_wrench( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    
    /* increment mentalstate by up to 50 */

    level	= UMAX(0, level);
    dam		= number_range( 0, 0 );
    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}


/* Non-offensive spell! */
ch_ret spell_revive( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    /* set mentalstate to mentalstate/2 */
    level	= UMAX(0, level);
    dam		= number_range( 0, 0 );
    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}

/*
 * n Acid Spells
 */
ch_ret spell_sulfurous_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(19, level);
    dam         = 1.3*(2*level*number_range( 1, 7 )+11);

    if ( saves_spell_staff( level, victim ) )
	dam /= 4;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_caustic_fount( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(42, level);
    dam         = 1.3*(2*level*number_range( 1, 6 ))-31;
    dam         = UMAX(0,dam);

    if ( saves_spell_staff( level, victim ) )
	dam = dam*1/2;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_acetum_primus( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    dam         = 1.3*(2*level*number_range( 1, 4 )+7);

    if ( saves_spell_staff( level, victim ) )
	dam = 3*dam/4;
    return damage( ch, victim, dam, sn );
}

/*
 *  Electrical
 */

ch_ret spell_galvanic_whip( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level	= UMAX(0, level);
    level	= UMIN(10, level);
    dam		= 1.3*(level*number_range( 1, 6 )+5);

    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_magnetic_thrust( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(29, level);
    dam         = .65* ((5*level*number_range( 1, 6 )) +16);

    if ( saves_spell_staff( level, victim ) )
	dam /= 3;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_quantum_spike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,l;

    level       = UMAX(0, level);
    l 		= 2*(level/10);
    dam         = 1.3*(l*number_range( 1,40)+145);

    if ( saves_spell_staff( level, victim ) )
	dam /= 2;
    return damage( ch, victim, dam, sn );
}

/*
 * Black-magicish guys
 */

/* L2 Mage Spell */
ch_ret spell_black_hand( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level	= UMAX(0, level);
    level	= UMIN(5, level);
    dam		= 1.3*(level*number_range( 1, 6 )+3);

    if ( saves_spell_staff( level, victim ) )
	dam /= 4;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_black_fist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    level       = UMAX(0, level);
    level       = UMIN(30, level);
    dam         = 1.3*(level*number_range( 1, 9 )+4);

    if ( saves_spell_staff( level, victim ) )
	dam /= 4;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_black_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,l;

    level       = UMAX(0, level);
    l 		= 2*level/10;
    dam         = 1.3*(l*number_range(1,50)+135);

    if ( saves_spell_staff( level, victim ) )
	dam /= 4;
    return damage( ch, victim, dam, sn );
}

ch_ret spell_midas_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
   int val;
   OBJ_DATA *obj = (OBJ_DATA *) vo;

   if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
   {
       send_to_char( "You can't seem to let go of it.\n\r", ch );
       return rSPELL_FAILED;
   }

   if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE )
   &&   get_trust( ch ) < LEVEL_IMMORTAL ) /* was victim instead of ch!  Thanks Nick Gammon */
   {
       send_to_char( "That item is not for mortal hands to touch!\n\r", ch );
       return rSPELL_FAILED;   /* Thoric */
   }

   if ( !CAN_WEAR(obj, ITEM_TAKE)  || ( obj->item_type == ITEM_CORPSE_NPC) ||
        ( obj->item_type == ITEM_CORPSE_PC ) )
   {
      send_to_char( "You cannot seem to turn this item to gold!\n\r", ch);
      return rNONE;
   }

   separate_obj(obj);  /* nice, alty :) */

   val = obj->cost/2;
   val = UMAX(0, val);

   ch->gold += val;

   if ( obj_extracted(obj) )
       return rNONE;
   if ( cur_obj == obj->serial )
       global_objcode = rOBJ_SACCED;

   extract_obj( obj );
   send_to_char( "You transmogrify the item to gold!\n\r", ch );

   return rNONE;
}
