CPP = g++
CFLAGS = -I/home/dax/cppProj/rl_tools -I/opt/OpenBLAS/include/ -std=c++17 -O3 -mavx2 -march=native -ffast-math 
LIBS =  -L/opt/OpenBLAS/lib -lopenblas
SRC = EthernetCommunication.cpp
TARGET = RLserver

# Targets
all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
