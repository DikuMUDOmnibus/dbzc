
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
 * 			Table load/save Module				    *
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

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "mud.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}
bool load_race_file( char *fname );
void write_race_file( int ra );


/* global variables */
int top_sn;
int top_sskill;
int top_askill;
int MAX_PC_CLASS;
int MAX_PC_RACE;

SKILLTYPE *		sskill_table	[MAX_SSKILL];
SKILLTYPE *		askill_table	[MAX_ASKILL];
struct	class_type *	class_table	[MAX_CLASS];
RACE_TYPE *		race_table	[MAX_RACE];
SHIP_DATA *     ship_table  [MAX_SHIP];
char *			title_table	[MAX_CLASS]
					[MAX_LEVEL+1]
					[2];


char * const skill_tname[] = { "unknown", "SSkill", "ASkill" };

SPELL_FUN *spell_function( char *name )
{
   void *funHandle;
   const char *error;

   funHandle = dlsym( sysdata.dlHandle, name );
   if( ( error = dlerror() ) != NULL )
   {
	bug( "Error locating %s in symbol table. %s", name, error );
      return spell_notfound;
   }
   return (SPELL_FUN*)funHandle;
}

DO_FUN *skill_function( char *name )
{
   void *funHandle;
   const char *error;

   funHandle = dlsym( sysdata.dlHandle, name );
   if( ( error = dlerror() ) != NULL )
   {
	bug( "Error locating %s in symbol table. %s", name, error );
	return skill_notfound;
   }
   return (DO_FUN*)funHandle;
}


/*
SPELL_FUN *spell_function( char *name )
{
    if ( !str_cmp( name, "spell_smaug" ))	     return spell_smaug;
    if ( !str_cmp( name, "reserved" ))		     return NULL;
    if ( !str_cmp( name, "spell_null" ))	     return spell_null;
    return spell_notfound;
}

DO_FUN *skill_function( char *name )
{
    switch( name[3] )
    {
    case 'a':
        if ( !str_cmp( name, "do_aaccept" ))            return do_aaccept;
	if ( !str_cmp( name, "do_activate" ))           return do_activate;
        if ( !str_cmp( name, "do_absorb" ))		return do_absorb;
        if ( !str_cmp( name, "do_adecline" ))           return do_adecline;     
        if ( !str_cmp( name, "do_admin" ))		return do_admin;
	if ( !str_cmp( name, "do_aassign" ))		return do_aassign;
	if ( !str_cmp( name, "do_addroom" ))		return do_addroom;
	if ( !str_cmp( name, "do_advance" ))		return do_advance;
	if ( !str_cmp( name, "do_add_imm_host" ) )      return do_add_imm_host;
	if ( !str_cmp( name, "do_afk" ))		return do_afk;
        if ( !str_cmp( name, "do_ahall" ))              return do_ahall;
	if ( !str_cmp( name, "do_aid" ))		return do_aid;
        if ( !str_cmp( name, "do_alist" ))		return do_alist;
	if ( !str_cmp( name, "do_allow" ))		return do_allow;
        if ( !str_cmp( name, "do_analyze" ))            return do_analyze;
        if ( !str_cmp( name, "do_androidmod" ))		return do_androidmod;
	if ( !str_cmp( name, "do_ansi" ))		return do_ansi;
	if ( !str_cmp( name, "do_answer" ))		return do_answer;
	if ( !str_cmp( name, "do_apply" ))		return do_apply;
	if ( !str_cmp( name, "do_appraise" ))		return do_appraise;
	if ( !str_cmp( name, "do_areas" ))		return do_areas;
        if ( !str_cmp( name, "do_arena" ))              return do_arena;
	if ( !str_cmp( name, "do_aset" ))		return do_aset;
	if ( !str_cmp( name, "do_ask" ))		return do_ask;
	if ( !str_cmp( name, "do_astat" ))		return do_astat;
	if ( !str_cmp( name, "do_at" ))			return do_at;
	if ( !str_cmp( name, "do_auction" ))		return do_auction;
	if ( !str_cmp( name, "do_authorize" ))		return do_authorize;
	if ( !str_cmp( name, "do_avtalk" ))		return do_avtalk;
        if ( !str_cmp( name, "do_awho" ))               return do_awho;
	if ( !str_cmp( name, "do_absorption"))		return do_absorption;
	break;
    case 'b':
	if ( !str_cmp( name, "do_balzhur" ))		return do_balzhur;
	if ( !str_cmp( name, "do_bamfin" ))		return do_bamfin;
	if ( !str_cmp( name, "do_bamfout" ))		return do_bamfout;
	if ( !str_cmp( name, "do_ban" ))		return do_ban;
        if ( !str_cmp( name, "do_bank" ))               return do_bank;
	if ( !str_cmp( name, "do_barrier" ))		return do_barrier;
	if ( !str_cmp( name, "do_bestow" ))		return do_bestow;
	if ( !str_cmp( name, "do_bestowarea" ))		return do_bestowarea;
        if ( !str_cmp( name, "do_bet" ))                return do_bet;
        if ( !str_cmp( name, "do_betc" ))		return do_betc;
        if ( !str_cmp( name, "do_betr" ))		return do_betr;
	if ( !str_cmp( name, "do_bio" ))		return do_bio;
        if ( !str_cmp( name, "do_blast" ))  		return do_blast;
	if ( !str_cmp( name, "do_bless" ))		return do_bless;
        if ( !str_cmp( name, "do_board" ))              return do_board;
	if ( !str_cmp( name, "do_boards" ))		return do_boards;
	if ( !str_cmp( name, "do_bodybag" ))		return do_bodybag;
	if ( !str_cmp( name, "do_bolt" ))		return do_bolt;
	if ( !str_cmp( name, "do_bset" ))		return do_bset;
	if ( !str_cmp( name, "do_bstat" ))		return do_bstat;
	if ( !str_cmp( name, "do_buildhq" ))		return do_buildhq;
	if ( !str_cmp( name, "do_burnattack" ))		return do_burnattack;
	if ( !str_cmp( name, "do_bury" ))		return do_bury;
	if ( !str_cmp( name, "do_buy" ))		return do_buy;
        if ( !str_cmp( name, "do_buyhome" ))            return do_buyhome;
        if ( !str_cmp( name, "do_buzz" ))		return do_buzz;
	break;
    case 'c':
        if ( !str_cmp( name, "do_calculate" ))		return do_calculate;
        if ( !str_cmp( name, "do_calculator" ))        return do_calculator;
        if ( !str_cmp( name, "do_capture" ))		return do_capture;
	if ( !str_cmp( name, "do_cedit" ))		return do_cedit;
        if ( !str_cmp( name, "do_challenge" ))          return do_challenge;
	if ( !str_cmp( name, "do_channels" ))		return do_channels;
        if ( !str_cmp( name, "do_chaos" ))              return do_chaos;
        if ( !str_cmp( name, "do_charge" ))             return do_charge;
        if ( !str_cmp( name, "do_chargeto" ))		return do_chargeto;
	if ( !str_cmp( name, "do_chat" ))		return do_chat;
	if ( !str_cmp( name, "do_check_vnums" ))	return do_check_vnums;
        if ( !str_cmp( name, "do_checkradar" ))		return do_checkradar;
	if ( !str_cmp( name, "do_clans" ))		return do_clans;
        if ( !str_cmp( name, "do_clanset" ))            return do_clanset;
	if ( !str_cmp( name, "do_clanstatus"))		return do_clanstatus;
	if ( !str_cmp( name, "do_clantalk" ))		return do_clantalk;
	if ( !str_cmp( name, "do_claw" ))		return do_claw;
        if ( !str_cmp( name, "do_cleanroom" ))          return do_cleanroom;
 	if ( !str_cmp( name, "do_clear" ))		return do_clear;
	if ( !str_cmp( name, "do_climate" ))		return do_climate;
	if ( !str_cmp( name, "do_climb" ))		return do_climb;
	if ( !str_cmp( name, "do_close" ))		return do_close;
	if ( !str_cmp( name, "do_cmdtable" ))		return do_cmdtable;
        if ( !str_cmp( name, "do_cointoss" ))		return do_cointoss;
	if ( !str_cmp( name, "do_color" ))			return do_color;
	if ( !str_cmp( name, "do_commands" ))		return do_commands;
	if ( !str_cmp( name, "do_comment" ))		return do_comment;
	if ( !str_cmp( name, "do_compare" ))		return do_compare;
	if ( !str_cmp( name, "do_config" ))		return do_config;
        if ( !str_cmp( name, "do_connect" ))		return do_connect;
	if ( !str_cmp( name, "do_consent" ))		return do_consent;
	if ( !str_cmp( name, "do_consider" ))		return do_consider;
        if ( !str_cmp( name, "do_coordinates" ) )       return do_coordinates;
	if ( !str_cmp( name, "do_coords"))		return do_coords;
        if ( !str_cmp( name, "do_copyover" ))           return do_copyover;
	if ( !str_cmp( name, "do_createclan" ))		return do_createclan;
	if ( !str_cmp( name, "do_credits" ))		return do_credits;
	if ( !str_cmp( name, "do_cset" ))		return do_cset;
        if ( !str_cmp( name, "do_cuttail" ))            return do_cuttail;
        if ( !str_cmp( name, "do_cycle" ))		return do_cycle;
	break;
    case 'd':
	if ( !str_cmp( name, "do_dbzdlscore" ))		return do_dbzdlscore;
  	if ( !str_cmp( name, "do_deadly" ))		return do_deadly;
	if ( !str_cmp( name, "do_defuse" ))		return do_defuse;
	if ( !str_cmp( name, "do_delay" ))		return do_delay;
        if ( !str_cmp( name, "do_delete" ))             return do_delete;
        if ( !str_cmp( name, "do_deleteroom" ))		return do_deleteroom;
	if ( !str_cmp( name, "do_deny" ))		return do_deny;
	if ( !str_cmp( name, "do_description" ))	return do_description;
	if ( !str_cmp( name, "do_destro" ))		return do_destro;
	if ( !str_cmp( name, "do_destroy" ))		return do_destroy;
        if ( !str_cmp( name, "do_direct" ))             return do_direct;
	if ( !str_cmp( name, "do_disconnect" ))		return do_disconnect;
        if ( !str_cmp( name, "do_disk" ))               return do_disk;
	if ( !str_cmp( name, "do_dmesg" ))		return do_dmesg;
	if ( !str_cmp( name, "do_dnd" ))		return do_dnd;
	if ( !str_cmp( name, "do_docks" ))		return do_docks;
	if ( !str_cmp( name, "do_dockstat" ))		return do_dockstat;
	if ( !str_cmp( name, "do_down" ))		return do_down;
	if ( !str_cmp( name, "do_drag" ))		return do_drag;
	if ( !str_cmp( name, "do_dragonball" ))		return do_dragonball;
	if ( !str_cmp( name, "do_drink" ))		return do_drink;
	if ( !str_cmp( name, "do_drop" ))		return do_drop;
	break;
    case 'e':
	if ( !str_cmp( name, "do_east" ))		return do_east;
	if ( !str_cmp( name, "do_eat" ))		return do_eat;
	if ( !str_cmp( name, "do_ech" ))		return do_ech;
	if ( !str_cmp( name, "do_echo" ))		return do_echo;
        if ( !str_cmp( name, "do_elevate" ))            return do_elevate;
        if ( !str_cmp( name, "do_email" ))              return do_email;
	if ( !str_cmp( name, "do_emote" ))		return do_emote;
	if ( !str_cmp( name, "do_empty" ))		return do_empty;
        if ( !str_cmp( name, "do_energyball" ))         return do_energyball;
	if ( !str_cmp( name, "do_enter" ))		return do_enter;
	if ( !str_cmp( name, "do_equipment" ))		return do_equipment;
	if ( !str_cmp( name, "do_examine" ))		return do_examine;
	if ( !str_cmp( name, "do_exits" ))		return do_exits;
	break;
    case 'f':
        if ( !str_cmp( name, "do_fakequit" ))		return do_fakequit;
	if ( !str_cmp( name, "do_fill" ))		return do_fill;
	if ( !str_cmp( name, "do_finalflash" ))		return do_finalflash;
	if ( !str_cmp( name, "do_findnote" ))		return do_findnote;
        if ( !str_cmp( name, "do_finger" ))             return do_finger;
	if ( !str_cmp( name, "do_fingerbeam" ))         return do_fingerbeam;
	if ( !str_cmp( name, "do_fixchar" ))		return do_fixchar;
	if ( !str_cmp( name, "do_flee" ))		return do_flee;
        if ( !str_cmp( name, "do_fly" ))                return do_fly;
	if ( !str_cmp( name, "do_foldarea" ))		return do_foldarea;
	if ( !str_cmp( name, "do_follow" ))		return do_follow;
	if ( !str_cmp( name, "do_for" ))		return do_for;
	if ( !str_cmp( name, "do_force" ))		return do_force;
	if ( !str_cmp( name, "do_forceclose" ))		return do_forceclose;
	if ( !str_cmp( name, "do_form_password" ))	return do_form_password;
	if ( !str_cmp( name, "do_fprompt" ))		return do_fprompt;
	if ( !str_cmp( name, "do_fquit" ))		return do_fquit;
	if ( !str_cmp( name, "do_freeze" ))		return do_freeze;
	if ( !str_cmp( name, "do_fshow" ))		return do_fshow;
	if ( !str_cmp( name, "do_fuse" ))		return do_fuse;
 	if ( !str_cmp( name, "do_fusetalk" ))		return do_fusetalk;
	break;
    case 'g':
        if ( !str_cmp( name, "do_galic" ))              return do_galic;
	if ( !str_cmp( name, "do_gamble" ))		return do_gamble;
        if ( !str_cmp( name, "do_gemote"))              return do_gemote;
	if ( !str_cmp( name, "do_get" ))		return do_get;
	if ( !str_cmp( name, "do_gfighting" ))		return do_gfighting;
	if ( !str_cmp( name, "do_ghostattack" ))	return do_ghostattack;
	if ( !str_cmp( name, "do_give" ))		return do_give;
	if ( !str_cmp( name, "do_glance" ))		return do_glance;
        if ( !str_cmp( name, "do_global_boards" ))      return do_global_boards;
        if ( !str_cmp( name, "do_global_note" ))        return do_global_note;
        if ( !str_cmp( name, "do_gold" ))               return do_gold;
	if ( !str_cmp( name, "do_goto" ))		return do_goto;
        if ( !str_cmp( name, "do_gravset" ))            return do_gravset;
	if ( !str_cmp( name, "do_group" ))		return do_group;
        if ( !str_cmp( name, "do_growtail" ))           return do_growtail;
	if ( !str_cmp( name, "do_gtell" ))		return do_gtell;
	if ( !str_cmp( name, "do_gwhere" ))		return do_gwhere;
	break;
    case 'h':
	if ( !str_cmp( name, "do_heal" ))		return do_heal;
	if ( !str_cmp( name, "do_hedit" ))		return do_hedit;
        if ( !str_cmp( name, "do_height" ))		return do_height;
	if ( !str_cmp( name, "do_hell" ))		return do_hell;
	if ( !str_cmp( name, "do_hellsflash" ))		return do_hellsflash;
	if ( !str_cmp( name, "do_help" ))		return do_help;
	if ( !str_cmp( name, "do_hiscore" ))		return do_hiscore; 
	if ( !str_cmp( name, "do_hiscoset" ))		return do_hiscoset;
	if ( !str_cmp( name, "do_hl" ))			return do_hl;
	if ( !str_cmp( name, "do_hlist" ))		return do_hlist;
	if ( !str_cmp( name, "do_holylight" ))		return do_holylight;
        if ( !str_cmp( name, "do_homedesc" ))		return do_homedesc;
        if ( !str_cmp( name, "do_homeflag" ))           return do_homeflag;
        if ( !str_cmp( name, "do_homename" ))		return do_homename;
	if ( !str_cmp( name, "do_homepage" ))		return do_homepage;
        if ( !str_cmp( name, "do_homeroom" ))           return do_homeroom;
	if ( !str_cmp( name, "do_hset" ))		return do_hset;
	break;
    case 'i':
        if ( !str_cmp( name, "do_icq_number"))          return do_icq_number;
	if ( !str_cmp( name, "do_igive"))               return do_igive;
        if ( !str_cmp( name, "do_ignore" ))		return do_ignore;
	if ( !str_cmp( name, "do_immortalize" ))	return do_immortalize;
	if ( !str_cmp( name, "do_immtalk" ))		return do_immtalk;
        if ( !str_cmp( name, "do_imprison" ))		return do_imprison;
	if ( !str_cmp( name, "do_induct" ))		return do_induct;
	if ( !str_cmp( name, "do_installarea" ))	return do_installarea;
	if ( !str_cmp( name, "do_instaroom" ))		return do_instaroom;
	if ( !str_cmp( name, "do_instazone" ))		return do_instazone;
	if ( !str_cmp( name, "do_inventory" ))		return do_inventory;
	if ( !str_cmp( name, "do_invis" ))		return do_invis;
        if ( !str_cmp( name, "do_invite" ))		return do_invite;
        if ( !str_cmp( name, "do_ipcompare" ))		return do_ipcompare;
	break;
    case 'k':
        if ( !str_cmp( name, "do_kaioken" ))		return do_kaioken;
 	if ( !str_cmp( name, "do_kame" ))		return do_kame;
   	if ( !str_cmp( name, "do_keypad" ))		return do_keypad;
    	if ( !str_cmp( name, "do_khistory" ))		return do_khistory;
        if ( !str_cmp( name, "do_kickoff" ))            return do_kickoff;
	if ( !str_cmp( name, "do_kill" ))		return do_kill;
	if ( !str_cmp( name, "do_kilimeter"))		return do_kilimeter;
	if ( !str_cmp( name, "do_koth"))		return do_koth;
	break;
    case 'l':
	if ( !str_cmp( name, "do_landmarks" ))		return do_landmarks;
	if ( !str_cmp( name, "do_last" ))		return do_last;
	if ( !str_cmp( name, "do_launch" ))		return do_launch;
	if ( !str_cmp( name, "do_laws" ))		return do_laws;
        if ( !str_cmp( name, "do_learn" ))              return do_learn;
	if ( !str_cmp( name, "do_leave" ))		return do_leave;
        if ( !str_cmp( name, "do_leaveship" ))          return do_leaveship;
	if ( !str_cmp( name, "do_level" ))		return do_level;
	if ( !str_cmp( name, "do_light" ))		return do_light;
	if ( !str_cmp( name, "do_list" ))		return do_list;
	if ( !str_cmp( name, "do_litterbug" ))		return do_litterbug;
	if ( !str_cmp( name, "do_loadarea" ))		return do_loadarea;
	if ( !str_cmp( name, "do_loadup" ))		return do_loadup;
	if ( !str_cmp( name, "do_lock" ))		return do_lock;
        if ( !str_cmp( name, "do_lockship" ))           return do_lockship;
	if ( !str_cmp( name, "do_log" ))		return do_log;
	if ( !str_cmp( name, "do_look" ))		return do_look;
	if ( !str_cmp( name, "do_low_purge" ))		return do_low_purge;
	break;
    case 'm':
	if ( !str_cmp( name, "do_mailroom" ))		return do_mailroom;
	if ( !str_cmp( name, "do_makeboard" ))		return do_makeboard;
	if ( !str_cmp( name, "do_makeclan" ))		return do_makeclan;
        if ( !str_cmp( name, "do_makefree" ))           return do_makefree;
	if ( !str_cmp( name, "do_makerepair" ))		return do_makerepair;
 	if ( !str_cmp( name, "do_makeship" ))		return do_makeship;
	if ( !str_cmp( name, "do_makeshop" ))		return do_makeshop;
	if ( !str_cmp( name, "do_makewizlist" ))	return do_makewizlist;
	if ( !str_cmp( name, "do_mapedit" ))		return do_mapedit;
	if ( !str_cmp( name, "do_maplegend" ))		return do_maplegend;
        if ( !str_cmp( name, "do_maplegend2" ))          return do_maplegend2;
	if ( !str_cmp( name, "do_mapresets" ))		return do_mapresets;
	if ( !str_cmp( name, "do_mapsize" ))		return do_mapsize;
        if ( !str_cmp( name, "do_masenko" ))		return do_masenko;
	if ( !str_cmp( name, "do_massign" ))		return do_massign;
	if ( !str_cmp( name, "do_mcreate" ))		return do_mcreate;
	if ( !str_cmp( name, "do_mdelete" ))		return do_mdelete;
        if ( !str_cmp( name, "do_members" ))		return do_members;
	if ( !str_cmp( name, "do_memory" ))		return do_memory;
        if ( !str_cmp( name, "do_mental" ))  		return do_mental;
	if ( !str_cmp( name, "do_mfind" ))		return do_mfind;
	if ( !str_cmp( name, "do_minvoke" ))		return do_minvoke;
	if ( !str_cmp( name, "do_mlist" ))		return do_mlist;
        if ( !str_cmp( name, "do_mortalize" ))          return do_mortalize;
	if ( !str_cmp( name, "do_mp_close_passage" ))	return do_mp_close_passage;
	if ( !str_cmp( name, "do_mp_damage" ))		return do_mp_damage;
	if ( !str_cmp( name, "do_mp_deposit" ))		return do_mp_deposit;
	if ( !str_cmp( name, "do_mp_fill_in" ) )	return do_mp_fill_in;
        if ( !str_cmp( name, "do_mp_kidrain" ))         return do_mp_kidrain;
        if ( !str_cmp( name, "do_mp_kirestore" ))       return do_mp_kirestore;
	if ( !str_cmp( name, "do_mp_log" ))		return do_mp_log;
	if ( !str_cmp( name, "do_mp_open_passage" ))	return do_mp_open_passage;
	if ( !str_cmp( name, "do_mp_practice" ))	return do_mp_practice;
	if ( !str_cmp( name, "do_mp_restore" ))		return do_mp_restore;
	if ( !str_cmp( name, "do_mp_slay" ))		return do_mp_slay;
        if ( !str_cmp( name, "do_mp_stun" ))            return do_mp_stun;
	if ( !str_cmp( name, "do_mp_withdraw" ))	return do_mp_withdraw;
	if ( !str_cmp( name, "do_mpadvance" ))		return do_mpadvance;
	if ( !str_cmp( name, "do_mpasound" ))		return do_mpasound;
	if ( !str_cmp( name, "do_mpasupress" ))		return do_mpasupress;
	if ( !str_cmp( name, "do_mpat" ))		return do_mpat;
	if ( !str_cmp( name, "do_mpbodybag") )		return do_mpbodybag;
        if ( !str_cmp( name, "do_mpclanlevelraise" ))   return do_mpclanlevelraise;
	if ( !str_cmp( name, "do_mpcopy" ))		return do_mpcopy;
        if ( !str_cmp( name, "do_mpdeadly" ))		return do_mpdeadly;
	if ( !str_cmp( name, "do_mpdelay" ))		return do_mpdelay;
	if ( !str_cmp( name, "do_mpdream" ))		return do_mpdream;
	if ( !str_cmp( name, "do_mpecho" ))		return do_mpecho;
	if ( !str_cmp( name, "do_mpechoaround" ))	return do_mpechoaround;
	if ( !str_cmp( name, "do_mpechoat" ))		return do_mpechoat;
	if ( !str_cmp( name, "do_mpechozone" ))		return do_mpechozone;
	if ( !str_cmp( name, "do_mpedit" ))		return do_mpedit;
	if ( !str_cmp( name, "do_mpforce" ))		return do_mpforce;
        if ( !str_cmp( name, "do_mpgivearmor" ))        return do_mpgivearmor;
        if ( !str_cmp( name, "do_mpgivebless" ))        return do_mpgivebless;
 	if ( !str_cmp( name, "do_mpgivebio" ))		return do_mpgivebio;
        if ( !str_cmp( name, "do_mpgivegold" ))         return do_mpgivegold;
        if ( !str_cmp( name, "do_mpgivepl" ))           return do_mpgivepl;
	if ( !str_cmp( name, "do_mpgiverage" ))		return do_mpgiverage;
        if ( !str_cmp( name, "do_mpgiveunlock" ))       return do_mpgiveunlock;
        if ( !str_cmp( name, "do_mpgiveweapon" ))       return do_mpgiveweapon;
	if ( !str_cmp( name, "do_mpgoto" ))		return do_mpgoto;
	if ( !str_cmp( name, "do_mpinvis" ))		return do_mpinvis;
	if ( !str_cmp( name, "do_mpjunk" ))		return do_mpjunk;
	if ( !str_cmp( name, "do_mpkill" ))		return do_mpkill;
	if ( !str_cmp( name, "do_mpmload" ))		return do_mpmload;
	if ( !str_cmp( name, "do_mpmset" ))		return do_mpmset;
	if ( !str_cmp( name, "do_mpmudecho" ))		return do_mpmudecho;
	if ( !str_cmp( name, "do_mpmusic" ))		return do_mpmusic;
	if ( !str_cmp( name, "do_mpmusicaround" ))	return do_mpmusicaround;
	if ( !str_cmp( name, "do_mpmusicat" ))		return do_mpmusicat;
	if ( !str_cmp( name, "do_mpnothing" ))		return do_mpnothing;
	if ( !str_cmp( name, "do_mpoload" ))		return do_mpoload;
	if ( !str_cmp( name, "do_mposet" ))		return do_mposet;
	if ( !str_cmp( name, "do_mppardon" ))		return do_mppardon;
	if ( !str_cmp( name, "do_mppeace" ))		return do_mppeace;
	if ( !str_cmp( name, "do_mppkset" ))		return do_mppkset;
	if ( !str_cmp( name, "do_mppurge" ))		return do_mppurge;
     	if ( !str_cmp( name, "do_mproto" ))		return do_mproto;
        if ( !str_cmp( name, "do_mpscatter" ))          return do_mpscatter;
        if ( !str_cmp( name, "do_mpsound" ))		return do_mpsound;
	if ( !str_cmp( name, "do_mpsoundaround" ))	return do_mpsoundaround;
	if ( !str_cmp( name, "do_mpsoundat" ))		return do_mpsoundat;
	if ( !str_cmp( name, "do_mpstat" ))		return do_mpstat;
	if ( !str_cmp( name, "do_mptransfer" ))		return do_mptransfer;
        if ( !str_cmp( name, "do_mpzecho" ))            return do_mpzecho;
	if ( !str_cmp( name, "do_mrange" ))		return do_mrange;
 	if ( !str_cmp( name, "do_mreset" ))		return do_mreset;
	if ( !str_cmp( name, "do_mset" ))		return do_mset;
	if ( !str_cmp( name, "do_mstat" ))		return do_mstat;
	if ( !str_cmp( name, "do_mudstatus" ) )		return do_mudstatus;
	if ( !str_cmp( name, "do_murde" ))		return do_murde;
	if ( !str_cmp( name, "do_murder" ))		return do_murder;
	if ( !str_cmp( name, "do_muse" ))		return do_muse;
	if ( !str_cmp( name, "do_music" ))		return do_music;
  	if ( !str_cmp( name, "do_mutate" ))             return do_mutate;
        if ( !str_cmp( name, "do_mwhere" ))		return do_mwhere;
        if ( !str_cmp( name, "do_mystic" ))             return do_mystic;
	break;

    case 'n':
	if ( !str_cmp( name, "do_name" ))		return do_name;
	if ( !str_cmp( name, "do_newbiechat" ))		return do_newbiechat;
	if ( !str_cmp( name, "do_newbieset" ))		return do_newbieset;
        if ( !str_cmp( name, "do_newpractice"))		return do_newpractice;
	if ( !str_cmp( name, "do_news" ))               return do_news;
	if ( !str_cmp( name, "do_newscore" ))		return do_newscore;
	if ( !str_cmp( name, "do_newslist" ))		return do_newslist;
	if ( !str_cmp( name, "do_newzones" ))		return do_newzones;
        if ( !str_cmp( name, "do_nkick" ))              return do_nkick;
        if ( !str_cmp( name, "do_npunch" ))             return do_npunch;
	if ( !str_cmp( name, "do_noemote" ))		return do_noemote;
	if ( !str_cmp( name, "do_noresolve" ))		return do_noresolve;
	if ( !str_cmp( name, "do_north" ))		return do_north;
	if ( !str_cmp( name, "do_northeast" ))		return do_northeast;
	if ( !str_cmp( name, "do_northwest" ))		return do_northwest;
	if ( !str_cmp( name, "do_notell" ))		return do_notell;
        if ( !str_cmp( name, "do_notepeek" ))		return do_notepeek;
        if ( !str_cmp( name, "do_notitle" ))            return do_notitle;
	if ( !str_cmp( name, "do_noteroom" ))		return do_noteroom;
	if ( !str_cmp( name, "do_null"	))		return do_null;
	break;
    case 'o':
	if ( !str_cmp( name, "do_oassign" ))		return do_oassign;
	if ( !str_cmp( name, "do_ocreate" ))		return do_ocreate;
	if ( !str_cmp( name, "do_odelete" ))		return do_odelete;
	if ( !str_cmp( name, "do_ofind" ))		return do_ofind;
        if ( !str_cmp( name, "do_ojunk" ))              return do_ojunk;
	if ( !str_cmp( name, "do_oinvoke" ))		return do_oinvoke;
	if ( !str_cmp( name, "do_oldscore" ))		return do_oldscore;
	if ( !str_cmp( name, "do_olist" ))		return do_olist;
        if ( !str_cmp( name, "do_ooc" ))                return do_ooc;
        if ( !str_cmp( name, "do_oozaru" ))             return do_oozaru;
	if ( !str_cmp( name, "do_opcopy" ))		return do_opcopy;
	if ( !str_cmp( name, "do_opedit" ))		return do_opedit;
	if ( !str_cmp( name, "do_open" ))		return do_open;
	if ( !str_cmp( name, "do_oproto" ))		return do_oproto;
	if ( !str_cmp( name, "do_opstat" ))		return do_opstat;
	if ( !str_cmp( name, "do_orange" ))		return do_orange;
	if ( !str_cmp( name, "do_oset" ))		return do_oset;
	if ( !str_cmp( name, "do_ostat" ))		return do_ostat;
	if ( !str_cmp( name, "do_outcast" ))		return do_outcast;
        if ( !str_cmp( name, "do_outlaw" ))		return do_outlaw;
	if ( !str_cmp( name, "do_owhere" ))		return do_owhere;
	break;
    case 'p':
	if ( !str_cmp( name, "do_pager" ))		return do_pager;
	if ( !str_cmp( name, "do_pardon" ))		return do_pardon;
	if ( !str_cmp( name, "do_password" ))		return do_password;
	if ( !str_cmp( name, "do_pcrename" ) )		return do_pcrename;
	if ( !str_cmp( name, "do_peace" ))		return do_peace;
 	if ( !str_cmp( name, "do_pfiles" ))		return do_pfiles;
	if ( !str_cmp( name, "do_planets" ))		return do_planets;
	if ( !str_cmp( name, "do_plist" ))		return do_plist;
        if ( !str_cmp( name, "do_powerdown" ))          return do_powerdown;
        if ( !str_cmp( name, "do_powerup" ))            return do_powerup;
        if ( !str_cmp( name, "do_privacy" )) 		return do_privacy;
	if ( !str_cmp( name, "do_project" ))		return do_project;
	if ( !str_cmp( name, "do_prompt" ))		return do_prompt;
        if ( !str_cmp( name, "do_propaganda" ))		return do_propaganda;
	if ( !str_cmp( name, "do_pull" ))		return do_pull;
	if ( !str_cmp( name, "do_purge" ))		return do_purge;
	if ( !str_cmp( name, "do_push" ))		return do_push;
	if ( !str_cmp( name, "do_put" ))		return do_put;
	break;
    case 'q':
	if ( !str_cmp( name, "do_quaff" ))		return do_quaff;
	if ( !str_cmp( name, "do_qui" ))		return do_qui;
	if ( !str_cmp( name, "do_quit" ))		return do_quit;
	break;
    case 'r':
	if ( !str_cmp( name, "do_racetalk" ))		return do_racetalk;
	if ( !str_cmp( name, "do_rank" ))		return do_rank;
	if ( !str_cmp( name, "do_rap" ))		return do_rap;
	if ( !str_cmp( name, "do_rassign" ))		return do_rassign;
	if ( !str_cmp( name, "do_rat" ))		return do_rat;
	if ( !str_cmp( name, "do_rdelete" ))		return do_rdelete;
	if ( !str_cmp( name, "do_reboo" ))		return do_reboo;
	if ( !str_cmp( name, "do_reboot" ))		return do_reboot;
	if ( !str_cmp( name, "do_recho" ))		return do_recho;
	if ( !str_cmp( name, "do_recite" ))		return do_recite;
	if ( !str_cmp( name, "do_redit" ))		return do_redit;
	if ( !str_cmp( name, "do_regoto" ))		return do_regoto;
        if ( !str_cmp( name, "do_release" ))            return do_release;
	if ( !str_cmp( name, "do_remove" ))		return do_remove;
	if ( !str_cmp( name, "do_rent" ))		return do_rent;
	if ( !str_cmp( name, "do_repair" ))		return do_repair;
	if ( !str_cmp( name, "do_repairset" ))		return do_repairset;
	if ( !str_cmp( name, "do_repairshops" ))	return do_repairshops;
	if ( !str_cmp( name, "do_repairstat" ))		return do_repairstat;
	if ( !str_cmp( name, "do_repeat" ))		return do_repeat;
	if ( !str_cmp( name, "do_reply" ))		return do_reply;
	if ( !str_cmp( name, "do_report" ))		return do_report;
	if ( !str_cmp( name, "do_reserve" ))		return do_reserve;
	if ( !str_cmp( name, "do_reset" ))		return do_reset;
	if ( !str_cmp( name, "do_rest" ))		return do_rest;
	if ( !str_cmp( name, "do_restore" ))		return do_restore;
        if ( !str_cmp( name, "do_restorefile" ))        return do_restorefile;
	if ( !str_cmp( name, "do_restoretime" ))	return do_restoretime;
	if ( !str_cmp( name, "do_restrict" ))		return do_restrict;
	if ( !str_cmp( name, "do_retell" ))		return do_retell;
	if ( !str_cmp( name, "do_retire" ))		return do_retire;
	if ( !str_cmp( name, "do_retran" ))		return do_retran;
	if ( !str_cmp( name, "do_retrieve" ))		return do_retrieve;
	if ( !str_cmp( name, "do_return" ))		return do_return;
	if ( !str_cmp( name, "do_rip" ))		return do_rip;
	if ( !str_cmp( name, "do_rlist" ))		return do_rlist;
 	if ( !str_cmp( name, "do_rolldie" ))		return do_rolldie;
	if ( !str_cmp( name, "do_rpedit" ))		return do_rpedit;
        if ( !str_cmp( name, "do_rps" ))		return do_rps;
	if ( !str_cmp( name, "do_rpstat" ))		return do_rpstat;
 	if ( !str_cmp( name, "do_rset" ))		return do_rset;
 	if ( !str_cmp( name, "do_rstat" ))		return do_rstat;
	if ( !str_cmp( name, "do_run"	))		return do_run;
	break;
    case 's':
	if ( !str_cmp( name, "do_sacrifice" ))		return do_sacrifice;
	if ( !str_cmp( name, "do_save" ))		return do_save;
        if ( !str_cmp( name, "do_saveall" ))            return do_saveall;
	if ( !str_cmp( name, "do_savearea" ))		return do_savearea;
	if ( !str_cmp( name, "do_say" ))		return do_say;
  	if ( !str_cmp( name, "do_sbc" ))		return do_sbc;
	if ( !str_cmp( name, "do_scan" ))		return do_scan;
        if ( !str_cmp( name, "do_scatter" ) )           return do_scatter;
	if ( !str_cmp( name, "do_scattershot" )) 	return do_scattershot;
	if ( !str_cmp( name, "do_search" ))		return do_search;
	if ( !str_cmp( name, "do_sedit" ))		return do_sedit;
	if ( !str_cmp( name, "do_sell" ))		return do_sell;
	if ( !str_cmp( name, "do_sellhome" ))		return do_sellhome;
        if ( !str_cmp( name, "do_semimortal" ))         return do_semimortal;
	if ( !str_cmp( name, "do_sense" ))              return do_sense;
        if ( !str_cmp( name, "do_set_boot_time" ))	return do_set_boot_time;
	if ( !str_cmp( name, "do_setclan" ))		return do_setclan;
	if ( !str_cmp( name, "do_setdock" ))		return do_setdock;
	if ( !str_cmp( name, "do_setexit" ))		return do_setexit;
	if ( !str_cmp( name, "do_setmark" ))		return do_setmark;
	if ( !str_cmp( name, "do_setweather" ))		return do_setweather;
	if ( !str_cmp( name, "do_ships" ))		return do_ships;
	if ( !str_cmp( name, "do_shipset" ))		return do_shipset;
	if ( !str_cmp( name, "do_shipstat" ))		return do_shipstat;
	if ( !str_cmp( name, "do_shops" ))		return do_shops;
	if ( !str_cmp( name, "do_shopset" ))		return do_shopset;
	if ( !str_cmp( name, "do_shopstat" ))		return do_shopstat;
	if ( !str_cmp( name, "do_shout" ))		return do_shout;
	if ( !str_cmp( name, "do_shove" ))		return do_shove;
	if ( !str_cmp( name, "do_showclan" ))		return do_showclan;
	if ( !str_cmp( name, "do_showclass" ))		return do_showclass;
        if ( !str_cmp( name, "do_showmap" ))            return do_showmap;
	if ( !str_cmp( name, "do_showrace" ))		return do_showrace;
	if ( !str_cmp( name, "do_showweather" ))	return do_showweather;
	if ( !str_cmp( name, "do_shutdow" ))		return do_shutdow;
	if ( !str_cmp( name, "do_shutdown" ))		return do_shutdown;
	if ( !str_cmp( name, "do_silence" ))		return do_silence;
	if ( !str_cmp( name, "do_silencenew" ))		return do_silencenew;
	if ( !str_cmp( name, "do_sit" ))		return do_sit;
	if ( !str_cmp( name, "do_skillinfo" ))		return do_skillinfo;
	if ( !str_cmp( name, "do_skills" ))		return do_skills;
	if ( !str_cmp( name, "do_skin" ))		return do_skin;
	if ( !str_cmp( name, "do_sla" ))		return do_sla;
	if ( !str_cmp( name, "do_slay" ))		return do_slay;
	if ( !str_cmp( name, "do_sleep" ))		return do_sleep;
	if ( !str_cmp( name, "do_slevel" ))		return do_slevel;
	if ( !str_cmp( name, "do_slookup" ))		return do_slookup;
	if ( !str_cmp( name, "do_smoke" ))		return do_smoke;
        if ( !str_cmp( name, "do_snamek" ))             return do_snamek;
	if ( !str_cmp( name, "do_snoop" ))		return do_snoop;
	if ( !str_cmp( name, "do_sober" ))		return do_sober;
	if ( !str_cmp( name, "do_socials" ))		return do_socials;
	if ( !str_cmp( name, "do_south" ))		return do_south;
	if ( !str_cmp( name, "do_southeast" ))		return do_southeast;
	if ( !str_cmp( name, "do_southwest" ))		return do_southwest;
        if ( !str_cmp( name, "do_spar" ))               return do_spar;
        if ( !str_cmp( name, "do_spectate" ))           return do_spectate;
	if ( !str_cmp( name, "do_split" ))		return do_split;
	if ( !str_cmp( name, "do_sset" ))		return do_sset;
	if ( !str_cmp( name, "do_ssj" ))                return do_ssj;
        if ( !str_cmp( name, "do_stage" ))              return do_stage;
        if ( !str_cmp( name, "do_stand" ))		return do_stand;
	if ( !str_cmp( name, "do_start" ))		return do_start;
	if ( !str_cmp( name, "do_stat" ) )		return do_stat;
	if ( !str_cmp( name, "do_statreport" ) )	return do_statreport;
	if ( !str_cmp( name, "do_statshield" ) )	return do_statshield;
        if ( !str_cmp( name, "do_status" ))             return do_status;
 	if ( !str_cmp( name, "do_stopspec" ))           return do_stopspec;
	if ( !str_cmp( name, "do_strew" ))		return do_strew;
	if ( !str_cmp( name, "do_strip" ))		return do_strip;
	if ( !str_cmp( name, "do_study" ))		return do_study;
	if ( !str_cmp( name, "do_style" ))		return do_style;
        if ( !str_cmp( name, "do_summon" ))		return do_summon;
	if ( !str_cmp( name, "do_superhuman" ))		return do_superhuman;
        if ( !str_cmp( name, "do_suppress" ))		return do_suppress;
	if ( !str_cmp( name, "do_survey" ))		return do_survey;
	if ( !str_cmp( name, "do_switch" ))		return do_switch;
	if ( !str_cmp( name, "do_steam" ))		return do_steam;
	break;
    case 't':
 	if ( !str_cmp( name, "do_tail" ))		return do_tail;
 	if ( !str_cmp( name, "do_tamp" ))		return do_tamp;
	if ( !str_cmp( name, "do_tell" ))		return do_tell;
	if ( !str_cmp( name, "do_think" ))		return do_think;
	if ( !str_cmp( name, "do_time" ))		return do_time;
	if ( !str_cmp( name, "do_timecmd" ))		return do_timecmd;
	if ( !str_cmp( name, "do_timers" ))		return do_timers;
	if ( !str_cmp( name, "do_title" ))		return do_title;
        if ( !str_cmp( name, "do_toggle" ))             return do_toggle;
	if ( !str_cmp( name, "do_togglechat" ))		return do_togglechat;
	if ( !str_cmp( name, "do_train" ))              return do_train;
        if ( !str_cmp( name, "do_traffic" ))		return do_traffic;
	if ( !str_cmp( name, "do_transfer" ))		return do_transfer;
	if ( !str_cmp( name, "do_transform" )) 		return do_transform;
        if ( !str_cmp( name, "do_tribeam" ))            return do_tribeam;
	if ( !str_cmp( name, "do_trust" ))		return do_trust;
	break;
    case 'u':
        if ( !str_cmp( name, "do_ultranamek" ))		return do_ultranamek;
	if ( !str_cmp( name, "do_unbless" ))		return do_unbless;
	if ( !str_cmp( name, "do_unbolt" ))		return do_unbolt;
	if ( !str_cmp( name, "do_uncover" ))		return do_uncover;
	if ( !str_cmp( name, "do_unfoldarea" ))		return do_unfoldarea;
	if ( !str_cmp( name, "do_unhell" ))		return do_unhell;
	if ( !str_cmp( name, "do_unlock" ))		return do_unlock;
        if ( !str_cmp( name, "do_unlockship" ))         return do_unlockship;
        if ( !str_cmp( name, "do_unsilence" ))          return do_unsilence;
        if ( !str_cmp( name, "do_unsilencenew" ))       return do_unsilencenew;
        if ( !str_cmp( name, "do_unsuppress" ))         return do_unsuppress;
	if ( !str_cmp( name, "do_up" ))			return do_up;
	if ( !str_cmp( name, "do_users" ))		return do_users;
        if ( !str_cmp( name, "do_ussj" ))		return do_ussj;
	break;
    case 'v':
	if ( !str_cmp( name, "do_value" ))		return do_value;
	if ( !str_cmp( name, "do_version" ))		return do_version;
	if ( !str_cmp( name, "do_victories" ))		return do_victories;
	if ( !str_cmp( name, "do_vnums" ))		return do_vnums;
	if ( !str_cmp( name, "do_vsearch" ))		return do_vsearch;
	break;
    case 'w':
	if ( !str_cmp( name, "do_wake" ))		return do_wake;
	if ( !str_cmp( name, "do_wartalk" ))		return do_wartalk;
        if ( !str_cmp( name, "do_warn" ))		return do_warn;
	if ( !str_cmp( name, "do_watch" ))		return do_watch;
	if ( !str_cmp( name, "do_wear" ))		return do_wear;
	if ( !str_cmp( name, "do_weather" ))		return do_weather;
        if ( !str_cmp( name, "do_weight" ))		return do_weight;
	if ( !str_cmp( name, "do_west" ))		return do_west;
	if ( !str_cmp( name, "do_where" ))		return do_where;
	if ( !str_cmp( name, "do_whisper"))		return do_whisper;
	if ( !str_cmp( name, "do_who" ))		return do_who;
	if ( !str_cmp( name, "do_whois" ))		return do_whois;
	if ( !str_cmp( name, "do_wimpy" ))		return do_wimpy;
	if ( !str_cmp( name, "do_wizhelp" ))		return do_wizhelp;
	if ( !str_cmp( name, "do_wizlist" ))		return do_wizlist;
	if ( !str_cmp( name, "do_wizlock" ))		return do_wizlock;
	break;
    case 'y':
	if ( !str_cmp( name, "do_yell" ))		return do_yell;
	break;
    case 'z':
	if ( !str_cmp( name, "do_zap" ))		return do_zap;
        if ( !str_cmp( name, "do_zecho" ))              return do_zecho;
	if ( !str_cmp( name, "do_zones" ))		return do_zones;
    }
    return skill_notfound;
}


char *spell_name( SPELL_FUN *spell )
{
    if ( spell == spell_smaug )		    return "spell_smaug";
    if ( spell == spell_null )		    return "spell_null";
    return "reserved";
}

char *skill_name( DO_FUN *skill )
{
    static char buf[64];

    if ( skill == NULL )		return "reserved";
    if ( skill == do_aassign )		return "do_aassign";
    if ( skill == do_aaccept )          return "do_aaccept";
    if ( skill == do_activate )         return "do_activate";    
    if ( skill == do_absorb )		return "do_absorb";
    if ( skill == do_absorption )	return "do_absorption";
    if ( skill == do_adecline )         return "do_adecline";
    if ( skill == do_admin )		return "do_admin";
    if ( skill == do_addroom )		return "do_addroom";
    if ( skill == do_advance )		return "do_advance";
    if ( skill == do_afk )		return "do_afk";
    if ( skill == do_ahall )            return "do_ahall";
    if ( skill == do_aid )		return "do_aid";
    if ( skill == do_alist )		return "do_alist";
    if ( skill == do_allow )		return "do_allow";
    if ( skill == do_analyze )          return "do_analyze";
    if ( skill == do_androidmod )	return "do_androidmod";
    if ( skill == do_ansi )		return "do_ansi";
    if ( skill == do_answer )		return "do_answer";
    if ( skill == do_apply )		return "do_apply";
    if ( skill == do_appraise )		return "do_appraise";
    if ( skill == do_areas )		return "do_areas";
    if ( skill == do_arena )            return "do_arena";
    if ( skill == do_aset )		return "do_aset";
    if ( skill == do_ask )		return "do_ask";
    if ( skill == do_astat )		return "do_astat";
    if ( skill == do_at )		return "do_at";
    if ( skill == do_auction )		return "do_auction";
    if ( skill == do_authorize )	return "do_authorize";
    if ( skill == do_avtalk )		return "do_avtalk";
    if ( skill == do_awho )             return "do_awho";
    if ( skill == do_balzhur )		return "do_balzhur";
    if ( skill == do_bamfin )		return "do_bamfin";
    if ( skill == do_bamfout )		return "do_bamfout";
    if ( skill == do_ban )		return "do_ban";
    if ( skill == do_bank )             return "do_bank";
    if ( skill == do_barrier )		return "do_barrier";
    if ( skill == do_bestow )		return "do_bestow";
    if ( skill == do_bestowarea )	return "do_bestowarea";
    if ( skill == do_bet )              return "do_bet";
    if ( skill == do_betc )		return "do_betc";
    if ( skill == do_betr )		return "do_betr";
    if ( skill == do_bio )		return "do_bio";
    if ( skill == do_blast ) 		return "do_blast";
    if ( skill == do_bless )		return "do_bless";
    if ( skill == do_board )            return "do_board";
    if ( skill == do_boards )		return "do_boards";
    if ( skill == do_bodybag )		return "do_bodybag";
    if ( skill == do_bolt )		return "do_bolt";
    if ( skill == do_bset )		return "do_bset";
    if ( skill == do_bstat )		return "do_bstat";
    if ( skill == do_buildhq )		return "do_buildhq";
    if ( skill == do_burnattack )	return "do_burnattack";
    if ( skill == do_bury )		return "do_bury";
    if ( skill == do_buy )		return "do_buy";
    if ( skill == do_buyhome )		return "do_buyhome";
    if ( skill == do_buzz )		return "do_buzz";
    if ( skill == do_calculate ) 	return "do_calculate";
    if ( skill == do_calculator )       return "do_calculator";
    if ( skill == do_capture )		return "do_capture";
    if ( skill == do_cedit )		return "do_cedit";
    if ( skill == do_challenge )        return "do_challenge";
    if ( skill == do_channels )		return "do_channels";
    if ( skill == do_charge )           return "do_charge";
    if ( skill == do_chargeto )		return "do_chargeto";
    if ( skill == do_chaos )            return "do_chaos";
    if ( skill == do_chat )		return "do_chat";
    if ( skill == do_check_vnums )	return "do_check_vnums";
    if ( skill == do_checkradar )	return "do_checkradar";
    if ( skill == do_clans )		return "do_clans";
    if ( skill == do_clanset )          return "do_clanset";
    if ( skill == do_clanstatus )	return "do_clanstatus";
    if ( skill == do_clantalk )		return "do_clantalk";
    if ( skill == do_claw )		return "do_claw";
    if ( skill == do_cleanroom )        return "do_cleanroom";
    if ( skill == do_clear )		return "do_clear";
    if ( skill == do_climate )		return "do_climate";
    if ( skill == do_climb )		return "do_climb";
    if ( skill == do_close )		return "do_close";
    if ( skill == do_cmdtable )		return "do_cmdtable";
    if ( skill == do_cointoss )		return "do_cointoss";
    if ( skill == do_color )		return "do_color";
    if ( skill == do_commands )		return "do_commands";
    if ( skill == do_comment )		return "do_comment";
    if ( skill == do_compare )		return "do_compare";
    if ( skill == do_config )		return "do_config";
    if ( skill == do_connect )		return "do_connect";	
    if ( skill == do_consent )		return "do_consent";
    if ( skill == do_consider )		return "do_consider";
    if ( skill == do_coordinates )	return "do_coordinates";
    if ( skill == do_coords )		return "do_coords";
    if ( skill == do_copyover )         return "do_copyover";
    if ( skill == do_createclan )	return "do_createclan";
    if ( skill == do_credits )		return "do_credits";
    if ( skill == do_cset )		return "do_cset";
    if ( skill == do_cuttail )          return "do_cuttail";
    if ( skill == do_cycle ) 		return "do_cycle";
    if ( skill == do_dbzdlscore )	return "do_dbzdlscore";
    if ( skill == do_deadly )		return "do_deadly";
    if ( skill == do_defuse )		return "do_defuse";
    if ( skill == do_delay )		return "do_delay";
    if ( skill == do_delete )           return "do_delete";
    if ( skill == do_deleteroom )	return "do_deleteroom";
    if ( skill == do_deny )		return "do_deny";
    if ( skill == do_description )	return "do_description";
    if ( skill == do_destro )		return "do_destro";
    if ( skill == do_destroy )		return "do_destroy";
    if ( skill == do_direct )           return "do_direct";
    if ( skill == do_disconnect )	return "do_disconnect";
    if ( skill == do_disk )             return "do_disk";
    if ( skill == do_dmesg )		return "do_dmesg";
    if ( skill == do_dnd )		return "do_dnd";
    if ( skill == do_docks )		return "do_docks" ;
    if ( skill == do_dockstat )		return "do_dockstat";
    if ( skill == do_down )		return "do_down";
    if ( skill == do_drag )		return "do_drag";
     if ( skill == do_dragonball )	return "do_dragonball";
    if ( skill == do_drink )		return "do_drink";
    if ( skill == do_drop )		return "do_drop";
    if ( skill == do_east )		return "do_east";
    if ( skill == do_eat )		return "do_eat";
    if ( skill == do_ech )		return "do_ech";
    if ( skill == do_echo )		return "do_echo";
    if ( skill == do_elevate )          return "do_elevate";
    if ( skill == do_email )		return "do_email";
    if ( skill == do_emote )		return "do_emote";
    if ( skill == do_empty )		return "do_empty";
    if ( skill == do_energyball )       return "do_energyball";
    if ( skill == do_enter )		return "do_enter";
    if ( skill == do_equipment )	return "do_equipment";
    if ( skill == do_examine )		return "do_examine";
    if ( skill == do_exits )		return "do_exits";
    if ( skill == do_fakequit )		return "do_fakequit";
    if ( skill == do_fill )		return "do_fill";
    if ( skill == do_finalflash )	return "do_finalflash";
    if ( skill == do_findnote )		return "do_findnote";
    if ( skill == do_finger )           return "do_finger";
    if ( skill == do_fingerbeam )       return "do_fingerbeam";
    if ( skill == do_fixchar )		return "do_fixchar";
    if ( skill == do_flee )		return "do_flee";
    if ( skill == do_fly )              return "do_fly";
    if ( skill == do_foldarea )		return "do_foldarea";
    if ( skill == do_follow )		return "do_follow";
    if ( skill == do_for )		return "do_for";
    if ( skill == do_force )		return "do_force";
    if ( skill == do_forceclose )	return "do_forceclose";
    if ( skill == do_form_password )	return "do_form_password";
    if ( skill == do_fprompt )		return "do_fprompt";
    if ( skill == do_fquit )		return "do_fquit";
    if ( skill == do_freeze )		return "do_freeze";
    if ( skill == do_fshow )		return "do_fshow";
    if ( skill == do_fuse )		return "do_fuse";
    if ( skill == do_fusetalk )		return "do_fusetalk";
    if ( skill == do_galic )            return "do_galic";
    if ( skill == do_gamble )		return "do_gamble";
    if ( skill == do_gemote )           return "do_gemote";
    if ( skill == do_get )		return "do_get";
    if ( skill == do_gfighting )	return "do_gfighting";
    if ( skill == do_ghostattack )	return "do_ghostattack";
    if ( skill == do_give )		return "do_give";
    if ( skill == do_glance )		return "do_glance";
    if ( skill == do_global_boards )    return "do_global_boards";
    if ( skill == do_global_note )      return "do_global_note";
    if ( skill == do_gold )             return "do_gold";
    if ( skill == do_goto )		return "do_goto";
    if ( skill == do_gravset )          return "do_gravset";
    if ( skill == do_group )		return "do_group";
    if ( skill == do_growtail )         return "do_growtail";
    if ( skill == do_gtell )		return "do_gtell";
    if ( skill == do_gwhere )		return "do_gwhere";
    if ( skill == do_heal )		return "do_heal";
    if ( skill == do_hedit )		return "do_hedit";
    if ( skill == do_height ) 		return "do_height";
    if ( skill == do_hell )		return "do_hell";
    if ( skill == do_hellsflash )	return "do_hellsflash";
    if ( skill == do_help )		return "do_help";
    if ( skill == do_hiscore )		return "do_hiscore";
    if ( skill == do_hiscoset )		return "do_hiscoset";
    if ( skill == do_hl )		return "do_hl";
    if ( skill == do_hlist )		return "do_hlist";
    if ( skill == do_holylight )	return "do_holylight";
    if ( skill == do_homedesc )		return "do_homedesc";
    if ( skill == do_homeflag )		return "do_homeflag";
    if ( skill == do_homename )		return "do_homename";
    if ( skill == do_homepage )		return "do_homepage";
    if ( skill == do_homeroom )         return "do_homeroom";
    if ( skill == do_hset )		return "do_hset";
    if ( skill == do_icq_number ) 	return "do_icq_number";
    if ( skill == do_igive )            return "do_igive";
    if ( skill == do_ignore )		return "do_ignore";
    if ( skill == do_immortalize )	return "do_immortalize";
    if ( skill == do_add_imm_host )     return "do_add_imm_host";
    if ( skill == do_immtalk )		return "do_immtalk";
    if ( skill == do_imprison )		return "do_imprison";
    if ( skill == do_induct )		return "do_induct";
    if ( skill == do_installarea )	return "do_installarea";
    if ( skill == do_instaroom )	return "do_instaroom";
    if ( skill == do_instazone )	return "do_instazone";
    if ( skill == do_inventory )	return "do_inventory";
    if ( skill == do_invis )		return "do_invis";
    if ( skill == do_invite )		return "do_invite";
    if ( skill == do_ipcompare )	return "do_ipcompare";
    if ( skill == do_kaioken ) 		return "do_kaioken";
    if ( skill == do_kame )		return "do_kame";
    if ( skill == do_keypad )		return "do_keypad";
    if ( skill == do_khistory )		return "do_khistory";
    if ( skill == do_kickoff )          return "do_kickoff";
    if ( skill == do_kill )		return "do_kill";
    if ( skill == do_kilimeter )	return "do_kilimeter";
    if ( skill == do_koth )		return "do_koth";
    if ( skill == do_landmarks )	return "do_landmarks";
    if ( skill == do_launch )		return "do_launch";
    if ( skill == do_last )		return "do_last";
    if ( skill == do_laws )		return "do_laws";
    if ( skill == do_learn )            return "do_learn";
    if ( skill == do_leave )		return "do_leave";
    if ( skill == do_leaveship )        return "do_leaveship";
    if ( skill == do_level )		return "do_level";
    if ( skill == do_light )		return "do_light";
    if ( skill == do_list )		return "do_list";
    if ( skill == do_litterbug )	return "do_litterbug";
    if ( skill == do_loadarea )		return "do_loadarea";
    if ( skill == do_loadup )		return "do_loadup";
    if ( skill == do_lock )		return "do_lock";
    if ( skill == do_lockship )         return "do_lockship";
    if ( skill == do_log )		return "do_log";
    if ( skill == do_look )		return "do_look";
    if ( skill == do_low_purge )	return "do_low_purge";
    if ( skill == do_mailroom )		return "do_mailroom";
    if ( skill == do_makeboard )	return "do_makeboard";
    if ( skill == do_makeclan )		return "do_makeclan";
    if ( skill == do_makefree )         return "do_makefree";
    if ( skill == do_makerepair )	return "do_makerepair";
    if ( skill == do_makeship ) 	return "do_makeship";
    if ( skill == do_makeshop )		return "do_makeshop";
    if ( skill == do_makewizlist )	return "do_makewizlist";
    if ( skill == do_mapedit )		return "do_mapedit";
    if ( skill == do_maplegend )	return "do_maplegend";
    if ( skill == do_maplegend2 )        return "do_maplegend2";
    if ( skill == do_mapresets )	return "do_mapresets";
    if ( skill == do_mapsize )		return "do_mapsize";
    if ( skill == do_masenko ) 		return "do_masenko";
    if ( skill == do_massign )		return "do_massign";
    if ( skill == do_mcreate )		return "do_mcreate";
    if ( skill == do_mdelete )		return "do_mdelete";
    if ( skill == do_members )		return "do_members";
    if ( skill == do_memory )		return "do_memory";
    if ( skill == do_mental )		return "do_mental";
    if ( skill == do_mfind )		return "do_mfind";
    if ( skill == do_minvoke )		return "do_minvoke";
    if ( skill == do_mlist )		return "do_mlist";
    if ( skill == do_mortalize )        return "do_mortalize";
    if ( skill == do_mp_close_passage )	return "do_mp_close_passage";
    if ( skill == do_mp_damage )	return "do_mp_damage";
    if ( skill == do_mp_deposit )	return "do_mp_deposit";
    if ( skill == do_mp_fill_in )	return "do_mp_fill_in";
    if ( skill == do_mp_kidrain )       return "do_mp_kidrain";
    if ( skill == do_mp_kirestore )     return "do_mp_kirestore";
    if ( skill == do_mp_log )		return "do_mp_log";
    if ( skill == do_mp_open_passage )	return "do_mp_open_passage";
    if ( skill == do_mp_practice )	return "do_mp_practice";
    if ( skill == do_mp_restore )	return "do_mp_restore";
    if ( skill == do_mp_slay )		return "do_mp_slay";
    if ( skill == do_mp_stun )		return "do_mp_stun";
    if ( skill == do_mp_withdraw )	return "do_mp_withdraw";
    if ( skill == do_mpadvance )	return "do_mpadvance";
    if ( skill == do_mpasound )		return "do_mpasound";
    if ( skill == do_mpasupress )	return "do_mpasupress";
    if ( skill == do_mpat )		return "do_mpat";
    if ( skill == do_mpbodybag )	return "do_mpbodybag";
    if ( skill == do_mpclanlevelraise ) return "do_mpclanlevelraise";
    if ( skill == do_mpcopy )		return "do_mpcopy";
    if ( skill == do_mpdeadly )		return "do_mpdeadly";
    if ( skill == do_mpdelay )		return "do_mpdelay";
    if ( skill == do_mpdream )		return "do_mpdream";
    if ( skill == do_mpecho )		return "do_mpecho";
    if ( skill == do_mpechoaround )	return "do_mpechoaround";
    if ( skill == do_mpechoat )		return "do_mpechoat";
    if ( skill == do_mpechozone )	return "do_mpechozone";
    if ( skill == do_mpedit )		return "do_mpedit";
    if ( skill == do_mpforce )		return "do_mpforce";
    if ( skill == do_mpgivearmor )      return "do_mpgivearmor";
    if ( skill == do_mpgivebless )      return "do_mpgivebless";
    if ( skill == do_mpgivebio )	return "do_mpgivebio";
    if ( skill == do_mpgivegold )       return "do_mpgivegold";
    if ( skill == do_mpgivepl )         return "do_mpgivepl";
    if ( skill == do_mpgiverage )	return "do_mpgiverage";
    if ( skill == do_mpgiveunlock )     return "do_mpgiveunlock";
    if ( skill == do_mpgiveweapon )     return "do_mpgiveweapon";
    if ( skill == do_mpgoto )		return "do_mpgoto";
    if ( skill == do_mpinvis )		return "do_mpinvis";
    if ( skill == do_mpjunk )		return "do_mpjunk";
    if ( skill == do_mpkill )		return "do_mpkill";
    if ( skill == do_mpmload )		return "do_mpmload";
    if ( skill == do_mpmset )		return "do_mpmset";
    if ( skill == do_mpmudecho )	return "do_mpmudecho";
    if ( skill == do_mpmusic )		return "do_mpmusic";
    if ( skill == do_mpmusicaround )	return "do_mpmusicaround";
    if ( skill == do_mpmusicat )	return "do_mpmusicat";
    if ( skill == do_mpnothing )	return "do_mpnothing";
    if ( skill == do_mpoload )		return "do_mpoload";
    if ( skill == do_mposet )		return "do_mposet";
    if ( skill == do_mppardon )		return "do_mppardon";
    if ( skill == do_mppeace )		return "do_mppeace";
    if ( skill == do_mppkset )		return "do_mppkset";
    if ( skill == do_mppurge )		return "do_mppurge";
    if ( skill == do_mproto )		return "do_mproto";
    if ( skill == do_mpscatter )        return "do_mpscatter";
    if ( skill == do_mpsound )		return "do_mpsound";
    if ( skill == do_mpsoundaround )	return "do_mpsoundaround";
    if ( skill == do_mpsoundat )	return "do_mpsoundat";
    if ( skill == do_mpstat )		return "do_mpstat";
    if ( skill == do_mptransfer )	return "do_mptransfer";
    if ( skill == do_mpzecho )          return "do_mpzecho";
    if ( skill == do_mrange )		return "do_mrange";
    if ( skill == do_mreset )		return "do_mreset";
    if ( skill == do_mset )		return "do_mset";
    if ( skill == do_mstat )		return "do_mstat";
    if ( skill == do_mudstatus )	return "do_mudstatus";
    if ( skill == do_murde )		return "do_murde";
    if ( skill == do_murder )		return "do_murder";
    if ( skill == do_muse )		return "do_muse";
    if ( skill == do_music )		return "do_music";
    if ( skill == do_mutate )           return "do_mutate";
    if ( skill == do_mwhere )		return "do_mwhere";
    if ( skill == do_mystic )           return "do_mystic";
    if ( skill == do_name )		return "do_name";
    if ( skill == do_newbiechat )	return "do_newbiechat";
    if ( skill == do_newbieset )	return "do_newbieset";
    if ( skill == do_newpractice )	return "do_newpractice";
    if ( skill == do_news )             return "do_news";
    if ( skill == do_newscore )		return "do_newscore";
    if ( skill == do_newslist )		return "do_newslist";
    if ( skill == do_newzones )		return "do_newzones";
    if ( skill == do_nkick )            return "do_nkick";
    if ( skill == do_npunch )           return "do_npunch";
    if ( skill == do_noemote )		return "do_noemote";
    if ( skill == do_noresolve )	return "do_noresolve";
    if ( skill == do_north )		return "do_north";
    if ( skill == do_northeast )	return "do_northeast";
    if ( skill == do_northwest )	return "do_northwest";
    if ( skill == do_notell )		return "do_notell";
    if ( skill == do_notepeek )		return "do_notepeek";
    if ( skill == do_notitle )          return "do_notitle";
    if ( skill == do_noteroom )		return "do_noteroom";
    if ( skill == do_null )		return "do_null";
    if ( skill == do_oassign )		return "do_oassign";
    if ( skill == do_ocreate )		return "do_ocreate";
    if ( skill == do_odelete )		return "do_odelete";
    if ( skill == do_ofind )		return "do_ofind";
    if ( skill == do_ojunk )            return "do_ojunk";
    if ( skill == do_oinvoke )		return "do_oinvoke";
    if ( skill == do_oldscore )		return "do_oldscore";
    if ( skill == do_olist )		return "do_olist";
    if ( skill == do_ooc )              return "do_ooc";
    if ( skill == do_oozaru )           return "do_oozaru";
    if ( skill == do_opcopy )		return "do_opcopy";
    if ( skill == do_opedit )		return "do_opedit";
    if ( skill == do_open )		return "do_open";
    if ( skill == do_oproto )		return "do_oproto";
    if ( skill == do_opstat )		return "do_opstat";
    if ( skill == do_orange )		return "do_orange";
    if ( skill == do_oset )		return "do_oset";
    if ( skill == do_ostat )		return "do_ostat";
    if ( skill == do_outcast )		return "do_outcast";
    if ( skill == do_outlaw )		return "do_outlaw";
    if ( skill == do_owhere )		return "do_owhere";
    if ( skill == do_pager )		return "do_pager";
    if ( skill == do_pardon )		return "do_pardon";
    if ( skill == do_password )		return "do_password";
    if ( skill == do_pcrename )		return "do_pcrename";
    if ( skill == do_peace )		return "do_peace";
    if ( skill == do_pfiles )		return "do_pfiles";
    if ( skill == do_planets)		return "do_planets";
    if ( skill == do_plist )		return "do_plist";
    if ( skill == do_powerdown )        return "do_powerdown";
    if ( skill == do_powerup )          return "do_powerup";
    if ( skill == do_privacy )		return "do_privacy";
    if ( skill == do_prompt )		return "do_prompt";
    if ( skill == do_project )		return "do_project";
    if ( skill == do_propaganda )	return "do_propaganda";
    if ( skill == do_pull )		return "do_pull";
    if ( skill == do_purge )		return "do_purge";
    if ( skill == do_push )		return "do_push";
    if ( skill == do_put )		return "do_put";
    if ( skill == do_quaff )		return "do_quaff";
    if ( skill == do_qui )		return "do_qui";
    if ( skill == do_quit )		return "do_quit";
    if ( skill == do_racetalk )		return "do_racetalk";
    if ( skill == do_rank )		return "do_rank";
    if ( skill == do_rap )		return "do_rap";
    if ( skill == do_rassign )		return "do_rassign";
    if ( skill == do_rat )		return "do_rat";
    if ( skill == do_rdelete )		return "do_rdelete";
    if ( skill == do_reboo )		return "do_reboo";
    if ( skill == do_reboot )		return "do_reboot";
    if ( skill == do_recho )		return "do_recho";
    if ( skill == do_recite )		return "do_recite";
    if ( skill == do_redit )		return "do_redit";
    if ( skill == do_regoto )		return "do_regoto";
    if ( skill == do_release )          return "do_release";
    if ( skill == do_remove )		return "do_remove";
    if ( skill == do_rent )		return "do_rent";
    if ( skill == do_repair )		return "do_repair";
    if ( skill == do_repairset )	return "do_repairset";
    if ( skill == do_repairshops )	return "do_repairshops";
    if ( skill == do_repairstat )	return "do_repairstat";
    if ( skill == do_repeat )		return "do_repeat";
    if ( skill == do_reply )		return "do_reply";
    if ( skill == do_report )		return "do_report";
    if ( skill == do_reserve )		return "do_reserve";
    if ( skill == do_reset )		return "do_reset";
    if ( skill == do_rest )		return "do_rest";
    if ( skill == do_restore )		return "do_restore";
    if ( skill == do_restorefile )	return "do_restorefile";
    if ( skill == do_restoretime )	return "do_restoretime";
    if ( skill == do_restrict )		return "do_restrict";
    if ( skill == do_retell )		return "do_retell";
    if ( skill == do_retire )		return "do_retire";
    if ( skill == do_retran )		return "do_retran";
    if ( skill == do_retrieve )		return "do_retrieve";
    if ( skill == do_return )		return "do_return";
    if ( skill == do_rip )		return "do_rip";
    if ( skill == do_rlist )		return "do_rlist";
    if ( skill == do_rolldie )		return "do_rolldie";
    if ( skill == do_rpedit )		return "do_rpedit";
    if ( skill == do_rps )		return "do_rps";
    if ( skill == do_rpstat )		return "do_rpstat";
    if ( skill == do_rset )		return "do_rset";
    if ( skill == do_rstat )		return "do_rstat";
    if ( skill == do_run )		return "do_run";
    if ( skill == do_steam )		return "do_steam";
    if ( skill == do_sacrifice )	return "do_sacrifice";
    if ( skill == do_save )		return "do_save";
    if ( skill == do_saveall )          return "do_saveall";
    if ( skill == do_savearea )		return "do_savearea";
    if ( skill == do_say )		return "do_say";
    if ( skill == do_sbc ) 		return "do_sbc";
    if ( skill == do_scan )		return "do_scan";
    if ( skill == do_scatter )          return "do_scatter";
    if ( skill == do_scattershot )	return "do_scattershot";
    if ( skill == do_search )		return "do_search";
    if ( skill == do_sedit )		return "do_sedit";
    if ( skill == do_sell )		return "do_sell";
    if ( skill == do_sellhome )		return "do_sellhome";
    if ( skill == do_semimortal )       return "do_semimortal";
    if ( skill == do_sense )            return "do_sense";
    if ( skill == do_set_boot_time )	return "do_set_boot_time";
    if ( skill == do_setclan )		return "do_setclan";
    if ( skill == do_setdock )		return "do_setdock";
    if ( skill == do_setexit )		return "do_setexit";
    if ( skill == do_setmark )		return "do_setmark";
    if ( skill == do_setweather )	return "do_setweather";
    if ( skill == do_ships )		return "do_ships";
    if ( skill == do_shipset )		return "do_shipset";
    if ( skill == do_shipstat )		return "do_shipstat";
    if ( skill == do_shops )		return "do_shops";
    if ( skill == do_shopset )		return "do_shopset";
    if ( skill == do_shopstat )		return "do_shopstat";
    if ( skill == do_shout )		return "do_shout";
    if ( skill == do_shove )		return "do_shove";
    if ( skill == do_showclan )		return "do_showclan";
    if ( skill == do_showclass )	return "do_showclass";
    if ( skill == do_showmap )          return "do_showmap";
    if ( skill == do_showrace )		return "do_showrace";
    if ( skill == do_showweather )	return "do_showweather";
    if ( skill == do_shutdow )		return "do_shutdow";
    if ( skill == do_shutdown )		return "do_shutdown";
    if ( skill == do_silence )		return "do_silence";
    if ( skill == do_silencenew )	return "do_silencenew";
    if ( skill == do_sit )		return "do_sit";
    if ( skill == do_skillinfo )	return "do_skillinfo";
    if ( skill == do_skills )		return "do_skills";
    if ( skill == do_skin )		return "do_skin";
    if ( skill == do_sla )		return "do_sla";
    if ( skill == do_slay )		return "do_slay";
    if ( skill == do_sleep )		return "do_sleep";
    if ( skill == do_slevel )		return "do_slevel";
    if ( skill == do_slookup )		return "do_slookup";
    if ( skill == do_smoke )		return "do_smoke";
    if ( skill == do_snamek )           return "do_snamek";
    if ( skill == do_snoop )		return "do_snoop";
    if ( skill == do_sober )		return "do_sober";
    if ( skill == do_socials )		return "do_socials";
    if ( skill == do_south )		return "do_south";
    if ( skill == do_southeast )	return "do_southeast";
    if ( skill == do_southwest )	return "do_southwest";
    if ( skill == do_spar )             return "do_spar";
    if ( skill == do_spectate )         return "do_spectate";
    if ( skill == do_split )		return "do_split";
    if ( skill == do_sset )		return "do_sset";
    if ( skill == do_ssj )              return "do_ssj";
    if ( skill == do_stage )            return "do_stage";
    if ( skill == do_stand )		return "do_stand";
    if ( skill == do_start )		return "do_start";
    if ( skill == do_stat )		return "do_stat";
    if ( skill == do_statreport )	return "do_statreport";
    if ( skill == do_statshield )	return "do_statshield";
    if ( skill == do_status )           return "do_status";
    if ( skill == do_stopspec )         return "do_stopspec";
    if ( skill == do_strew )		return "do_strew";
    if ( skill == do_strip )		return "do_strip";
    if ( skill == do_study )		return "do_study";
    if ( skill == do_style )		return "do_style";
    if ( skill == do_summon )		return "do_summon";
    if ( skill == do_superhuman )	return "do_superhuman";
    if ( skill == do_suppress )		return "do_suppress";
    if ( skill == do_survey )		return "do_survey";
    if ( skill == do_switch )		return "do_switch";
    if ( skill == do_tail )		return "do_tail";
    if ( skill == do_tamp )		return "do_tamp";
    if ( skill == do_tell )		return "do_tell";
    if ( skill == do_think )		return "do_think";
    if ( skill == do_time )		return "do_time";
    if ( skill == do_timecmd )		return "do_timecmd";
    if ( skill == do_timers )		return "do_timers";
    if ( skill == do_title )		return "do_title";
    if ( skill == do_toggle )           return "do_toggle";
    if ( skill == do_togglechat)	return "do_togglechat";
    if ( skill == do_train )            return "do_train";
    if ( skill == do_traffic )		return "do_traffic";
    if ( skill == do_transfer )		return "do_transfer";
    if ( skill == do_transform )        return "do_transform";
    if ( skill == do_tribeam )          return "do_tribeam";
    if ( skill == do_trust )		return "do_trust";
    if ( skill == do_ultranamek )	return "do_ultranamek";
    if ( skill == do_unbless ) 		return "do_unbless";
    if ( skill == do_unbolt )		return "do_unbolt";
    if ( skill == do_uncover )		return "do_uncover";
    if ( skill == do_unfoldarea )	return "do_unfoldarea";
    if ( skill == do_unhell )		return "do_unhell";
    if ( skill == do_unlock )		return "do_unlock";
    if ( skill == do_unlockship )       return "do_unlockship";
    if ( skill == do_unsilence )        return "do_unsilence";
    if ( skill == do_unsilencenew )	return "do_unsilencenew";
    if ( skill == do_unsuppress )	return "do_unsuppress";
    if ( skill == do_up )		return "do_up";
    if ( skill == do_users )		return "do_users";
    if ( skill == do_ussj )		return "do_ussj";
    if ( skill == do_value )		return "do_value";
    if ( skill == do_version )		return "do_version";
    if ( skill == do_victories )	return "do_victories";
    if ( skill == do_vnums )		return "do_vnums";
    if ( skill == do_vsearch )          return "do_vsearch";
    if ( skill == do_wake )		return "do_wake";
    if ( skill == do_wartalk )		return "do_wartalk";
    if ( skill == do_warn )		return "do_warn";
    if ( skill == do_watch )		return "do_watch";
    if ( skill == do_wear )		return "do_wear";
    if ( skill == do_weather )		return "do_weather";
    if ( skill == do_weight )		return "do_weight";
    if ( skill == do_west )		return "do_west";
    if ( skill == do_where )		return "do_where";
    if ( skill == do_whisper )		return "do_whisper";
    if ( skill == do_who )		return "do_who";
    if ( skill == do_whois )		return "do_whois";
    if ( skill == do_wimpy )		return "do_wimpy";
    if ( skill == do_wizhelp )		return "do_wizhelp";
    if ( skill == do_wizlist )		return "do_wizlist";
    if ( skill == do_wizlock )		return "do_wizlock";
    if ( skill == do_yell )		return "do_yell";
    if ( skill == do_zap )		return "do_zap";
    if ( skill == do_zecho )            return "do_zecho";
    if ( skill == do_zones )		return "do_zones";

    sprintf( buf, "(%p)", skill );
    return buf;
}
*/

