COPT= -mhuge
LOPT=

EXE=mixtest.exe
LIB=smix.lib

# Do a complete rebuild if the makefile has changed
MAK=makefile

$(EXE) : mixtest.obj $(LIB) $(MAK)
  bcc $(COPT) -e$(EXE) mixtest.obj $(LIB)

mixtest.obj : mixtest.c smix.h detect.h $(MAK)
  bcc -c $(COPT) mixtest.c

smix.lib : smix.obj xms.obj detect.obj $(MAK)
  tlib smix.lib +-smix.obj +-xms.obj +-detect.obj

xms.obj : xms.c $(MAK)
  bcc -c $(COPT) xms.c

detect.obj : detect.c $(MAK)
  bcc -c $(COPT) detect.c

smix.obj : smix.c xms.h $(MAK)
  bcc -c $(COPT) smix.c
