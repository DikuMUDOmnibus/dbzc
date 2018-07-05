/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2002 Alsherok. Contributors: Samson, Dwip, Whir,   *
 * Cyberfox, Karangi, Rathian, Cam, Raine, and Tarl.                        *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *                        Finger and Wizinfo Module                         *
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

/******************************************************
        Additions and changes by Edge of Acedia
              Rewritten do_finger to better
             handle info of offline players.
           E-mail: nevesfirestar2002@yahoo.com
 ******************************************************/ 

#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "mud.h"

#if defined(KEY)
#undef KEY
#endif


bool    check_parse_name        args( ( char *name, bool newchar ) );

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				      field = value;			\
				      fMatch = TRUE;			\
				      break;				\
				}



/* Finger snippet courtesy of unknown author. Installed by Samson 4-6-98 */
/* File read/write code redone using standard Smaug I/O routines - Samson 9-12-98 */
/* Data gathering now done via the pfiles, eliminated separate finger files - Samson 
12-21-98 */
/* Improvements for offline players by Edge of Acedia 8-26-03 */
/* Further refined by Samson on 8-26-03 */
void do_finger( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim = NULL;
   CMDTYPE *command;
   ROOM_INDEX_DATA *temproom, *original = NULL;
   int level = LEVEL_IMMORTAL;
   char buf[MAX_STRING_LENGTH], fingload[MAX_INPUT_LENGTH];
   struct stat fst;
   bool loaded = FALSE, skip = FALSE;

   if( IS_NPC(ch) )
   {
      send_to_char( "Mobs can't use the finger command.\n\r", ch );
      return;
   }
   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Finger whom?\n\r", ch );
      return;
   }

   sprintf( buf, "0.%s", argument );
  
   /* If player is online, check for fingerability (yeah, I coined that one)	-Edge */       
   if( ( victim = get_char_world( ch, buf ) ) != NULL )
   {
	if( xIS_SET( victim->pcdata->flags, PCFLAG_PRIVACY ) && !IS_IMMORTAL(ch) )
    	{
	   ch_printf( ch, "%s has privacy enabled.\n\r", victim->name );
	   return;
    	}

    	if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    	{
	   send_to_char( "You cannot finger an immortal.\n\r", ch );
	   return;
    	}
        if ( ch->level < victim->level )
        {
          send_to_char("&RYou cannot fingerfile someone above you.", ch);
          return;
        }
   }
   /* Check for offline players - Edge */
   else
   {
      DESCRIPTOR_DATA *d;

      sprintf( fingload, "%s%c/%s", PLAYER_DIR, tolower(argument[0]), capitalize( argument ) );
      /* Bug fix here provided by Senir to stop /dev/null crash */
      if( stat( fingload, &fst ) == -1 || !check_parse_name( capitalize( argument ), FALSE ) )
      {
         ch_printf( ch, "&YNo such player named '%s'.\n\r", argument );
         return;
      }

//      laston = ctime( &fst.st_mtime );
      temproom = get_room_index( ROOM_VNUM_LIMBO );
      if( !temproom )
      {
         bug( "%s", "do_finger: Limbo room is not available!" );
         send_to_char( "Fatal error, report to the immortals.\n\r", ch );
         return;
      }

	CREATE( d, DESCRIPTOR_DATA, 1 );
	d->next = NULL;
	d->prev = NULL;
	d->connected = CON_GET_NAME;
	d->outsize = 2000;
	CREATE( d->outbuf, char, d->outsize );
      argument[0] = UPPER( argument[0] );

	loaded = load_char_obj( d, argument, FALSE, FALSE ); /* Remove second FALSE if 
compiler complains */
      LINK( d->character, first_char, last_char, next, prev );
	original = d->character->in_room;
	char_to_room( d->character, temproom );
	victim = d->character; /* Hopefully this will work, if not, we're SOL */
	d->character->desc	= NULL;
	d->character		= NULL;	
	DISPOSE( d->outbuf );
	DISPOSE( d );

	if( xIS_SET( victim->pcdata->flags, PCFLAG_PRIVACY ) && !IS_IMMORTAL(ch) )
    	{
	   ch_printf( ch, "%s has privacy enabled.\n\r", victim->name );
         skip = TRUE;
    	}

    	if( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    	{
	   send_to_char( "You cannot finger an immortal.\n\r", ch );
         skip = TRUE;
    	}
      loaded = TRUE;
   }

   if( !skip )
   {
      send_to_char( "\n\r&w          &RF&zinger &RI&znfo\n\r", ch );
      send_to_char( "          &W-----------\n\r", ch );
      ch_printf( ch, "&zName    &R: &Y%-20s &zMUD Age&R: &Y%d\n\r", victim->name, get_age( victim ) );
      if (IS_IMMORTAL(ch))
      ch_printf( ch, "&zLevel   &R: &Y%-20d &z  Class&R: &Y%s\n\r", victim->level, capitalize( get_class(victim) ) );
      ch_printf( ch, "&zSex     &R: &Y%-20s &z  Race &R: &Y%s\n\r", victim->sex == SEX_MALE ? "Male" : victim->sex == SEX_FEMALE ? "Female" : "Neutral", capitalize( get_race(victim) ) );
      ch_printf( ch, "&zTitle   &R:&Y%s\n\r", victim->pcdata->title );
      ch_printf( ch, "&zHomepage&R: &Y%s\n\r", victim->pcdata->homepage != NULL ? victim->pcdata->homepage : "Not specified" );
      ch_printf( ch, "&zEmail   &R: &Y%s\n\r", victim->pcdata->email != NULL ? victim->pcdata->email : "Not specified" );
      ch_printf( ch, "&zAIM#    &R: &Y%s\n\r", victim->pcdata->aim );
      if (victim->pcdata->clan)
      ch_printf( ch, "&zClan    &R: &Y%s\n\r", victim->pcdata->clan->name );
      if( !loaded )
      ch_printf( ch, "&zStatus  &R: &GOnline\n\r" );
      else
      ch_printf( ch, "&zStatus  &R: &YOffline\n\r" );
      if( loaded )
         ch_printf( ch, "&zLast on &R: &G%s\n\r", ctime( &victim->pcdata->lastlogon ) );
/*      else
         ch_printf( ch, "&wLast on : &GCurrently Online\n\n\r", laston );*/
/*
      if( !loaded )
         ch_printf( ch, "&zLast on &R: &Y%s\n\r", ctime( &victim->logon ) );
      else
         ch_printf( ch, "&zLast on &R: &Y%s\n\r", laston );
*/
      if ( IS_IMMORTAL(ch) )
      {
//	   send_to_char( "\n\r&wImmortal Information\n\r", ch );
	   send_to_char( "&r____________________________________________________\n\r", ch );
//	   ch_printf( ch, "&zIP Info       &R: &Y%s\n\r", loaded ? "Unknown" : victim->desc->host );
	   ch_printf( ch, "&zTime played   &R: &Y%ld hours\n\r", (long int)GET_TIME_PLAYED( victim ) );
         ch_printf( ch, "&zAuthorized by &R: &Y%s\n\r",
	      victim->pcdata->authed_by ? victim->pcdata->authed_by : ( sysdata.WAIT_FOR_AUTH ? "Not Authed" : "The Code" ) );
         ch_printf( ch, "&zPrivacy Status&R: &Y%s\n\r", xIS_SET( victim->pcdata->flags, PCFLAG_PRIVACY ) ? "Enabled" : "Disabled" );
         if( victim->level < ch->level )
         {
            /* Added by Tarl 19 Dec 02 to remove Huh? when ch not high enough to view comments. */
            command = find_command( "comment" );
            if( !command )
               level = LEVEL_IMMORTAL;
            else
               level = command->level;
            if( ch->level >= command->level )
            {
               sprintf( buf, "comment list %s", victim->name );
               interpret( ch, buf );
            }
         }
      }
     // ch_printf( ch, "&zBio&R:\n\r&W%s\n\r", victim->pcdata->bio ? victim->pcdata->bio : "&YNot created" );
   }

   if( loaded )
   {
      int x, y;

      char_from_room( victim );
	char_to_room( victim, original );

      quitting_char = victim;
      save_char_obj( victim );

	if( sysdata.save_pets && victim->pcdata->pet )
	   extract_char( victim->pcdata->pet, TRUE );

      saving_char = NULL;

      /*
       * After extract_char the ch is no longer valid!
       */
      extract_char( victim, TRUE );
      for ( x = 0; x < MAX_WEAR; x++ )
	  for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;
   }
   return;
}

