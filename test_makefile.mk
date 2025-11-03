# Test Makefile without GPU
OBJDIR = obj
OBJET = $(OBJDIR)/keccak256.o

CXX = g++
CXXFLAGS = -m64 -mssse3 -Wno-write-strings -O2 -I.

$(OBJDIR)/keccak256.o: hash/keccak256.cpp hash/keccak256.h
mkdir -p $(OBJDIR)
$(CXX) $(CXXFLAGS) -o $@ -c hash/keccak256.cpp

test: $(OBJET)
@echo "Keccak256 compiled successfully"

clean:
rm -f $(OBJDIR)/*.o
