src = `find src -name "*.cpp" | tr '\n' ' '`

unix:
	g++ -o bin/unix/G13 $(src) -lGLEW -lGLU -lGL -lglfw -lXrandr -Isrc -DUNIX -O2

unix-d:
	g++ -o bin/unix/G13 $(src) -lGLEW -lGLU -lGL -lglfw -lXrandr -Isrc -DUNIX -DDEBUG -g

osx:
	g++ -o bin/osx/G13 $(src) -lGLEW -lglfw -framework OpenGL -Isrc -DOSX -O2

osx-d:
	g++ -o bin/osx/G13 $(src) -lGLEW -lglfw -framework OpenGL -Isrc -DOSX -DDEBUG -g
