/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 *			 Tracking/hunting module			    *
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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


#define BFS_ERROR	   -1
#define BFS_ALREADY_THERE  -2
#define BFS_NO_PATH	   -3
#define BFS_MARK    536870912

#define TRACK_THROUGH_DOORS

extern sh_int	top_room;

/* You can define or not define TRACK_THOUGH_DOORS, above, depending on
   whether or not you want track to find paths which lead through closed
   or hidden doors.
*/

typedef struct bfs_queue_struct BFS_DATA;
struct bfs_queue_struct
{
    ROOM_INDEX_DATA *	room;
    char		dir;
    BFS_DATA *		next;
};

static BFS_DATA	*queue_head = NULL,
		*queue_tail = NULL,
		*room_queue = NULL;

/* Utility macros */
#define MARK(room)	(SET_BIT(	(room)->room_flags, BFS_MARK) )
#define UNMARK(room)	(REMOVE_BIT(	(room)->room_flags, BFS_MARK) )
#define IS_MARKED(room)	(IS_SET(	(room)->room_flags, BFS_MARK) )

bool valid_edge( EXIT_DATA *pexit )
{
    if ( pexit->to_room
#ifndef TRACK_THROUGH_DOORS
    &&  !IS_SET(pexit->exit_info, EX_CLOSED)
#endif
    &&  !IS_MARKED(pexit->to_room) )
	return TRUE;
    else
	return FALSE;
}

void bfs_enqueue(ROOM_INDEX_DATA *room, char dir)
{
    BFS_DATA *curr;

    curr = malloc( sizeof(BFS_DATA) );
    curr->room = room;
    curr->dir = dir;
    curr->next = NULL;

    if ( queue_tail )
    {
	queue_tail->next = curr;
	queue_tail = curr;
    }
    else
	queue_head = queue_tail = curr;
}


void bfs_dequeue(void)
{
    BFS_DATA *curr;

    curr = queue_head;

    if ( !(queue_head = queue_head->next) )
	queue_tail = NULL;
    free(curr);
}


void bfs_clear_queue(void) 
{
    while (queue_head)
	bfs_dequeue();
}

void room_enqueue(ROOM_INDEX_DATA *room)
{
   BFS_DATA *curr;

   curr = malloc( sizeof(BFS_DATA) );
   curr->room = room;
   curr->next = room_queue;

   room_queue = curr;
}

void clean_room_queue(void) 
{
    BFS_DATA *curr, *curr_next;

    for (curr = room_queue; curr; curr = curr_next )
    {
	UNMARK(curr->room);
	curr_next = curr->next;
	free(curr);
    }
    room_queue = NULL;
}


int find_first_step(ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist )
{
    int curr_dir, count;
    EXIT_DATA *pexit;

    if ( !src || !target )
    {
	bug("Illegal value passed to find_first_step (track.c)", 0 );
	return BFS_ERROR;
    }

    if (src == target)
	return BFS_ALREADY_THERE;

    if ( src->area != target->area )
	return BFS_NO_PATH;

    room_enqueue( src );
    MARK(src);

    /* first, enqueue the first steps, saving which direction we're going. */
    for ( pexit = src->first_exit; pexit; pexit = pexit->next )
	if (valid_edge(pexit))
	{
	    curr_dir = pexit->vdir;
            MARK(pexit->to_room);
	    room_enqueue(pexit->to_room);
            bfs_enqueue(pexit->to_room, curr_dir);
	}

    count = 0;
    while (queue_head)
    {
	if ( ++count > maxdist )
	{
	    bfs_clear_queue();
	    clean_room_queue();
	    return BFS_NO_PATH;
	}
	if (queue_head->room == target)
	{
	    curr_dir = queue_head->dir;
	    bfs_clear_queue();
	    clean_room_queue();
	    return curr_dir;
	}
	else
	{
	    for (pexit = queue_head->room->first_exit; pexit; pexit = pexit->next )
		if ( valid_edge(pexit) )
		{
		    curr_dir = pexit->vdir;
		    MARK(pexit->to_room);
		    room_enqueue(pexit->to_room);
		    bfs_enqueue(pexit->to_room,queue_head->dir);
		}
	    bfs_dequeue();
	}
    }
    clean_room_queue();

    return BFS_NO_PATH;
}




