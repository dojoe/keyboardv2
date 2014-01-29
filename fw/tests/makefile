
all: menu_tests

menu_tests: 
	gcc -o test-Menus menu_tests.c -I../greatest -D__NO_INCLUDE_AVR
	./test-Menus
	rm ./test-Menus menu_tests.o ../menu.o

clean: 
	rm -f menu_test.o ../menu.o ./test-Menus