/* Added a clone of homepage to let players input their email addy - Samson 4-18-98 */
void do_email( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->email )
	  ch->pcdata->email = str_dup( "" );
	ch_printf( ch, "Your email address is: %s\n\r",	show_tilde( ch->pcdata->email ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->email )
	  DISPOSE(ch->pcdata->email);
	ch->pcdata->email = str_dup("");
	send_to_char( "Email address cleared.\n\r", ch );
	return;
    }

    strcpy( buf, argument );
    
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->email )
      DISPOSE(ch->pcdata->email);
    ch->pcdata->email = str_dup(buf);
    send_to_char( "Email address set.\n\r", ch );
}
/*
void do_icq_number( CHAR_DATA *ch, char *argument )
{
    int icq;

    if ( IS_NPC( ch ) )
	return;

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->icq )
	  ch->pcdata->icq = 0;
	ch_printf( ch, "Your ICQ# is: %d\n\r", ch->pcdata->icq );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	ch->pcdata->icq = 0;

	send_to_char( "ICQ# cleared.\n\r", ch );
	return;
    }

    if ( !is_number( argument ) )
    {
	send_to_char( "You must enter numeric data.\n\r", ch );
	return;
    }

    icq = atoi( argument );

    if ( icq < 1 )
    {
	send_to_char( "Valid range is greater than 0.\n\r", ch );
	return;
    }

    ch->pcdata->icq = icq;
    send_to_char( "ICQ# set.\n\r", ch );
    return;
}
*/

