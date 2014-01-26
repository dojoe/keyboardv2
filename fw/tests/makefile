
CFLAGS=-I../greatest -D__NO_INCLUDE_AVR

all: menu_tests



menu_tests: menu_tests.o ../menu.o
	c++ -o test-Menus menu_tests.o ../menu.o 
	./test-Menus
	rm ./test-Menus menu_tests.o ../menu.o

