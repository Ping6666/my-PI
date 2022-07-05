OUT_DIR = ./out
SRC_DIR = ./src

DEPENDENCY_O = 	$(OUT_DIR)/FFT.o $(OUT_DIR)/BigFloat.o \
				$(OUT_DIR)/Printer.o \
				$(OUT_DIR)/PI_Chudnovsky.o $(OUT_DIR)/BBP_Formula.o

all: $(DEPENDENCY_O)
	g++ -Wall -g -o3 $(DEPENDENCY_O) Main.cpp -o my_PI

$(DEPENDENCY_O): out_dir

$(OUT_DIR)/FFT.o: $(SRC_DIR)/BBPCore.h $(SRC_DIR)/FFT.cpp $(SRC_DIR)/FFT.h
	g++ -Wall -g -o3 -c $(SRC_DIR)/FFT.cpp -o $(OUT_DIR)/FFT.o

$(OUT_DIR)/BigFloat.o: $(SRC_DIR)/BBPCore.h $(SRC_DIR)/BigFloat.cpp $(SRC_DIR)/BigFloat.h
	g++ -Wall -g -o3 -c $(SRC_DIR)/BigFloat.cpp -o $(OUT_DIR)/BigFloat.o

$(OUT_DIR)/Printer.o: $(SRC_DIR)/BBPCore.h $(SRC_DIR)/Printer.cpp $(SRC_DIR)/Printer.h
	g++ -Wall -g -o3 -c $(SRC_DIR)/Printer.cpp -o $(OUT_DIR)/Printer.o

$(OUT_DIR)/PI_Chudnovsky.o: $(SRC_DIR)/BBPCore.h $(SRC_DIR)/PI_Chudnovsky.cpp $(SRC_DIR)/PI_Chudnovsky.h
	g++ -Wall -g -o3 -c $(SRC_DIR)/PI_Chudnovsky.cpp -o $(OUT_DIR)/PI_Chudnovsky.o

$(OUT_DIR)/BBP_Formula.o: $(SRC_DIR)/BBPCore.h $(SRC_DIR)/BBP_Formula.cpp $(SRC_DIR)/BBP_Formula.h
	g++ -Wall -g -o3 -c $(SRC_DIR)/BBP_Formula.cpp -o $(OUT_DIR)/BBP_Formula.o

out_dir:
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)
	rm -rf my_PI
