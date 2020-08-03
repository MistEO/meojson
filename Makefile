INC := include
SRC := src
# LIBS := -lpthread

SRC_FILES := $(shell ls $(SRC)/*.cpp)
SAMPLE_FILE := sample/sample.cpp

CCSTD := c++11
CCRELEASE := g++ -Wall -O2 -std=$(CCSTD)
CCDEBUG := g++ -Wall -g -std=$(CCSTD) -DDEBUG

BUILD := build
TARGET := $(BUILD)/libjson.so
DEMO := $(BUILD)/demo.out
OBJS := $(patsubst %.cpp, $(BUILD)/%.o, $(notdir $(SRC_FILES) $(SAMPLE_FILE)))
TEST := $(BUILD)/test.out

### release
release: clean $(BUILD) $(TARGET) $(DEMO)

$(TARGET): $(SRC_FILES) $(INC)/*.h
	$(CCRELEASE) $(SRC_FILES) -o $(TARGET) -I$(INC) $(LIBS) -fPIC -shared

$(DEMO): $(SAMPLE_FILE) $(INC)/*.h
	$(CCRELEASE) $(SAMPLE_FILE) -o $(DEMO) -I$(INC) $(LIBS) -L$(BUILD) -ljson

### debug
debug: $(BUILD) $(OBJS)
	$(CCDEBUG) -o $(TEST) $(OBJS) -I$(INC) $(LIBS)

$(BUILD)/sample.o: $(SAMPLE_FILE) $(INC)/*.h
	$(CCDEBUG) -o $@ -c $< -I$(INC)

$(BUILD)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CCDEBUG) -o $@ -c $< -I$(INC)

### public
$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -f $(TARGET) $(DEMO) $(OBJS) $(TEST)
