BINARY=scolex
SOURCES=$(wildcard *.cc)
OBJECT_FILES=$(SOURCES:.cc=.o)
DEP_FILES=$(SOURCES:.cc=.d)

CFLAGS+=-Wall -Wextra -Wno-c++98-compat -Wno-c++98-compat-pedantic
CXXFLAGS+=-std=c++11 -stdlib=libc++


.PHONY: all clean deps

all: $(BINARY)

-include $(DEP_FILES)

%.o: %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -MMD -MP -MF $*.d -c -o $@ $<

$(BINARY): $(OBJECT_FILES)
	$(CXX) $(LDFLAGS) $(LDFLAGS_EXTRA) -o $@ $^

clean:
	$(RM) $(BINARY) $(OBJECT_FILES) $(DEP_FILES)
