all:
	@g++ -std=c++17 -g -m64 -Wextra -Werror -pedantic-errors `pkg-config --static --libs glfw3` -lvulkan -o main main.cpp
release:
	@g++ -std=c++17 -m64 -Wextra -Werror -pedantic-errors -O2 -s -DNDEBUG `pkg-config --static --libs glfw3` -lvulkan -o main main.cpp
