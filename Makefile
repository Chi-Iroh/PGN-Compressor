SHELL = /bin/bash
SRC_DIR =   src
OBJ_DIR =   obj
TESTS_DIR =   tests
DEPS_DIR    =   deps

SRC_NO_MAIN	=	$(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
SRC 	=	$(SRC_NO_MAIN) $(SRC_DIR)/main.c
OBJ_NO_MAIN	=	$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC_NO_MAIN:.c=.o))
OBJ =   $(OBJ_NO_MAIN) $(OBJ_DIR)/main.o
DEPS    =   $(patsubst $(SRC_DIR)/%,$(DEPS_DIR)/%,$(SRC:.c=.d))

TESTS_SRC	=	$(wildcard $(TESTS_DIR)/*.c)
TESTS_OBJ	=	$(patsubst $(TESTS_DIR)/%,$(TESTS_DIR)/$(OBJ_DIR)/%,$(TESTS_SRC:.c=.o))
TESTS_DEPS  =   $(patsubst $(TESTS_DIR)/%,$(TESTS_DIR)/$(DEPS_DIR)/%,$(TESTS_SRC:.c=.d))
TESTS_EXE	=	$(TESTS_DIR)/test

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
re: cleanall all

.PHONY: debug redebug
debug: CFLAGS += $(DEBUG)
debug: $(NAME)
redebug: cleanall debug

.PHONY: sanitize resanitize
sanitize: CFLAGS += $(DEBUG) $(SANITIZE)
sanitize: LD_PRELOAD += -lasan -lubsan
sanitize: $(NAME)
resanitize: cleanall sanitize

.PHONY: analyzer
analyzer:
	@$(CC) --analyze -Xanalyzer -analyzer-output=text $(SRC) 2>&1 | tee $(ANALYZER_LOG)

$(TESTS_EXE): $(OBJ_NO_MAIN) $(TESTS_OBJ)
	$(CC) -lcriterion $(OBJ_NO_MAIN) $(TESTS_OBJ) -o $(TESTS_EXE)

tests: $(TESTS_EXE)

testsclean:
	rm -rf $(TESTS_EXE) $(TESTS_OBJ) $(TESTS_DEPS)

retests: testsclean tests

$(NAME): $(OBJ)
	@$(CC) $(OBJ) $(LD_PRELOAD) $(LDFLAGS) -o $(NAME)

-include $(DEPS)
-include $(TESTS_DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$< -> $@"
	@$(CC) -c $(CFLAGS) $< -o $@ -MMD -MF $(DEPS_DIR)/$*.d

$(TESTS_DIR)/$(OBJ_DIR)/%.o: $(TESTS_DIR)/%.c
	@echo "$< -> $@"
	@$(CC) $(CFLAGS) -c $< -o $@ -g3 -O0 -MMD -MF $(TESTS_DIR)/$(DEPS_DIR)/$*.d

.PHONY: clean
clean: testsclean
	rm -f $(OBJ) $(ANALYZER_LOG) $(DEPS)

.PHONY: cleanall
cleanall: clean
	rm -f $(NAME)