void do_icq_number( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        if ( !ch->pcdata->email )
          ch->pcdata->aim = str_dup( "" );
        ch_printf( ch, "Your AIM Screen-Name is: %s\n\r",  ch->pcdata->aim );
        return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
        if ( ch->pcdata->aim )
          DISPOSE(ch->pcdata->aim);
        ch->pcdata->aim = str_dup("");

        send_to_char( "AIM Screen-Name cleared.\n\r", ch );
        return;
    }

    strcpy( buf, argument );

    if ( strlen(buf) > 70 )
        buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->aim )
      DISPOSE(ch->pcdata->aim);
    ch->pcdata->aim = str_dup(buf);
    send_to_char( "AIM Screen-Name set.\n\r", ch );
}


void do_homepage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( !argument || argument[0] == '\0' )
    {
	if ( !ch->pcdata->homepage )
	  ch->pcdata->homepage = str_dup( "" );
	ch_printf( ch, "Your homepage is: %s\n\r", show_tilde( ch->pcdata->homepage ) );
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
}

void do_privacy( CHAR_DATA *ch, char *argument )
{
   if ( IS_NPC( ch ) )
   {
	send_to_char( "Mobs can't use the privacy toggle.\n\r", ch );
	return;
   }

   xTOGGLE_BIT( ch->pcdata->flags, PCFLAG_PRIVACY );

   if ( xIS_SET( ch->pcdata->flags, PCFLAG_PRIVACY ) )
   {
	send_to_char( "Privacy flag enabled.\n\r", ch );
	return;
   }
   else
   {
	send_to_char( "Privacy flag disabled.\n\r", ch );
	return;
   }
}

