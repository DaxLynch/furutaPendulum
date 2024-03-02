CPP = g++
CFLAGS = -I/home/dax/rl_tools -I/opt/OpenBLAS/include/ -std=c++17 -O3 -mavx2 -march=native -ffast-math -DRL_TOOLS_BACKEND_ENABLE_OPENBLAS -flto 
LIBS =  -L/opt/OpenBLAS/lib -lopenblas
SRC = EthernetCommunication.cpp
TARGET = RLServer

# Targets
all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
