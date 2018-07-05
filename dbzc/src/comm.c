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
 *			 Low-level communication module			    *
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
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <dlfcn.h>
#ifdef CYGWIN
#include <crypt.h>
#endif
#include "mud.h"
//#include <zlib.h>
#include <arpa/telnet.h>

char *  num_punct        args( (  int foo ) );
void shutdown_mud( char *reason );
static  OBJ_DATA *      rgObjNest       [MAX_NEST];
void	update_skill( CHAR_DATA *ch );
/*
 * Socket and TCP/IP stuff.
 */
#ifdef WIN32
  #include <io.h>
  #undef EINTR
  #undef EMFILE
  #define EINTR WSAEINTR
  #define EMFILE WSAEMFILE
  #define EWOULDBLOCK WSAEWOULDBLOCK
  #define MAXHOSTNAMELEN 32

  #define  TELOPT_ECHO        '\x01'
  #define  GA                 '\xF9'
  #define  SB                 '\xFA'
  #define  WILL               '\xFB'
  #define  WONT               '\xFC'
  #define  DO                 '\xFD'
  #define  DONT               '\xFE'
  #define  IAC                '\xFF'
  void bailout(void);
  void shutdown_checkpoint (void);
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/in_systm.h>
  #include <netinet/ip.h>
  #include <arpa/inet.h>
  #include <arpa/telnet.h>
  #include <netdb.h>
  #define closesocket close
#endif

#ifdef sun
int gethostname ( char *name, int namelen );
#endif

/* Terminal detection stuff start */
#define  IS                 '\x00'
#define  TERMINAL_TYPE      '\x18'
#define  SEND	          '\x01'

const char term_call_back_str [] = { IAC, SB, TERMINAL_TYPE, IS };
const char req_termtype_str   [] = { IAC, SB, TERMINAL_TYPE, SEND, IAC, SE, '\0' };
const char do_term_type     [] = { IAC, DO, TERMINAL_TYPE, '\0' };
/* Terminal detection stuff end */


const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
/*
void	auth_maxdesc	args( ( int *md, fd_set *ins, fd_set *outs,
				fd_set *excs ) );
void	auth_check	args( ( fd_set *ins, fd_set *outs, fd_set *excs ) );
void	set_auth	args( ( DESCRIPTOR_DATA *d ) );
void	kill_auth	args( ( DESCRIPTOR_DATA *d ) );
*/

void    save_sysdata args( ( SYSTEM_DATA sys ) );
void    send_to_desc_color args( ( const char *txt, DESCRIPTOR_DATA *d ) );

/*
 * Global variables.
 */
bool             emergancy_copy;
IMMORTAL_HOST * immortal_host_start;    /* Start of Immortal legal domains */
IMMORTAL_HOST * immortal_host_end;    /* End of Immortal legal domains */
DESCRIPTOR_DATA *   first_descriptor;	/* First descriptor		*/
DESCRIPTOR_DATA *   last_descriptor;	/* Last descriptor		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
int		    num_descriptors;
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    mud_down;		/* Shutdown			*/
bool		    service_shut_down;  /* Shutdown by operator closing down service */
bool		    wizlock;		/* Game is wizlocked		*/
time_t              boot_time;
HOUR_MIN_SEC  	    set_boot_time_struct;
HOUR_MIN_SEC *      set_boot_time;
struct tm *         new_boot_time;
struct tm           new_boot_struct;
char		    str_boot_time[MAX_INPUT_LENGTH];
char		    lastplayercmd[MAX_INPUT_LENGTH*2];
time_t		    current_time;	/* Time of this pulse		*/
int		    control;		/* Controlling descriptor	*/
int		    control2;		/* Controlling descriptor #2	*/
int		    conclient;		/* MUDClient controlling desc	*/
int		    conjava;		/* JavaMUD controlling desc	*/
int		    newdesc;		/* New descriptor		*/
fd_set		    in_set;		/* Set of desc's for reading	*/
fd_set		    out_set;		/* Set of desc's for writing	*/
fd_set		    exc_set;		/* Set of desc's with errors	*/
int 		    maxdesc;
char *		    alarm_section = "(unknown)";
bool                emergancy_copy;
//static void         SegVio();
/*
static void SegVio()
{
  char buf[MAX_STRING_LENGTH];
  char bufB[MAX_STRING_LENGTH];
  char *strtime;

  strtime                    = ctime( &current_time );
  strtime[strlen(strtime)-1] = '\0';

  log_string( "--- SEGMENTATION VIOLATION ---" );

  sprintf( buf, "%slastcmd.log", LOG_DIR );

  if ( file_size(buf) < 5000000 )
  {
       sprintf( bufB, "%s :: %s", strtime, lastplayercmd );
       append_to_file( buf, bufB );
  }

  if ( !sysdata.ECOPYOVER )
  {
      log_string( "Emergancy copyover not enabled. Shutting down." );
  }
  else if ( emergancy_copy == TRUE )
  {
      emergancy_copyover( );
      log_string( "Emergancy copyover not ready. Shutting down." );
  }
  else
  {
      log_string( "Emergancy copyover not ready. Shutting down." );
  }

  exit(0);
}

*/

/*
 * OS-dependent local functions.
 */
void	game_loop		args( ( ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int new_desc ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );

/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name, bool newchar ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name, bool kick ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	flush_buffer		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void	free_desc		args( ( DESCRIPTOR_DATA *d ) );
void	display_prompt		args( ( DESCRIPTOR_DATA *d ) );
void	set_pager_input		args( ( DESCRIPTOR_DATA *d,
					char *argument ) );
bool	pager_output		args( ( DESCRIPTOR_DATA *d ) );

void	mail_count		args( ( CHAR_DATA *ch ) );

int port;

#ifdef WIN32
  int mainthread( int argc, char **argv )
#else
  int main( int argc, char **argv )
#endif
{
    struct timeval now_time;
    char hostn[128];
    bool fCopyOver = !TRUE;
    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    DONT_UPPER			= FALSE;
    num_descriptors		= 0;
    emergancy_copy              = TRUE;
    first_descriptor		= NULL;
    last_descriptor		= NULL;
    sysdata.NO_NAME_RESOLVING	= TRUE;
    sysdata.WAIT_FOR_AUTH	= TRUE;

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
/*  gettimeofday( &boot_time, NULL);   okay, so it's kludgy, sue me :) */
    boot_time = time(0);         /*  <-- I think this is what you wanted */
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Init boot time.
     */
    set_boot_time = &set_boot_time_struct;
    set_boot_time->manual = 0;
    
    new_boot_time = update_time(localtime(&current_time));
    /* Copies *new_boot_time to new_boot_struct, and then points
       new_boot_time to new_boot_struct again. -- Alty */
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    new_boot_time->tm_mday += 1;
    if(new_boot_time->tm_hour > 12)
    	new_boot_time->tm_mday += 1;
    new_boot_time->tm_sec = 0;
    new_boot_time->tm_min = 0;
    new_boot_time->tm_hour = 6;

    /* Update new_boot_time (due to day increment) */
    new_boot_time = update_time(new_boot_time);
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    /* Bug fix submitted by Gabe Yoder */
    new_boot_time_t = mktime(new_boot_time);
    reboot_check(mktime(new_boot_time));
    /* Set reboot time string for do_time */
    get_reboot_string();
    init_pfile_scan_time(); /* Pfile autocleanup initializer - Samson 5-8-99 */
    /*
     * Reserve two channels for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }
    if ( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
/*    port = 1674;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }*/
   port = 8112;
   if ( argc > 1 )
   {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}

      if (argv[2] && argv[2][0])
      {
         fCopyOver = TRUE;
         control = atoi( argv[3] );
/*         control2 = atoi( argv[4] );
         conclient = atoi( argv[5] );
         conjava = atoi( argv[6] );*/
      }
      else
         fCopyOver = FALSE;
   }

    /*
     * Run the game.
     */
#ifdef WIN32
    {
	/* Initialise Windows sockets library */

	unsigned short wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsadata;
	int err;

	/* Need to include library: wsock32.lib for Windows Sockets */
	err = WSAStartup(wVersionRequested, &wsadata);
	if (err)
	{
	    fprintf(stderr, "Error %i on WSAStartup\n", err);
	    exit(1);
	}

	/* standard termination signals */
	signal(SIGINT, (void *) bailout);
	signal(SIGTERM, (void *) bailout);
  }
#endif /* WIN32 */

/*    log_string("Booting Database");
    boot_db( );
    log_string("Initializing socket");
    control  = init_socket( port   );
    control2 = init_socket( port+1 );
    conclient= init_socket( port+10);
    conjava  = init_socket( port+20);*/
    log_string("Booting Database");
    boot_db( fCopyOver );
    log_string("Initializing socket"); 
    if (!fCopyOver) /* We have already the port if copyover'ed */
    {
	control  = init_socket( port   );
/*	control2 = init_socket( port+1 );
	conclient= init_socket( port+10);
	conjava  = init_socket( port+20);*/
    }
    
    /* I don't know how well this will work on an unnamed machine as I don't
       have one handy, and the man pages are ever-so-helpful.. -- Alty */
    if (gethostname(hostn, sizeof(hostn)) < 0)
    {
      perror("main: gethostname");
      strcpy(hostn, "unresolved");
    }
    sprintf( log_buf, "%s ready at address %s on port %d.",	sysdata.mud_name, hostn, port );
    log_string( log_buf );
      if( fCopyOver )
      {
         log_string( "Initiating copyover recovery." );
         copyover_recover();
      }
  
  game_loop( );    

    closesocket( control  );

#ifdef WIN32
    if (service_shut_down)
    {
	CHAR_DATA *vch;

	/* Save all characters before booting. */
	for ( vch = first_char; vch; vch = vch->next )
	    if ( !IS_NPC( vch ) )
	    {
		shutdown_checkpoint ();
		save_char_obj( vch );
	    }
	}
    /* Shut down Windows sockets */
    
    WSACleanup();                 /* clean up */
    kill_timer();                 /* stop timer thread */
#endif


    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}


int init_socket( int port )
{
    char hostname[64];
    struct sockaddr_in	 sa;
    int x = 1;
    int fd;

    gethostname(hostname, sizeof(hostname));
    

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
		    (void *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	closesocket( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
			(void *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    closesocket( fd );
	    exit( 1 );
	}
    }
#endif

    memset(&sa, '\0', sizeof(sa));
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) == -1 )
    {
	perror( "Init_socket: bind" );
	closesocket( fd );
	exit( 1 );
    }

    if ( listen( fd, 50 ) < 0 )
    {
	perror( "Init_socket: listen" );
	closesocket( fd );
	exit( 1 );
    }

    return fd;
}


/*
 * LAG alarm!							-Thoric
 */
void caught_alarm()
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "ALARM CLOCK!  In section %s", alarm_section );
    bug( buf );
    strcpy( buf, "Alas, the hideous malevalent entity known only as 'Lag' rises once more!\n\r" );
    echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
    if ( newdesc )
    {
	FD_CLR( newdesc, &in_set );
	FD_CLR( newdesc, &out_set );
	FD_CLR( newdesc, &exc_set );
	log_string( "clearing newdesc" );
    }
}

bool check_bad_desc( int desc )
{
    if ( FD_ISSET( desc, &exc_set ) )
    {
	FD_CLR( desc, &in_set );
	FD_CLR( desc, &out_set );
	log_string( "Bad FD caught and disposed." );
	return TRUE;
    }
    return FALSE;
}

/*
 * Determine whether this player is to be watched  --Gorog
 */
bool chk_watch(sh_int player_level, char *player_name, char *player_site)
{
    WATCH_DATA *pw;
/*
    char buf[MAX_INPUT_LENGTH];
    sprintf( buf, "che_watch entry: plev=%d pname=%s psite=%s",
                  player_level, player_name, player_site);
    log_string(buf);
*/
    if ( !first_watch ) return FALSE;

    for ( pw = first_watch; pw; pw = pw->next )
    {
        if ( pw->target_name )
        {
           if ( !str_cmp(pw->target_name, player_name)
           &&   player_level < pw->imm_level )
                 return TRUE;
        }
        else 
        if ( pw->player_site )
        {
           if ( !str_prefix(pw->player_site, player_site)
           &&   player_level < pw->imm_level )
                 return TRUE;
        }
    }
    return FALSE; 
}


void accept_new( int ctrl )
{
	static struct timeval null_time;
	DESCRIPTOR_DATA *d;
	/* int maxdesc; Moved up for use with id.c as extern */

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( ctrl, &in_set );
	maxdesc	= ctrl;
	newdesc = 0;
	for ( d = first_descriptor; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	    if ( d == last_descriptor )
	      break;
	}
//	auth_maxdesc(&maxdesc, &in_set, &out_set, &exc_set);
	
	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "accept_new: select: poll" );
	    exit( 1 );
	}

	if ( FD_ISSET( ctrl, &exc_set ) )
	{
	    bug( "Exception raise on controlling descriptor %d", ctrl );
	    FD_CLR( ctrl, &in_set );
	    FD_CLR( ctrl, &out_set );
	}
	else
	if ( FD_ISSET( ctrl, &in_set ) )
	{
	    newdesc = ctrl;
	    new_descriptor( newdesc );
	}
}

void emergancy_copyover( void )
{
  FILE *fp;
  DESCRIPTOR_DATA *d;
  char buf[100], buf2[100], buf3[100], buf4[100], buf5[100];

  log_string( "--- Engaging Emergancy Copyover! ---" );

  fp = fopen (COPYOVER_FILE, "w");

  if (!fp)
  {
     log_string ( "Could not write to copyover file!" );
     perror ("emergancy_copyover:fopen");
     return;
  }
	
  sprintf (buf, "\n\r [ALERT]: EMERGENCY COPYOVER - Keep calm, we might pull through!\n\r" );
	  
  /* For each playing descriptor, save its state */
  for (d = first_descriptor; d ; d = d->next)
  {
      CHAR_DATA * och = CH (d);
      d_next = d->next; /* We delete from the list , so need to save this */
		
      if (!och || !d->character || d->connected > CON_PLAYING) /* drop those logging on */
      {
	  write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
	  /* close_socket (d, FALSE); */
      }
      else
      {
	  fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
	  save_char_obj (och);
	  write_to_descriptor (d->descriptor, buf, 0);
      }
  }
	
  fprintf (fp, "-1\n");
  fclose (fp);
	
  fclose (fpReserve);
  fclose (fpLOG);

  sprintf (buf, "%d", port);
  sprintf (buf2, "%d", control);
  sprintf (buf3, "%d", control2);

  execl (EXE_FILE,  "dbzc", buf, "copyover", buf2, buf3, buf4, buf5, (char *) NULL);

  perror ("emergancy_copyover: failed to copyover in 'execl'");
		
  if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
  {
      perror( NULL_FILE );
      exit( 1 );
  }
  if ( ( fpLOG = fopen( NULL_FILE, "r" ) ) == NULL )
  {
      perror( NULL_FILE );
      exit( 1 );
  }
}

void emergancy_arm( )
{
     if ( !emergancy_copy )
     {
	  bug( "Notice: Emergancy hotboot system is ready." );
	  emergancy_copy = TRUE;
     }
     return;
}

 

int file_size( char * buf )
{
    FILE *fp;
    int size=0;

    if ( ( fp = fopen( buf, "rb" ) ) == NULL )
    {
       bug( "file_size: could not open file to retrive size.");
       return -1;
    }

    /* Seek to end of file */
    if ( fseek( fp, 0, SEEK_END ) != 0 )
    {
       bug( "file_size: failed seek-to-end operation.");
       fclose( fp );
       return -1;
    }

    /* Returns the number of characters from the beginning */
    size = ftell( fp );

    fclose( fp );

    return size;
}


 


void game_loop( )
{
    struct timeval	  last_time;
    char cmdline[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
/*  time_t	last_check = 0;  */

   
//    signal( SIGSEGV, SegVio ); 
    
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !mud_down )
    {
	accept_new( control  );
/*	accept_new( control2 );
	accept_new( conclient);
	accept_new( conjava  );*/
	
//	auth_check(&in_set, &out_set, &exc_set);
	
	/*
	 * Kick out descriptors with raised exceptions
	 * or have been idle, then check for input.
	 */
	for ( d = first_descriptor; d; d = d_next )
	{
	    if ( d == d->next )
	    {
	      bug( "descriptor_loop: loop found & fixed" );
	      d->next = NULL;
	    }
 	    d_next = d->next;   

	    d->idle++;	/* make it so a descriptor can idle out */
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character
		&& ( d->connected == CON_PLAYING
		||   d->connected == CON_EDITING ) )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d, TRUE );
		continue;
	    }
	    else 
	    if ( (!d->character && d->idle > 360)		  /* 2 mins */
            ||   ( d->connected != CON_PLAYING && d->idle > 1200) /* 5 mins */
	    ||     (d->idle > 28800 && d->character->level < 65) )  /* 2 hrs  */
	    {
		write_to_descriptor( d->descriptor,
		 "Idle timeout... disconnecting.\n\r", 0 );
		d->outtop	= 0;
		close_socket( d, TRUE );
		continue;
	    }
	    else
	    {
		d->fcommand	= FALSE;

		if ( FD_ISSET( d->descriptor, &in_set ) )
		{
			d->idle = 0;
			if ( d->character )
			  d->character->timer = 0;
			if ( !read_from_descriptor( d ) )
			{
			    FD_CLR( d->descriptor, &out_set );
			    if ( d->character
			    && ( d->connected == CON_PLAYING
			    ||   d->connected == CON_EDITING ) )
				save_char_obj( d->character );
			    d->outtop	= 0;
			    close_socket( d, FALSE );
			    continue;
			}
		}

		if ( d->character && d->character->wait > 0 )
		{
			--d->character->wait;
			continue;
		}

		read_from_buffer( d );
		if ( d->incomm[0] != '\0' )
		{
			d->fcommand	= TRUE;
			stop_idling( d->character );

			strcpy( cmdline, d->incomm );
			d->incomm[0] = '\0';
			
			if ( d->character )
			  set_cur_char( d->character );

			if ( d->pagepoint )
			  set_pager_input(d, cmdline);
			else
			  switch( d->connected )
			  {
			   default:
 				nanny( d, cmdline );
				break;
			   case CON_PLAYING:
				interpret( d->character, cmdline );
				break;
			   case CON_EDITING:
				edit_buffer( d->character, cmdline );
				break;
			  }
		}
	    }
	    if ( d == last_descriptor )
	      break;
	}


	/*
	 * Autonomous game motion.
	 */
	update_handler( );

	/*
	 * Output.
	 */
	for ( d = first_descriptor; d; d = d_next )
	{
	    d_next = d->next;   

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
	        if ( d->pagepoint )
	        {
	          if ( !pager_output(d) )
	          {
	            if ( d->character
	            && ( d->connected == CON_PLAYING
	            ||   d->connected == CON_EDITING ) )
	                save_char_obj( d->character );
	            d->outtop = 0;
	            close_socket(d, FALSE);
	          }
	        }
		else if ( !flush_buffer( d, TRUE ) )
		{
		    if ( d->character
		    && ( d->connected == CON_PLAYING
		    ||   d->connected == CON_EDITING ) )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d, FALSE );
		}
	    }
	    if ( d == last_descriptor )
	      break;
	}
	
	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
#ifdef WIN32
		Sleep( (stall_time.tv_sec * 1000L) + (stall_time.tv_usec / 1000L) );
#else
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 && errno != EINTR )
		{
		    perror( "game_loop: select: stall" );
		    exit( 1 );
		}
#endif
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;

        /* Check every 5 seconds...  (don't need it right now)
	if ( last_check+5 < current_time )
	{
	  CHECK_LINKS(first_descriptor, last_descriptor, next, prev,
	      DESCRIPTOR_DATA);
	  last_check = current_time;
	}
	*/
    }

    fflush(stderr);	/* make sure strerr is flushed */
    return;
}



void new_descriptor( int new_desc )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    size_t desc, size;
    char bugbuf[MAX_STRING_LENGTH];
//    char buf2[MSL];
#ifdef WIN32
    unsigned long arg = 1;
#endif

    size = sizeof(sock);
    if ( check_bad_desc( new_desc ) )
    {
      set_alarm( 0 );
      return;
    }
    set_alarm( 20 );
    alarm_section = "new_descriptor::accept";
    if ( ( desc = accept( new_desc, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	sprintf(bugbuf, "[*****] BUG: New_descriptor: accept");
	log_string_plus( bugbuf, LOG_COMM, sysdata.log_level );
	set_alarm( 0 );
	return;
    }
    if ( check_bad_desc( new_desc ) )
    {
      set_alarm( 0 );
      return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    set_alarm( 20 );
    alarm_section = "new_descriptor: after accept";

#ifdef WIN32
    if ( ioctlsocket(desc, FIONBIO, &arg) == -1 )
#else
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
#endif
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	set_alarm( 0 );
	return;
    }
    if ( check_bad_desc( new_desc ) )
      return;

    CREATE( dnew, DESCRIPTOR_DATA, 1 );
    dnew->next		= NULL;
    dnew->descriptor	= desc;
    dnew->connected	= CON_GET_NAME;
    dnew->ansi          = TRUE;
    dnew->outsize	= 2000;
    dnew->idle		= 0;
    dnew->lines		= 0;
    dnew->scrlen	= 24;
    dnew->port		= ntohs( sock.sin_port );
  //dnew->user 		= STRALLOC("(unknown)");
    dnew->newstate	= 0;
    dnew->prevcolor	= 0x07;
    dnew->client       = STRALLOC( "Unidentified" ); /* Terminal detect */

    CREATE( dnew->outbuf, char, dnew->outsize );
    strcpy( buf, inet_ntoa( sock.sin_addr ) );

/*
 *   HardBan - Disconnects user before they have a chance to name resolve.
 *             Damned spammers ;) -Tawnos
 *
 *
 *  if(!str_cmp(buf,"204.38.47.131"))
 *  {
       set_alarm( 0 );
 *	write_to_descriptor(desc, "Your personalized message here!", 0);
 *	free_desc(dnew);
 *	return;
 *   }

172.141.248.16
   if(!str_cmp(buf,"70.70.56.74"))
   {
       set_alarm( 0 );
       free_desc(dnew);
       return;
    }
*/
   if(!str_cmp(buf,"63.78.203.210"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "You are not welcomed. Later.", 0);
       free_desc(dnew);
       return;
    }
/*
   if(!str_cmp(buf,"69.73.203.238"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go advertise in those DBsaga muds, ass", 0);
       free_desc(dnew);
       return;
    }
*/
/*   if(!str_cmp(buf,"216.45.165.55"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Now this is supposed to scare you...", 0);
       free_desc(dnew);
       return;
    }
*/
   if(!str_cmp(buf,"172.141.248.16"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go fuck somewhere else, Kiro.", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"70.145.41.46"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go fuck yourself, asshole.", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"70.246.127.94"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go learn to code from a firstgrade teacher.", 0);
       free_desc(dnew);
       return;
    }


/*   if(!str_cmp(buf,"150.208.213.176"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "This is a test, please keep contact with Ananaeias.", 0);
       free_desc(dnew);
       return;
    }


   sprintf( buf2, "%s", buf );

   buf2[3] = 0;
   if(!str_cmp(buf2,"172"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Sorry, all Ips starting with 172 have been hardbanned.", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"172.146.213.183"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go read the rules.", 0);
       free_desc(dnew);
       return;
    }
  
   if(!str_cmp(buf,"172.153.143.199"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go play somewhere else if you think Im biased, asshole", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"172.158.164.223"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go advertise somewhere else, asshole", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"172.142.87.10"))
   {
       set_alarm( 0 );
       write_to_descriptor(desc, "Go advertise to yo mama, faggot!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"24.16.120.64") || !str_cmp(buf, "218.252.87.31" ) || !str_cmp( buf, "69.60.229.138"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"81.56.64.39") || !str_cmp(buf,"80.36.152.192") || !str_cmp(buf, "24.177.54.141"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.215.32.253") || !str_cmp(buf, "62.178.126.35") || !str_cmp(buf, "62.178.126.35")) 
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"200.171.91.254") || !str_cmp(buf, "200.44.224.72") || !str_cmp(buf, "4.26.91.177") || !str_cmp(buf, "68.46.59.132"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"61.182.248.183") || !str_cmp(buf,"200.68.252.28"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"201.248.90.103") )
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"61.65.88.189"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"80.38.122.67"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"211.161.142.9"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"24.15.75.175"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"211.21.99.210"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"211.21.99.210"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"68.185.244.81"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"4.227.23.6"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"61.218.0.62"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"202.109.126.27"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"200.107.170.66"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.232.35.26"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"24.3.23.46"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"203.122.0.202"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"200.171.174.24"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.34.8.213"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"203.80.119.194"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.117.168.115"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.10.31.71"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.18.63.19"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.2.220.190"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"67.165.131.252"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"211.58.122.45"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"202.108.35.12"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"68.37.73.253"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

   if(!str_cmp(buf,"218.75.60.42"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"203.130.194.187"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"82.77.40.199"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.20.201.147"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"61.240.230.179"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"24.7.239.71"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }
   if(!str_cmp(buf,"4.46.178.71"))
   {
      set_alarm( 0 );
       write_to_descriptor(desc, "Lookie here, proxy this bitch!", 0);
       free_desc(dnew);
       return;
    }

if(!str_cmp(buf,"12.203.200.101"))
{
 set_alarm( 0 );
 write_to_descriptor(desc, "Try'ed to hack my account eh?", 0);
 free_desc(dnew);
 return;
}   
*/
/*    if(!str_cmp(buf, "216.194.99.14"))
     sprintf(log_buf,"Mudconnector.com active connection check (Possible Multiplay): %s", buf);
    else if(!str_cmp(buf, "216.40.217.211"))
     sprintf(log_buf,"Kyndig.com active connection check (Possible Multiplay): %s", buf);
    else*/
    sprintf( log_buf, "Sock.sinaddr:  %s, on port %hd.",
		buf, dnew->port );

    log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
    if ( sysdata.NO_NAME_RESOLVING )
      dnew->host = STRALLOC( buf );
    else
    {
       from = gethostbyaddr( (char *) &sock.sin_addr,
	  	sizeof(sock.sin_addr), AF_INET );
       dnew->host = STRALLOC( (char *)( from ? from->h_name : buf) );
    }

    if ( check_total_bans( dnew ) )
    {
          write_to_descriptor(desc,
                         "Your site has been banned from this Mud.\n\r", 0);
          free_desc (dnew);
          set_alarm (0);
          return;
     }
    /*
     * Init descriptor data.
     */

    if ( !last_descriptor && first_descriptor )
    {
	DESCRIPTOR_DATA *d;

	bug( "New_descriptor: last_desc is NULL, but first_desc is not! ...fixing" );
	for ( d = first_descriptor; d; d = d->next )
	   if ( !d->next )
		last_descriptor = d;
    }

    LINK( dnew, first_descriptor, last_descriptor, next, prev );


    /*
     * Send the greeting.
     */
  /*  {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( dnew, help_greeting+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting  , 0 );
    }*/
      {
	     extern char * help_greeting;
             extern char * help_greeting2;
             extern char * help_greeting3;
             extern char * help_greeting4;

        switch( number_range(0, 3) )
        {
    		default:
    	        	if ( help_greeting[0] == '.' )
		        send_to_desc_color( help_greeting+1, dnew );
		        else
		        send_to_desc_color( help_greeting , dnew );
			break;
                case 1:
                        if ( help_greeting2[0] == '.' )
                        send_to_desc_color( help_greeting2+1, dnew );
                        else
                        send_to_desc_color( help_greeting2 , dnew );
			break;
                case 2:
                        if ( help_greeting3[0] == '.' )
                        send_to_desc_color( help_greeting3+1, dnew );
                        else
                        send_to_desc_color( help_greeting3 , dnew );
			break;
                case 3:
                        if ( help_greeting4[0] == '.' )
                        send_to_desc_color( help_greeting4+1, dnew );
                        else
                        send_to_desc_color( help_greeting4 , dnew );
			break;
        }
      }


    alarm_section = "new_descriptor: set_auth";
//    set_auth(dnew);
    alarm_section = "new_descriptor: after set_auth";

    if ( ++num_descriptors > sysdata.maxplayers )
	sysdata.maxplayers = num_descriptors;
    if ( sysdata.maxplayers > sysdata.alltimemax )
    {
	if ( sysdata.time_of_max )
	  DISPOSE(sysdata.time_of_max);
	sprintf(buf, "%24.24s", ctime(&current_time));
	sysdata.time_of_max = str_dup(buf);
	sysdata.alltimemax = sysdata.maxplayers;
	sprintf( log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax );
	log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
	to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
	save_sysdata( sysdata );
    }
    set_alarm(0);
    return;
}


void free_desc( DESCRIPTOR_DATA *d )
{
    closesocket( d->descriptor );
    if(d->host)
      STRFREE( d->host );
    if(d->outbuf)
      DISPOSE( d->outbuf );
    if ( d->pagebuf )
	DISPOSE( d->pagebuf );
//        STRFREE( d->client );

    DISPOSE( d );
    --num_descriptors;
    return;
}


void close_socket( DESCRIPTOR_DATA *dclose, bool force )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    bool DoNotUnlink = FALSE;

    /* flush outbuf */
    if ( !force && dclose->outtop > 0 )
	flush_buffer( dclose, FALSE );

    /* say bye to whoever's snooping this descriptor */
    if ( dclose->snoop_by )
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    if ( dclose->fused_with )
        write_to_buffer( dclose->fused_with,
            "Your fused partner has left the game.\n\r", 0 );

    /* stop snooping everyone else */

    for ( d = first_descriptor; d; d = d->next )
    {
	if ( d->snoop_by == dclose )
	  d->snoop_by = NULL;
        if ( d->fused_with == dclose )
          d->fused_with = NULL;
    }
    /* Check for switched people who go link-dead. -- Altrag */
    if ( dclose->original )
    {
	if ( ( ch = dclose->character ) != NULL )
	  do_return(ch, "");
	else
	{
	  bug( "Close_socket: dclose->original without character %s",
		(dclose->original->name ? dclose->original->name : "unknown") );
	  dclose->character = dclose->original;
	  dclose->original = NULL;
	}
    }
    
    ch = dclose->character;

    /* sanity check :( */
    if ( !dclose->prev && dclose != first_descriptor )
    {
	DESCRIPTOR_DATA *dp, *dn;
	bug( "Close_socket: %s desc:%p != first_desc:%p and desc->prev = NULL!",
		ch ? ch->name : d->host, dclose, first_descriptor );
	dp = NULL;
	for ( d = first_descriptor; d; d = dn )
	{
	   dn = d->next;
	   if ( d == dclose )
	   {
		bug( "Close_socket: %s desc:%p found, prev should be:%p, fixing.",
		    ch ? ch->name : d->host, dclose, dp );
		dclose->prev = dp;
		break;
	   }
	   dp = d;
	}
	if ( !dclose->prev )
	{
	    bug( "Close_socket: %s desc:%p could not be found!.",
		    ch ? ch->name : dclose->host, dclose );
	    DoNotUnlink = TRUE;
	}
    }
    if ( !dclose->next && dclose != last_descriptor )
    {
	DESCRIPTOR_DATA *dp, *dn;
	bug( "Close_socket: %s desc:%p != last_desc:%p and desc->next = NULL!",
		ch ? ch->name : d->host, dclose, last_descriptor );
	dn = NULL;
	for ( d = last_descriptor; d; d = dp )
	{
	   dp = d->prev;
	   if ( d == dclose )
	   {
		bug( "Close_socket: %s desc:%p found, next should be:%p, fixing.",
		    ch ? ch->name : d->host, dclose, dn );
		dclose->next = dn;
		break;
	   }
	   dn = d;
	}
	if ( !dclose->next )
	{
	    bug( "Close_socket: %s desc:%p could not be found!.",
		    ch ? ch->name : dclose->host, dclose );
	    DoNotUnlink = TRUE;
	}
    }

    if ( dclose->character )
    {
	sprintf( log_buf, "Closing link to %s.", ch->pcdata->filename );
	log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );
/*
	if ( ch->level < LEVEL_DEMI )
	  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->level );
*/
/*	if ( dclose->connected == CON_PLAYING
	||   dclose->connected == CON_EDITING )*/
        if ( (dclose->connected == CON_PLAYING
             || dclose->connected == CON_EDITING)
             ||(dclose->connected >= CON_NOTE_TO
             && dclose->connected <= CON_NOTE_FINISH))
	{
	    act( AT_ACTION, "$n has lost $s link.", ch, NULL, NULL, TO_CANSEE );
            do_drop( ch, "all.dragonball");
            do_defuse( ch, "" );
            if (xIS_SET(ch->powerup, PLR_BOOST))
            {
               xREMOVE_BIT(ch->powerup, PLR_BOOST);
               check_boost();
            }
	    ch->desc = NULL;
	}
	else
	{
	    /* clear descriptor pointer to get rid of bug message in log */
	    dclose->character->desc = NULL;
	    free_char( dclose->character );
	}
    }


    if ( !DoNotUnlink )
    {
	/* make sure loop doesn't get messed up */
	if ( d_next == dclose )
	  d_next = d_next->next;
	UNLINK( dclose, first_descriptor, last_descriptor, next, prev );
    }

    if ( dclose->descriptor == maxdesc )
      --maxdesc;

    free_desc( dclose );
    --num_descriptors;
    return;
}


bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart, iErr;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 20 consecutive times!\n\r", 0 );
	return FALSE;
    }

    for ( ; ; )
    {
	int nRead;

	nRead = recv( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart, 0 );
#ifdef WIN32
	iErr = WSAGetLastError ();
#else
	iErr = errno;
#endif
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string_plus( "EOF encountered on read.", LOG_COMM, sysdata.log_level );
	    return FALSE;
	}
	else if ( iErr == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;
    unsigned char *p;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;


   /* Thanks Nick! */
   for( p = (unsigned char *)d->inbuf; *p; p++ )
   {
      if( *p == IAC )
      {
         if( memcmp( p, term_call_back_str, sizeof( term_call_back_str ) ) == 0 )
         {
            int pos = (char *)p - d->inbuf;  /* where we are in buffer */
            int len = sizeof( d->inbuf ) - pos - sizeof( term_call_back_str );  /* how much to go */
            char tmp[100];
            unsigned int x = 0;
            unsigned char * oldp = p;       

            p += sizeof( term_call_back_str );  /* skip TERMINAL_TYPE / IS characters */

            for( x = 0; x < ( sizeof (tmp) - 1 ) 
               && *p != 0     /* null marks end of buffer */
               && *p != IAC;  /* should terminate with IAC */
               x++, p++ )
               tmp[x] = *p;
              
            tmp[x] = '\0';
		   STRFREE( d->client );
		d->client = STRALLOC( tmp );
            p += 2; /* skip IAC and SE */
            len -= strlen( tmp ) + 2;
            if( len < 0 )
            len = 0;

            /* remove string from input buffer */
            memmove( oldp, p, len );
         } /* end of getting terminal type */
      } /* end of finding an IAC */
   }


    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && i<MAX_INBUF_SIZE;
	  i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	//if ( k >= 254 )
	if ( k >= 1022)
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    /*
	    for ( ; d->inbuf[i] != '\0' || i>= MAX_INBUF_SIZE ; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    */
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 40 )
	    {
/*		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf );
*/
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\rYou cannot enter the same command more than 40 consecutive times!\n\r", 0 );
		strcpy( d->incomm, "quit" );
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool flush_buffer( DESCRIPTOR_DATA *d, bool fPrompt )
{
    char buf[MAX_INPUT_LENGTH];
    extern bool mud_down;

    /*
     * If buffer has more than 4K inside, spit out .5K at a time   -Thoric
     */
    if ( !mud_down && d->outtop > 4096 )
    {
	memcpy( buf, d->outbuf, 512 );
	d->outtop -= 512;
	memmove( d->outbuf, d->outbuf + 512, d->outtop );


	if ( d->snoop_by )
	{
	    char snoopbuf[MAX_INPUT_LENGTH];

	    buf[512] = '\0';
	    if ( d->character && d->character->name )
	    {
		if (d->original && d->original->name)
		    sprintf( snoopbuf, "%s (%s)", d->character->name, d->original->name );
		else          
		    sprintf( snoopbuf, "%s", d->character->name);
		write_to_buffer( d->snoop_by, snoopbuf, 0);
	    }
	    write_to_buffer( d->snoop_by, "% ", 2 );
	    write_to_buffer( d->snoop_by, buf, 0 );
	}
        if ( d->fused_with )
        {
            char fusebuf[MAX_INPUT_LENGTH];

            buf[512] = '\0';
            if ( d->character && d->character->name )
            {
                if (d->original && d->original->name)
                    sprintf( fusebuf, "%s (%s)", d->character->name, d->original->name );
                else
                    sprintf( fusebuf, "%s", d->character->name);
                write_to_buffer( d->fused_with, fusebuf, 0);
            }
            write_to_buffer( d->fused_with, "% ", 2 );
            write_to_buffer( d->fused_with, buf, 0 );
        }
        if ( !write_to_descriptor( d->descriptor, buf, 512 ) )
        {
	    d->outtop = 0;
	    return FALSE;
        }
        return TRUE;
    }
                                                                                        

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !mud_down && d->connected == CON_PLAYING )
    {
	CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;
	if ( xIS_SET(ch->act, PLR_BLANK) )
	    write_to_buffer( d, "\n\r", 2 );

	    
	if ( xIS_SET(ch->act, PLR_PROMPT))
	    display_prompt(d);

	if ( xIS_SET(ch->act, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by )
    {
        /* without check, 'force mortal quit' while snooped caused crash, -h */
	if ( d->character && d->character->name )
	{
	    /* Show original snooped names. -- Altrag */
	    if ( d->original && d->original->name )
		sprintf( buf, "%s (%s)", d->character->name, d->original->name );
	    else
		sprintf( buf, "%s", d->character->name);
	    write_to_buffer( d->snoop_by, buf, 0);
	}
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }
    if ( d->fused_with )
    {
        /* without check, 'force mortal quit' while snooped caused crash, -h */
        if ( d->character && d->character->name )
        {
            /* Show original snooped names. -- Altrag */
            if ( d->original && d->original->name )
                sprintf( buf, "%s (%s)", d->character->name, d->original->name );
            else
                sprintf( buf, "%s", d->character->name);
            write_to_buffer( d->fused_with, buf, 0);
        }
        write_to_buffer( d->fused_with, "% ", 2 );
        write_to_buffer( d->fused_with, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    if ( !d )
    {
	bug( "Write_to_buffer: NULL descriptor" );
	return;
    }

    /*
     * Normally a bug... but can happen if loadup is used.
     */
    if ( !d->outbuf )
    	return;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

/* Uncomment if debugging or something
    if ( length != strlen(txt) )
    {
	bug( "Write_to_buffer: length(%d) != strlen(txt)!", length );
	length = strlen(txt);
    }
*/

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
        if (d->outsize > 32000)
	{
	    /* empty buffer */
	    d->outtop = 0;
          /* Bugfix by Samson - moved bug() call up */
	    bug("Buffer overflow. Closing (%s).", d->character ? d->character->name : "???" );
	    close_socket(d, TRUE);
	    return;
 	}
	d->outsize *= 2;
	RECREATE( d->outbuf, char, d->outsize );
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    d->outbuf[d->outtop] = '\0';
    return;
}


/*
* Lowest level output function. Write a block of text to the file descriptor.
* If this gives errors on very long blocks (like 'ofind all'), try lowering
* the max block size.
*
* Added block checking to prevent random booting of the descriptor. Thanks go
* out to Rustry for his suggestions. -Orion
*/

bool write_to_descriptor( int desc, char *txt, int length )
{
   int iStart = 0;
   int nWrite = 0;
   int nBlock = 0;
   int iErr   = 0;

   if ( length <= 0 )
       length = strlen(txt);

   for ( iStart = 0; iStart < length; iStart += nWrite )
   {
       nBlock = UMIN( length - iStart, 4096 );
       nWrite = send( desc, txt + iStart, nBlock, 0 );

       if ( nWrite == -1 )
       {
#ifdef WIN32
           iErr = WSAGetLastError ();
#else
           iErr = errno;
#endif
           if ( iErr == EWOULDBLOCK )
           {
               nWrite = 0;
               continue;
           }
           else
           {
               perror( "Write_to_descriptor" );
               return FALSE;
           }
       }
   }

   return TRUE;
}


void show_title( DESCRIPTOR_DATA *d )
{
    CHAR_DATA *ch;

    ch = d->character;

    if ( !xIS_SET( ch->pcdata->flags, PCFLAG_NOINTRO ) )
    {
	if (xIS_SET(ch->act, PLR_RIP))
	  send_rip_title(ch);
	else
	if (xIS_SET(ch->act, PLR_ANSI))
	  send_ansi_title(ch);
	else
	  send_ascii_title(ch);
    }
    else
    {
      write_to_buffer( d, "Press enter...\n\r", 0 );
    }
    d->connected = CON_PRESS_ENTER;
}

void check_multi( DESCRIPTOR_DATA *d , char *name )
{
        DESCRIPTOR_DATA *dold;
        
    for ( dold = first_descriptor; dold; dold = dold->next )
    {
	if ( dold != d
	&& (  dold->character || dold->original )
	&&   str_cmp( name, dold->original
		 ? dold->original->name : dold->character->name ) 
        && !str_cmp(dold->host , d->host ) )
	{
	        const char *ok = "194.234.177";
	        const char *ok2 = "209.183.133.229";
	        int iloop;
	        
		for ( iloop = 0 ; iloop < 11 ; iloop++ )
	        {
	            if ( ok[iloop] != d->host[iloop] )
	              break;
	        }
	        if ( iloop >= 10 )
	           return; 
		for ( iloop = 0 ; iloop < 11 ; iloop++ )
	        {
	            if ( ok2[iloop] != d->host[iloop] )
	              break;
	        }
	        if ( iloop >= 10 )
	           return; 
                if ( dold->character->level == 65)
                   break;
                if ( d->character->level == 65 )
                   break;
		sprintf( log_buf, "%s attempting to multiplay with %s.", dold->original ? dold->original->name : dold->character->name , d->character->name );
		log_string_plus( log_buf, LOG_COMM, sysdata.log_level );	

	}
    }


}                


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
/*	extern int lang_array[];
	extern char *lang_names[];*/
    int col=0;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    char log_buf[MSL];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iRace;
    bool fOld, chk;

  iRace = 0;
/*    while ( isspace(*argument) )
	argument++;*/
    if( d->connected != CON_NOTE_TEXT )
    {
        while ( isspace(*argument) )
            argument++;
    }

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d, TRUE );
	return;

    case CON_GET_NAME:

/*	if ( argument[0] == '\0' )
	{
	    close_socket( d, FALSE );
	    return;
	}*/
        if( !argument || argument[0] == '\0' )
        {
            close_socket( d, FALSE );
            return;
        }

        argument = strlower( argument ); /* Modification to force proper name display */
	argument[0] = UPPER(argument[0]);

	/* Old players can keep their characters. -- Alty */
	if ( !check_parse_name( argument, (d->newstate != 0) ) )
	{
	    send_to_desc_color( "\n\r&RIllegal name, try another.\n\r&YName&O:&w ", d );
	    return;
	}
                                                
        if ( !str_cmp( argument, "New" ) )
	{
	    if (d->newstate == 0)
	    {
              /* New player */
              /* Don't allow new players if DENY_NEW_PLAYERS is true */
      	      if (sysdata.DENY_NEW_PLAYERS == TRUE)
      	      {
                send_to_desc_color( "&RThe MUD is currently preparing for a reboot.\n\rNew players are not accepted during this time.\n\rPlease try again in a few minutes.\n\r", d);
		close_socket( d, FALSE );
              }
              send_to_desc_color( "\n\r&zChoosing a name is one of the most important parts of this game...\n\rMake sure to pick a name appropriate to the character you are going\n\rto role play, and be sure that it suits a &WDBZ Theme&z.\n\rIf the name you select is not acceptable, you will be asked to choose\n\ranother one.\n\r\n\rPlease choose a &WName&z for your character:&w\n\r", d);
	      d->newstate++;
	      d->connected = CON_GET_NAME;
	      return;
	    }
	    else
   	    {
	      send_to_desc_color("\n\r&RIllegal name, try another.\n\r&YName&O: ", d);
	      return;
	    }
	}
	if ( check_playing( d, argument, FALSE ) == BERR )
	{
	    send_to_desc_color( "&YName&O: ", d );
	    return;
	}

	fOld = load_char_obj( d, argument, TRUE, FALSE );
	if ( !d->character )
	{
	    sprintf( log_buf, "Bad player file %s@%s.", argument, d->host );
	    log_string( log_buf );
	    send_to_desc_color( "&zYour playerfile is corrupt...Please notify &WRiketsukirai@hotmail.com\n\r", d );
	    close_socket( d, FALSE );
	    return;
	}
	ch   = d->character;
      if ( check_bans( ch, BAN_SITE ) )
      {
              send_to_desc_color( "&RYour site has been banned from this Mud.\n\r", d);
              close_socket (d, FALSE);
              return;
      }

      if ( fOld ) {
      if ( check_bans( ch, BAN_CLASS ) )
      {
              send_to_desc_color("&RYour class has been banned from this Mud.\n\r", d);
              close_socket (d, FALSE);
              return;
      }
      if ( check_bans( ch, BAN_RACE ) )
      {
              send_to_desc_color("&RYour race has been banned from this Mud.\n\r", d);
              close_socket (d, FALSE);
              return;
      }
      }

	if ( xIS_SET(ch->act, PLR_DENY) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
	    if (d->newstate != 0)
	    {
              send_to_desc_color( "&zThat name is already taken.  Please choose another: ", d );
	      d->connected = CON_GET_NAME;
	      d->character->desc = NULL;
	      free_char( d->character ); /* Big Memory Leak before --Shaddai */
	      d->character = NULL;
	      return;
	    }
	    send_to_desc_color( "&RYou are denied access.\n\r", d );
	    close_socket( d, FALSE );
	    return;
	}
      /*
       *  Make sure the immortal host is from the correct place.
       *  Shaddai
       */

      if ( IS_IMMORTAL(ch) && sysdata.check_imm_host
           && !check_immortal_domain( ch , d->host) )
        {
        sprintf (log_buf, "%s's char being hacked from %s.", argument, d->host);
        log_string_plus (log_buf, LOG_COMM, sysdata.log_level);
        send_to_desc_color("&RThis hacking attempt has been logged.\n\r", d);
        close_socket (d, FALSE);
        return;
        }


	chk = check_reconnect( d, argument, FALSE );
	if ( chk == BERR )
	  return;

	if ( chk )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && !IS_IMMORTAL(ch) )
	    {
		send_to_desc_color( "&YThe game is wizlocked.  Only immortals can connect now.\n\r", d );
		send_to_desc_color( "&YPlease try back later.\n\r", d );
		close_socket( d, FALSE );
		return;
	    }
	}

	if ( fOld )
	{
	    if (d->newstate != 0)
	    {
	      send_to_desc_color( "&RThat name is already taken.\n\r&RPlease choose another&r: ", d );
	      d->connected = CON_GET_NAME;
	      d->character->desc = NULL;
	      free_char( d->character ); /* Big Memory Leak before --Shaddai */
	      d->character = NULL;
	      return;
	    }
	    /* Old player */
	    send_to_desc_color( "&YPassword&O: ", d );
	    //write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    if (d->newstate == 0)
	    {
	      /* No such player */
              sprintf( buf, "\n\rThat player doesnt exist, please check spelling or type &Wnew&R.\n\r");
//	      send_to_desc_color( "\n\r&RNo such player exists.\n\rPlease check your spelling, or type &Wnew&R to start a new player.\n\r\n\r&YName&O: ", d );
              send_to_desc_color( buf, d);
	      d->connected = CON_GET_NAME;
	      d->character->desc = NULL;
	      free_char( d->character ); /* Big Memory Leak before --Shaddai */
	      d->character = NULL;
	      return;
	    }
            sprintf( buf, "\n\rDid I get that right %s, (Y|N)?", argument );
            send_to_desc_color( buf, d);
            d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
	send_to_desc_color( "\n\r", d );


        if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
        {
	    send_to_desc_color( "&RWrong password.\n\r", d );
	    /* clear descriptor pointer to get rid of bug message in log */
	    d->character->desc = NULL;
	    close_socket( d, TRUE );
            
	    return;
	}

//	write_to_buffer( d, echo_on_str, 0 );

	if ( check_playing( d, ch->pcdata->filename, TRUE ) )
	    return;

	chk = check_reconnect( d, ch->pcdata->filename, TRUE );
	if ( chk == BERR )
	{
	    if ( d->character && d->character->desc )
	      d->character->desc = NULL;
	    close_socket( d, FALSE );
	    return;
	}
	if ( chk == TRUE )
	  return;

        check_multi( d, ch->name );
	sprintf( buf, ch->pcdata->filename );
	d->character->desc = NULL;
	free_char( d->character );
	d->character = NULL;
	fOld = load_char_obj( d, buf, FALSE, FALSE );
	ch = d->character;
        if ( ch->position ==  POS_FIGHTING )
		ch->position = POS_STANDING;

	sprintf( log_buf, "%s@%s has connected.", ch->pcdata->filename, 
		d->host );
	if ( ch->level < LEVEL_DEMI )
	{
	  /*to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->level );*/
	  log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
	}
	else
	  log_string_plus( log_buf, LOG_COMM, ch->level );
	show_title(d);
	break;

    case CON_CONFIRM_NEW_NAME:
    xSET_BIT(ch->act,PLR_ANSI); 
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "\n\r&zMake sure to use a password that won't be easily guessed by someone else."
	    		  "\n\r&zPick a good password for &W%s&z: %s",
		ch->name, echo_off_str );
	    send_to_desc_color( buf, d );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    send_to_desc_color( "\n\r&zOk, what IS it, then? ", d );
	    /* clear descriptor pointer to get rid of bug message in log */
	    d->character->desc = NULL;
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    send_to_desc_color( "&zPlease type &WYes &zor &WNo&z. ", d );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
	send_to_desc_color( "\n\r", d );

	if ( strlen(argument) < 5 )
	{
	    send_to_desc_color( "Password must be at least five characters long.\n\rPassword: ", d);
	    return;
	}

	if (argument[0] == '!' )
	 {
	  send_to_char( "New password cannot begin with the '!' character.",ch);
	  return;
	 }
        pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		send_to_desc_color( "New password not acceptable, try again.\n\rPassword: ", d);
		return;
	    }
	}

	DISPOSE( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	send_to_desc_color( "\n\rPlease retype the password to confirm: ", d );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
	send_to_desc_color("\n\r", d );

        if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
        {
            send_to_desc_color( "Passwords don't match.\n\rRetype password: ", d);
            d->connected = CON_GET_NEW_PASSWORD;
            return;
        }
	    
	send_to_desc_color( "\n\r&zYou may choose from the following races, or type help [race] to learn more:\n\r", d);
	buf[0] = '\0';
/*	for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	{
	  if (iRace != RACE_VAMPIRE && iRace != RACE_TESTRACE
	  && race_table[iRace]->race_name && race_table[iRace]->race_name[0] != '\0'
	  && str_cmp(race_table[iRace]->race_name,"unused") )
	  {
            if ( iRace > 0 )
	    {
		if ( strlen(buf)+strlen(race_table[iRace]->race_name) > 77 )
		{
		   strcat( buf, "\n\r" );
		   send_to_desc_color( buf, d );
		   buf[0] = '\0';
		}
		else
		   strcat( buf, " " );
	    }
	    strcat( buf, race_table[iRace]->race_name );
          }
        }
	strcat( buf, "]\n\r: " );
	send_to_desc_color( buf, d );
	d->connected = CON_GET_NEW_RACE;
	break;*/
	col=0;
	// So players can't play Buu
	for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	{
	  if ( race_table[iRace]->race_name && race_table[iRace]->race_name[0] != '\0')
	  {
            if ( iRace >= 0 )
	    {
		/*if ( strlen(buf)+strlen(race_table[iRace]->race_name) > 77 )
		{
		   strcat( buf, "\n\r" );
		   
		   write_to_buffer( d, buf, 0 );
		   buf[0] = '\0';
		}*/
	        sprintf(buf2, "&W[&z%-15.15s&W]&w  ", race_table[iRace]->race_name);
	        strcat( buf, buf2 );
	        if ( ++col % 3 == 0 ){
	          strcat( buf, "\n\r");
                  send_to_desc_color(buf, d );
                  buf[0] = '\0';
                }
	        
	    }
          }
        }
        if (col % 3 != 0)
          strcat( buf, "\n\r" );
        strcat( buf, "&z:&w " );
	send_to_desc_color( buf, d );
	d->connected = CON_GET_NEW_RACE;
	break;


    case CON_GET_NEW_RACE:
	argument = one_argument(argument, arg);
        if (!str_cmp( arg, "help") )
        {
          for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	  {
	    if ( toupper(argument[0]) == toupper(race_table[iRace]->race_name[0])
	    &&  !str_prefix( argument, race_table[iRace]->race_name) )
	    {
	      do_help(ch, argument);
      	      send_to_desc_color( "&zPlease choose a race&r: ", d);
	      return;
	    }
	  }
   	  send_to_desc_color( "No help on that topic.  Please choose a race: ", d );
	  return;
	}
	

	for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	{
	    if ( toupper(arg[0]) == toupper(race_table[iRace]->race_name[0])
	    &&   !str_prefix( arg, race_table[iRace]->race_name ) )
	    {
		ch->race = iRace;
		break;
	    }
	}

    if ( iRace == MAX_PC_RACE
    ||  !race_table[iRace]->race_name || race_table[iRace]->race_name[0] == '\0'
    ||   iRace == RACE_VAMPIRE
    ||   !str_cmp(race_table[iRace]->race_name,"unused")
       )
	{
	    send_to_desc_color( "That's not a race.\n\rWhat IS your race? ", d );
	    return;
	}
        if ( check_bans( ch, BAN_RACE ) )
        {
          send_to_desc_color("That race is not currently available.\n\rWhat is your race? ", d);
          return;
        }
        
  write_to_buffer( d, echo_on_str, 0 );
	if(str_cmp(race_table[ch->race]->race_name, "Buu") && str_cmp(race_table[ch->race]->race_name, "Namek") 
		&& str_cmp(race_table[ch->race]->race_name , "Biodroid") )
        	send_to_desc_color( "\n\r&zWhat is your sex (&WM&z/&WF&z/&WN&z)? ", d );
        d->connected = CON_GET_NEW_SEX;
	// Break statement will make the player press enter, but nothing there to enter if they
	// can't pick a sex! - Gatz
	if(str_cmp(race_table[ch->race]->race_name, "Buu") && str_cmp(race_table[ch->race]->race_name, "Namek")
                && str_cmp(race_table[ch->race]->race_name , "Biodroid") )
        	break;

    case CON_GET_NEW_SEX:
	if(str_cmp(race_table[ch->race]->race_name, "Buu") && str_cmp(race_table[ch->race]->race_name,"Namek") 
		&& str_cmp(race_table[ch->race]->race_name, "Biodroid"))
	{
        	switch ( argument[0] )
        	{
        		case 'm': case 'M': ch->sex = SEX_MALE;    break;
        		case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
        		case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
        		default:
            			send_to_desc_color( "That's not a sex.\n\rWhat IS your sex? ", d );
            			return;
             	}
	}
	else
		ch->sex = SEX_NEUTRAL;
        send_to_desc_color("\n\r&zWould you like RIP, ANSI or no graphic/color support, &z(&WR&z/&WA&z/&WN&z)?&w", d );
	d->connected = CON_GET_WANT_RIPANSI;
        break;

    case CON_GET_WANT_RIPANSI:
	switch ( argument[0] )
	{
	case 'r': case 'R':
	    xSET_BIT(ch->act,PLR_RIP);
	    xSET_BIT(ch->act,PLR_ANSI);
	    break;
	case 'a': case 'A': xSET_BIT(ch->act,PLR_ANSI);  break;
	case 'n': case 'N': break;
	default:
	    send_to_desc_color( "Invalid selection.\n\rRIP, ANSI or NONE? ", d );
	    return;
	}
       ch->class = 0;
	sprintf( log_buf, "%s@%s new %s %s.", ch->name, d->host,
		race_table[ch->race]->race_name,
		class_table[ch->class]->who_name );
	log_string_plus( log_buf, LOG_COMM, sysdata.log_level);
	to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
	send_to_desc_color( "\n\r&w", d );
	show_title(d);
	ch->level = 0;
	ch->position = POS_STANDING;
	d->connected = CON_PRESS_ENTER;
	set_pager_color( AT_PLAIN, ch );
	return;
	break;

    case CON_PRESS_ENTER:
/*        if ( chk_watch(get_trust(ch), ch->name, d->host) )
           xSET_BIT( ch->pcdata->flags, PCFLAG_WATCH );
        else
           xREMOVE_BIT( ch->pcdata->flags, PCFLAG_WATCH );
*/
	if ( ch->position == POS_MOUNTED )
		ch->position = POS_STANDING;

	set_pager_color( AT_PLAIN, ch );
	if ( xIS_SET(ch->act, PLR_RIP) )
	  send_rip_screen(ch);
	if ( xIS_SET(ch->act, PLR_ANSI) )
	  send_to_pager( "\033[2J", ch );
	else
	  send_to_pager( "\014", ch );
	if ( IS_IMMORTAL(ch) )
	  do_help( ch, "imotd" );
	if ( ch->level < 50 && ch->level > 0 )
	  do_help( ch, "motd" );
	if ( ch->level == 0 )
	  do_help( ch, "motd" );
	send_to_pager( "\n\rPress [ENTER] ", ch );
        d->connected = CON_READ_MOTD;
        break;

    case CON_READ_MOTD:
	{
	  char motdbuf[MAX_STRING_LENGTH];

	  sprintf( motdbuf, "\n\rWelcome to %s...\n\r", sysdata.mud_name);
	  send_to_desc_color( motdbuf, d );
	}
	add_char( ch );
	d->connected	= CON_PLAYING;
      /* hopefully clear up some ansi changing issues	  -Nopey */
	      set_char_color( AT_DGREEN, ch );
	if( !xIS_SET(ch->act, PLR_ANSI) && d->ansi == TRUE )
	    d->ansi = FALSE;
	else if( xIS_SET(ch->act, PLR_ANSI) && d->ansi == FALSE)
	    d->ansi = TRUE;

	if ( ch->level == 0 )
	{
	    OBJ_DATA *obj;

	    ch->pcdata->clan	  = NULL;
	    ch->height = number_range(race_table[ch->race]->height *.9, race_table[ch->race]->height *1.1);
	    ch->weight = number_range(race_table[ch->race]->weight *.9, race_table[ch->race]->weight *1.1);

/*
	    if ( (iLang = skill_lookup( "common" )) < 0 )
	    	bug( "Nanny: cannot find common language." );
	    else
	    	ch->pcdata->learned[iLang] = 100;
	    	
	    for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
	    	if ( lang_array[iLang] == race_table[ch->race]->language )
	    		break;
	    if ( lang_array[iLang] == LANG_UNKNOWN )
	    	bug( "Nanny: invalid racial language." );
	    else
	    {
	    	if ( (iLang = skill_lookup( lang_names[iLang] )) < 0 )
	    		bug( "Nanny: cannot find racial language." );
	    	else
	    		ch->pcdata->learned[iLang] = 100;
	    }*/

            /* ch->resist           += race_table[ch->race]->resist;    drats */
            /* ch->susceptible     += race_table[ch->race]->suscept;    drats */

		reset_colors( ch );
	    name_stamp_stats( ch );

	    ch->level	= 5;
	    ch->exp	= 5000; /* give em some pl */
            ch->mod     = 1;
            ch->kimod   = 1;
            ch->max_hit    = 150;
            ch->max_mana   = 100;
	    ch->hit	= 150;
	    ch->mana	= 100;
	    ch->move	= 100;
            ch->gold    = 20000;
           
	    ch->auracolor = -1;
	    ch->aurapowerup = -1;
            ch->style   = STYLE_AGGRESSIVE;
            ch->skigain   = 1;
            ch->statgain  = 1;
            ch->stat      = 4; 
	    sprintf( buf, " the Fighter");
	    ch->pcdata->title = STRALLOC(buf);
            if (CAN_OOZARU(ch)) { xSET_BIT(ch->act, PLR_TAIL); }
            if (IS_ICER(ch))    { xSET_BIT(ch->act, PLR_TAIL ); xSET_BIT(ch->powerup, PU_TRANSFORM1); }
            if (IS_MUTANT(ch))  { xSET_BIT(ch->powerup, PU_MUTATE1 ); }
            if (IS_BIODROID(ch))  { xSET_BIT(ch->powerup, PU_LARVAE ); }
	    xSET_BIT( ch->act, PLR_AUTOGOLD ); 
	    xSET_BIT( ch->act, PLR_AUTOEXIT ); 
  	    xSET_BIT(ch->pcdata->flags, PCFLAG_STAT );
            xSET_BIT(ch->pcdata->flags, PCFLAG_COMBAT );
            xSET_BIT(ch->pcdata->flags, PCFLAG_INFO );
            xSET_BIT(ch->pcdata->flags, PCFLAG_PKILL );
            xSET_BIT(ch->pcdata->flags, PCFLAG_CLAN );

            /* Added by Brittany, Nov 24/96.  The object is the adventurer's guide
               to the realms of despair, part of Academy.are. */
            {
            OBJ_INDEX_DATA *obj_ind = get_obj_index( 10333 );
            if ( obj_ind != NULL )
            {
              obj = create_object( obj_ind, 0 );
              obj_to_char( obj, ch );
              equip_char( ch, obj, WEAR_HOLD );
            }
            }
	    if (!sysdata.WAIT_FOR_AUTH)
	      char_to_room( ch, get_room_index( ROOM_AUTH_START) );
	    else
	    {
	      char_to_room( ch, get_room_index( ROOM_AUTH_START ) );
	      ch->pcdata->auth_state = 3;
//	      xSET_BIT(ch->pcdata->flags, PCFLAG_UNAUTHED);
	    }
	}
	else
	if ( !IS_IMMORTAL(ch) && ch->pcdata->release_date > 0 && 
		ch->pcdata->release_date > current_time )
	{
/*	    if ( ch->in_room->vnum == 6
	    ||   ch->in_room->vnum == 8
	    ||   ch->in_room->vnum == 1206 )
		char_to_room( ch, ch->in_room );
	    else*/
	      char_to_room( ch, get_room_index(6) );
	}
	else
/*	if ( ch->in_room && ( IS_IMMORTAL( ch ) || !IS_SET( ch->in_room->room_flags, 
ROOM_PROTOTYPE ) ) )
	{
	    char_to_room( ch, ch->in_room );
	}
	else
	if ( IS_IMMORTAL(ch) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}*/
        char_to_room( ch, ch->in_room );


    if ( get_timer( ch, TIMER_SHOVEDRAG ) > 0 )
        remove_timer( ch, TIMER_SHOVEDRAG );

    if ( get_timer( ch, TIMER_PKILLED ) > 0 )
	remove_timer( ch, TIMER_PKILLED );

    if ( ch->plr_home != NULL )
    {
         char filename[256];
         FILE *fph;
         ROOM_INDEX_DATA *storeroom = ch->plr_home;
         OBJ_DATA *obj;
         OBJ_DATA *obj_next;
          
         for ( obj = storeroom->first_content; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            extract_obj( obj );
         }
            
         sprintf( filename, "%s%s.home", HOME_DIR, capitalize( ch->name ) );
         if ( ( fph = fopen( filename, "r" ) ) != NULL )
         {
            int iNest;
            bool found;
            OBJ_DATA *tobj, *tobj_next;
            
            rset_supermob(storeroom);
            for ( iNest = 0; iNest < MAX_NEST; iNest++ )
                rgObjNest[iNest] = NULL;
               
            found = TRUE;
            for ( ; ; )
            {
                char letter;
                char *word;
                letter = fread_letter( fph );
                if ( letter == '*' )
                {
                    fread_to_eol( fph );
                    continue;
                }
                if ( letter != '#' )
                {
                    bug( "Load_plr_home: # not found.", 0 );
                    bug( ch->name, 0 );
                    break;
                }
                word = fread_word( fph );
                if ( !str_cmp( word, "OBJECT" ) )       /* Objects      */
                  fread_obj  ( supermob, fph, OS_CARRY );
                else
                if ( !str_cmp( word, "END"    ) )       /* Done         */
                  break;
                else
                {
                    bug( "Load_plr_home: bad section.", 0 );
                    bug( ch->name, 0 );
                    break;
                }
            }

            fclose( fph );

            for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
            {
                tobj_next = tobj->next_content;
                obj_from_char( tobj );
                obj_to_room( tobj, storeroom, supermob );
            }

            release_supermob();

         }
    }
    xREMOVE_BIT(ch->act, PLR_FUSED);
    xREMOVE_BIT(ch->act, PLR_INVIS2);
    if ( !IS_IMMORTAL(ch) && ch->level < 50 && !IS_NPC(ch))
    {
       adjust_hiscore( "reputation", ch, calculate_rep( ch ), FALSE);
       adjust_hiscore( "bless", ch, ch->pcdata->bless, FALSE );
       adjust_hiscore( "pkill", ch, ch->pcdata->pkills, FALSE );
       adjust_hiscore( "mkill", ch, ch->pcdata->mkills, FALSE );
       adjust_hiscore( "gold", ch, ch->gold, FALSE );
       adjust_hiscore( "bank", ch, ch->balance, FALSE );
    }
    if (ch->in_room->vnum == 98 )
    do_defuse(ch, "" );
    de_equip_char(ch);
    ch->pdef = ch->patk = ch->edef = ch->eatk = ch->evasion = ch->critical = ch->accuracy = 0;
    re_equip_char( ch );
    update_skill( ch );
    ch->aurapowerup = -1;
    ch->auracolor = URANGE( -1, ch->auracolor, MAX_AURA );
    act( AT_ACTION, "$n has entered the game.", ch, NULL, NULL, TO_CANSEE );
        add_timer( ch, TIMER_GRAVITY, 60, NULL, 0 );
    if (IS_BIODROID(ch)) { xSET_BIT(ch->powerup, PU_LARVAE); }
    if (IS_ICER(ch)) { xSET_BIT(ch->powerup, PU_TRANSFORM1); }
    clear_mods(ch);
    if ( IS_ANDROID(ch))
    do_androidmod( ch, "" );
    ch->charge = 0;
    add_timer( ch, TIMER_HEAL, 20, NULL, 0 );
    add_timer( ch, TIMER_GRAVITY, 60, NULL, 0 );
    /*End of DBZ Remove Stuff*/
    if ( ch->pcdata->pet )
    {
           act( AT_ACTION, "$n returns to $s master from the Void.",
                      ch->pcdata->pet, NULL, ch, TO_NOTVICT );
           act( AT_ACTION, "$N returns with you to the realms.",
                        ch, NULL, ch->pcdata->pet, TO_CHAR );
    }         
    if ( IS_MUTANT(ch))
    add_mutate( ch, 1 );

    if(xIS_SET(ch->act, PLR_SKINNY) && IS_BUU(ch))
    {
             ch->mod_dex = 3;
             ch->mod_int = 3;
             ch->mod_str = 3;
             ch->mod_con = -5;
             ch->mod = 8.8;
    }
    else if(xIS_SET(ch->act, PLR_FAT) && IS_BUU(ch))
    {
             ch->mod_str = 3;
             ch->mod_int = -3;
             ch->mod_con = 5;
             ch->mod = 8.6;
    }

    if(xIS_SET(ch->act, PLR_ULTIMATE) && IS_BUU(ch))
    {
            if(IS_EVIL(ch))
	    {
                ch->mod = 9.77;
		do_suppress(ch, "0");
	    }
    }

    do_look( ch, "auto" );
    do_global_boards( ch, "" );
    mail_count(ch);

    if ( !ch->was_in_room && ch->in_room == get_room_index( ROOM_VNUM_TEMPLE ))
      	ch->was_in_room = get_room_index( ROOM_VNUM_TEMPLE );
    else if ( ch->was_in_room == get_room_index( ROOM_VNUM_TEMPLE ))
        ch->was_in_room = get_room_index( ROOM_VNUM_TEMPLE );
    else if ( !ch->was_in_room )
    	ch->was_in_room = ch->in_room;

    break;
    case CON_NOTE_TO:
        handle_con_note_to (d, argument);
        break;

    case CON_NOTE_SUBJECT:
        handle_con_note_subject (d, argument);
        break; /* subject */

    case CON_NOTE_EXPIRE:
        handle_con_note_expire (d, argument);
        break;

    case CON_NOTE_TEXT:
        handle_con_note_text (d, argument);
        break;
		
    case CON_NOTE_FINISH:
        handle_con_note_finish (d, argument);
        break;
    }

    return;
}

