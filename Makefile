SRC 	=	$(wildcard src/*.cpp)


OBJ =   $(patsubst src/%.cpp, obj/%.o, $(SRC))

CXX  =   g++
DEBUG   =   -ggdb3 -DDEBUG_MODE

RELEASE = -O2
SANITIZE	=	-fsanitize=address,undefined -fsanitize-recover=address,undefined
ANALYZER	=

CXXFLAGS  +=  -Wall -Wextra -pedantic -fsigned-char -funsigned-bitfields -std=c++23
LDFLAGS	+=
LD_PRELOAD	=

NAME    =   pgn_compressor

.PHONY: all re
all: CXXFLAGS += $(RELEASE)
all: $(NAME)
re: fclean all

.PHONY: debug redebug
debug: CXXFLAGS += $(DEBUG)
debug: $(NAME)
redebug: fclean debug

.PHONY: sanitize resanitize
sanitize: CXXFLAGS += $(DEBUG) $(SANITIZE)
sanitize: LD_PRELOAD += -lasan -lubsan
sanitize: $(NAME)
resanitize: fclean sanitize

.PHONY: analyzer reanalyzer
analyzer: ANALYZER += on
analyzer: CXXFLAGS += $(DEBUG) -fanalyzer
analyzer: $(NAME)
reanalyzer: fclean analyzer

.PHONY: display_info
display_info:
	@$(CXX) --version | head -n 1
	@echo CXXFLAGS : $(CXXFLAGS)
	@echo LDFLAGS : $(LD_PRELOAD) $(LDFLAGS)
	@echo -------------

.PHONY: remove_old_analyzer
remove_old_analyzer:
	@if [[ "$(ANALYZER)" != "" ]]; then		\
		rm -f ./analyzer.log;				\
		echo "Removing old analyzer log.";	\
	fi

$(NAME): display_info remove_old_analyzer $(OBJ)
	@$(CXX) $(OBJ) $(LD_PRELOAD) $(LDFLAGS) -o $(NAME)

obj/%.o: src/%.cpp
	@if [[ "$(ANALYZER)" != "" ]]; then						\
		$(CXX) -c $(CXXFLAGS) $< -o $@ 2>> ./analyzer.log;	\
	else													\
		$(CXX) -c $(CXXFLAGS) $< -o $@;						\
	fi

.PHONY: clean_vgcore
clean_vgcore:
	@echo Removing Core Dumped files.
	@rm -f vgcore.*
	@rm -f valgrind*.log.core.*

.PHONY: clean
clean: clean_vgcore
	@echo Removing temporary and object files.
	@rm -f $(OBJ)

.PHONY: fclean
fclean: clean
	@echo Removing binary.
	@rm -f $(NAME)
