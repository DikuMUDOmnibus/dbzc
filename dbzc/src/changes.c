 /*********************************************************************************** 
 *                                                                                  *
 *          _______.____    __    ____       _______                 _______        *
 *         /       )\   \  /  \  /   /  _   |   ____)         __    |   ____)       *
 *        (   (----` \   \/    \/   /  (_)  |  |__    ___   _/  |_  |  |__          *
 *         \   \      \            /    _   |   __)  / _ \ (_   __) |   __)         *
 *     .----)   )      \    /\    /    (_)  |  |    ( (_) )  |  |   |  |____        *
 *    (________/        \__/  \__/          |__|     \___/   |__|   |_______)       *
 *                                                                                  *
 * SWFotE v2.0 (FotE v1.1 cleaned up and considerably modded) by                    *
 * Greg (Keberus) Mosley                                                            *
 * Roman (Trelar) Arnold                                                            *
 *                                                                                  *
 * SWFotE v1 & v1.1 copyright (c) 2002 was created by                               *
 * Chris 'Tawnos' Dary (cadary@uwm.edu),                                            *
 * Korey 'Eleven' King (no email),                                                  *
 * Matt 'Trillen' White (mwhite17@ureach.com),                                      *
 * Daniel 'Danimal' Berrill (danimal924@yahoo.com),                                 *
 * Richard 'Bambua' Berrill (email unknown),                                        *
 * Stuart 'Ackbar' Unknown (email unknown)                                          *
 *                                                                                  *
 * SWR 1.0 copyright (c) 1997, 1998 was created by Sean Cooper                      *
 * based on a concept and ideas from the original SWR immortals:                    *
 * Himself (Durga), Mark Matt (Merth), Jp Coldarone (Exar), Greg Baily (Thrawn),    *
 * Ackbar, Satin, Streen and Bib as well as much input from our other builders      *
 * and players.                                                                     *
 *                                                                                  *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,                *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,                *
 * Grishnakh, Fireblade, and Nivek.                                                 *
 *                                                                                  *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                              *
 *                                                                                  *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,              *
 * Michael Seifert, and Sebastian Hammer.                                           *
 *                                                                                  *
 ***********************************************************************************/
/******************************************************************
 *                [D]ragon [B]all [Z] [C]hronicles                *
 *----------------------------------------------------------------*
 * DBZB, DBZDL, DBZC versions done by: Seventeen and Vegeta       *
 *----------------------------------------------------------------*
 * All modifications made by Seventeen and Vegeta are sole        *
 * property of them. Credit must be given if any part is ripped   *
 * or used from this version or any other version of DBZC.        *
 *****************************************************************/


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "mud.h"
//#include "changes.h"

/*
 * Globals
 */
char * current_date   args( ( void ) );
int    num_changes    args( ( void ) );

/*
 * Local Functions
 */

int      maxChanges;
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )
CHANGE_DATA * changes_table;

void load_changes( )
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( CHANGE_FILE, "r")) )
    {
        bug( "Could not open Changes File for reading.", 0 );
        return;
    }

    fscanf( fp, "%d\n", &maxChanges );

    /* Use malloc so we can realloc later on */
    changes_table = malloc( sizeof( CHANGE_DATA) * (maxChanges+1) );
         for( i = 0; i < maxChanges; i++ )
    {
        changes_table[i].change = fread_string( fp );
        changes_table[i].coder = fread_string( fp );
        changes_table[i].date = fread_string( fp );
        changes_table[i].mudtime = fread_number( fp );
    }
    changes_table[maxChanges].coder = str_dup("");    
    fclose(fp);
    return; /* just return */
}

size_t my_strftime( char *s, size_t max, const char *fmt, const struct tm *tm)
{
	return strftime(s, max, fmt, tm);
}

char * current_date( )
{
    static char buf [ 128 ];
    struct tm * datetime;

    datetime = localtime( &current_time );
    my_strftime( buf, sizeof( buf ), "%x", datetime );
    return buf;
}

