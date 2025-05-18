# Compiler and tools
CXX = g++
FLEX = flex
BISON = bison

# Output binary name
TARGET = parser

# Source files
LEXER = lexer.l
PARSER = parser.y
SRCS = main.cpp ast.cpp ast_printer.cpp parser.tab.c lex.yy.c SymbolTable.cpp

# Default rule
all: $(TARGET)

# Generate parser files
parser.tab.c parser.tab.h: $(PARSER)
	$(BISON) -d $(PARSER)

# Generate lexer file
lex.yy.c: $(LEXER)
	$(FLEX) $(LEXER)

# Build the executable
$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) main.cpp ast_printer.cpp ast.cpp parser.tab.c lex.yy.c SymbolTable.cpp -lfl

# Clean up generated files
clean:
	rm -f $(TARGET) parser.tab.c parser.tab.h lex.yy.c *.o

# Run the parser with test input
run: $(TARGET)
	./$(TARGET) test.prog //add an option here