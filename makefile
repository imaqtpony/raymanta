SRC_DIR := src
BUILD_DIR := build
DEPS_DIR := .deps
TARGET := raymanta

COMMON_SRC := lodepng.cpp
COMMON_SRC := $(addprefix $(SRC_DIR)/, $(COMMON_SRC))
COMMON_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(COMMON_SRC))

CXXFLAGS := -std=c++17 -Wall -Wextra
INC := -I$(SRC_DIR)

.PHONY: all
all: $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)

$(TARGET): $(BUILD_DIR)/main.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $@
$(DEPS_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(DEPS_DIR)
	$(CXX) $(CXXFLAGS) $(INC) -MMD -MF $(DEPS_DIR)/$*.d -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(BUILD_DIR)/*.o $(DEPS_DIR)/*.d

-include $(DEPS_DIR)/*.d