bool is_reserved_name( char *name )
{
  RESERVE_DATA *res;
  
  for (res = first_reserved; res; res = res->next)
    if ((*res->name == '*' && !str_infix(res->name+1, name)) ||
        !str_cmp(res->name, name))
      return TRUE;
  return FALSE;
}


/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name, bool newchar )
{
 /*
  * Names checking should really only be done on new characters, otherwise
  * we could end up with people who can't access their characters.  Would
  * have also provided for that new area havoc mentioned below, while still
  * disallowing current area mobnames.  I personally think that if we can
  * have more than one mob with the same keyword, then may as well have
  * players too though, so I don't mind that removal.  -- Alty
  */
  
     if ( is_reserved_name(name) && newchar )
	return FALSE;

     /*
      * Outdated stuff -- Alty
      */
/*     if ( is_name( name, "all auto immortal self someone god supreme demigod dog guard cityguard cat cornholio spock hicaine hithoric death ass fuck shit piss crap quit" ) )
       return FALSE;*/
 
    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

    if ( strlen(name) > 12 )
	return FALSE;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Code that followed here used to prevent players from naming
     * themselves after mobs... this caused much havoc when new areas
     * would go in...
     */

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    for ( ch = first_char; ch; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || !ch->desc )
	&&    ch->pcdata->filename
	&&   !str_cmp( name, ch->pcdata->filename ) )
	{
	    if ( fConn && ch->switched )
	    {
	      send_to_desc_color( "&RAlready playing.\n\r&YName&O: ", d );
	      d->connected = CON_GET_NAME;
	      if ( d->character )
	      {
		 /* clear descriptor pointer to get rid of bug message in log */
		 d->character->desc = NULL;
		 free_char( d->character );
		 d->character = NULL;
	      }
	      return BERR;
	    }
	    if ( fConn == FALSE )
	    {
		DISPOSE( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		/* clear descriptor pointer to get rid of bug message in log */
		d->character->desc = NULL;
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		do_look( ch, "auto" );
		act( AT_ACTION, "$n has reconnected.", ch, NULL, NULL, TO_CANSEE );
                xREMOVE_BIT(ch->powerup, PLR_BOOST);
                if (ch->in_room->vnum == 98)
                do_defuse( ch, "" );
		sprintf( log_buf, "%s@%s reconnected.", 
				ch->pcdata->filename, d->host );
		log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );
/*
		if ( ch->level < LEVEL_SAVIOR )
		  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->level );
*/
		d->connected = CON_PLAYING;
                /* Inform the character of a note in progress and the possbility of continuation! */
                if (ch->pcdata->in_progress)
                    send_to_char ("You have a note in progress. Type \"note write\" to continue it.\n\r",ch);
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name, bool kick )
{
    CHAR_DATA *ch;

    DESCRIPTOR_DATA *dold;
    int	cstate;

    for ( dold = first_descriptor; dold; dold = dold->next )
    {
	if ( dold != d
	&& (  dold->character || dold->original )
	&&   !str_cmp( name, dold->original
		 ? dold->original->pcdata->filename : 
		 dold->character->pcdata->filename ) )
	{
	    cstate = dold->connected;
	    ch = dold->original ? dold->original : dold->character;
	    if ( !ch->name
	    || ( cstate != CON_PLAYING && cstate != CON_EDITING ))
	    {
		write_to_buffer( d, "Already connected - try again.\n\r", 0 );
		sprintf( log_buf, "%s already connected.", 
				ch->pcdata->filename );
		log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
		return BERR;
	    }
	    if ( !kick )
	      return TRUE;
	    write_to_buffer( d, "Already playing... Kicking off old connection.\n\r", 0 );
	    write_to_buffer( dold, "Kicking off old connection... bye!\n\r", 0 );
	    close_socket( dold, FALSE );
	    /* clear descriptor pointer to get rid of bug message in log */
	    d->character->desc = NULL;
	    free_char( d->character );
	    d->character = ch;
	    ch->desc	 = d;
	    ch->timer	 = 0;
	    if ( ch->switched )
	      do_return( ch->switched, "" );
	    ch->switched = NULL;
	    send_to_char( "Reconnecting.\n\r", ch );
	    do_look( ch, "auto" );
	    act( AT_ACTION, "$n has reconnected, kicking off old link.",
	         ch, NULL, NULL, TO_CANSEE );
	    sprintf( log_buf, "%s@%s reconnected, kicking off old link.",
	             ch->pcdata->filename, d->host );
	    log_string_plus( log_buf, LOG_COMM, UMAX( sysdata.log_level, ch->level ) );
/*
	    if ( ch->level < LEVEL_SAVIOR )
	      to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->level );
*/
	    d->connected = cstate;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;

    /*
    if ( !ch
    ||   !ch->desc
    ||    ch->desc->connected != CON_PLAYING
    ||   !ch->was_in_room
    ||    ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;
	*/

    if ( !ch
    ||   !ch->desc 
    ||   ch->desc->connected != CON_PLAYING
    ||   !IS_IDLE(ch) )
    	return;

    /*
    if ( IS_IMMORTAL(ch) )
    	return;
    */

    ch->timer = 0;
    was_in_room = ch->was_in_room;
    char_from_room( ch );
    char_to_room( ch, was_in_room );
    ch->was_in_room = ch->in_room;
    /*
    ch->was_in_room	= NULL;
    */
    xREMOVE_BIT(ch->pcdata->flags, PCFLAG_IDLE );
    act( AT_ACTION, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

void write_to_pager( DESCRIPTOR_DATA *d, const char *txt, int length )
{
  int pageroffset;	/* Pager fix by thoric */

  if ( length <= 0 )
    length = strlen(txt);
  if ( length == 0 )
    return;
  if ( !d->pagebuf )
  {
    d->pagesize = MAX_STRING_LENGTH;
    CREATE( d->pagebuf, char, d->pagesize );
  }
  if ( !d->pagepoint )
  {
    d->pagepoint = d->pagebuf;
    d->pagetop = 0;
    d->pagecmd = '\0';
  }
  if ( d->pagetop == 0 && !d->fcommand )
  {
    d->pagebuf[0] = '\n';
    d->pagebuf[1] = '\r';
    d->pagetop = 2;
  }
  pageroffset = d->pagepoint - d->pagebuf;	/* pager fix (goofup fixed 08/21/97) */
  while ( d->pagetop + length >= d->pagesize )
  {
    if ( d->pagesize > 32000 )
    {
      bug( "Pager overflow.  Ignoring.\n\r" );
      d->pagetop = 0;
      d->pagepoint = NULL;
      DISPOSE(d->pagebuf);
      d->pagesize = MAX_STRING_LENGTH;
      return;
    }
    d->pagesize *= 2;
    RECREATE(d->pagebuf, char, d->pagesize);
  }
  d->pagepoint = d->pagebuf + pageroffset;	/* pager fix (goofup fixed 08/21/97) */
  strncpy(d->pagebuf+d->pagetop, txt, length);
  d->pagetop += length;
  d->pagebuf[d->pagetop] = '\0';
  return;
}

/*
 * Function to strip off the "a" or "an" or "the" or "some" from an object's
 * short description for the purpose of using it in a sentence sent to
 * the owner of the object.  (Ie: an object with the short description
 * "a long dark blade" would return "long dark blade" for use in a sentence
 * like "Your long dark blade".  The object name isn't always appropriate
 * since it contains keywords that may not look proper.		-Thoric
 */
char *myobj( OBJ_DATA *obj )
{
    if ( !str_prefix("a ", obj->short_descr) )
	return obj->short_descr + 2;
    if ( !str_prefix("an ", obj->short_descr) )
	return obj->short_descr + 3;
    if ( !str_prefix("the ", obj->short_descr) )
	return obj->short_descr + 4;
    if ( !str_prefix("some ", obj->short_descr) )
	return obj->short_descr + 5;
    return obj->short_descr;
}


char *obj_short( OBJ_DATA *obj )
{
    static char buf[MAX_STRING_LENGTH];

    if ( obj->count > 1 )
    {
	sprintf( buf, "%s (%d)", obj->short_descr, obj->count );
	return buf;
    }
    return obj->short_descr;
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */

void ch_printf_color(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_char_color(buf, ch);
}
 
void pager_printf_color(CHAR_DATA *ch, char *fmt, ...)
{
     char buf[MAX_STRING_LENGTH*2];
     va_list args;
 
     va_start(args, fmt);
     vsprintf(buf, fmt, args);
     va_end(args);
 
     send_to_pager_color(buf, ch);
}

#define NAME(ch)        (IS_NPC(ch) ? ch->short_descr :  ch->name)

char *act_string(const char *format, CHAR_DATA *to, CHAR_DATA *ch,
		 const void *arg1, const void *arg2, int flags)
{
  static char * const he_she  [] = { "it",  "he",  "she" };
  static char * const him_her [] = { "it",  "him", "her" };
  static char * const his_her [] = { "its", "his", "her" };
  static char buf[MAX_STRING_LENGTH];
  char fname[MAX_INPUT_LENGTH];
//  char temp[MAX_STRING_LENGTH];
  char *point = buf;
  const char *str = format;
  const char *i;
  CHAR_DATA *vch = (CHAR_DATA *) arg2;
  OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
  OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;

  if ( str[0] == '$' )
  	DONT_UPPER = FALSE;

  while ( *str != '\0' )
  {
    if ( *str != '$' )
    {
      *point++ = *str++;
      continue;
    }
    ++str;
    if ( !arg2 && *str >= 'A' && *str <= 'Z' )
    {
      bug( "Act: missing arg2 for code %c:", *str );
      bug( format );
      i = " <@@@> ";
    }
    else
    {
      switch ( *str )
      {
      default:  bug( "Act: bad code %c.", *str);
		i = " <@@@> ";						break;
      case 't': i = (char *) arg1;					break;
      case 'T': i = (char *) arg2;					break;
      case 'n':
                  i = (to ? PERS ( ch, to, TRUE ): NAME ( ch ) );
              break;
      case 'N':
                   i = (to ? PERS ( vch, to, TRUE ) : NAME( vch ) );
              break;

      case 'e': if (ch->sex > 2 || ch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", ch->name,
		      ch->sex);
		  i = "it";
		}
		else
		  i = he_she [URANGE(0,  ch->sex, 2)];
		break;
      case 'E': if (vch->sex > 2 || vch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", vch->name,
		      vch->sex);
		  i = "it";
		}
		else
		  i = he_she [URANGE(0, vch->sex, 2)];
		break;
      case 'm': if (ch->sex > 2 || ch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", ch->name,
		      ch->sex);
		  i = "it";
		}
		else
		  i = him_her[URANGE(0,  ch->sex, 2)];
		break;
      case 'M': if (vch->sex > 2 || vch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", vch->name,
		      vch->sex);
		  i = "it";
		}
		else
		  i = him_her[URANGE(0, vch->sex, 2)];
		break;
      case 's': if (ch->sex > 2 || ch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", ch->name,
		      ch->sex);
		  i = "its";
		}
		else
		  i = his_her[URANGE(0,  ch->sex, 2)];
		break;
      case 'S': if (vch->sex > 2 || vch->sex < 0)
		{
		  bug("act_string: player %s has sex set at %d!", vch->name,
		      vch->sex);
		  i = "its";
		}
		else
		  i = his_her[URANGE(0, vch->sex, 2)];
		break;
      case 'q': i = (to == ch) ? "" : "s";				break;
      case 'Q': i = (to == ch) ? "your" :
		    his_her[URANGE(0,  ch->sex, 2)];			break;
      case 'p': i = (!to || can_see_obj(to, obj1)
		  ? obj_short(obj1) : "something");			break;
      case 'P': i = (!to || can_see_obj(to, obj2)
		  ? obj_short(obj2) : "something");			break;
      case 'd':
        if ( !arg2 || ((char *) arg2)[0] == '\0' )
          i = "door";
        else
        {
          one_argument((char *) arg2, fname);
          i = fname;
        }
        break;
      }
    }
    ++str;
    while ( (*point = *i) != '\0' )
      ++point, ++i;
  
   /*  #0  0x80c6c62 in act_string (
    format=0x81db42e "$n has reconnected, kicking off old link.", to=0x0, 
    ch=0x94fcc20, arg1=0x0, arg2=0x0, flags=2) at comm.c:2901 */
  }
  strcpy(point, "\n\r");
  if ( !DONT_UPPER )
     buf[0] = UPPER(buf[0]);
  return buf;
}
#undef NAME
  
void act( sh_int AType, const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    char *txt;
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;

    /*
     * Discard null and zero-length messages.
     */
    if ( !format || format[0] == '\0' )
	return;

    if ( !ch )
    {
	bug( "Act: null ch. (%s)", format );
	return;
    }

    if ( !ch->in_room )
      to = NULL;
    else if ( type == TO_CHAR )
      to = ch;
    else
      to = ch->in_room->first_person;

    /*
     * ACT_SECRETIVE handling
     */
    if ( IS_NPC(ch) && xIS_SET(ch->act, ACT_SECRETIVE) && type != TO_CHAR )
	return;

    if ( type == TO_VICT )
    {
	if ( !vch )
	{
	    bug( "Act: null vch with TO_VICT." );
	    bug( "%s (%s)", ch->name, format );
	    return;
	}
	if ( !vch->in_room )
	{
	    bug( "Act: vch in NULL room!" );
	    bug( "%s -> %s (%s)", ch->name, vch->name, format );
	    return;
	}
	to = vch;
/*	to = vch->in_room->first_person;*/
    }

    if ( MOBtrigger && type != TO_CHAR && type != TO_VICT && to )
    {
      OBJ_DATA *to_obj;
      
      txt = act_string(format, NULL, ch, arg1, arg2, STRING_IMM);
      if ( HAS_PROG(to->in_room, ACT_PROG) )
        rprog_act_trigger(txt, to->in_room, ch, (OBJ_DATA *)arg1, (void *)arg2);
      for ( to_obj = to->in_room->first_content; to_obj;
            to_obj = to_obj->next_content )
        if ( HAS_PROG(to_obj->pIndexData, ACT_PROG) )
          oprog_act_trigger(txt, to_obj, ch, (OBJ_DATA *)arg1, (void *)arg2);
    }

    /* Anyone feel like telling me the point of looping through the whole
       room when we're only sending to one char anyways..? -- Alty */
    for ( ; to; to = (type == TO_CHAR || type == TO_VICT)
                     ? NULL : to->next_in_room )
    {
	if ((!to->desc 
	&& (  IS_NPC(to) && !HAS_PROG(to->pIndexData, ACT_PROG) ))
	||   !IS_AWAKE(to) )
	    continue;

/*        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;
        if ( type == TO_CANSEE && ( to == ch ||
            (!IS_IMMORTAL(to) && !IS_NPC(ch) && (xIS_SET(ch->act, PLR_WIZINVIS)
            && (get_trust(to) < (ch->pcdata ? ch->pcdata->wizinvis : 0) ) ) ) ) )
            continue;

        if ( IS_IMMORTAL(to) )
            txt = act_string (format, to, ch, arg1, arg2, STRING_IMM);
        else
            txt = act_string (format, to, ch, arg1, arg2, STRING_NONE);
*/


	if( type == TO_CHAR )
	{
	   if( to != ch )
	      continue;

	   if( !is_same_map( ch, to ) )
	      continue;
	}
	if ( type == TO_VICT && ( to != vch || to == ch ) )
	    continue;
	if( type == TO_ROOM )
	{
	   if( to == ch )
	      continue;

	   if( !is_same_map( ch, to ) )
	      continue;
        }
	if( type == TO_NOTVICT )
	{
	   if( to == ch || to == vch )
	      continue;

	   if( !is_same_map( ch, to ) )
	      continue;
	}
	if( type == TO_CANSEE )
	{
 	   if( to == ch )
		continue;

	   if( IS_IMMORTAL(ch) && IS_PLR_FLAG( ch, PLR_WIZINVIS ) )
	   {
 		if( to->level < ch->pcdata->wizinvis )
		continue;
	   }

	   if( !is_same_map( ch, to ) )
	      continue;
	}

        if ( IS_IMMORTAL(to) )
            txt = act_string (format, to, ch, arg1, arg2, STRING_IMM);
        else
       	    txt = act_string (format, to, ch, arg1, arg2, STRING_NONE);

	if (to->desc)
	{
        set_char_color(AType, to);
        send_to_char( txt, to );
	}
	if (MOBtrigger)
        {
          /* Note: use original string, not string with ANSI. -- Alty */
	  mprog_act_trigger( txt, to, ch, (OBJ_DATA *)arg1, (void *)arg2 );
        }
    }
    MOBtrigger = TRUE;
    return;
}

void do_name( CHAR_DATA *ch, char *argument )
{
  char fname[1024];
  struct stat fst;
  CHAR_DATA *tmp;

  if ( !NOT_AUTHED(ch) || ch->pcdata->auth_state != 2)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  argument[0] = UPPER(argument[0]);

  if (!check_parse_name(argument, TRUE))
  {
    send_to_char("Illegal name, try another.\n\r", ch);
    return;
  }

  if (!str_cmp(ch->name, argument))
  {
    send_to_char("That's already your name!\n\r", ch);
    return;
  }

  for ( tmp = first_char; tmp; tmp = tmp->next )
  {
    if (!str_cmp(argument, tmp->name))
    break;
  }

  if ( tmp )
  {
    send_to_char("That name is already taken.  Please choose another.\n\r", ch);
    return;
  }

  sprintf( fname, "%s%c/%s", PLAYER_DIR, tolower(argument[0]),
                        capitalize( argument ) );
  if ( stat( fname, &fst ) != -1 )
  {
    send_to_char("That name is already taken.  Please choose another.\n\r", ch);
    return;
  }

  STRFREE( ch->name );
  ch->name = STRALLOC( argument );
  STRFREE( ch->pcdata->filename );
  ch->pcdata->filename = STRALLOC( argument ); 
  send_to_char("Your name has been changed.  Please apply again.\n\r", ch);
  ch->pcdata->auth_state = 1;
  return;
}
  
char *breath_meter( CHAR_DATA *ch)
{
        int percent;
        static char pbuf[MAX_STRING_LENGTH];
        percent =  URANGE(0, ch->breath, 100);
        if (percent >= 100)
                        sprintf (pbuf, "&C||||||||||");
        else if (percent >= 90)
                        sprintf (pbuf, "&C||||||||| ");
        else if (percent >= 80)
                        sprintf (pbuf, "&C||||||||  ");
        else if (percent >= 70)
                        sprintf (pbuf, "&C|||||||   ");
        else if (percent >= 60)
                        sprintf (pbuf, "&C||||||    ");
        else if (percent >= 50)
                        sprintf (pbuf, "&C|||||     ");
        else if (percent >= 40)
                        sprintf (pbuf, "&C||||      ");
        else if (percent >= 30)
                        sprintf (pbuf, "&C|||       ");
        else if (percent >= 20)
                        sprintf (pbuf, "&C||        ");
        else if (percent >= 10)
                        sprintf (pbuf, "&C|         ");
        else
                        sprintf (pbuf, "&C          ");

        return pbuf;
}



char *default_fprompt( CHAR_DATA *ch )
{
  static char buf [MAX_STRING_LENGTH];
//  static char buf[60];
  strcpy( buf,"" );

  if (xIS_SET(ch->act,PLR_ONMAP) && get_mapbelow(ch->map) == TRUE && !IS_ANDROID(ch) && !IS_ICER(ch))
   strcpy(buf, "&z<&C%B&z>%L");
  strcat(buf, "&z<&cLife&W [&Y%h&W] &cKi&W [&Y%m&W] &cPowerlevel&W [&Y%X&W/&Y%D&W] &cStyle&W[&Y%S&W] &cEnemy &W[&Y%E&W]  %s &w%Z");
  strcat(buf, "&z>&w");
  return buf;
}

char *hit_epercent(CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        int per, per2;
        static char pbuf[MAX_STRING_LENGTH];
        if (( victim = who_fighting( ch ) ) != NULL)
        {
                per = per2 = 0;
	    if (victim->max_hit > 0)
	    {
	        per =  (100 * victim->hit) / victim->max_hit;
                per2 = victim->hit;

	    }
        if ((per <= 100) && (per >= 65)) sprintf(pbuf, "&G%d&w", per2);
   else if ((per <= 64 )&& (per >= 25)) sprintf(pbuf, "&Y%d&w", per2);
   else if ((per <= 24 ) && (per >= -15)) sprintf(pbuf, "&R%d&w", per2);
   }
   return pbuf;
}

char *show_cskills(CHAR_DATA *ch)
{
    static char buf[MSL];
 
    buf[0] = 0;

   if ( !xIS_EMPTY( ch->skill ) )
   {
    strcat( buf, "&z[" );
    if (xIS_SET(ch->skill, SKILL_ENRAGE) )
      strcat(buf, " &RE&rN");
    if (xIS_SET(ch->skill, SKILL_BERSERK) )
      strcat(buf, " &YB&OE");
    if (xIS_SET(ch->skill, SKILL_OVERDRIVE) )
      strcat(buf, " &GO&BD");
    if (xIS_SET(ch->skill, SKILL_BBARRIER) )
      strcat(buf, " &BB&bB");
    if (xIS_SET(ch->skill, SKILL_KYODAIKA) )
      strcat(buf, " &gK&cY");
    if (xIS_SET(ch->skill, SKILL_SOULCRY) )
      strcat(buf, " &pS&zC");
    if (xIS_SET(ch->skill, SKILL_TIMEFREEZE) )
      strcat(buf, " &bT&OF");
    if (xIS_SET(ch->skill, SKILL_PBARRIER) )
      strcat(buf, " &PP&WB");
    if (xIS_SET(ch->skill, SKILL_REGENERATION) )
      strcat(buf, " &CR&cG");
    if (xIS_SET(ch->skill, SKILL_ZANZOKEN) )
      strcat(buf, " &WZ&zN");
    if (xIS_SET(ch->skill, SKILL_VSTANCE) )
      strcat(buf, " &PV&pS");
    if (xIS_SET(ch->skill, SKILL_ACCURACY) )
      strcat(buf, " &GA&gC");
    strcat( buf, " &z]" );
   }
 return buf;
}

char *default_prompt( CHAR_DATA *ch )
{
  static char buf [MAX_STRING_LENGTH];
//  static char buf[60];
  strcpy( buf,"" );

   strcpy(buf, "&z<");
  if (xIS_SET(ch->act,PLR_ONMAP) && get_mapbelow(ch->map) == TRUE && !IS_ANDROID(ch) && !IS_ICER(ch))
   strcat(buf, "&cBreath &z[&C%B&z] ");
  strcat(buf, "&cLife&W [&Y%h&W] &cKi&W [&Y%m&W] &cPowerlevel&W [&Y%X&W/&Y%D&W] &cStyle&W[&Y%S&W] %s &w%Z");
  strcat(buf, "&z>&w");

  return buf;
}

int getcolor(char clr)
{
  static const char colors[16] = "xrgObpcwzRGYBPCW";
  int r;
  
  for ( r = 0; r < 16; r++ )
    if ( clr == colors[r] )
      return r;
  return -1;
}

void display_prompt( DESCRIPTOR_DATA *d )
{
  CHAR_DATA *ch = d->character;
  CHAR_DATA *och = (d->original ? d->original : d->character);
  CHAR_DATA *victim;
  bool ansi = (!IS_NPC(och) && xIS_SET(och->act, PLR_ANSI));
  const char *prompt;
  const char *helpstart = "<Type HELP START>";
  char buf[MAX_STRING_LENGTH];
  char *pbuf = buf;
  int stat, percent;

  if ( !ch )
  {
    bug( "display_prompt: NULL ch" );
    return;
  }

  if ( !IS_NPC(ch) && !xIS_SET(ch->pcdata->flags, PCFLAG_HELPSTART ) )
	prompt = helpstart;
  else if ( !IS_NPC(ch) && ch->substate != SUB_NONE && ch->pcdata->subprompt
  &&   ch->pcdata->subprompt[0] != '\0' )
    prompt = ch->pcdata->subprompt;
  else if (IS_NPC (ch) || (!ch->fighting && (!ch->pcdata->prompt
                || !*ch->pcdata->prompt) ) )
    prompt = default_prompt (ch);
  else if ( ch->fighting )
  {
        if ( !ch->pcdata->fprompt || !*ch->pcdata->fprompt )
                prompt = default_fprompt ( ch );
        else
                prompt = ch->pcdata->fprompt;
  }
  else
    prompt = ch->pcdata->prompt;
  if ( ansi )
  {
    strcpy( pbuf, ANSI_RESET );
    d->prevcolor = 0x08;
    pbuf += 4;
  }
  /* Clear out old color stuff */
  for ( ; *prompt; prompt++ )
  {
    /*
     * '%' = prompt commands
     * Note: foreground changes will revert background to 0 (black)
     */
    if( *prompt != '%' )
    {
      *(pbuf++) = *prompt;
      continue;
    }
    ++prompt;
    if ( !*prompt )
      break;
    if ( *prompt == *(prompt-1) )
    {
      *(pbuf++) = *prompt;
      continue;
    }
    switch(*(prompt-1))
    {
    default:
      bug( "Display_prompt: bad command char '%c'.", *(prompt-1) );
      break;
    case '%':
      *pbuf = '\0';
      stat = 0x80000000;
      switch(*prompt)
      {
      case '%':
	*pbuf++ = '%';
	*pbuf = '\0';
	break;
      case 'a':
	if ( ch->level >= 10 )
	  stat = ch->alignment;
	else if ( IS_GOOD(ch) )
	  strcpy(pbuf, "good");
	else if ( IS_EVIL(ch) )
	  strcpy(pbuf, "evil");
	else
	  strcpy(pbuf, "neutral");
	break;
        case 'C':  /* Tank */
	  if ( !IS_IMMORTAL( ch ) ) break;
          if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
             strcpy( pbuf, "N/A" );
          else if(!victim->fighting||(victim = victim->fighting->who)==NULL)
             strcpy( pbuf, "N/A" );
          else {
              if ( victim->max_hit > 0 )
                    percent = (100 * victim->hit) / victim->max_hit;
              else
                    percent = -1;
                   if (percent >= 100)
                       strcpy (pbuf, "perfect health");
                   else if (percent >= 90)
                       strcpy (pbuf, "slightly scratched");
                   else if (percent >= 80)
                       strcpy (pbuf, "few bruises");
                    else if (percent >= 70)
                       strcpy (pbuf, "some cuts");
                    else if (percent >= 60)
                       strcpy (pbuf, "several wounds");
                    else if (percent >= 50)
                       strcpy (pbuf, "nasty wounds");
                    else if (percent >= 40)
                       strcpy (pbuf, "bleeding freely");
                    else if (percent >= 30)
                       strcpy (pbuf, "covered in blood");
                    else if (percent >= 20)
                       strcpy (pbuf, "leaking guts");
                    else if (percent >= 10)
                       strcpy (pbuf, "almost dead");
                    else
                       strcpy (pbuf, "DYING");
             }
          break;
        case 'c':
	  if ( !IS_IMMORTAL( ch ) ) break;
          if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
             strcpy( pbuf, "N/A" );
          else {
              if ( victim->max_hit > 0 )
                    percent = (100 * victim->hit) / victim->max_hit;
              else
                    percent = -1;
                if (percent >= 100)
                       strcpy (pbuf, "perfect health");
                    else if (percent >= 90)
                       strcpy (pbuf, "slightly scratched");
                    else if (percent >= 80)
                       strcpy (pbuf, "few bruises");
                    else if (percent >= 70)
                       strcpy (pbuf, "some cuts");
                    else if (percent >= 60)
                       strcpy (pbuf, "several wounds");
                    else if (percent >= 50)
                       strcpy (pbuf, "nasty wounds");
                    else if (percent >= 40)
                       strcpy (pbuf, "bleeding freely");
                    else if (percent >= 30)
                       strcpy (pbuf, "covered in blood");
                    else if (percent >= 20)
                       strcpy (pbuf, "leaking guts");
                    else if (percent >= 10)
                       strcpy (pbuf, "almost dead");
                    else
                       strcpy (pbuf, "DYING");
           }
          break;
      case 'E':
        strcpy(pbuf, hit_epercent(ch));
        break;
      case 's':
        strcpy(pbuf, show_cskills(ch));
        break;
      case 'h':
	stat = ch->hit;
	break;
      case 'H':
        stat = 150;
	break;   
      case 'L':
         sprintf( pbuf, "\n" );
         break;
      case 'B':
        strcpy(pbuf, breath_meter(ch));
        break;
      case 't':
        if ( xIS_SET(ch->act, PLR_TOGGLE) ) strcpy( pbuf, "S" );
        else strcpy( pbuf, "K" );
      
         break;
      case 'm':
	if ( IS_VAMPIRE(ch) )
	  stat = 0;
	else
	  stat = ch->mana;
	break;
      case 'M':
	if ( IS_VAMPIRE(ch) )
	  stat = 0;
	else
	  stat = ch->max_mana;
	break;
        case 'N': /* Tank */
	  if ( !IS_IMMORTAL(ch) ) break;
          if ( !ch->fighting || ( victim = ch->fighting->who ) == NULL )
            strcpy( pbuf, "N/A" );
          else if(!victim->fighting||(victim=victim->fighting->who)==NULL)
            strcpy( pbuf, "N/A" );
          else {
            if ( ch == victim )
                    strcpy ( pbuf, "You" );
            else if ( IS_NPC(victim) )
                    strcpy ( pbuf, victim->short_descr );
            else
                    strcpy ( pbuf, victim->name );
                pbuf[0] = UPPER( pbuf[0] );
          }
          break;
        case 'n':
	  if ( !IS_IMMORTAL(ch) ) break;
          if (!ch->fighting || (victim = ch->fighting->who) == NULL )
            strcpy( pbuf, "N/A" );
          else {
            if ( ch == victim )
                    strcpy ( pbuf, "You" );
            else if ( IS_NPC(victim) )
                    strcpy ( pbuf, victim->short_descr );
            else
                    strcpy ( pbuf, victim->name );
            pbuf[0] = UPPER( pbuf[0] );
          }
          break;
      case 'T':
        if      ( time_info.hour <  5 ) strcpy( pbuf, "night" );
        else if ( time_info.hour <  6 ) strcpy( pbuf, "dawn" );
        else if ( time_info.hour < 19 ) strcpy( pbuf, "day" );
        else if ( time_info.hour < 21 ) strcpy( pbuf, "dusk" );
        else                            strcpy( pbuf, "night" );
        break;
      case 'u':
        if (IS_IMMORTAL(ch))
	stat = num_descriptors;
	break;
      case 'U':
	stat = sysdata.maxplayers;
	break;
      case 'v':
	stat = ch->move;
	break;
      case 'V':
	stat = ch->max_move;
	break;
      case 'g':
	stat = ch->gold;
	break;
      case 'G':
        sprintf( pbuf, "%s", num_punct(ch->gold));

        break;
      case 'Z':
        if (get_timer( ch, TIMER_CLOAK) > 0)
        sprintf( pbuf, " (Invisible) ");

        break;

      case 'r':
	if ( IS_IMMORTAL(och) )
	  stat = ch->in_room->vnum;
	break;
      case 'F':
	if ( IS_IMMORTAL( och ) )
	  sprintf( pbuf, "%s", flag_string( ch->in_room->room_flags, r_flags) );
	break;
      case 'R':
	if ( xIS_SET(och->act, PLR_ROOMVNUM) )
	  sprintf(pbuf, "<#%d> ", ch->in_room->vnum);
	break;
      case 'd':
        stat = get_curr_pl(ch);
        break;
      case 'D':
       if (!xIS_SET(ch->powerup, PU_SUPPRESS))
        sprintf( pbuf, "%s", num_punct(ch->exp * ch->mod));
       else
        sprintf( pbuf, "%s", num_punct(ch->suppress));
        break;
      case 'x':
	stat = ch->exp;
	break;
      case 'X':
        sprintf( pbuf, "%s", num_punct(ch->exp));
	break;
      case 'o':         /* display name of object on auction */
        if ( auction->item )
           strcpy( pbuf, auction->item->name );
        break;
      case 'S':
        if      ( ch->style == STYLE_BERSERK )    strcpy( pbuf, "B" );
        else if ( ch->style == STYLE_AGGRESSIVE ) strcpy( pbuf, "A" );
        else if ( ch->style == STYLE_DEFENSIVE )  strcpy( pbuf, "D" );
        else if ( ch->style == STYLE_EVASIVE )    strcpy( pbuf, "E" );
        else                                      strcpy( pbuf, "S" );
	break;
      case 'i':
	if ( (!IS_NPC(ch) && xIS_SET(ch->act, PLR_WIZINVIS)) ||
	      (IS_NPC(ch) && xIS_SET(ch->act, ACT_MOBINVIS)) )
	  sprintf(pbuf, "(Invis %d) ", (IS_NPC(ch) ? ch->mobinvis : ch->pcdata->wizinvis));
	break;

      case 'I':
	stat = (IS_NPC(ch) ? (xIS_SET(ch->act, ACT_MOBINVIS) ? ch->mobinvis : 0)
	     : (xIS_SET(ch->act, PLR_WIZINVIS) ? ch->pcdata->wizinvis : 0));
	break;
      }
      if ( stat != 0x80000000 )
	sprintf(pbuf, "%d", stat);
      pbuf += strlen(pbuf);
      break;
    }
  }
  *pbuf = '\0';
  send_to_char( buf, ch );
  return;
}

void set_pager_input( DESCRIPTOR_DATA *d, char *argument )
{
  while ( isspace(*argument) )
    argument++;
  d->pagecmd = *argument;
  return;
}

bool pager_output( DESCRIPTOR_DATA *d )
{
  register char *last;
  CHAR_DATA *ch;
  int pclines;
  register int lines;
  bool ret;

  if ( !d || !d->pagepoint || d->pagecmd == -1 )
    return TRUE;
  ch = d->original ? d->original : d->character;
  pclines = UMAX(ch->pcdata->pagerlen, 5) - 1;
  switch(LOWER(d->pagecmd))
  {
  default:
    lines = 0;
    break;
  case 'b':
    lines = -1-(pclines*2);
    break;
  case 'r':
    lines = -1-pclines;
    break;
  case 'n':
    lines = 0;
    pclines = 0x7FFFFFFF;	/* As many lines as possible */
    break;
  case 'q':
    d->pagetop = 0;
    d->pagepoint = NULL;
    flush_buffer(d, TRUE);
    DISPOSE(d->pagebuf);
    d->pagesize = MAX_STRING_LENGTH;
    return TRUE;
  }
  while ( lines < 0 && d->pagepoint >= d->pagebuf )
    if ( *(--d->pagepoint) == '\n' )
      ++lines;
  if ( *d->pagepoint == '\n' && *(++d->pagepoint) == '\r' )
      ++d->pagepoint;
  if ( d->pagepoint < d->pagebuf )
    d->pagepoint = d->pagebuf;
  for ( lines = 0, last = d->pagepoint; lines < pclines; ++last )
    if ( !*last )
      break;
    else if ( *last == '\n' )
      ++lines;
  if ( *last == '\r' )
    ++last;
  if ( last != d->pagepoint )
  {
    if ( !write_to_descriptor(d->descriptor, d->pagepoint,
          (last-d->pagepoint)) )
      return FALSE;
    d->pagepoint = last;
  }
  while ( isspace(*last) )
    ++last;
  if ( !*last )
  {
    d->pagetop = 0;
    d->pagepoint = NULL;
    flush_buffer(d, TRUE);
    DISPOSE(d->pagebuf);
    d->pagesize = MAX_STRING_LENGTH;
    return TRUE;
  }
  d->pagecmd = -1;
   if( xIS_SET( ch->act, PLR_ANSI ) )
      if( write_to_descriptor( d->descriptor, ANSI_LBLUE, 0 ) == FALSE )
	   return FALSE;
  if ( (ret=write_to_descriptor(d->descriptor,
	"(C)ontinue, (N)on-stop, (R)efresh, (B)ack, (Q)uit: [C] ", 0)) == FALSE )
	return FALSE;
  if ( xIS_SET( ch->act, PLR_ANSI ) )
  {
      char buf[32];

	sprintf( buf, "%s", color_str( d->pagecolor, ch ) );
      ret = write_to_descriptor( d->descriptor, buf, 0 );
  }
  return ret;
}


#ifdef WIN32

void shutdown_mud( char *reason );

void bailout(void)
{
    echo_to_all( AT_IMMORT, "MUD shutting down by system operator NOW!!", ECHOTAR_ALL );
    shutdown_mud( "MUD shutdown by system operator" );
    log_string ("MUD shutdown by system operator");
    Sleep (5000);		/* give "echo_to_all" time to display */
    mud_down = TRUE;		/* This will cause game_loop to exit */
    service_shut_down = TRUE;	/* This will cause characters to be saved */
    fflush(stderr);
    return;
}

#endif



