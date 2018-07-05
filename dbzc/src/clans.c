/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 *			     Special clan module			    *
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
/* #include <stdlib.h> */
#include <time.h>
#include "mud.h"



#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

CLAN_DATA * first_clan;
CLAN_DATA * last_clan;

/* local routines */
void	fread_clan	args( ( CLAN_DATA *clan, FILE *fp ) );
void	save_docks	args( ( void ) );
bool	load_clan_file	args( ( char *clanfile ) );
void	write_clan_list	args( ( void ) );
void delete_reset( RESET_DATA *pReset );

void create_room( CHAR_DATA *ch, char *argument, int vnum );

void clanchan(char *argument)
{
  char buf1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *i;
  sprintf(buf1, "&b|&GCLAN&b|&z: &w%s\r\n", argument);

  for (i = first_descriptor; i; i = i->next)
  {
    if (!i->connected && i->character)
    {
      send_to_char(buf1, i->character);
    }
  }
}



sh_int get_mem_limit( sh_int level )
{
  switch ( level )
  {
    case 1:
        return 10;
    case 2:
        return 15;
    case 3:
        return 20;
    case 4:
        return 25;
    case 5:
    default:
        return 0;
  }
 return 0;
}

sh_int get_room_limit( sh_int level )
{
  switch ( level )
  {
    case 1:
        return 10;
    case 2:
        return 15;
    case 3:
        return 20;
    case 4:
        return 25;
    case 5:
        return 30;
    default:
        return 0;
  }
 return 0;
}

sh_int get_guard_limit( sh_int level )
{
  switch ( level )
  {
    case 1:
        return 2;
    case 2:
        return 4;
    case 3:
        return 6;
    case 4:
        return 8;
    case 5:
        return 10;
    default:
        return 0;
  }
 return 0;
}

sh_int get_door_limit( sh_int level )
{
  switch ( level )
  {
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    case 5:
        return 5;
    default:
        return 0;
  }
 return 0;
}



/*
 * Get pointer to clan structure from clan name.
 */
CLAN_DATA *get_clan( char *name )
{
    CLAN_DATA *clan;
    
    for ( clan = first_clan; clan; clan = clan->next )
       if ( !str_cmp( name, clan->name ) )
         return clan;
    return NULL;
}


/*
 * Get pointer to clan structure from clan name.
 */
CLAN_DATA *get_clan_short( char *name )
{
    CLAN_DATA *clan;

    if ( !name )
    return NULL;
    for ( clan = first_clan; clan; clan = clan->next )
    {
       if ( !str_cmp( name, clan->shortname ) )
         return clan;
    }
    return NULL;
}



void write_clan_list( )
{
    CLAN_DATA *tclan;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", CLAN_DIR, CLAN_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open clan.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tclan = first_clan; tclan; tclan = tclan->next )
	fprintf( fpout, "%s\n", tclan->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}


/*
 * Save a clan's data to its data file
 */
void save_clan( CLAN_DATA *clan )
{
    FILE *fp;
    char filename[256];

    if ( !clan )
    {
	bug( "save_clan: null clan pointer!", 0 );
	return;
    }
        
    if ( !clan->filename || clan->filename[0] == '\0' )
    {
	bug( "save_clan: %s has no filename", clan->name );
	return;
    }
    
    sprintf( filename, "%s%s", CLAN_DIR, clan->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_clan: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#CLAN\n" );
	fprintf( fp, "Name         %s~\n",	clan->name		);
	fprintf( fp, "Filename     %s~\n",	clan->filename		);
        fprintf( fp, "Shortname    %s~\n",      clan->shortname         );
	fprintf( fp, "Motto        %s~\n",	clan->motto		);
	fprintf( fp, "Description  %s~\n",	clan->description	);
	fprintf( fp, "Deity        %s~\n",	clan->deity		);
	fprintf( fp, "Leader       %s~\n",	clan->leader		);
	fprintf( fp, "NumberOne    %s~\n",	clan->number1		);
	fprintf( fp, "NumberTwo    %s~\n",	clan->number2		);
	fprintf( fp, "Badge        %s~\n",	clan->badge		);
	fprintf( fp, "Leadrank     %s~\n",	clan->leadrank		);
	fprintf( fp, "Onerank      %s~\n",	clan->onerank		);
	fprintf( fp, "Tworank      %s~\n",	clan->tworank		);
	fprintf( fp, "PKillRangeNew   %d %d %d %d %d %d %d\n",
		clan->pkills[0], clan->pkills[1], clan->pkills[2],
		clan->pkills[3], clan->pkills[4], clan->pkills[5],
		clan->pkills[6]);
	fprintf( fp, "PDeathRangeNew  %d %d %d %d %d %d %d\n",	
		clan->pdeaths[0], clan->pdeaths[1], clan->pdeaths[2],
		clan->pdeaths[3], clan->pdeaths[4], clan->pdeaths[5],
		clan->pdeaths[6]);
        fprintf( fp, "RGDCount     %d %d %d\n",    clan->roomcount, clan->guardcount, clan->doorcount );
	fprintf( fp, "MKills       %d\n",	clan->mkills		);
	fprintf( fp, "MDeaths      %d\n",	clan->mdeaths		);
	fprintf( fp, "IllegalPK    %d\n",	clan->illegal_pk	);
	fprintf( fp, "Members      %d\n",	clan->members		);
        fprintf( fp, "Level        %d\n",       clan->level             );
	fprintf( fp, "Board        %d\n",	clan->board		);
	fprintf( fp, "Storeroom    %d\n",	clan->storeroom		);
	fprintf( fp, "Jail	   %d\n",	clan->jail		);
	fprintf( fp, "Guard        %d  %d\n", clan->guard1, clan->guard2);
        fprintf( fp, "Gold	   %lld\n",	clan->gold		);

	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Read in actual clan data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

/*
 * Reads in PKill and PDeath still for backward compatibility but now it
 * should be written to PKillRange and PDeathRange for multiple level pkill
 * tracking support. --Shaddai
 * Added a hardcoded limit memlimit to the amount of members a clan can 
 * have set using setclan.  --Shaddai
 */

void fread_clan( CLAN_DATA *clan, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'B':
            KEY( "Badge",       clan->badge,            fread_string( fp ) );
	    KEY( "Board",	clan->board,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Deity",	clan->deity,		fread_string( fp ) );
	    KEY( "Description",	clan->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!clan->name)
		  clan->name		= STRALLOC( "" );
		if (!clan->leader)
		  clan->leader		= STRALLOC( "" );
		if (!clan->description)
		  clan->description 	= STRALLOC( "" );
                if (!clan->shortname)
                  clan->shortname       = STRALLOC( "" );
		if (!clan->motto)
		  clan->motto		= STRALLOC( "" );
		if (!clan->number1)
		  clan->number1		= STRALLOC( "" );
		if (!clan->number2)
		  clan->number2		= STRALLOC( "" );
		if (!clan->deity)
		  clan->deity		= STRALLOC( "" );
		if (!clan->badge)
	  	  clan->badge		= STRALLOC( "" );
		if (!clan->leadrank)
		  clan->leadrank	= STRALLOC( "" );
		if (!clan->onerank)
		  clan->onerank		= STRALLOC( "" );
		if (!clan->tworank)
		  clan->tworank		= STRALLOC( "" );
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	clan->filename,		fread_string_nohash( fp ) );

	case 'G':
	    KEY( "Gold",	clan->gold,		fread_number( fp ) );
            if ( !str_cmp ( word, "Guard" ) )
            {
                fMatch = TRUE;
                clan->guard1 = fread_number( fp );
                clan->guard2 = fread_number( fp );
            }
	    break;

	case 'I':
	    KEY( "IllegalPK",	clan->illegal_pk,	fread_number( fp ) );
	    break;

        case 'J':
            KEY( "Jail",	clan->jail,		fread_number( fp ) );
            break;

	case 'L':
	    KEY( "Leader",	clan->leader,		fread_string( fp ) );
	    KEY( "Leadrank",	clan->leadrank,		fread_string( fp ) );
            KEY( "Level",       clan->level,            fread_number( fp ) );
	    break;

	case 'M':
	    KEY( "MDeaths",	clan->mdeaths,		fread_number( fp ) );
	    KEY( "Members",	clan->members,		fread_number( fp ) );
	    KEY( "MKills",	clan->mkills,		fread_number( fp ) );
	    KEY( "Motto",	clan->motto,		fread_string( fp ) );
	    break;
 
	case 'N':
	    KEY( "Name",	clan->name,		fread_string( fp ) );
	    KEY( "NumberOne",	clan->number1,		fread_string( fp ) );
	    KEY( "NumberTwo",	clan->number2,		fread_string( fp ) );
	    break;

	case 'O':
	    KEY( "Onerank",	clan->onerank,		fread_string( fp ) );
	    break;

	case 'P':
	    KEY( "PDeaths",	clan->pdeaths[6],	fread_number( fp ) );
	    KEY( "PKills",	clan->pkills[6],	fread_number( fp ) );
	    /* Addition of New Ranges */
	    if ( !str_cmp ( word, "PDeathRange" ) )
	    {
		fMatch = TRUE;
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
            }
	    if ( !str_cmp ( word, "PDeathRangeNew" ) )
            {
		fMatch = TRUE;
		clan->pdeaths[0] = fread_number( fp );
		clan->pdeaths[1] = fread_number( fp );
		clan->pdeaths[2] = fread_number( fp );
		clan->pdeaths[3] = fread_number( fp );
		clan->pdeaths[4] = fread_number( fp );
		clan->pdeaths[5] = fread_number( fp );
		clan->pdeaths[6] = fread_number( fp );
	    }
	    if ( !str_cmp ( word, "PKillRangeNew" ) )
            {
		fMatch = TRUE;
		clan->pkills[0] = fread_number( fp );
		clan->pkills[1] = fread_number( fp );
		clan->pkills[2] = fread_number( fp );
		clan->pkills[3] = fread_number( fp );
		clan->pkills[4] = fread_number( fp );
		clan->pkills[5] = fread_number( fp );
		clan->pkills[6] = fread_number( fp );
	    }
	    if ( !str_cmp ( word, "PKillRange" ) )
	    {
		fMatch = TRUE;
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
            }
	    break;

	case 'R':
            if ( !str_cmp ( word, "RGDCount" ) )
            {
                fMatch = TRUE;
                clan->roomcount  = fread_number( fp );
                clan->guardcount = fread_number( fp );
                clan->doorcount = fread_number( fp );
            }
	    break;

	case 'S':
            KEY( "Shortname",   clan->shortname,        fread_string( fp ) );
	    KEY( "Storeroom",	clan->storeroom,	fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Tworank",	clan->tworank,		fread_string( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_clan: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}



/*
 * Load a clan file
 */

bool load_clan_file( char *clanfile )
{
    char filename[256];
    CLAN_DATA *clan;
    FILE *fp;
    bool found;

    CREATE( clan, CLAN_DATA, 1 );

    /* Make sure we default these to 0 --Shaddai */
    clan->pkills[0] = 0;
    clan->pkills[1] = 0;
    clan->pkills[2] = 0;
    clan->pkills[3] = 0;
    clan->pkills[4] = 0;
    clan->pkills[5] = 0;
    clan->pkills[6] = 0;
    clan->pdeaths[0]= 0;
    clan->pdeaths[1]= 0;
    clan->pdeaths[2]= 0;
    clan->pdeaths[3]= 0;
    clan->pdeaths[4]= 0;
    clan->pdeaths[5]= 0;
    clan->pdeaths[6]= 0;

    found = FALSE;
    sprintf( filename, "%s%s", CLAN_DIR, clanfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_clan_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "CLAN"	) )
	    {
	    	fread_clan( clan, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_clan_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( found )
    {
	ROOM_INDEX_DATA *storeroom;

	LINK( clan, first_clan, last_clan, next, prev );
        if ( clan->storeroom == 0 )
         return found;
	if ( (storeroom = get_room_index( clan->storeroom )) == NULL )
	{
	    log_string( "Storeroom not found" );
	    return found;
	}
	
	sprintf( filename, "%s%s.vault", CLAN_DIR, clan->filename );
	if ( ( fp = fopen( filename, "r" ) ) != NULL )
	{
	    int iNest;
	    bool found;
	    OBJ_DATA *tobj, *tobj_next;

	    log_string( "Loading clan storage room" );
	    rset_supermob(storeroom);
	    for ( iNest = 0; iNest < MAX_NEST; iNest++ )
		rgObjNest[iNest] = NULL;

	    found = TRUE;
	    for ( ; ; )
	    {
		char letter;
		char *word;

		letter = fread_letter( fp );
		if ( letter == '*' )
		{
		    fread_to_eol( fp );
		    continue;
		}

		if ( letter != '#' )
		{
		    bug( "Load_clan_vault: # not found.", 0 );
		    bug( clan->name, 0 );
		    break;
		}

		word = fread_word( fp );
		if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
		  fread_obj  ( supermob, fp, OS_CARRY );
		else
		if ( !str_cmp( word, "END"    ) )	/* Done		*/
		  break;
		else
		{
		    bug( "Load_clan_vault: bad section.", 0 );
		    bug( clan->name, 0 );
		    break;
		}
	    }
	    fclose( fp );
	    for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
	    {
		tobj_next = tobj->next_content;
		obj_from_char( tobj );
		obj_to_room( tobj, storeroom, NULL );
	    }
	    release_supermob();
	}
	else
	    log_string( "Cannot open clan vault" );
    }
    else
      DISPOSE( clan );

    return found;
}


/*
 * Load in all the clan files.
 */
void load_clans( )
{
    FILE *fpList;
    char *filename;
    char clanlist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_clan	= NULL;
    last_clan	= NULL;

    log_string( "Loading clans..." );

    sprintf( clanlist, "%s%s", CLAN_DIR, CLAN_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( clanlist, "r" ) ) == NULL )
    {
	perror( clanlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_clan_file( filename ) )
	{
	  sprintf( buf, "Cannot load clan file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done clans " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
void do_make( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    CLAN_DATA *clan;
    int level;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( str_cmp( ch->name, clan->leader )
    &&   str_cmp( ch->name, clan->deity )
    &&   str_cmp( ch->name, clan->number1 )) 
    {

	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Make what?\n\r", ch );
	return;
    }

    pObjIndex = get_obj_index( clan->clanobj1 );
    level = 40;

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj2 );
      level = 45;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj3 );
      level = 50;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj4 );
      level = 35;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj5 );
      level = 1;
    }

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
	send_to_char( "You don't know how to make that.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    xSET_BIT( obj->extra_flags, ITEM_CLANOBJECT );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
      obj = obj_to_char( obj, ch );
    else
      obj = obj_to_room( obj, ch->in_room, ch );
    act( AT_MAGIC, "$n makes $p!", ch, obj, NULL, TO_ROOM );
    act( AT_MAGIC, "You make $p!", ch, obj, NULL, TO_CHAR );
    return;
}
*/


void do_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("induct", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->deity   )
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "You can't induct such a godly presence.\n\r", ch );
	return;
    }
    if ( victim->exp < 5000000)
    {
        send_to_char( "They aren't strong enough.\n\r", ch );
        return;
    }
    if ( victim->alignment == 0 )
    {
        send_to_char( "They need an alignment.\n\r", ch );
        return;
    }


    if ( victim->pcdata->clan )
    {
	if ( victim->pcdata->clan == clan )
	  send_to_char( "This player already belongs to your clan!\n\r", ch );
	else
	  send_to_char( "This player already belongs to a clan!\n\r", ch );
	return;
    }
    if ( get_mem_limit(clan->level) > 0 && clan->members >= get_mem_limit(clan->level) )
    {
    	send_to_char("Your clan is too big to induct anymore players.\n\r",ch);
	return;
    }
    clan->members++;


    victim->pcdata->clan = clan;
    STRFREE(victim->pcdata->clan_name);
    victim->pcdata->clan_name = QUICKLINK( clan->name );
    act( AT_MAGIC, "You induct $N into $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n inducts $N into $t", ch, clan->name, victim, TO_NOTVICT );
    act( AT_MAGIC, "$n inducts you into $t", ch, clan->name, victim, TO_VICT );
    add_member(victim->name, clan->shortname);
    save_char_obj( victim );
    save_clan( clan );
    return;
}


void do_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char buf[MAX_STRING_LENGTH];
    sh_int lvl, lvl2;
    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;

    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("outcast", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->deity   )
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }


    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	    send_to_char( "Kick yourself out of your own clan?\n\r", ch );
	    return;
    }
 
    if ( IS_IMMORTAL(victim))
    {
      send_to_char("Can't do that.\n\r", ch);
      return;
    }
    if ( victim->pcdata->clan != ch->pcdata->clan )
    {
	    send_to_char( "This player does not belong to your clan!\n\r", ch );
	    return;
    }
    if ( !str_cmp( victim->name, clan->deity ) )
    lvl2 = 4;
    else if ( !str_cmp( victim->name, clan->leader ) )
    lvl2 = 3;
    else if ( !str_cmp( victim->name, clan->number1 ) )
    lvl2 = 2;
    else if ( !str_cmp( victim->name, clan->number2 ) )
    lvl2 = 1;    
    else
    lvl2 = 0;

    if ( !str_cmp( ch->name, clan->deity ) )
    lvl = 4;
    else if ( !str_cmp( ch->name, clan->leader ) )
    lvl = 3;
    else if ( !str_cmp( ch->name, clan->number1 ) )
    lvl = 2;
    else if ( !str_cmp( ch->name, clan->number2 ) )
    lvl = 1;
    else
    lvl = 0;

    if ( lvl2 > lvl )
    {
            send_to_char( "You cannot outcast a higher rank.\n\r", ch );
            return;
    }


    --clan->members;
    if( clan->members < 0 )
       clan->members = 0;

    if ( !str_cmp( victim->name, clan->number1 ) )
    {
	STRFREE( clan->number1 );
	clan->number1 = STRALLOC( "" );
    }
    if ( !str_cmp( victim->name, clan->number2 ) )
    {
	STRFREE( clan->number2 );
	clan->number2 = STRALLOC( "" );
    }
    if( !str_cmp( victim->name, clan->deity ) )
    {
       STRFREE( clan->deity );
       clan->deity = STRALLOC( "" );
    }

    victim->pcdata->clan = NULL;
    STRFREE(victim->pcdata->clan_name);
    victim->pcdata->clan_name = STRALLOC( "" );
    victim->exp = URANGE( 250, victim->exp, 100000000 );
    act( AT_MAGIC, "You outcast $N from $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n outcasts $N from $t", ch, clan->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n outcasts you from $t", ch, clan->name, victim, TO_VICT );
    sprintf(buf, "%s has been outcast from %s!", victim->name, clan->name);
    echo_to_all(AT_MAGIC, buf, ECHOTAR_ALL);
    save_char_obj( victim );	/* clan gets saved when pfile is saved */
    save_clan( clan );
    remove_member(victim->name, clan->shortname);
    return;
}

