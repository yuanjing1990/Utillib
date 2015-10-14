ifeq ($(platform),) 
	platform=WIN32
endif

ifeq ($(platform),WIN32)
SRC_EXTEND=.cpp
OBJ_EXTEND=.obj
EXE_EXTEND=.exe

CPP=cl.exe
LD=link.exe

#compile flags setting
CPPFLAGS+=-D_WIN32_WINNT=0x0501
CPPFLAGS+=/MDd
CPPFLAGS+=/EHsc
#LDFLAGS+=/MANIFEST:no
LDFLAGS+=/DYNAMICBASE

#default rule setting
%.exe: %.obj
	$(LD) $(LDFLAGS) $< /OUT:$@
%.obj: %.cpp
	$(CPP) /c $(CPPFLAGS) $< /Fo$@
endif

ifeq ($(platform),LINUX)
SRC_EXTEND=.cpp
OBJ_EXTEND=.o
EXE_EXTEND=.exe

CPP=g++
CC=gcc
LD=ld

#compile flags setting
CPPFLAGS+=-DDEBUG -Iwtoeutil/ -g
LDFLAGS+=-Lwtoeutil -lwtoeutil -lboost_system

#default rule setting
%$(EXE_EXTEND): %$(OBJ_EXTEND)
	$(CPP) $< -o $@ $(LDFLAGS)
%$(OBJ_EXTEND): %$(SRC_EXTEND)
	$(CPP) -c $(CPPFLAGS) $< -o $@
endif

#source file,obj file and exe file setting
SRC_FILES=$(shell ls *$(SRC_EXTEND))
OBJ_FILES=$(SRC_FILES:%$(SRC_EXTEND)=%$(OBJ_EXTEND))
EXE_FILES=$(SRC_FILES:%$(SRC_EXTEND)=%$(EXE_EXTEND))

#dest setting
all:$(EXE_FILES)

clean:
	@-rm -rf $(OBJ_FILES) $(EXE_FILES) *.ilk *.pdb *.manifest

.PHONY:all clean
