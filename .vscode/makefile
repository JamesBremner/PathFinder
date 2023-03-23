#source file search paths
VPATH = ./src ../raven-set ../autocell/src

# compiler include search paths
INCS = -I../raven-set \
	-I../windex/include \
	-I../autocell/src

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_test = \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/tourNodes.o \
	$(ODIR)/test.o \
	$(ODIR)/cutest.o

OBJ_pathfinder = \
	$(ODIR)/pathfinder.o \
	$(ODIR)/cPathFinderGUI.o \
	$(ODIR)/cVE.o \
	$(ODIR)/cAttribute.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/tourNodes.o \
	$(ODIR)/GraphViz.o \
	$(ODIR)/readfile.o

OBJ_obstacle = \
	$(ODIR)/obstacle.o \
	$(ODIR)/cObstacleGUI.o \
	$(ODIR)/cObstacle.o \
	$(ODIR)/cVE.o \
	$(ODIR)/cAttribute.o \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/tourNodes.o \
	$(ODIR)/autocell.o \
	$(ODIR)/cxy.o \

OBJ_graphex = \
	$(ODIR)/graphex.o \
	$(ODIR)/cVE.o \
	$(ODIR)/cAttribute.o \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/cRunWatch.o

DEPS = \
	cGraph.h GraphTheory.h

### COMPILE

$(ODIR)/graphex.o : graphex.cpp $(DEPS)
	g++ $<  \
	-c -o $@ $(INCS) $(CCflags) \
	-I../../code_ext/boost/boost1_78

$(ODIR)/cRunWatch.o : cRunWatch.cpp
	g++ $<  \
	-c -o $@ $(INCS) $(CCflags) \
	-I../../code_ext/boost/boost1_78

$(ODIR)/%.o: %.cpp $(DEPS)
	g++ $<  \
	-c -o $@ $(INCS) $(CCflags)

### LINK

test: $(OBJ_test)
	g++ -o bin/test.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

pathfinder: $(OBJ_pathfinder)
	g++ -o bin/pathfinder.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

obstacle: $(OBJ_obstacle)
	g++ -o bin/obstacle.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

graphex: $(OBJ_graphex)
	g++ -o bin/graphex.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)




.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

rebuild: clean test pathfinder obstacle graphex