void do_clanset( CHAR_DATA *ch, char *argument )
{
  char arg[MSL];
  CLAN_DATA *clan;
  argument = one_argument( argument, arg );
  
   if ( IS_NPC(ch) )
   {
     send_to_char("Huh?\n\r", ch);
     return;
   }
   if ( !ch->pcdata || !ch->pcdata->clan )
   {
     send_to_char("Huh?\n\r", ch);
     return;
   }
   if ( str_cmp( ch->name, ch->pcdata->clan->leader ) && str_cmp( ch->name, ch->pcdata->clan->number1 )  )
   {
     send_to_char("Huh?\n\r", ch);
     return;
   }
   clan = ch->pcdata->clan;
   if ( arg[0] == '\0' )
   {
        send_to_char( "\n\rUsage: clanset <field> <value>\n\r", ch );
        send_to_char( "\n\rField being one of:\n\r", ch );
        send_to_char( "     number1 number2 motto desc\n\r", ch );
        send_to_char( "     claimleadership forfeitleadership\n\r", ch);
        return;
   }
   if ( !str_cmp( arg, "motto" ) )
   {
       STRFREE( clan->motto );
       clan->motto = STRALLOC( argument );
       send_to_char( "Done.\n\r", ch );
       save_clan( clan );
       return;
   }
   if ( !str_cmp( arg, "number1" ) )
   {
       if ( str_cmp( ch->name, clan->leader ) )
       {
          send_to_char( "You can't do that.\n\r", ch);
          return;
       }
       STRFREE( clan->number1 );
       clan->number1 = STRALLOC( argument );
       send_to_char( "Done.\n\r", ch );
       save_clan( clan );
       return;
   }
   if ( !str_cmp( arg, "claimleadership" ) )
   {
       if ( !clan->leader || clan->leader[0] == '\0' )
       {
         if ( !str_cmp( ch->name, clan->number1 ) )
         {
            send_to_char( "You have taken the leadership position.\n\r", ch);
            if ( clan->leader )
            STRFREE( clan->leader );
            if ( clan->number1 )
            STRFREE( clan->number1 );
            clan->leader = STRALLOC( ch->name );
            clan->number1 = STRALLOC( "" );
            return;
         }
       }
       send_to_char( "You can't do that.\n\r", ch);
       return;
   }
   if ( !str_cmp( arg, "forfeitleadership" ) )
   {
       if ( !clan->number1 || clan->number1[0] == '\0' )
       {
          send_to_char( "You must leave a possible successor!\n\r", ch);
          return;
       }
       if ( clan->leader || clan->leader[0] != '\0' )
       {
         if ( !str_cmp( ch->name, clan->leader ) )
         {
            STRFREE( clan->leader );
            clan->leader = STRALLOC( "" );
            return;
         }
       }
       send_to_char( "You can't do that.\n\r", ch);
       return;
   }

   if ( !str_cmp( arg, "number2" ) )
   {
       STRFREE( clan->number2 );
       clan->number2 = STRALLOC( argument );
       send_to_char( "Done.\n\r", ch );
       save_clan( clan );
       return;
   }
   if ( !str_cmp( arg, "desc" ) )
   {
       STRFREE( clan->description );
       clan->description = STRALLOC( argument );
       send_to_char( "Done.\n\r", ch );
       save_clan( clan );
       return;
   }
   do_clanset(ch, "");
}

void do_setclan( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CLAN_DATA *clan;

    set_char_color( AT_PLAIN, ch );
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setclan <clan> <field> <deity|leader|number1|number2> <player>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " deity leader number1 number2 shortname\n\r", ch ); 
	send_to_char( " members board storage jail guard1 guard2\n\r", ch );
	send_to_char( " gold level leadrank onerank tworank\n\r", ch );
	if ( get_trust( ch ) >= LEVEL_GOD )
	{
	  send_to_char( " name filename motto desc\n\r", ch );
	}
	if ( get_trust( ch ) >= LEVEL_IMPLEMENTOR )
	  send_to_char(" pkill1-7 pdeath1-7\n\r", ch );
	return;
    }

    clan = get_clan( arg1 );
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "deity" ) )
    {
	STRFREE( clan->deity );
	clan->deity = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "leader" ) )
    {
	STRFREE( clan->leader );
	clan->leader = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "number1" ) )
    {
	STRFREE( clan->number1 );
	clan->number1 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "number2" ) )
    {
	STRFREE( clan->number2 );
	clan->number2 = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "leadrank" ) )
    {
	STRFREE( clan->leadrank );
	clan->leadrank = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "onerank" ) )
    {
	STRFREE( clan->onerank );
	clan->onerank = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "tworank" ) )
    {
	STRFREE( clan->tworank );
	clan->tworank = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "badge" ) )
    {
	STRFREE( clan->badge );
	clan->badge = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "board" ) )
    {
	clan->board = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "level") )
    {
        clan->level = atoi( argument );
        send_to_char( "Done.\n\r", ch  );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "members" ) )
    {
	clan->members = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "gold" ) )
    {
      if ( ch->level < 65 )
      return;
        clan->gold = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }

    if ( !str_cmp( arg2, "storage" ) )
    {
	clan->storeroom = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "jail" ) )
    {
        clan->jail = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "guard1" ) )
    {
	clan->guard1 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "guard2" ) )
    {
	clan->guard2 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( get_trust( ch ) < LEVEL_GOD )
    {
	do_setclan( ch, "" );
	return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( clan->name );
	clan->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( clan->filename );
	clan->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	write_clan_list( );
	return;
    }
    if ( !str_cmp( arg2, "motto" ) )
    {
	STRFREE( clan->motto );
	clan->motto = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( !str_cmp( arg2, "shortname" ) )
    {
        STRFREE( clan->shortname );
        clan->shortname = STRALLOC( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( clan->description );
	clan->description = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }
    if ( get_trust( ch ) < LEVEL_IMPLEMENTOR )
    {
        do_setclan( ch, "" );
        return;
    }
    if ( !str_prefix( "pkill", arg2) )
    {
	int temp_value;
	if ( !str_cmp( arg2, "pkill1" ) )
		temp_value = 0;
	else if ( !str_cmp( arg2, "pkill2" ) )
		temp_value = 1;
	else if ( !str_cmp( arg2, "pkill3" ) )
		temp_value = 2;
	else if ( !str_cmp( arg2, "pkill4" ) )
		temp_value = 3;
	else if ( !str_cmp( arg2, "pkill5" ) )
		temp_value = 4;
	else if ( !str_cmp( arg2, "pkill6" ) )
		temp_value = 5;
	else if ( !str_cmp( arg2, "pkill7" ) )
		temp_value = 6;
	else
	{
		do_setclan ( ch, "" );
		return;
	}
	clan->pkills[temp_value] = atoi( argument );
	send_to_char ("Ok.\n\r", ch );
	return;
    }
    if ( !str_prefix( "pdeath", arg2) )
    {
	int temp_value;
	if ( !str_cmp( arg2, "pdeath1" ) )
		temp_value = 0;
	else if ( !str_cmp( arg2, "pdeath2" ) )
		temp_value = 1;
	else if ( !str_cmp( arg2, "pdeath3" ) )
		temp_value = 2;
	else if ( !str_cmp( arg2, "pdeath4" ) )
		temp_value = 3;
	else if ( !str_cmp( arg2, "pdeath5" ) )
		temp_value = 4;
	else if ( !str_cmp( arg2, "pdeath6" ) )
		temp_value = 5;
	else if ( !str_cmp( arg2, "pdeath7" ) )
		temp_value = 6;
	else
	{
		do_setclan ( ch, "" );
		return;
	}
	clan->pdeaths[temp_value] = atoi( argument );
	send_to_char ("Ok.\n\r", ch );
	return;
    }
    do_setclan( ch, "" );
    return;
}


/*
 * Added multiple levels on pkills and pdeaths. -- Shaddai
 */

void do_showclan( CHAR_DATA *ch, char *argument )
{   
    CLAN_DATA *clan;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showclan <clan>\n\r", ch );
	return;
    }

    clan = get_clan( argument );
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }

    ch_printf_color( ch, "\n\r&wClan    : &W%s\t\t&wBadge: %s\n\r&wFilename : &W%s\n\r&wMotto    : &W%s\n\r",
    			clan->name,
			clan->badge ? clan->badge : "(not set)",
    			clan->filename,
    			clan->motto );
    ch_printf_color( ch, "&wLevel    : &W%d\n\r", clan->level );
    ch_printf_color( ch, "&wShortname: &W%s\n\r", clan->shortname);
    ch_printf_color( ch, "&wDesc     : &W%s\n\r&wDeity    : &W%s\n\r",
    			clan->description,
    			clan->deity );
    ch_printf_color( ch, "&wLeader   : &W%-19.19s\t&wRank: &W%s\n\r",
			clan->leader,
			clan->leadrank );
    ch_printf_color( ch, "&wNumber1  : &W%-19.19s\t&wRank: &W%s\n\r",
			clan->number1,
			clan->onerank );
    ch_printf_color( ch, "&wNumber2  : &W%-19.19s\t&wRank: &W%s\n\r",
			clan->number2,
			clan->tworank );
    ch_printf_color( ch, "&wPKills   : &w1-9:&W%-3d &w10-14:&W%-3d &w15-19:&W%-3d &w20-29:&W%-3d &w30-39:&W%-3d &w40-49:&W%-3d &w50:&W%-3d\n\r",  
    			clan->pkills[0], clan->pkills[1], clan->pkills[2],
    			clan->pkills[3], clan->pkills[4], clan->pkills[5],
			clan->pkills[6]);
    ch_printf_color( ch, "&wPDeaths  : &w1-9:&W%-3d &w10-14:&W%-3d &w15-19:&W%-3d &w20-29:&W%-3d &w30-39:&W%-3d &w40-49:&W%-3d &w50:&W%-3d\n\r",  
    			clan->pdeaths[0], clan->pdeaths[1], clan->pdeaths[2],
    			clan->pdeaths[3], clan->pdeaths[4], clan->pdeaths[5],
			clan->pdeaths[6] );
    ch_printf_color( ch, "&wIllegalPK: &W%-6d\n\r",
			clan->illegal_pk );
    ch_printf_color( ch, "&wMKills   : &W%-6d   &wMDeaths: &W%-6d\n\r",
    			clan->mkills,
    			clan->mdeaths );
    ch_printf_color( ch, "&wMembers  : &W%-6d  &wGold: &W%-10lld\n\r",
    			clan->members,
                        clan->gold );
    send_to_char( "\n\r", ch );
    ch_printf_color( ch, "&wBoard    : &W%-5d    &wStorage: &W%-5d    &wJail: &W%-5d\n\r",
			clan->board,
			clan->storeroom,
                        clan->jail ); 
    ch_printf_color( ch, "&wGuard1   : &W%-5d    &wGuard2 : &W%-5d\n\r",
 			clan->guard1,
			clan->guard2 );
    return;
}


