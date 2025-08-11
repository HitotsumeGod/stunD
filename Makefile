CC=gcc
DBG=valgrind
DIS=objdump
X=xxd
SRC=src/main
INC=src/include
BUILD=build

test: $(BUILD)/pilot
prod: $(BUILD)/stunD
$(BUILD)/pilot: $(SRC)/*.c $(INC)/*.h $(BUILD)
	$(CC) -g -o $@ $(SRC)/*.c -I $(INC)
$(BUILD)/stunD: $(SRC)/*.c $(INC)/*.h $(BUILD)
	$(CC) -Wall -Werror -o $@ $(SRC)/*.c -I $(INC)
debug: $(BUILD)/pilot
	$(DBG) --leak-check=full --show-leak-kinds=all --track-origins=yes -s $^ 4
dump: $(BUILD)/pilot
	$(DIS) -d $<
hdump: $(BUILD)/pilot
	$(X) $<
bdump: $(BUILD)/pilot
	$(X) -b $<
clean:
	rm -rf $(BUILD)
	rm -rf vgcore*
$(BUILD):
	if [ ! -d $@ ]; then	\
		mkdir $@;	\
	fi
