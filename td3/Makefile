CPP = g++
CFLAGS = -I~/rl_tools/include/ -I/opt/OpenBLAS/include/ -std=c++17 -O3 -mavx2 -march=native -ffast-math -flto -DRL_TOOLS_BACKEND_ENABLE_OPENBLAS 
LIBS =  -L/opt/OpenBLAS/lib -lopenblas 
SRC = td3.cpp 
TARGET = td3 

# Targets
all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
