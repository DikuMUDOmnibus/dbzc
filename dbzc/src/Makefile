CC      = gcc
#PROF    = -p
NOCRYPT =

#Uncomment to compile in Cygwin
#CYGWIN = -DCYGWIN

#Uncomment if compiling in Windows under Cygwin
#WINDOZE = 1

# Uncomment the two lines below if compiling on a Solaris box
#SOLARIS_FLAG = -Dsun -DSYSV
#SOLARIS_LINK = -lnsl -lsocket

#Uncomment the line below if you are getting undefined crypt errors
NEED_CRYPT = -lcrypt

#Uncomment the line below if you want a performance increase though beware
#your core files may not be as much of a benefit if you do.
#OPT_FLAG = -finline-functions -funroll-loops -fdefer-pop -fstrength-reduce

C_FLAGS = $(OPT_FLAG) -O -g2 -Wall $(CYGWIN) $(PROF) $(NOCRYPT) $(SOLARIS_FLAG) -export-dynamic
L_FLAGS = $(OPT_FLAG) $(PROF) $(SOLARIS_LINK) $(NEED_CRYPT) -lm 

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o mortgames.o ban.o boards.o \
          build.o clans.o color.o comm.o comments.o const.o copyover.o db.o dbzfunc.o dbzmisc.o \
          dbzskills.o dock.o editor.o fight.o finger.o gamble.o gboards.o handler.o hashstr.o \
          hiscores.o imm_host.o interp.o magic.o malloc.o makeobjs.o misc.o mpxset.o mud_comm.o mud_prog.o \
          overland.o pfiles.o player.o powerups.o reset.o save.o ships.o shops.o \
          skills.o special.o tables.o track.o update.o changes.o\

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c mortgames.c ban.c boards.c \
          build.c clans.c color.c comm.c comments.c const.c copyover.c db.c dbzfunc.o dbzmisc.c \
          dbzskills.c dock.c editor.c fight.c finger.c gamble.c gboards.c handler.c hashstr.c \
          hiscores.c imm_host.c interp.c magic.c malloc.c makeobjs.c misc.c mpxset.c mud_comm.c mud_prog.c \
          overland.c pfiles.c player.c powerups.c reset.c save.c ships.c shops.c \
          skills.c special.c tables.c track.c update.c changes.c\

H_FILES = mud.h color.h bet.h copyover.h gboards.h dbzskills.h pfiles.h overland.h malloc.h

all:
	$(MAKE) dbzc

dbzc:	$(O_FILES)
	rm -f dbzc
	$(CC) -export-dynamic -o dbzc $(O_FILES) $(L_FLAGS)
	chmod g+w dbzc
	chmod a+x dbzc
	chmod g+w $(O_FILES)
	
clean:
	rm -f *.o dbzc *~

.c.o: mud.h
	$(CC) -c $(C_FLAGS) $<
