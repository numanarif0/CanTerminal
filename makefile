CXX = g++

SOURCES = main.cpp MainWindow.cpp pcan.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = arayuz_app.exe

PCAN_INCLUDE_PATH = -I"C:/Users/Numan/Downloads/PCAN-Basic/Include"
PCAN_LIB_PATH = -L"C:/Users/Numan/Downloads/PCAN-Basic/x64" 
PCAN_LIB = -lPCANBasic
CXXFLAGS = -std=c++17 -g -Wall `pkg-config --cflags gtkmm-4.0` $(PCAN_INCLUDE_PATH)
LDFLAGS = `pkg-config --libs gtkmm-4.0` $(PCAN_LIB_PATH) $(PCAN_LIB)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean