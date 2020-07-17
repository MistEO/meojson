INC = include
SRC = src
# LIBS = -lpthread
BUILD = build

SRC_FILES = $(shell ls $(SRC)/*.cpp)
SAMPLE_FILE = sample/sample.cpp

CXXRELEASE = g++ -Wall -O2 -std=c++11
CXXDEBUG = g++ -Wall -g -std=c++11 -DDEBUG

TARGET = $(BUILD)/libjson.so
DEMO = $(BUILD)/demo.out
OBJS = $(patsubst %.cpp, $(BUILD)/%.o, $(notdir $(SRC_FILES) $(SAMPLE_FILE)))
TEST = $(BUILD)/test.out

### release
release: $(BUILD) clean $(TARGET)

$(TARGET): $(SRC_FILES) $(INC)/*.h
	$(CXXRELEASE) $(SRC_FILES) -o $(TARGET) -I$(INC) $(LIBS) -fPIC -shared

# $(DEMO): $(SAMPLE_FILE) $(INC)/*.h
# 	$(CXXRELEASE) $(SAMPLE_FILE) -o $(DEMO) -I$(INC) -L$(BUILD) -ljson $(LIBS)

### debug
debug: $(BUILD) $(OBJS)
	$(CXXDEBUG) -o $(TEST) $(OBJS) -I$(INC) $(LIBS)

$(BUILD)/sample.o: $(SAMPLE_FILE) $(INC)/*.h
	$(CXXDEBUG) -o $@ -c $< -I$(INC)

$(BUILD)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CXXDEBUG) -o $@ -c $< -I$(INC)

### public
$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -f $(TARGET) $(DEMO) $(OBJS) $(TEST)
