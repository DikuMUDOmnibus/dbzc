/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud (c)1997-2002 Alsherok. Contributors: Samson, Dwip, Whir,          *
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
 *                             Copyover module                              *
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

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include "mud.h"


void    send_to_desc_color args( ( const char *txt, DESCRIPTOR_DATA *d ) );


#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

bool write_to_descriptor( int desc, char *txt, int length );
void update_skill( CHAR_DATA *ch );
extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];

OBJ_DATA *get_obj_ship args( (int vnum) );



/*
 * Save the world's objects and mobs in their current positions -- Scion
 */
void save_timers()
{
   char filename[MSL];
   FILE *fpout;
    unlink( TIMER_FILE );
    sprintf( filename, "%s", TIMER_FILE );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
      bug( "FATAL: Cannot open Timer.txt!\n\r", 0 );
      return;
    }
    fprintf( fpout, "%d %d %d %d %d\n\r", dbtimer, dbtimer2, dbtimer3, income, moonphase );
    fclose( fpout );
    return;
}

void load_timers()
{
  char filename[MSL];
  FILE *fp;


    sprintf( filename, "%s", TIMER_FILE );
    if ( ( fp = fopen( filename, "r" ) ) == NULL )
    {
      bug( "FATAL: Cannot open Timer.txt!\n\r", 0 );
      return;
    }
    dbtimer = fread_number( fp );
    dbtimer2 = fread_number( fp );
    dbtimer3 = fread_number( fp );
    income = fread_number( fp );
    moonphase = fread_number( fp );
    fclose( fp );
    return;
}
/*
void save_mobile( FILE *fp, CHAR_DATA *mob )
{

   if ( !IS_NPC( mob ) || !fp )
	return;
   fprintf( fp, "%s", "#MOBILE\n" );
   fprintf( fp, "Vnum	%d\n", mob->pIndexData->vnum );
   fprintf( fp, "Level   %d\n", mob->level );
   fprintf( fp, "Gold	%lld\n", mob->gold );
   if ( mob->in_room ) 
   {
      if( xIS_SET( mob->act, ACT_SENTINEL ) )
      {
	   fprintf( fp, "Room	%d\n", mob->home_vnum );
	} 
      else
	   fprintf( fp, "Room	%d\n", mob->in_room->vnum );
   }
   else
	fprintf( fp, "Room	%d\n", ROOM_VNUM_LIMBO );

   if ( QUICKMATCH( mob->name, mob->pIndexData->player_name) == 0 )
        fprintf( fp, "Name     %s~\n", mob->name );
   if ( QUICKMATCH( mob->short_descr, mob->pIndexData->short_descr) == 0 )
  	fprintf( fp, "Short	%s~\n", mob->short_descr );
   if ( QUICKMATCH( mob->long_descr, mob->pIndexData->long_descr) == 0 )
  	fprintf( fp, "Long	%s~\n", mob->long_descr );
   if ( QUICKMATCH( mob->description, mob->pIndexData->description) == 0 )
  	fprintf( fp, "Description %s~\n", mob->description );
   fprintf( fp, "HpManaMove   %d %d %ld %ld %d %d\n",
	mob->hit, mob->max_hit, mob->mana, mob->max_mana, mob->move, mob->max_move );
   fprintf( fp, "Position %d\n", mob->position );
   fprintf( fp, "Flags %s\n",   print_bitvector(&mob->act) );
   fprintf( fp, "Coordinates  %d %d %d\n", mob->x, mob->y, mob->map );

   de_equip_char( mob );

   if ( mob->first_carrying )
	fwrite_obj( mob, mob->last_carrying, fp, 0, OS_CARRY, TRUE );

   re_equip_char( mob );

   fprintf( fp, "%s", "EndMobile\n\n" );
   return;
}
*/
void save_mobile( FILE *fp, CHAR_DATA *mob )
{

   if ( !IS_NPC( mob ) || !fp )
	return;
   fprintf( fp, "%s", "#MOBILE\n" );
   fprintf( fp, "Vnum	%d\n", mob->pIndexData->vnum );
   fprintf( fp, "Level   %d\n", mob->level );
   fprintf( fp, "Gold	%lld\n", mob->gold );
   if ( mob->in_room ) 
   {
      if( IS_ACT_FLAG( mob, ACT_SENTINEL ) )
      {
		  /* Sentinel mobs get stamped with a "home room" when they are created
		  by create_mobile(), so we need to save them in their home room regardless
		  of where they are right now, so they will go to their home room when they
		  enter the game from a reboot or copyover -- Scion */
	   fprintf( fp, "Room	%d\n", mob->home_vnum );
	} 
      else
	   fprintf( fp, "Room	%d\n", mob->in_room->vnum );
   }
   else
	fprintf( fp, "Room	%d\n", ROOM_VNUM_LIMBO );
   fprintf( fp, "Coordinates  %d %d %d\n", mob->x, mob->y, mob->map );
   if( mob->name && mob->pIndexData->player_name && str_cmp( mob->name, mob->pIndexData->player_name ) )
      fprintf( fp, "Name     %s~\n", mob->name );
   if( mob->short_descr && mob->pIndexData->short_descr && str_cmp( mob->short_descr, mob->pIndexData->short_descr ) )
      fprintf( fp, "Short    %s~\n", mob->short_descr );
   if( mob->long_descr && mob->pIndexData->long_descr && str_cmp( mob->long_descr, mob->pIndexData->long_descr ) )
      fprintf( fp, "Long    %s~\n", mob->long_descr );
   if( mob->description && mob->pIndexData->description && str_cmp( mob->description, mob->pIndexData->description ) )
      fprintf( fp, "Description %s~\n", mob->description );
   fprintf( fp, "HpManaMove   %d %d %ld %ld %d %d\n",
	mob->hit, mob->max_hit, mob->mana, mob->max_mana, mob->move, mob->max_move );
   fprintf( fp, "Position %d\n", mob->position );
   fprintf( fp, "Flags %s\n",   print_bitvector( &mob->act ) );


   de_equip_char( mob );

   if ( mob->first_carrying )
	fwrite_obj( mob, mob->last_carrying, fp, 0, OS_CARRY, TRUE );

   re_equip_char( mob );

   fprintf( fp, "%s", "EndMobile\n\n" );
   return;
}

