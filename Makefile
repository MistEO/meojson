INC := include
SRC := src
# LIBS := -lpthread

CXX := g++
OPT := -O2
CXXFLAGS :=
AR := ar
ARFLAGS := -rcs

SRC_FILES := $(shell ls $(SRC)/*.cpp)
SAMPLE_FILE := sample/sample.cpp

BUILD_DIR := $(shell mkdir -p build)build
OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SRC_FILES)))
SHARED_LIB := $(BUILD_DIR)/libmeojson.so
STATIC_LIB := $(BUILD_DIR)/libmeojson.a
SAMPLE_OBJ := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SAMPLE_FILE)))
SAMPLE_OUT := $(patsubst %.o, $(BUILD_DIR)/%.out, $(notdir $(SAMPLE_OBJ)))

static: $(OBJS)
	$(AR) $(ARFLAGS) $(STATIC_LIB) $(OBJS)
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $(SAMPLE_OUT) $(SAMPLE_FILE) $(STATIC_LIB) -I$(INC) $(LIBS)

shared:
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $(SHARED_LIB) $(SRC_FILES) -I$(INC) $(LIBS) -fPIC -shared
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $(SAMPLE_OUT) $(SAMPLE_FILE) -I$(INC) $(LIBS) -L$(BUILD_DIR) -lmeojson

debug: $(OBJS) $(SAMPLE_OBJ)
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $(SAMPLE_OUT) $(OBJS) $(SAMPLE_OBJ) -I$(INC) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC)/%.cpp $(INC)/*.h
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $@ -c $< -I$(INC) $(LIBS)

$(SAMPLE_OBJ): $(SAMPLE_FILE) $(INC)/*.h
	$(CXX) -Wall $(OPT) -std=c++17 $(CXXFLAGS) -o $@ -c $< -I$(INC) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(SHARED_LIB) $(STATIC_LIB) $(SAMPLE_OBJ) $(SAMPLE_OUT) 