void do_makeclan( CHAR_DATA *ch, char *argument )
{
   CLAN_DATA *clan;

    set_char_color( AT_IMMORT, ch );

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeclan <clan name>\n\r", ch );
	return;
    }

    set_char_color( AT_PLAIN, ch );
    clan = get_clan( argument );
    if ( clan )
     {
	send_to_char( "There is already a clan with that name.\n\r", ch );
	return;
     }

    CREATE( clan, CLAN_DATA, 1 );
    LINK( clan, first_clan, last_clan, next, prev );

    clan->name		= STRALLOC( argument );
    /* Let's refix this, STRALLOC shouldn't be used for the 'filename'
       member without changing load_clan() and do_setclan() to employ hashstrings too... */
    clan->filename	= str_dup( "" );
    clan->motto		= STRALLOC( "" );
    clan->shortname     = STRALLOC( "" );
    clan->description	= STRALLOC( "" );
    clan->deity		= STRALLOC( "" );
    clan->leader	= STRALLOC( "" );
    clan->number1	= STRALLOC( "" );
    clan->number2	= STRALLOC( "" );
    clan->leadrank	= STRALLOC( "" );
    clan->onerank	= STRALLOC( "" );
    clan->tworank	= STRALLOC( "" );
    clan->badge		= STRALLOC( "" );
}

void do_createclan( CHAR_DATA *ch, char *argument )
{
   CLAN_DATA *clan;
   DOCK_DATA *dock;
   char clanname[MIL];
   char buf[MIL], buf2[MIL];
   char arg1[MIL];
   char arg2[MIL];
   char arg3[MIL];

    if ( IS_NPC(ch))
    {
        send_to_char( "Only players are allowed to create clans.\n\r", ch );
        return;
    }

    if ( !argument || argument[0] == '\0' )
    {
        send_to_char( "Usage: createclan <clan name>\n\r", ch );
        return;
    }
    if ( ch->pcdata->clan )
    {
        send_to_char( "You are already in a clan.\n\r", ch );
        return;
    }
    if ( ch->exp < 100000000 )
    {
        send_to_char( "You need to be stronger to be able to lead a clan.\n\r", ch);
        return;
    }
    if ( ch->gold < 250000000 )
    {
        send_to_char( "You need &Y250,000,000&w gold coins to create a clan.\n\r", ch);
        return;
    }
    if ( ch->pcdata->bless < 3 )
    {
        send_to_char( "You need atleast 3 blesses to prove your worthy of making a clan.\n\r", ch);
        return;
    }
    if ( strlen(argument) >= 18 )
    {
        send_to_char( "A clan name cannot be bigger than 18 characters.\n\r", ch);
        return;
    }
    clan = get_clan( argument );
    if ( clan )
    {
        send_to_char( "There is already a clan with that name.\n\r", ch );
        return;
    }
    sprintf( clanname, "%s", argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    if ( arg3[0] != '\0' )
    {
        send_to_char( "3 worded clans are illegal.\n\r", ch);
        return;
    }
    sprintf( arg1, "%s", strlower(arg1));
    sprintf( arg2, "%s", strlower(arg2));
    sprintf( buf, "%s%s", arg1, arg2 );
    sprintf( buf2, "%s%s.clan", arg1, arg2 );
    clan = get_clan_short( buf );
    if ( clan )
    {
        send_to_char( "There is already a clan with that name.\n\r", ch );
        return;
    }

    CREATE( clan, CLAN_DATA, 1 );
    LINK( clan, first_clan, last_clan, next, prev );
    CREATE( dock, DOCK_DATA, 1 );
    LINK( dock, first_dock, last_dock, next, prev );
    dock->onlyshuttle = FALSE;

    clan->name          = STRALLOC( clanname );
    /* Let's refix this, STRALLOC shouldn't be used for the 'filename'
       member without changing load_clan() and do_setclan() to employ hashstrings too... */
    clan->filename      = str_dup( buf2 );
    clan->motto         = STRALLOC( "" );
    clan->shortname     = STRALLOC( buf );
    clan->description   = STRALLOC( "" );
    clan->deity         = STRALLOC( "" );
    clan->leader        = STRALLOC( ch->name );
    clan->number1       = STRALLOC( "" );
    clan->number2       = STRALLOC( "" );
    clan->leadrank      = STRALLOC( "" );
    clan->onerank       = STRALLOC( "" );
    clan->tworank       = STRALLOC( "" );
    clan->badge         = STRALLOC( "" );
    clan->gold		= 100000000;
    sprintf( buf, "%s HQ", clan->name );
    dock->name = STRALLOC( buf );
    arg1[1] = 0;
    arg2[1] = 0;
    sprintf( buf, "%s%s_hq", arg1, arg2 );
    dock->coordname = STRALLOC( buf );
    dock->x = number_range( 0, 500 );
    dock->y = number_range( 0, 500 );
    dock->z = number_range( 0, 500 );
    dock->d = 0;
    dock->type = 2;
    while ( get_dockcoord( dock->x, dock->y, dock->z, dock->d ) != TRUE )
    {
      dock->x = number_range( 0, 500 );
      dock->y = number_range( 0, 500 );
      dock->z = number_range( 0, 500 );
      dock->d = 0;
    }
    dock->room = -1;
    save_docks();
    ch->gold -= 250000000;
    ch->pcdata->bless -= 3;
    ch->pcdata->clan = clan;
    save_clan( clan );
    write_clan_list();
    ch_printf(ch, "\n\r&YYou have created a new clan!\n\r&B250,000,000 gold coins and 3 blesses have dissappeared magically!\n\r");
    add_member(ch->name, clan->shortname);
    clan->members++;
    clan->level = 1;
    sprintf( buf, "&W%s &Ohas been created by &Y%s&O!&w\n\r", clanname, ch->name );
    clanchan( buf );
    do_save(ch, "" );
    return;
}




/*
 * Added multiple level pkill and pdeath support. --Shaddai
 */

void do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    FILE *www = NULL;
    char buf[MSL];

    if ( argument[0] == '\0' || !str_cmp( argument, "www" ))
    {
        if ( !ch )
        {
          unlink( WEBCLANS_FILE );
          www = fopen( WEBCLANS_FILE, "w" );
          if ( !www )
          {
            bug( "do_clans: cannot open clans web file!" );
            return;
          }
          fprintf( www, htmlcolor("\n\n&cClan                 Leader        Number1       Number2     Kills  Lvl  Members\n\r&z==============================================================================&W\n"));
        }
        else
        send_to_char("\n\r\n\r&cClan                 Leader        Number1       Number2     Kills  Lvl  Members\n\r&z=================================================================================\n\r", ch);
       

        for ( clan = first_clan; clan; clan = clan->next )
        {
           if (!str_cmp( clan->name, "None"))
           continue;
            sprintf( buf, "&w%-18.18s   %-10.10s    %-10.10s    %-10.10s  %-5d  %-1d    %-3d\n", clan->name, clan->leader, clan->number1, 
            clan->number2, (clan->pkills[6]+clan->pkills[2]+clan->pkills[3]+
	    clan->pkills[4]+clan->pkills[5]+clan->pkills[1]+clan->pkills[0]), clan->level, clan->members );
           if ( !ch )
             fprintf( www,  htmlcolor(buf)  );
           else
           send_to_char(buf, ch);
        }
          if ( !ch )
           fprintf( www, htmlcolor("&z=================================================================================" ));
          else
           send_to_char( "&z=================================================================================", ch );

          if ( ch )
          send_to_char("\n\r&WUse 'clans <clan>' for detailed information and a breakdown of victories.\n\r", ch );
          if ( !ch )
          fclose( www );
        return;
    }
    if ( !ch )
    {
       bug("Error in do_clans, null CH" );
       return;
    }    
    clan = get_clan( argument );
    if ( !clan  )
    {
        set_char_color( AT_BLOOD, ch );
        send_to_char( "No such clan.\n\r", ch );
        return;
    }
    set_char_color( AT_GREEN, ch );
    ch_printf( ch, "\n\r%s, '%s'\n\r\n\r", clan->name, clan->motto );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "&wLevel: &G%d\n\r", clan->level );
    send_to_char_color( "Victories:&w\n\r", ch );
   ch_printf_color( ch, "      &w5M-9M...  &G%-4d\n\r    &w10M-19M...  &G%-4d\n\r    &w20M-39M...  &G%-4d\n\r    &w40M-59M...  &G%-4d\n\r    &w60M-79M...  &G%-4d\n\r    &w80M-95M...  &G%-4d\n\r",
        clan->pkills[0],
        clan->pkills[1],
	clan->pkills[2],
	clan->pkills[3],
	clan->pkills[4],
	clan->pkills[5] );
    ch_printf_color( ch, "    &wSupreme...  &G%-4d\n\r", 
	clan->pkills[6] );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "Clan Leader:  %s\n\rNumber One :  %s\n\rNumber Two :  %s\n\r",
                        clan->leader,
                        clan->number1,
                        clan->number2);
    if ( !str_cmp( ch->name, clan->deity   )
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	ch_printf( ch, "Members    :  %d\n\r",
			clan->members );
        ch_printf( ch, "Gold       :  %d\n\r",
                        clan->gold);
    set_char_color( AT_GREEN, ch );
    ch_printf( ch, "\n\rDescription:  %s\n\r", clan->description );
    return;
}


void do_victories( CHAR_DATA *ch, char *argument )
{
    char filename[256]; 

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
        sprintf( filename, "%s%s.record",
          CLAN_DIR, ch->pcdata->clan->name );
	set_pager_color( AT_PURPLE, ch );
	if ( !str_cmp( ch->name, ch->pcdata->clan->leader )
	&&   !str_cmp( argument, "clean" ) )
        {
	  FILE *fp = fopen( filename, "w" );
	  if ( fp )
	    fclose( fp );
	  send_to_pager( "\n\rVictories ledger has been cleared.\n\r", ch );
	  return;
	}
        else
        {
	  send_to_pager( "\n\rLVL  Character       LVL  Character\n\r", ch );
	  show_file( ch, filename );
	  return;
	}
}


