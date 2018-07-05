/******************************************************************
 *                [D]ragon [B]all [Z] [C]hronicles                *
 *----------------------------------------------------------------*
 * DBZB, DBZDL, DBZC versions done by: Seventeen and Vegeta       *
 *----------------------------------------------------------------*
 * All modifications made by Seventeen and Vegeta are sole        *
 * property of them. Credit must be given if any part is ripped   *
 * or used from this version or any other version of DBZC.        *
 *****************************************************************/

#define FINGERCODE /* Do not remove, used to interact with other snippets! - Samson 10-18-98 
*/

#ifndef FCLOSE
   #define FCLOSE(fp)  fclose(fp); fp=NULL;
#endif

#ifndef GET_TIME_PLAYED
   #define GET_TIME_PLAYED(ch)     (((ch)->played + (current_time - (ch)->logon)) / 3600)
#endif
/* KOTH Defines */
#define KOTH_PREP_START    249
#define KOTH_START         number_range(153, 157)
#define KOTH_LOSER_ROOM    151
#define KOTH_HEALING_ROOM  152
/* Local Functions */
long long int max_pl_char	args( ( CHAR_DATA *ch ) );
int	calculatelevel		args( ( CHAR_DATA *ch ) );
char    *get_skillname          args( ( CHAR_DATA *ch, int sn ) );
char  	*get_skillname2		args( ( CHAR_DATA *ch, int sn ) );
bool	is_clanned		args( ( CHAR_DATA *ch ) );
ENTRANCE_DATA 	*checkforentrance args( ( int vnum ) );
DOCK_DATA *get_dock		args( ( char *name ) );
char   *get_dockname		args( ( short x, short y, short z, short d ) );
void	load_docks		args( ( void ) );
int     convert_pos		args( ( int number ) );
void	destroy_area		args( ( CHAR_DATA *victim ) );
void    planet_support		args( ( CHAR_DATA *ch ) );
void	clear_mutateflags	args( ( CHAR_DATA *ch ) );
long long int	calc_power	args( ( CHAR_DATA *ch ) );
bool	is_fused	args( ( CHAR_DATA *ch ) );
void    clear_mods      args( ( CHAR_DATA *ch ) );
bool	controlling_ki	args( ( CHAR_DATA *ch ) );
void	release_ki	args( ( CHAR_DATA *ch ) );
char	*hisherit	args( ( short gender ) );
char    *himherit       args( ( short gender ) );
char    *hesheit        args( ( short gender ) );
int	pl_dmg		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	warcry		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	getsamemap	args( ( int map ) );
void	showblockmsg	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	showlckmsg	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	showconmsg	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	add_damage	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
int	add_kidamage	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
int	check_extra	args( ( int ch, int vic, int div ) );
void	consume_ki	args( ( CHAR_DATA *ch, const char *name ) );
void 	waitstate	args( ( CHAR_DATA *ch, const char *name ) );
OBJ_DATA *get_db	args( ( int vnum ) );
bool 	check_spec	args( ( CHAR_DATA *ch ) );
bool	has_dragonball	args( ( CHAR_DATA *ch ) );
int	getsamemap	args( ( int map	) );
CHAR_DATA *check_map	args( ( CHAR_DATA *ch, char *argument, int gsn ) );
bool 	is_kistunned	args( ( CHAR_DATA *ch ) );
bool 	is_awake	args( ( CHAR_DATA *ch ) );
void 	clear_mods 	args( ( CHAR_DATA *ch ) );
void 	add_mutate	args( ( CHAR_DATA *ch, int stage ) );
void 	do_finalflashbuild	args( ( CHAR_DATA *ch, char *argument ) );
void 	do_hellsflashbuild  	args( ( CHAR_DATA *ch, char *argument ) );
void 	do_rocketpunchbuild 	args( ( CHAR_DATA *ch, char *argument ) );
bool 	get_mapabove	args( ( int map ) );
bool 	get_mapmiddle	args( ( int map ) );
bool 	get_mapbelow	args( ( int map ) );
bool 	can_attack	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int 	getmapbelow	args( ( int map ) );
int 	getmapabove	args( ( int map ) );
void 	damage_area	args( ( short map, short x, short y ) );
bool 	can_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    db_update	args( ( void ) );
void    income_update   args( ( void ) );
double 	sqrt    	args( (double Nbr) );
void 	mobile_update	args( ( void ) );
void 	space_update    args( ( void ) );
void 	char_update	args( ( void ) );
void 	time_update	args( ( void ) );
void 	moon_update	args( ( void ) );
int 	find_fighters   args( ( void ) );
int 	dbtimer;
int 	dbtimer2;
int 	dbtimer3;
int 	redscore;
int 	bluescore;
bool    ctf;

DECLARE_DO_FUN( do_createclan	);
DECLARE_DO_FUN( do_koth		);
DECLARE_DO_FUN( do_null		);
DECLARE_DO_FUN( do_slevel	);
DECLARE_DO_FUN( do_mpzecho	);
DECLARE_DO_FUN( do_mudstatus	);
DECLARE_DO_FUN( do_docks	);
DECLARE_DO_FUN( do_setdock	);
DECLARE_DO_FUN( do_dockstat	);
DECLARE_DO_FUN( do_coordinates );
DECLARE_DO_FUN( do_addroom	);
/* Organizing every new stuff in the MUD */
DECLARE_DO_FUN( do_gamble	);
DECLARE_DO_FUN( do_absorption	);
DECLARE_DO_FUN( do_ghostattack	);
DECLARE_DO_FUN( do_search	);
DECLARE_DO_FUN( do_mapresets	);
DECLARE_DO_FUN( do_mreset	);
DECLARE_DO_FUN( do_chargeto	);
DECLARE_DO_FUN( do_clanset	);
DECLARE_DO_FUN( do_checkradar	);
DECLARE_DO_FUN( do_run		);
DECLARE_DO_FUN( do_maplegend	);
DECLARE_DO_FUN( do_maplegend2	);
DECLARE_DO_FUN( do_mapsize	);
DECLARE_DO_FUN( do_mapedit	);
DECLARE_DO_FUN( do_survey	);
DECLARE_DO_FUN( do_coords	);
DECLARE_DO_FUN( do_landmarks	);
DECLARE_DO_FUN( do_setexit	);
DECLARE_DO_FUN( do_setmark	);
DECLARE_DO_FUN( do_icq_number );  /* User can enter icq# for finger - Samson 1-4-99 */
DECLARE_DO_FUN( do_email        );  /* User can enter email addy for finger - Samson 4-18-98
*/
DECLARE_DO_FUN( do_timers	);
/* Finger File */
DECLARE_DO_FUN( do_finger     );  /* Finger command - Samson 4-6-98 */
DECLARE_DO_FUN( do_privacy      );  /* Privacy flag toggle - Samson 6-11-99 */
DECLARE_DO_FUN( do_hiscoset	); /* cronel hiscores */
DECLARE_DO_FUN( do_hiscore	); /* cronel hiscores */
/* Ship Code */
DECLARE_DO_FUN( do_makeship	);
DECLARE_DO_FUN( do_shipset	);
DECLARE_DO_FUN( do_ships	);
DECLARE_DO_FUN( do_shipstat	);
DECLARE_DO_FUN( do_leaveship    );
DECLARE_DO_FUN( do_launch       );
DECLARE_DO_FUN( do_study	);
DECLARE_DO_FUN( do_start        );
DECLARE_DO_FUN( do_calculate    );
DECLARE_DO_FUN( do_lockship	);
DECLARE_DO_FUN( do_unlockship	);
DECLARE_DO_FUN( do_defuse	);
DECLARE_DO_FUN( do_fuse		);
/* Energy Attacks */
DECLARE_DO_FUN( do_burnattack	);
DECLARE_DO_FUN( do_hellsflash	);
DECLARE_DO_FUN( do_rocketpunch  );
DECLARE_DO_FUN( do_dragonfist   );
DECLARE_DO_FUN( do_finalflash	);
DECLARE_DO_FUN( do_heal		);
DECLARE_DO_FUN( do_absorb	);
DECLARE_DO_FUN( do_kilimeter	);
DECLARE_DO_FUN( do_kame		);
DECLARE_DO_FUN( do_sbc		);
DECLARE_DO_FUN( do_scattershot	);
DECLARE_DO_FUN( do_blast	);
DECLARE_DO_FUN( do_charge       );
DECLARE_DO_FUN( do_direct       );
DECLARE_DO_FUN( do_disk         );
DECLARE_DO_FUN( do_energyball   );
DECLARE_DO_FUN( do_galic        );
DECLARE_DO_FUN( do_barrier	);
DECLARE_DO_FUN( do_release      );
DECLARE_DO_FUN( do_claw         );
DECLARE_DO_FUN( do_fingerbeam	);
DECLARE_DO_FUN(	do_masenko	);
DECLARE_DO_FUN( do_tribeam      );
DECLARE_DO_FUN( do_mental	);
DECLARE_DO_FUN( do_eyebeam      );
DECLARE_DO_FUN( do_deathbeam    );
DECLARE_DO_FUN( do_makosen      );
DECLARE_DO_FUN( do_dodompa      );
DECLARE_DO_FUN( do_renzoku      );
DECLARE_DO_FUN( do_dk           );
DECLARE_DO_FUN( do_dp           );
DECLARE_DO_FUN( do_donut	);
/* Powerups */
DECLARE_DO_FUN( do_ultranamek	);
DECLARE_DO_FUN( do_superhuman	);
DECLARE_DO_FUN( do_kaioken	);
DECLARE_DO_FUN( do_transform	);
DECLARE_DO_FUN( do_mystic       );
DECLARE_DO_FUN( do_oozaru       );
DECLARE_DO_FUN( do_snamek       );
DECLARE_DO_FUN( do_mutate       );
DECLARE_DO_FUN( do_powerup      );
DECLARE_DO_FUN( do_powerdown    );
DECLARE_DO_FUN( do_stage	);
DECLARE_DO_FUN( do_ussj		);

/* Homes */
DECLARE_DO_FUN( do_buyhome );
DECLARE_DO_FUN( do_sellhome     );
DECLARE_DO_FUN( do_homedesc	);
DECLARE_DO_FUN( do_homename	);
DECLARE_DO_FUN( do_invite	);
DECLARE_DO_FUN( do_buzz		);
DECLARE_DO_FUN( do_homeroom	);
DECLARE_DO_FUN( do_homeflag	);
/* Capture Code */
DECLARE_DO_FUN( do_capture	);
DECLARE_DO_FUN( do_planets	);
DECLARE_DO_FUN( do_retrieve	);
DECLARE_DO_FUN( do_propaganda	);
/* Misc */
DECLARE_DO_FUN( do_fusetalk	);
DECLARE_DO_FUN( do_plist	);
DECLARE_DO_FUN( do_consent	);
DECLARE_DO_FUN( do_clear	);
DECLARE_DO_FUN( do_dbzdlscore	);
DECLARE_DO_FUN( do_rps		);
DECLARE_DO_FUN( do_cointoss	);
DECLARE_DO_FUN( do_dragonball	);
DECLARE_DO_FUN( do_summon	);
DECLARE_DO_FUN( do_calculator   );
DECLARE_DO_FUN( do_keypad 	);
DECLARE_DO_FUN( do_admin	);
DECLARE_DO_FUN( do_imprison	);
DECLARE_DO_FUN( do_oproto	);
DECLARE_DO_FUN( do_mproto	);
DECLARE_DO_FUN( do_showmapreverse);
DECLARE_DO_FUN( do_showmap	);
DECLARE_DO_FUN( do_members	);
DECLARE_DO_FUN( do_outlaw	);
DECLARE_DO_FUN( do_silencenew	);
DECLARE_DO_FUN( do_unsilencenew );
DECLARE_DO_FUN( do_weight	);
DECLARE_DO_FUN( do_height	);
DECLARE_DO_FUN( do_uncover	);
DECLARE_DO_FUN( do_betc		);
DECLARE_DO_FUN( do_betr 	);
DECLARE_DO_FUN( do_notepeek	);
DECLARE_DO_FUN( do_cycle	);
DECLARE_DO_FUN( do_androidmod	);
DECLARE_DO_FUN( do_deadly	);
DECLARE_DO_FUN( do_connect	);
DECLARE_DO_FUN( do_fakequit	);
DECLARE_DO_FUN( do_restorefile );
DECLARE_DO_FUN( do_semimortal   );
DECLARE_DO_FUN( do_unsuppress	);
DECLARE_DO_FUN( do_kickoff	);
DECLARE_DO_FUN( do_saveall	);
DECLARE_DO_FUN( do_makefree	);
DECLARE_DO_FUN( do_learn	);
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN( do_skillinfo 	);
DECLARE_DO_FUN( do_skilltree	);
DECLARE_DO_FUN( do_newslist	);
DECLARE_DO_FUN( do_bless	);
DECLARE_DO_FUN( do_unbless	);
DECLARE_DO_FUN( do_togglechat   );
DECLARE_DO_FUN( do_newpractice  );
DECLARE_DO_FUN( do_spar         );
DECLARE_DO_FUN( do_bank         );
DECLARE_DO_FUN( do_zecho        );
DECLARE_DO_FUN( do_ojunk        );
DECLARE_DO_FUN( do_cuttail      );
DECLARE_DO_FUN( do_fly          );
DECLARE_DO_FUN( do_growtail     );
DECLARE_DO_FUN( do_nkick        );
DECLARE_DO_FUN( do_npunch       );
DECLARE_DO_FUN( do_sense        );
DECLARE_DO_FUN( do_status       );
DECLARE_DO_FUN( do_finger       );
DECLARE_DO_FUN( do_gravset      );
DECLARE_DO_FUN(	do_cleanroom    );
DECLARE_DO_FUN( do_alist	);

