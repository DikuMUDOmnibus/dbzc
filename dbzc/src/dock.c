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


DOCK_DATA   *first_dock;
DOCK_DATA   *last_dock;


#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
                                if ( !str_cmp( word, literal ) )        \
                                {                                       \
                                      field = value;                    \
                                      fMatch = TRUE;                    \
                                      break;                            \
                                }


DOCK_DATA *get_dock( char *name )
{
    DOCK_DATA *dock;
    
    if ( name[0] == '\0' )
       return NULL;
    
    for ( dock = first_dock; dock; dock = dock->next )
       if ( !str_cmp( name, dock->coordname ) )
         return dock;
    
    return NULL;
}

bool get_dockcoord( short x, short y, short z, short d )
{
    DOCK_DATA *dock;
    for ( dock = first_dock; dock; dock = dock->next )
    {
        if ( !dock->coordname || dock->coordname[0] == '\0')
        continue;
        if ( dock->x == x && dock->y == y && dock->z == z && dock->d == d )
        return TRUE;
    }
  return FALSE;
}


char *get_dockname( short x, short y, short z, short d )
{
    DOCK_DATA *dock;
    for ( dock = first_dock; dock; dock = dock->next )
    {
        if ( !dock->coordname || dock->coordname[0] == '\0')
        continue;
        if ( dock->x == x && dock->y == y && dock->z == z && dock->d == d )
	return dock->coordname;
    }
  return "unknown";
}

void  save_docks( void )
{
    DOCK_DATA *dock;
    FILE *fp;

    if ( (fp=fopen( DOCK_FILE, "w" )) == NULL )
    {
	bug( "Cannot open docks.dat for writting", 0 );
	perror( DOCK_FILE );
	return;
    }
    for ( dock = first_dock; dock; dock = dock->next )
    {
        if ( !dock->name || dock->name[0] == '\0')
        continue;
 	fprintf( fp, "#DOCK\n" );
	fprintf( fp, "Name         %s~\n",	dock->name	      );
	fprintf( fp, "Coordname    %s~\n",	dock->coordname       );
	fprintf( fp, "X            %d\n",	dock->x               );
	fprintf( fp, "Y            %d\n",	dock->y               );
	fprintf( fp, "Z            %d\n",	dock->z               );
        fprintf( fp, "D            %d\n",       dock->d               );
	fprintf( fp, "Room         %d\n",	dock->room            );
	fprintf( fp, "Onlyshuttle  %d\n",	dock->onlyshuttle     );
	fprintf( fp, "Type	   %d\n", 	dock->type	      );
	fprintf( fp, "End\n\n"					      );
    }
    fprintf( fp, "#END\n" );
    fclose( fp );
}

void fread_dock( DOCK_DATA *dock, FILE * fp )
{
   const char *word;
   bool fMatch;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;
         case 'N':
            KEY( "Name", dock->name, fread_string( fp ) );
            break;
         case 'C':
            KEY( "Coordname", dock->coordname, fread_string( fp ) );
            break;
         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               return;
            }
            break;
         case 'O':
            KEY( "Onlyshuttle", dock->onlyshuttle,  	fread_number( fp ) );
            break;
         case 'R':
            KEY( "Room", dock->room,		 	fread_number( fp ) );
            break;
         case 'T':
            KEY( "Type", dock->type,			fread_number( fp ) );
            break;
         case 'X':
            KEY( "X", dock->x,				fread_number( fp ) );
            break;
         case 'Y':
            KEY( "Y", dock->y,				fread_number( fp ) );
            break;
         case 'Z':
            KEY( "Z", dock->z,				fread_number( fp ) );
            break;
         case 'D':
            KEY( "D", dock->d,				fread_number( fp ) );
            break;
      }

      if( !fMatch )
         bug( "Fread_dock: no match: %s", word );
   }
}


void load_docks( void )
{
   char filename[256];
   DOCK_DATA *dock;
   FILE *fp;

   first_dock = NULL;
   last_dock = NULL;

   snprintf( filename, 256, "%s", DOCK_FILE );

   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {
     for( ;; )
      {
         char letter;
         char *word;

         letter = fread_letter( fp );
         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }

         if( letter != '#' )
         {
            bug( "%s", "Load_dock: # not found." );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "DOCK" ) )
         {
            CREATE( dock, DOCK_DATA, 1 );
            fread_dock( dock, fp );
            LINK( dock, first_dock, last_dock, next, prev );
            continue;
         }
         else if( !str_cmp( word, "END" ) )
            break;
         else
         {
            bug( "Load_dock: bad section: %s.", word );
            continue;
         }
      }
      fclose( fp );
      fp = NULL;
   }
   return;
}

void do_dockstat( CHAR_DATA * ch, char *argument )
{
   DOCK_DATA *dock;

   if( !first_dock )
   {
      send_to_char( "No docks defined.\n\r", ch );
      return;
   }
   if ( argument[0] == '\0' )
   {
      send_to_char( "Dockstat which dock?.\n\r", ch );
      return;
   }

   if ( (dock = get_dock( argument )) == NULL )
   {
      send_to_char( "This dock doesn't exist.\n\r", ch);
      return;
   }
   send_to_char( "\n\r&z+---------------------------------------------------+", ch);
   send_to_char( "\n\r&z|                  &RDock Stats &z                      |", ch);
   send_to_char( "\n\r&z+---------------------------------------------------+\n\r", ch);
   ch_printf( ch,"\n\r&YName:    &W%s", dock->name );
   ch_printf( ch,"\n\r&YCName:   &W%s", dock->coordname );
   ch_printf( ch,"\n\r&YRoom:    &W%d", dock->room );
   ch_printf( ch,"\n\r&YCoords:  &W%d&z.&W%d&z.&W%d&z.&W%d", dock->x, dock->y, dock->z, dock->d );
   ch_printf( ch,"\n\r&YPrivate: &W%s\n\r", dock->onlyshuttle == TRUE ? "Yes" : "No" );
   send_to_char( "\n\r&z+---------------------------------------------------+", ch);

   return;

}

void do_setdock( CHAR_DATA * ch, char *argument )
{
   DOCK_DATA *dock = NULL;
   char arg1[MIL], arg2[MIL];

   if( IS_NPC( ch ) )
   {
      send_to_char( "Sorry, NPCs can't edit docks.\n\r", ch );
      return;
   }
   if( !ch->desc )
   {
      send_to_char( "You have no descriptor.\n\r", ch );
      return;
   }
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || !str_cmp( arg1, "help" ) )
   {
      send_to_char( "\n\rUsage: setdock <coordname> <field> <value>\n\r", ch );
      send_to_char( "       setdock <coordname> create\n\r", ch );
      send_to_char( "       setdock <coordname> delete\n\r", ch );
      send_to_char( "       setdock <coordname> private\n\r", ch );
      send_to_char( "       setdock save\n\r", ch );
      send_to_char( "Field being one of:\n\r", ch );
      send_to_char( "   name, x, y, z, d, coordname, room, type\n\r", ch );
      return;
   }
   if ( !str_cmp( arg1, "save" ) )
   {
      save_docks();
      send_to_char( "Docks saved.\n\r", ch );
      return;
   }
   dock = get_dock( arg1 );
   if ( !str_cmp( arg2, "create" ) )
   {
     if ( dock )
     {
        send_to_char("This dock already exists.\n\r", ch );
        return;
     }
      CREATE( dock, DOCK_DATA, 1 );
      LINK( dock, first_dock, last_dock, next, prev );
      dock->coordname = STRALLOC( arg1 );
      dock->name = STRALLOC( arg1 );
      dock->x = dock->y = dock->z = dock->d = dock->room = -1;
      dock->onlyshuttle = FALSE;
      send_to_char( "Dock added.\n\r", ch );
      return;
   }
   if ( !dock )
   {
       send_to_char("This dock doesnt exist.\n\r", ch );
       return;
   }
   if ( !str_cmp( arg2, "delete" ) )
   {
     STRFREE( dock->name );
     STRFREE( dock->coordname  );
     UNLINK( dock, first_dock, last_dock, next, prev );
     DISPOSE( dock );
     send_to_char( "Dock deleted.\n\r", ch);
   }
   if ( !str_cmp( arg2, "private" ) )
   {
     if ( dock->onlyshuttle == TRUE )
     { 
       send_to_char( "This dock is now available for all ships.\n\r", ch);
       dock->onlyshuttle = FALSE;
     }
     else
     {
       send_to_char( "This dock is now only available for shuttles only.\n\r", ch);
       dock->onlyshuttle = TRUE;
     }
     return;
   }
   if ( argument[0] == '\0' )
   {
     do_setdock( ch, "" );
     return;
   }
   if ( !str_cmp( arg2, "name" ) )
   {
     STRFREE( dock->name );
     dock->name = STRALLOC( argument );
     ch_printf( ch, "Changing Dock's name to: %s", dock->name ); 
     return;
   }
   if ( !str_cmp( arg2, "coordname" ) )
   {
     STRFREE( dock->coordname );
     dock->coordname = STRALLOC( argument );
     ch_printf( ch, "Changing Dock's coordname to: %s", dock->coordname );
     return;
   }
   if ( !is_number(argument))
   {
     send_to_char("Value must be a number.\n\r", ch);
     return;
   }
   if ( !str_cmp( arg2, "x" ) )
   {
     dock->x = atoi(argument);
     ch_printf( ch, "Setting Dock's X Coordinate to: %d\n\r", dock->x );
     return;
   }
   if ( !str_cmp( arg2, "y" ) )
   {
     dock->y = atoi(argument);
     ch_printf( ch, "Setting Dock's Y Coordinate to: %d\n\r", dock->y );
     return;
   }
   if ( !str_cmp( arg2, "z" ) )
   {
     dock->z = atoi(argument);
     ch_printf( ch, "Setting Dock's Z Coordinate to: %d\n\r", dock->z );
     return;
   }
   if ( !str_cmp( arg2, "d" ) )
   {
     dock->d = atoi(argument);
     ch_printf( ch, "Setting Dock's D Coordinate to: %d\n\r", dock->d );
     return;
   }
   if ( !str_cmp( arg2, "room" ) )
   {
     dock->room = atoi(argument);
     ch_printf( ch, "Setting Dock's Room Coordinate to: %d\n\r", dock->room );
     return;
   }
   if ( !str_cmp( arg2, "type" ) )
   {
     dock->type = atoi( argument);
     ch_printf( ch, "Setting Dock's Type to: %d\n\r", dock->type );
     ch_printf( ch, "1 - Spaceport\n\r2 - Planet\n\r3 - HQ\n\r4 - Other\n\r", ch);
     return;
   }
   do_setdock( ch, "" );
   return;

}


void do_coordinates( CHAR_DATA *ch, char *argument )
{
   sh_int x;
   bool first;
   DOCK_DATA *dock;

   if( !first_dock )
   {
      send_to_char( "No docks defined.\n\r", ch );
      return;
   }
   send_to_pager( "\n\r&z+----------------------------+------------------+----------------------+\n\r", ch );
   send_to_pager( "&z|          &RLocation&z          |&G   X   Y   Z   D&z  |&R     Coord Name&z       |\n\r", ch );
   send_to_pager( "&z+----------------------------+------------------+----------------------+\n\r", ch );

   for ( x = 0; x <= DOCK_OTHER; x++ )
   {
     first = FALSE;
     for( dock = first_dock; dock; dock = dock->next )
     {
       if ( dock->onlyshuttle == TRUE )
       continue;

       if ( x == DOCK_SPACEPORT && first == FALSE )
       ch_printf( ch, "&z|&YSpaceport Docks&O:            &z|                  |                      |\n\r",ch);
       if ( x == DOCK_PLANET && first == FALSE )
       ch_printf( ch, "&z|&YPlanet Docks&O:               &z|                  |                      |\n\r", ch);
       if ( x == DOCK_HQ && first == FALSE )
       ch_printf( ch, "&z|&YHead Quarter Docks&O:         &z|                  |                      |\n\r", ch);
       if ( x == DOCK_OTHER && first == FALSE )
       ch_printf( ch, "&z|&YOther Docks&O:                &z|                  |                      |\n\r", ch);

       first = TRUE;
       if ( x == dock->type )
       ch_printf( ch, "&z|&W %-25s  &z| &W%3d %3d %3d %3d  &z| &W%-20s &z|\n\r",   dock->name, dock->x, dock->y, dock->z, dock->d, dock->coordname );
       if ( dock == last_dock )
       break;
    }
   }
   send_to_pager( "&z+----------------------------+------------------+----------------------+\n\r", ch );
   return;
}


void do_docks( CHAR_DATA *ch, char *argument )
{
   sh_int x;
   bool first;
   DOCK_DATA *dock;

   if( !first_dock )
   {
      send_to_char( "No docks defined.\n\r", ch );
      return;
   }
   send_to_pager( "\n\r&z+----------------------------+------------------+----------------------+------------+\n\r", ch );
   send_to_pager( "&z|          &RLocation&z          |&G   X   Y   Z   D&z  |&R     Coord Name&z       |    &WRoom&z    |\n\r", ch );
   send_to_pager( "&z+----------------------------+------------------+----------------------+------------+\n\r", ch );
   for ( x = 0; x <= DOCK_OTHER; x++ )
   {
     first = FALSE;
     for( dock = first_dock; dock; dock = dock->next )
     {
       if ( x == DOCK_SPACEPORT && first == FALSE )
       ch_printf( ch, "&z|&YSpaceport Docks&O:            &z|                  |                      |            |\n\r", ch);
       if ( x == DOCK_PLANET && first == FALSE )
       ch_printf( ch, "&z|&YPlanet Docks&O:               &z|                  |                      |            |\n\r", ch);
       if ( x == DOCK_HQ && first == FALSE )
       ch_printf( ch, "&z|&YHead Quarter Docks&O:         &z|                  |                      |            |\n\r", ch);
       if ( x == DOCK_OTHER && first == FALSE )
       ch_printf( ch, "&z|&YOther Docks&O:                &z|                  |                      |            |\n\r", ch);

       first = TRUE;
       if ( x == dock->type )
       ch_printf( ch, "&z|&W %-25s  &z| &W%3d %3d %3d %3d  &z| &W%-20s &z|  &W%6d&z    |\n\r",   dock->name, dock->x, dock->y, dock->z, dock->d, dock->coordname, dock->room );
       if ( dock == last_dock )
       break;
     }
   }
   send_to_pager( "&z+----------------------------+------------------+----------------------+------------+\n\r", ch );
   return;
}

