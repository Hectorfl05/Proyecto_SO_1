# ─────────────────────────────────────────────────────────────
#  Makefile - Productor Consumidor
#  Universidad Rafael Landívar - Sistemas Operativos
# ─────────────────────────────────────────────────────────────

CXX      = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra

TARGET   = productor_consumidor
SOURCES  = main.cpp src/Buffer.cpp src/Productor.cpp src/Consumidor.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
