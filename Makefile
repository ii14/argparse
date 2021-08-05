SOURCES  = argparse.cpp test.cpp
CXXFLAGS = -std=c++11 -Wall -Wextra -g

OBJS     = $(addprefix build/,$(SOURCES:.cpp=.o))
DEPS     = $(OBJS:.o=.d)

all: test

build/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -MMD -o $@ $<

build/test: $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o build/test $(OBJS) $(LDFLAGS)

-include $(DEPS)

test: build/test
	@./build/test

.PHONY: all test clean info

clean:
	@rm -rvf build/test $(OBJS) $(DEPS)

info:
	@echo "[*] Sources:      $(SOURCES)"
	@echo "[*] Objects:      $(OBJS)"
	@echo "[*] Dependencies: $(DEPS)"
