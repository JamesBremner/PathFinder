#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS = 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj


OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_GraphDataTest = \
	$(ODIR)/cGraphData.o \
	$(ODIR)/GraphDataTest.o 

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

GraphDataTest: $(OBJ_GraphDataTest)
	g++ $(OPT) \
	-o bin/GraphDataTest.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