bool load_class_file( char *fname )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    struct class_type *class;
    int cl = -1;
    int tlev = 0;
    FILE *fp;

    sprintf( buf, "%s%s", CLASS_DIR, fname );
    if ( ( fp = fopen( buf, "r" ) ) == NULL )
    {
	perror( buf );
	return FALSE;
    }

    CREATE( class, struct class_type, 1 );

    /* Setup defaults for additions to class structure */

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

	case 'C':
	    KEY( "Class",	cl,			fread_number( fp )	);
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		fclose( fp );
		if ( cl < 0 || cl >= MAX_CLASS )
		{
		    sprintf( buf, "Load_class_file: Class (%s) bad/not found (%d)",
			class->who_name ? class->who_name : "name not found", cl );
		    bug( buf, 0 );
		    if ( class->who_name )
			STRFREE( class->who_name );
		    DISPOSE( class );
		    return FALSE;
		}
		class_table[cl] = class;
		return TRUE;
	    }
	    KEY( "ExpBase",	class->exp_base,	fread_number( fp )	);
	    break;

	case 'M':
	    KEY( "Mana",	class->fMana,		fread_number( fp )	);
	    break;

	case 'N':
	    KEY( "Name",	class->who_name,	fread_string( fp )	);
	    break;

	case 'T':
	    if ( !str_cmp( word, "Title" ) )
	    {
		if ( cl < 0 || cl >= MAX_CLASS )
		{
		    char *tmp;

		    sprintf( buf, "load_class_file: Title -- class bad/not found (%d)", cl );
		    bug( buf, 0 );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		}
		else
		if ( tlev < MAX_LEVEL+1 )
		{
		    title_table[cl][tlev][0] = fread_string_nohash( fp );
		    title_table[cl][tlev][1] = fread_string_nohash( fp );
		    ++tlev;
		}
		else
		    bug( "load_class_file: Too many titles" );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Thac0",	class->thac0_00,	fread_number( fp )	);
	    KEY( "Thac32",	class->thac0_32,	fread_number( fp )	);
	    break;

	case 'W':
	    KEY( "Weapon",	class->weapon,	fread_number( fp )	);
	    break;
	}
	if ( !fMatch )
	{
            sprintf( buf, "load_class_file: no match: %s", word );
	    bug( buf, 0 );
	}
    }
    return FALSE;
}

