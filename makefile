targets := win32 win32-d unix unix-d osx osx-d

help:
	@echo "Build targets: $(targets)"

# common
debug := no
platform := unknown
cxx := g++
inc := -Isrc
opt := -Wall

ifeq ($(patsubst %-d,-d,$(MAKECMDGOALS)),-d)
  debug := yes
endif

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),win32)
  platform := win32
endif

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),unix)
  platform := unix
endif

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),osx)
  platform := osx
endif

ifeq ($(debug),yes)
  out-dir-suffix := -d
endif

ifeq ($(platform),win32)
  out-dir := bin/win32$(out-dir-suffix)
  out := $(out-dir)/G13.exe
  lib := -lglew32 -lglfw -lopengl32 -lpng -lz
  def := -DWIN32 -DGLEW_STATIC
  ifeq ($(debug),no)
    opt += -mwindows
  endif
endif

ifeq ($(platform),unix)
  out-dir := bin/unix$(out-dir-suffix)
  out := $(out-dir)/G13
  lib := -lGLEW -lGLU -lGL -lglfw -lXrandr -lpng -lz
  def := -DUNIX -DPNG_SKIP_SETJMP_CHECK
endif

ifeq ($(platform),osx)
  out-dir := bin/osx$(out-dir-suffix)
  out := $(out-dir)/G13
  lib := -lGLEW -lglfw -framework OpenGL -lpng -lz
  def := -DOSX
endif

# if debug
ifeq ($(debug),yes)
  opt += -g
  def += -DDEBUG
else
  opt += -O2
endif

modules := . $(patsubst src/%,%,$(shell find src -depth -type d))
src-dir := $(addprefix src/,$(modules))
bin-dir := $(addprefix $(out-dir)/,$(modules))
src := $(foreach sdir,$(src-dir),$(wildcard $(sdir)/*.cpp))
obj := $(patsubst src/%.cpp,$(out-dir)/%.o,$(src))
dep := $(patsubst src/%.cpp,$(out-dir)/%.d,$(src))

$(targets): $(bin-dir) $(out)

$(out): makefile $(obj)
	$(cxx) -o $(out) $(obj) $(lib) $(opt)

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),help)
    ifneq ($(MAKECMDGOALS),)
      -include $(dep)
    endif
  endif
endif

# make-depend(dep-file,src-file,stem)
define make-depend
	$(cxx) -MM $(inc) $(def) $2 | \
	sed -e 's,\($3\)\.o[ :]*,$(patsubst %.d,%.o,$1) $1 : ,g' \
	-e 's/\\$$/^^/' \
	-e 's/\\/\//g' \
	-e 's/\^\^$$/\\/' > $1.tmp1
	sed -e 's/#.*//' \
	-e 's/^[^:]*: //' \
	-e 's/^ *//' \
	-e 's/ *\\$$/:/' \
	-e 's/[^:]$$/&:/' \
	-e 's/\\/\//g' $1.tmp1 > $1.tmp2
	cat $1.tmp1 $1.tmp2 > $1
	rm $1.tmp1
	rm $1.tmp2
endef

# obj-rule(module)
define obj-rule
$(out-dir)/$1/%.o: src/$1/%.cpp
	@$$(call make-depend,$$(patsubst %.o,%.d,$$@),$$<,$$*)
	$(cxx) -o $$@ -c $$< $(inc) $(def) $(opt)
endef

$(foreach module,$(modules),$(eval $(call obj-rule,$(module))))

$(bin-dir):
	@mkdir -p $@

clean:
	rm -r bin

.PHONY: clean help $(targets)
