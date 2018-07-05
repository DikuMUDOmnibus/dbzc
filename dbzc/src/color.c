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
 *               Color Module -- Allow user customizable Colors.            *
 *                                   --Matthew                              *
 *                      Enhanced ANSI parser by Samson                      *
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

/*
* The following instructions assume you know at least a little bit about
* coding.  I firmly believe that if you can't code (at least a little bit),
* you don't belong running a mud.  So, with that in mind, I don't hold your
* hand through these instructions.
*
* You may use this code provided that:
*
*     1)  You understand that the authors _DO NOT_ support this code
*         Any help you need must be obtained from other sources.  The
*         authors will ignore any and all requests for help.
*     2)  You will mention the authors if someone asks about the code.
*         You will not take credit for the code, but you can take credit
*         for any enhancements you make.
*     3)  This message remains intact.
*
* If you would like to find out how to send the authors large sums of money,
* you may e-mail the following address:
*
* Matthew Bafford & Christopher Wigginton
* wiggy@mudservices.com
*/

/*
 * To add new color types:
 *
 * 1.  Edit color.h, and:
 *     1.  Add a new AT_ define.
 *     2.  Increment MAX_COLORS by however many AT_'s you added.
 * 2.  Edit color.c and:
 *     1.  Add the name(s) for your new color(s) to the end of the pc_displays array.
 *     2.  Add the default color(s) to the end of the default_set array.
 */

#include <string.h>
#include "mud.h"

char * const pc_displays[MAX_COLORS] =
{
   "black",      "dred",      "dgreen",      "orange",   "dblue",
   "purple",     "cyan",      "grey",        "dgrey",    "red",
   "green",      "yellow",    "blue",        "pink",     "lblue",
   "white",      "blink",     "plain",       "action",   "say",
   "chat",       "yells",     "tell",        "hit",      "hitme",
   "immortal",   "hurting",   "falling",     "danger",   "magic",
   "consider",   "report",    "poison",      "social",   "dying",
   "dead",       "skills",    "carnage",     "damage",   "fleeing",
   "rmname",     "rmdesc",    "objects",     "people",   "list",
   "bye",        "gold",      "gtells",      "note",     "hungry",
   "thirsty",    "fire",      "sober",       "wearoff",  "exits",
   "score",      "reset",     "log",         "die_msg",  "wartalk",
   "arena",      "muse",      "think",       "aflags",   "who",
   "racetalk",   "ignore",    "whisper",     "divider",  "morph",
   "shout",      "rflags",    "stype",       "aname",    "auction",
   "score2",     "score3",    "score4",      "who2",     "who3",
   "who4",	 "intermud",  "helpfiles",   "who5", "corpse" 
};

/* All defaults are set to Alsherok default scheme, if you don't 
like it, change it around to suite your own needs - Samson */
const sh_int default_set [MAX_COLORS] =
{
     AT_BLACK,     AT_BLOOD,  AT_DGREEN,     AT_ORANGE,
     AT_DBLUE,     AT_PURPLE, AT_CYAN,       AT_WHITE,
     AT_DGREY,     AT_RED,    AT_GREEN,      AT_YELLOW,
     AT_BLUE,      AT_PINK,   AT_LBLUE,      AT_GREY,
/*blink     plain     action    say       chat      yells     tell      hit       
hitme     immortal  hurting   falling   danger    magic     consider  report    
poison    social    dying     dead      skills    carnage   damage    fleeing   
rmname    rmdesc    objects   people    list      bye       gold      gtells    
note      hungry    thirsty   fire      sober     wearoff   exits     score     
reset     log       die_msg   wartalk   arena     muse      think     aflags    
who       racetalk  ignore    whisper   divider   morph     shout     rflags    
stype     aname     auction   score2    score3    score4    who2      who3      
who4      intermud  helpfiles who5  */
 AT_RED+AT_BLINK,  AT_GREY,   AT_GREY,      AT_WHITE,
     AT_WHITE,     AT_LBLUE,  AT_WHITE,     AT_WHITE,
     AT_YELLOW,    AT_YELLOW, AT_RED,       AT_RED,
     AT_RED,       AT_BLUE,   AT_GREY,      AT_GREY,
     AT_DGREEN,    AT_CYAN,   AT_GREY,      AT_GREY,
     AT_GREEN,      AT_GREY,   AT_GREY,      AT_GREY,
     AT_RED,       AT_GREY,   AT_RED,      AT_WHITE,
     AT_GREY,      AT_GREY,   AT_YELLOW,    AT_GREY,
     AT_GREY,      AT_ORANGE, AT_BLUE,      AT_RED,
     AT_GREY,      AT_GREY,   AT_GREEN,     AT_GREY,
     AT_DGREEN,    AT_ORANGE, AT_GREY,      AT_RED,
     AT_GREY,      AT_DGREEN, AT_RED,       AT_BLUE,
     AT_GREEN,     AT_CYAN,   AT_YELLOW,    AT_WHITE,
     AT_DGREEN,    AT_PINK,   AT_WHITE,     AT_BLUE,
     AT_BLUE,      AT_BLUE,   AT_GREEN,     AT_GREEN,
     AT_YELLOW,    AT_LBLUE,  AT_GREY,      AT_BLUE,
     AT_GREY,      AT_PINK,   AT_DGREEN,    AT_CYAN, AT_DGREY
};

char * const valid_color[] =
{
   "black",
   "dred",
   "dgreen",
   "orange",
   "dblue",
   "purple",
   "cyan",
   "grey",
   "dgrey",
   "red",
   "green",
   "yellow",
   "blue",
   "pink",
   "lblue",
   "white",
   "\0"
};

/* Color align functions by Justice@Aaern */
int const_color_str_len( const char *argument )
{
    int  str, count = 0;
    bool IS_COLOR = FALSE;

    for ( str = 0; argument[str] != '\0'; str++ )
    {
        if ( argument[str] == '&' )
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
#ifdef OVERLANDCODE
	  else if ( argument[str] == '{' )
#else
        else if ( argument[str] == '^' )
#endif
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
	  else if ( argument[str] == '}' )
	  {
		if ( IS_COLOR == TRUE )
		{
		    count++;
		    IS_COLOR = FALSE;
		}
		else
		    IS_COLOR = TRUE;
	  } 
        else
        {
            if ( IS_COLOR == FALSE ) count++;
            else IS_COLOR = FALSE;
        }
    }

    return count;
}

int const_color_strnlen( const char *argument, int maxlength )
{
    int str, count = 0;
    bool IS_COLOR = FALSE;

    for ( str = 0; argument[str] != '\0'; str++ )
    {
        if ( argument[str] == '&' )
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
#ifdef OVERLANDCODE
	  else if ( argument[str] == '{' )
#else
        else if ( argument[str] == '^' )
#endif
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
	  else if ( argument[str] == '}' )
	  {
		if ( IS_COLOR == TRUE )
		{
		    count++;
		    IS_COLOR = FALSE;
		}
		else
		    IS_COLOR = TRUE;
	  }
        else
        {
            if ( IS_COLOR == FALSE ) count++;
            else IS_COLOR = FALSE;
        }

	if ( count >= maxlength ) break;
    }
    if ( count < maxlength ) return ((str - count) + maxlength);

    str++;
    return str;
}

int color_str_len( char *argument )
{
    int str, count = 0;
    bool IS_COLOR = FALSE;

    for ( str = 0; argument[str] != '\0'; str++ )
    {
        if ( argument[str] == '&' )
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
#ifdef OVERLANDCODE
	  else if ( argument[str] == '{' )
#else
        else if ( argument[str] == '^' )
#endif
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
	  else if ( argument[str] == '}' )
	  {
		if ( IS_COLOR == TRUE )
		{
		    count++;
		    IS_COLOR = FALSE;
		}
		else
		    IS_COLOR = TRUE;
	  }
        else
        {
            if ( IS_COLOR == FALSE ) count++;
            else IS_COLOR = FALSE;
        }
    }

    return count;
}

int color_strnlen( char *argument, int maxlength )
{
    int str, count = 0;
    bool IS_COLOR = FALSE;

    for ( str = 0; argument[str] != '\0'; str++ )
    {
        if ( argument[str] == '&' )
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
#ifdef OVERLANDCODE
	  else if ( argument[str] == '{' )
#else
        else if ( argument[str] == '^' )
#endif
        {
            if ( IS_COLOR == TRUE )
            {
                count++;
                IS_COLOR = FALSE;
            }
            else
                IS_COLOR = TRUE;
        }
	  else if ( argument[str] == '}' )
	  {
		if ( IS_COLOR == TRUE )
		{
		    count++;
		    IS_COLOR = FALSE;
		}
		else
		    IS_COLOR = TRUE;
	  }
        else
        {
            if ( IS_COLOR == FALSE ) count++;
            else IS_COLOR = FALSE;
        }

	if ( count >= maxlength ) break;
    }
    if ( count < maxlength ) return ((str - count) + maxlength );

    str++;
    return str;
}

const char *const_color_align( const char *argument, int size, int align )
{
    int space = ( size - const_color_str_len( argument ) );
    static char buf[MAX_STRING_LENGTH];

    if ( align == ALIGN_RIGHT || const_color_str_len( argument ) >= size )
        sprintf( buf, "%*.*s", const_color_strnlen( argument, size ),
            const_color_strnlen( argument, size ), argument );
    else if ( align == ALIGN_CENTER )
        sprintf( buf, "%*s%s%*s", ( space/2 ),"",argument,
            ((space/2)*2)==space ? (space/2) : ((space/2)+1),"" );
    else
        sprintf( buf, "%s%*s", argument, space, "" );

    return buf;
}

char *color_align( char *argument, int size, int align )
{
    int space = ( size - color_str_len( argument ) );
    static char buf[MAX_STRING_LENGTH];

    if ( align == ALIGN_RIGHT || color_str_len( argument ) >= size )
        sprintf( buf, "%*.*s", color_strnlen( argument, size ),
            color_strnlen( argument, size ), argument );
    else if ( align == ALIGN_CENTER )
        sprintf( buf, "%*s%s%*s", ( space/2 ),"",argument,
            ((space/2)*2)==space ? (space/2) : ((space/2)+1),"" );
    else if ( align == ALIGN_LEFT )
        sprintf( buf, "%s%*s", argument, space, "" );

    return buf;
}

void show_colors( CHAR_DATA *ch )
{
   sh_int count;

   send_to_pager_color( "\n\r&BSyntax: color [color type] [color] | default\n\r", ch );
   send_to_pager_color( "&BSyntax: color _reset_ (Resets all colors to default set)\n\r", ch );
   send_to_pager_color( "&BSyntax: color _all_ [color] (Sets all color types to [color])\n\r\n\r", ch );

   send_to_pager_color( "&W********************************[ COLORS ]*********************************\n\r", ch );

   for ( count = 0; count < 16; ++count ) 
   {
      if ( (count % 8) == 0 && count != 0 ) 
      {
        send_to_pager( "\n\r",ch );
      }
      pager_printf( ch, "%s%-10s", color_str( count, ch ), pc_displays[count] );
   }

   send_to_pager( "\n\r\n\r&W******************************[ COLOR TYPES ]******************************\n\r", ch );

   for ( count = 16; count < MAX_COLORS; ++count ) 
   {
      if ( (count % 8) == 0 && count != 16) 
	{
        send_to_pager( "\n\r",ch );
      }
      pager_printf( ch, "%s%-10s%s", color_str( count, ch ), pc_displays[count], ANSI_RESET );
   }
   send_to_pager( "\n\r\n\r", ch );
   send_to_pager( "&YAvailable colors are:\n\r", ch );

   for ( count = 0; valid_color[count][0] != '\0'; ++count) 
   {
     if ( (count%8) == 0 && count != 0 )
        send_to_pager( "\n\r", ch );

     pager_printf( ch, "%s%-10s", color_str( AT_PLAIN, ch ), valid_color[count] );
   }
   send_to_pager( "\n\r", ch );
   return;
}

void do_color( CHAR_DATA *ch, char *argument ) 
{
   bool dMatch, cMatch;
   sh_int count = 0, y = 0;
   char arg[MIL];
   char arg2[MIL];
   char arg3[MIL];
   char buf[MSL];

   dMatch = FALSE;
   cMatch = FALSE;

   if (IS_NPC(ch)) 
   {
     send_to_pager( "Only PC's can change colors.\n\r", ch );
     return;
   }

   if ( !argument || argument[0] == '\0' ) 
   {
     show_colors( ch );
     return;
   }

   argument = one_argument( argument, arg );

   if( !str_cmp( arg, "ansitest" ) )
   {
	sprintf( log_buf, "%sBlack\n\r", ANSI_BLACK );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Red\n\r", ANSI_DRED );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Green\n\r", ANSI_DGREEN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sOrange/Brown\n\r", ANSI_ORANGE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Blue\n\r", ANSI_DBLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sPurple\n\r", ANSI_PURPLE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sCyan\n\r", ANSI_CYAN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sGrey\n\r", ANSI_GREY );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Grey\n\r", ANSI_DGREY );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sRed\n\r", ANSI_RED );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sGreen\n\r", ANSI_GREEN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sYellow\n\r", ANSI_YELLOW );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sBlue\n\r", ANSI_BLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sPink\n\r", ANSI_PINK );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sLight Blue\n\r", ANSI_LBLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sWhite\n\r", ANSI_WHITE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sBlack\n\r", BLINK_BLACK );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Red\n\r", BLINK_DRED );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Green\n\r", BLINK_DGREEN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sOrange/Brown\n\r", BLINK_ORANGE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Blue\n\r", BLINK_DBLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sPurple\n\r", BLINK_PURPLE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sCyan\n\r", BLINK_CYAN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sGrey\n\r", BLINK_GREY );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sDark Grey\n\r", BLINK_DGREY );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sRed\n\r", BLINK_RED );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sGreen\n\r", BLINK_GREEN );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sYellow\n\r", BLINK_YELLOW );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sBlue\n\r", BLINK_BLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sPink\n\r", BLINK_PINK );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sLight Blue\n\r", BLINK_LBLUE );
	write_to_buffer( ch->desc, log_buf, 0 );
	sprintf( log_buf, "%sWhite\n\r", BLINK_WHITE );
	write_to_buffer( ch->desc, log_buf, 0 );
      write_to_buffer( ch->desc, ANSI_RESET, 0 );
      sprintf( log_buf, "%s%sBlack\n\r", ANSI_WHITE, BACK_BLACK );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sDark Red\n\r", ANSI_BLACK, BACK_DRED );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sDark Green\n\r", ANSI_BLACK, BACK_DGREEN );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sOrange/Brown\n\r", ANSI_BLACK, BACK_ORANGE );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sDark Blue\n\r", ANSI_BLACK, BACK_DBLUE );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sPurple\n\r", ANSI_BLACK, BACK_PURPLE );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sCyan\n\r", ANSI_BLACK, BACK_CYAN );
      write_to_buffer( ch->desc, log_buf, 0 );
      sprintf( log_buf, "%s%sGrey\n\r", ANSI_BLACK, BACK_GREY );
      write_to_buffer( ch->desc, log_buf, 0 );
	write_to_buffer( ch->desc, ANSI_RESET, 0 );

	return;
   }

   if ( !str_prefix(arg, "_reset_" ) ) 
   {
     reset_colors(ch);
     send_to_pager( "All color types reset to default colors.\n\r", ch );
     return;
   }
   
   argument = one_argument( argument, arg2 );

   if ( arg[0] == '\0' )
   {
    send_to_char("Change which color type?\r\n", ch);
    return;
   }

   argument = one_argument( argument, arg3 );

   if ( !str_prefix(arg, "_all_") )
   {
     dMatch = TRUE;
     count = -1;

     /* search for a valid color setting*/
     for ( y = 0; y < 16 ; y++ ) 
     {
       if (!str_cmp(arg2, valid_color[y])) 
       {
         cMatch = TRUE;
         break;
       }
     }
   }
   else if ( arg2[0] == '\0' )
   {
     cMatch = FALSE;
   }
   else
   {
     /* search for the display type and strcmp*/
     for ( count = 0; count < MAX_COLORS ; count++ ) 
     {
       if (!str_prefix (arg, pc_displays[count])) 
       {
         dMatch = TRUE;
         break;
       }
     }

     if(!dMatch) 
     {
       ch_printf( ch, "%s is an invalid color type.\n\r", arg );
       send_to_char( "Type color with no arguments to see available options.\n\r", ch );
       return;
     }

     if (!str_cmp (arg2, "default")) 
     {
       ch->colors[count] = default_set[count];
       sprintf(buf, "Display %s set back to default.\n\r", pc_displays[count] );
       send_to_pager( buf, ch );
       return;
     }

     /* search for a valid color setting*/
     for ( y = 0; y < 16 ; y++ ) 
     {
       if (!str_cmp(arg2, valid_color[y])) 
       {
          cMatch = TRUE;
          break;
       }
     }
   }

   if(!cMatch)
   {
     if ( arg[0] )
     {
       ch_printf(ch, "Invalid color for type %s.\n", arg);
     }
     else
     {
       send_to_pager( "Invalid color.\n\r", ch );
     }

     send_to_pager( "Choices are:\n\r", ch );

     for ( count = 0; count < 16; count++ )
     {
       if ( count % 5 == 0 && count != 0 )
         send_to_pager("\r\n", ch);

       pager_printf(ch, "%-10s", valid_color[count]);
     }

     pager_printf(ch, "%-10s\r\n", "default");
     return;
   }
   else
   {
     sprintf(buf, "Color type %s set to color %s.\n\r",count==-1?"_all_":pc_displays[count], valid_color[y] );
   }

   if (!str_cmp (arg3, "blink"))
   {
     y += AT_BLINK;
   }

   if ( count == -1 )
   {
     int ccount;

     for ( ccount = 0; ccount < MAX_COLORS; ++ccount )
     {
       ch->colors[ccount] = y;
     }

     set_pager_color(y, ch);

     sprintf(buf, "All color types set to color %s%s.%s\n\r",
        valid_color[y>AT_BLINK?y-AT_BLINK:y], y>AT_BLINK ? " [BLINKING]" : "", ANSI_RESET );

     send_to_pager(buf, ch);
   }
   else
   {
     ch->colors[count] = y;

     set_pager_color( count , ch);

     if (!str_cmp (arg3, "blink"))
       sprintf(buf, "Display %s set to color %s [BLINKING]%s\n\r", pc_displays[count], valid_color[y-AT_BLINK], ANSI_RESET );
     else
       sprintf(buf, "Display %s set to color %s.\n\r", pc_displays[count], valid_color[y] );

     send_to_pager( buf, ch );
   }
   set_pager_color( AT_PLAIN , ch);

   return;
}



void reset_colors( CHAR_DATA *ch )
{
   memcpy( &ch->colors, &default_set, sizeof(default_set) );
}


/*
 * If the color directory has a file named "default" this will be used if possible.
 * The fallback will go to the hardcoded table in color.h if the "default" file is not available.
 */
/*
void reset_colors( CHAR_DATA * ch )
{
   FILE *fp;
   int max_colors = 0;
   char filename[256];

   snprintf( filename, 256, "%s%s", COLOR_DIR, "default" );

   if( !( fp = fopen( filename, "r" ) ) )
   {
      memcpy( &ch->colors, &default_set, sizeof( default_set ) );
      return;
   }

   while( !feof( fp ) )
   {
      char *word = fread_word( fp );
      if( !str_cmp( word, "MaxColors" ) )
      {
         max_colors = fread_number( fp );
         continue;
      }
      if( !str_cmp( word, "Colors" ) )
      {
         int x;

         for( x = 0; x < max_colors; ++x )
            ch->colors[x] = fread_number( fp );
         continue;
      }
      if( !str_cmp( word, "End" ) )
      {
         fclose( fp );
         fp = NULL;
         return;
      }
   }
   fclose( fp );
   fp = NULL;
   return;
}
*/

char *color_str( sh_int AType, CHAR_DATA *ch )
{
   if( !ch )
   {
	bug( "%s", "color_str: NULL ch!" );
	return( "" );
   }

   if( IS_NPC(ch) || !xIS_SET( ch->act, PLR_ANSI ) )
      return( "" );

   switch( ch->colors[AType] )
   {
	case 0:  return( ANSI_BLACK );	break;
	case 1:  return( ANSI_DRED );		break;
	case 2:  return( ANSI_DGREEN );	break;
	case 3:  return( ANSI_ORANGE );	break;
	case 4:  return( ANSI_DBLUE );	break;
	case 5:  return( ANSI_PURPLE );	break;
	case 6:  return( ANSI_CYAN );		break;
	case 7:  return( ANSI_GREY );		break;
	case 8:  return( ANSI_DGREY );	break;
	case 9:  return( ANSI_RED );		break;
	case 10: return( ANSI_GREEN );	break;
	case 11: return( ANSI_YELLOW );	break;
	case 12: return( ANSI_BLUE );		break;
	case 13: return( ANSI_PINK );		break;
	case 14: return( ANSI_LBLUE );	break;
	case 15: return( ANSI_WHITE );	break;

	/* 16 thru 31 are for blinking colors */
	case 16: return( BLINK_BLACK );	break;
	case 17: return( BLINK_DRED );	break;
	case 18: return( BLINK_DGREEN );	break;
	case 19: return( BLINK_ORANGE );	break;
	case 20: return( BLINK_DBLUE );	break;
	case 21: return( BLINK_PURPLE );	break;
	case 22: return( BLINK_CYAN );	break;
	case 23: return( BLINK_GREY );	break;
	case 24: return( BLINK_DGREY );	break;
	case 25: return( BLINK_RED );	      break;
	case 26: return( BLINK_GREEN );	break;
	case 27: return( BLINK_YELLOW );	break;
	case 28: return( BLINK_BLUE );	break;
	case 29: return( BLINK_PINK );	break;
	case 30: return( BLINK_LBLUE );	break;
	case 31: return( BLINK_WHITE );	break;

	default: return( ANSI_RESET );	break;
   }
}

int colorcode( const char *col, char *code, CHAR_DATA *ch )
{
   const char *ctype = col;
   int ln;
   bool ansi;

   if( !ch )
      ansi = TRUE;
   else
	ansi = ( !IS_NPC(ch) && xIS_SET( ch->act, PLR_ANSI ) );

   col++;

   if( !*col )
      ln = -1; 
#ifdef OVERLANDCODE
   else if ( *ctype != '&' && *ctype != '{' && *ctype != '}' )
   {
      bug( "colorcode: command '%c' not '&', '{' or '}'", *ctype );
      ln = -1;
   }
#else
   else if ( *ctype != '&' && *ctype != '^' && *ctype != '}' )
   {
      bug( "colorcode: command '%c' not '&', '^' or '}'", *ctype );
      ln = -1;
   }
#endif
   else if( *col == *ctype )
   {
      code[0] = *col;
      code[1] = '\0';
      ln = 1;
   }
   else if( !ansi )
      ln = 0;
   else
   {
	/* Foreground text - non-blinking */
	if( *ctype == '&' )
	{
         switch( *col )
         {
            default:
		   code[0] = *ctype;
		   code[1] = *col;
		   code[2] = '\0';
		   return 2;
		case 'i': /* Italic text */
		case 'I':
		   strcpy( code, ANSI_ITALIC );
		   break;
		case 'v': /* Reverse colors */
		case 'V':
		   strcpy( code, ANSI_REVERSE );
		   break;
		case 'u': /* Underline */
		case 'U':
		   strcpy( code, ANSI_UNDERLINE );
		   break;
		case 's': /* Strikeover */
		case 'S':
		   strcpy( code, ANSI_STRIKEOUT );
		   break;
	      case 'd': /* Player's client default color */
		   strcpy( code, ANSI_RESET );
		   break;
	      case 'D': /* Reset to custom color for whatever is being displayed */
		   strcpy( code, ANSI_RESET ); /* Yes, this reset here is quite necessary to cancel out other things */
		   strcat( code, color_str( ch->desc->pagecolor, ch ) );
		   break;
            case 'x': /* Black */
               strcpy( code, ANSI_BLACK );
               break;
            case 'O': /* Orange/Brown */
               strcpy( code, ANSI_ORANGE );
               break;
            case 'c': /* Cyan */
               strcpy( code, ANSI_CYAN );
               break;
            case 'z': /* Dark Grey */
               strcpy( code, ANSI_DGREY );
               break;
            case 'g': /* Dark Green */
               strcpy( code, ANSI_DGREEN );
               break;
            case 'G': /* Light Green */
               strcpy( code, ANSI_GREEN );
               break;
            case 'P': /* Pink/Light Purple */
               strcpy( code, ANSI_PINK );
               break;
            case 'r': /* Dark Red */
               strcpy( code, ANSI_DRED );
               break;
            case 'b': /* Dark Blue */
               strcpy( code, ANSI_DBLUE );
               break;
            case 'w': /* Grey */
               strcpy( code, ANSI_GREY );
               break;
            case 'Y': /* Yellow */
               strcpy( code, ANSI_YELLOW );
               break;
            case 'C': /* Light Blue */
               strcpy( code, ANSI_LBLUE );
               break;
            case 'p': /* Purple */
               strcpy( code, ANSI_PURPLE );
               break;
            case 'R': /* Red */
               strcpy( code, ANSI_RED );
               break;
            case 'B': /* Blue */
               strcpy( code, ANSI_BLUE );
               break;
            case 'W': /* White */
               strcpy( code, ANSI_WHITE );
               break;
         }
	}
	/* Foreground text - blinking */
	if( *ctype == '}' )
	{
         switch( *col )
         {
            default:
		   code[0] = *ctype;
		   code[1] = *col;
		   code[2] = '\0';
		   return 2;
            case 'x': /* Black */
               strcpy( code, BLINK_BLACK );
               break;
            case 'O': /* Orange/Brown */
               strcpy( code, BLINK_ORANGE );
               break;
            case 'c': /* Cyan */
               strcpy( code, BLINK_CYAN );
               break;
            case 'z': /* Dark Grey */
               strcpy( code, BLINK_DGREY );
               break;
            case 'g': /* Dark Green */
               strcpy( code, BLINK_DGREEN );
               break;
            case 'G': /* Light Green */
               strcpy( code, BLINK_GREEN );
               break;
            case 'P': /* Pink/Light Purple */
               strcpy( code, BLINK_PINK );
               break;
            case 'r': /* Dark Red */
               strcpy( code, BLINK_DRED );
               break;
            case 'b': /* Dark Blue */
               strcpy( code, BLINK_DBLUE );
               break;
            case 'w': /* Grey */
               strcpy( code, BLINK_GREY );
               break;
            case 'Y': /* Yellow */
               strcpy( code, BLINK_YELLOW );
               break;
            case 'C': /* Light Blue */
               strcpy( code, BLINK_LBLUE );
               break;
            case 'p': /* Purple */
               strcpy( code, BLINK_PURPLE );
               break;
            case 'R': /* Red */
               strcpy( code, BLINK_RED );
               break;
            case 'B': /* Blue */
               strcpy( code, BLINK_BLUE );
               break;
            case 'W': /* White */
               strcpy( code, BLINK_WHITE );
               break;
         }
	}
	/* Background color */
#ifdef OVERLANDCODE
	if( *ctype == '{' )
#else
	if( *ctype == '^' )
#endif
	{
         switch( *col )
         {
            default:
		   code[0] = *ctype;
		   code[1] = *col;
		   code[2] = '\0';
		   return 2;
            case 'x': /* Black */
               strcpy( code, BACK_BLACK );
               break;
            case 'r': /* Dark Red */
               strcpy( code, BACK_DRED );
               break;
            case 'g': /* Dark Green */
               strcpy( code, BACK_DGREEN );
               break;
            case 'O': /* Orange/Brown */
               strcpy( code, BACK_ORANGE );
               break;
            case 'b': /* Dark Blue */
               strcpy( code, BACK_DBLUE );
               break;
            case 'p': /* Purple */
               strcpy( code, BACK_PURPLE );
               break;
            case 'c': /* Cyan */
               strcpy( code, BACK_CYAN );
               break;
            case 'w': /* Grey */
               strcpy( code, BACK_GREY );
               break;
         }
	}
      ln = strlen( code );
   }
   if ( ln <= 0 )
      *code = '\0';
   return ln;
}

/* Moved from comm.c */
void set_char_color( sh_int AType, CHAR_DATA *ch )
{
   if ( !ch || !ch->desc )
      return;

   write_to_buffer( ch->desc, color_str( AType, ch ), 0 );
   ch->desc->pagecolor = ch->colors[AType];
}

void set_pager_color( sh_int AType, CHAR_DATA *ch )
{
   if ( !ch || !ch->desc )
      return;
    
   write_to_pager( ch->desc, color_str( AType, ch ), 0 );
   ch->desc->pagecolor = ch->colors[AType];
}

/* Writes to a descriptor, usually best used when there's no character to send to ( like logins ) */
/*
void send_to_desc_color( const char *txt, DESCRIPTOR_DATA *d )
{
   char *colstr;
   const char *prevstr = txt;
   char colbuf[20];
   int ln;

   if ( !d )
   {
      bug( "%s", "send_to_desc_color: NULL *d" );
      return;
   }

   if ( !txt || !d->descriptor )
      return;

#ifdef OVERLANDCODE
   while( ( colstr = strpbrk( prevstr, "&{}" ) ) != NULL )
#else
   while( ( colstr = strpbrk( prevstr, "&^}" ) ) != NULL )
#endif
   {
      if( colstr > prevstr )
         write_to_buffer( d, prevstr, ( colstr-prevstr ) );

      ln = colorcode( colstr, colbuf, d->character );
      if ( ln < 0 )
      {
         prevstr = colstr+1;
         break;
      }

      else if ( ln > 0 )
         write_to_buffer( d, colbuf, ln );
      prevstr = colstr+2;
   }
   if ( *prevstr )
      write_to_buffer( d, prevstr, 0 );

   return;
}*/

void send_to_desc_color( const char *txt, DESCRIPTOR_DATA *d )
{
  char *colstr;
  const char *prevstr = txt;
  char colbuf[20];
  int ln;
  
  if ( !d )
  {
    bug( "Send_to_desc_color: NULL *d" );
    return;
  }
  if ( !txt || !d )
    return;

  /* Clear out old color stuff */
  while ( (colstr = strpbrk(prevstr, "&^")) != NULL )
  {
    if (colstr > prevstr)
       write_to_buffer(d, prevstr, (colstr-prevstr));
      ln = colorcode( colstr, colbuf, d->character );
    if ( ln < 0 )
    {
      prevstr = colstr+1;
      break;
    }
    else if ( ln > 0 )
      write_to_buffer(d, colbuf, ln);
    prevstr = colstr+2;
  }
  if ( *prevstr )
    write_to_buffer(d, prevstr, 0);
  return;
}

