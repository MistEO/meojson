INC := include
SRC := src
# LIBS := -lpthread

SRC_FILES := $(shell ls $(SRC)/*.cpp)
SAMPLE_FILE := sample/sample.cpp

CXX := g++
OPT := -O2
CXXFLAGS := -Wall $(OPT) -std=c++17
CXXRELEASE := $(CXX) $(CXXFLAGS)
CXXDEBUG := $(CXX) -g -D_DEBUG $(CXXFLAGS)

BUILD := build
OBJS := $(patsubst %.cpp, $(BUILD)/%.o, $(notdir $(SRC_FILES) $(SAMPLE_FILE)))
SAMPLE_OBJ := $(patsubst %.cpp,%.o,$(notdir $(SAMPLE_FILE)))
TARGET := $(BUILD)/libmeojson.so
DEMO := $(patsubst %.o,$(BUILD)/%.out,$(SAMPLE_OBJ))

### release
release: $(BUILD) $(TARGET)
	$(CXXRELEASE) $(SAMPLE_FILE) -o $(DEMO) -I$(INC) $(LIBS) -L$(BUILD) -lmeojson

$(TARGET): $(SRC_FILES) $(INC)/*.h
	$(CXXRELEASE) $(SRC_FILES) -o $(TARGET) -I$(INC) $(LIBS) -fPIC -shared

install: $(TARGET)
	cp $(TARGET) /usr/lib

uninstall:
	rm /usr/lib/libjson.so

### debug
debug: $(BUILD) $(OBJS)
	$(CXXDEBUG) -o $(DEMO) $(OBJS) -I$(INC) $(LIBS)

$(BUILD)/$(SAMPLE_OBJ): $(SAMPLE_FILE) $(INC)/*.h
	$(CXXDEBUG) -o $@ -c $< -I$(INC) $(LIBS)

$(BUILD)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CXXDEBUG) -o $@ -c $< -I$(INC) $(LIBS)

### public
$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -f $(TARGET) $(DEMO) $(OBJS)
