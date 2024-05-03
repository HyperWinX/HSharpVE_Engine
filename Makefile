all: clean
	@mkdir build
	@cmake -S . -B build
	@cmake --build build
	@cp build/compile_commands.json .

clean:
	@rm -rf build