void do_shove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    bool nogo;
    ROOM_INDEX_DATA *to_room;    
    int chance = 0;
    int race_bonus = 0;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch)
    || !xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
    {
	send_to_char("Only deadly characters can shove.\n\r", ch);
	return;
    }

    if  ( get_timer(ch, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't shove a player right now.\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Shove whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch)
    {
	send_to_char("You shove yourself around, to no avail.\n\r", ch);
	return;
    }
    if ( IS_NPC(victim)
    || !xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
    {
	send_to_char("You can only shove deadly characters.\n\r", ch);
	return;
    }
    
    if ( ch->level - victim->level > 5 
    ||   victim->level - ch->level > 5 )
    {
	send_to_char("There is too great an experience difference for you to even bother.\n\r", ch);
	return;
    }

    if  ( get_timer(victim, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't shove that player right now.\n\r", ch);
	return;
    }

    if ( (victim->position) != POS_STANDING )
    {
	act( AT_PLAIN, "$N isn't standing up.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Shove them in which direction?\n\r", ch);
	return;
    }

    exit_dir = get_dir( arg2 );
    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&  get_timer(victim, TIMER_SHOVEDRAG) <= 0)
    {
	send_to_char("That character cannot be shoved right now.\n\r", ch);
	return;
    }
    victim->position = POS_SHOVE;
    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
        victim->position = POS_STANDING;
	return;
    }
    to_room = pexit->to_room;
    if (IS_SET(to_room->room_flags, ROOM_DEATH))
    {
      send_to_char("You cannot shove someone into a death trap.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }
    
    if (ch->in_room->area != to_room->area
    &&  !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }


/* Add 3 points to chance for every str point above 15, subtract for 
below 15 */

chance += ((get_curr_str(ch) - 15) * 3);

chance += (ch->level - victim->level);

if (ch->race == 1)
race_bonus = -3;

if (ch->race == 2)
race_bonus = 3;

if (ch->race == 3)
race_bonus = -5;

if (ch->race == 4)
race_bonus = -7;
 
if (ch->race == 6)
race_bonus = 5;
 
if (ch->race == 7)
race_bonus = 7;
 
if (ch->race == 8)
race_bonus = 10;
 
if (ch->race == 9)
race_bonus = -2;
 
chance += race_bonus;
 
/* Debugging purposes - show percentage for testing */

/* sprintf(buf, "Shove percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/

if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    act( AT_ACTION, "You shove $M.", ch, NULL, victim, TO_CHAR );
    act( AT_ACTION, "$n shoves you.", ch, NULL, victim, TO_VICT );
    move_char( victim, get_exit(ch->in_room,exit_dir), 0, exit_dir );
    if ( !char_died(victim) )
      victim->position = POS_STANDING;
    WAIT_STATE(ch, 12);
    /* Remove protection from shove/drag if char shoves -- Blodkai */
    if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)   
    &&   get_timer(ch, TIMER_SHOVEDRAG) <= 0 )
      add_timer( ch, TIMER_SHOVEDRAG, 10, NULL, 0 );
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    bool nogo;
    int chance = 0;
    int race_bonus = 0;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
    /* || !xIS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )  */
    {
	send_to_char("Only characters can drag.\n\r", ch);
	return;
    }

    if  ( get_timer(ch, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't drag a player right now.\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drag whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char("You take yourself by the scruff of your neck, but go nowhere.\n\r", ch);
	return; 
    }

    if ( IS_NPC(victim)  )
         /* || !xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) ) */
    {
	send_to_char("You can only drag characters.\n\r", ch);
	return;
    }

    if  ( get_timer(victim, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't drag that player right now.\n\r", ch);
	return;
    }

    if ( victim->fighting )
    {
        send_to_char( "You try, but can't get close enough.\n\r", ch);
        return;
    }
          
    if( !xIS_SET(ch->pcdata->flags, PCFLAG_DEADLY ) && xIS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) ){
	send_to_char("You cannot drag a deadly character.\n\r", ch);
	return;
    }

    if ( !xIS_SET(victim->pcdata->flags, PCFLAG_DEADLY) && victim->position > 3 )
    {
	send_to_char("They don't seem to need your assistance.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Drag them in which direction?\n\r", ch);
	return;
    }

    if ( ch->level - victim->level > 5
    ||   victim->level - ch->level > 5 )
    {
	if ( xIS_SET(victim->pcdata->flags, PCFLAG_DEADLY)
	&&   xIS_SET(ch->pcdata->flags, PCFLAG_DEADLY) )
	{
          send_to_char("There is too great an experience difference for you to even bother.\n\r", ch);
          return;
	}
    }

    exit_dir = get_dir( arg2 );

    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&   get_timer( victim, TIMER_SHOVEDRAG ) <= 0)
    {
	send_to_char("That character cannot be dragged right now.\n\r", ch);
	return;
    }

    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
	return;
    }

    to_room = pexit->to_room;
    if (IS_SET(to_room->room_flags, ROOM_DEATH))
    {
      send_to_char("You cannot drag someone into a death trap.\n\r", ch);
      return;
    }

    if (ch->in_room->area != to_room->area
    && !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }
    
/* Add 3 points to chance for every str point above 15, subtract for 
below 15 */

chance += ((get_curr_str(ch) - 15) * 3);

chance += (ch->level - victim->level);

if (ch->race == 1)
race_bonus = -3;

if (ch->race == 2)
race_bonus = 3;

if (ch->race == 3)
race_bonus = -5;

if (ch->race == 4)
race_bonus = -7;

if (ch->race == 6)
race_bonus = 5;

if (ch->race == 7)
race_bonus = 7;

if (ch->race == 8)
race_bonus = 10;

if (ch->race == 9)
race_bonus = -2;

chance += race_bonus;
/*
sprintf(buf, "Drag percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/
if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    if ( victim->position < POS_STANDING )
    {
	sh_int temp;

	temp = victim->position;
	victim->position = POS_DRAG;
	act( AT_ACTION, "You drag $M into the next room.", ch, NULL, victim, TO_CHAR ); 
	act( AT_ACTION, "$n grabs your hair and drags you.", ch, NULL, victim, TO_VICT ); 
	move_char( victim, get_exit(ch->in_room,exit_dir), 0, exit_dir);
	if ( !char_died(victim) )
	  victim->position = temp;
/* Move ch to the room too.. they are doing dragging - Scryn */
	move_char( ch, get_exit(ch->in_room,exit_dir), 0, exit_dir);
	WAIT_STATE(ch, 12);
	return;
    }
    send_to_char("You cannot do that to someone who is standing.\n\r", ch);
    return;
}

void do_members(CHAR_DATA *ch, char *argument)
{
 FILE *fpList;
 char *buf;
 char thebuf[MAX_STRING_LENGTH];
 char list[MAX_STRING_LENGTH];
 char color[MAX_STRING_LENGTH];
 char color2[MAX_STRING_LENGTH];
 char display[MAX_STRING_LENGTH];
 char prefix[MAX_STRING_LENGTH];
 int i=0;
 CLAN_DATA *clan;

 if(IS_NPC(ch)) return;

 if(!ch->pcdata->clan)
 {
   send_to_char("&RYou must first belong to a clan to see its members.\n\r", ch);
   return;
 }

 if(!ch->pcdata->clan->shortname)
 {
   send_to_char("&RYour clan doesn't have a shortname. tell an imm this.\n\r", ch);
   return;
 }

 clan = ch->pcdata->clan;

 sprintf(list, "%s%s.list", CLAN_DIR, clan->shortname);

 if ( ( fpList = fopen(list, "r" ) ) == NULL )
 {
    send_to_char("Something wen't wrong. The imms have been notified.\n\r", ch);
    bug("Do_members: Unable to open member list");
    return;
 }
   sprintf(color, "&G");
   sprintf(color2, "&W");


       sprintf(thebuf, "\n\r%s---------------============<%s%s%s>============---------------\n\r", color, color2, centertext(clan->name, 18), color);
       send_to_char(thebuf, ch);
       send_to_char( "\n\r", ch);
 for ( ; ; )
 {
     if(feof(fpList))
	break;
     buf = feof( fpList ) ? "End" : fread_string( fpList );
     if(!str_cmp(buf, "End") || buf[0] == '\0')
	break;
     if(strlen(buf) < 3)
	break;

     if( i % 4 == 0)
	send_to_char("", ch);

	sprintf(prefix, "   &w");

     sprintf(display, "&w%-15s", buf);
     send_to_char(display, ch);
     ++i;

     if( i % 4 == 0)
	send_to_char("\n\r", ch);

 }
 send_to_char( "\n\r", ch);
 sprintf(thebuf, "\n\r%s----------------------------------------------------------------------", color);
 send_to_char(thebuf, ch);
 send_to_char("\n\r", ch);
 fclose( fpList );
}

void add_member(char *name, char *shortname)
{
  char buf[MAX_STRING_LENGTH];
  char fbuf[MAX_STRING_LENGTH];

 if(name[0] == '\0' || !name)
 {
   bug("add_member: No name!\n\r");
   return;
 }

 if(shortname[0] == '\0' || !shortname)
 {
   bug("add_member: No shortname!\n\r");
   return;
 }

 sprintf(fbuf, "%s%s.list", CLAN_DIR, shortname);
 sprintf(buf, "%s~", name);
 append_to_file(fbuf, buf);

}

void remove_member(char *name, char *shortname)
{
 FILE *fpList;
 FILE *fpNew;
 char *buf;
 char list[MAX_STRING_LENGTH];
 char temp[MAX_STRING_LENGTH];

 if(name[0] == '\0')
 {
   bug("remove_member: No name!\n\r");
   return;
 }

 if(shortname[0] == '\0' || !shortname)
 {
   bug("remove_member: No shortname!\n\r");
   return;
 }

 sprintf(list, "%s%s.list", CLAN_DIR, shortname);
 sprintf(temp,"%s.temp", list);

 if ( ( fpList = fopen(list, "r" ) ) == NULL )
 {
    bug("Unable to open member list");
    return;
 }

 if ( ( fpNew = fopen(temp, "w" ) ) == NULL)
 {
    bug("remove_member: Unable to write temp list");
    return;
 }

 for ( ; ; )
 {
     if(feof(fpList))
        break;
     buf = feof( fpList ) ? "End" : fread_string( fpList );
     if(!str_cmp(buf, "End") || buf[0] == '\0')
        break;
     if(str_cmp(name, buf) && strlen(buf) > 2)
        fprintf(fpNew, "%s~\n", buf);
 }
 fclose( fpNew );
 fclose( fpList );
 rename(temp, list);
}

void do_imprison( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MIL];
  char buf[MSL];

  if IS_NPC(ch)
  return;

  if (!ch->pcdata->clan)
  {
    send_to_char( "Your not in a clan.\n\r", ch);
    return;
  }
  if (!ch->pcdata->clan->jail)
  {
    send_to_char( "Your clan doesnt have a Jail.\n\r", ch);
    return;
  }
  if ( ch->position == POS_SLEEPING || ch->position == POS_RESTING || ch->position == POS_STUNNED )
  {
    send_to_char( "You have to stand up first.\n\r", ch);
    return;
  }
  if (IS_SET( ch->in_room->area->flags, AFLAG_NOPKILL ))
  {
    send_to_char( "You can't do that here.\n\r", ch);
    return;
  }
  if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch);
    return;
  }
  if ( victim->position != POS_STUNNED )
  {
    send_to_char( "They aren't stunned.\n\r", ch);
    return;
  }
  if ( !victim->pcdata->clan )
  {
    send_to_char( "You may only imprison clanned members.\n\r", ch);
    return;
  }
  if ( victim->pcdata->clan == ch->pcdata->clan )
  {
    send_to_char( "They are in your clan.\n\r", ch);
    return;
  }
  ch_printf( ch, "You imprison %s.\n\r", victim->name);
  ch_printf( victim, "%s imprisons you.\n\r", ch->name);
  sprintf( buf, "&R%s imprisons %s." , ch->name, victim->name);
  act( AT_WHITE, buf, ch, NULL, victim, TO_NOTVICT );
  char_from_room( victim );
  char_to_room( victim, get_room_index( ch->pcdata->clan->jail) );
  return;
}

void  do_retrieve( CHAR_DATA *ch, char *argument )
{
  int gold;
  CLAN_DATA *clan;

  if (!ch->pcdata->clan)
  {
     send_to_char( "You aren't in a clan.\n\r", ch);
     return;
  }
  clan = ch->pcdata->clan;
  gold = atoi(argument);
  if ( clan->number1 != ch->name && clan->number2 != ch->name && clan->leader != ch->name && clan->deity != ch->name)
  {
     send_to_char( "Huh?\n\r", ch);
     return;
  }
  if ( argument[0] == '\0' )
  {
     send_to_char( "Retrieve how much gold?\n\r", ch );
     return;
  }
  if ( gold > 10000000 )
  {
    send_to_char( "Thats too much.\n\r", ch);
    return;
 }
 if ( gold >= clan->gold )
 {
    send_to_char( "Your clan doesn't have that much.\n\r", ch);
    return;
 }
 clan->gold -= gold;
 ch->gold += gold;
 ch_printf( ch, "You retrieve %d gold from your clan.\n\r", gold);
}

void do_addroom( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA     *location, *location2, *wasinroom, *area;
    EXIT_DATA           *xit;
    CLAN_DATA		*clan, *clan2;
    AREA_DATA           *pArea;
    int                  vnum, room_count, room_hold[MAX_RGRID_ROOMS], edir, x;


    area = get_room_index( 12000 );
    pArea = area->area;
    location2 = ch->in_room;

    if ( IS_NPC(ch))
    {
         send_to_char( "&RMobs can't use this command\n\r&w", ch);
         return;
    }
    if ( !ch->pcdata->clan )
    {
         send_to_char( "&RYou don't even HAVE a clan!\n\r&w", ch);
         return;
    }
    clan = get_clan_short( location2->clan );
    if ( !clan )
    {
         send_to_char( "&RYou can't do this here!\n\r&w", ch);
         return;
    }
    if ( !argument || argument[0] == '\0' )
    {
        send_to_char( "Usage: AddRoom <Direction>\n\r", ch );
        return;
    }
    clan2 = ch->pcdata->clan;
    if ( clan != clan2 )
    {
         send_to_char( "&RYour standing inside another clan's base.\n\r&w", ch);
         return;
    }

    edir = get_dir( argument );
    xit = get_exit( location2, edir );
    x = 1;
    if ( str_cmp( ch->name, clan->leader ) )
    {
         send_to_char( "&RYou aren't a leader!\n\r&w", ch);
         return;
    }
    if ( get_room_limit(clan2->level) > 0 && clan2->roomcount > get_room_limit(clan2->level))
    {
        send_to_char( "&RYou have reached the maximum of rooms allowed for your base.\n\r", ch );
        return;
    }

    if ( clan2->gold <  4000000 )
    {
         send_to_char( "&RIt costs 4,000,000 gold coins to create a new room in your base!\n\r&w", ch);
         return;
    }
    if (!pArea)
    {
        send_to_char( "&RSomething went wrong, contact an Immortal.\n\r", ch );
        return;
    }
    if ( xit )
    {
        send_to_char( "There is already a room in that direction.\n\r", ch );
        return;
    }
    room_count = 0;
    if(pArea->low_r_vnum + x > pArea->hi_r_vnum)
    {

      send_to_char( "Area is full.\r\n", ch);
      return;
    }
    for(vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++)
    {

      if(get_room_index(vnum) == NULL)
        room_count++;

      if(room_count >= x)
        break;
    }
    if(room_count < x)
    {
      send_to_char( "There aren't enough free rooms in your assigned range!\r\n", ch);
      return;
    }
    send_to_char( "Creating the rooms...\r\n", ch);
    room_count = 0;
    vnum = pArea->low_r_vnum;


    while(room_count < x)
    {
      if(get_room_index(vnum) == NULL)
      {
        ch->pcdata->area = ch->in_room->area;
        room_hold[room_count++] = vnum;
        command( ch, do_goto, "%d", vnum);
        location = ch->in_room;
        REMOVE_BIT( location->room_flags, ROOM_PROTOTYPE );
        location->clan = STRALLOC( ch->pcdata->clan->shortname );
        do_regoto(ch, "");
        if ( !location )
        {
          bug( "%s: make_room failed", __FUNCTION__ );
          return;
        }
        fold_area( location->area, location->area->filename, FALSE, FALSE );
        ch->pcdata->area = NULL;
      }
      vnum++;
    }
    vnum -= 1;
    wasinroom = ch->in_room;
    clan2->roomcount++;
    ch_printf( ch, "&RYou have created a new room for your base.\r\n", room_count);
    create_room( ch, argument, vnum );
    clan2->gold -= 4000000;
    do_save( ch , "" );
   return;
}


void do_deleteroom( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA     *location, *location2, *area;
    EXIT_DATA           *xit, *pexit;
    CLAN_DATA           *clan, *clan2;
    AREA_DATA           *pArea;
    int                  count, edir;
    char		buf[MSL];
    area = get_room_index( 12000 );
    pArea = area->area;
    location2 = ch->in_room;

    if ( IS_NPC(ch))
    {
         send_to_char( "&RMobs can't use this command\n\r&w", ch);
         return;
    }
    if ( !ch->pcdata->clan )
    {
         send_to_char( "&RYou don't even HAVE a clan!\n\r&w", ch);
         return;
    }
    clan = get_clan_short( location2->clan );
    if ( !clan )
    {
         send_to_char( "&RYou can't do this here!\n\r&w", ch);
         return;
    }
    if ( !argument || argument[0] == '\0' )
    {
        send_to_char( "Usage: RemoveRoom <Direction>\n\r", ch );
        return;
    }
    clan2 = ch->pcdata->clan;
    if ( clan != clan2 )
    {
         send_to_char( "&RYour standing inside another clan's base.\n\r&w", ch);
         return;
    }
    edir = get_dir( argument );
    xit = get_exit( location2, edir );
    if ( str_cmp( ch->name, clan->leader ) )
    {
         send_to_char( "&RYou aren't a leader!\n\r&w", ch);
         return;
    }
    if (!pArea)
    {
        send_to_char( "&RSomething went wrong, contact an Immortal.\n\r", ch );
        return;
    }
    if ( !xit )
    {
        send_to_char( "There is no room in that direction.\n\r", ch );
        return;
    }
    if ( IS_SET(xit->exit_info, EX_OVERLAND))
    {
        send_to_char( "You can't delete exits to the outside world.\n\r", ch );
        return;
    }
    if ( IS_SET(xit->exit_info, EX_CLOSED) || IS_SET(xit->exit_info, EX_ISDOOR))
    {
        send_to_char( "You must remove the door first.\n\r", ch );
        return;
    }

    send_to_char( "Deleting room...\r\n", ch);
    location = xit->to_room;
    for ( count = 0, pexit = location->first_exit; pexit; pexit = pexit->next )
     count++;

    if ( count > 1 )
    {
       send_to_char( "That room has other rooms attached to it, delete them first.\n\r", ch);
       return;
    }
    extract_exit(location2, xit);
    sprintf( buf, "%d", xit->to_room->vnum );
    do_rdelete( ch, buf );
    clan2->roomcount--;
    ch_printf( ch, "&RYou have removed a room of your base.\r\n");

    clan2->gold += 3000000;
    do_save( ch , "" );
   return;
}


