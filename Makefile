CC=gcc
DBG=valgrind
DIS=objdump
X=xxd
SRC=src/main
INC=src/include
BUILD=build

$(BUILD)/pilot: $(SRC)/*.c $(INC)/*.h $(BUILD)
	$(CC) -o $@ $(SRC)/*.c -I $(INC)
$(BUILD)/pilotg: $(SRC)/*.c $(INC)/*.h $(BUILD)
	$(CC) -g -o $@ $(SRC)/*.c -I $(INC)
debug: $(BUILD)/pilotg
	$(DBG) --leak-check=full --show-leak-kinds=all --track-origins=yes -s $^ -all
dump: $(BUILD)/pilot
	$(DIS) -d $<
hdump: $(BUILD)/pilot
	$(X) $<
bdump: $(BUILD)/pilot
	$(X) -b $<
clean:
	rm -rf $(BUILD)
$(BUILD):
	if [ ! -d $@ ]; then	\
		mkdir $@;	\
	fi
