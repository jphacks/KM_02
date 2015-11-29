# ------------------------------------
# Makefile
# ------------------------------------

# target
TARGET = $(shell basename `pwd`)

# directory
OUT_DIR = build
SRC_DIR = src
HEADER_DIR = include
PROGRAM_DIR = $(OUT_DIR)/bin
LIB_OUT_DIR = $(OUT_DIR)/lib
OBJ_DIR = $(OUT_DIR)/obj

# input files
SRCS = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJ_DIR)/,$(SRCS:.cpp=.d))


# compiler
CXX = g++ -std=c++11 
CXXFLAGS = -I./$(HEADER_DIR)
LDFLAGS  = `pkg-config --libs libssl` `pkg-config --libs oauth` -liconv

OPENCV_INC = `pkg-config --cflags opencv`
OPENCV_LIB = `pkg-config --libs opencv`
OPENSSL_LIB = `pkg-config --libs libssl`
ICONV_LIB = -liconv
OS = $(shell uname)
ifeq ($(OS), Linux)
	ICONV_LIB = -liconv_hook
else ifeq ($(OS), Darwin)
	CXX = clang++ -std=c++11 -stdlib=libc++
	OPENSSL_LIB = `pkg-config --libs openssl`
else ifeq ($(OS), MINGW64_NT-6.1)
	CXX = g++ -std=c++14
	OPENCV_INC = `pkg-config --cflags opencv_static`
	OPENCV_LIB = `pkg-config --libs opencv_static`
endif

# dummy target
.PHONY : release debug all clean

# release
release: CXX += -O2
# release: STATICFLAG = -static
release: CXXFLAGS = -I./$(HEADER_DIR) $(OPENCV_INC) `pkg-config --cflags jsoncpp`
release: LDFLAGS  = -L./ -ltwicppspp $(OPENCV_LIB) $(OPENSSL_LIB) `pkg-config --libs oauth` `pkg-config --libs jsoncpp` $(ICONV_LIB)
release: all

# debug
debug: CXX += -g -O0
debug: all

# all
all: $(TARGET)
$(TARGET): $(OBJS)
	@mkdir -p $(PROGRAM_DIR)
	$(CXX) $(STATICFLAG) $^ $(LDFLAGS) -o $(PROGRAM_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -MMD -MP -MF $(@:%.o=%.d) $< -o $@

# コマンドラインから与えた最終ターゲットが "clean" 以外
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

# clean
clean:
	$(RM) -r $(OUT_DIR)