void found_prey( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    char victname[MAX_STRING_LENGTH];

    if (victim == NULL)
    {
	bug("Found_prey: null victim", 0);
	return;
    }

    if ( victim->in_room == NULL )
    {
        bug( "Found_prey: null victim->in_room", 0 );
        return;
    }

    sprintf( victname, IS_NPC( victim ) ? victim->short_descr : victim->name );

    if ( !can_see(ch, victim, FALSE) )
    {
	if ( number_percent( ) < 90 )
	    return;
	switch( number_bits( 2 ) )
	{
	case 0: sprintf( buf, "Don't make me find you, %s!", victname );
		do_say( ch, buf );
	        break;
	case 1: act( AT_ACTION, "$n sniffs around the room for $N.", ch, NULL, victim, TO_NOTVICT );
		act( AT_ACTION, "You sniff around the room for $N.", ch, NULL, victim, TO_CHAR );
		act( AT_ACTION, "$n sniffs around the room for you.", ch, NULL, victim, TO_VICT );
		sprintf( buf, "I can smell your blood!" );
		do_say( ch, buf );
		break;
	case 2: sprintf( buf, "I'm going to tear %s apart!", victname );
		do_yell( ch, buf );
		break;
	case 3: do_say( ch, "Just wait until I find you...");
		break;
	}
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	if ( number_percent( ) < 90 )
	    return;
	switch( number_bits( 2 ) )
	{
	case 0:	do_say( ch, "C'mon out, you coward!" );
		sprintf( buf, "%s is a bloody coward!", victname );
		do_yell( ch, buf );
		break;
	case 1: sprintf( buf, "Let's take this outside, %s", victname );
		do_say( ch, buf );
		break;
	case 2: sprintf( buf, "%s is a yellow-bellied wimp!", victname );
		do_yell( ch, buf );
		break;
	case 3: act( AT_ACTION, "$n takes a few swipes at $N.", ch, NULL, victim, TO_NOTVICT );
		act( AT_ACTION, "You try to take a few swipes $N.", ch, NULL, victim, TO_CHAR );
		act( AT_ACTION, "$n takes a few swipes at you.", ch, NULL, victim, TO_VICT );
		break;
	}
	return;
    }

    switch( number_bits( 2 ) )
    {
     case 0: sprintf( buf, "Your blood is mine, %s!", victname );
	     do_yell( ch, buf);
	     break;
     case 1: sprintf( buf, "Alas, we meet again, %s!", victname );
     	     do_say( ch, buf );
     	     break;
     case 2: sprintf( buf, "What do you want on your tombstone, %s?", victname );
     	     do_say( ch, buf );
     	     break;
     case 3: act( AT_ACTION, "$n lunges at $N from out of nowhere!", ch, NULL, victim, TO_NOTVICT );
	     act( AT_ACTION, "You lunge at $N catching $M off guard!", ch, NULL, victim, TO_CHAR );
	     act( AT_ACTION, "$n lunges at you from out of nowhere!", ch, NULL, victim, TO_VICT );
    }
    stop_hunting( ch );
    set_fighting( ch, victim );
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
} 

void hunt_victim( CHAR_DATA *ch )
{
    bool found;
    CHAR_DATA *tmp;
    EXIT_DATA *pexit;
    sh_int ret;

    if (!ch || !ch->hunting||ch->position<5)
	return;

    /* make sure the char still exists */
    for ( found = FALSE, tmp = first_char; tmp && !found; tmp = tmp->next )
	if ( ch->hunting->who == tmp )
	    found = TRUE;

    if (!found)
    {
	do_say(ch, "Damn!  My prey is gone!!" );
	stop_hunting( ch );
	return;
    }

    if ( ch->in_room == ch->hunting->who->in_room )
    {
	if ( ch->fighting )
	    return;
	found_prey( ch, ch->hunting->who );
	return;
    }
   
    ret = find_first_step(ch->in_room, ch->hunting->who->in_room, 500 + ch->level * 25);
    if ( ret < 0 )
    {
	do_say( ch, "Damn!  Lost my prey!" );
	stop_hunting( ch );
	return;
    }
    else
    {
	if ( (pexit=get_exit(ch->in_room, ret)) == NULL )
	{
	    bug( "Hunt_victim: lost exit?", 0 );
	    return;
	}
	move_char( ch, pexit, FALSE, pexit->vdir );

	/* Crash bug fix by Shaddai */
	if ( char_died(ch) )
	   return;

	if ( !ch->hunting )
	{
	    if ( !ch->in_room )
	    {
		bug( "Hunt_victim: no ch->in_room!  Mob #%d, name: %s.  Placing mob in limbo.",
		    ch->pIndexData->vnum, ch->name );
		char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		return;
	    } 
	    do_say( ch, "Damn!  Lost my prey!" );
	    return;
	}
	if ( ch->in_room == ch->hunting->who->in_room )
	    found_prey(ch, ch->hunting->who);
	else
	{
	
	    /* perform a ranged attack if possible */
	    /* Changed who to name as scan_for_victim expects the name and
 	     * Not the char struct. --Shaddai
	     */
	}
	return;
    }
}
