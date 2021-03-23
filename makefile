CC=g++

vpath %.cpp src

all: bin/shader_sandbox

bin/shader_sandbox: main.o stb_image.o
	$(CC) -o $@ $^ -l glfw -l epoxy

%.o: %.cpp
	$(CC) -c $< -Iinclude -Wall

clean:
	rm main.o bin/shader_sandbox
