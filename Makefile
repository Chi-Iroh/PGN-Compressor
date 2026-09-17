SHELL = /bin/bash
SRC_NO_MAIN	=	$(filter-out src/main.c, $(wildcard src/*.c))
SRC 	=	$(SRC_NO_MAIN) src/main.c
OBJ_NO_MAIN	=	$(patsubst src/%,obj/%,$(SRC_NO_MAIN:.c=.o))
OBJ =   $(OBJ_NO_MAIN) obj/main.o

TESTS_DIR	=	tests
TESTS_SRC	=	$(wildcard $(TESTS_DIR)/*.c)
TESTS_OBJ	=	$(patsubst tests/%,tests/obj/%,$(TESTS_SRC:.c=.o))
TESTS_EXE	=	tests/test

CC  =   clang
DEBUG   =   -ggdb3 -DDEBUG_MODE

RELEASE = -O2
SANITIZE	=	-fsanitize=address,undefined -fsanitize-recover=address,undefined
C_VERSION	=	-std=c99

CFLAGS  +=  -Wall -Wextra -pedantic -fsigned-char $(C_VERSION)
LDFLAGS	+=
LD_PRELOAD	=

ANALYZER_LOG    =   analyzer.log

NAME    =   pgn_compressor

.PHONY: all re
all: CFLAGS += $(RELEASE)
all: $(NAME)
re: fclean all

.PHONY: debug redebug
debug: CFLAGS += $(DEBUG)
debug: $(NAME)
redebug: fclean debug

.PHONY: sanitize resanitize
sanitize: CFLAGS += $(DEBUG) $(SANITIZE)
sanitize: LD_PRELOAD += -lasan -lubsan
sanitize: $(NAME)
resanitize: fclean sanitize

.PHONY: analyzer
analyzer:
	@$(CC) --analyze -Xanalyzer -analyzer-output=text $(SRC) 2>&1 | tee $(ANALYZER_LOG)

$(TESTS_EXE): $(OBJ_NO_MAIN) $(TESTS_OBJ)
	$(CC) -lcriterion $(OBJ_NO_MAIN) $(TESTS_OBJ) -o $(TESTS_EXE)

tests: $(TESTS_EXE)

testsclean:
	rm -rf $(TESTS_EXE) $(TESTS_OBJ) config_files

retests: testsclean tests

.PHONY: display_info
display_info:
	@$(CC) --version | head -n 1
	@echo CFLAGS : $(CFLAGS)
	@echo LDFLAGS : $(LD_PRELOAD) $(LDFLAGS)
	@echo -------------


$(NAME): display_info $(OBJ)
	@$(CC) $(OBJ) $(LD_PRELOAD) $(LDFLAGS) -o $(NAME)

obj/%.o: src/%.c
	@echo "$< -> $@"
	@$(CC) -c $(CFLAGS) $< -o $@

tests/obj/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -g3 -O0

.PHONY: clean_vgcore
clean_vgcore:
	@echo Removing Core Dumped files.
	@rm -f vgcore.*
	@rm -f valgrind*.log.core.*

.PHONY: clean
clean: clean_vgcore
	@echo Removing temporary and object files.
	rm -f $(OBJ) $(ANALYZER_LOG)

.PHONY: fclean
fclean: clean
	@echo Removing binary.
	rm -f $(NAME)