void add_doorreset( ROOM_INDEX_DATA *room, EXIT_DATA *pexit )
{
  int state = 0;

  if( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
  return;
  if( IS_SET( pexit->exit_info, EX_CLOSED ) )
  {
      if( IS_SET( pexit->exit_info, EX_LOCKED ) )
         state = 2;
      else
         state = 1;
   }

   if ( !room )
   return;
   
   add_reset( room, 'D', 0, room->vnum, pexit->vdir, state );
   return;
}

void rem_doorreset( ROOM_INDEX_DATA *room, sh_int dir )
{
  RESET_DATA *pReset, *pReset_next;

   for( pReset = room->first_reset; pReset; pReset = pReset_next )
   {
      pReset_next = pReset->next;

      switch( pReset->command )
      {
         default:
           continue;
         case 'D':
            if ( pReset->arg2 == dir )
            {
              UNLINK( pReset, room->first_reset, room->last_reset, next, prev );
              delete_reset( pReset );
              log_string( "Deleted." );
            }
            break;
      }
   }
    return;
}


void do_buildhq( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA     *location;
    CLAN_DATA           *clan, *clan2;
    char	arg1[MSL];

    location = ch->in_room;
    argument = one_argument( argument, arg1 );

    switch( ch->substate )
    {
        default:
          break;
        case SUB_ROOM_DESC:
          location = ch->dest_buf;
          if ( !location )
          {
                bug( "redit: sub_room_desc: NULL ch->dest_buf", 0 );
                location = ch->in_room;
          }
          STRFREE( location->description );
          location->description = copy_buffer( ch );
          stop_editing( ch );
          ch->substate = ch->tempnum;
          return;
    }

    if ( IS_NPC(ch) || !ch->pcdata->clan)
    {
         send_to_char( "&wHuh?\n\r", ch);
         return;
    }
    if ( !location->clan || (clan = get_clan_short( location->clan )) == NULL )
    {
         send_to_char( "\n\r&RYou can't do this here!\n\r&w", ch);
         return;
    }
    clan2 = ch->pcdata->clan;
    if ( str_cmp( ch->name, clan2->leader ) )
    {
         send_to_char( "&wHuh?\n\r", ch);
         return;
    } 
    if ( !arg1 || arg1[0] == '\0' )
    {
        send_to_char( "\n\rbuildhq name [new room name]\n\r", ch );
        send_to_char( "        desc\n\r", ch );
//        send_to_char( "        claimhq\n\r", ch);
        send_to_char( "        addroom [direction]\n\r", ch );
        send_to_char( "        remroom [direction]\n\r", ch );
        send_to_char( "        addflag [flag]\n\r", ch );
        send_to_char( "        remflag [flag]\n\r", ch );
        send_to_char( "        adddoor [direction]\n\r", ch );
        send_to_char( "        remdoor [direction]\n\r", ch );
        send_to_char( "        addkeypad [direction]\n\r", ch );
        send_to_char( "        remkeypad [direction]\n\r", ch );
        send_to_char( "        changecode [direction]\n\r", ch );         
        return;
    }
    if ( clan != clan2 && str_cmp(arg1, "claimhq"))
    {
         send_to_char( "\n\r&RYour standing inside another clan's base.\n\r&w", ch);
         return;
    }
    if ( !str_cmp( arg1, "name" ) )
    {
        if ( argument[0] == '\0' )
        {
           send_to_char( "\n\rSet the room name.  A very brief single line room description.\n\r", ch );
           send_to_char( "Usage: Buildhq name <Room summary>\n\r", ch );
           return;
        }
        if ( clan->gold < 10000 )
        {
           send_to_char( "Not enough clan gold.\n\r", ch );
           return;
        }
        send_to_char("\n\rRoom renamed.\n\r", ch);
        clan->gold -= 10000;
        STRFREE( location->name );
        location->name = STRALLOC( argument );
        return;
    }
    if ( !str_cmp( arg1, "desc" ) )
    {
        if ( clan->gold < 15000 )
        {
           send_to_char( "\n\rNot enough clan gold.\n\r", ch );
           return;
        }
        clan->gold -= 15000;
        ch->tempnum = SUB_NONE;
        ch->substate = SUB_ROOM_DESC;
        ch->dest_buf = location;
        start_editing( ch, location->description );
        return;
    }
/*    if ( !str_cmp( arg1, "claimhq" ) )
    {
      if ( clan2->hasbase > 0 )
      {
          send_to_char( "You already have a base.\n\r", ch);
          return;
      }
      if ( !IS_SET(location->room_flags2, ROOM_CLAIMABLE) )
      {
          send_to_char( "You can't do that here.\n\r", ch);
          return;
      }
      if ( location->clan && location->clan[0] != '\0' && str_cmp( location->clan, "none" ) )
      {
          send_to_char( "This base is already owned.\n\r", ch);
          return;
      }
      clan2->hasbase = 1;
      location->clan = STRALLOC( clan2->shortname );
      send_to_char( "You have claimed this base as your own, you may begin to modify it.\n\r", ch);

    }*/
    if ( !str_cmp( arg1, "addroom" ) )
    {
       do_addroom( ch, argument );
       return;
    }
    if ( !str_cmp( arg1, "remroom" ) )
    {
       do_deleteroom( ch, argument );
       return;
    }
    if ( !str_cmp( arg1, "adddoor" ) )
    {
       sh_int edir;
       EXIT_DATA *xit, *xit2;
 
       edir = get_dir( argument );
       if ( edir < 0 )
       {
            send_to_char( "&RInvalid direction.\n\r&w", ch);
            return;
       }

       xit = get_exit( location, edir );
       xit2 = get_exit( xit->to_room, rev_dir[xit->vdir] );
       if ( !xit )
       {
            send_to_char( "\n\rNo exit in that direction.\n\r", ch );
            return;
       }
       if ( !xit2 )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }

       if ( IS_SET( xit->exit_info, EX_ISDOOR) || IS_SET( xit->exit_info, EX_CLOSED) || IS_SET( xit->exit_info, EX_KEYPAD) )
       {
            send_to_char( "\n\rThere is already a door in that direction.\n\r", ch );
            return;
       }
       if ( clan2->doorcount >= get_door_limit(clan2->level) )
       {
            send_to_char( "\n\rYou have reached the clan's door maximum.\n\r", ch );
            return;
       }
       if ( clan2->gold < 5000000 )
       {
            send_to_char( "\n\rYour clan doesn't have enough gold.\n\r", ch );
            return;
       }
       clan2->gold -= 5000000;
       send_to_char( "\n\rThe door has been added.\n\r",ch);
       SET_BIT( xit->exit_info, EX_CLOSED );
       SET_BIT( xit->exit_info, EX_ISDOOR );
       SET_BIT( xit2->exit_info, EX_CLOSED );
       SET_BIT( xit2->exit_info, EX_ISDOOR );

       add_doorreset( ch->in_room, xit );
       add_doorreset( xit->to_room, xit2 );

       clan2->doorcount++;
       return;
    }
    if ( !str_cmp( arg1, "remdoor" ) )
    {
       sh_int edir;
       EXIT_DATA *xit, *xit2;

       edir = get_dir( argument );
       if ( edir < 0 )
       {
            send_to_char( "&RInvalid direction.\n\r&w", ch);
            return;
       }
       xit = get_exit( location, edir );
       xit2 = get_exit( xit->to_room, rev_dir[xit->vdir] );
       if ( !xit )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }
       if ( !xit2 )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }

       if ( !IS_SET( xit->exit_info, EX_ISDOOR))
       {
            send_to_char( "There is no door in that direction.\n\r", ch );
            return;
       }
       send_to_char( "The door has been removed.\n\r", ch);
       clan2->gold += 2500000;
       clan2->doorcount--;
       REMOVE_BIT( xit->exit_info, EX_CLOSED );
       REMOVE_BIT( xit->exit_info, EX_ISDOOR );
       REMOVE_BIT( xit2->exit_info, EX_CLOSED );
       REMOVE_BIT( xit2->exit_info, EX_ISDOOR );

       rem_doorreset( ch->in_room, xit->vdir );
       rem_doorreset( xit->to_room, rev_dir[xit->vdir] );

       return;
    }
    if ( !str_cmp( arg1, "addkeypad" ) )
    {
       sh_int edir;
       EXIT_DATA *xit, *xit2;

       edir = get_dir( argument );
       if ( edir < 0 )
       {
            send_to_char( "&RInvalid direction.\n\r&w", ch);
            return;
       }

       xit = get_exit( location, edir );
       xit2 = get_exit( xit->to_room, rev_dir[xit->vdir] );

       if ( !xit )
       {
            send_to_char( "\n\rNo exit in that direction.\n\r", ch );
            return;
       }
       if ( !xit2 )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }

       if ( IS_SET( xit->exit_info, EX_ISDOOR) || IS_SET( xit->exit_info, EX_CLOSED) || IS_SET( xit->exit_info, EX_KEYPAD) )
       {
            send_to_char( "\n\rThere is already a door in that direction.\n\r", ch );
            return;
       }
       if ( clan2->doorcount >= get_door_limit(clan2->level) )
       {
            send_to_char( "\n\rYou have reached the clan's door maximum.\n\r", ch );
            return;
       }
       if ( clan2->gold < 6000000 )
       {
            send_to_char( "\n\rYour clan doesn't have enough gold.\n\r", ch );
            return;
       }
       clan2->doorcount++;
       clan2->gold -= 6000000;
       xit->key = 0;
       send_to_char( "\n\rKeypad Door has been added.\n\rConfigure it with Changecode.\n\r",ch);

       SET_BIT( xit->exit_info, EX_CLOSED );
       SET_BIT( xit->exit_info, EX_ISDOOR );
       SET_BIT( xit->exit_info, EX_KEYPAD );
       SET_BIT( xit->exit_info, EX_LOCKED );
       SET_BIT( xit2->exit_info, EX_CLOSED );
       SET_BIT( xit2->exit_info, EX_ISDOOR );
       SET_BIT( xit2->exit_info, EX_KEYPAD );
       SET_BIT( xit2->exit_info, EX_LOCKED );
       add_doorreset( ch->in_room, xit );
       add_doorreset( xit->to_room, xit2 );

       return;
    }
    if ( !str_cmp( arg1, "remkeypad" ) )
    {
       sh_int edir;
       EXIT_DATA *xit, *xit2;

       edir = get_dir( argument );
       if ( edir < 0 )
       {
            send_to_char( "&RInvalid direction.\n\r&w", ch);
            return;
       }

       xit = get_exit( location, edir );
       xit2 = get_exit( xit->to_room, rev_dir[xit->vdir] );
       if ( !xit )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }
       if ( !xit2 )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }

       if ( !IS_SET( xit->exit_info, EX_ISDOOR) && !IS_SET( xit->exit_info, EX_KEYPAD))
       {
            send_to_char( "There is no door in that direction.\n\r", ch );
            return;
       }
       clan2->doorcount--;
       send_to_char( "The keypad door has been removed.\n\r", ch);
       xit->key = 0;
       clan2->gold += 3000000;
       REMOVE_BIT( xit->exit_info, EX_CLOSED );
       REMOVE_BIT( xit->exit_info, EX_ISDOOR );
       REMOVE_BIT( xit->exit_info, EX_KEYPAD );
       REMOVE_BIT( xit->exit_info, EX_LOCKED );
       REMOVE_BIT( xit2->exit_info, EX_CLOSED );
       REMOVE_BIT( xit2->exit_info, EX_ISDOOR );
       REMOVE_BIT( xit2->exit_info, EX_KEYPAD );
       REMOVE_BIT( xit2->exit_info, EX_LOCKED );
       rem_doorreset( ch->in_room, xit->vdir );
       rem_doorreset( xit->to_room, rev_dir[xit->vdir] );


       return;
    }
    if ( !str_cmp( arg1, "changecode" ) )
    {
       sh_int edir;
       EXIT_DATA *xit, *xit2;
       char arg2[MSL];
       char arg3[MSL];
       argument = one_argument( argument, arg2 );
       argument = one_argument( argument, arg3 );
 
       edir = get_dir( arg2 );
       if ( edir < 0 )
       {
            send_to_char( "&RInvalid direction.\n\r&w", ch);
            return;
       }

       xit = get_exit( location, edir );
       xit2 = get_exit( xit->to_room, rev_dir[xit->vdir] ); 
       if ( !xit )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }
       if ( !xit2 )
       {
            send_to_char( "No exit in that direction.\n\r", ch );
            return;
       }

       if ( !IS_SET( xit->exit_info, EX_KEYPAD))
       {
            send_to_char( "There is no keypad in that dierction.\n\r", ch );
            return;
       }
       if ( clan2->gold < 1000000 )
       {
            send_to_char( "\n\rYour clan doesn't have enough gold.\n\r", ch );
            return;
       }
       if ( strlen(arg3) != 5 )
       {
            send_to_char( "buildhq changecode [direction] #####\n\r", ch );
            return;
       }
       send_to_char( "The keypad code has been reconfigured.\n\r", ch);
       xit->key = atoi( arg3 );
       xit2->key = atoi( arg3 );
       clan2->gold -= 1000000;
       return;
    }
    if ( !str_cmp( arg1, "addflag" ) )
    {
       if ( !str_cmp( argument, "gravity" ) )
       {
         if ( clan2->level < 3 )
         {
            send_to_char( "\n\rYou can't add a gravitron because your clan level is not high enough.\n\r", ch);
	    return;
         }
         if ( IS_SET(location->room_flags2, ROOM_GRAVITY) )
         {
            send_to_char( "\n\rThis room is already contains a gravitron.\n\r", ch);
	    return;
         }
         if ( clan->gold < 10000000 )
         {
            send_to_char( "\n\rNot enough clan gold.\n\r", ch );
            return;
         }
         clan->gold -= 10000000;
         SET_BIT(location->room_flags2, ROOM_GRAVITY);
         send_to_char("\n\rYou have added a gravitron to this room.\n\r", ch);
         return;
       }
       if ( !str_cmp( argument, "heal" ) )
       {
         if ( clan2->level < 4 )
         {
            send_to_char( "\n\rYou can't add a gravitron because your clan level is not high enough.\n\r", ch);
	    return;
         }
         if ( IS_SET(location->room_flags2, ROOM_HEAL) )
         {
            send_to_char( "\n\rThis room is already contains healing pods.\n\r", ch);
	    return;
         }
         if ( clan->gold < 10000000 )
         {
            send_to_char( "\n\rNot enough clan gold.\n\r", ch );
            return;
         }
         clan->gold -= 10000000;
         SET_BIT(location->room_flags2, ROOM_HEAL);
         send_to_char("\n\rYou have added healing pods to this room.\n\r", ch);
         return;
       }
       if ( !str_cmp( argument, "vault" ) )
       {
         if ( clan2->level < 5 )
         {
            send_to_char( "\n\rYou can't add a gravitron because your clan level is not high enough.\n\r", ch);
	    return;
         }
         if ( clan2->storeroom != 0 )
         {
            send_to_char( "\n\rYou already have a vault.\n\r", ch);
	    return;
         }
         if ( IS_SET(location->room_flags, ROOM_CLANSTOREROOM) )
         {
            send_to_char( "This room is already a vault.\n\r", ch);
	    return;
         }
         if ( clan->gold < 10000000 )
         {
            send_to_char( "Not enough clan gold.\n\r", ch );
            return;
         }
         clan->gold -= 10000000;
         clan2->storeroom = ch->in_room->vnum;
         SET_BIT(location->room_flags, ROOM_CLANSTOREROOM);
         send_to_char("You have re-inforced all the walls in order to make this room a vault.\n\r", ch);
         return;
       }
    }
    if ( !str_cmp( arg1, "remflag" ) )
    {
       if ( !str_cmp( argument, "gravity" ) )
       {
         if ( !IS_SET(location->room_flags2, ROOM_GRAVITY) )
         {
            send_to_char( "This room doesn't have a gravitron.\n\r", ch);
	    return;
         }
         if ( clan2->level < 3 )
         {
            send_to_char( "You can't remove the gravitron because your clan level is not high enough.\n\r", ch);
	    return;
         }
         clan->gold += 5000000;
         REMOVE_BIT(location->room_flags2, ROOM_GRAVITY);
         send_to_char("You have removed the gravitron of this room.\n\r", ch);
         return;
       }
       if ( !str_cmp( argument, "heal" ) )
       {
         if ( !IS_SET(location->room_flags2, ROOM_HEAL) )
         {
            send_to_char( "This room doesn't have healing pods.\n\r", ch);
	    return;
         }
         if ( clan2->level < 4 )
         {
            send_to_char( "You can't remove the healing pods because your clan level is not high enough.\n\r", ch);
	    return;
         }
         clan->gold += 5000000;
         REMOVE_BIT(location->room_flags2, ROOM_HEAL);
         send_to_char("You have removed the healing pods of this room.\n\r", ch);
         return;
       }
       if ( !str_cmp( argument, "vault" ) )
       {
         if ( clan2->storeroom == 0 )
         {
            send_to_char( "You don't even have a vault.\n\r", ch);
	    return;
         }
         if ( !IS_SET(location->room_flags, ROOM_CLANSTOREROOM) )
         {
            send_to_char( "This room doesn't seem to be re-inforced with steel.\n\r", ch);
	    return;
         }
         if ( clan2->level < 5 )
         {
            send_to_char( "You can't remove the vault because your clan level is not high enough.\n\r", ch);
	    return;
         }
         clan->gold += 5000000;
         REMOVE_BIT(location->room_flags, ROOM_CLANSTOREROOM);
         send_to_char("You have removed the vault.\n\r", ch);
         clan2->storeroom = 0;
         return;
       }
    }
}




