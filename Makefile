INTERFACE_SRC=src/interface.c src/interface.h
LIB_SRC=$(shell find src -name "*.bjou")
DRIVER_SRC=src/yodaw_driver.c
CLEAN_O=$(shell find . -name "*.o")
CLEAN_SO=$(shell find . -name "*.so")
CLEAN_DSYM=$(shell find . -name "*.dSYM")

all: _yodaw
	@echo "Done."

_yodaw: libnk_yodaw.so libyodaw.so $(DRIVER_SRC)
	@echo "Compiling the driver.."
	@$(CC) src/yodaw_driver.c $(DRIVER_C_FLAGS) $(cfg) -o _yodaw

libyodaw.so: libnk_yodaw.so src/yodaw.o src/interface.o $(LIB_SRC)
	@echo "Linking libyodaw.so.."
	@$(CC) src/interface.o src/yodaw.o $(LIB_LINK_FLAGS) -o libyodaw.so

libnk_yodaw.so: src/nuklear.c src/nuklear.h src/nuklear_sdl_gl2.h
	@echo "Compiling libnk_yodaw.so.."
	@$(CC) src/nuklear.c $(NK_C_FLAGS) $(cfg) -o libnk_yodaw.so

src/yodaw.o: $(LIB_SRC)
	@echo "Compiling yodaw.o.."
	@bjou -c $(LIB_BJOU_FLAGS) $(bjou_cfg) src/yodaw.bjou -o src/yodaw.o | sed 's/\x1b\[[0-9;]*[a-zA-Z]//g'

src/interface.o: $(INTERFACE_SRC)
	@echo "Compiling interface.o.."
	@$(CC) -c src/interface.c $(INTERFACE_C_FLAGS) $(cfg) -o src/interface.o

clean:
	rm -rf $(CLEAN_O) $(CLEAN_SO) $(CLEAN_DSYM) _yodaw
