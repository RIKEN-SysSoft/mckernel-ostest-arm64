# Makefile COPYRIGHT FUJITSU LIMITED 2016-2017

# target name.
TARGET := cpu_pa_info

#---------------
# path settings.
#---------------
# directory.
BIN_DIR      := bin
SRC_ROOT_DIR := src
OBJ_ROOT_DIR := obj
INC_DIRS     := . $(SRC_ROOT_DIR)

SRC_DIRS := $(shell find $(SRC_ROOT_DIR) -type d)
OBJ_DIRS := $(patsubst $(SRC_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(SRC_DIRS))

# file lists.
BIN     := $(TARGET)
PROGRAM := $(BIN)
SRCS    := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
ASMS    := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.S))
OBJS    := $(patsubst $(SRC_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(SRCS:.c=.o))
ASMOBJS := $(patsubst $(SRC_ROOT_DIR)%, $(OBJ_ROOT_DIR)%, $(ASMS:.S=.o))
TEST_CASE_LIST := test_case.list
TEST_CASE_LIST_NEW := __test_case.list
CLEAN_FILES := $(TEST_CASE_LIST) $(TEST_CASE_LIST_NEW)

#--------------
# build otions.
#--------------
# compile.
CFLAGS += -g
CFLAGS += -O0
CFLAGS += -Wall
ASFLAGS += -mcpu=all
LDFLAGS += -lihk

# preprocess
override CPPFLAGS += $(foreach dir,$(INC_DIRS),-I$(dir))

#--------
# targets
#--------
RM := rm -rf
MV := mv
SED := sed
MKDIR := mkdir -p
DIFF := diff
FIND := find
GREP := grep
SORT := sort

__update_test_case_list := \
	$(shell $(FIND) $(SRC_ROOT_DIR) | $(GREP) [0-9]${3}.c | $(SORT) | \
		$(SED) "s|$(SRC_ROOT_DIR)/\([0-9]*\)\.c|TEST_CASE_DEF(\1)|g">> $(TEST_CASE_LIST_NEW))\
	$(shell \
	 if $(DIFF) $(TEST_CASE_LIST_NEW) $(TEST_CASE_LIST) >/dev/null 2>&1; then\
	   $(RM) $(TEST_CASE_LIST_NEW);\
	 else\
	   $(MV) $(TEST_CASE_LIST_NEW) $(TEST_CASE_LIST);\
	 fi)

# Default target.
all: $(TARGET)

# Generate Program.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -L$(LIBIHK_DIR) -I$(LIBIHK_DIR) \
		-I$(IHKUSR_INCLUDE_DIR) -o $(PROGRAM) -Wl,-rpath=$(LIBDIR)

clean:
	$(RM) $(CLEAN_FILES) $(OBJ_ROOT_DIR) $(PROGRAM)

$(OBJS):
	-@ $(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -L$(LIBIHK_DIR) -I$(LIBIHK_DIR) -I$(IHKUSR_INCLUDE_DIR) \
		-o $@ -c $(patsubst $(OBJ_ROOT_DIR)%, $(SRC_ROOT_DIR)%, $(@:.o=.c)) \
		-DTEST_NUMBER=$(shell echo $@ | $(SED) "s|$(OBJ_ROOT_DIR)/\([0-9]*\)\.o|\1|g")
