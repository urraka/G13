platform := unknown
debug := no

# detect platform with uname

ifeq ($(shell uname | grep 'MINGW32_NT' -c),1)
  platform := win32
endif

ifeq ($(shell uname | grep 'Linux' -c),1)
  platform := linux
endif

ifeq ($(shell uname | grep 'Darwin' -c),1)
  platform := osx
endif

# detect platform from target name

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),win32)
  platform := win32
endif

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),linux)
  platform := linux
endif

ifeq ($(patsubst %-d,%,$(MAKECMDGOALS)),osx)
  platform := osx
endif

# make sure that a platform is set

ifeq ($(platform),unknown)
  ifneq ($(MAKECMDGOALS),clean)
    ifneq ($(MAKECMDGOALS),help)
      $(error Could not detect platform.)
    endif
  endif
endif

# detect debug mode

ifeq ($(MAKECMDGOALS),debug)
  debug := yes
endif

ifeq ($(patsubst %-d,-d,$(MAKECMDGOALS)),-d)
  debug := yes
endif

# configuration (vars: cxx, opt, lib, def, out-dir, out-dir-suffix, out)

cxx := g++
opt := -Wall -fno-exceptions -fno-rtti -D__STDC_LIMIT_MACROS -DGLM_FORCE_RADIANS
inc := -Isrc
glsl-binary := bin/glsl

ifeq ($(debug),yes)
  opt += -g
  def += -DDEBUG
  out-dir-suffix := -d
else
  opt += -O3 -ffast-math
endif

ifeq ($(platform),win32)
  glsl-binary := bin/glsl.exe
  out-dir := bin/win32$(out-dir-suffix)
  out := $(out-dir)/G13.exe
  lib := lib/win32/libglew32.a
  lib += lib/win32/libglfw3.a
  lib += lib/win32/libpng.a
  lib += lib/win32/libz.a
  lib += lib/win32/libfixmath.a
  lib += lib/win32/libenet.a
  lib += lib/win32/libfreetype.a
  lib += -lgdi32 -lopengl32 -lws2_32 -lwinmm
  def += -DWIN32 -DGLEW_STATIC
  opt += -static-libgcc -static-libstdc++
  ifeq ($(debug),no)
    opt += -s -mwindows
  endif
endif

ifeq ($(platform),linux)
  out-dir := bin/linux$(out-dir-suffix)
  out := $(out-dir)/G13.bin
  lib += lib/linux/libGLEW.a
  lib += lib/linux/libglfw3.a
  lib += lib/linux/libpng15.a
  lib += lib/linux/libz.a
  lib += lib/linux/libfixmath.a
  lib += lib/linux/libenet.a
  lib += lib/linux/libfreetype.a
  lib += -lpthread -lXcursor -lXi -lX11 -lXxf86vm -lXrandr -lGL
  inc += -Iinclude
  def += -DUNIX -DPNG_SKIP_SETJMP_CHECK
  ifeq ($(debug),no)
    opt += -s
  endif
endif

ifeq ($(platform),osx)
  cxx := clang++
  out-dir := bin/osx$(out-dir-suffix)
  out := $(out-dir)/G13.bin
  lib += lib/osx/libGLEW.a
  lib += lib/osx/libglfw3.a
  lib += lib/osx/libpng15.a
  lib += lib/osx/libz.a
  lib += lib/osx/libfixmath.a
  lib += lib/osx/libenet.a
  lib += lib/osx/libfreetype.a
  lib += -framework OpenGL
  lib += -framework Cocoa
  lib += -framework IOKit
  lib += -framework CoreFoundation
  lib += -framework CoreVideo
  inc += -Iinclude
  def += -DOSX
endif

# cpp/obj/dep
modules := . $(patsubst src/%,%,$(shell find src -depth -type d))
src-dir := $(addprefix src/,$(modules))
bin-dir := $(addprefix $(out-dir)/,$(modules))
src := $(foreach sdir,$(src-dir),$(wildcard $(sdir)/*.cpp))
obj := $(patsubst src/%.cpp,$(out-dir)/%.o,$(src))
dep := $(patsubst src/%.cpp,$(out-dir)/%.d,$(src))

# glsl
glsl-dir := ./src/gfx/glsl
glsl-src := $(wildcard $(glsl-dir)/*.vert) $(wildcard $(glsl-dir)/*.frag)
glsl-out := $(glsl-dir)/glsl.h

# default target
win32 win32-d linux linux-d osx osx-d debug: $(bin-dir) $(glsl-out) $(out)

# help
help:
	@echo "Usage: make [debug]"

# glsl.h
$(glsl-out): $(glsl-binary) $(glsl-src)
	@echo Compiling shaders...
	@$(glsl-binary) $(filter-out $(glsl-binary),$^) > $@

# glsl binary
$(glsl-binary): tools/glsl.cpp
	@echo Compiling glsl.cpp...
	@$(cxx) $< -O2 -o $@

# linking
$(out): makefile $(obj)
	@echo Linking...
	@$(cxx) -o $(out) $(obj) $(lib) $(opt)

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),help)
    -include $(dep)
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
	@echo Compiling $$<...
	@$(cxx) -o $$@ -c $$< $(inc) $(def) $(opt)
endef

$(foreach module,$(modules),$(eval $(call obj-rule,$(module))))

$(bin-dir):
	@mkdir -p $@

clean:
	rm -r bin

.PHONY: clean help debug win32 win32-d linux linux-d osx osx-d
