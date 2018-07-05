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
 *		     Character saving and loading module		    *
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
#include <time.h>
#include <sys/stat.h>
#ifndef WIN32
  #include <dirent.h>
#endif
#include "mud.h"


/*
 * Increment with every major format change.
 */
#define SAVEVERSION	3
bool can_userace( CHAR_DATA *ch, int gsn );

/*
 * Array to keep track of equipment temporarily.		-Thoric
 */
OBJ_DATA *save_equipment[MAX_WEAR][MAX_LAYERS];
CHAR_DATA *quitting_char, *loading_char, *saving_char;

int file_ver;

/*
 * Externals
 */
void fwrite_comments( CHAR_DATA *ch, FILE *fp );
void fread_comment( CHAR_DATA *ch, FILE *fp );
bool check_parse_name( char *name, bool newchar );

/*
 * Array of containers read for proper re-nesting of objects.
 */
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch, FILE *fp ) );
void	fread_char	args( ( CHAR_DATA *ch, FILE *fp, bool preload, bool copyover) );
void	write_corpses	args( ( CHAR_DATA *ch, char *name, OBJ_DATA *objrem ) );

#ifdef WIN32                    /* NJG */
UINT timer_code = 0;            /* needed to kill the timer */
/* Note: need to include: WINMM.LIB to link to timer functions */
void caught_alarm();
void CALLBACK alarm_handler(
	UINT IDEvent,		/* identifies timer event */
	UINT uReserved,		/* not used */
	DWORD dwUser,		/* application-defined instance data */
	DWORD dwReserved1,	/* not used */
	DWORD dwReserved2)	/* not used */
{
    caught_alarm();
}

void kill_timer()
{
    if (timer_code)
	timeKillEvent(timer_code);
    timer_code = 0;
}

#endif



/*
 * Un-equip character before saving to ensure proper	-Thoric
 * stats are saved in case of changes to or removal of EQ
 */
void de_equip_char( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int x,y;

    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;
    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	if ( obj->wear_loc > -1 && obj->wear_loc < MAX_WEAR )
	{
/*          Fixes obj bugs which shouldn't exist in DBZ:C
	    if ( get_trust( ch ) >= obj->level )
	    {
*/
		for ( x = 0; x < MAX_LAYERS; x++ )
		   if ( !save_equipment[obj->wear_loc][x] )
		   {
			save_equipment[obj->wear_loc][x] = obj;
			break;
		   }
		if ( x == MAX_LAYERS )
		{
		    sprintf( buf, "%s had on more than %d layers of clothing in one location (%d): %s",
			ch->name, MAX_LAYERS, obj->wear_loc, obj->name );
		    bug( buf, 0 );
		}
/*
	    }
	    else
	    {
	       sprintf( buf, "%s had on %s:  ch->level = %d  obj->level = %d",
		ch->name, obj->name,
	       	ch->level, obj->level );
	       bug( buf, 0 );
	    }
*/
	    unequip_char(ch, obj);
	}
}

/*
 * Re-equip character					-Thoric
 */
void re_equip_char( CHAR_DATA *ch )
{
    int x,y;

    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	   if ( save_equipment[x][y] != NULL )
	   {
		if ( quitting_char != ch )
		   equip_char(ch, save_equipment[x][y], x);
		save_equipment[x][y] = NULL;
	   }
	   else
		break;
}

void save_home( CHAR_DATA *ch )
{
    if(!ch->desc)
    return;
    if ( ch->plr_home )
    {
        FILE *fp;
        char filename[256];
        sh_int templvl;
        OBJ_DATA *contents;


        sprintf( filename, "%s%s.home", HOME_DIR, capitalize( ch->name ) );
        if ( ( fp = fopen( filename, "w" ) ) == NULL )
        {
        }
        else
        {
                templvl = ch->level;
                ch->level = LEVEL_HERO;             /* make sure EQ doesn't get lost */
                contents = ch->plr_home->last_content;
                if (contents)
                  fwrite_obj(ch, contents, fp, 0, OS_CARRY, TRUE );
                fprintf( fp, "#END\n" );
                ch->level = templvl;
                fclose( fp );
        }
    }
}


/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    char strback[MAX_INPUT_LENGTH];
    FILE *fp;

    if ( !ch )
    {
	bug( "Save_char_obj: null ch!", 0 );
	return;
    }

    if ( IS_NPC(ch) || ch->level < 2 )
	return;

    saving_char = ch;

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    de_equip_char( ch );

    ch->save_time = current_time;
    sprintf( strsave, "%s%c/%s",PLAYER_DIR,tolower(ch->pcdata->filename[0]),
				 capitalize( ch->pcdata->filename ) );

    /*
     * Auto-backup pfile (can cause lag with high disk access situtations).
     */
    /* Backup of each pfile on save as above can cause lag in high disk
       access situations on big muds like Realms.  Quitbackup saves most
       of that and keeps an adequate backup -- Blodkai, 10/97 */

    if ( IS_SET( sysdata.save_flags, SV_BACKUP ) || 
       ( IS_SET( sysdata.save_flags, SV_QUITBACKUP ) && quitting_char == ch ))
    {
        sprintf( strback,"%s%s",BACKUP_DIR,  capitalize( ch->pcdata->filename ) );
        rename( strsave, strback );
    }

    /*
     * Save immortal stats, level & vnums for wizlist		-Thoric
     * and do_vnums command
     *
     * Also save the player flags so we the wizlist builder can see
     * who is a guest and who is retired.
     */
