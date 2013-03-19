modules := . Game Graphics System

targets := win32 win32-d unix unix-d osx osx-d

help:
	@echo "Build targets: $(targets)"

# common for all systems
cxx := g++
inc := -Isrc

# if debug
ifeq ($(patsubst %-d,-d,$(MAKECMDGOALS)),-d)
  dbg := -dbg
endif

# windows
ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),win32)
  out-dir := bin/win32$(dbg)
  out := $(out-dir)/G13.exe
  lib := -lglew32 -lglfw -lopengl32 -lpng -lz
  def := -DWIN32 -DGLEW_STATIC
endif

# unix
ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),unix)
  out-dir := bin/unix$(dbg)
  out := $(out-dir)/G13
  lib := -lGLEW -lGLU -lGL -lglfw -lXrandr -lpng -lz
  def := -DUNIX
endif

# osx
ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),osx)
  out-dir := bin/osx$(dbg)
  out := $(out-dir)/G13
  lib := -lGLEW -lglfw -framework OpenGL -lpng -lz
  def := -DOSX
endif

# if debug
ifeq ($(patsubst %-d,-d,$(MAKECMDGOALS)),-d)
  opt += -g
  def += -DDEBUG
else
  opt += -O2
endif

src-dir := $(addprefix src/,$(modules))
bin-dir := $(addprefix $(out-dir)/,$(modules))
src := $(foreach sdir,$(src-dir),$(wildcard $(sdir)/*.cpp))
obj := $(patsubst src/%.cpp,$(out-dir)/%.o,$(src))
dep := $(patsubst src/%.cpp,$(out-dir)/%.d,$(src))

$(targets): $(bin-dir) $(out)

$(out): $(obj)
	$(cxx) -o $(out) $(obj) $(lib)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),help)
ifneq ($(MAKECMDGOALS),)
-include $(dep)
endif
endif
endif

# dep-rule(module)
define dep-rule
$(out-dir)/$1/%.d: src/$1/%.cpp
	$(cxx) -M $(inc) $(def) $$< > $$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@; \
	rm -f $$@.$$$$
endef

# obj-rule(module)
define obj-rule
$(out-dir)/$1/%.o: src/$1/%.cpp
	$(cxx) -o $$@ -c $$< $(lib) $(inc) $(def) $(opt)
endef

$(foreach module,$(modules),$(eval $(call dep-rule,$(module))))
$(foreach module,$(modules),$(eval $(call obj-rule,$(module))))

$(bin-dir):
	@mkdir -p $@

clean:
	rm -r bin

.PHONY: clean help $(targets)
