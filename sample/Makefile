CXX = g++ -Wall -g -std=gnu++14
LIBS = -lpthread

INC = ../include
SRC = ../src
BUILD = ../build
TARGET = demo
SRC_FILES = $(shell ls $(SRC)/*.cpp sample.cpp)
OBJS = $(patsubst %.cpp, $(BUILD)/%.o, $(notdir $(SRC_FILES)))

TARGET: $(BUILD) $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) -I$(INC) $(LIBS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/sample.o: sample.cpp $(INC)/*.h
	$(CXX) -o $@ -c $< -I$(INC)

$(BUILD)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CXX) -o $@ -c $< -I$(INC)

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