void save_changes(void)
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( CHANGE_FILE,"w")) )
    {
        perror( CHANGE_FILE );
        return;
    }

    fprintf( fp, "%d\n", maxChanges );
    for( i = 0; i < maxChanges; i++ )
    {
        fprintf (fp, "%s~\n", changes_table[i].change);
  	fprintf (fp, "%s~\n", changes_table[i].coder);
        fprintf (fp, "%s~\n", changes_table[i].date);
        fprintf (fp, "%d\n",  changes_table[i].mudtime );
        fprintf( fp, "\n" );
    }
 
    fclose(fp);
    return;
}

void delete_change(int iChange)
{
    int i,j;
    CHANGE_DATA * new_table;

    new_table = malloc( sizeof( CHANGE_DATA ) * maxChanges );

    if( !new_table )
    {
       return;
    }
    
    for ( i= 0, j = 0; i < maxChanges+1; i++)
    {
        if( i != (iChange -1) )
        {
            new_table[j] = changes_table[i];
            j++;
        }
    }
    
    free( changes_table );
    changes_table = new_table;

    maxChanges--;
    
    return;
}
 

void do_addchange(CHAR_DATA *ch, char *argument )
{
  char buf1[MAX_STRING_LENGTH];
    CHANGE_DATA * new_table;
    
    if ( IS_NPC( ch ) )
        return;
    
    if ( argument[0] == '\0' )
    {
        send_to_char( "&YSyntax&O: &WAddchange &z[&wChangeString&z]\n\r", ch );
        send_to_char( "&cType &z'&Wchanges&z'&c to view the list.\n\r", ch );
        return;
    }

    maxChanges++;
    new_table = realloc( changes_table, sizeof( CHANGE_DATA ) *(maxChanges+1) );

    if (!new_table) /* realloc failed */
    {
        send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
        return;
    }

    changes_table = new_table;
    
    changes_table[maxChanges-1].change  = str_dup( argument );
    changes_table[maxChanges-1].coder   = str_dup( ch->name );
    changes_table[maxChanges-1].date    = str_dup(current_date());
    changes_table[maxChanges-1].mudtime = current_time;
    
    send_to_char("\n\r&GChanges Added.\n\r",ch);
    send_to_char("&cType &z'&wchanges&Y'&c to see the changes.\n\r",ch);
    do_echo(ch, "&z[&RINFO&z] &cNew Change has been added to the mud, type &z'&wchanges&z'&c to see it.&w");
    save_changes();
sprintf(buf1, "<li> %s<br><br>", argument);
// prepend_to_file(CHANGEHTML_FILE, buf1);
    return;
}