/*
 * Load in all the class files.
 */
void load_classes( )
{
    FILE *fpList;
    char *filename;
    char classlist[256];
    char buf[MAX_STRING_LENGTH];
    int i;
     
    MAX_PC_CLASS = 0;

    /*
     * Pre-init the class_table with blank classes
     */
    for ( i=0;i<MAX_CLASS;i++ )
	class_table[i] = NULL;
    	
    sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
    if ( ( fpList = fopen( classlist, "r" ) ) == NULL )
    {
	perror( classlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;

	if ( !load_class_file( filename ) )
	{
	  sprintf( buf, "Cannot load class file: %s", filename );
	  bug( buf, 0 );
	}
	else
	  MAX_PC_CLASS++;
    }
    fclose( fpList );
    for ( i=0;i<MAX_CLASS;i++ )
    {
    	if ( class_table[i] == NULL ) 
	{
	  CREATE( class_table[i], struct class_type, 1 );
	}
    }
    return;
}


void write_class_file( int cl )
{
    FILE *fpout;
    char buf[MAX_STRING_LENGTH];
    char filename[MAX_INPUT_LENGTH];
    struct class_type *class = class_table[cl];
    int x;

    sprintf( filename, "%s%s.class", CLASSDIR, class->who_name );
    if ( (fpout=fopen(filename, "w")) == NULL )
    {
	sprintf( buf, "Cannot open: %s for writing", filename );
	bug( buf, 0 );
	return;
    }
    fprintf( fpout, "Name        %s~\n",	class->who_name		);
    fprintf( fpout, "Class       %d\n",		cl			);
    fprintf( fpout, "Weapon      %d\n",		class->weapon		);
    fprintf( fpout, "Skilladept  %d\n",		class->skill_adept	);
    fprintf( fpout, "Thac0       %d\n",		class->thac0_00		);
    fprintf( fpout, "Thac32      %d\n",		class->thac0_32		);
    fprintf( fpout, "Mana        %d\n",		class->fMana		);
    fprintf( fpout, "Expbase     %d\n",		class->exp_base		);
    for ( x = 0; x <= MAX_LEVEL; x++ )
	fprintf( fpout, "Title\n%s~\n%s~\n",
		title_table[cl][x][0], title_table[cl][x][1] );
    fprintf( fpout, "End\n" );
    fclose( fpout );
}


/*
 * Load in all the race files.
 */
void load_races( )
{
    FILE *fpList;
    char *filename;
    char racelist[256];
    char buf[MAX_STRING_LENGTH];
    int i; 

    MAX_PC_RACE = 0;
   /*
    * Pre-init the race_table with blank races
    */
    for(i=0;i<MAX_RACE;i++)
    	race_table[i] = NULL;

    sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
    if ( ( fpList = fopen( racelist, "r" ) ) == NULL )
    {
	perror( racelist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;

	if ( !load_race_file( filename ) )
	{
	  sprintf( buf, "Cannot load race file: %s", filename );
	  bug( buf, 0 );
	}
	else
	  MAX_PC_RACE++;
    }
    for ( i = 0; i < MAX_RACE; i++ )
    {
        if ( race_table[i] == NULL )
        {
          CREATE( race_table[i], struct race_type, 1 );
          sprintf( race_table[i]->race_name, "%s", "unused" );
        }
    }
    fclose( fpList );
    return;
}

void write_race_file( int ra )
{
    FILE *fpout;
    char buf[MAX_STRING_LENGTH];
    char filename[MAX_INPUT_LENGTH];
    struct race_type *race = race_table[ra];
    int i;

    if( !race->race_name)
    {
	sprintf( buf, "Race %d has null name, not writing .race file.", ra );
	bug( buf, 0 );
	return;
    }

    sprintf( filename, "%s%s.race", RACEDIR, race->race_name );
    if ( (fpout=fopen(filename, "w+")) == NULL )
    {
	sprintf( buf, "Cannot open: %s for writing", filename );
	bug( buf, 0 );
	return;
    }

    fprintf( fpout, "Name        %s~\n",	race->race_name		);
    fprintf( fpout, "Race        %d\n",		ra  			);
    fprintf( fpout, "Min_Align  %d\n",		race->minalign		);
    fprintf( fpout, "Max_Align	%d\n",		race->maxalign		);
    fprintf( fpout, "Exp_Mult   %d\n",		race->exp_multiplier	); 
    fprintf( fpout, "Height     %d\n",		race->height		);
    fprintf( fpout, "Weight     %d\n",		race->weight		);
    for ( i = 0; i < MAX_WHERE_NAME; i++ )
	fprintf( fpout, "WhereName  %s~\n",
		race->where_name[i] );

    fprintf( fpout, "End\n" );
    fclose( fpout );
}


bool load_race_file( char *fname )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    char *race_name = NULL;
    bool fMatch;
    struct race_type *race;
    int ra = -1;
    FILE *fp;
    int i, wear=0;

    sprintf( buf, "%s%s", RACEDIR, fname );
    if ( ( fp = fopen( buf, "r" ) ) == NULL )
    {
	perror( buf );
	return FALSE;
    }

    CREATE( race, struct race_type, 1 );
    for(i=0;i<MAX_WHERE_NAME;i++)
       race->where_name[i] = where_name[i];

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


	case 'E':
            if ( !str_cmp( word, "End" ) )
            {
                fclose( fp );
                if ( ra < 0 || ra >= MAX_RACE )
                {
                    bug( "Load_race_file: Race (%s) bad/not found (%d)",
                        race->race_name ? race->race_name : "name not found", ra );
			  if( race_name )
  				STRFREE( race_name );
                    DISPOSE( race );
                    return FALSE;
                }
                race_table[ra] = race;
		    if( race_name )
		      STRFREE( race_name );
                return TRUE;
            }

	    KEY( "Exp_Mult",	race->exp_multiplier,	fread_number( fp )	);

	    break;


	case 'H':
	    KEY( "Height",	race->height,		fread_number( fp )	);
	    break;

	case 'M':
	    KEY( "Min_Align",	race->minalign,		fread_number( fp )	);
	    KEY( "Max_Align",	race->maxalign,		fread_number( fp )	);
	    break;

	case 'N':
	    KEY( "Name",	race_name,	fread_string( fp )	);
	    break;

	case 'R':
	    KEY( "Race",	ra,			fread_number( fp )	);
	    break;


	case 'W':
	    KEY( "Weight",	race->weight,		fread_number( fp )	);
	    if ( !str_cmp( word, "WhereName" ) )
	    {
		if ( ra < 0 || ra >= MAX_RACE )
		{
		    char *tmp;

		    sprintf( buf, "load_race_file: Title -- race bad/not found (%d)", ra );
		    bug( buf, 0 );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		    tmp = fread_string_nohash( fp );
		    DISPOSE( tmp );
		}
		else
		if ( wear < MAX_WHERE_NAME )
		{
		    race->where_name[wear] = fread_string_nohash( fp );
		    ++wear;
		}
		else
		    bug( "load_race_file: Too many where_names" );
		fMatch = TRUE;
		break;
	    }
	    break;
	}

        if ( race_name != NULL) 
             sprintf(race->race_name,"%-.16s",race_name);

	if ( !fMatch )
	{
            sprintf( buf, "load_race_file: no match: %s", word );
	    bug( buf, 0 );
	}
    }
    return FALSE;
}

