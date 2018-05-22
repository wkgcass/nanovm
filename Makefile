NAME := nanovm
VERSION := 0.0.1-DEV

GMSL_DIR := gmsl/
include $(GMSL_DIR)/gmsl

TMP := tmp/
OS := $(shell uname -s)

STD = -std=c11
CFLAGS += -g -Wall -Wextra -Werror -Wno-unused-parameter $(STD)
LDLIBS += -lpthread

ifeq ($(OS),Linux)
LDLIBS += -lrt
endif

marker = $1.f

JAVAC := @javac

TEST_NAME := $(NAME)test

OBJ := o/

JAVA := test/

TEST_JAVA := $(JAVA)*.java

TEST_CLASS := $(JAVA)*.class

EXTERN_LIB_OBJS := $(addprefix $(OBJ),zmalloc.o)
LIB_OBJS := $(addprefix $(OBJ),class.o code.o memory.o nanovm.o native.o stack.o thread.o util.o) $(EXTERN_LIB_OBJS)
MAIN_OBJS := $(LIB_OBJS) $(addprefix $(OBJ),main.o)
TEST_OBJS := $(LIB_OBJS) $(addprefix $(OBJ),test.o)
OBJS := $(MAIN_OBJS) $(TEST_OBJS)
EXECS := $(addprefix $(OBJ),$(NAME) $(TEST_NAME))

.PHONY: all clean
all: $(OBJ) $(EXECS) ; $(JAVAC) $(TEST_JAVA)
clean: ; @rm -rf $(OBJ) $(TEST_CLASS)
$(OBJ):
	@mkdir -p $@

$(OBJ)$(NAME): $(MAIN_OBJS) ; @$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJ)$(TEST_NAME): $(TEST_OBJS) ; @$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJ)%.o: src/%.c ; @$(COMPILE.c) $(OUTPUT_OPTION) $<