void do_clanstatus ( CHAR_DATA *ch, char *argument )
{
	CLAN_DATA *clan;

   if ( IS_NPC(ch) )
   {
       send_to_char( "Huh?\n\r", ch);
       return;
   }
   if ( (clan = ch->pcdata->clan) == NULL )
   {
       send_to_char( "Huh?\n\r", ch);
       return;
   }




	ch_printf( ch, "\n\r");
	ch_printf( ch, "&z+-------------------------------------------------------+\n\r" );
	ch_printf( ch, "&z|                      &RClan Status&z                      |\n\r" );
	ch_printf( ch, "&z+-------------------------------------------------------+\n\r" );
	ch_printf( ch, "&z|       &WName&w: %-41.41s&z |\n\r", clan->name );
	ch_printf( ch, "&z|      &WMotto&w: %-41.41s&z |\n\r", clan->motto );
	ch_printf( ch, "&z|     &WLeader&w: %-15.15s   &WNumber1&w: %-15.15s&z|\n\r", clan->leader, clan->number1 );
	ch_printf( ch, "&z|    &WNumber2&w: %-15.15s&z                           |\n\r", clan->number2 );
	ch_printf( ch, "&z+-------------------------------------------------------+\n\r" );
	ch_printf( ch, "&z|    &WMembers&w: %-5d&W               Level&w: %-5d&z          |\n\r", clan->members, clan->level ); 
	ch_printf( ch, "&z|     &WPKills&w: %-5d&W             PDeaths&w: %-5d&z          |\n\r",(clan->pkills[6]+clan->pkills[2]+clan->pkills[3]+
                                                                                     clan->pkills[4]+clan->pkills[5]+clan->pkills[1]+
                                                                                     clan->pkills[0]),
                                                                                    (clan->pdeaths[6]+clan->pdeaths[2]+clan->pdeaths[3]+
                                                                                     clan->pdeaths[4]+clan->pdeaths[5]+clan->pdeaths[1]+
                                                                                     clan->pdeaths[0]) );
	ch_printf( ch, "&z|&W     MKills&w: %-5d&W             MDeaths&w: %-5d&z          |\n\r", clan->mkills, clan->mdeaths );
	ch_printf( ch, "&z|&W  Mem_Limit&w: %-5d&W          Room_Limit&w: %-5d&z          |\n\r", get_mem_limit( clan->level ), get_room_limit( clan->level) );
        ch_printf( ch, "&z|&W Door_Limit&w: %-5d&W         Guard_Limit&w: %-5d&z          |\n\r", get_door_limit(clan->level), get_guard_limit( clan->level) );
        ch_printf( ch, "&z|&W       Gold&w: %-41.41s&z |\n\r", num_punct(clan->gold));
      if ( !str_cmp( ch->name, clan->leader ) || !str_cmp( ch->name, clan->deity ) )
      {
	ch_printf( ch, "&z+-------------------------------------------------------+\n\r" );
	ch_printf( ch, "&z|  &WRoomcount&w: %-5d          &WGuardcount&w: %-5d&z          |\n\r", clan->roomcount, clan->guardcount );
        ch_printf( ch, "&z|  &WDoorcount&w: %-5d                                     &z|\n\r", clan->doorcount);
	ch_printf( ch, "&z+-------------------------------------------------------+\n\r" );
      }

}