/*
 * Function used by qsort to sort skills
 */
int skill_comp( SKILLTYPE **sk1, SKILLTYPE **sk2 )
{
    SKILLTYPE *skill1 = (*sk1);
    SKILLTYPE *skill2 = (*sk2);

    if ( !skill1 && skill2 )
	return 1;
    if ( skill1 && !skill2 )
	return -1;
    if ( !skill1 && !skill2 )
	return 0;
    if ( skill1->type < skill2->type )
	return -1;
    if ( skill1->type > skill2->type )
	return 1;
    return strcmp( skill1->name, skill2->name );
}



/*
 * Write skill data to a file
 */
void fwrite_skill( FILE *fpout, SKILLTYPE *skill )
{

    fprintf( fpout, "Name         %s~\n",	skill->name	);
    if ( skill->keyword && skill->keyword[0] != '\0' )
    fprintf( fpout, "Keyword      %s~\n",   skill->keyword );

    fprintf( fpout, "Type         %s\n",	skill_tname[skill->type]);
    fprintf( fpout, "Info         %d\n",	skill->info	);
    fprintf( fpout, "Flags        %d\n",	skill->flags	);
    if ( skill->target )
	fprintf( fpout, "Target       %d\n",	skill->target	);
    /*
     * store as new minpos (minpos>=100 flags new style in character loading)
     */
    if ( skill->minimum_position )
	fprintf( fpout, "Minpos       %d\n",	skill->minimum_position+100 );
    if ( skill->spell_sector )
    	fprintf( fpout, "Ssector      %d\n",	skill->spell_sector );
    if ( skill->saves )
	fprintf( fpout, "Saves        %d\n",	skill->saves    );
    if ( skill->slot )
	fprintf( fpout, "Slot         %d\n",	skill->slot	);
    if ( skill->min_mana )
	fprintf( fpout, "Mana         %d\n",	skill->min_mana );

    if ( skill->beats )
	fprintf( fpout, "Rounds       %d\n",	skill->beats	);
    if ( skill->radius )
        fprintf( fpout, "Radius	      %d\n",	skill->radius	);
    if ( skill->range )
	fprintf( fpout, "Range        %d\n",	skill->range	);
   if( skill->skill_fun )
      fprintf( fpout, "Code         %s\n", skill->skill_fun_name );

    fprintf( fpout, "Dammsg       %s~\n",	skill->noun_damage );
    if ( skill->msg_off && skill->msg_off[0] != '\0' )
	fprintf( fpout, "Wearoff      %s~\n",	skill->msg_off	);

    if ( skill->hit_char && skill->hit_char[0] != '\0' )
	fprintf( fpout, "Hitchar      %s~\n",	skill->hit_char );
    if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
	fprintf( fpout, "Hitvict      %s~\n",	skill->hit_vict );
    if ( skill->hit_room && skill->hit_room[0] != '\0' )
	fprintf( fpout, "Hitroom      %s~\n",	skill->hit_room );
    if ( skill->hit_dest && skill->hit_dest[0] != '\0' )
	fprintf( fpout, "Hitdest      %s~\n",	skill->hit_dest );

    if ( skill->miss_char && skill->miss_char[0] != '\0' )
	fprintf( fpout, "Misschar     %s~\n",	skill->miss_char );
    if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
	fprintf( fpout, "Missvict     %s~\n",	skill->miss_vict );
    if ( skill->miss_room && skill->miss_room[0] != '\0' )
	fprintf( fpout, "Missroom     %s~\n",	skill->miss_room );

    if ( skill->die_char && skill->die_char[0] != '\0' )
	fprintf( fpout, "Diechar      %s~\n",	skill->die_char );
    if ( skill->die_vict && skill->die_vict[0] != '\0' )
	fprintf( fpout, "Dievict      %s~\n",	skill->die_vict );
    if ( skill->die_room && skill->die_room[0] != '\0' )
	fprintf( fpout, "Dieroom      %s~\n",	skill->die_room );

    if ( skill->imm_char && skill->imm_char[0] != '\0' )
	fprintf( fpout, "Immchar      %s~\n",	skill->imm_char );
    if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	fprintf( fpout, "Immvict      %s~\n",	skill->imm_vict );
    if ( skill->imm_room && skill->imm_room[0] != '\0' )
	fprintf( fpout, "Immroom      %s~\n",	skill->imm_room );

    if ( skill->dice && skill->dice[0] != '\0' )
	fprintf( fpout, "Dice         %s~\n",	skill->dice );
    if ( skill->value )
	fprintf( fpout, "Value        %d\n",	skill->value );
    if ( skill->difficulty )
	fprintf( fpout, "Difficulty   %d\n",	skill->difficulty );
    if ( skill->participants )
	fprintf( fpout, "Participants %d\n",	skill->participants );
    if ( skill->components && skill->components[0] != '\0' )
	fprintf( fpout, "Components   %s~\n",	skill->components );
    if ( skill->teachers && skill->teachers[0] != '\0' )
	fprintf( fpout, "Teachers     %s~\n",	skill->teachers );

	fprintf( fpout, "Minlevel     %d\n",	skill->min_level		);
    if ( skill->type == SKILL_SSKILL || skill->type == SKILL_ASKILL )
    {
      if ( skill->pl )
        fprintf( fpout, "Powerlevel   %lld\n",    skill->pl );


    }
    if ( skill->type == SKILL_ASKILL )
    {
      if ( !xIS_EMPTY(skill->raceskill) )
         fprintf( fpout, "RaceSkill     %s\n",      print_bitvector(&skill->raceskill)       );

    }
    fprintf( fpout, "End\n\n" );
}




