CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Isrc
EMXX     = em++

SRCS = $(wildcard src/*.cpp src/*/*.cpp)
OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))
LIB_OBJS = $(filter-out build/main.o,$(OBJS))

.PHONY: all test wasm clean

all: build/app

build/app: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# tests/test_*.cpp 를 각각 빌드해서 실행
test: $(LIB_OBJS)
	@mkdir -p build/tests
	@for t in $(wildcard tests/test_*.cpp); do \
		name=$$(basename $$t .cpp); \
		$(CXX) $(CXXFLAGS) $$t $(LIB_OBJS) -o build/tests/$$name && ./build/tests/$$name || exit 1; \
	done

# WebAssembly 빌드 (Emscripten 필요)
wasm:
	$(EMXX) -std=c++17 -O2 -Isrc $(SRCS) -o web/app.js \
		-s MODULARIZE=1 -s EXPORT_NAME=createApp \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		--preload-file data/processed@/data

clean:
	rm -rf build web/app.js web/app.wasm web/app.data
