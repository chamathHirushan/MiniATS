# Compiler
CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -Iexternal/asio/include

#directories
BUILD_DIR := build
SRC_DIR   := src

# # Detect OS for linking flags and executable extension
# ifeq ($(OS),Windows_NT)
#     LDFLAGS := -lws2_32
#     EXE_EXT := .exe
#     MKDIR_CMD := if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
#     RMDIR_CMD := rmdir /s /q $(BUILD_DIR)
# else
#     LDFLAGS :=
#     EXE_EXT :=
#     MKDIR_CMD := mkdir -p $(BUILD_DIR)
#     RMDIR_CMD := rm -rf $(BUILD_DIR)
# endif

# Source files
SERVER_SRCS := $(SRC_DIR)/server/server_app.cpp \
               $(SRC_DIR)/server/ServerMain.cpp \
               $(SRC_DIR)/server/OrderBook.cpp \
               $(SRC_DIR)/server/OrderBookEntry.cpp \
               $(SRC_DIR)/server/CSVReader.cpp \
               $(SRC_DIR)/server/Wallet.cpp

CLIENT_SRCS := $(SRC_DIR)/client/client_app.cpp \
               $(SRC_DIR)/client/ClientMain.cpp

# Object files
SERVER_OBJS := $(patsubst $(SRC_DIR)/server/%.cpp,$(BUILD_DIR)/server_%.o,$(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/client/%.cpp,$(BUILD_DIR)/client_%.o,$(CLIENT_SRCS))

# Executables
SERVER_EXE := $(BUILD_DIR)/server$(EXE_EXT)
CLIENT_EXE := $(BUILD_DIR)/client$(EXE_EXT)

.PHONY: all clean directories

# Default target
all: directories $(SERVER_EXE) $(CLIENT_EXE)

# # Create build directory
# directories:
# 	@$(MKDIR_CMD)

# Linking server
$(SERVER_EXE): $(SERVER_OBJS)
	@echo "Linking Server..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Linking client
$(CLIENT_EXE): $(CLIENT_OBJS)
	@echo "Linking Client..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile server .cpp to .o
$(BUILD_DIR)/server_%.o: $(SRC_DIR)/server/%.cpp
	@echo "Compiling server $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile client .cpp to .o
$(BUILD_DIR)/client_%.o: $(SRC_DIR)/client/%.cpp
	@echo "Compiling client $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# # Clean build artifacts
# clean:
# 	@echo "Cleaning build directory..."
# 	@$(RMDIR_CMD)
