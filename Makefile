# Compiler
CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -Iexternal/asio/include

# Build directories
BUILD_DIR := build
SRC_DIR   := src

# Detect OS for linking flags and executable extension
ifeq ($(OS),Windows_NT)
    LDFLAGS := -lws2_32
    EXE_EXT := .exe
else
    LDFLAGS :=
    EXE_EXT :=
endif

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
SERVER_EXE := $(BUILD_DIR)/server$(EXE_EXT)
CLIENT_EXE := $(BUILD_DIR)/client$(EXE_EXT)

.PHONY: all clean directories

# Default target
all: directories $(SERVER_EXE) $(CLIENT_EXE)

# Create build directory
directories:
	@mkdir -p $(BUILD_DIR)

# Linking server
$(SERVER_EXE): $(SERVER_OBJS)
	@echo "Linking Server..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Linking client
$(CLIENT_EXE): $(CLIENT_OBJS)
	@echo "Linking Client..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .cpp to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
