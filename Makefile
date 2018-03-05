BASE = main
LDFLAGS = `pkg-config --libs glfw3 glew glu gl`
CPPFLAGS = -MMD -MP
CXXFLAGS = -O2
SOURCEDIR = src
BUILDDIR = build
RESOURCEDIR = res

VERTSHADERS := $(wildcard $(SOURCEDIR)/*.vert)
FRAGSHADERS := $(wildcard $(SOURCEDIR)/*.frag)
BUILDSHADERS := $(VERTSHADERS:$(SOURCEDIR)/%.vert=$(BUILDDIR)/%.vert) $(FRAGSHADERS:$(SOURCEDIR)/%.frag=$(BUILDDIR)/%.frag)

SOURCEOBJ := $(wildcard $(SOURCEDIR)/*.obj)
BUILDOBJ := $(SOURCEOBJ:$(SOURCEDIR)/%.obj=$(BUILDDIR)/%.obj)

SOURCES := $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SOURCEDIR)/%.cpp=$(BUILDDIR)/%.o)
DEPENDS := $(OBJECTS:.o=.d)
CXX = g++ 

all: $(BUILDDIR)/$(BASE) $(BASE) extras
debug: CXXFLAGS = -g
debug: $(BUILDDIR)/ $(BUILDDIR)/$(BASE) $(BASE) extras

$(BUILDDIR)/$(BASE): $(OBJECTS) | $(BUILDDIR)
	$(LINK.cpp) $(OBJECTS) -o $@

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp | $(BUILDDIR)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(BUILDDIR):
	@mkdir -p $@

$(BASE):
	@echo "#!/bin/sh" > $(BASE)
	@echo "(cd $(BUILDDIR) && exec ./$(BASE))" >> $(BASE)
	@chmod +x $(BASE)

-include $(DEPENDS)

extras: $(BUILDSHADERS) $(BUILDDIR)/$(RESOURCEDIR)

$(BUILDDIR)/%.vert: $(SOURCEDIR)/%.vert
	@ln -s ../$< $@

$(BUILDDIR)/%.frag: $(SOURCEDIR)/%.frag
	@ln -s ../$< $@

$(BUILDDIR)/$(RESOURCEDIR): $(RESOURCEDIR)
	@ln -s ../$< $@

clean:
	rm -f $(BASE) $(BUILDDIR)/*