void save_world( CHAR_DATA *ch )
{
   FILE *mobfp;
   FILE *objfp;
   int mobfile=0;
   char filename[256];
   CHAR_DATA *rch;
   ROOM_INDEX_DATA *pRoomIndex;
   int iHash;

   log_string( "Preserving world state...." );

   sprintf( filename, "%s%s", SYSTEM_DIR, MOB_FILE );
   if ( ( mobfp = fopen( filename, "w" ) ) == NULL )
   {
	bug( "%s", "save_world: fopen mob file" );
	perror( filename );
   }
   else
	mobfile++;
	
   for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
   {
	for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next 
)
	{
	   if ( pRoomIndex )
	   {
	      if( !pRoomIndex->first_content                      /* Skip room if nothing in it */
	        || IS_SET( pRoomIndex->room_flags, ROOM_CLANSTOREROOM ) /* These rooms save on their own */
	        )
	         continue;

	      sprintf( filename, "%s%d", COPYOVER_DIR, pRoomIndex->vnum );
	      if( ( objfp = fopen( filename, "w" ) ) == NULL )
	      {
		   bug( "save_world: fopen %d", pRoomIndex->vnum );
		   perror( filename );
		   continue;
	      }
            fwrite_obj( NULL, pRoomIndex->last_content, objfp, 0, OS_CARRY, TRUE );
	      fprintf( objfp, "%s", "#END\n" );
	      FCLOSE( objfp );
	   }
	}
   }
	
   if( mobfile )
   {
	for( rch = first_char; rch; rch = rch->next )
      {
           if ( !IS_NPC(rch) )
           {
		de_equip_char( rch );
 		rch->pdef = 0;
                rch->patk = 0;
                rch->edef = 0;
                rch->eatk = 0;
                rch->accuracy = 0;
                rch->critical = 0;
                rch->evasion = 0;
	
		re_equip_char( rch );
            update_skill(rch);
	   }
	   if( !IS_NPC(rch) || rch == supermob || xIS_SET( rch->act, ACT_PROTOTYPE )  )
	      continue;
	   else
		save_mobile( mobfp, rch );
	}
	fprintf( mobfp, "%s", "#END\n" );
	FCLOSE( mobfp );
   }
   return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !strcmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

