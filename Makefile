CC=gcc
DIS=objdump
X=xxd
SRC=src/main
INC=src/include
BUILD=build

$(BUILD)/pilot: $(SRC)/*.c $(INC)/*.h $(BUILD)
	$(CC) -o $@ $(SRC)/*.c -I $(INC)
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
