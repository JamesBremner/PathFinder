#source file search paths
VPATH = ./src ../raven-set

# compiler include search paths
INCS = -I../raven-set

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_test = \
	$(ODIR)/cGraphData.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/test.o \
	$(ODIR)/cutest.o

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ $<  \
	-c -o $@ $(INCS) $(CCflags)

test: $(OBJ_test)
	g++ -o bin/test.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

rebuild: clean test