CHAR_DATA *load_mobile( FILE *fp )
{
   CHAR_DATA *mob = NULL;
   const char *word;
   bool fMatch;
   int inroom = 0;
   ROOM_INDEX_DATA *pRoomIndex = NULL;

  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
  if ( !str_cmp( word, "Vnum" ) )
  {
    int vnum;
    
    vnum = fread_number( fp );
    if( get_mob_index( vnum ) == NULL )
    {
	bug( "load_mobile: No index data for vnum %d", vnum );
	return NULL;
    }
    mob = create_mobile( get_mob_index( vnum ) );
    mob->max_hit = 150;
    mob->hit = mob->max_hit;

    if ( !mob )
    {
	for ( ; ; ) 
      {
	  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
	  /* So we don't get so many bug messages when something messes up
	   * --Shaddai 
	   */
	  if ( !str_cmp( word, "EndMobile" ) )
		break;
	}
	bug( "load_mobile: Unable to create mobile for vnum %d", vnum );
	return NULL;
	}
  }
  else
  {
	for ( ; ; ) 
      {
	  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
	  /* So we don't get so many bug messages when something messes up
	   * --Shaddai 
	   */
	  if ( !str_cmp( word, "EndMobile" ) )
		break;
      }
	extract_char( mob, TRUE );
	bug( "%s", "load_mobile: Vnum not found" );
	return NULL;
  }
  for( ; ; ) 
  {
      word   = feof( fp ) ? "EndMobile" : fread_word( fp );
      fMatch = FALSE; 
      switch ( UPPER(word[0]) ) 
	{
	   case '*':
           fMatch = TRUE;
           fread_to_eol( fp );
           break;  
	   case '#':
		if ( !str_cmp( word, "#OBJECT" ) )
		{
		   mob->tempnum = -9999; /* Hackish, yes. Works though doesn't it? */
			fread_obj( mob, fp, OS_CARRY );
		}
	   case 'C':
	      if ( !str_cmp( word, "Coordinates" ) )
	      {
		  mob->x = fread_number( fp );
		  mob->y = fread_number( fp );
		  mob->map	 = fread_number( fp );

		  fMatch = TRUE;
		  break;
	      }
		break;
	   case 'D':
		KEY( "Description", mob->description, fread_string(fp));
		break;
           case 'E':
                if ( !str_cmp( word, "EndMobile" ) )
                {
                   if ( inroom == 0 )
                        inroom = ROOM_VNUM_LIMBO;
                   pRoomIndex = get_room_index( inroom );
                   if ( !pRoomIndex )
                        pRoomIndex = get_room_index( ROOM_VNUM_LIMBO );
                   char_to_room( mob, pRoomIndex );
                   return mob;
                }
                if( !str_cmp( word, "End" ) ) /* End of object, need to ignore this. sometimes they creep in theresomehow -- Scion */
                   fMatch = TRUE; /* Trick the system into thinking it matched something */
                break;

 	   case 'F':
		KEY( "Flags", mob->act, fread_bitvector( fp ) );
         case 'G':
		KEY( "Gold",	mob->gold,	fread_number( fp ) );
	 	break;
	   case 'H':
	      if ( !str_cmp( word, "HpManaMove" ) )
	      {
		   mob->hit		= fread_number( fp );
		   mob->max_hit	= fread_number( fp );
		   mob->mana	= fread_number( fp );
		   mob->max_mana	= fread_number( fp );
		   mob->move	= fread_number( fp );
		   mob->max_move	= fread_number( fp );

		   if ( mob->max_move <= 0 )
		      mob->max_move = 150;

		   fMatch = TRUE;
		   break;
	      }
		break;
	   case 'L':
		KEY( "Long", mob->long_descr, fread_string( fp ) );
	      KEY( "Level", mob->level,	fread_number( fp ) );
		break;
	   case 'N':
		KEY( "Name", mob->name, fread_string( fp ) );
		break;
	   case 'P':
		KEY( "Position", mob->position, fread_number( fp ) );
		break;
	   case 'R':
		KEY( "Room",  inroom, fread_number(fp));
		break;
	   case 'S':
		KEY( "Short", mob->short_descr, fread_string(fp));
		break;
	}
	if ( !fMatch && str_cmp( word, "End" ) )
	   bug( "load_mobile: no match: %s", word );
  }
  return NULL;
}


