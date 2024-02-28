CPP = g++
CFLAGS = -I/home/dax/cppProj/rl_tools -I/opt/OpenBLAS/include/ -L/opt/OpenBLAS/lib -lopenblas -DRL_TOOLS_BACKEND_ENABLE_OPENBLAS -std=c++17 -O3 -mavx2 -march=native -ffast-math 

SRC = minimalFailure.cpp
TARGET = minFail

# Targets
all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)