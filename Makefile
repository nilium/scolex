BINARY=scolex

OBJECT_DIR=objs
DEPS_DIR=deps

SOURCES=$(wildcard *.cc)
OBJECT_FILES=$(SOURCES:.cc=.o)
DEP_FILES=$(addprefix $(DEPS_DIR)/,$(OBJECT_FILES:.o=.d))
OBJECT_PATHS=$(addprefix $(OBJECT_DIR)/,$(OBJECT_FILES))

MKDIR_P=mkdir -p

CFLAGS+=-Wall -Wextra -Wno-c++98-compat -Wno-c++98-compat-pedantic
CXXFLAGS+=-std=c++11 -stdlib=libc++

.PHONY: all clean directories

all: directories $(BINARY)

directories: $(OBJECT_DIR) $(DEPS_DIR)

$(DEPS_DIR) $(OBJECT_DIR):
	$(MKDIR_P) $@

-include $(DEP_FILES)

$(OBJECT_DIR)/%.o: %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -MMD -MP -MF $(DEPS_DIR)/$*.d -c -o $@ $<

$(BINARY): $(OBJECT_PATHS)
	$(CXX) $(LDFLAGS) $(LDFLAGS_EXTRA) -o $@ $^

clean:
	$(RM) -r $(OBJECT_DIR) $(DEPS_DIR)
	$(RM) $(BINARY)