/*
 * Write to one char. Convert color into ANSI sequences.
 */
void send_to_char_color( const char *txt, CHAR_DATA *ch )
{
   char *colstr;
   const char *prevstr = txt;
   char colbuf[20];
   int ln;

   if( !ch )
   {
	bug( "%s", "send_to_char_color: NULL ch!" );
	return;
   }

   if( txt && ch->desc )
   {

#ifdef OVERLANDCODE
      while( ( colstr = strpbrk( prevstr, "&{}" ) ) != NULL )
#else
      while( ( colstr = strpbrk( prevstr, "&^}" ) ) != NULL )
#endif
      {
         if( colstr > prevstr )
            write_to_buffer( ch->desc, prevstr, ( colstr-prevstr ) );
         ln = colorcode( colstr, colbuf, ch );
         if ( ln < 0 )
         {
            prevstr = colstr+1;
            break;
         }
         else if( ln > 0 )
            write_to_buffer( ch->desc, colbuf, ln );
         prevstr = colstr+2;
      }
      if ( *prevstr )
         write_to_buffer( ch->desc, prevstr, 0 );
   }
   return;
}

void send_to_pager_color( const char *txt, CHAR_DATA *ch )
{
   char *colstr;
   const char *prevstr = txt;
   char colbuf[20];
   int ln;

   if( !ch )
   {
	bug( "%s", "send_to_pager_color: NULL ch!" );
	return;
   }

   if ( txt && ch->desc )
   {
      DESCRIPTOR_DATA *d = ch->desc;
    
      ch = d->original ? d->original : d->character;
      if ( IS_NPC(ch) || !xIS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) )
      {
	   send_to_char_color( txt, d->character );
	   return;
      }
#ifdef OVERLANDCODE
      while( ( colstr = strpbrk( prevstr, "&{}" ) ) != NULL )
#else
      while( ( colstr = strpbrk( prevstr, "&^}" ) ) != NULL )
#endif
      {
         if( colstr > prevstr )
            write_to_pager( ch->desc, prevstr, ( colstr-prevstr ) );
         ln = colorcode( colstr, colbuf, ch );
         if ( ln < 0 )
         {
            prevstr = colstr+1;
            break;
         }
         else if( ln > 0 )
            write_to_pager( ch->desc, colbuf, ln );
         prevstr = colstr+2;
      }
      if ( *prevstr )
         write_to_pager( ch->desc, prevstr, 0 );
   }
   return;
}

void send_to_char( const char *txt, CHAR_DATA *ch )
{
   send_to_char_color( txt, ch );
   return;
}

void send_to_pager( const char *txt, CHAR_DATA *ch )
{
   send_to_pager_color( txt, ch );
   return;
}

void ch_printf( CHAR_DATA *ch, char *fmt, ... )
{
     char buf[MSL*2];
     va_list args;
 
     va_start( args, fmt );
     vsprintf( buf, fmt, args );
     va_end( args );
 
     send_to_char_color( buf, ch );
}

void pager_printf( CHAR_DATA *ch, char *fmt, ... )
{
   char buf[MSL*2];
   va_list args;
 
   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );
 
   send_to_pager_color( buf, ch );
}


void buffer_printf(DESCRIPTOR_DATA *d, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
}

void log_printf( sh_int log_type, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
	
    log_string_plus( buf, log_type, LEVEL_LOG );
}

void command( CHAR_DATA *ch, DO_FUN *command, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
	
    (command)( ch, buf);
}

void stralloc_printf( char **pointer, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
	
    if (*pointer)
     STRFREE(*pointer);
    *pointer = STRALLOC( buf);
    return;
}

void strdup_printf( char **pointer, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
	
    if (*pointer)
     DISPOSE(*pointer);
    *pointer = str_dup( buf);
    return;
}

CHAR_DATA *get_mob(int vnum)
{
    CHAR_DATA *wch;

    if ( get_mob_index( vnum) == NULL)
     return NULL;

    /* check the world for an exact match */
    for ( wch = first_char; wch; wch = wch->next )
	if (IS_NPC(wch) && vnum == wch->pIndexData->vnum )
		return wch;

    return NULL;
}

OBJ_DATA *obj_by_vnum(int vnum)
{
    OBJ_DATA *obj;

    if ( get_obj_index( vnum) == NULL)
     return NULL;

    for ( obj = first_object; obj; obj = obj->next )
	if ( vnum == obj->pIndexData->vnum)
		return obj;

    return NULL;
}

