SRCDIR		= .
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
CD_OBJECTS	   = 
OBJECTS        = $(OBJ_C)
ELF_TARGET     = wor.elf
BIN_TARGET     = wor.bin
ADD_FILES      = 
CDOUT          = wor_cd

include ../example.mk

CFLAGS		+= -I. -Wall