/*
 * Save the socials to disk
 */
void save_socials()
{
    FILE *fpout;
    SOCIALTYPE *social;
    int x;

    if ( (fpout=fopen( SOCIAL_FILE, "w" )) == NULL )
    {
	bug( "Cannot open socials.dat for writting", 0 );
	perror( SOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
	for ( social = social_index[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
	    {
		bug( "Save_socials: blank social in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#SOCIAL\n" );
	    fprintf( fpout, "Name        %s~\n",	social->name );
	    if ( social->char_no_arg )
		fprintf( fpout, "CharNoArg   %s~\n",	social->char_no_arg );
	    else
	        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
	    if ( social->others_no_arg )
		fprintf( fpout, "OthersNoArg %s~\n",	social->others_no_arg );
	    if ( social->char_found )
		fprintf( fpout, "CharFound   %s~\n",	social->char_found );
	    if ( social->others_found )
		fprintf( fpout, "OthersFound %s~\n",	social->others_found );
	    if ( social->vict_found )
		fprintf( fpout, "VictFound   %s~\n",	social->vict_found );
	    if ( social->char_auto )
		fprintf( fpout, "CharAuto    %s~\n",	social->char_auto );
	    if ( social->others_auto )
		fprintf( fpout, "OthersAuto  %s~\n",	social->others_auto );
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

int get_skill( char *skilltype )
{
    if ( !str_cmp( skilltype, "SSkill" ) )
      return SKILL_SSKILL;
    if ( !str_cmp( skilltype, "ASkill" ) )
      return SKILL_ASKILL;

    return SKILL_UNKNOWN;
}

/*
 * Save the commands to disk
 * Added flags Aug 25, 1997 --Shaddai
 */
void save_commands()
{
    FILE *fpout;
    CMDTYPE *command;
    int x;

    if ( (fpout=fopen( COMMAND_FILE, "w" )) == NULL )
    {
	bug( "Cannot open commands.dat for writing", 0 );
	perror( COMMAND_FILE );
	return;
    }

    for ( x = 0; x < 126; x++ )
    {
	for ( command = command_hash[x]; command; command = command->next )
	{
	    if ( !command->name || command->name[0] == '\0' )
	    {
		bug( "Save_commands: blank command in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#COMMAND\n" );
	    fprintf( fpout, "Name        %s~\n", command->name		);
            fprintf( fpout, "Code        %s\n", command->fun_name?command->fun_name:"" ); // Modded to use new field - Trax
/* Oops I think this may be a bad thing so I changed it -- Shaddai */
	    if ( command->position < 100 )
	       fprintf( fpout, "Position    %d\n",	 command->position+100);
	    else
	       fprintf( fpout, "Position    %d\n",	 command->position);
	    fprintf( fpout, "Level       %d\n",	 command->level		);
	    fprintf( fpout, "Log         %d\n",	 command->log		);
    	    fprintf( fpout, "Ooc		 %d\n",	 command->ooc		);
            fprintf( fpout, "Fuse		 %d\n",	 command->fuse		);

            if ( command->flags )
                fprintf( fpout, "Flags       %d\n",  command->flags);
	    fprintf( fpout, "End\n\n" );
	}
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}

SKILLTYPE *fread_skill( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    bool got_info = FALSE;
    SKILLTYPE *skill;

    CREATE( skill, SKILLTYPE, 1 );
    skill->slot = 0;
    skill->min_mana = 0;
    skill->target = 0;
    skill->skill_fun = NULL;
    skill->spell_fun = NULL;
    skill->spell_sector = 0;


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

	case 'C':
	    if ( !str_cmp( word, "Code" ) )
	    {
		SPELL_FUN *spellfun;
		DO_FUN *dofun;
		char *w = fread_word( fp );
		
		fMatch = TRUE;
		if( !str_prefix( "do_", w ) && ( dofun = skill_function(w) ) != skill_notfound )
		{
		   skill->skill_fun = dofun;
		   skill->spell_fun = NULL;
		   skill->skill_fun_name = str_dup(w);
		}
		else if( str_prefix( "do_", w ) && ( spellfun = spell_function(w) ) != spell_notfound )
		{
		   skill->spell_fun = spellfun;
		   skill->skill_fun = NULL;
		   skill->spell_fun_name = str_dup(w);
		}
		else
		{
		   bug( "fread_skill: unknown skill/spell %s", w );
		   skill->spell_fun = spell_null;
		}
		break;
	    }

/*
	    if ( !str_cmp( word, "Code" ) )
	    {
		DO_FUN	  *dofun;
		char	  *w = fread_word(fp);
		
		fMatch = TRUE;
		if ( (dofun=skill_function(w)) != skill_notfound )
		{
		   skill->skill_fun = dofun;
		}
		else
		{
		   bug( "fread_skill: unknown skill %s", w );
		}
		break;
	    }
*/
	    KEY( "Components",	skill->components,	fread_string_nohash( fp ) );
	    break;
 
	case 'D':
            KEY( "Dammsg",	skill->noun_damage,	fread_string_nohash( fp ) );
	    KEY( "Dice",	skill->dice,		fread_string_nohash( fp ) );
	    KEY( "Diechar",	skill->die_char,	fread_string_nohash( fp ) );
	    KEY( "Dieroom",	skill->die_room,	fread_string_nohash( fp ) );
	    KEY( "Dievict",	skill->die_vict,	fread_string_nohash( fp ) );
	    KEY( "Difficulty",	skill->difficulty,	fread_number( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( skill->saves != 0 && SPELL_SAVE(skill) == SE_NONE )
		{
		    bug( "fread_skill(%s):  Has saving throw (%d) with no saving effect.",
			skill->name, skill->saves );
		    SET_SSAV(skill, SE_NEGATE);
		}
		return skill;
	    }
	    break;
	    
	case 'F':
	    if ( !str_cmp( word, "Flags" ) )
	    {
		skill->flags = fread_number(fp);
		/*
		 * convert to new style			-Thoric
		 */
		if ( !got_info )
		{
		    skill->info = skill->flags & (BV11-1);
		    if ( IS_SET(skill->flags, OLD_SF_SAVE_NEGATES) )
		    {
			if ( IS_SET(skill->flags, OLD_SF_SAVE_HALF_DAMAGE) )
			{
			    SET_SSAV(skill, SE_QUARTERDAM);
			    REMOVE_BIT(skill->flags, OLD_SF_SAVE_HALF_DAMAGE);
			}
			else
			    SET_SSAV(skill, SE_NEGATE);
			REMOVE_BIT(skill->flags, OLD_SF_SAVE_NEGATES);
		    }
		    else
		    if ( IS_SET(skill->flags, OLD_SF_SAVE_HALF_DAMAGE) )
		    {
			SET_SSAV(skill, SE_HALFDAM);
			REMOVE_BIT(skill->flags, OLD_SF_SAVE_HALF_DAMAGE);
		    }
		    skill->flags >>= 11;
		}
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'H':
	    KEY( "Hitchar",	skill->hit_char,	fread_string_nohash( fp ) );
	    KEY( "Hitdest",	skill->hit_dest,	fread_string_nohash( fp ) );
	    KEY( "Hitroom",	skill->hit_room,	fread_string_nohash( fp ) );
	    KEY( "Hitvict",	skill->hit_vict,	fread_string_nohash( fp ) );
	    break;

	case 'I':
	    KEY( "Immchar",	skill->imm_char,	fread_string_nohash( fp ) );
	    KEY( "Immroom",	skill->imm_room,	fread_string_nohash( fp ) );
	    KEY( "Immvict",	skill->imm_vict,	fread_string_nohash( fp ) );
	    if ( !str_cmp( word, "Info" ) )
	    {
		skill->info = fread_number(fp);
		got_info = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

        case 'K':
            KEY( "Keyword",    skill->keyword,       fread_string_nohash( fp ) );

            break;
	case 'M':
	    KEY( "Mana",	skill->min_mana,	fread_number( fp ) );

	    if ( !str_cmp( word, "Minlevel" ) )
	    {
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    /*KEY( "Minpos",	skill->minimum_position, fread_number( fp ) ); */
	   /*
	    * 
	    */
	    if ( !str_cmp(word, "Minpos") )
	    {
		fMatch = TRUE;
		skill->minimum_position = fread_number(fp);
		if ( skill->minimum_position < 100 )
		{
		    switch( skill->minimum_position )
		    {
			default:
			case 0:
			case 1:
			case 2:
			case 3:
			case 4: break;
			case 5: skill->minimum_position=6; break;
			case 6: skill->minimum_position=8; break;
			case 7: skill->minimum_position=9; break;
			case 8: skill->minimum_position=12; break;
			case 9: skill->minimum_position=13; break;
			case 10: skill->minimum_position=14; break;
			case 11: skill->minimum_position=15; break;
		    }
		}
		else
		    skill->minimum_position-=100;
		break;
	    }

	    KEY( "Misschar",	skill->miss_char,	fread_string_nohash( fp ) );
	    KEY( "Missroom",	skill->miss_room,	fread_string_nohash( fp ) );
	    KEY( "Missvict",	skill->miss_vict,	fread_string_nohash( fp ) );
	    break;
	
	case 'N':
            KEY( "Name",	skill->name,		fread_string_nohash( fp ) );

	    break;

	case 'P':
	    KEY( "Participants",skill->participants,	fread_number( fp ) );
            KEY( "Powerlevel", skill->pl,		fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Radius",	skill->radius,		fread_number( fp ) );
	    KEY( "Range",	skill->range,		fread_number( fp ) );
	    KEY( "Rounds",	skill->beats,		fread_number( fp ) );
            KEY( "Raceskill",	skill->raceskill,	fread_bitvector( fp ) );

	    break;

	case 'S':
	    KEY( "Saves",	skill->saves,		fread_number( fp ) );
	    KEY( "Slot",	skill->slot,		fread_number( fp ) );
	    KEY( "Ssector",     skill->spell_sector,    fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Target",	skill->target,		fread_number( fp ) );
	    KEY( "Teachers",	skill->teachers,	fread_string_nohash( fp ) );
	    KEY( "Type",	skill->type,  get_skill(fread_word( fp ))  );
	    break;

	case 'V':
	    KEY( "Value",	skill->value,		fread_number( fp ) );
	    break;

	case 'W':
	    KEY( "Wearoff",	skill->msg_off,		fread_string_nohash( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_skill: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_askill_table()
{
    FILE *fp;

    if ( ( fp = fopen( ASKILL_FILE, "r" ) ) != NULL )
    {
        top_askill = 0;
        for ( ;; )
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
                bug( "Load_askill_table: # not found.", 0 );
                break;
            }


            word = fread_word( fp );
            if ( !str_cmp( word, "ASKILL"      ) )
            {
                if ( top_askill >= MAX_ASKILL )
                {
                    bug( "load_askill_table: more askill than MAX_ASKILL %d", MAX_ASKILL );
                    fclose( fp );
                    return;
                }
                askill_table[top_askill++] = fread_skill( fp );
                if ( askill_table[top_askill-1]->slot == 0 )
                    askill_table[top_askill-1]->slot = top_askill-1;
                continue;
            }
            else
            if ( !str_cmp( word, "END"  ) )
                break;
            else
            {
                bug( "Load_askill_table: bad section.", 0 );
                continue;
            }
        }
        fclose( fp );
    }
    else
    {
        bug( "Cannot open askill.dat", 0 );
        exit(0);
    }

}

void load_sskill_table()
{
    FILE *fp;

    if ( ( fp = fopen( SSKILL_FILE, "r" ) ) != NULL )
    {
        top_sskill = 0;
        for ( ;; )
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
                bug( "Load_sskill_table: # not found.", 0 );
                break;
            }

            word = fread_word( fp );
            if ( !str_cmp( word, "SSKILL"      ) )
            {
                if ( top_sskill >= MAX_SSKILL )
                {
                    bug( "load_sskill_table: more sskill than MAX_SSKILL %d", MAX_SSKILL );
                    fclose( fp );
                    return;
                }
                sskill_table[top_sskill++] = fread_skill( fp );
                if ( sskill_table[top_sskill-1]->slot == 0 )
                    sskill_table[top_sskill-1]->slot = top_sskill-1;
                continue;
            }
            else
            if ( !str_cmp( word, "END"  ) )
                break;
            else
            {
                bug( "Load_sskill_table: bad section.", 0 );
                continue;
            }
        }
        fclose( fp );
    }
    else
    {
        bug( "Cannot open sskill.dat", 0 );
        exit(0);
    }
}

void save_sskill_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( SSKILL_FILE, "w" )) == NULL )
    {
        bug( "Cannot open sskill.dat for writting", 0 );
        perror( SSKILL_FILE );
        return;
    }

    for ( x = 0; x < top_sskill; x++ )
    {
        if ( !sskill_table[x]->name || sskill_table[x]->name[0] == '\0' )
           break;
        fprintf( fpout, "#SSKILL\n" );
        fwrite_skill( fpout, sskill_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}
void save_askill_table()
{
    int x;
    FILE *fpout;

    if ( (fpout=fopen( ASKILL_FILE, "w" )) == NULL )
    {
        bug( "Cannot open askill.dat for writting", 0 );
        perror( ASKILL_FILE );
        return;
    }

    for ( x = 0; x < top_askill; x++ )
    {
        if ( !askill_table[x]->name || askill_table[x]->name[0] == '\0' )
           break;
        fprintf( fpout, "#ASKILL\n" );
        fwrite_skill( fpout, askill_table[x] );
    }
    fprintf( fpout, "#END\n" );
    fclose( fpout );
}




void fread_social( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    SOCIALTYPE *social;

    CREATE( social, SOCIALTYPE, 1 );

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "CharNoArg",	social->char_no_arg,	fread_string_nohash(fp) );
	    KEY( "CharFound",	social->char_found,	fread_string_nohash(fp) );
	    KEY( "CharAuto",	social->char_auto,	fread_string_nohash(fp) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    KEY( "Name",	social->name,		fread_string_nohash(fp) );
	    break;

	case 'O':
	    KEY( "OthersNoArg",	social->others_no_arg,	fread_string_nohash(fp) );
	    KEY( "OthersFound",	social->others_found,	fread_string_nohash(fp) );
	    KEY( "OthersAuto",	social->others_auto,	fread_string_nohash(fp) );
	    break;

	case 'V':
	    KEY( "VictFound",	social->vict_found,	fread_string_nohash(fp) );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_socials()
{
    FILE *fp;

    if ( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
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
                bug( "Load_socials: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "SOCIAL"      ) )
	    {
                fread_social( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_socials: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open socials.dat", 0 );
 	exit(0);
    }
}

/*
 *  Added the flags Aug 25, 1997 --Shaddai
 */

void fread_command( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    CMDTYPE *command;

    CREATE( command, CMDTYPE, 1 );
    command->lag_count = 0; /* can't have caused lag yet... FB */
    command->flags   = 0;  /* Default to no flags set */

    for ( ;; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    KEY( "Code",	command->fun_name, str_dup( fread_word( fp ) ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if( !command->name )
		{
		   bug( "%s", "Fread_command: Name not found" );
		   free_command( command );
		   return;
		}
		if( !command->fun_name )
		{
		   bug( "fread_command: No function name supplied for %s", command->name );
		   free_command( command );
		   return;
		}
		/*
	 	 * Mods by Trax
		 * Fread in code into char* and try linkage here then
		 * deal in the "usual" way I suppose..
		 */
	      command->do_fun = skill_function( command->fun_name );
		if( command->do_fun == skill_notfound )
		{
		   bug( "Fread_command: Function %s not found for %s", command->fun_name, command->name );
		   free_command( command );
		   return;
		}
		add_command( command );
		return;
	    }
	    break;
        case 'F':
            KEY ("Flags", command->flags, fread_number (fp));
            KEY( "Fuse",         command->fuse,           fread_number(fp) );
            break;

	case 'L':
	    KEY( "Level",	command->level,		fread_number(fp) );
	    KEY( "Log",		command->log,		fread_number(fp) );
	    break;

	case 'N':
	    KEY( "Name",	command->name,		fread_string_nohash(fp) );
	    break;
        case 'O':
  	    KEY( "Ooc",		command->ooc,		fread_number(fp) );
            break;
	case 'P':
	    /* KEY( "Position",	command->position,	fread_number(fp) ); */
	    if ( !str_cmp(word, "Position") )
	    {
		fMatch = TRUE;
		command->position = fread_number(fp);
		if( command->position<100 )
		{
		    switch(command->position)
		    {
			default:
			case 0:
			case 1:
			case 2:
			case 3:
			case 4: break;
			case 5: command->position=6; break;
			case 6: command->position=8; break;
			case 7: command->position=9; break;
 			case 8: command->position=12; break;
			case 9: command->position=13; break;
			case 10: command->position=14; break;
			case 11: command->position=15; break;
		    }
		}
		else
		    command->position-=100;
		break;
	    }
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_command: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

void load_commands()
{
    FILE *fp;

    if ( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
    {
	top_sn = 0;
	for ( ;; )
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
                bug( "Load_commands: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
            if ( !str_cmp( word, "COMMAND"      ) )
	    {
                fread_command( fp );
	    	continue;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
                bug( "Load_commands: bad section.", 0 );
		continue;
	    }
	}
	fclose( fp );
    }
    else
    {
	bug( "Cannot open commands.dat", 0 );
 	exit(0);
    }

}

void save_classes()
{
    int x;

    for ( x = 0; x < MAX_PC_CLASS; x++ )
      write_class_file( x );
}

