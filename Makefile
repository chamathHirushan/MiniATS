CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -Iexternal/asio/include
LDFLAGS  := -lws2_32

BUILD_DIR := build
SRC_DIR   := src

# Source files
SERVER_SRCS := $(SRC_DIR)/server_app.cpp \
               $(SRC_DIR)/ServerMain.cpp \
               $(SRC_DIR)/OrderBook.cpp \
               $(SRC_DIR)/OrderBookEntry.cpp \
               $(SRC_DIR)/Wallet.cpp \
               $(SRC_DIR)/CSVReader.cpp

CLIENT_SRCS := $(SRC_DIR)/client_app.cpp \
               $(SRC_DIR)/ClientMain.cpp

# Object files
SERVER_OBJS := $(SERVER_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CLIENT_OBJS := $(CLIENT_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executables
SERVER_EXE := $(BUILD_DIR)/server.exe
CLIENT_EXE := $(BUILD_DIR)/client.exe

.PHONY: all clean directories

all: directories $(SERVER_EXE) $(CLIENT_EXE)

directories:
	@mkdir -p $(BUILD_DIR)

$(SERVER_EXE): $(SERVER_OBJS)
	@echo "Linking Server..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXE): $(CLIENT_OBJS)
	@echo "Linking Client..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