/*    if ( ch->level >= LEVEL_IMMORTAL )
    {
      sprintf( strback, "%s%s", GOD_DIR, capitalize( ch->pcdata->filename ) );

      if ( ( fp = fopen( strback, "w" ) ) == NULL )
      {
	perror( strsave );
	bug( "Save_god_level: fopen", 0 );
      }
      else
      {
	fprintf( fp, "Level        %d\n", ch->level );
        if ( !xIS_EMPTY(ch->pcdata->flags) )
           fprintf( fp, "Pcflags         %s\n",	print_bitvector(&ch->pcdata->flags)		);
//	fprintf( fp, "Pcflags      %d\n", ch->pcdata->flags );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  fprintf( fp, "RoomRange    %d %d\n", ch->pcdata->r_range_lo,
	  				       ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  fprintf( fp, "ObjRange     %d %d\n", ch->pcdata->o_range_lo,
	  				       ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  fprintf( fp, "MobRange     %d %d\n", ch->pcdata->m_range_lo,
	  				       ch->pcdata->m_range_hi	);
	fclose( fp );
      }
    }*/
    if ( ch->level >= LEVEL_IMMORTAL )
    {
      sprintf( strback, "%s%s", GOD_DIR, capitalize( ch->pcdata->filename ) );

      if ( ( fp = fopen( strback, "w" ) ) == NULL )
      {
	  perror( strsave );
	  bug( "Save_god_level: fopen", 0 );
      }
      else
      {
	fprintf( fp, "Level        %d\n", ch->level );
        if ( !xIS_EMPTY(ch->pcdata->flags) )
           fprintf( fp, "Pcflags         %s\n", print_bitvector(&ch->pcdata->flags)            );
//      fprintf( fp, "Pcflags      %d\n", ch->pcdata->flags );
	if( ch->pcdata->homepage && ch->pcdata->homepage[0] != '\0' )
	   fprintf( fp, "Homepage    %s~\n", ch->pcdata->homepage );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  fprintf( fp, "RoomRange    %d %d\n", ch->pcdata->r_range_lo, ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  fprintf( fp, "ObjRange     %d %d\n", ch->pcdata->o_range_lo, ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  fprintf( fp, "MobRange     %d %d\n", ch->pcdata->m_range_lo, ch->pcdata->m_range_hi	);
      if ( ch->pcdata->email && ch->pcdata->email[0] != '\0' )
	  fprintf( fp, "Email        %s~\n", ch->pcdata->email );
      if ( ch->pcdata->aim > 0 )
	  fprintf( fp, "AIM          %s~\n", ch->pcdata->aim );
	fprintf( fp, "End\n" );
	fclose( fp );
      }
    }
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
	perror( strsave );
	bug( "Save_char_obj: fopen", 0 );
    }
    else
    {
	bool ferr;
	
	fwrite_char( ch, fp );
	if ( ch->first_carrying )
	  fwrite_obj( ch, ch->last_carrying, fp, 0, OS_CARRY, ch->pcdata->copyover  );

	if ( sysdata.save_pets && ch->pcdata->pet )
		fwrite_mobile( fp, ch->pcdata->pet );
	if ( ch->comments )                 /* comments */
	  fwrite_comments( ch, fp );        /* comments */
	fprintf( fp, "#END\n" );
	ferr = ferror(fp);
	fclose( fp );
	if (ferr)
	{
	  perror(strsave);
	  bug("Error writing temp file for %s -- not copying", strsave);
	}
	else
	  rename(TEMP_FILE, strsave);
    }

    re_equip_char( ch );

    quitting_char = NULL;
    saving_char   = NULL;
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    int sn, track, i;
    sh_int pos;

    fprintf( fp, "#PLAYER\n"		);

    fprintf( fp, "Version      %d\n",   SAVEVERSION		);
    fprintf( fp, "Name         %s~\n",	ch->name		);
   if ( ch->description[0] != '\0' )
    fprintf( fp, "Description  %s~\n",	ch->description	);
    fprintf( fp, "Sex          %d\n",	ch->sex			);
    fprintf( fp, "Class        %d\n",	ch->class		);
    fprintf( fp, "Race         %d\n",	ch->race		);
    fprintf( fp, "Level        %d\n",	ch->level		);
    fprintf( fp, "Played       %d\n",	ch->played + (int) (current_time - ch->logon)		);
    fprintf( fp, "Room         %d\n",
	(  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	&& ch->was_in_room )
	    ? ch->was_in_room->vnum
	    : ch->in_room->vnum );

   if ( ch->plr_home != NULL )
    fprintf( fp, "PlrHome      %d\n",          ch->plr_home->vnum );
   if ( ch->plr_home1 != NULL )
    fprintf( fp, "PlrHome1     %d\n",          ch->plr_home1->vnum );
   if ( ch->plr_home2 != NULL )
    fprintf( fp, "PlrHome2     %d\n",          ch->plr_home2->vnum );
   if ( ch->plr_home3 != NULL )
    fprintf( fp, "PlrHome3     %d\n",          ch->plr_home3->vnum );
   if ( ch->plr_home4 != NULL )
    fprintf( fp, "PlrHome4     %d\n",          ch->plr_home4->vnum );
    fprintf( fp, "HpManaMove   %d %d %ld %ld %d %d\n",	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Breath       %d\n", ch->breath );
    fprintf( fp, "Gold         %lld\n",	ch->gold		);
    fprintf( fp, "Aura         %d %d\n", ch->auracolor, ch->aurapowerup );
    fprintf( fp, "Balance      %lld\n", ch->balance            );
    fprintf( fp, "Exp          %ld\n",	ch->exp			);
    fprintf( fp, "Quest        %.1f\n", ch->quest		);
    fprintf( fp, "Mod          %f\n",   ch->mod              );
    fprintf( fp, "Kimod        %f\n",  ch->kimod		);
    fprintf( fp, "Height          %d %d\n",	ch->height, ch->height_mod	);
    fprintf( fp, "Weight          %d %d\n",	ch->weight, ch->weight_mod	);
   if ( !xIS_EMPTY(ch->act) )
    fprintf( fp, "Act          %s\n", print_bitvector(&ch->act) );
   if ( !xIS_EMPTY(ch->powerup) )
    fprintf( fp, "Powerup      %s\n", print_bitvector(&ch->powerup) );
   if ( !xIS_EMPTY(ch->skill) )
    fprintf( fp, "Skill      %s\n", print_bitvector(&ch->skill) );
   if ( ch->drunk  )
    fprintf( fp, "Drunk      %d\n",          ch->drunk );
    pos = ch->position;
    if ( pos == POS_FIGHTING )
    pos = POS_STANDING;
    pos +=100;
    fprintf( fp, "Position     %d\n", 	pos			);
    fprintf( fp, "Style        %d\n",   ch->style		);
    fprintf( fp, "Stat         %d\n",   ch->stat                );
    fprintf( fp, "StatGain     %d\n",   ch->statgain            );
    fprintf( fp, "SKiGain      %d\n",   ch->skigain             );
    fprintf( fp, "SkillGain    %d\n",   ch->skillgain		);
    fprintf( fp, "SkillLvl     %d\n",   ch->skill_lvl           );
    fprintf( fp, "Gravity      %d %d %d\n", ch->gravset, ch->gravsucc, ch->gravtotal );

    fprintf( fp, "Alignment    %d\n",	ch->alignment		);
    fprintf( fp, "Glory        %d\n",   ch->pcdata->bless	);
//    fprintf( fp, "RP           %d %d\n", ch->rplevel, ch->rpneed);
    fprintf( fp, "AtkDef       %d %d %d %d %d %d %d\n", ch->patk, ch->pdef, ch->eatk, ch->edef, ch->evasion, ch->critical, ch->accuracy );
    fprintf( fp, "AtkDef2      %d %d %d %d\n", ch->patk2, ch->pdef2, ch->eatk2, ch->edef2);
    fprintf( fp, "Armory       %d\n",   ch->armory		);
    fprintf( fp,  "Biomass     %d\n",   ch->biomass             );
    fprintf( fp,  "Barrier     %lld\n", ch->barrier             );
    fprintf( fp,  "Suppress    %lld\n", ch->suppress            );

    fprintf( fp,  "Rage        %d\n",   ch->rage                );  
    fprintf( fp,  "Kili	       %d\n",   ch->pcdata->kili	);
    fprintf( fp,  "NdCount     %d\n",   ch->ndcount             );
    fprintf( fp,  "Mapsize     %d\n",	ch->mapsize		);
    fprintf( fp,  "Set         %d %d\n", ch->runset, ch->flyset );
    if ( ch->wimpy )
    fprintf( fp, "Wimpy        %d\n",	ch->wimpy		);

    fprintf( fp, "Password     %s~\n",	ch->pcdata->pwd		);
    fprintf( fp, "Rank         %s~\n",	ch->pcdata->rank	);
    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
    fprintf( fp, "Bestowments  %s~\n", 	ch->pcdata->bestowments );

    fprintf( fp, "Title        %s~\n",	ch->pcdata->title	);
    if ( ch->pcdata->homepage && ch->pcdata->homepage[0] != '\0' )
    fprintf( fp, "Homepage     %s~\n",	ch->pcdata->homepage	);
    if ( ch->pcdata->email && ch->pcdata->email[0] != '\0' ) /* Samson 4-19-98 */
    fprintf( fp, "Email	     %s~\n", 	ch->pcdata->email );
    if ( ch->pcdata->aim > 0 ) /* Samson 1-4-99 */
    fprintf( fp, "AIM          %s~\n",		ch->pcdata->aim );
    if ( ch->pcdata->bio && ch->pcdata->bio[0] != '\0' )
    fprintf( fp, "Bio          %s~\n",	ch->pcdata->bio 	);
    if ( ch->pcdata->authed_by && ch->pcdata->authed_by[0] != '\0' )
    fprintf( fp, "AuthedBy     %s~\n",	ch->pcdata->authed_by	);
	if ( ch->pcdata->min_snoop )
	  fprintf( fp, "Minsnoop     %d\n",	ch->pcdata->min_snoop	);
	if ( ch->pcdata->prompt && *ch->pcdata->prompt )
	  fprintf( fp, "Prompt       %s~\n",	ch->pcdata->prompt	);
 	if ( ch->pcdata->fprompt && *ch->pcdata->fprompt )
	  fprintf( fp, "FPrompt	     %s~\n",    ch->pcdata->fprompt	);
	if ( ch->pcdata->pagerlen != 24 )
	  fprintf( fp, "Pagerlen     %d\n",	ch->pcdata->pagerlen	);

        /* Save note board status */
        /* Save number of boards in case that number changes */
        fprintf (fp, "Boards       %d ", MAX_BOARD);
        for (i = 0; i < MAX_BOARD; i++)
            fprintf (fp, "%s %d ", boards[i].short_name, ch->pcdata->last_note[i]);
        fprintf (fp, "\n");
	
	/* If ch is ignoring players then store those players */
	{
    	IGNORE_DATA *temp;
	for(temp = ch->pcdata->first_ignored; temp; temp = temp->next)
	{
		fprintf(fp,"Ignored      %s~\n", temp->name);
	}
        }
	  
	if ( IS_IMMORTAL( ch ))
	{
	  if ( ch->pcdata->bamfin && ch->pcdata->bamfin[0] != '\0' )
	    fprintf( fp, "Bamfin       %s~\n",	ch->pcdata->bamfin	);
	  if ( ch->pcdata->bamfout && ch->pcdata->bamfout[0] != '\0' )
	    fprintf( fp, "Bamfout      %s~\n",	ch->pcdata->bamfout	);
          if ( ch->trust )
            fprintf( fp, "Trust        %d\n",	ch->trust		);
          if ( ch->pcdata && ch->pcdata->restore_time )
            fprintf( fp, "Restore_time %ld\n",ch->pcdata->restore_time );
	  fprintf( fp, "WizInvis     %d\n", ch->pcdata->wizinvis );
	  if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	    fprintf( fp, "RoomRange    %d %d\n", ch->pcdata->r_range_lo,
	  					 ch->pcdata->r_range_hi	);
	  if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	    fprintf( fp, "ObjRange     %d %d\n", ch->pcdata->o_range_lo,
	  					 ch->pcdata->o_range_hi	);
	  if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	    fprintf( fp, "MobRange     %d %d\n", ch->pcdata->m_range_lo,
	  					 ch->pcdata->m_range_hi	);
	}
	if ( ch->pcdata->clan_name && ch->pcdata->clan_name[0] != '\0' )
	  fprintf( fp, "Clan         %s~\n",	ch->pcdata->clan_name	);
        if ( !xIS_EMPTY(ch->pcdata->flags) )
          fprintf( fp, "Flags        %s\n", print_bitvector(&ch->pcdata->flags)            );
//        fprintf( fp, "Flags        %d\n",	ch->pcdata->flags	);
	fprintf( fp, "Deaf	   %d\n",	ch->deaf 	);
        if ( ch->pcdata->release_date )
            fprintf( fp, "Helled       %d %s~\n",
        	(int)ch->pcdata->release_date, ch->pcdata->helled_by );
        if ( ch->pcdata->silenced_date )
            fprintf( fp, "Silenced       %d %s~\n",
                (int)ch->pcdata->silenced_date, ch->pcdata->silenced_by );
	fprintf( fp, "PKills       %d\n",	ch->pcdata->pkills	);
	fprintf( fp, "PDeaths      %d\n",	ch->pcdata->pdeaths	);
 	if ( get_timer( ch , TIMER_PKILLED)       
         && ( get_timer( ch , TIMER_PKILLED) > 0 ) )
         fprintf( fp, "PTimer       %d\n",     get_timer(ch, TIMER_PKILLED));
        if ( get_timer( ch , TIMER_GRAVITY)
         && ( get_timer( ch , TIMER_GRAVITY) > 0 ) )
         fprintf( fp, "PGTimer       %d\n",     get_timer(ch, TIMER_GRAVITY));

        fprintf( fp, "MKills       %d\n",	ch->pcdata->mkills	);
	fprintf( fp, "MDeaths      %d\n",	ch->pcdata->mdeaths	);
	fprintf( fp, "IllegalPK    %d\n",	ch->pcdata->illegal_pk	);
	fprintf( fp, "AttrPerm     %d %d %d %d %d %d %d\n",
	    ch->perm_str,
	    ch->perm_int,
	    ch->perm_wis,
	    ch->perm_dex,
	    ch->perm_con,
	    ch->perm_cha,
	    ch->perm_lck );
        fprintf( fp, "ModPerm      %d %d %d %d %d %d %d\n",
            ch->mod_str,
            ch->mod_int,
            ch->mod_wis,
            ch->mod_dex,
            ch->mod_con,
            ch->mod_cha,
            ch->mod_lck );


	if ( ch->desc && ch->desc->host )
      fprintf( fp, "Site           %s\n", ch->desc->host );
	else
      fprintf( fp, "Site           %s\n", ch->pcdata->lasthost );
        if ( ch->desc )
      fprintf( fp, "LastOn         %d\n", (int)current_time );
         else
      fprintf( fp, "LastOn         %d\n", (int)ch->pcdata->lastlogon);

        for ( sn = 0; sn < top_sskill; sn++ )
        {
            if ( sskill_table[sn]->name && ch->pcdata->learned[sn] > 0 )
                fprintf( fp, "SSkill        %d %d '%s'\n", ch->pcdata->learned[sn], ch->pcdata->skill_level[sn], sskill_table[sn]->name );
        }
        for ( sn = 0; sn < top_askill; sn++ )
        {
            if ( askill_table[sn]->name && ch->pcdata->learned2[sn] > 0 && IS_IMMORTAL(ch)  )
                 fprintf( fp, "ASkill        %d %d '%s'\n", ch->pcdata->learned2[sn], ch->pcdata->skill_level2[sn], askill_table[sn]->name );
            else if (  askill_table[sn]->name && ch->pcdata->learned2[sn] > 0 && can_userace(ch, sn) )
                fprintf( fp, "ASkill        %d %d '%s'\n", ch->pcdata->learned2[sn], ch->pcdata->skill_level2[sn], askill_table[sn]->name );
                
        }
	

    track = URANGE( 2, ((ch->level+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK );
    for ( sn = 0; sn < track; sn++ )
    {
	if ( ch->pcdata->killed[sn].vnum == 0 )
	  break;
	fprintf( fp, "Killed       %d %d\n",
		ch->pcdata->killed[sn].vnum,
		ch->pcdata->killed[sn].count );
    }
    /* Overland Map - Samson 7-31-99 */
    fprintf( fp, "Coordinates	%d %d %d\n", ch->x, ch->y, ch->map );
    /* Save color values - Samson 9-29-98 */
    { 
	int x;
      fprintf( fp, "MaxColors    %d\n",     MAX_COLORS            );
      fprintf( fp, "Colors       " );
        for ( x = 0; x < MAX_COLORS ; x++ )
            fprintf( fp, "%d ", ch->colors[x] );
      fprintf( fp, "\n" );
    }

    if (fprintf(fp, "End\n\n") < 5) { bug("Reached Max Quota"); exit(-666); }
//    fprintf( fp, "End\n\n" );
    return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, sh_int os_type, bool copyover )
{
    EXTRA_DESCR_DATA *ed;
    sh_int wear, wear_loc, x;

    if ( iNest >= MAX_NEST )
    {
	bug( "fwrite_obj: iNest hit MAX_NEST %d", iNest );
	return;
    }

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
//    if ( obj->prev_content && os_type != OS_CORPSE )
//	fwrite_obj( ch, obj->prev_content, fp, iNest, OS_CARRY );

    /*
     * Castrate storage characters.
     * Catch deleted objects                                    -Thoric
     * Do NOT save prototype items!				-Thoric
     */
/*    if ( (ch && ch->level < obj->level)
    || ( obj->item_type == ITEM_KEY && !IS_OBJ_STAT(obj, ITEM_CLANOBJECT ))
    ||   obj_extracted(obj) 
    ||   IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
	return;*/

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->prev_content && os_type != OS_CORPSE )
    if( os_type == OS_CARRY )
	   fwrite_obj( ch, obj->prev_content, fp, iNest, OS_CARRY, copyover );

    /*
     * Castrate storage characters.
     * Catch deleted objects                                    -Thoric
     * Do NOT save prototype items!				-Thoric
     */
    if( !copyover )
    {
       if ( (ch && ch->level < obj->level)
	// Keys don't gib! - Gatz
       //|| ( obj->item_type == ITEM_KEY && !IS_OBJ_STAT(obj, ITEM_CLANOBJECT ) )
       ||   obj_extracted(obj) 
       ||   IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
       return;
    }

    /*    Munch magic flagged containers for now - bandaid */
    if ( obj->item_type == ITEM_CONTAINER
    &&   IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	xTOGGLE_BIT( obj->extra_flags, ITEM_MAGIC );

    /* DO NOT save corpses lying on the ground as a copyover item, they already saved elsewhere! - Samson */
    if( copyover && obj->item_type == ITEM_CORPSE_PC )
	return;
    /* Corpse saving. -- Altrag */
    fprintf( fp, (os_type == OS_CORPSE ? "#CORPSE\n" : "#OBJECT\n") );

    if ( iNest )
	fprintf( fp, "Nest         %d\n",	iNest		     );
    if ( obj->count > 1 )
	fprintf( fp, "Count        %d\n",	obj->count	     );
   if( obj->name && obj->pIndexData->name && str_cmp( obj->name, obj->pIndexData->name ) )
      fprintf( fp, "Name         %s~\n", obj->name );
   if( obj->short_descr && obj->pIndexData->short_descr && str_cmp( obj->short_descr, obj->pIndexData->short_descr ) )
      fprintf( fp, "ShortDescr   %s~\n", obj->short_descr );
   if( obj->description && obj->pIndexData->description && str_cmp( obj->description, obj->pIndexData->description ) )
      fprintf( fp, "Description  %s~\n", obj->description );
   if( obj->action_desc && obj->pIndexData->action_desc && str_cmp( obj->action_desc, obj->pIndexData->action_desc ) )
      fprintf( fp, "ActionDesc   %s~\n", obj->action_desc );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
    if ( (os_type == OS_CORPSE  )&& obj->in_room )
    {
      fprintf( fp, "Room         %d\n",   obj->in_room->vnum         );
      fprintf( fp, "Rvnum	   %d\n",   obj->room_vnum		   );
    }
    if ( !xSAME_BITS(obj->extra_flags, obj->pIndexData->extra_flags) )
	fprintf( fp, "ExtraFlags   %s\n",	print_bitvector(&obj->extra_flags) );
    if ( obj->wear_flags != obj->pIndexData->wear_flags )
	fprintf( fp, "WearFlags    %d\n",	obj->wear_flags	     );
    wear_loc = -1;
    for ( wear = 0; wear < MAX_WEAR; wear++ )
	for ( x = 0; x < MAX_LAYERS; x++ )
	   if ( obj == save_equipment[wear][x] )
	   {
		wear_loc = wear;
		break;
	   }
	   else
	   if ( !save_equipment[wear][x] )
		break;
    if ( wear_loc != -1 )
	fprintf( fp, "WearLoc      %d\n",	wear_loc	     );
    if ( obj->item_type != obj->pIndexData->item_type )
	fprintf( fp, "ItemType     %d\n",	obj->item_type	     );
    if ( obj->weight != obj->pIndexData->weight )
      fprintf( fp, "Weight       %d\n",	obj->weight		     );
    if ( obj->level )
      fprintf( fp, "Level        %d\n",	obj->level		     );
    if ( obj->timer )
      fprintf( fp, "Timer        %d\n",	obj->timer		     );
    if ( obj->cost != obj->pIndexData->cost )
      fprintf( fp, "Cost         %d\n",	obj->cost		     );

    fprintf( fp, "Coords	%d %d %d\n", obj->x, obj->y, obj->map );

    if ( obj->value[0] || obj->value[1] || obj->value[2]
    ||   obj->value[3] || obj->value[4] || obj->value[5] )
      fprintf( fp, "Values       %d %d %d %d %d %d\n",
        obj->pIndexData->value[0], obj->pIndexData->value[1], obj->pIndexData->value[2],
        obj->pIndexData->value[3], obj->value[4], obj->pIndexData->value[5]     );

/*	obj->value[0], obj->value[1], obj->value[2],
	obj->value[3], obj->value[4], obj->value[5]     );*/



    for ( ed = obj->first_extradesc; ed; ed = ed->next )
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
	    ed->keyword, ed->description );

    fprintf( fp, "End\n\n" );

    if ( obj->first_content )
	fwrite_obj( ch, obj->last_content, fp, iNest + 1, OS_CARRY, copyover );

    return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name, bool preload, bool copyover )
{
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    struct stat fst;
    int i, x;
    extern FILE *fpArea;
    extern char strArea[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    CREATE( ch, CHAR_DATA, 1 );
    for ( x = 0; x < MAX_WEAR; x++ )
	for ( i = 0; i < MAX_LAYERS; i++ )
	    save_equipment[x][i] = NULL;
    clear_char( ch );
    loading_char = ch;

    CREATE( ch->pcdata, PC_DATA, 1 );
    d->character		= ch;
    ch->desc			= d;
    ch->pcdata->filename	= STRALLOC( name );
    ch->name			= NULL;
    ch->act			= multimeb(PLR_BLANK, PLR_COMBINE, PLR_PROMPT, -1);
    ch->perm_str		= 13;
    ch->perm_int		= 13;
    ch->perm_wis		= 13;
    ch->perm_dex		= 13;
    ch->perm_con		= 13;
    ch->perm_cha		= 13;
    ch->perm_lck		= 13;
    ch->was_in_room		= NULL;
    ch->pcdata->wizinvis		= 0;
    ch->pcdata->charmies		= 0;
    ch->mobinvis			= 0;
    for(i = 0; i < MAX_SKILL; i++)
    {
      ch->pcdata->learned[i]		= 0;
      ch->pcdata->skill_level[i]	= 0;
    }
    for( i = 0; i < MAX_ASKILL; i++) 
    {
      ch->pcdata->learned2[i]            = 0;
      ch->pcdata->skill_level2[i]        = 0;
    }

    ch->pcdata->release_date		= 0;
    ch->pcdata->helled_by		= NULL;
    ch->pcdata->silenced_date           = 0;
    ch->pcdata->silenced_by             = NULL;
    ch->style 				= STYLE_FIGHTING;
    ch->comments                        = NULL;    /* comments */
    ch->pcdata->pagerlen		= 24;
    ch->pcdata->first_ignored		= NULL;    /* Ignore list */
    ch->pcdata->last_ignored		= NULL;
    ch->pcdata->secedit			= SECT_OCEAN; /* Initialize Map OLC sector - Samson 8-1-99 */
    ch->pcdata->tell_history		= NULL;	/* imm only lasttell cmnd */
    ch->pcdata->lt_index		= 0;	/* last tell index */
    ch->pcdata->email			= NULL; /* Initialize email address - Samson 1-4-99 */
    ch->pcdata->homepage		= NULL; /* Initialize homepage - Samson 1-4-99 */
    ch->pcdata->aim			= NULL; /* Initalize aim# - Samson 1-4-99 */
    ch->plr_home                        = NULL;
    ch->plr_home1                       = NULL;
    ch->plr_home2                       = NULL;
    ch->plr_home3                       = NULL;
    ch->plr_home4                       = NULL;
    ch->pcdata->copyover                 = FALSE; /* Never changed except when PC is saved during copyover save */

    
    found = FALSE;
    sprintf( strsave, "%s%c/%s", PLAYER_DIR, tolower(name[0]),
			capitalize( name ) );
    if ( stat( strsave, &fst ) != -1 )
    {
      if ( fst.st_size == 0 )
      {
	sprintf( strsave, "%s%s", BACKUP_DIR, capitalize( name ) );
	send_to_char( "Restoring your backup player file...", ch );
      }
      else
      {
	sprintf( buf, "%s player data for: %s (%dK)",
	  preload ? "Preloading" : "Loading", ch->pcdata->filename,
		(int) fst.st_size/1024 );
	log_string_plus( buf, LOG_COMM, LEVEL_GREATER );
      }
    }
    /* else no player file */

    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	/* Cheat so that bug will show line #'s -- Altrag */
	fpArea = fp;
	strcpy(strArea, strsave);
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
		bug( "Load_char_obj: # not found.", 0 );
		bug( name, 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !strcmp( word, "PLAYER" ) )
	    {
		fread_char ( ch, fp, preload, copyover );
		if ( preload )
		  break;
	    }
	    else
	    if ( !strcmp( word, "OBJECT" ) )	/* Objects	*/
		fread_obj  ( ch, fp, OS_CARRY );
	    else 
	    if ( !strcmp( word, "COMMENT") )
		fread_comment(ch, fp );		/* Comments	*/
	    else
            if( !strcmp( word, "MOBILE" ) )
            {
                 CHAR_DATA *mob;

                 if( !( mob = fread_mobile( fp ) ) )
                 {
                    bug( "Deleted mob saved on %s - skipping", ch->name );
                    break;
                 }
                 ch->pcdata->pet = mob;
                 mob->master = ch;
            }
	    else
	    if ( !strcmp( word, "END"    ) )	/* Done		*/
		break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		bug( name, 0 );
		break;
	    }
	}
	fclose( fp );
	fpArea = NULL;
	strcpy(strArea, "$");
    }

    if ( !found )
    {
        ch->name	   		= STRALLOC( name );
        ch->fusionname                  = STRALLOC( "" );
	ch->short_descr			= STRALLOC( "" );
	ch->long_descr			= STRALLOC( "" );
	ch->description			= STRALLOC( "" );
	ch->editor			= NULL;
	ch->pcdata->clan_name		= STRALLOC( "" );
	ch->pcdata->clan		= NULL;
	ch->pcdata->pet			= NULL;
	ch->pcdata->pwd			= str_dup( "" );
        /* every characters starts at default board from login.. this board
         should be read_level == 0 !
         */
        ch->pcdata->board		= &boards[DEFAULT_BOARD];
	ch->pcdata->bamfin		= str_dup( "" );
	ch->pcdata->bamfout		= str_dup( "" );
	ch->pcdata->rank		= str_dup( "" );
	ch->pcdata->bestowments		= str_dup( "" );
	ch->pcdata->title		= STRALLOC( "" );
	ch->pcdata->homepage		= str_dup( "" );
	ch->pcdata->email               = str_dup( "" ); /* Samson 4-19-98 */
	ch->pcdata->aim                 = str_dup( "" ); /* Samson 1-4-99 */
	ch->pcdata->bio 		= STRALLOC( "" );
	ch->pcdata->authed_by		= STRALLOC( "" );
	ch->pcdata->prompt		= STRALLOC( "" );
	ch->pcdata->fprompt		= STRALLOC( "" );
	ch->pcdata->r_range_lo		= 0;
	ch->pcdata->r_range_hi		= 0;
	ch->pcdata->m_range_lo		= 0;
	ch->pcdata->m_range_hi		= 0;
	ch->pcdata->o_range_lo		= 0;
	ch->pcdata->o_range_hi		= 0;
	ch->pcdata->wizinvis		= 0;
    }
    else
    {
	if ( !ch->name )
    		ch->name	= STRALLOC( name );
	if ( !ch->pcdata->clan_name )
	{
	  ch->pcdata->clan_name	= STRALLOC( "" );
	  ch->pcdata->clan	= NULL;
	}
        if ( !ch->pcdata->bio )
          ch->pcdata->bio	 = STRALLOC( "" );

	if ( !ch->pcdata->authed_by )
	  ch->pcdata->authed_by	 = STRALLOC( "" );

	if ( xIS_SET(ch->act, PLR_FLEE) )
	  xREMOVE_BIT(ch->act, PLR_FLEE);

	if ( IS_IMMORTAL( ch ) )
	{
	  if ( ch->pcdata->wizinvis < 2 )
	    ch->pcdata->wizinvis = ch->level;
 	  assign_area( ch );
	}
	if ( file_ver > 1 )
      {
	  for ( i = 0; i < MAX_WEAR; i++ )
	    for ( x = 0; x < MAX_LAYERS; x++ )
		if ( save_equipment[i][x] )
		{
		    equip_char( ch, save_equipment[i][x], i );
		    save_equipment[i][x] = NULL;
		}
		else
		    break;
      }

    }
    
    loading_char = NULL;
    return found;
}



/*
 * Read in a char.
 */

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

void fread_char( CHAR_DATA *ch, FILE *fp, bool preload, bool copyover )
{
    char buf[MAX_STRING_LENGTH];
    char *line;
    char *word;
    int x1, x2, x3, x4, x5, x6, x7;
    sh_int killcnt;
    int sn;
    bool fMatch;
    int max_colors = 0; /* Color code */

    file_ver = 0;
    killcnt = 0;

    /* Setup color values in case player has none set - Samson */
    memcpy( &ch->colors, &default_set, sizeof(default_set) );

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

	case 'A':
	    KEY( "Act",		ch->act,		fread_bitvector( fp ) );
            if ( !strcmp( word, "AtkDef") )
            {
              ch->patk = fread_number(fp);
              ch->pdef = fread_number(fp);
              ch->eatk = fread_number(fp);
              ch->edef = fread_number(fp);
              ch->evasion = fread_number(fp);
              ch->critical = fread_number(fp);
              ch->accuracy = fread_number(fp);
              fMatch = TRUE;
              break;
            }
            if ( !strcmp( word, "AtkDef2") )
            {
              ch->patk2 = fread_number(fp);
              ch->pdef2 = fread_number(fp);
              ch->eatk2 = fread_number(fp);
              ch->edef2 = fread_number(fp);
              fMatch = TRUE;
              break;
            }
            if ( !str_cmp( word, "Aura" ) )
            {
                  ch->auracolor = fread_number ( fp );
                  ch->aurapowerup = fread_number ( fp );
                  fMatch = TRUE;
                  break;
            }

	    KEY( "Alignment",	ch->alignment,		fread_number( fp ) );
            KEY( "Armory",      ch->armory,             fread_number( fp ) );
            KEY( "AIM", ch->pcdata->aim,        fread_string_nohash( fp ) );
	    if ( !strcmp( word, "AttrPerm" ) )
	    {
		line = fread_line( fp );
		x1=x2=x3=x4=x5=x6=x7=0;
		sscanf( line, "%d %d %d %d %d %d %d",
		      &x1, &x2, &x3, &x4, &x5, &x6, &x7 );
		ch->perm_str = x1;
		ch->perm_int = x2;
		ch->perm_wis = x3;
		ch->perm_dex = x4;
		ch->perm_con = x5;
		ch->perm_cha = x6;
		ch->perm_lck = x7;
		if (!x7 || x7 == 0)
		  ch->perm_lck = 13;
		fMatch = TRUE;
		break;
	    }
            if ( !str_cmp( word, "ASkill" ) )
            {
                int value, value2;

                if ( preload )
                  word = "End";
                else
                {
                  value = fread_number( fp );
		  value2 = fread_number( fp );
                  sn = askill_lookup( ch, fread_word( fp ) );
                  if ( sn < 0 )
                    bug( "Fread_char: unknown askill.", 0 );
                  else
                  {
                    ch->pcdata->learned2[sn] = value;
		    ch->pcdata->skill_level2[sn] = value2;
                  }
                  fMatch = TRUE;
                  break;
                }
            }

	    KEY( "AuthedBy",	ch->pcdata->authed_by,	fread_string( fp ) );
	    break;

	case 'B':
            KEY( "Balance",     ch->balance,            fread_number( fp ) );
     
	    KEY( "Bamfin",	ch->pcdata->bamfin,	fread_string_nohash( fp ) );
	    KEY( "Bamfout",	ch->pcdata->bamfout,	fread_string_nohash( fp ) );
            KEY( "Barrier",     ch->barrier,            fread_number( fp ) );
            KEY( "Breath",      ch->breath,            fread_number( fp ) );


            /* Read in board status */
            if (!str_cmp(word, "Boards" ))
            {
                int i,num = fread_number (fp); /* number of boards saved */
                char *boardname;

                for (; num ; num-- ) /* for each of the board saved */
                {
                    boardname = fread_word (fp);
                    i = board_lookup (boardname); /* find board number */

                    if (i == BOARD_NOTFOUND) /* Does board still exist ? */
                    {
                        sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.", ch->name, boardname);
                        log_string (buf);
                        fread_number (fp); /* read last_note and skip info */
                    }
                    else /* Save it */
                        ch->pcdata->last_note[i] = fread_number (fp);
                }	 /* for */

                fMatch = TRUE;
            } /* Boards */

	    KEY( "Bestowments", ch->pcdata->bestowments, fread_string_nohash( fp ) );
	    KEY( "Bio",		ch->pcdata->bio,	fread_string( fp ) );
            KEY( "Biomass",     ch->biomass,            fread_number( fp ) );
	    break;

	case 'C':
	    if ( !strcmp( word, "Clan" ) )
	    {
		ch->pcdata->clan_name = fread_string( fp );

		if ( !preload
		&&   ch->pcdata->clan_name[0] != '\0'
		&& ( ch->pcdata->clan = get_clan( ch->pcdata->clan_name )) == NULL )
		{
		  sprintf( buf, "Warning: the organization %s no longer exists, and therefore you no longer\n\rbelong to that organization.\n\r",
		           ch->pcdata->clan_name );
		  send_to_char( buf, ch );
		  STRFREE( ch->pcdata->clan_name );
		  ch->pcdata->clan_name = STRALLOC( "" );
		}
		fMatch = TRUE;
		break;
	    }
	    KEY( "Class",	ch->class,		fread_number( fp ) );

          /* Load color values - Samson 9-29-98 */
          {
		int x;
		if ( !str_cmp( word, "Colors" ) )
            {
               for ( x = 0; x < max_colors; x++ )
                  ch->colors[x] = fread_number( fp );
               fMatch = TRUE;
               break;
            }
          }

	    if ( !str_cmp( word, "Coordinates" ) )
	    {
		ch->x = fread_number( fp );
		ch->y = fread_number( fp );
		ch->map	 = fread_number( fp );

		if( !IS_PLR_FLAG( ch, PLR_ONMAP ) )
	      {
		   ch->x = -1;
		   ch->y = -1;
		   ch->map	    = -1;
		}

		fMatch = TRUE;
		break;
	    }
	    break;

	case 'D':
	    KEY( "Deaf",	ch->deaf,		fread_number( fp ) );
	    KEY( "Description",	ch->description,	fread_string( fp ) );
            KEY( "Drunk",        ch->drunk,               fread_number( fp ) );
	    break;

	/* 'E' was moved to after 'S' */
        case 'F':
	    if ( !strcmp( word, "Filename" ) )
	    {
		/*
		 * File Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
            KEY( "Flags",       ch->pcdata->flags,      fread_bitvector( fp ) );
//	    KEY( "Flags",	ch->pcdata->flags,	fread_number( fp ) );
	    KEY( "FPrompt",	ch->pcdata->fprompt,	fread_string( fp ) );
            break;

	case 'G':
	    KEY( "Glory",       ch->pcdata->bless, 	fread_number( fp ) );
	    KEY( "Gold",	ch->gold,		fread_number( fp ) );
            if ( !str_cmp( word, "Gravity" ) )
            {
                  ch->gravset = fread_number ( fp );
                  ch->gravsucc = fread_number ( fp );
		  ch->gravtotal = fread_number ( fp );
                  fMatch = TRUE;
                  break;
            }
            break;

	case 'H':
            if ( !str_cmp( word, "Height" ) )
            {
                  ch->height = fread_number ( fp );
                  ch->height_mod = fread_number ( fp );
                  fMatch = TRUE;
                  break;
            }

	    if ( !strcmp(word, "Helled") )
	    {
	      ch->pcdata->release_date = fread_number(fp);
	      ch->pcdata->helled_by = fread_string(fp);
	      fMatch = TRUE;
	      break;
	    }

	    KEY( "Homepage",	ch->pcdata->homepage,	fread_string_nohash( fp ) );
	    if ( !strcmp( word, "HpManaMove" ) )
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'I':
	    if(!strcmp(word, "Ignored"))
	    {
	    	char *temp;
	    	char fname[1024];
	    	struct stat fst;
	    	int ign;
	    	IGNORE_DATA *inode;
	    	
	    	/* Get the name */
	    	temp = fread_string(fp);
	    	
	    	sprintf(fname, "%s%c/%s", PLAYER_DIR,
	    		tolower(temp[0]), capitalize(temp));
	    	
	    	/* If there isn't a pfile for the name */
	    	/* then don't add it to the list       */
	    	if(stat(fname, &fst) == -1)
	    	{
			if( temp )
			  STRFREE( temp );
	    		fMatch = TRUE;
	    		break;
	    	}
	    	
	    	/* Count the number of names already ignored */
	    	for(ign = 0, inode = ch->pcdata->first_ignored; inode;
	    				inode = inode->next)
	    	{
	    		ign++;
	    	}
	    	
	    	/* Add the name unless the limit has been reached */
	    	if(ign >= MAX_IGN)
	    	{
	    		bug("fread_char: too many ignored names");
	    	}
	    	else
	    	{
	    		/* Add the name to the list */
	    		CREATE(inode, IGNORE_DATA, 1);
	    		inode->name = STRALLOC(temp);
	    		inode->next = NULL;
	    		inode->prev = NULL;
	    		
	    		LINK(inode, ch->pcdata->first_ignored,
	    			ch->pcdata->last_ignored, next,
	    			prev);
	    	}
		if( temp )
		  STRFREE( temp );
	    	fMatch = TRUE;
	    	break;
	    }
	    KEY( "IllegalPK",	ch->pcdata->illegal_pk,	fread_number( fp ) );
	    break;

	case 'K':
 	    KEY( "Kili",	ch->pcdata->kili,	fread_number( fp ));
	    if ( !strcmp( word, "Killed" ) )
	    {
		fMatch = TRUE;
		if ( killcnt >= MAX_KILLTRACK )
		    bug( "fread_char: killcnt (%d) >= MAX_KILLTRACK", killcnt );
		else
		{
		    ch->pcdata->killed[killcnt].vnum    = fread_number( fp );
		    ch->pcdata->killed[killcnt++].count = fread_number( fp );
		}
	    }
            KEY( "Kimod",         ch->kimod,                fread_float( fp ) );

	    break;
      
	case 'L':
            KEY( "LastOn",      ch->pcdata->lastlogon,          fread_number( fp ) );
	    KEY( "Level",	ch->level,		fread_number( fp ) );
	    KEY( "LongDescr",	ch->long_descr,		fread_string( fp ) );
	    break;

	case 'M':
            if ( !strcmp( word, "ModPerm" ) )
            {
                line = fread_line( fp );
                x1=x2=x3=x4=x5=x6=x7=0;
                sscanf( line, "%d %d %d %d %d %d %d",
                      &x1, &x2, &x3, &x4, &x5, &x6, &x7 );
                ch->mod_str = x1;
                ch->mod_int = x2;
                ch->mod_wis = x3;
                ch->mod_dex = x4;
                ch->mod_con = x5;
                ch->mod_cha = x6;
                ch->mod_lck = x7;
                if (!x7 || x7 == 0)
                  ch->mod_lck = 0;
                fMatch = TRUE;
                break;
            }

            KEY ("Mapsize", ch->mapsize, fread_number( fp ) );
	    KEY( "MaxColors",   max_colors,             fread_number( fp ) );
	    KEY( "MDeaths",	ch->pcdata->mdeaths,	fread_number( fp ) );
	    KEY( "Minsnoop",	ch->pcdata->min_snoop,	fread_number( fp ) );
	    KEY( "MKills",	ch->pcdata->mkills,	fread_number( fp ) );
	    KEY( "Mobinvis",	ch->mobinvis,		fread_number( fp ) );
            KEY( "Mod",         ch->mod,                fread_float( fp ) );
	    if ( !strcmp( word, "MobRange" ) )
	    {
		ch->pcdata->m_range_lo = fread_number( fp );
		ch->pcdata->m_range_hi = fread_number( fp );
		fMatch = TRUE;
	    }
	    break;

	case 'N':
	    KEY ("Name", ch->name, fread_string( fp ) );
            KEY ("NdCount", ch->ndcount, fread_number( fp ) );
	    break;
	case 'O':
	    if ( !strcmp( word, "ObjRange" ) )
	    {
		ch->pcdata->o_range_lo = fread_number( fp );
		ch->pcdata->o_range_hi = fread_number( fp );
		fMatch = TRUE;
	    }
	    break;

	case 'P':
            KEY( "Powerup",     ch->powerup,            fread_bitvector( fp ) );
	    KEY( "Pagerlen",	ch->pcdata->pagerlen,	fread_number( fp ) );
	    KEY( "Password",	ch->pcdata->pwd,	fread_string_nohash( fp ) );
	    KEY( "PDeaths",	ch->pcdata->pdeaths,	fread_number( fp ) );
	    KEY( "PKills",	ch->pcdata->pkills,	fread_number( fp ) );
	    KEY( "Played",	ch->played,		fread_number( fp ) );
	    /* KEY( "Position",	ch->position,		fread_number( fp ) );*/
            /*
             *  new positions are stored in the file from 100 up
             *  old positions are from 0 up
             *  if reading an old position, some translation is necessary
             */
            if (!strcmp ( word, "Position" ) )
            {
               ch->position          = fread_number( fp );
               if(ch->position<100){
                  switch(ch->position){
                      default: ;
                      case 0: ;
                      case 1: ;
                      case 2: ;
                      case 3: ;
                      case 4: break;
                      case 5: ch->position=6; break;
                      case 6: ch->position=8; break;
                      case 7: ch->position=9; break;
                      case 8: ch->position=12; break;
                      case 9: ch->position=13; break;
                      case 10: ch->position=14; break;
                      case 11: ch->position=15; break;
                  }
                  fMatch = TRUE;
               } else {
                  ch->position-=100;
                  fMatch = TRUE;
               }
            }
 	    KEY( "Prompt",	ch->pcdata->prompt,	fread_string( fp ) );
            if (!strcmp ( word, "PGTimer" ) )
            {
                add_timer( ch , TIMER_GRAVITY, fread_number(fp), NULL, 0 );
                fMatch = TRUE;
                break;
            }
	    if (!strcmp ( word, "PTimer" ) )
	    {
		add_timer( ch , TIMER_PKILLED, fread_number(fp), NULL, 0 );	
		fMatch = TRUE;
		break;
	    }

            if ( !str_cmp( word, "PlrHome" ) )
            {
                ch->plr_home = get_room_index( fread_number( fp ) );
                if ( !ch->plr_home )
                    ch->plr_home = NULL;
                fMatch = TRUE;
                break;
            }
            if ( !str_cmp( word, "PlrHome1" ) )
            {
                ch->plr_home1 = get_room_index( fread_number( fp ) );
                if ( !ch->plr_home1 )
                    ch->plr_home1 = NULL;
                fMatch = TRUE;
                break;
            }
            if ( !str_cmp( word, "PlrHome2" ) )
            {
                ch->plr_home2 = get_room_index( fread_number( fp ) );
                if ( !ch->plr_home2 )
                    ch->plr_home2 = NULL;
                fMatch = TRUE;
                break;
            }
            if ( !str_cmp( word, "PlrHome3" ) )
            {
                ch->plr_home3 = get_room_index( fread_number( fp ) );
                if ( !ch->plr_home3 )
                    ch->plr_home3 = NULL;
                fMatch = TRUE;
                break;
            }
            if ( !str_cmp( word, "PlrHome4" ) )
            {
                ch->plr_home4 = get_room_index( fread_number( fp ) );
                if ( !ch->plr_home4 )
                    ch->plr_home4 = NULL;
                fMatch = TRUE;
                break;
            }

	    break;

        case 'Q':
	    KEY( "Quest",	ch->quest,		fread_float( fp ) );
	case 'R':
	    KEY( "Race",        ch->race,		fread_number( fp ) );
	    KEY( "Rage",        ch->rage,               fread_number( fp ) );
            KEY( "Rank",        ch->pcdata->rank,	fread_string( fp ) );
	    KEY( "Restore_time",ch->pcdata->restore_time, fread_number( fp ) );

	    if ( !strcmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( !ch->in_room )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }
	    if ( !strcmp( word, "RoomRange" ) )
	    {
		ch->pcdata->r_range_lo = fread_number( fp );
		ch->pcdata->r_range_hi = fread_number( fp );
		fMatch = TRUE;
	    }

	    break;

	case 'S':
            KEY( "Skill",       ch->skill,            fread_bitvector( fp ) );
            KEY( "Suppress",    ch->suppress,            fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEY( "ShortDescr",	ch->short_descr,	fread_string( fp ) );
            KEY( "SKiGain",     ch->skigain,            fread_number( fp ) );
            KEY( "SkillGain",   ch->skillgain,		fread_number( fp ) );
            KEY( "SkillLvl",    ch->skill_lvl,          fread_number( fp ) );
            KEY( "Stat",        ch->stat,               fread_number( fp ) );
            KEY( "StatGain",    ch->statgain,           fread_number( fp ) );
	    KEY( "Style",	ch->style,		fread_number( fp ) );
            if ( !strcmp( word, "Set") )
            {
              ch->runset = fread_number(fp);
              ch->flyset = fread_number(fp);
              fMatch = TRUE;
              break;
            }
           if ( !strcmp(word, "Silenced") )
            {
              ch->pcdata->silenced_date = fread_number(fp);
              ch->pcdata->silenced_by = fread_string(fp);
              fMatch = TRUE;
              break;
            }
	    if ( !strcmp( word, "Site" ) )
	    {
		if ( !preload && !copyover)
		{
		  ch->pcdata->lasthost = STRALLOC( fread_word(fp) );
		  sprintf( buf, "Last connected from: %s\n\r", ch->pcdata->lasthost );
		  send_to_char( buf, ch );
		  if (ch->desc && ch->desc->host)
		  	ch->pcdata->lasthost = STRALLOC( ch->desc->host );
		}
		else
		  fread_to_eol( fp );
		fMatch = TRUE;
		if ( preload )
		  word = "End";
		else
		  break;
	    }
            if ( !str_cmp( word, "SSkill" ) )
            {
                int value, value2;

                if ( preload )
                  word = "End";
                else
                {
                  value = fread_number( fp );
		  value2 = fread_number( fp );
                  sn = sskill_lookup( ch, fread_word( fp ) );
                  if ( sn < 0 )
                    bug( "Fread_char: unknown sskill.", 0 );
                  else
                  {
                    ch->pcdata->learned[sn] = value;
		    ch->pcdata->skill_level[sn] = value2;
                  }
                  fMatch = TRUE;
                  break;
                }
            }
	    if ( strcmp( word, "End" ) )
		break;

	case 'E':
	    if ( !strcmp( word, "End" ) )
	    {
		if (!ch->short_descr)
		  ch->short_descr	= STRALLOC( "" );
		if (!ch->long_descr)
		  ch->long_descr	= STRALLOC( "" );
		if (!ch->description)
		  ch->description	= STRALLOC( "" );
		if (!ch->pcdata->pwd)
		  ch->pcdata->pwd	= str_dup( "" );
		if (!ch->pcdata->bamfin)
		  ch->pcdata->bamfin	= str_dup( "" );
		if (!ch->pcdata->bamfout)
		  ch->pcdata->bamfout	= str_dup( "" );
		if (!ch->pcdata->bio)
		  ch->pcdata->bio	= STRALLOC( "" );
		if (!ch->pcdata->rank)
		  ch->pcdata->rank	= str_dup( "" );
		if (!ch->pcdata->bestowments)
		  ch->pcdata->bestowments = str_dup( "" );
		if (!ch->pcdata->title)
		  ch->pcdata->title	= STRALLOC( "" );
		if (!ch->pcdata->homepage)
		  ch->pcdata->homepage	= str_dup( "" );
                if (!ch->pcdata->email)
		  ch->pcdata->email = str_dup( "" );
		if (!ch->pcdata->authed_by)
		  ch->pcdata->authed_by = STRALLOC( "" );
		if (!ch->pcdata->prompt )
		  ch->pcdata->prompt	= STRALLOC( "" );
	        if (!ch->pcdata->fprompt )
		  ch->pcdata->fprompt   = STRALLOC( "" );
		ch->editor		= NULL;
		killcnt = URANGE( 2, ((ch->level+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK );
		if ( killcnt < MAX_KILLTRACK )
		  ch->pcdata->killed[killcnt].vnum = 0;

		if ( IS_IMMORTAL( ch ) )
		{
			int i;
			
			CREATE(ch->pcdata->tell_history, char *, 26);
			for(i = 0; i < 26; i++)
				ch->pcdata->tell_history[i] = NULL;
		}
		if ( !ch->pcdata->prompt )
		  ch->pcdata->prompt = STRALLOC("");

                /* this disallows chars from being 6', 180lbs, but easier than a flag */
                if ( ch->height == 72)
                   ch->height = number_range(race_table[ch->race]->height *.9, race_table[ch->race]->height *1.1);
		if ( ch->weight ==180)
		   ch->weight = number_range(race_table[ch->race]->weight *.9, race_table[ch->race]->weight *1.1);
		REMOVE_PLR_FLAG( ch, PLR_MAPEDIT ); /* In case they saved while editing */
		return;
	    }
	    KEY( "Email",	ch->pcdata->email,	fread_string_nohash( fp ) );
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    break;

            case 'T':
	    KEY( "Trust", ch->trust, fread_number( fp ) );
            /* Let no character be trusted higher than one below maxlevel -- Narn */
	    ch->trust = UMIN( ch->trust, MAX_LEVEL - 1 );

	    if ( !strcmp( word, "Title" ) )
	    {
		ch->pcdata->title = fread_string( fp );
		if ( isalpha(ch->pcdata->title[0])
		||   isdigit(ch->pcdata->title[0]) )
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    if ( ch->pcdata->title )
		      STRFREE( ch->pcdata->title );
		    ch->pcdata->title = STRALLOC( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    if ( !strcmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Version",	file_ver,		fread_number( fp ) );
	    break;

	case 'W':
            if ( !str_cmp( word, "Weight" ) )
            {
                  ch->weight = fread_number ( fp );
                  ch->weight_mod = fread_number ( fp );
                  fMatch = TRUE;
                  break;
            }
	    KEY( "Wimpy",	ch->wimpy,		fread_number( fp ) );
	    KEY( "WizInvis",	ch->pcdata->wizinvis,	fread_number( fp ) );
	    break;
	}

	if ( !fMatch )
	{
	    sprintf( buf, "Fread_char: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}


void fread_obj( CHAR_DATA *ch, FILE *fp, sh_int os_type )
{
    OBJ_DATA *obj;
    char *word;
    char buf[MAX_STRING_LENGTH];
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    ROOM_INDEX_DATA *room = NULL;

//    if ( ch )
//	room = ch->in_room;
   if ( ch )
   {
	room = ch->in_room;
	if( ch->tempnum == -9999 )
	   file_ver = 0;
   }
    CREATE( obj, OBJ_DATA, 1 );
    obj->count		= 1;
    obj->wear_loc	= -1;
    obj->weight		= 1;
    obj->map = -1;
    obj->x = -1;
    obj->y = -1;
    fNest		= TRUE;		/* Requiring a Nest 0 is a waste */
    fVnum		= TRUE;
    iNest		= 0;

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

	case 'A':
	    KEY( "ActionDesc", obj->action_desc, 	fread_string( fp ) );
	    break;

	case 'C':
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    KEY( "Count",	obj->count,		fread_number( fp ) );
	    if( !strcmp( word, "Coords" ) )
	    {
		obj->x = fread_short( fp );
	 	obj->y = fread_short( fp );
		obj->map = fread_short( fp );
	      fMatch = TRUE;
		break;
	    }
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
            if ( !str_cmp( word, "Drunk" ) )
            {
                ch->drunk = fread_number( fp );
                if ( !ch->drunk )
                    ch->drunk = 0;
                fMatch = TRUE;
                break;
            }
	    break;

	case 'E':
	    KEY( "ExtraFlags",	obj->extra_flags,	fread_bitvector( fp ) );

	    if ( !strcmp( word, "ExtraDescr" ) )
	    {
		EXTRA_DESCR_DATA *ed;

		CREATE( ed, EXTRA_DESCR_DATA, 1 );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		LINK(ed, obj->first_extradesc, obj->last_extradesc, next, prev );
		fMatch 				= TRUE;
	    }

	    if ( !strcmp( word, "End" ) )
	    {
		if ( !fNest || !fVnum )
		{
		    if ( obj->name )
			sprintf ( buf, "Fread_obj: %s incomplete object.",
				obj->name );
		    else
			sprintf ( buf, "Fread_obj: incomplete object." );
		    bug( buf, 0 );
		    if ( obj->name )
		      STRFREE( obj->name        );
		    if ( obj->description )
		      STRFREE( obj->description );
		    if ( obj->short_descr )
		      STRFREE( obj->short_descr );
		    DISPOSE( obj );
		    return;
		}
		else
		{
		    sh_int wear_loc = obj->wear_loc;

		    if ( !obj->name )
			obj->name = QUICKLINK( obj->pIndexData->name );
		    if ( !obj->description )
			obj->description = QUICKLINK( obj->pIndexData->description );
		    if ( !obj->short_descr )
			obj->short_descr = QUICKLINK( obj->pIndexData->short_descr );
		    if ( !obj->action_desc )
			obj->action_desc = QUICKLINK( obj->pIndexData->action_desc );
		    LINK(obj, first_object, last_object, next, prev );
		    obj->pIndexData->count += obj->count;
		    if ( !obj->serial )
		    {
			cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1), 1);
			obj->serial = obj->pIndexData->serial = cur_obj_serial;
		    }
		    if ( fNest )
		      rgObjNest[iNest] = obj;
		    numobjsloaded += obj->count;
		    ++physicalobjects;
		    if ( file_ver > 1 || obj->wear_loc < -1
		    ||   obj->wear_loc >= MAX_WEAR )
		      obj->wear_loc = -1;
		    /* Corpse saving. -- Altrag */
		    if ( os_type == OS_CORPSE )
		    {
		        if ( !room )
		        {
		          bug( "Fread_obj: Corpse without room", 0);
		          room = get_room_index(ROOM_VNUM_LIMBO);
		        }
			/* Give the corpse a timer if there isn't one */

			if ( obj->timer < 1 )
			   obj->timer = 40;
			if ( room->vnum == ROOM_VNUM_HALLOFFALLEN 
				&& obj->first_content )
			   obj->timer = -1;
		        obj = obj_to_room( obj, room, NULL );
		    }
		    else if ( iNest == 0 || rgObjNest[iNest] == NULL )
		    {
			int slot = -1;
			bool reslot = FALSE;

			if ( file_ver > 1
			&&   wear_loc > -1
			&&   wear_loc < MAX_WEAR )
			{
			   int x;

			   for ( x = 0; x < MAX_LAYERS; x++ )
			      if ( !save_equipment[wear_loc][x] )
			      {
				  save_equipment[wear_loc][x] = obj;
				  slot = x;
				  reslot = TRUE;
				  break;
			      }
			   if ( x == MAX_LAYERS )
				bug( "Fread_obj: too many layers %d", wear_loc );
			}
			obj = obj_to_char( obj, ch );
			if ( reslot && slot != -1 )
			  save_equipment[wear_loc][slot] = obj;
		    }
		    else
		    {
			if ( rgObjNest[iNest-1] )
			{
			   separate_obj( rgObjNest[iNest-1] );
			   obj = obj_to_obj( obj, rgObjNest[iNest-1] );
			}
			else
			   bug( "Fread_obj: nest layer missing %d", iNest-1 );
		    }
		    if ( fNest )
		      rgObjNest[iNest] = obj;
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Level",	obj->level,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !strcmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		    iNest = 0;
		    fNest = FALSE;
		}
		fMatch = TRUE;
	    }
	    break;
	    
	case 'R':
	    KEY( "Room", room, get_room_index(fread_number(fp)) );
	    KEY( "Rvnum", obj->room_vnum,	fread_number(fp));
	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !strcmp( word, "Values" ) )
	    {
		int x1,x2,x3,x4,x5,x6;
		char *ln = fread_line( fp );

		x1=x2=x3=x4=x5=x6=0;
		sscanf( ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6 );
		/* clean up some garbage */

		obj->value[0]	= x1;
		obj->value[1]	= x2;
		obj->value[2]	= x3;
		obj->value[3]	= x4;
		obj->value[4]	= x5;
		obj->value[5]	= x6;
		fMatch		= TRUE;
		break;
	    }

	    if ( !strcmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		/*  bug( "Fread_obj: bad vnum %d.", vnum );  */
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    fVnum = FALSE;
		else
		{
		    fVnum = TRUE;
		    obj->cost = obj->pIndexData->cost;
		    obj->weight = obj->pIndexData->weight;
		    obj->item_type = obj->pIndexData->item_type;
		    obj->wear_flags = obj->pIndexData->wear_flags;
		    obj->extra_flags = obj->pIndexData->extra_flags;
		}
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    EXTRA_DESCR_DATA *ed;

	    bug( "Fread_obj: no match.", 0 );
	    bug( word, 0 );
	    fread_to_eol( fp );
	    if ( obj->name )
		STRFREE( obj->name        );
	    if ( obj->description )
		STRFREE( obj->description );
	    if ( obj->short_descr )
		STRFREE( obj->short_descr );
	    while ( (ed=obj->first_extradesc) != NULL )
	    {
		STRFREE( ed->keyword );
		STRFREE( ed->description );
		UNLINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
		DISPOSE( ed );
	    }
	    DISPOSE( obj );
	    return;
	}
    }
}

void set_alarm( long seconds )
{
#ifdef WIN32
    kill_timer ();    /* kill old timer */
    timer_code = timeSetEvent(seconds * 1000L, 1000, alarm_handler, 0, TIME_PERIODIC);
#else
    alarm( seconds );
#endif
}

/*
 * Based on last time modified, show when a player was last on	-Thoric
 */
void do_last( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char arg [MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    struct stat fst;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Usage: last <playername>\n\r", ch );
	return;
    }
    strcpy( name, capitalize(arg) );
    sprintf( buf, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), name );
    if ( stat( buf, &fst ) != -1 && check_parse_name( capitalize(name), FALSE ))
      sprintf( buf, "%s was last on: %s\r", name, ctime( &fst.st_mtime ) );
//      sprintf( buf, "%s was last on: %s\r", name, ctime( &victim->pcdata->lastlogon ) );
    else
      sprintf( buf, "%s was not found.\n\r", name );
   send_to_char( buf, ch );
}

/*
 * Added support for removeing so we could take out the write_corpses
 * so we could take it out of the save_char_obj function. --Shaddai
 */

void write_corpses( CHAR_DATA *ch, char *name, OBJ_DATA *objrem )
{
  OBJ_DATA *corpse;
  FILE *fp = NULL;
  
  /* Name and ch support so that we dont have to have a char to save their
     corpses.. (ie: decayed corpses while offline) */
  if ( ch && IS_NPC(ch) )
  {
    bug( "Write_corpses: writing NPC corpse.", 0 );
    return;
  }
  if ( ch )
    name = ch->name;
  /* Go by vnum, less chance of screwups. -- Altrag */
  for ( corpse = first_object; corpse; corpse = corpse->next )
    if ( corpse->pIndexData->vnum == OBJ_VNUM_CORPSE_PC &&
         corpse->in_room != NULL &&
        !str_cmp(corpse->short_descr+14, name) &&
	objrem != corpse )
    {
      if ( !fp )
      {
        char buf[127];
        
        sprintf(buf, "%s%s", CORPSE_DIR, capitalize(name));
        if ( !(fp = fopen(buf, "w")) )
        {
          bug( "Write_corpses: Cannot open file.", 0 );
          perror(buf);
          return;
        }
      }
      fwrite_obj(ch, corpse, fp, 0, OS_CORPSE, FALSE);
    }
  if ( fp )
  {
    fprintf(fp, "#END\n\n");
    fclose(fp);
  }
  else
  {
    char buf[127];
    
    sprintf(buf, "%s%s", CORPSE_DIR, capitalize(name));
    remove(buf);
  }
  return;
}

void load_corpses( void )
{
  DIR *dp;
  struct dirent *de;
  extern FILE *fpArea;
  extern char strArea[MAX_INPUT_LENGTH];
  extern int falling;
  
  if ( !(dp = opendir(CORPSE_DIR)) )
  {
    bug( "Load_corpses: can't open CORPSE_DIR", 0);
    perror(CORPSE_DIR);
    return;
  }

  falling = 1; /* Arbitrary, must be >0 though. */
  while ( (de = readdir(dp)) != NULL )
  {
    if ( de->d_name[0] != '.' )
    {
      sprintf(strArea, "%s%s", CORPSE_DIR, de->d_name );
      fprintf(stderr, "Corpse -> %s\n", strArea);
      if ( !(fpArea = fopen(strArea, "r")) )
      {
        perror(strArea);
        continue;
      }
      for ( ; ; )
      {
        char letter;
        char *word;
        
        letter = fread_letter( fpArea );
        if ( letter == '*' )
        {
          fread_to_eol(fpArea);
          continue;
        }
        if ( letter != '#' )
        {
          bug( "Load_corpses: # not found.", 0 );
          break;
        }
        word = fread_word( fpArea );
        if ( !strcmp(word, "CORPSE" ) )
          fread_obj( NULL, fpArea, OS_CORPSE );
        else if ( !strcmp(word, "OBJECT" ) )
          fread_obj( NULL, fpArea, OS_CARRY );
        else if ( !strcmp( word, "END" ) )
          break;
        else
        {
          bug( "Load_corpses: bad section.", 0 );
          break;
        }
      }
      fclose(fpArea);
    }
  }
  fpArea = NULL;
  strcpy(strArea, "$");
  closedir(dp);
  falling = 0;
  return;
}

/*
 * This will write one mobile structure pointed to be fp --Shaddai
 */

void fwrite_mobile( FILE *fp, CHAR_DATA *mob )
{
  if ( !IS_NPC( mob ) || !fp )
	return;
  fprintf( fp, "#MOBILE\n" );
  fprintf( fp, "Vnum	%d\n", mob->pIndexData->vnum );
  if ( mob->in_room )
	fprintf( fp, "Room	%d\n", 
       		(  mob->in_room == get_room_index( ROOM_VNUM_LIMBO )
        		&& mob->was_in_room )
            		? mob->was_in_room->vnum
            		: mob->in_room->vnum ); 
  fprintf( fp, "Coordinates  %d %d %d\n", mob->x, mob->y, mob->map );
   if( mob->name && mob->pIndexData->player_name && str_cmp( mob->name, mob->pIndexData->player_name ) )
      fprintf( fp, "Name     %s~\n", mob->name );
   if( mob->short_descr && mob->pIndexData->short_descr && str_cmp( mob->short_descr, mob->pIndexData->short_descr ) )
      fprintf( fp, "Short    %s~\n", mob->short_descr );
   if( mob->long_descr && mob->pIndexData->long_descr && str_cmp( mob->long_descr, mob->pIndexData->long_descr ) )
      fprintf( fp, "Long    %s~\n", mob->long_descr );
   if( mob->description && mob->pIndexData->description && str_cmp( mob->description, mob->pIndexData->description ) )
      fprintf( fp, "Description %s~\n", mob->description );
  fprintf( fp, "Position %d\n", mob->position );
  fprintf( fp, "Flags %s\n",   print_bitvector(&mob->act) );
/* Might need these later --Shaddai
  de_equip_char( mob );
  re_equip_char( mob );
  */
  if ( mob->first_carrying )
	fwrite_obj( mob, mob->last_carrying, fp, 0, OS_CARRY, FALSE );
  fprintf( fp, "EndMobile\n" );
  return;
}

/*
 * This will read one mobile structure pointer to by fp --Shaddai
 */
CHAR_DATA *  fread_mobile( FILE *fp )
{
  CHAR_DATA *mob = NULL;
  char *word;
  bool fMatch;
  int inroom = 0;
  ROOM_INDEX_DATA *pRoomIndex = NULL;

  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
  if ( !strcmp(word, "Vnum") )
  {
    int vnum;
    
    vnum = fread_number( fp );
    mob = create_mobile( get_mob_index(vnum) );
    if ( !mob )
    {
	for ( ; ; ) {
	  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
	  /* So we don't get so many bug messages when something messes up
	   * --Shaddai 
	   */
	  if ( !strcmp( word, "EndMobile" ) )
		break;
        }
	bug("Fread_mobile: No index data for vnum %d", vnum );
	return NULL;
    }
  }
  else
  {
	for ( ; ; ) {
	  word   = feof( fp ) ? "EndMobile" : fread_word( fp );
	  /* So we don't get so many bug messages when something messes up
	   * --Shaddai 
	   */
	  if ( !strcmp( word, "EndMobile" ) )
		break;
        }
	extract_char(mob, TRUE);
	bug("Fread_mobile: Vnum not found", 0 );
	return NULL;
  }
  for ( ; ;) {
       word   = feof( fp ) ? "EndMobile" : fread_word( fp );
       fMatch = FALSE; 
       switch ( UPPER(word[0]) ) {
	case '*':
           fMatch = TRUE;
           fread_to_eol( fp );
           break;  
	case '#':
		if ( !strcmp( word, "#OBJECT" ) )
			fread_obj ( mob, fp, OS_CARRY );
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
		if ( !strcmp( word, "EndMobile" ) )
		{
		if ( inroom == 0 )
			inroom = ROOM_VNUM_TEMPLE;
		pRoomIndex = get_room_index( inroom );
		if ( !pRoomIndex )
			pRoomIndex = get_room_index( ROOM_VNUM_TEMPLE );
		char_to_room(mob, pRoomIndex);
		return mob;
		}
		break;
 	case 'F':
		KEY( "Flags", mob->act, fread_bitvector(fp));
	case 'L':
		KEY( "Long", mob->long_descr, fread_string(fp ) );
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
	if ( !fMatch )
	{
	   bug ( "Fread_mobile: no match.", 0 );
	   bug ( word, 0 );
	}
  }
  return NULL;
}

/*
 * This will write in the saved mobile for a char --Shaddai
 */
void write_char_mobile( CHAR_DATA *ch , char *argument )
{
  FILE *fp;
  CHAR_DATA *mob;
  char buf[MAX_STRING_LENGTH];

  if ( IS_NPC( ch ) || !ch->pcdata->pet )
	return;

  fclose( fpReserve );
  if ( (fp = fopen( argument, "w")) == NULL )
  {
	sprintf(buf, "Write_char_mobile: couldn't open %s for writing!\n\r", 
		argument );
	bug(buf, 0 );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
  }
  mob = ch->pcdata->pet;
  fwrite_mobile( fp, mob );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

/*
 * This will read in the saved mobile for a char --Shaddai
 */

void read_char_mobile( char *argument )
{
  FILE *fp;
  CHAR_DATA *mob;
  char buf[MAX_STRING_LENGTH];

  fclose( fpReserve );
  if ( (fp = fopen( argument, "r")) == NULL )
  {
	sprintf(buf, "Read_char_mobile: couldn't open %s for reading!\n\r", 
		argument );
	bug(buf, 0 );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
  }
  mob = fread_mobile( fp );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

/*void read_finger( CHAR_DATA *ch, char *argument )
{
  FILE *fp;
  char buf [ MAX_STRING_LENGTH ];
  char fingload [ MAX_INPUT_LENGTH ];
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  buf[0] = '\0';

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );

  sprintf( fingload, "%s%c/%s.Finger", PLAYER_DIR, tolower(argument[0]),
                     capitalize( argument ) );
                     
  fclose(fpReserve); 
  if ( !( fp = fopen ( fingload, "r" ) ) )
  {
    sprintf( buf, "Load_finger_Info: fopen %s ", argument );
    bug( buf, 0 );
    perror( fingload );
    ch_printf(ch, "The character %s doesn't exist.\n\r", argument );
  }
  else
     fread_finger ( ch, fp );

  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  return;
} 

void fread_finger ( CHAR_DATA *ch, FILE *fp )
{  
  char *finger;
  char  buf[MAX_STRING_LENGTH];
  
  buf[0] = '\0';
   
  send_to_char("          &RF&zinger &RI&znfo&w\n\r", ch);
  send_to_char("          &W===========\n\r\n\r", ch);
  finger = fread_string( fp );
  ch_printf(ch, "%s\n\r", finger);
  return;
}

void fwrite_finger( CHAR_DATA *ch, FILE *fp )
{  
  fprintf( fp, "&zName&w:  &W%s\n\r",                ch->name        );
  fprintf( fp, "&zAge&w:   &W%d\n\r",            get_age( ch )   );
  fprintf( fp, "&zLevel&w: &W%d\n\r",              ch->level       );
  fprintf( fp, "&zSex&w:   &W%s\n\r",   ch->sex == SEX_MALE ? "Male" : ch->sex == SEX_FEMALE ? "Female" : "Neutral" );
  fprintf( fp, "&zClass&w: &W%s\n\r",      capitalize(get_class(ch)) );   
  fprintf( fp, "&zRace&w:  &W%s\n\r",        capitalize(get_race(ch)) ); 
  fprintf( fp, "&zTitle&w: &W%s\n\r",                       ch->pcdata->title );
  fprintf( fp, "&zLast&w:  &W%s\n\r", (char * ) ctime(&ch->logon) );
  return;
}

void save_finger( CHAR_DATA *ch )
{
    FILE *fp;
    char buf [MAX_STRING_LENGTH];
    char fng [MAX_INPUT_LENGTH];
 
    if( IS_NPC(ch) || ch->level < 2 )
        return;
    if ( ch->desc && ch->desc->original )
        ch = ch->desc->original;
    sprintf( fng, "%s%c/%s.Finger", PLAYER_DIR, tolower(ch->name[0]),  capitalize( ch->name ) );
    fclose(fpReserve);
    if( !(fp = fopen(fng, "w")) )
    {                
        sprintf( buf, "Save_finger: fopen %s ", ch->name);
        bug( buf, 0 );
        perror(fng);
    }
    else
        fwrite_finger( ch, fp );

    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}*/

