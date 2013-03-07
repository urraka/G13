out = -o bin/G13.bin
src = `find src -name "*.cpp" | tr '\n' ' '`
lib = -lGLEW -lGLU -lGL -lglfw -lXrandr
inc = -Isrc
def = -DUNIX

rel = -O2
dbg = -DDEBUG -g

cmd = g++ $(out) $(src) $(lib) $(inc) $(def)

release:
	$(cmd) $(rel)

debug:
	$(cmd) $(dbg)