void do_chedit( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MSL];
    char buf1[MAX_STRING_LENGTH];
 
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( IS_NPC(ch) )
        return;
    
    if (!ch->desc || NULLSTR(arg1) )
    {
        send_to_char("Syntax: chedit load/save\n\r",ch);
        send_to_char("Syntax: chedit delete (change number)\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"load") )
    {
        load_changes( );
        send_to_char("Changes Loaded.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"save") )
    {
        save_changes( );
        sprintf(buf1, "<li> %s<br><br>", argument);
        // prepend_to_file(CHANGEHTML_FILE, buf1);
        send_to_char("&GChanges Saved.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1, "delete"))
    {
        int num;
        
        if ( NULLSTR(arg2) || !is_number( arg2 ) )
        {
            send_to_char("&wFor chedit delete, you must provide a change number.{x\n\r",ch);
            send_to_char("Syntax: chedit delete (change number)\n\r",ch);
            return;
        }

        num = atoi( arg2 );
        if ( num < 1 || num > maxChanges )
        {
            sprintf( buf, "Valid changes are from 1 to %d.\n\r", maxChanges );
            send_to_char( buf, ch );
            return;
        }
        delete_change( num );
        send_to_char("&RChange deleted.\n\r",ch);
        save_changes( );
        return;
    }

    return;
}

/*
 * The following format code has been adapted from KaViR's justify
 * snippet -- Dreimas
 */

static void AddSpaces( char **ppszText, int iNumber )
{
   int iLoop;

   for ( iLoop = 0; iLoop < iNumber; iLoop++ )
   {
      *(*ppszText)++ = ' ';
   }
}
 
char *change_justify( char *pszText, int iAlignment )
{
   static char s_szResult[4096];
   char *      pszResult = &s_szResult[0];
   char        szStore[4096];
   int         iMax;
   int         iLength = iAlignment-1;
   int         iLoop = 0;

   if ( strlen( pszText ) < 10 )
   {
      strcpy( s_szResult, "BUG: Justified string cannot be less than 10 characters long." );
      return( &s_szResult[0] );
   }

   while ( *pszText == ' ' ) pszText++;

   szStore[iLoop++] = *pszText++;

   if ( szStore[iLoop-1] >= 'a' && szStore[iLoop-1] <= 'z' )
      szStore[iLoop-1] = UPPER( szStore[iLoop] );

   /* The first loop goes through the string, copying it into szStore. The
    * string is formatted to remove all newlines, capitalise new sentences,
    * remove excess white spaces and ensure that full stops, commas and
    * exclaimation marks are all followed by two white spaces.
    */
   while ( *pszText )
   {
      switch ( *pszText )
      {
         default:
            szStore[iLoop++] = *pszText++;
            break;
         case ' ':
            if ( *(pszText+1) != ' ' )
            {
               /* Store the character */
               szStore[iLoop++] = *pszText;
            }
            pszText++;
            break;
         case '.': case '?': case '!':
            szStore[iLoop++] = *pszText++;
            switch ( *pszText )
            {
               default:
                  szStore[iLoop++] = ' ';
                  szStore[iLoop++] = ' ';
                  /* Discard all leading spaces */
                  while ( *pszText == ' ' ) pszText++;
                  /* Store the character */
                  szStore[iLoop++] = *pszText++;
                  if ( szStore[iLoop-1] >= 'a' && szStore[iLoop-1] <= 'z' )
                     szStore[iLoop-1] &= ~32;
                  break;
               case '.': case '?': case '!':
                  break;
            }
            break;
         case ',':
            /* Store the character */
            szStore[iLoop++] = *pszText++;
            /* Discard all leading spaces */
            while ( *pszText == ' ' ) pszText++;
            /* Commas shall be followed by one space */
            szStore[iLoop++] = ' ';
            break;
         case '$':
            szStore[iLoop++] = *pszText++;
            while ( *pszText == ' ' ) pszText++;
            break;
         case '\n': case '\r':
            pszText++;
            break;
      }
   }

   szStore[iLoop] = '\0';

   /* Initialise iMax to the size of szStore */
   iMax = strlen( szStore );

   /* The second loop goes through the string, inserting newlines at every
    * appropriate point.
    */
   while ( iLength < iMax )
   {
      /* Go backwards through the current line searching for a space */
      while ( szStore[iLength] != ' ' && iLength > 1 )
         iLength--;

      if ( szStore[iLength] == ' ' )
      {
         szStore[iLength] = '\n';
	 	 
         iLength += iAlignment;
      }
      else
         break;
   }


   /* Reset the counter */
   iLoop = 0;

   /* The third and final loop goes through the string, making sure that there
    * is a \r (return to beginning of line) following every newline, with no
    * white spaces at the beginning of a particular line of text.
    */
   while ( iLoop < iMax )
   {
      /* Store the character */
      *pszResult++ = szStore[iLoop];
      switch ( szStore[iLoop] )
      {
         default:
            break;
         case '\n':
            *pszResult++ = '\r';
            while ( szStore[iLoop+1] == ' ' ) iLoop++;
            /* Add spaces to the front of the line as appropriate */
            AddSpaces( &pszResult, 25 );
            break;
      }
      iLoop++;
   }

   *pszResult++ = '\0';

   return( &s_szResult[0] );
}       

int num_changes( void )
{
	char *test;
    int   today;
    int   i;
    
    i = 0;
    test = current_date();
    today = 0;
    
     for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;
            
     return today;
}
    

void do_changes(CHAR_DATA *ch, char *argument) 
{
    FILE *www = NULL;
    char  arg[MAX_INPUT_LENGTH];
    char  buf[MSL], buf2[MSL];
    char *test;
    int   today;
    int   i;
    bool  fAll;
    
    one_argument( argument, arg );

    if (ch && IS_NPC(ch))
        return;

    if( maxChanges < 1 )
	return;

    i = 0;
    test = current_date();
    today = 0;
    if ( !ch )
    {
       unlink( WEBCHANGES_FILE );
       www = fopen( WEBCHANGES_FILE, "w" );
       if ( !www )
       {
          bug( "do_changes: cannot open changes web file!" );
          return;
       }
       sprintf( buf2, "&w-&W<&z( &YDragonball Z: Chronicles Changelog &z)&W>&w-" );
       fprintf( www, "%s", htmlcolor (centertext( buf2, 75 )));
       fprintf( www, htmlcolor("\n&z---------------------------------------------------------------------------") );
       fprintf( www, htmlcolor("\n&RNo.    Date          Change\n") );
       fprintf( www, htmlcolor("&z---------------------------------------------------------------------------\n") );
    }

    for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;

    if( NULLSTR( arg ) )
        fAll = FALSE;
    else fAll = !str_cmp( arg, "all" );
    if ( ch )
    {
       sprintf( buf2, "&w-&W<&z( &YDragonball Z: Chronicles Changelog &z)&W>&w-" );
       ch_printf( ch, "\n\r\n\r%s", centertext( buf2, 75 ));
       send_to_char("\n\r&z---------------------------------------------------------------------------", ch );
       send_to_char("\n\r&RNo.    Coder          Date          Change\n\r",ch );
       send_to_char("&z---------------------------------------------------------------------------\n\r", ch );
    }
    for (i = 0; i < maxChanges; i++)
    {
        
        if( !fAll
            && i < (maxChanges-18) && ch )
            continue;
        if ( !ch )        
        {
           sprintf( buf,"&z[&w%2d&z]&W   &g*%-6s     &C%-55s\n\r",
                     (i+1),
                     changes_table[i].date,
                     changes_table[i].change );
                     //change_justify( changes_table[i].change, 55 ));
           fprintf( www, htmlcolor( buf ) );
        }    

        if ( ch )
        {
           sprintf( buf,"&z[&w%2d&z]&W   %-9s   &g*%-6s     &C%-55s&d\n\r",
                     (i+1),
		     changes_table[i].coder,
                     changes_table[i].date,
                     changes_table[i].change );
                     //change_justify( changes_table[i].change, 55 ));

          send_to_char( buf, ch );
        }

    }
    sprintf(buf, "&cThere is a total of &w%3d &cchanges in the database.\n\r", maxChanges);
    if ( !ch )
    {
      fprintf( www, htmlcolor("&z---------------------------------------------------------------------------\n"));
      fprintf( www, htmlcolor(buf) );
      fprintf( www, htmlcolor("&cAlso see: &z'&wchanges all&z'&c for a list of all the changes.&z\n"));
      fprintf( www, htmlcolor("&z---------------------------------------------------------------------------\n" ));
      fclose( www );
    }
    if ( ch )
    {
     send_to_char("&z---------------------------------------------------------------------------\n\r", ch );
     send_to_char( buf, ch );
     send_to_char("&cAlso see: &z'&wchanges all&z'&c for a list of all the changes.&z\n\r",ch);
     send_to_char("&z---------------------------------------------------------------------------\n\r", ch );
    }
    return;
}


