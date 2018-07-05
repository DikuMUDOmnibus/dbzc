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
 *			   Player communication module			    *
 ****************************************************************************/
/*****************************************************************
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
#include <sys/stat.h>
#ifdef CYGWIN
#include <crypt.h>
#endif
#include "mud.h"

/*
 *  Externals
 */
void send_obj_page_to_char(CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page);
void send_room_page_to_char(CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page);
void send_page_to_char(CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page);
void send_control_page_to_char(CHAR_DATA * ch, char page);
void    send_to_desc_color args( ( const char *txt, DESCRIPTOR_DATA *d ) );

/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );

char *  drunk_speech    args( ( const char *argument, CHAR_DATA *ch ) ); 


void play_sound( CHAR_DATA *ch, char *sound, int type)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *vch;
  if( (sound == NULL)  || (ch == NULL))
   return;
  sprintf( buf, "!!SOUND(%s)\n\r", sound);
  switch( type)
  {
   case SND_CHAR:
    if( HAS_SOUND(ch))
     send_to_char(buf, ch);
    break;
   case SND_ROOMALL:
        for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
         if (!IS_NPC(vch))
          if (HAS_SOUND(vch))
           send_to_char( buf, vch);
   break;
   case SND_ROOM:
        for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
        {
          if (vch == ch)
          continue;
          if (!IS_NPC(vch))
           if (HAS_SOUND(vch))
           send_to_char( buf, vch);
        }
   break;
   case SND_AREA:
        for ( d = first_descriptor; d; d = d->next )
        {
            vch = d->character;
            if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
            &&   !IS_NPC(vch)
            &&   (vch->in_room->area == ch->in_room->area ))
            {
               if (HAS_SOUND(vch))
               send_to_char( buf, vch);
            }
        }
   break;
   case SND_WORLD:
        for ( d = first_descriptor; d; d = d->next )
        {
            vch = d->character; 
            if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
            &&   !IS_NPC(vch))
            {
               if (HAS_SOUND(vch))
               send_to_char( buf, vch);
            }
        }
   break;
   default:
   break;
  }
  return;
}




char *drunk_speech( const char *argument, CHAR_DATA *ch )
{
  const char *arg = argument;
  static char buf[MAX_INPUT_LENGTH*2];
  char buf1[MAX_INPUT_LENGTH*2];
  sh_int drunk;
  char *txt;
  char *txt1;  

  if ( IS_NPC( ch ) || !ch->pcdata ) return (char *) argument;

  drunk = ch->drunk;

  if ( drunk <= 0 )
    return (char *) argument;

  buf[0] = '\0';
  buf1[0] = '\0';

  if ( !argument )
  {
     bug( "Drunk_speech: NULL argument", 0 );
     return "";
  }

  /*
  if ( *arg == '\0' )
    return (char *) argument;
  */

  txt = buf;
  txt1 = buf1;

  while ( *arg != '\0' )
  {
    if ( toupper(*arg) == 'T' )
    {
	if ( number_percent() < ( drunk * 2 ) )		/* add 'h' after an 'T' */
	{
	   *txt++ = *arg;
	   *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( toupper(*arg) == 'X' )
    {
	if ( number_percent() < ( drunk * 2 / 2 ) )
	{
	  *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( number_percent() < ( drunk * 2 / 5 ) )  /* slurred letters */
    {
      sh_int slurn = number_range( 1, 2 );
      sh_int currslur = 0;	

      while ( currslur < slurn )
	*txt++ = *arg, currslur++;
    }
   else
    *txt++ = *arg;

    arg++;
  };

  *txt = '\0';

  txt = buf;

  while ( *txt != '\0' )   /* Let's mess with the string's caps */
  {
    if ( number_percent() < ( 2 * drunk / 2.5 ) )
    {
      if ( isupper(*txt) )
        *txt1 = tolower( *txt );
      else
      if ( islower(*txt) )
        *txt1 = toupper( *txt );
      else
        *txt1 = *txt;
    }
    else
      *txt1 = *txt;

    txt1++, txt++;
  };

  *txt1 = '\0';
  txt1 = buf1;
  txt = buf;

  while ( *txt1 != '\0' )   /* Let's make them stutter */
  {
    if ( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
    {			 /* along there somewhere soon */

      while ( *txt1 == ' ' )  /* Don't stutter on spaces */
        *txt++ = *txt1++;

      if ( ( number_percent() < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
      {
	sh_int offset = number_range( 0, 2 );
	sh_int pos = 0;

	while ( *txt1 != '\0' && pos < offset )
	  *txt++ = *txt1++, pos++;

	if ( *txt1 == ' ' )  /* Make sure not to stutter a space after */
	{		     /* the initial offset into the word */
	  *txt++ = *txt1++;
	  continue;
	}

	pos = 0;
	offset = number_range( 2, 4 );	
	while (	*txt1 != '\0' && pos < offset )
	{
	  *txt++ = *txt1;
	  pos++;
	  if ( *txt1 == ' ' || pos == offset )  /* Make sure we don't stick */ 
	  {		               /* A hyphen right before a space	*/
	    txt1--;
	    break;
	  }
	  *txt++ = '-';
	}
	if ( *txt1 != '\0' )
	  txt1++;
      }     
    }
   else
    *txt++ = *txt1++;
  }

  *txt = '\0';

  return buf;
}

/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int position;
    int length;

                if (channel == CHANNEL_CHAT || channel == CHANNEL_GEMOTE)
                length = MAX_INPUT_LENGTH + 30;
                else
                length = MAX_INPUT_LENGTH + 4;


    if ( IS_NPC( ch ) && channel == CHANNEL_CLAN )
    {
	send_to_char( "Mobs can't be in clans.\n\r", ch );
	return;
    }

    if ( !IS_PKILL( ch ) && channel == CHANNEL_WARTALK )
    {
	if ( !IS_IMMORTAL( ch ) )
	{
	  send_to_char( "Peacefuls have no need to use wartalk.\n\r", ch );
	  return;
	}
    }
                                          
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }
    
    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );	/* where'd this line go? */
	return;
    }

    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
    {
	ch_printf( ch, "You can't %s.\n\r", verb );
	return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {
    default:
	ch_printf( ch, "&CYou %s '&w%s&C'\n\r", verb, argument );
	sprintf( buf, "&C$n %ss '&w$t&C'",     verb );
	break;
    case CHANNEL_RACETALK:
	ch_printf( ch, "&z(&G%s&z)&W: &G'&w%s&G'&w\n\r", verb, argument );
	sprintf( buf, "&z(&G%s&z) &w$n&W:  &G'&w$t&G'",     verb );
        break;
    case CHANNEL_YELL:
        ch_printf( ch, "&CYou %s '%s'\n\r", verb, argument );
        sprintf( buf, "&C$n %ss '$t'",     verb );
        break;
    case CHANNEL_CLAN:
         ch_printf( ch, "&z(&R%s&z)&W: &C'&c%s&C'\n\r", verb, argument );
         sprintf( buf, "&z(&R%s&z) &W$n: &C'&c$t&C'", verb );
         break;
    case CHANNEL_CHAT:
        ch_printf( ch, "&w(&C%s&w)&z: &C'&W%s&C'&w\n\r", verb, argument );
        sprintf( buf, "&w(&C%s&w) &w$n&z:&W &C'&W$t&C'&w",     verb );
        break;
    case CHANNEL_OOC:
	ch_printf( ch, "&b(&Y%s&b)&O: &z'&W%s&z'&w\n\r", verb, argument );
	sprintf( buf, "&b(&Y%s&b) &w$n&O:&W &z'&W$t&z'&w",     verb );
        break;
    case CHANNEL_ADMIN:
        ch_printf( ch, "&r[&OAdmin&r]&W: %s\n\r", argument );
        sprintf( buf, "&r[&OAdmin&r] &w$n&W: $t" );
        break;
    case CHANNEL_GEMOTE:
	ch_printf( ch, "&w(&CCHAT&w)&z: &W&C'&w%s %s&C'&w\n\r",ch->name, argument );
	sprintf( buf, "&w(&CCHAT&w)&z: &W&C'&w$n $t&C'&w");
        break;
    case CHANNEL_HIGHGOD:
        ch_printf( ch, "&p(&PBrolli likes little boys&p)&z: &R'&G%s&R'&w\n\r", argument );
        sprintf( buf, "&p(&PBrolli likes little boys&p)&z: &C$n&c: &R'&G$t&R'&w");
        break;
/*
    case CHANNEL_TRAFFIC:
	set_char_color( AT_GOSSIP, ch );
	ch_printf( ch, "You %s:  %s\n\r", verb, argument );
	sprintf( buf, "$n %ss:  $t", verb );
	break;*/
    case CHANNEL_WARTALK:
        set_char_color( AT_WARTALK, ch );
        ch_printf( ch, "&RYou %s '%s'&w\n\r", verb, argument );
        sprintf( buf, "&R$n %ss '$t'&w", verb );
        break;
    case CHANNEL_IMMTALK:
         ch_printf( ch, "&C[&YIMM&C]&W: &B%s\n\r", argument);
         sprintf( buf, "&C[&YIMM&C] &W$n: &B$t");
         break;
    case CHANNEL_AVTALK:
	sprintf( buf, "&z[&cAvTalk&z] &W$n&z%c&w $t", channel == CHANNEL_IMMTALK ? ':' : ':' );
	position	= ch->position;
	ch->position	= POS_STANDING;
    act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument, verb );
	append_to_file( LOG_FILE, buf2 );
    }

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	&&   vch != ch
	&&  !IS_SET(och->deaf, channel) )
	{
		char *sbuf = argument;
                char lbuf[length]; /* invis level string + buf */
 


  /* fix by Gorog os that players can ignore others' channel talk */
            if ( is_ignoring(och, ch) && get_trust(ch) <= get_trust(och) )
               continue;

	    if ( channel != CHANNEL_NEWBIE && NOT_AUTHED(och) )
		continue;		
	    if ( channel == CHANNEL_IMMTALK && (!IS_IMMORTAL(och) && !xIS_SET(och->act, PLR_SEMI)) )
 		continue;
            if ( xIS_SET(och->act, PLR_FUSED) && och->fused_with && och->in_room->vnum == 98 )
                  continue;
/*
            if ( channel == CHANNEL_INFO && !IS_IMMORTAL(och) )
                continue;
*/
            if ( channel == CHANNEL_ADMIN && (och->level < 65) )
                continue;
            if ( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
                continue;
	    if ( channel == CHANNEL_AVTALK && !IS_HERO(och) )
		continue;
	    if ( channel == CHANNEL_HIGHGOD && get_trust( och ) < sysdata.muse_level )
		continue;
	    if ( channel == CHANNEL_HIGH    && get_trust( och ) < sysdata.think_level )
		continue;
/*
	    if ( channel == CHANNEL_TRAFFIC
	    &&  !IS_IMMORTAL( och )
	    &&  !IS_IMMORTAL(  ch ) )
	    {
		if ((  IS_HERO( ch ) && !IS_HERO( och ) )
		||  ( !IS_HERO( ch ) &&  IS_HERO( och ) ))
		  continue;
	    }
*/
	    if ( channel == CHANNEL_NEWBIE  && 
                  ( !IS_IMMORTAL(och) && !NOT_AUTHED(och) ) )
		continue;
	    if ( IS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
	    	continue;
	    if ( channel == CHANNEL_YELL
	    &&   vch->in_room->area != ch->in_room->area )
		continue;

	    if ( channel == CHANNEL_CLAN  )
	    {
		if ( IS_NPC( vch ) )
		  continue;
		if ( vch->pcdata->clan != ch->pcdata->clan )
	    	  continue;
	    }


	    if ( channel == CHANNEL_RACETALK )
	      if ( vch->race != ch->race )
		continue;

	    if ( xIS_SET(ch->act, PLR_WIZINVIS) &&
	    	can_see(vch, ch, FALSE) && IS_IMMORTAL(vch))
	    {
	    	sprintf(lbuf, "(%d) ", (!IS_NPC(ch))?ch->pcdata->wizinvis
			:ch->mobinvis);
	    }
	    else
	    {
	    	lbuf[0] = '\0';
	    }
	    
	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;

	    MOBtrigger = FALSE;
	    if ( channel == CHANNEL_IMMTALK || channel == CHANNEL_AVTALK || channel == CHANNEL_ADMIN)
		act( AT_IMMORT, strcat(lbuf,buf), ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_WARTALK)
        	act( AT_WARTALK, strcat(lbuf,buf), ch, sbuf, vch, TO_VICT );
	    else if (channel == CHANNEL_RACETALK)
		act( AT_RACETALK, strcat(lbuf,buf), ch, sbuf, vch, TO_VICT );
	    else
		act( AT_GOSSIP, strcat(lbuf,buf), ch, sbuf, vch, TO_VICT );
	    vch->position	= position;
	}
    }

    /* too much system degradation with 300+ players not to charge 'em a bit */
    /* 600 players now, but waitstate on clantalk is bad for pkillers */
/*    if ( ( ch->level<49 )
    && ( channel != CHANNEL_WARTALK )
    && ( channel != CHANNEL_CLAN ) )
       WAIT_STATE( ch, 6 );
*/
    return;
}

void to_channel( const char *argument, int channel, const char *verb, sh_int level )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( !first_descriptor || argument[0] == '\0' )
      return;

    sprintf(buf, "%s: %s\r\n", verb, argument );

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( !och || !vch )
	  continue;
	if ( !IS_IMMORTAL(vch)
	|| ( get_trust(vch) < sysdata.build_level && channel == CHANNEL_BUILD )
	|| ( get_trust(vch) < sysdata.log_level
	&& ( channel == CHANNEL_LOG || channel == CHANNEL_HIGH || 
	channel == CHANNEL_WARN ||channel == CHANNEL_COMM) ) )
	  continue;

	if ( d->connected == CON_PLAYING
	&&  !IS_SET(och->deaf, channel)
	&&   get_trust( vch ) >= level )
	{
	  set_char_color( AT_LOG, vch );
	  send_to_char_color( buf, vch );
	}
    }

    return;
}


void do_chat( CHAR_DATA *ch, char *argument )
{
   long long int pl;
   int gain;
   char buf[MSL];
   pl = gain = 0;
   int chance;
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if (!xIS_SET(ch->act, PLR_CHAT) && !IS_IMMORTAL(ch))
    {
      send_to_char("You can't RP until a Immortal has authed your biography.\n\r", ch);
      return;
    }
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }
    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
    {
        ch_printf( ch, "You can't chat.\n\r" );
        return;
    }
    gain = strlen(argument);
    chance = number_range(0, 1);  
    if ( gain > 254 )
    gain = 254;

    gain *= number_range(5, 20);
//    gain *= ch->rplevel;
    gain += ch->exp * 0.000005;
    gain *= number_range(5, 50);
//    ch->rpneed++;
    if ( ch->exp >= 5000000 && ch->exp < 25000000)  {pl = ch->pcdata->learned[gsn_chikone] * 250000;}
    else if ( ch->exp >= 25000000 && ch->exp < 55000000){pl = ch->pcdata->learned[gsn_chiktwo] * 550000; }
    else if ( ch->exp >= 55000000 && ch->exp < 75000000){pl = ch->pcdata->learned[gsn_chikthree] * 750000; }
    else if ( ch->exp >= 75000000 && ch->exp < 100000000){pl = ch->pcdata->learned[gsn_chikfour] * 1000000;}
    else if ( ch->exp >= 100000000 && ch->exp < 125000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}
    if (ch->exp >= 5000000)
    {
     if (  pl < ch->exp )
     gain = 0;
    }

    talk_channel( ch, argument, CHANNEL_CHAT, "CHAT" );
    sprintf( buf, "%s: %s", ch->name, argument);
    append_to_file( CHAT_FILE, buf);
    if ( gain > 0 && chance == 1)
    {
      ch_printf(ch, "&WYou have gained &w%d&W powerlevel for your RP.", gain);
      ch->exp += gain;
      return;
    }
    return;
}


void do_ooc( CHAR_DATA *ch, char *argument )
{
    char buf [MSL];
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_OOC, "OOC" );
    sprintf( buf, "%s: %s", ch->name, argument);
    append_to_file( OOC_FILE, buf);
    return;
}
void do_admin( CHAR_DATA *ch, char *argument )
{
    if (ch->level < 65)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ADMIN, "Admin" );
    return;
}
void do_fusetalk( CHAR_DATA *ch, char *argument )
{
    if (!xIS_SET(ch->act, PLR_FUSED))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Fusetalk what?\n\r", ch );
        return;
    }

    if (xIS_SET(ch->act, PLR_FUSED) && ch->fused_with && xIS_SET(ch->act, PLR_INVIS2))
    ch_printf(ch->fused_with, "\n\r&c[&CFusetalk&c] &W%s&z: &R%s&w\n\r", ch->fusionname, argument);
    else if (xIS_SET(ch->act, PLR_FUSED) && ch->fused_with && !xIS_SET(ch->act, PLR_INVIS2))
    ch_printf(ch, "&c[&CFusetalk&c] &W%s&z: &R%s&w\n\r", ch->fusionname, argument);

    return;
}

void do_gemote( CHAR_DATA *ch, char *argument )
{
   long long int pl;
   int gain;
   char buf[MSL];
   int chance;
   pl = gain = 0;
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if (!xIS_SET(ch->act, PLR_CHAT) && !IS_IMMORTAL(ch))
    {
      send_to_char("You can't RP until a Immortal has authed your biography.\n\r", ch);
      return;
    }
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }
    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
    {
        ch_printf( ch, "You can't gemote.\n\r" );
        return;
    }
    gain = strlen(argument);

    if ( gain > 254 )
    	gain = 254;
    chance = number_range( 0, 2);
    gain *= number_range(5, 20);
//    gain *= ch->rplevel;
    gain += ch->exp * 0.000005;
    gain *= number_range(5, 50);
//    ch->rpneed++;
    if ( ch->exp >= 5000000 && ch->exp < 25000000)  {pl = ch->pcdata->learned[gsn_chikone] * 250000;}
    else if ( ch->exp >= 25000000 && ch->exp < 55000000){pl = ch->pcdata->learned[gsn_chiktwo] * 550000; }
    else if ( ch->exp >= 55000000 && ch->exp < 75000000){pl = ch->pcdata->learned[gsn_chikthree] * 750000; }
    else if ( ch->exp >= 75000000 && ch->exp < 100000000){pl = ch->pcdata->learned[gsn_chikfour] * 1000000;}
    else if ( ch->exp >= 100000000 && ch->exp < 125000000) {pl = ch->pcdata->learned[gsn_chikfive] * 1250000;}

    if (ch->exp >= 5000000)
    {
     if (  pl < ch->exp )
	     gain = 0;
    }

    sprintf( buf, "%s: %s", ch->name, argument);
    append_to_file( CHAT_FILE, buf);
    talk_channel( ch, argument, CHANNEL_GEMOTE, "CHAT" );
    if ( gain > 0 && chance == 1)
    {
      ch_printf(ch, "&WYou have gained &w%d&W powerlevel for your RP.", gain);
      ch->exp += gain;
      return;
    }
    return;
}




void do_clantalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Have you gone through afterlife often? No? Then it means you aint in a clan.\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_CLAN, "Clan" );
    return;
}

void do_newbiechat( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) || ( !NOT_AUTHED( ch ) && !IS_IMMORTAL(ch)) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    talk_channel( ch, argument, CHANNEL_NEWBIE, "newbiechat" );
    return;
}



void do_music( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}


void do_quest( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_QUEST, "quest" );
    return;
}

void do_ask( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "ask" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "answer" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "shout" );
  WAIT_STATE( ch, 12 );
  return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "yell" );
  return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}


void do_muse( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_HIGHGOD, "muse" );
    return;
}


void do_think( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_HIGH, "think" );
    return;
}


void do_avtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_AVTALK, "avtalk" );
    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
    EXT_BV actflags;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) && !IS_IMMORTAL(ch) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC(ch) )
	xREMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
    {
	char *sbuf = argument;

	if ( vch == ch )
		continue;
			
	/* Check to see if a player on a map is at the same coords as the recipient 
	   don't need to verify the PLR_ONMAP flags here, it's a room occupants check */
	if( !is_same_map( vch, ch ) )
		continue;

	/* Check to see if character is ignoring speaker */
	if (is_ignoring(vch, ch))
	{
		/* continue unless speaker is an immortal */
		if(!IS_IMMORTAL(ch) || get_trust(vch) > get_trust(ch))
			continue;
		else
		{
			set_char_color(AT_IGNORE, vch);
			ch_printf(vch,"You attempt to ignore %s, but"
				" are unable to do so.\n\r", ch->name);
		}
	}
				
	sbuf = drunk_speech( sbuf, ch );

	MOBtrigger = FALSE;
        if (xIS_SET(ch->act, PLR_FUSED) && ch->fusionname)
        act( AT_SAY, "$n says in a doubled voice: '$t'", ch, sbuf, vch, TO_VICT );
        else
	act( AT_SAY, "$n says '$t'", ch, sbuf, vch, TO_VICT );
    }
/*    MOBtrigger = FALSE;
    act( AT_SAY, "$n says '$T'", ch, NULL, argument, TO_ROOM );*/
    ch->act = actflags;
    MOBtrigger = FALSE;

    if (xIS_SET(ch->act, PLR_FUSED) && ch->fusionname)
    act( AT_SAY, "\n\rYou say in a doubled voice: '$T'&w", ch, NULL, drunk_speech( argument, ch ), TO_CHAR );
    else
    act( AT_SAY, "\n\rYou say '$T'&w", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 

    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    if ( char_died(ch) )
	return;
    oprog_speech_trigger( argument, ch ); 
    if ( char_died(ch) )
	return;
    rprog_speech_trigger( argument, ch ); 
    return;
}


void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;

    REMOVE_BIT( ch->deaf, CHANNEL_WHISPER );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Whisper to whom what?\n\r", ch );
	return;
    }


    if ( (victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You have a nice little chat with yourself.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_AFK) )
      {
      send_to_char( "That player is afk.\n\rBut will still recieve the message.\n\r", ch );
      }
    if ( IS_SET( victim->deaf, CHANNEL_WHISPER ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S whispers turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }
    if ( !IS_NPC(victim) &&  xIS_SET(victim->act, PLR_SILENCE) )
	send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if target of tell is ignoring the sender */
    if(is_ignoring(victim, ch))	
    {
	/* If the sender is an imm then they cannot be ignored */
	if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
	{
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch,"%s is ignoring you.\n\r",
			victim->name);
		return;
	}
	else
	{
		set_char_color(AT_IGNORE, victim);
		ch_printf(victim, "You attempt to ignore %s, but "
			"are unable to do so.\n\r", ch->name);
	}
    }

    act( AT_WHISPER, "You whisper to $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    MOBtrigger = FALSE;

    victim->position	= POS_STANDING;
	act( AT_WHISPER, "$n whispers to you '$t'", ch, argument, victim, TO_VICT );

    if ( !IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
	act( AT_WHISPER, "$n whispers something to $N.", ch, argument, victim, TO_NOTVICT );
    MOBtrigger = TRUE;

    victim->position	= position;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (whisper to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }
    
    mprog_speech_trigger( argument, ch );
    return;
}

void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    int position;
    CHAR_DATA *victim;
    CHAR_DATA *switched_victim = NULL;

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }
    if (!IS_NPC(ch)
    && ( xIS_SET(ch->act, PLR_SILENCE)
    ||   xIS_SET(ch->act, PLR_NO_TELL) ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You have a nice little chat with yourself.\n\r", ch );
	return;
    }

    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }

   else if ( !IS_NPC( victim ) && ( victim->switched ) )
     switched_victim = victim->switched;

   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }
    if ( !IS_NPC(victim) && xIS_SET(victim->pcdata->flags, PCFLAG_DND) )
    {
    ch_printf( ch, "&RSorry, %s does not wish to be disturbed.\n\r", victim->name);
    ch_printf( victim, "&RYour DND flag just stoped %s's tell.\n\r", ch->name);
    return;
    }
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_AFK) )
    {
      send_to_char( "That player is afk.\n\rBut will still recieve the message.\n\r", ch );
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_SILENCE) )
	send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

   /* if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) ) )
    {
      act( AT_PLAIN, "$E is too tired to discuss such matters with you now.",
      	ch, 0, victim, TO_CHAR );
      return;
    }*/

    if (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) )
    {
      act(AT_PLAIN, "A magic force prevents your message from being heard.",
      	ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if target of tell is ignoring the sender */
    if(is_ignoring(victim, ch))	
    {
	/* If the sender is an imm then they cannot be ignored */
	if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
	{
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch,"%s is ignoring you.\n\r",
			victim->name);
		return;
	}
	else
	{
		set_char_color(AT_IGNORE, victim);
		ch_printf(victim, "You attempt to ignore %s, but "
			"are unable to do so.\n\r", ch->name);
	}
    }

    ch->retell = victim;
    
    if(!IS_NPC(victim) && IS_IMMORTAL(victim) && victim->pcdata->tell_history &&
    	isalpha(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0]))
    {
	sprintf(buf, "%s told you '%s'\n\r",
		capitalize(IS_NPC(ch) ? ch->short_descr : ch->name),
		argument);
	
	/* get lasttell index... assumes names begin with characters */
	victim->pcdata->lt_index =
		tolower(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0]) - 'a';
	
	/* get rid of old messages */
	if(victim->pcdata->tell_history[victim->pcdata->lt_index])
		STRFREE(victim->pcdata->tell_history[victim->pcdata->lt_index]);
	
	/* store the new message */
	victim->pcdata->tell_history[victim->pcdata->lt_index] =
		STRALLOC(buf);
    }   	

    if(switched_victim)
      victim = switched_victim;


   
    act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    MOBtrigger = FALSE;
    victim->position	= POS_STANDING;
	act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
    MOBtrigger = TRUE;

    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (tell to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }
    
    mprog_speech_trigger( argument, ch );
    return;
}


void do_reply( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;
    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched )
	&& can_see( ch, victim, FALSE ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_AFK) )
    {
      send_to_char( "That player is afk.\n\rBut will still recieve the message.\n\r", ch );
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
	TO_CHAR );
      return;
    }

    if ( ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || ( !IS_NPC(victim) && IS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

    /* Check to see if the receiver is ignoring the sender */
    if(is_ignoring(victim, ch))
    {
    	/* If the sender is an imm they cannot be ignored */
    	if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
    	{
    		set_char_color(AT_IGNORE, ch);
    		ch_printf(ch,"%s is ignoring you.\n\r",
    			victim->name);
    		return;
    	}
    	else
    	{
    		set_char_color(AT_IGNORE, victim);
    		ch_printf(victim, "You attempt to ignore %s, but "
    			"are unable to do so.\n\r", ch->name);
    	}
    }

    act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
		act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;
    ch->retell		= victim;

    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (reply to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }

    if(!IS_NPC(victim) && IS_IMMORTAL(victim) && victim->pcdata->tell_history &&
    	isalpha(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0]))
    {
    	sprintf(buf, "%s told you '%s'\n\r",
    		capitalize(IS_NPC(ch) ? ch->short_descr : ch->name),
    		argument);
    	
    	/* get lasttell index... assumes names begin with characters */
    	victim->pcdata->lt_index =
    		tolower(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0]) - 'a';
    	
    	/* get rid of old messages */
    	if(victim->pcdata->tell_history[victim->pcdata->lt_index])
    		STRFREE(victim->pcdata->tell_history[victim->pcdata->lt_index]);
    	
    	/* store the new message */
    	victim->pcdata->tell_history[victim->pcdata->lt_index] =
    		STRALLOC(buf);
    }
    mprog_speech_trigger( argument, ch );
    return;
}

void do_retell(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int position;
	CHAR_DATA *switched_victim = NULL;

	if(IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
	{
		send_to_char("You can't do that here.\n\r", ch);
		return;
	}
	
	if ( !IS_NPC(ch) && (xIS_SET(ch->act, PLR_SILENCE)
	||   xIS_SET(ch->act, PLR_NO_TELL)) )
	{
		send_to_char("You can't do that.\n\r", ch);
		return;
	}
	
	if(argument[0] == '\0')
	{
		ch_printf(ch, "What message do you wish to send?\n\r");
		return;
	}
	
	victim = ch->retell;
	
	if(!victim)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	
	if(!IS_NPC(victim) && (victim->switched) &&
		(get_trust(ch) > LEVEL_AVATAR) )
	{
		send_to_char("That player is switched.\n\r", ch);
		return;
	}
	else if(!IS_NPC(victim) && (victim->switched) )
	{
		switched_victim = victim->switched;
	}
	else if(!IS_NPC(victim) &&(!victim->desc))
	{
		send_to_char("That player is link-dead.\n\r", ch);
		return;
	}
	
	if(!IS_NPC(victim) && xIS_SET(victim->act, PLR_AFK) )
	{
          send_to_char( "That player is afk.\n\rBut will still recieve the message.\n\r", ch );
	}
	
	if(IS_SET(victim->deaf, CHANNEL_TELLS) &&
		(!IS_IMMORTAL(ch) || (get_trust(ch) < get_trust(victim))))
	{
		act(AT_PLAIN, "$E has $S tells turned off.", ch, NULL,
			victim, TO_CHAR);
		return;
	}
	
	if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_SILENCE) )
		send_to_char("That player is silenced. They will receive your message, but can not respond.\n\r", ch);
	
	if((!IS_IMMORTAL(ch) && !IS_AWAKE(victim)) ||
		(!IS_NPC(victim) &&
		IS_SET(victim->in_room->room_flags, ROOM_SILENCE)))
	{
		act(AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}
	
	if(victim->desc && victim->desc->connected == CON_EDITING &&
		get_trust(ch) < LEVEL_GOD)
	{
		act(AT_PLAIN, "$E is currently in a writing buffer. Please "
			"try again in a few minutes.", ch, 0, victim, TO_CHAR);
		return;
	}
	
	/* check to see if the target is ignoring the sender */
	if(is_ignoring(victim, ch))
	{
		/* if the sender is an imm then they cannot be ignored */
		if(!IS_IMMORTAL(ch) || get_trust(victim) > get_trust(ch))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch, "%s is ignoring you.\n\r",
				victim->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE,victim);
			ch_printf(victim, "You attempy to ignore %s, but "
				"are unable to do so.\n\r", ch->name);
		}
	}

	/* store tell history for victim */
	if(!IS_NPC(victim) && IS_IMMORTAL(victim) && victim->pcdata->tell_history &&
		isalpha(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0]))
	{
		sprintf(buf, "%s told you '%s'\n\r",
			capitalize(IS_NPC(ch) ? ch->short_descr : ch->name),
			argument);
		
		/* get lasttel index... assumes names begin with chars */
		victim->pcdata->lt_index =
			tolower(IS_NPC(ch) ? ch->short_descr[0] : ch->name[0])
			 - 'a';
		
		/* get rid of old messages */
		if(victim->pcdata->tell_history[victim->pcdata->lt_index])
			STRFREE(victim->pcdata->tell_history[victim->pcdata->lt_index]);
		
		/* store the new messagec */
		victim->pcdata->tell_history[victim->pcdata->lt_index] =
			STRALLOC(buf);
	}
	
	if(switched_victim)
		victim = switched_victim;
	
	act(AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR);
	position = victim->position;
        MOBtrigger = FALSE;

	victim->position = POS_STANDING;
		act(AT_TELL, "$n tells you '$t'", ch,
			argument, victim, TO_VICT);
        MOBtrigger = TRUE;

	victim->position = position;
	victim->reply = ch;
	if(IS_SET(ch->in_room->room_flags, ROOM_LOGSPEECH))
	{
		sprintf(buf, "%s: %s (retell to) %s.",
			IS_NPC(ch) ? ch->short_descr : ch->name,
			argument,
			IS_NPC(victim) ? victim->short_descr : victim->name);
		append_to_file(LOG_FILE, buf);
	}
	
	mprog_speech_trigger(argument, ch);
	return;
}

void do_repeat(CHAR_DATA *ch, char *argument)
{
	int tindex;
	
	if(IS_NPC(ch) || !IS_IMMORTAL(ch) || !ch->pcdata->tell_history)
	{
		ch_printf(ch, "Huh?\n\r");
		return;
	}
	
	if(argument[0] == '\0')
	{
		tindex = ch->pcdata->lt_index;
	}
	else if(isalpha(argument[0]) && argument[1] == '\0')
	{
		tindex = tolower(argument[0]) - 'a';
	}
	else
	{
		ch_printf(ch, "You may only index your tell history using "
			"a single letter.\n\r");
		return;
	}
	
   if( ch->pcdata->tell_history[tindex] )
   {
      set_char_color( AT_TELL, ch );
      send_to_char( ch->pcdata->tell_history[tindex], ch );
   }
   else
   {
      send_to_char( "No one like that has sent you a tell.\n\r", ch );
   }
   return;
}


void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    CHAR_DATA *vch;
    EXT_BV actflags;

    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC(ch) )
	xREMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = buf;

		/* Check to see if a player on a map is at the same coords as the recipient 
		   don't need to verify the PLR_ONMAP flags here, it's a room occupants check */
		if( !is_same_map( vch, ch ) )
			continue;
		
		/* Check to see if character is ignoring emoter */
		if(is_ignoring(vch, ch))
		{
			/* continue unless emoter is an immortal */
			if(!IS_IMMORTAL(ch) || get_trust(vch) > get_trust(ch))
				continue;
			else
			{
				set_char_color(AT_IGNORE, vch);
				ch_printf(vch,"You attempt to ignore %s, but"
					" are unable to do so.\n\r", ch->name);
			}
		}
		MOBtrigger = FALSE;
		act( AT_ACTION, "$n $t", ch, sbuf, vch, (vch == ch ? TO_CHAR : TO_VICT) );
	}
/*    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_ROOM );
    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_CHAR );*/
    ch->act = actflags;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_emote_trigger( argument, ch );
    if ( char_died(ch) )
        return;
    oprog_emote_trigger( argument, ch );
    if ( char_died(ch) )
        return;
    rprog_emote_trigger( argument, ch );
    return;
}


void do_rent( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_WHITE, ch );
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}

void do_quit( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;  /* Unused */
    OBJ_DATA *obj_next;
    int x, y;
 
    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING  )
    { 
	set_char_color( AT_RED, ch );
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

/*    if ( ch->position  < POS_STUNNED  )
    {
	set_char_color( AT_BLOOD, ch );
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }*/
    if ( IN_NOQUITROOM(ch))
    {
        send_to_char("This is not a safe spot to quit.\n\r", ch);
        return;
    }
    if (xIS_SET(ch->act, PLR_FUSED))
    {
        send_to_char("Defuse first!\n\r", ch);
        return;
    }

    if (get_timer( ch, TIMER_PROPAGANDA) > 0)
    {
        send_to_char("&RYou can't quit while spreading propaganda.\n\r", ch);
        return;
    }

    if ( get_timer(ch, TIMER_RECENTFIGHT) > 0
    &&  !IS_IMMORTAL(ch) )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
	return;
    }

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller) ) )
    {
	send_to_char("Wait until you have bought/sold the item on auction.\n\r", ch);
	return;

    }
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
      if (obj->item_type == ITEM_DRAGONBALL || obj->item_type == ITEM_DRAGONBALL2)
      {
        extract_obj( obj );
      }
    }
    if ( IS_PKILL( ch ) && ch->wimpy > (int) ch->max_hit / 2.25 )
    {
        send_to_char( "Your wimpy has been adjusted to the maximum level for deadlies.\n\r", ch );
        do_wimpy( ch, "max" );
    }
    /*DBZ Remove Stuff */
    //if (xIS_SET(ch->act, PLR_SPEC)) {do_transfer(ch, 251);}
    if (IS_ICER(ch)) { xSET_BIT(ch->powerup, PU_TRANSFORM1); }
    if ( !IS_ANDROID(ch)){ ch->mod = 1; ch->mod_str = 0; ch->mod_int = 0; ch->mod_dex = 0; ch->mod_wis = 0; ch->mod_con = 0; ch->mod_cha = 0; ch->mod_lck = 0;}
    ch->charge = ch->barrier = ch->suppress = ch->height_mod = ch->weight_mod = 0;
    ch->mod_str = ch->mod_int = ch->mod_wis = ch->mod_dex = ch->mod_cha = ch->mod_con = ch->mod_lck = 0;
    ch->auracolor = URANGE( -1, ch->auracolor, MAX_AURA );
    ch->aurapowerup = -1;
    xCLEAR_BITS( ch->powerup );
    add_timer( ch, TIMER_HEAL, 20, NULL, 0 );
    add_timer( ch, TIMER_GRAVITY, 60, NULL, 0 );
    if (IS_MUTANT(ch))
    add_mutate( ch, 1 );
    if(xIS_SET(ch->act, PLR_GAMBLER))                                   
    xREMOVE_BIT( ch->act, PLR_GAMBLER);

    /*End of DBZ Remove Stuff*/
    /* Get 'em dismounted until we finish mount saving -- Blodkai, 4/97 */
    set_char_color( AT_WHITE, ch );
    send_to_char( "\n\r&WYour surroundings begin to fade as a mystical swirling vortex of colors\n\renvelops your body... When you come to, things are not as they were.\n\r\n\r", ch );
    act( AT_WHITE, "A strange voice says, 'We await your return, $n...'", ch, NULL, NULL, TO_CHAR );
    act( AT_GREEN, "$n has left the game.", ch, NULL, NULL, TO_CANSEE );
    set_char_color( AT_GREY, ch);

    sprintf( log_buf, "%s has quit (Room %d). ", ch->name, (ch->in_room ? ch->in_room->vnum : -1 ) );
    log_string_plus( log_buf, LOG_COMM, ch->level );
    quitting_char = ch;
    save_char_obj( ch );
    save_home(ch);

    if ( ch->plr_home )
    {
      for ( obj = ch->plr_home->first_content; obj; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( obj->item_type == ITEM_SPACESHIP )
            continue;
        extract_obj( obj );
      }
    }

//    save_finger( ch );

    if( ch->pcdata->in_progress )
        free_global_note( ch->pcdata->in_progress );

    /* Synch clandata up only when clan member quits now. --Shaddai
     */
    if ( ch->pcdata->clan )
        save_clan( ch->pcdata->clan );     

    saving_char = NULL;

    /*
     * After extract_char the ch is no longer valid!
     */
    extract_char( ch, TRUE );
    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;

    /* don't show who's logging off to leaving player */
/*
    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", level ); 
*/
    if (xIS_SET(ch->powerup, PLR_BOOST))
    {
      xREMOVE_BIT(ch->powerup, PLR_BOOST);
      check_boost();
    }

    return;
}


void send_rip_screen( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPSCREEN_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_rip_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
//      write_to_buffer(ch->desc,BUFF,num);
      send_to_desc_color(BUFF, ch->desc);
    }
}

void send_ansi_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(ANSITITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
//      write_to_buffer(ch->desc,BUFF,num);
      send_to_desc_color(BUFF, ch->desc);
    }
}

void send_ascii_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH];

    if ((rpfile = fopen(ASCTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
//      write_to_buffer(ch->desc,BUFF,num);
      send_to_desc_color(BUFF, ch->desc);
    }
}



void do_rip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rip ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	send_rip_screen(ch);
	xSET_BIT(ch->act, PLR_RIP);
	xSET_BIT(ch->act, PLR_ANSI);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	xREMOVE_BIT(ch->act,PLR_RIP);
	send_to_char( "!|*\n\rRIP now off...\n\r", ch );
	return;
    }
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "ANSI ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	xSET_BIT(ch->act,PLR_ANSI);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "ANSI ON!!!\n\r", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	xREMOVE_BIT(ch->act,PLR_ANSI);
	send_to_char( "Okay... ANSI support is now off\n\r", ch );
	return;
    }
}

void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;
    if ( NOT_AUTHED(ch) ) {
	send_to_char_color( "\n\r&BYou must be authorized by an Immortal.", ch );
	return;
    }
    WAIT_STATE( ch, 1 ); /* For big muds with save-happy players, like RoD */
    save_char_obj( ch );
    save_home( ch );
    saving_char = NULL;
    send_to_char( "Saved...\n\r", ch );
    return;
}


/*
 * Something from original DikuMUD that Merc yanked out.
 * Used to prevent following loops, which can cause problems if people
 * follow in a loop through an exit leading back into the same room
 * (Which exists in many maze areas)			-Thoric
 */
bool circle_follow( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *tmp;

    for ( tmp = victim; tmp; tmp = tmp->master )
	if ( tmp == ch )
	  return TRUE;
    return FALSE;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }


    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }
    if ( circle_follow( ch, victim ) )
    {
	send_to_char( "Following in loops is not allowed... sorry.\n\r", ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    /* Support for saving pets --Shaddai */

    if ( can_see( master, ch, FALSE ) )
    act( AT_ACTION, "$n now follows you.", ch, NULL, master, TO_VICT );

    act( AT_ACTION, "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_NPC(ch) && !IS_NPC(ch->master) && ch->master->pcdata->pet == ch )
	ch->master->pcdata->pet = NULL;


    if ( can_see( ch->master, ch, FALSE ) )
	if (!(!IS_NPC(ch->master) && IS_IMMORTAL(ch) && !IS_IMMORTAL(ch->master)))
	    act( AT_ACTION, "$n stops following you.",     ch, NULL, ch->master, TO_VICT  );
    act( AT_ACTION, "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = first_char; fch; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }
    return;
}




/* Overhauled 2/97 -- Blodkai */
void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ch->leader ? ch->leader : ch;
        set_char_color( AT_DGREEN, ch );
        ch_printf( ch, "\n\rFollowing %-12.12s            [Life]   [ Energy ]   [Race]\n\r",
		PERS(leader, ch, TRUE));
		
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		set_char_color( AT_DGREEN, ch );
              	    if      ( gch->alignment >  750 ) sprintf(buf, " G");
                    else if ( gch->alignment >  350 ) sprintf(buf, "-G");
                    else if ( gch->alignment >  150 ) sprintf(buf, "+N");
                    else if ( gch->alignment > -150 ) sprintf(buf, " N");
                    else if ( gch->alignment > -350 ) sprintf(buf, "-N");
                    else if ( gch->alignment > -750 ) sprintf(buf, "+E");
                    else                              sprintf(buf, " E");
		    send_to_char( "&g[", ch );
		    ch_printf( ch, "&G%-9d %2.2s %3.3s&g",
			gch->exp, buf,IS_NPC(gch) ? "Mob" : class_table[gch->class]->who_name );
		    send_to_char( "&g]", ch );
                    ch_printf( ch, " &G%-12.12s ",
                        capitalize( PERS(gch, ch, TRUE) ) );
                    ch_printf( ch, "&w%5d/%5d  ", gch->hit, gch->max_hit );
                    ch_printf( ch, "&w%-4d/%-5d", gch->mana, gch->max_mana);
		    ch_printf( ch, "&G%10s",
			gch->race == 0 ? "Android" :
			gch->race == 1 ? "Biodroid" :
			gch->race == 2 ? "Saiyan" :
                        gch->race == 3 ? "Halfbreed" :
                        gch->race == 4 ? "Human" :
                        gch->race == 5 ? "Namek" :
                        gch->race == 6 ? "Icer" :
                        gch->race == 7 ? "Mutant" :
                        gch->race == 8 ? "Buu" : "" );
		    set_char_color( AT_GREEN, ch );
		    send_to_char( "\n\r", ch);
	    }
	}
	return;
    }

    if ( !strcmp( arg, "disband" ))
    {
	CHAR_DATA *gch;
	int count = 0;

	if ( ch->leader || ch->master )
	{
	    send_to_char( "You cannot disband a group if you're following someone.\n\r", ch );
	    return;
	}
	
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( ch, gch )
	    && ( ch != gch ) )
	    {
		gch->leader = NULL;
		gch->master = NULL;
		count++;
		send_to_char( "Your group is disbanded.\n\r", gch );
	    }
	}

	if ( count == 0 )
	   send_to_char( "You have no group members to disband.\n\r", ch );
	else
	   send_to_char( "You disband your group.\n\r", ch );
	
	return;
    }

    if ( !strcmp( arg, "all" ) )
    {
	CHAR_DATA *rch;
	int count = 0;

        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
	{
           if ( ch != rch
           &&   !IS_NPC( rch )
	   &&   can_see( ch, rch, FALSE )
	   &&   rch->master == ch
	   &&   !ch->master
	   &&   !ch->leader
    	   &&   abs( ch->level - rch->level ) < 9
    	   &&   !is_same_group( rch, ch )
   	   &&   IS_PKILL( ch ) == IS_PKILL( rch )
	      )
	   {
		rch->leader = ch;
		count++;
	   }
	}
	
	if ( count == 0 )
	  send_to_char( "You have no eligible group members.\n\r", ch );
	else
	{
     	   act( AT_ACTION, "$n groups $s followers.", ch, NULL, NULL, TO_ROOM );
	   send_to_char( "You group your followers.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( AT_PLAIN, "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
        act( AT_PLAIN, "You can't group yourself.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( AT_ACTION, "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
	act( AT_ACTION, "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }
/*
    if ( ch->level - victim->level < -8
    ||   ch->level - victim->level >  8 
    ||   ( IS_PKILL( ch ) != IS_PKILL( victim ) ) )
    {
	act( AT_PLAIN, "$N cannot join $n's group.",     ch, NULL, victim, TO_NOTVICT );
	act( AT_PLAIN, "You cannot join $n's group.",    ch, NULL, victim, TO_VICT    );
	act( AT_PLAIN, "$N cannot join your group.",     ch, NULL, victim, TO_CHAR    );
	return;
    }*/

    victim->leader = ch;
    act( AT_ACTION, "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT);
    act( AT_ACTION, "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    
    if ( xIS_SET(ch->act, PLR_AUTOGOLD) && members < 2 )
	return;

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    set_char_color( AT_GOLD, ch );
    ch_printf( ch,
	"You split %d gold coins.  Your share is %d gold coins.\n\r",
	amount, share + extra );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }
    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( xIS_SET(ch->act, PLR_NO_TELL) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
/*    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );*/
    for ( gch = first_char; gch; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	{
	    set_char_color( AT_GTELL, gch );
		ch_printf( gch, "&B%s tells the group '&Y%s&B'.&D\n\r", ch->name, argument );
	}
    }

    return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;
    return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf (buf,"&CAuction: %s", argument); /* last %s to reset color */

    for (d = first_descriptor; d; d = d->next)
    {
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) 
        && !IS_SET(original->in_room->room_flags, ROOM_SILENCE) && !NOT_AUTHED(original))
            act( AT_GOSSIP, buf, original, NULL, NULL, TO_CHAR );
    }
}


void do_traffic( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
//    talk_channel( ch, argument, CHANNEL_TRAFFIC, "openly traffic" );
    return;
}

void do_wartalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if (ch->exp < 5000000)
    {
     send_to_char("You talk on wartalk? Your more pacifist than Mahama Ghandi\n\r", ch);
     return;
    }
    talk_channel( ch, argument, CHANNEL_WARTALK, "war" );
    return;
}

void do_racetalk( CHAR_DATA *ch, char *argument )
{
  if (NOT_AUTHED(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  talk_channel( ch, argument, CHANNEL_RACETALK, "RACETALK" );
  return;
}