void read_obj_file( char *dirname, char *filename )
{
   ROOM_INDEX_DATA *room;
   FILE *fp;
   char fname[MSL];
   int vnum;

   vnum = atoi( filename );

   if ( ( room = get_room_index( vnum ) ) == NULL )
   {
	bug( "read_obj_file: ARGH! Missing room index for %d!", vnum );
	return;
   }
   
   sprintf( fname, "%s%s", dirname, filename );
   if ( ( fp = fopen( fname, "r" ) ) != NULL )
   {
	sh_int iNest;
	bool found;
	OBJ_DATA *tobj, *tobj_next;

	rset_supermob( room );
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
		bug( "%s", "read_obj_file: # not found." );
		break;
	   }

	   word = fread_word( fp );
	   if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
		fread_obj( supermob, fp, OS_CARRY );
	   else
		if ( !str_cmp( word, "END"    ) )	/* Done		*/
		  break;
	      else
		{
		    bug( "read_obj_file: bad section: %s", word );
		    break;
		}
	}
	FCLOSE( fp );
      unlink( fname );
	for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
	{
	   tobj_next = tobj->next_content;
	   if( IS_OBJ_FLAG( tobj, ITEM_ONMAP ) )
	   {
		SET_ACT_FLAG( supermob, ACT_ONMAP );
		supermob->map = tobj->map;
		supermob->x = tobj->x;
		supermob->y = tobj->y;
	   }
           obj_from_char( tobj );
	   tobj = obj_to_room( tobj, room, supermob );
	   REMOVE_ACT_FLAG( supermob, ACT_ONMAP );
	   supermob->map = -1;
	   supermob->x = -1;
	   supermob->y = -1;
	}
/*
	for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
	{
	   tobj_next = tobj->next_content;
           obj_from_char( tobj );
	   obj_to_room( tobj, room, NULL);
           
	}
*/
	release_supermob();
   }
   else
	log_string( "Cannot open obj file" );

   return;
}

void load_obj_files( void )
{
   DIR *dp;
   struct dirent *dentry;
   char directory_name[100];

   log_string( "World state: loading objs" );
   sprintf( directory_name, "%s", COPYOVER_DIR );
   dp = opendir( directory_name );
   dentry = readdir( dp );
   while ( dentry )
   {
      /* Added by Tarl 3 Dec 02 because we are now using CVS */
      if ( !str_cmp( dentry->d_name, "CVS" ) ) 
      {
         dentry = readdir( dp );
         continue;
      }
      if ( dentry->d_name[0] != '.' )
	   read_obj_file( directory_name, dentry->d_name );
      dentry = readdir( dp );
   }
   closedir( dp );
   return;
}

void load_world( CHAR_DATA *ch )
{
   FILE *mobfp;
   char file1[256];
   char *word;
   int done = 0;
   bool mobfile = FALSE;

   sprintf( file1, "%s%s", SYSTEM_DIR, MOB_FILE );
   if( ( mobfp = fopen( file1, "r" ) ) == NULL )
   {
	bug( "%s", "load_world: fopen mob file" );
	perror( file1 );
   } 
   else
	mobfile = TRUE;

   if( mobfile )
   {
	log_string( "World state: loading mobs" );
	while( done == 0 )
      {
	   if( feof( mobfp ) )
	      done++;
	   else 
	   {
		word = fread_word( mobfp );
		if( str_cmp( word, "#END" ) )
		   load_mobile( mobfp );
		else
		   done++;
	   }	
	}
	FCLOSE( mobfp );
   }
	
   load_obj_files();

   /* Once loaded, the data needs to be purged in the event it causes a crash so that it 
won't try to reload */
   unlink( file1 );
   return;
}

/* Used by copyover code */
void save_areas( void )
{
   AREA_DATA   *tarea;
   char         filename[256];

   for( tarea = first_build; tarea; tarea = tarea->next )
   {
      if( !IS_SET( tarea->status, AREA_LOADED ) )
         continue;
      sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
      fold_area( tarea, filename, FALSE, FALSE );
   }
   return;
}

