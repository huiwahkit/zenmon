# Copyright (C) 2020 Denis Isai

# App name
TARGET = zenmon

LD = gcc

# App directories
INC_DIR := .
SRC_DIR := .
OUT_DIR := ./out

# App artifacts
INC := $(shell find $(INC_DIR) -type d | sed -e 's/\.\//-I.\//g' | sed '1d' | grep -v ".git\|src" | tr '\n' ' ')
LIB :=
SRC := $(shell find $(SRC_DIR) \( -name "*.c" \))
OBJ := $(SRC:.c=.o)
OUT := $(OUT_DIR)/$(TARGET)
FLG :=

# make
$(TARGET):
	mkdir -p out
	$(LD) $(SRC) $(INC) $(LIB) $(FLG) -o $(OUT_DIR)/$@ $^

# clean
clean:
	rm -f $(OUT)
