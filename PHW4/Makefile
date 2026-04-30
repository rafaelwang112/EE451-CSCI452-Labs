CXX = mpicxx

CXXFLAGS = -Wall -O2

TARGETS = p1 p2_1 p2_2 p2_3

all: $(TARGETS)

p1: p1.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

p2_1: p2_1.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

p2_2: p2_2.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

p2_3: p2_3.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean