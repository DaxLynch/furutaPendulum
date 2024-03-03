CPP = g++
CFLAGS = -I~/rl_tools/include/ -I/opt/OpenBLAS/include/ -std=c++17 -O3 -mavx2 -march=native -ffast-math -flto 
LIBS =  -L/opt/OpenBLAS/lib -lopenblas 
SRC = vpg.cpp 
TARGET = Vpg 

# Targets
all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
