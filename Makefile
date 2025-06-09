SRC_NO_MAIN	=	$(shell find src/ -type f -name "*.c" -not -name "main.c")
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
ANALYZER	=
C_VERSION	=	-std=c99

CFLAGS  +=  -Wall -Wextra -pedantic -fsigned-char $(C_VERSION)
LDFLAGS	+=
LD_PRELOAD	=

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

.PHONY: analyzer reanalyzer
analyzer: ANALYZER += on
analyzer: CFLAGS += $(DEBUG) -fanalyzer
analyzer: $(NAME)
reanalyzer: fclean analyzer

$(TESTS_EXE): $(OBJ_NO_MAIN) $(TESTS_OBJ)
	$(CC) -lcriterion $(OBJ_NO_MAIN) $(TESTS_OBJ) -o $(TESTS_EXE)

tests: $(TESTS_EXE)

testsclean:
	rm -rf $(TESTS_EXE) $(TESTS_OBJ) config_files

retests: testsclean tests

.PHONY: check_version
check_version:
	@echo -------------
	@echo -n "CC : "
	@if [[ "$(CC_VERSION)" == "" ]]; then	\
		echo 'Bad CC version !';			\
		echo $(CC_VERSION_ERROR);			\
		exit 1;								\
	fi

.PHONY: display_info
display_info:
	@$(CC) --version | head -n 1
	@echo CFLAGS : $(CFLAGS)
	@echo LDFLAGS : $(LD_PRELOAD) $(LDFLAGS)
	@echo -------------

.PHONY: remove_old_analyzer
remove_old_analyzer:
	@if [[ "$(ANALYZER)" != "" ]]; then		\
		rm -f ./analyzer.log;				\
		echo "Removing old analyzer log.";	\
	fi

$(NAME): display_info remove_old_analyzer $(OBJ)
	@$(CC) $(OBJ) $(LD_PRELOAD) $(LDFLAGS) -o $(NAME)

obj/%.o: src/%.c
	@echo "$< -> $@"
	@if [[ "$(ANALYZER)" != "" ]]; then					\
		$(CC) -c $(CFLAGS) $< -o $@ 2>&1 | tee -a ./analyzer.log;	\
	else												\
		$(CC) -c $(CFLAGS) $< -o $@;					\
	fi

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
	rm -f $(OBJ)

.PHONY: fclean
fclean: clean
	@echo Removing binary.
	rm -f $(NAME)
