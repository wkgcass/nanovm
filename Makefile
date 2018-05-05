NAME := nanovm
VERSION := 0.0.1-DEV

GMSL_DIR := gmsl/
include $(GMSL_DIR)/gmsl

TMP := tmp/
OS := $(shell uname -s)

CFLAGS += -g -Wall -Wextra -Werror -Wno-unused-parameter
LDLIBS += -lpthread

ifeq ($(OS),Linux)
LDLIBS += -lrt
endif

marker = $1.f

TEST_NAME := $(NAME)test

OBJ := o/

LIB_OBJS := $(addprefix $(OBJ),code.o memory.o nanovm.o native.o stack.o thread.o util.o)
MAIN_OBJS := $(LIB_OBJS) $(addprefix $(OBJ),main.o)
TEST_OBJS := $(LIB_OBJS) $(addprefix $(OBJ),test.o)
OBJS := $(MAIN_OBJS) $(TEST_OBJS)
EXECS := $(addprefix $(OBJ),$(NAME) $(TEST_NAME))

.PHONY: all clean
all: $(OBJ) $(EXECS)
clean: ; @rm -rf $(OBJ)

$(OBJ):
	@mkdir -p $@

$(OBJ)$(NAME): $(MAIN_OBJS) ; @$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJ)$(TEST_NAME): $(TEST_OBJS) ; @$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJ)%.o: src/%.c ; @$(COMPILE.c) $(OUTPUT_OPTION) $<
