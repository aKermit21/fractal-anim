
APPNAME := frexe
# build (and dependency files) subdirectories
OBJDIR := build
# SOURCEDIR := src
SOURCES := $(wildcard src/*.cpp)
# Object in build subdirectory
OBJECTS := $(patsubst src/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
# Compiler
CXX := g++

# Standard compilator warnings and speed optimization
CXXFLAGS := -std=c++17 -Wall -O2
# Full C++ standards checks and Warnings as Errors:
# CFLAGS := -std=c++17 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror
# Full C++ standards checks:
# CFLAGS := -std=c++17 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion

# Include directory needed to integrate lyra/*.hpp header only library
INC := -Isubprojects/Lyra/include

CXXFLAGS += $(INC)

# This optimization caused algo malfunction and was Off - problem solved
# OPT := -fno-toplevel-reorder 
  
# SFML (graphic) libraries - Dynamic?
LIBS := -lsfml-graphics -lsfml-window -lsfml-system

.PHONY: all clean release debug depend

# default development compilation
all: $(APPNAME)

# Additional flag for Release mode (switch off Asserts)
release: CXXFLAGS += -DNDEBUG
release: $(APPNAME)

# Debugging flag and no optimization
debug: CXXFLAGS := -std=c++17 -Wall -O0 -g 
debug: CXXFLAGS += $(INC)
debug: $(APPNAME)

# Creating depency file for all sources
depend:
	@echo Create raw dependency file for all sources
	$(CXX) $(INC) -MM $(SOURCES) > $(OBJDIR)/make_raw.d
	@echo Adjust dependency file
	@# 1. Add $(OBJDIR)/ before *.o ; 2. Remove lyra header library (include/lyra/*) ; \
	# 3 Remove effectively empty lines
	sed -r -e 's|[a-zA-Z0-9_]+\.o:|$(OBJDIR)/&|' \
	-e  's| include/lyra/[^ ]+||g' -e  '/^ +\\$$/d' < $(OBJDIR)/make_raw.d > $(OBJDIR)/make_tmp.d
	mv $(OBJDIR)/make_tmp.d make.d

clean:
	-rm $(OBJECTS)

$(APPNAME) : $(OBJECTS)
	@echo Linking now...
	$(CXX) -o $@ $^ $(LIBS)

# Compilation according to dependency rules file
-include make.d
$(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR): ; @mkdir -p $@

