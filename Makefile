INC := include
SRC := src
# LIBS := -lpthread

SRC_FILES := $(shell ls $(SRC)/*.cpp)
SAMPLE_FILE := sample/sample.cpp

CXX := g++
OPT := -O2
CXXFLAGS :=

BUILD := build
OBJS := $(patsubst %.cpp, $(BUILD)/%.o, $(notdir $(SRC_FILES) $(SAMPLE_FILE)))
SAMPLE_OBJ := $(patsubst %.cpp,%.o,$(notdir $(SAMPLE_FILE)))
TARGET := $(BUILD)/libmeojson.so
DEMO := $(patsubst %.o,$(BUILD)/%.out,$(SAMPLE_OBJ))

### release
release: $(BUILD) $(TARGET)
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) $(SAMPLE_FILE) -o $(DEMO) -I$(INC) $(LIBS) -L$(BUILD) -lmeojson

$(TARGET): $(SRC_FILES) $(INC)/*.h
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) $(SRC_FILES) -o $(TARGET) -I$(INC) $(LIBS) -fPIC -shared

install: $(TARGET)
	cp $(TARGET) /usr/lib

uninstall:
	rm /usr/lib/libjson.so

### debug
debug: $(BUILD) $(OBJS)
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $(DEMO) $(OBJS) -I$(INC) $(LIBS)

$(BUILD)/$(SAMPLE_OBJ): $(SAMPLE_FILE) $(INC)/*.h
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $@ -c $< -I$(INC) $(LIBS)

$(BUILD)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $@ -c $< -I$(INC) $(LIBS)

### public
$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -f $(TARGET) $(DEMO) $(OBJS)
