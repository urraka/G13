src = `find src -name "*.cpp" | tr '\n' ' '`

default:
	@echo "Usage: make (win32|win32-d|unix|unix-d|osx|osx-d|ios)"

win32:
	@mkdir -p bin/win32
	g++ -o bin/win32/G13.exe $(src) -lglew32 -lglfw -lopengl32 -lpng -lz -Isrc -DGLEW_STATIC -DWIN32 -O2

win32-d:
	@mkdir -p bin/win32
	g++ -o bin/win32/G13.exe $(src) -lglew32 -lglfw -lopengl32 -lpng -lz -Isrc -DGLEW_STATIC -DWIN32 -DDEBUG -g

unix:
	@mkdir -p bin/unix
	g++ -o bin/unix/G13 $(src) -lGLEW -lGLU -lGL -lglfw -lXrandr -lpng -lz -Isrc -DUNIX -O2

unix-d:
	@mkdir -p bin/unix
	g++ -o bin/unix/G13 $(src) -lGLEW -lGLU -lGL -lglfw -lXrandr -lpng -lz -Isrc -DUNIX -DDEBUG -g

osx:
	@mkdir -p bin/osx
	g++ -o bin/osx/G13 $(src) -lGLEW -lglfw -framework OpenGL -lpng -lz -Isrc -DOSX -O2

osx-d:
	@mkdir -p bin/osx
	g++ -o bin/osx/G13 $(src) -lGLEW -lglfw -framework OpenGL -lpng -lz -Isrc -DOSX -DDEBUG -g

ios:
	xcodebuild -project proj/xcode/G13.xcodeproj -configuration Debug -arch i386 -sdk iphonesimulator
