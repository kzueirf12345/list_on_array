.PHONY: all build clean rebuild \
		logger_build logger_clean logger_rebuild \
		clean_all clean_log clean_out clean_obj clean_deps clean_txt clean_bin \
		compile_asm

PROJECT_NAME = fist

BUILD_DIR = ./build
SRC_DIR = ./src
COMPILER ?= gcc

DEBUG_ ?= 1
USE_AVX2 ?= 0

ifeq ($(origin FLAGS), undefined)

FLAGS =	-Wall -Wextra -Waggressive-loop-optimizations \
		-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		-Wconversion -Wempty-body -Wfloat-equal \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self \
		-Wredundant-decls -Wshadow -Wsign-conversion \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand \
		-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs \
		-Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow \
		-flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie \
		-fPIE -Werror=vla \

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
		integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
		shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEBUG_FLAGS = -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER)
RELEASE_FLAGS = -DNDEBUG -O2

ifneq ($(DEBUG_),0)
FLAGS += $(DEBUG_FLAGS)

else
FLAGS += $(RELEASE_FLAGS)
endif

ifneq ($(USE_AVX2),0)
FLAGS += -mavx -mavx2 -march=native -fopenmp
else
FLAGS += -mno-avx -mno-avx2
endif

endif

FLAGS += $(ADD_FLAGS)


DIRS = fist verification dumb verify_utils
BUILD_DIRS = $(DIRS:%=$(BUILD_DIR)/%)

SOURCES = fist/fist.c verification/verification.c dumb/dumb.c verify_utils/verify_utils.c \
		  dumb/dumber.c dumb/utils.c dumb/dumb_text.c dumb/dumb_graphical.c

SOURCES_REL_PATH = $(SOURCES:%=$(SRC_DIR)/%)
OBJECTS_REL_PATH = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS_REL_PATH = $(OBJECTS_REL_PATH:%.o=%.d)

ASM_COMPILER = nasm
SYSTEM = elf64
SOURCES_ASM = fist/fist_find.asm
SOURCES_ASM_REL_PATH = $(SOURCES_ASM:%=$(SRC_DIR)/%)
OBJECTS_ASM_REL_PATH = $(SOURCES_ASM:%.asm=$(BUILD_DIR)/%.o)
LISTINGS_ASM_REL_PATH = $(SOURCES_ASM:%.asm=$(BUILD_DIR)/%.lst)


all: build start

build: lib$(PROJECT_NAME).a

rebuild: clean_all build


lib$(PROJECT_NAME).a: compile_asm $(OBJECTS_REL_PATH)
	ar -rcs lib$(PROJECT_NAME).a $(OBJECTS_REL_PATH) $(OBJECTS_ASM_REL_PATH) libs/logger/liblogger.a 

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c | ./$(BUILD_DIR)/ $(BUILD_DIRS) logger_build
	@$(COMPILER) $(FLAGS) -I$(SRC_DIR) -I./libs -c -MMD -MP $< -o $@

compile_asm: $(SOURCES_ASM_REL_PATH) | ./$(BUILD_DIR)/ $(BUILD_DIRS)
	@$(ASM_COMPILER) -f $(SYSTEM) -l $(LISTINGS_ASM_REL_PATH) -o $(OBJECTS_ASM_REL_PATH) $(SOURCES_ASM_REL_PATH)

-include $(DEPS_REL_PATH)

$(BUILD_DIRS):
	mkdir $@
./$(BUILD_DIR)/:
	mkdir $@


logger_rebuild: logger_build logger_clean

logger_build:
	@make ADD_FLAGS="$(ADD_FLAGS)" FLAGS="$(FLAGS)" DEBUG_=$(DEBUG_) build -C ./libs/logger

logger_clean:
	make ADD_FLAGS="$(ADD_FLAGS)" clean -C ./libs/logger



clean_all: clean_obj clean_deps clean_out logger_clean

clean: clean_obj clean_deps clean_out

clean_log:
	rm -rf ./log/*

clean_out:
	rm -rf ./*.out

clean_obj:
	rm -rf ./$(OBJECTS_REL_PATH) lib$(PROJECT_NAME).a

clean_deps:
	rm -rf ./$(DEPS_REL_PATH)

clean_txt:
	rm -rf ./*.txt

clean_bin:
	rm -rf ./*.bin