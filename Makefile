# Compiler and tools
CXX = g++
FLEX = flex
BISON = bison

# LLVM config
LLVM_CONFIG = llvm-config
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags --libs core orcjit native) -lpthread -ldl

# Output binary names
TARGET = parser
FLEX = flex

# Source files
LEXER = lexer.l
PARSER = parser.y
COMMON_SRCS = main.cpp ast.cpp SymbolTable.cpp codegen.cpp ast_interface.cpp
GEN_SRCS = parser.tab.c lex.yy.c
SRCS = $(COMMON_SRCS) $(GEN_SRCS)

# Compiler flags
CXXFLAGS = -std=c++17 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
CXXFLAGS += $(LLVM_CXXFLAGS)
CXXFLAGS += -fexceptions  # force exceptions enabled last


LDFLAGS = $(LLVM_LDFLAGS) -lfl

# Default rule
all: $(TARGET)

# Generate parser files
parser.tab.c parser.tab.h: $(PARSER)
	$(BISON) -d $(PARSER)

# Generate lexer file
lex.yy.c: $(LEXER)
	$(FLEX) $(LEXER)

# Build the parser with LLVM and exception support
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Build flec binary (if different)
$(FLEC): $(COMMON_SRCS)
	$(CXX) $(CXXFLAGS) -o $(FLEC) $^ $(LDFLAGS)

# Clean up generated files
clean:
	rm -f $(TARGET) $(FLEC) parser.tab.c parser.tab.h lex.yy.c *.o

# Run the parser with test input
run: $(TARGET)
	./$(TARGET) test4.prog
