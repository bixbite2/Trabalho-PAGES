CC = gcc
CFLAGS = -Wall -Iinclude -pthread
SRC_DIR = src
OBJ_DIR = obj
BIN = my_app.out

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ -pthread || exit 1

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard include/*.h)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ || exit 1

clean:
	rm -rf $(OBJ_DIR) $(BIN)
