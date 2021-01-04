ifeq ($(platform),) 
	platform=LINUX
endif

MODULE = wtoeutil

ifeq ($(platform),WIN32)
SRC_EXTEND=.cpp
OBJ_EXTEND=.obj
DLL_EXTEND=.dll
EXE_EXTEND=.exe

CPP=cl.exe
LD=link.exe

#compile flags setting
CPPFLAGS+=-D_WIN32_WINNT=0x0501 -DUTILLIB
CPPFLAGS+=/MDd
CPPFLAGS+=/EHsc
#LDFLAGS+=/MANIFEST:no
LDFLAGS+=/DYNAMICBASE

#default rule setting
%$(DLL_EXTEND): %$(OBJ_EXTEND)
	$(LD) $(LDFLAGS) $< /OUT:$@
%$(OBJ_EXTEND): %$(SRC_EXTEND)
	$(CPP) /c $(CPPFLAGS) $< /Fo$@
endif #WIN32

ifeq ($(platform),LINUX)
SRC_EXTEND=.cpp
OBJ_EXTEND=.o
DLL_EXTEND=.so
EXE_EXTEND=.exe

CPP=g++
CC=gcc
LD=ld

#compile flags setting
CPPFLAGS += -fPIC -DDEBUG -DUTILLIB -g -Iinclude/
LDFLAGS := -shared 

#LDFLAGS += -L/usr/lib/x86_64-linux-gnu
LDFLAGS += -lboost_system -lboost_thread -lboost_filesystem

#default rule setting
bin/%$(DLL_EXTEND): obj/%$(OBJ_EXTEND)
	$(CPP) $< -o $@ $(LDFLAGS)
obj/%$(OBJ_EXTEND): src/%$(SRC_EXTEND)
	$(CPP) -c $(CPPFLAGS) $< -o $@

bin/%$(EXE_EXTEND): obj/%$(OBJ_EXTEND)
	$(CPP) $< -o $@ -Lbin/ -l$(MODULE) -lboost_system -lboost_thread -lboost_filesystem
obj/%$(OBJ_EXTEND): test/%$(SRC_EXTEND)
	$(CPP) -c $(CPPFLAGS) $< -o $@
endif #LINUX

#source file,obj file and exe file setting
SRC_FILES=$(shell ls src/*$(SRC_EXTEND))
OBJ_FILES=$(SRC_FILES:src/%$(SRC_EXTEND)=obj/%$(OBJ_EXTEND))
#EXE_FILES=$(SRC_FILES:%$(SRC_EXTEND)=%$(EXE_EXTEND))
EXE_FILES=lib$(MODULE)$(DLL_EXTEND)

TEST_SRC_FILES=$(shell ls test/*$(SRC_EXTEND))
TEST_OBJ_FILES=$(TEST_SRC_FILES:test/%$(SRC_EXTEND)=obj/%$(OBJ_EXTEND))
TEST_EXE_FILES=$(TEST_SRC_FILES:test/%$(SRC_EXTEND)=bin/%$(EXE_EXTEND))

#dest setting
all:bin/$(EXE_FILES) test

test:$(TEST_EXE_FILES)

bin/$(EXE_FILES):$(OBJ_FILES)
	$(CPP) $^ -o $@ $(LDFLAGS)

clean:
	@-rm -rf $(OBJ_FILES) bin/$(EXE_FILES)

.PHONY:all clean
