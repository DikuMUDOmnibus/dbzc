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


void do_makeship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int vnum;
    OBJ_INDEX_DATA *obj;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeship <objvnum>\n\r", ch );
	return;
    }

    vnum = atoi( argument );
    
    if ( (obj = get_obj_index(vnum)) == NULL )
    {
	send_to_char( "Object not found.\n\r", ch );
	return;
    }


    if ( obj->pShip )
    {
	send_to_char( "This object is already a spaceship.\n\r", ch );
	return;
    }

    CREATE( ship, SHIP_DATA, 1 );

    LINK( ship, first_ship, last_ship, next, prev );
    ship->cockpit       = 0;
    ship->obj_vnum	= vnum;
    ship->home     	= 332;
    ship->speed    	= 10;
    ship->current_loc   = 332;
    ship->keyvnum	= ship->obj_vnum + 1;
    ship->lock		= HATCH_UNLOCKED;
    ship->entrance	= ship->obj_vnum;
    ship->first_stop	= NULL;
    ship->second_stop	= NULL;
    ship->launch1 	= 0;
    ship->launch2	= 0;
    obj->pShip		= ship;
    send_to_char( "Done.\n\r", ch );
    return;
}

void do_shipset( CHAR_DATA *ch, char *argument )
{
    DOCK_DATA *dock;
    SHIP_DATA *ship;
    OBJ_INDEX_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    sh_int vnum;
    int value;
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "\n\rUsage: shipset <ship vnum> <field> value\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "  name keyvnum speed cockpit home curr_loc destination\n\r", ch);
        send_to_char( "  ship_state hatch x1 x2 y1 y2 z1 z2 d1 d2\n\rFor Shuttles:\n\r", ch );
	send_to_char( "  first_stop second_stop launch1 launch2\n\r", ch );
	return;
    }

    vnum = atoi( arg1 );

    if ( (obj = get_obj_index(vnum)) == NULL )
    {
	send_to_char( "Object not found.\n\r", ch );
	return;
    }

    if ( !obj->pShip )
    {
	send_to_char( "This object ain't a spaceship.\n\r", ch );
	return;
    }
    ship = obj->pShip;
    value = atoi( arg3 );


    if ( !str_cmp( arg2, "keyvnum" ) )
    {
	if ( !is_number(arg3) )
      return;
	if ( value < 0 || value > 1000000000 )
	{
	    send_to_char( "Invalid vnum number!\n\r", ch );
	    return;
	}
	ship->keyvnum = value;
      ch_printf(ch, "Setting the Ship's Key Vnum to: %d", value);
	send_to_char( "\n\rDone.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "hatch" ) )
    {
        if ( !str_cmp( arg3, "locked" ) )
        {
          ch_printf(ch, "%s's hatch is now locked.", ship->name );
          ship->lock = HATCH_LOCKED;
        }
        else if ( !str_cmp( arg3, "unlocked" ) )
        {
          ch_printf(ch, "%s's hatch is now unlocked.", ship->name );
          ship->lock = HATCH_UNLOCKED;
        }
        else if ( !str_cmp( arg3, "shuttle" ) )
        {
          ch_printf(ch, "%s is now a shuttle.", ship->name );
          ship->lock = SHIP_SHUTTLE;
        }
        else if ( !str_cmp( arg3, "public" ) )
        {
          ch_printf(ch, "%s is now a public.", ship->name );
          ship->lock = SHIP_PUBLIC;
        }
        else
        {
          ch_printf(ch, "Is it locked, unlocked, public or is it a shuttle?.", ship->name );
        }
     }
     if ( !str_cmp( arg2, "name" ) )
     {
        if ( !arg3 || arg3[0] == '\0' )
        {
           send_to_char( "Names can not be set to an empty string.\n\r", ch );
           return;
        }
        ch_printf( ch, "Setting the Ship's Name to: %s\n\r", strupper(arg3) );
        ship->name = str_dup( strupper(arg3) );
    }
    if ( !str_cmp( arg2, "home" ) )
    {
        if ( !is_number(arg3) )
       return;
        if ( value < 0 || value > 1000000000 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->home = value;
        ch_printf(ch, "Setting the Ship's Home to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "entrance" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 1000000000 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->entrance = value;
      ch_printf(ch, "Setting the Ship's Entrance to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "destination" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 1000000000 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->destination = value;
      ch_printf(ch, "Setting the Ship's Destination to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "ship_state" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 2 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->ship_state = value;
      ch_printf(ch, "Setting the Ship's Ship_State to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "x1" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->x1 = value;
        ch_printf(ch, "Setting the Ship's Coord (x1) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "x2" ) )
    {
        if ( !is_number(arg3) )
        return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->x2 = value;
        ch_printf(ch, "Setting the Ship's Coord (x2) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "y1" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->y1 = value;
      ch_printf(ch, "Setting the Ship's Coord (y1) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "y2" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->y2 = value;
      ch_printf(ch, "Setting the Ship's Coord (y2) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "z1" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->z1 = value;
      ch_printf(ch, "Setting the Ship's Coord (z1) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "z2" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < -50 || value > 80 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->z2 = value;
      ch_printf(ch, "Setting the Ship's Coord (z2) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "d1" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 40 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->d1 = value;
      ch_printf(ch, "Setting the Ship's Coord (d1) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "d2" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 40 )
        {
            send_to_char( "Invalid vnum number!\n\r", ch );
            return;
        }
        ship->d2 = value;
      ch_printf(ch, "Setting the Ship's Coord (d2) to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "speed" ) )
    {
	if ( !is_number(arg3) )
      return;
	if ( value < 0 || value > 80 )
	{
	    send_to_char( "Exceeding Speed Limits!\n\r", ch );
	    return;
	}
	ship->speed = value;
      ch_printf(ch, "Setting the Ship's Speed to: %d", value);
	send_to_char( "\n\rDone.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "curr_loc" ) )
    {
        if ( !is_number(arg3) )
      return;
        if ( value < 0 || value > 1000000000 )
        {
            send_to_char( "Exceeding Vnum Limits!\n\r", ch );
            return;
        }
        ship->current_loc = value;
      ch_printf(ch, "Setting the Ship's Curr_Loc to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }

/*    if ( !str_cmp( arg2, "idNumber" ) )
    {
      STRFREE( ship->idNumber );
      ship->idNumber = arg3;
      ch_printf(ch, "Setting the Ship's idNumber to: %d", str_dup(arg3));
	send_to_char( "\n\rDone.\n\r", ch );
	return;
    }*/
    if ( !str_cmp( arg2, "cockpit" ) )
    {
	if ( !is_number(arg3) )
      return;
	if ( value < 0 || value > 100000000 )
	{
	    send_to_char( "Invalid Vnum Number\n\r", ch );
	    return;
	}
	ship->cockpit = value;
      ch_printf(ch, "Setting the Ship's Cockpit Vnum to: %d", value);
	send_to_char( "\n\rDone.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "launch1" ) )
    {
        if ( !is_number(arg3) )
        return;
        if ( value < 0 || value > 24 )
        {
            send_to_char( "Invalid Number\n\r", ch );
            return;
        }
        ship->launch1 = value;
        ch_printf(ch, "Setting the Ship's Launch1 to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "launch2" ) )
    {
        if ( !is_number(arg3) )
        return;
        if ( value < 0 || value > 24 )
        {
            send_to_char( "Invalid Number\n\r", ch );
            return;
        }
        ship->launch2 = value;
        ch_printf(ch, "Setting the Ship's Launch2 to: %d", value);
        send_to_char( "\n\rDone.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "first_stop" ) )
    {
       if ( ( dock = get_dock( arg3 ) ) == NULL )
       {
          send_to_char("That dock doesn't exist.\n\r", ch );
          return;
       }
       ship->first_stop = dock;
       ch_printf(ch,"\n\rSetting the Ship's First Stop to: %s", dock->coordname);
       send_to_char( "\n\rDone.\n\r", ch );
       return;
    }
    if ( !str_cmp( arg2, "second_stop" ) )
    {
       if ( ( dock = get_dock( arg3 ) ) == NULL )
       {
          send_to_char("That dock doesn't exist.\n\r", ch );
          return;
       }
       ship->second_stop = dock;
       ch_printf(ch,"\n\rSetting the Ship's Second Stop to: %s", dock->coordname);
       send_to_char( "\n\rDone.\n\r", ch );
       return;
    }

    return;
}

void do_shipstat( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    OBJ_INDEX_DATA *obj;
    int vnum;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: shipstat <Obj vnum>\n\r", ch );
	return;
    }

    vnum = atoi( argument );
    
    if ( (obj = get_obj_index(vnum)) == NULL )
    {
	send_to_char( "Object not found.\n\r", ch );
	return;
    }

    if ( !obj->pShip )
    {
	send_to_char( "This object isnt a ship.\n\r", ch );
	return;
    }
    ship = obj->pShip;
    send_to_char( "\n\r&z+---------------------------------------------------+", ch);
    send_to_char( "\n\r&z|                  &RShip Stats &z                      |", ch);
    send_to_char( "\n\r&z+---------------------------------------------------+\n\r", ch);
    
    ch_printf( ch, "\n\r&YObj:      &W%d\n\r&YDesc:     &W%s\n\r", ship->obj_vnum, obj->short_descr );
    ch_printf( ch, "&YShipName: &W%s\n\r", ship->name);
    ch_printf( ch, "&YKeyvnum:  &W%d\n\r", ship->keyvnum);
    ch_printf( ch, "&YSpeed:    &W%d\n\r", ship->speed);
    ch_printf( ch, "&YCurr_Loc: &W%d\n\r", ship->current_loc);
    ch_printf( ch, "&YCockpit:  &W%d\n\r", ship->cockpit);
    ch_printf( ch, "&YHome:     &W%d\n\r", ship->home);
    ch_printf( ch, "&YEntrance: &W%d\n\r", ship->entrance);
    ch_printf( ch, "&YHeading:  &W%d\n\r", ship->destination);
    ch_printf( ch, "&YState:    &W%d\n\r", ship->ship_state);
    ch_printf( ch, "&YHatch:    &W%s\n\r", ship->lock == HATCH_LOCKED ? "Locked" : ship->lock == SHIP_PUBLIC ? "Public" : ship->lock == SHIP_SHUTTLE ? "Shuttle" : "Unlocked" );
    ch_printf( ch, "&YCurrent:  &W%d&z.&W%d&z.&W%d&z.&W%d\n\r", ship->x1, ship->y1, ship->z1, ship->d1);
    ch_printf( ch, "&YTarget:   &W%d&z.&W%d&z.&W%d&z.&W%d\n\r", ship->x2, ship->y2, ship->z2, ship->d2);
    if ( ship->lock == SHIP_SHUTTLE )
    {
      ch_printf( ch, "\n\r&RShuttle Data&W:");
      ch_printf( ch, "\n\r&YFirst_Stop :  &W%s\n\r", ship->first_stop ? ship->first_stop->coordname : "None" );
      ch_printf( ch, "&YSecond_Stop:  &W%s\n\r", ship->second_stop ? ship->second_stop->coordname : "None" );
      ch_printf( ch, "&YLaunch Times: &W%d&z/&W%d\n\r", ship->launch1, ship->launch2);
    }
   send_to_char( "\n\r&z+---------------------------------------------------+", ch);
    
    return;
}

void do_ships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;

    if ( !first_ship )
    {
	send_to_char( "There are no ships.\n\r", ch );
	return;
    }

    set_char_color( AT_NOTE, ch );
    for ( ship = first_ship; ship; ship = ship->next )
    {
	ch_printf( ch, "\n\rVnum: %5d Home: %5d Speed: %2d Cockpit: %5d Entrance: %5d\n\r",
		ship->obj_vnum, ship->home, ship->speed,
		ship->cockpit, ship->entrance );
    }
    return;
}

void do_board( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *inroom;
   CHAR_DATA *nextinroom;
   ROOM_INDEX_DATA *fromroom;
   ROOM_INDEX_DATA *toroom;
   ROOM_INDEX_DATA *dock;
   OBJ_DATA *obj;
   SHIP_DATA *ship;
   sh_int chance;
//   OBJ_DATA *obj2;
//   OBJ_DATA *obj_next;  
//   bool dragonball=FALSE;
   dock = ch->in_room;

   chance = number_range( 0, 2 );
   if ( !argument || argument[0] == '\0')
   {
       send_to_char( "Board what?\n\r", ch );
       return;
   }
   
   if ( ( obj = get_obj_here( ch, argument ) ) == NULL )
   {
      act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
      return;
   }
    fromroom = ch->in_room;
    if (  (ship = ship_from_entrance(obj->value[0]))  == NULL )
    {
       send_to_char("&RThat isnt a spaceship.\n\r",ch);
       return;
    }
    if ( ch->exp < 5000000 && ship->lock != SHIP_SHUTTLE)
    {
       send_to_char( "You are not strong enough to use personal ships.\n\r", ch);
       return;
    }

    if ( ship->lock == HATCH_LOCKED )
    {
       send_to_char( "&RThe hatch is locked.\n\r", ch);
       return;
    }
   
    if (get_timer( ch, TIMER_PROPAGANDA) > 0)
    {
       send_to_char( "&RYou can't leave while spreading propaganda.\n\r", ch);
       return;
    }
/*    for ( obj2 = ch->first_carrying; obj2; obj2 = obj_next )
    {
       obj_next = obj2->next_content;

      if (obj2->item_type == ITEM_DRAGONBALL)
        dragonball = TRUE;
    }
    if (dragonball)
    {
        send_to_char("&RYou can't enter having a dragonball at hand.\n\r", ch);
        return;
    }*/
    if ( get_db_carried( ch, FALSE ) > 0 )
    {
        send_to_char("&RYou can't enter having a dragonball at hand.\n\r", ch);
        return;
    }

        if ( ( toroom = get_room_index( obj->value[0] ) ) != NULL )
   	{
          if ( (chance == 1 || chance == 2) && get_timer(ch, TIMER_RECENTFIGHT) > 0)
          {
            send_to_char( "&RIn your haste, you are unable to type in the correct code.\n\r", ch);
            WAIT_STATE(ch, 3);
            return;
         }
	    /*
	     * Check if player can fit in the room
	     */
	    if ( toroom->tunnel > 0 )
	    {
	        CHAR_DATA *ctmp;
	        int count = 0;
	
	        for ( ctmp = toroom->first_person; ctmp; ctmp = ctmp->next_in_room )
	          if ( ++count >= toroom->tunnel )
	          {
	                  send_to_char( "There is no room for you in there.\n\r", ch );
	                return;
	          }
	    }
            act( AT_PLAIN, "&W$n walks over the spaceship and enters it.", ch, NULL, NULL , TO_ROOM );
            act( AT_PLAIN, "&WYou enter the spaceship.", ch, NULL, NULL , TO_CHAR );
            char_from_room( ch );
            char_to_room( ch , toroom );
            act( AT_PLAIN, "&W$n enters the ship.", ch, NULL, NULL, TO_ROOM );
            do_look( ch , "auto" );

          for ( inroom = dock->first_person; inroom; inroom = nextinroom )
          {
               nextinroom = inroom->next_in_room;
               if (inroom->position == POS_STANDING && inroom->leader == ch && get_timer(ch, TIMER_PROPAGANDA) <= 0)
               {
                 if ( toroom->tunnel > 0 )
                 {
                    CHAR_DATA *ctmp;
                    int count = 0;

                    for ( ctmp = toroom->first_person; ctmp; ctmp = ctmp->next_in_room )
                       count++;

                      if ( count >= toroom->tunnel )
		      continue;
                 }
    		 if ( get_db_carried( inroom, FALSE ) > 0 )
		 continue;
                  act( AT_PLAIN, "&W$n walks over the spaceship and enters it.", inroom, NULL, NULL , TO_ROOM );
                  act( AT_PLAIN, "&WYou enter the spaceship.", inroom, NULL, NULL , TO_CHAR );
                  char_from_room( inroom );
                  char_to_room( inroom, toroom );
                  do_look( inroom, "auto" );
                  act( AT_PLAIN, "&W$n enters the ship.", inroom, NULL, NULL, TO_ROOM );
               }
          }
        }                                                                  
        else
          send_to_char("That ship has no entrance!\n\r", ch);
}

SHIP_DATA *ship_from_entrance( int vnum )
{
    SHIP_DATA *ship;

    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->entrance )
         return ship;
    return NULL;
}

OBJ_DATA *get_obj_ship( int vnum )
{
    OBJ_DATA *obj;

    for ( obj = first_object; obj; obj = obj->next )
       if ( vnum == obj->value[0] )
         return obj;
    return NULL;
}

void do_calculate( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    DOCK_DATA *dock;
    OBJ_DATA *obj;
    char arg[MIL];
    argument = one_argument( argument, arg);

      if ( arg[0] == '\0')
      {
       send_to_char("\n\rSyntax: Calculate <Name>\n\r", ch);
       return;
      }
      if (!IN_CONTROL(ch))
      {
       send_to_char("You aren't in a control room", ch);
       return;
      }
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  == NULL )
      {
           send_to_char("&RYou must be in the control room of a ship to do that!\n\r",ch);
           return;
      }
      if (  (obj = has_key( ch, ship->keyvnum)) == NULL && ch->level < 65 && ship->lock != SHIP_PUBLIC) 
      {
           send_to_char("&wYou need the controlkey for that ship to do that!\n\r",ch);
           return;
      }
      for ( dock = first_dock; dock; dock = dock->next )
      {
         if ( !dock->coordname || dock->coordname[0] == '\0')
         continue;
         if ( dock->room <= 0 )
         {
            send_to_char( "You can't reach this coordiante.\n\r", ch);
	    return;
         }
         if ( !str_cmp( dock->coordname, arg ) && dock->onlyshuttle == FALSE)
         {
             ch_printf(ch, "\n\r&z[&YBoard Computer&z] &WCoordinates set to %s\n\r", dock->name );
             ship->x2 = dock->x;
             ship->y2 = dock->y;
             ship->z2 = dock->z;
             ship->d2 = dock->d;
             ship->destination = dock->room;
             act(AT_GREY, "&w$n enters new target coordinates.", ch, NULL, NULL, TO_CANSEE);
             return;
         }
         continue; 
     }
      send_to_char("\n\rInvalid Coordinate name.\n\r", ch);
      return;
}

void do_start( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;

      if (!IN_CONTROL(ch))
      {
       send_to_char("You aren't in a control room", ch);
       return;
      }
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  == NULL )
      {
           send_to_char("&RYou must be in the control room of a ship to do that!\n\r",ch);
           return;
      }
      if (  (obj = has_key( ch, ship->keyvnum)) == NULL && ch->level < 65 && ship->lock != SHIP_PUBLIC)
      {
           send_to_char("&wYou need the controlkey for that ship to do that!\n\r",ch);
           return;
      }
      if (ship->ship_state == 2 || ship->ship_state == 1)
      {
           send_to_char("&RIts already powered up!\n\r",ch);
           return;
      }
      ship->ship_state = 1;
      ch_printf(ch, "\n\rYou use your controlcard to active the ship's powersupply...\n\r\n\r");
      ch_printf(ch, "The monitors and controls switch glow up...the system is online.\n\r");
      act(AT_WHITE, "$n use $s controlcard to active the ship's powersupply...\n\r", ch, NULL, NULL, TO_CANSEE);
      act(AT_WHITE, "The monitors and controls switch glow up...the system is online.", ch, NULL, NULL, TO_CANSEE);
}

void do_launch( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    char buf[MIL];
      if (!IN_CONTROL(ch))
      {
       send_to_char("You aren't in a control room", ch);
       return;
      }
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  == NULL )
      {
           send_to_char("&RYou must be in the control room of a ship to do that!\n\r",ch);
           return;
      }
      if ( ship->x1 == ship->x2 && ship->y1 == ship->y2 && ship->z1 == ship->z2 && ship->d1 == ship->d2)
      {
           send_to_char("&RInput some coordinates!\n\r",ch);
           return;
      }
      if (ship->ship_state == 2 )
      {
           send_to_char("&RIts already in orbit!\n\r",ch);
           return;
      }
      if (ship->ship_state == 0 )
      {
           send_to_char("&RThe system is offline.\n\r",ch);
           return;
      }
      if ( ch->gold < 50000 && ship->lock == SHIP_PUBLIC)
      {
           send_to_char("&RYou don't have enough gold to pay the rent.\n\r",ch);
           return;
      }

      location = get_room_index(ship->current_loc);
      ship->ship_state = 2;
      ch_printf(ch, "\n\rYou press the launch button to start your journey...\n\r\n\r");
      ch_printf(ch, "You feel the spaceship trembling and shaking as the engines power up...\n\r\n\r");
      ch_printf(ch, "You get firmly pressed into your chair as the spaceship blasts off into orbit!\n\r");
      act(AT_WHITE, "$n presses the launch button to start your journey...\n\r", ch, NULL, NULL, TO_CANSEE);
      act(AT_WHITE, "You feel the spaceship trembling and shaking as the engines power up...\n\r", ch, NULL, NULL, TO_CANSEE);
      act(AT_WHITE, "You get firmly pressed into your chair as the spaceship blasts off into orbit!\n\r", ch, NULL, NULL, TO_CANSEE);
  
      sprintf( buf,"&wYou hear space-engines hauling as they are being throttled up...\n\r\n\rSmoke and bright light surrounds %s as it rises\n\roff the ground slowly, turning around, preparing for lift-off...\n\r\n\rWith a loud explosion of power the spaceship blasts off into orbit!", ship->name);
      echo_to_room( AT_WHITE, location, buf);
      if ( ship->lock == SHIP_PUBLIC )
      {
         send_to_char( "&GYou pay 50,000 to rent this ship.\n\r", ch);
         ch->gold -= 50000;
      }
      ship->current_loc = 0;
      if ( ( obj = get_obj_ship( ship->obj_vnum ) ) != NULL )
      {
           extract_obj( obj );
           return;
      }
      
}

void do_land( SHIP_DATA *ship )
{
 OBJ_DATA *obj;
 OBJ_INDEX_DATA *pObjIndex;
 ROOM_INDEX_DATA *room;
 int  parsec,  dist1, dist2, dist3, dist4, speed, remain;
 ROOM_INDEX_DATA *location;
 int vnum, level;
 char buf[MIL];

  if (!ship)
  return;
  if (ship->ship_state == 0)
  return;
            location = get_room_index(ship->entrance);
            speed = ship->speed;

             
            dist1 = ship->x1 - ship->x2;
            dist2 = ship->y1 - ship->y2;
            dist3 = ship->z1 - ship->z2;
            dist4 = ship->d1 - ship->d2;
            parsec = convert_pos(dist1) + convert_pos(dist2)+ convert_pos(dist3) + convert_pos(dist4) + 20;

            vnum = ship->obj_vnum;
            parsec -= ship->speed;

            if (parsec <= 10 && parsec >= 1)
               echo_to_room(AT_YELLOW, location, "The spaceship rocks slightly as it speeds through space...");
            else if (parsec > 10)
               echo_to_room(AT_YELLOW, location, "The ship dodges past asteroids as it speeds to it's destination...");

            if ( parsec <= 0 )
            {
             room = get_room_index( ship->destination );
             location = get_room_index( ship->entrance );
             pObjIndex = get_obj_index( vnum );
             level = 0;
             obj = create_object( pObjIndex, level );
             ship->ship_state = 0;
             ship->current_loc = ship->destination;
             if ( ( room = get_room_index( ship->destination ) ) == NULL )
             {
                sprintf( buf, "Something went wrong with %s\n\r", ship->name);
                bug( buf );
                return;
             }
             obj = obj_to_room( obj, room, NULL );
             sprintf(buf, "&wYou hear a high pitched sound as a spaceship enters the atmosphere...\n\r\n\rWith a lot of noise %s approaches the platform to land on...\n\r\n\rThings calm down again as the spaceship turn off its engines...", ship->name);
             echo_to_room(AT_WHITE, room, buf );
             echo_to_room(AT_YELLOW, location, "&YThe spaceship reduces speed as it prepares for orbit...\n\rYou have reached your destination!\n\r\n\r&wYou enter the atmosphere and prepare for final descent...\n\r\n\rYou feel the  spaceship trembling as it touches the landing platform...\n\r\n\rThe spaceship engines shut down as you stand still at your destination.");
             ship->x1 = ship->x2;
             ship->y1 = ship->y2;
             ship->z1 = ship->z2;
             ship->d1 = ship->d2;
           }
           else
           {
            speed /= 3;
            /* x */
            if (ship->x1 != ship->x2 && (speed > 0 ))
            {
              if (ship->x1 > ship->x2 )
              {
                  remain = ship->x1 - ship->x2;
                  ship->x1 -= speed;
                  if ( ship->x1 < ship->x2 )
	          ship->x1 = ship->x2;
              }
              else
              {
                  remain = ship->x2 - ship->x1;
                  ship->x1 += speed;
                  if ( ship->x1 > ship->x2 )
                  ship->x1 = ship->x2;
              }
            }
            /* y */
            if (ship->y1 != ship->y2 && (speed > 0))
            {
              if (ship->y1 > ship->y2 )
              {
                  remain = ship->y1 - ship->y2;
                  ship->y1 -= speed;
                  if ( ship->y1 < ship->y2 )
                  ship->y1 = ship->y2;
              }
              else
              {
                  remain = ship->y2 - ship->y1;
                  ship->y1 += speed;
                  if ( ship->y1 > ship->y2 )
                  ship->y1 = ship->y2;
              }
            }
            /* z */
            if (ship->z1 != ship->z2 && (speed > 0))
            {
              if (ship->z1 > ship->z2 )
              {
                  remain = ship->z1 - ship->z2;
                  ship->z1 -= speed;
                  if ( ship->z1 < ship->z2 )
                  ship->z1 = ship->z2;
              }
              else
              {
                  remain = ship->z2 - ship->z1;
                  ship->z1 += speed;
                  if ( ship->z1 > ship->z2 )
                  ship->z1 = ship->z2;
              }
            }
            /* d */
            if (ship->d1 != ship->d2 && (speed > 0))
            {
              if (ship->d1 > ship->d2 )
              {
                  remain = ship->d1 - ship->d2;
                  ship->d1 -= speed;
                  if ( ship->d1 < ship->d2 )
                  ship->d1 = ship->d2;
              }
              else
              {
                  remain = ship->d2 - ship->d1;
                  ship->d1 += speed;
                  if ( ship->d1 > ship->d2 )
                  ship->d1 = ship->d2;
              }
            }

     }
  return;
}


/*void do_land( SHIP_DATA *ship )
{
 OBJ_DATA *obj;
 OBJ_INDEX_DATA *pObjIndex;
 ROOM_INDEX_DATA *room;
 int target, current, parsecs, parsecs2;
 ROOM_INDEX_DATA *location;
 int vnum, level;
 char buf[MIL];

            location = get_room_index(ship->entrance);
            current = ship->x1; 
            current += ship->y1;
            current += ship->z1;
            current += ship->d1;
            target = ship->x2;
	    target += ship->y2;
            target += ship->z2; 
            target += ship->d2;
            vnum = ship->obj_vnum;
            if ( current >= target)
            {parsecs = current - target;}
            else
            {parsecs = target - current;}

            parsecs2 = parsecs;
            parsecs2 -= ship->speed;
            parsecs = parsecs2;

            if (parsecs2 <= 10 && parsecs2 >= 1)
               echo_to_room(AT_YELLOW, location, "The ship is reaching its destination...");
            else if (parsecs > 10)
               echo_to_room(AT_YELLOW, location, "The ship is in space...");

            if ( parsecs2 <= ship->speed )
            {
            room = get_room_index( ship->destination );
            location = get_room_index( ship->entrance );
            pObjIndex = get_obj_index( vnum );
            level = 0;
            obj = create_object( pObjIndex, level );
 	    ship->ship_state = 0;
            ship->current_loc = ship->destination;
            ship->x1 = ship->x2; ship->y1 = ship->y2; ship->z1 = ship->z2; ship->d1 = ship->d2;
            obj = obj_to_room( obj, room );
             sprintf(buf, "&wYou hear a high pitched sound as a spaceship enters the atmosphere...\n\r\n\rWith a lot of  
 noise %s approaches the platform to land on...\n\r\n\rThings calm down again as the spaceship turn off its 
engines...\n\r", ship->name);
            echo_to_room(AT_WHITE, room, buf );
             echo_to_room(AT_YELLOW, location, "&YThe spaceship reduces speed as it prepares for orbit...\n\rYou have  
 reached your destination!\n\r\n\r&wYou enter the atmosphere and prepare for final descent...\n\r\n\rYou feel the  
 spaceship trembling as it touches the landing platform...\n\r\n\rThe spaceship engines shut down as you stand still at 
your destination."); 
            ship->x1 = ship->x2;
            ship->y1 = ship->y2;
            ship->z1 = ship->z2;
            ship->d1 = ship->d2;
           }
            else
            {
             parsecs /= 4;
             if ( current > target)
             {ship->x1 -= parsecs; ship->y1 -= parsecs; ship->z1 -= parsecs; ship->d1 -= parsecs;}
             else
             {ship->x1 += parsecs; ship->y1 += parsecs; ship->z1 += parsecs; ship->d1 += parsecs;}

            }
}*/

void do_leaveship( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *fromroom;
    ROOM_INDEX_DATA *toroom;
    SHIP_DATA *ship;
    
    fromroom = ch->in_room;

    if  ( (ship = ship_from_entrance(fromroom->vnum)) == NULL )
    {
        send_to_char( "I see no exit here.\n\r" , ch );
        return;
    }   
    if ( ship->current_loc <= 0)
    {
        send_to_char( "Your in the middle of space.\n\r" , ch );
        return;
    }
   
    if ( ( toroom = get_room_index( ship->current_loc ) ) != NULL )
    {
            act( AT_PLAIN, "&W$n leaves the spaceship.", ch,
		NULL, argument , TO_ROOM );
	    act( AT_PLAIN, "&WYou leave the spaceship.", ch,
		NULL, argument , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "&W$n walks out of $s spaceship and closes the hatch behind $m.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );
     }       
     else
        send_to_char ( "The exit doesn't seem to be working properly.\n\r", ch );  
}



void do_lockship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    OBJ_DATA *key;
    bool found;
    char arg[MIL], buf[MIL];
    argument = one_argument( argument, arg);

      if ( arg[0] == '\0')
      {
       send_to_char("\n\rLock what ship?\n\r", ch);
       return;
      }
      found = FALSE;
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  != NULL )
      {
        found = TRUE;
      }
      else if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
      {
       found = TRUE;
       if (  (ship = ship_from_entrance(obj->value[0]))  == NULL )
       {
         send_to_char("&RThat isnt a spaceship.\n\r",ch);
         return;
       }
      }
      if ( !found )
      {
        send_to_char( "You can't find it.\n\r", ch );
        return;
      }
      if ( ship->lock == SHIP_SHUTTLE || ship->lock == SHIP_PUBLIC)
      {
           send_to_char("&wThat ship can't be locked\n\r",ch);
           return;
      }

      if (  (key = has_key( ch, ship->keyvnum) ) == NULL )
      {
           send_to_char("&wYou need the key for that ship to do that!\n\r",ch);
           return;
      }
      if ( ship->lock == HATCH_LOCKED )
      {
           send_to_char("\n\r&RIts already locked.\n\r",ch);
           return;
      }
      ship->lock = HATCH_LOCKED;
      ch_printf(ch, "\n\r&RYou lock your spaceship.\n\r");
      act(AT_WHITE, "$n locks $s spaceship.", ch, NULL, NULL, TO_CANSEE);
      if ( !IS_NPC(ch))
      {
        sprintf( buf, "%s (%s) has locked spaceship: %s",
        ch->name, ch->pcdata->clan ? ch->pcdata->clan->name: "Null", ship->name );
        append_to_file( LOCK_FILE, buf );
      }
      return;
}

void do_unlockship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    OBJ_DATA *key;
    bool found;
    char arg[MIL], buf[MIL];
    argument = one_argument( argument, arg);

      if ( arg[0] == '\0')
      {
       send_to_char("\n\rUnlock what ship?\n\r", ch);
       return;
      }
      found = FALSE;
      if (  (ship = ship_from_entrance(ch->in_room->vnum))  != NULL )
      {
        found = TRUE;
      }
      else if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
      {
       found = TRUE;
       if (  (ship = ship_from_entrance(obj->value[0]))  == NULL )
       {
         send_to_char("&RThat isnt a spaceship.\n\r",ch);
         return;
       }
      }
      if ( !found )
      {
        send_to_char( "You can't find it.\n\r", ch );
        return;
      }
      if ( ship->lock == SHIP_SHUTTLE || ship->lock == SHIP_PUBLIC  )
      {
           send_to_char("&wThat ship can't be locked\n\r",ch);
           return;
      }
      if (  (key = has_key( ch, ship->keyvnum)) == NULL )
      {
           send_to_char("&wYou need the key for that ship to do that!\n\r",ch);
           return;
      }
      if ( ship->lock == HATCH_UNLOCKED )
      {
           send_to_char("\n\r&RIts already unlocked.\n\r",ch);
           return;
      }
      ship->lock = HATCH_UNLOCKED;
      ch_printf(ch, "\n\r&RYou unlock your spaceship.\n\r");
      act(AT_WHITE, "$n unlocks $s spaceship.", ch, NULL, NULL, TO_CANSEE);
      if ( !IS_NPC(ch))
      {
        sprintf( buf, "%s (%s) has unlocked spaceship: %s",
        ch->name, ch->pcdata->clan ? ch->pcdata->clan->name: "Null", ship->name );
        append_to_file( LOCK_FILE, buf );
      }
      return;
}