/*  Warm reboot stuff, gotta make sure to thank Erwin for this :) */
void do_copyover( CHAR_DATA *ch, char *argument )
{
   FILE *fp;
   CHAR_DATA *victim = NULL;
   DESCRIPTOR_DATA *d, *de_next;
   char buf [100], buf2[100], buf3[100], buf4[100], buf5[100], buf6[100];
   extern int control;
   int count = 0;
   bool found = FALSE;

 if(str_cmp(argument, "now") && str_cmp(argument, "warn") && str_cmp(argument, "poscrash") && str_cmp(argument, "shutdown") && str_cmp(argument, "reboot"))
 {
  send_to_char("&YSyntax&O: &CCopyover &z(&wWarn&z|&wNow&z|&wPosCrash&z|&wShutdown&z|Reboot&z)\n\r", ch);
  return;
 }

  if(!str_cmp(argument, "warn"))
  {
      do_echo(ch, "\n\r{c {x &wCopyover Warning {c {x");
      return;
  }
  if(!str_cmp(argument, "shutdown"))
  {
      do_echo(ch, "\n\r{g {x &wShutdown Warning {g {x");
      return;
  }
  if(!str_cmp(argument, "reboot"))
  {
      do_echo(ch, "\n\r{g {x &wShutdown Warning {g {x");
      return;
  }
  if(!str_cmp(argument, "poscrash"))
    {
      do_echo(ch, "\n\r{r {x  &zPossible Crash  {r {x");
      return;
    }

   for( d = first_descriptor; d; d = d->next )
   {
      if ( ( d->connected == CON_PLAYING || d->connected == CON_EDITING )
        && ( victim = d->character ) != NULL && !IS_NPC( victim ) && victim->in_room
        &&   victim->fighting && victim->level >= 1 && victim->level <= MAX_LEVEL )
      {
         found = TRUE;
	   count++;
      }
   }

/*   if( found )
   {
	ch_printf( ch, "Cannot copyover at this time. There are %d combats in progress.\n\r", count );
	return;
   }*/

   found = FALSE;
   for( d = first_descriptor; d; d = d->next )
   {
	if( d->connected == CON_EDITING && d->character )
	{
	   found = TRUE;
	   break;
	}
   }

   if( found )
   {
	send_to_char( "Cannot copyover at this time. Someone is using the line editor.\n\r", ch );
	return;
   }
/*
   for( d = first_descriptor; d; d = d->next )
   {
        if( d->character && xIS_SET(d->character->act, PLR_FUSED))
        {
           found = TRUE;
           break;
        }
   }

   if( found )
   {
        send_to_char( "Cannot copyover at this time. Someone is fused.\n\r", ch );
        return;
   }
*/
    sprintf( log_buf, "Copyover initiated by %s.", ch->name );
    log_string( log_buf );

    fp = fopen( COPYOVER_FILE, "w" );
    
    if( !fp )
    {
      send_to_char( "Copyover file not writeable, aborted.\n\r", ch );
	bug( "Could not write to copyover file: %s. Copyover aborted.", COPYOVER_FILE );
      perror( "do_copyover:fopen" );
      return;
    }
    
    /* Consider changing all loaded prototype areas here, if you use OLC */
    /*
    log_string( "Saving modified area files..." );
    save_areas();
    */

    /* And this one here will save the status of all objects and mobs in the game.
     * This really should ONLY ever be used here. The less we do stuff like this the better.
     */
    save_world( ch );


    log_string( "Saving player files and connection states...." );
   if( ch && ch->desc )
        write_to_descriptor( ch->desc->descriptor, "\e[0m", 0 );
//    sprintf( buf, "\n\r&gYou hear some squeaking sounds...\n\r" );
    /* For each playing descriptor, save its state */
    for( d = first_descriptor; d ; d = de_next )
    {
        CHAR_DATA *och = CH(d);
        de_next = d->next; /* We delete from the list , so need to save this */
        if( !d->character || d->connected < CON_PLAYING ) /* drop those logging on */
        {
            write_to_descriptor( d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0 );
            close_socket( d, FALSE ); /* throw'em out */
        }
        else
        {
         fprintf( fp, "%d %d %d %d %d %d %s %s %s\n", d->descriptor, 0,
                  och->in_room->vnum, och->was_in_room->vnum, d->port, d->idle, och->name, d->host, d->client );

            /* One of two places this gets changed */
            och->pcdata->copyover = TRUE;
            save_char_obj( och );
/*	    if( argument && str_cmp( argument, "debug" ) )
	    {
		send_to_desc_color( buf, d );
            }*/
        }
    }

    fprintf( fp, "0 0 0 0 0 %d maxp maxp\n", sysdata.maxplayers );
    fprintf( fp, "%s", "-1\n" );
    FCLOSE( fp );


   /* added this in case there's a need to debug the contents of the various files */
   if( argument && !str_cmp( argument, "debug" ) )
   {
	log_string( "Copyover debug - Aborting before execl" );
	return;
   }

    log_string( "Executing Copyover...." );
    save_timers();
    /* Close reserve and other always-open files and release other resources */
    FCLOSE( fpReserve );
    FCLOSE( fpLOG );
    
    /* exec - descriptors are inherited */
    sprintf( buf,  "%d", port );
    sprintf( buf2, "%d", control );
    sprintf( buf3, "%d", 0 );
    sprintf( buf4, "%d", 0 );
    sprintf( buf5, "%d", 0 );
    strcpy ( buf6, "-1" );

    /* Uncomment this bfd_close line if you've installed the BFD snippet, you'll need it. */
    // bfd_close( abfd );
    dlclose( sysdata.dlHandle );
    execl( EXE_FILE, "dbzc", buf, "copyover",  buf2, buf3, buf4, buf5, 
buf6, (char *)NULL );
    
    /* Failed - sucessful exec will not return */
    perror( "do_copyover: execl" );
   sysdata.dlHandle = dlopen( NULL, RTLD_LAZY );
   if( !sysdata.dlHandle )
   {
	bug( "%s", "FATAL ERROR: Unable to reopen system executable handle!" );
	exit( 1 );
   }
    /* Here you might want to reopen fpReserve */
    /* Since I'm a neophyte type guy, I'll assume this is a good idea and cut and past from 
main()  */
    
    if( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
        perror( NULL_FILE );
        exit( 1 );
    }
    if( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
    {
        perror( NULL_FILE );
        exit( 1 );
    }
    bug( "%s", "Copyover execution failed!!" );
    send_to_char( "Copyover FAILED!\n\r", ch );
}

/* Recover from a Copyover - load players */
void copyover_recover( void )
{
    DESCRIPTOR_DATA *d = NULL;
    FILE *fp;
    char name[100];
    char host[MSL], client[MSL];
    int desc, dcompress, room, lastroom, dport, idle, maxp = 0;
    bool fOld;
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    int vnum, x, y;

    vnum = 0;

    for ( ship = first_ship; ship; ship = ship->next )
    {
     vnum = ship->obj_vnum;
     obj = get_obj_ship( vnum );
     if ( obj )
     {
     separate_obj( obj );
     extract_obj( obj );
     }
     obj = create_object( get_obj_index( ship->obj_vnum ), 0 );
     obj_to_room( obj, get_room_index( ship->home ), NULL );
     ship->ship_state = 0;
     ship->current_loc = ship->home;
     get_curr_home( ship );
     ship->destination = 0;
    } 

    fp = fopen( COPYOVER_FILE, "r" );
    
    if (!fp) /* there are some descriptors open which will hang forever then ? */
    {
        perror( "copyover_recover: fopen" );
        bug( "%s", "Copyover file not found. Exitting." );
        exit( 1 );
    }
    
    unlink( COPYOVER_FILE ); /* In case something crashes - doesn't prevent reading */
    load_timers();

    for( ; ; )
    {
        d = NULL;

        fscanf( fp, "%d %d %d %d %d %d %s %s %s\n", &desc, &dcompress, &room, &lastroom, &dport, &idle, name, host, client );        

        if (desc == -1 || feof(fp) )
            break;

	  if( !str_cmp( name, "maxp" ) || !str_cmp( host, "maxp" ) )
	  {
	   maxp = idle;
	   continue;
	  }

      /* Write something, and check if it goes error-free */
//      if( !write_to_descriptor( desc, "\n\rThe ether swirls in chaos.\n\r", 0 ) )
      if( !write_to_descriptor( desc, "", 0 ) )
      {
         close( desc ); /* nope */
         continue;
      }        
        CREATE( d, DESCRIPTOR_DATA, 1 );

        d->next		= NULL;
        d->descriptor	= desc;
        d->connected	= CON_GET_NAME;
        d->outsize	= 2000;
        d->idle		= 0;
        d->lines		= 0;
        d->scrlen		= 24;
        d->newstate	= 0;
        d->prevcolor	= 0x08;

        CREATE( d->outbuf, char, d->outsize );
//        d->user = STRALLOC( "Unknown" );
        d->host = STRALLOC( host );
        d->port = dport;
        d->idle = idle;
        LINK( d, first_descriptor, last_descriptor, next, prev );
        d->connected = CON_COPYOVER_RECOVER; /* negative so close_socket will cut them off 
*/

        /* Now, find the pfile */
        fOld = load_char_obj( d, name, FALSE, TRUE );
        
        if( !fOld ) /* Player file not found?! */
        {
   	    write_to_descriptor( d->descriptor, "\n\rSomehow, your character was lost during hotboot. Contact the immortals ASAP.\n\r", 0 );
            close_socket( d, FALSE );
        }
        else /* ok! */
        {
//   	    write_to_descriptor( d->descriptor, "\n\rTime resumes its normal flow.\n\r", 0 );
         d->character->in_room = get_room_index( room );
         d->character->was_in_room = get_room_index( lastroom );
         if( !d->character->in_room && !d->character->was_in_room )
         {
            d->character->in_room = get_room_index( ROOM_VNUM_TEMPLE );
            d->character->was_in_room = get_room_index( ROOM_VNUM_TEMPLE );
         }
            
            /* Insert in the char_list */
            LINK( d->character, first_char, last_char, next, prev );
         if( d->character->in_room )
           char_to_room( d->character, d->character->in_room );
         else
           char_to_room( d->character, d->character->was_in_room );
//	      act( AT_GREY, "You re-materialize back into your normal self.", d->character, NULL, NULL, TO_CHAR );
//            act( AT_GREY, "$n materializes!", d->character, NULL, NULL, TO_ROOM );
            xREMOVE_BIT(d->character->act, PLR_FUSED);
            xREMOVE_BIT(d->character->act, PLR_INVIS2);
            d->character->fused_with = NULL;
//            do_look(d->character, "" );
            if (xIS_SET(d->character->powerup, PLR_BOOST))
            {
                xREMOVE_BIT(d->character->powerup, PLR_BOOST);
                check_boost();
            }
                if ( !IS_IMMORTAL(d->character) && d->character->level < 50 && !IS_NPC(d->character))
                {
                         adjust_hiscore( "bless", d->character, d->character->pcdata->bless, TRUE );
                         adjust_hiscore( "pkill", d->character, d->character->pcdata->pkills, TRUE );
                         adjust_hiscore( "mkill", d->character, d->character->pcdata->mkills, TRUE );
                         adjust_hiscore( "gold", d->character, d->character->gold, TRUE );
                         adjust_hiscore( "bank", d->character, d->character->balance, TRUE );
                         adjust_hiscore( "reputation", d->character, calculate_rep( d->character ), TRUE);
                 }

//            do_global_boards(d->character, "");
            send_to_char( "&gYou hear some squeaking sounds...\n\r", d->character );
            xREMOVE_BIT(d->character->act, PLR_SPAR);
            if (IS_ANDROID(d->character))
            do_androidmod( d->character, "" );
            if (IS_BIODROID(d->character)) {
            xSET_BIT(d->character->powerup, PU_LARVAE );}

            if(xIS_SET(d->character->act, PLR_SKINNY) && IS_BUU(d->character))
            {
                        d->character->mod_dex = 3;
                        d->character->mod_int = 3;
                        d->character->mod_str = 3;
                        d->character->mod_con = -5;
                        d->character->mod = 8.8;
            }
            else if(xIS_SET(d->character->act, PLR_FAT) && IS_BUU(d->character))
            {
                        d->character->mod_str = 4;
                        d->character->mod_int = -5;
                        d->character->mod_con = 5;
                        d->character->mod = 8.6;
            }
            if(xIS_SET(d->character->act, PLR_ULTIMATE) && IS_BUU(d->character))
	    {
			if(IS_EVIL(d->character))
			{
				d->character->mod = 9.77;
				do_suppress(d->character, "0");
			}
			else if ( IS_GOOD(d->character) && xIS_SET(d->character->powerup, PU_STEAM))
                        {
                                d->character->mod = 9.4;
                                do_suppress(d->character, "0");
                        }

	    }

            d->connected = CON_PLAYING;
            if ( ++num_descriptors > sysdata.maxplayers )
	         sysdata.maxplayers = num_descriptors;
        }
    }
    for ( x = 0; x < MAX_WEAR; x++ )
        for ( y = 0; y < MAX_LAYERS; y++ )
                   save_equipment[x][y] = NULL;

    FCLOSE( fp );
    if( maxp > sysdata.maxplayers )
	sysdata.maxplayers = maxp;
    log_string( "Copyover recovery complete." );
    do_pfiles(supermob, "" );
    return;
}