/*Mob Prog*/
DECLARE_DO_FUN( do_mpgiverage	);
DECLARE_DO_FUN( do_mpgivebio	);
DECLARE_DO_FUN( do_mp_kidrain   );
DECLARE_DO_FUN( do_mp_kirestore );
DECLARE_DO_FUN( do_mp_stun	);
DECLARE_DO_FUN( do_mpdeadly	);
DECLARE_DO_FUN( do_mpgivearmor  );
DECLARE_DO_FUN( do_mpgiveweapon );
DECLARE_DO_FUN( do_mpgivebless  );
DECLARE_DO_FUN( do_mpgivegold   );
DECLARE_DO_FUN( do_mpgivepl     );
DECLARE_DO_FUN( do_mpgiveunlock );
/* Arena */
DECLARE_DO_FUN( do_bet          );
DECLARE_DO_FUN( do_chaos        );
DECLARE_DO_FUN( do_challenge    );
DECLARE_DO_FUN( do_aaccept      );
DECLARE_DO_FUN( do_adecline     );
DECLARE_DO_FUN( do_awho         );
DECLARE_DO_FUN( do_ahall        );
DECLARE_DO_FUN( do_arena        );
DECLARE_DO_FUN( do_board        );
DECLARE_DO_FUN( do_deleteroom	);
DECLARE_DO_FUN( do_buildhq	);
DECLARE_DO_FUN( do_clanstatus   );
DECLARE_DO_FUN( do_mpmudecho	);
DECLARE_DO_FUN( do_mpclanlevelraise );

#define 	AURA_BLACK		0
#define		AURA_RED		1
#define		AURA_BLUE		2
#define		AURA_YELLOW		3
#define		AURA_GREEN		4
#define		AURA_WHITE		5
#define		AURA_DRED		6
#define		AURA_DBLUE		7
#define		AURA_BROWN		8
#define		AURA_CYAN		9
#define		AURA_DGREEN		10
#define		AURA_PURPLE		11
#define		AURA_PINK		12
#define		MAX_AURA		13
