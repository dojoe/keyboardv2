
all: menu_tests

menu_tests: 
	gcc -o test-Menus menu_tests.c -I../greatest -D__NO_INCLUDE_AVR
	./test-Menus
	rm  -f ./test-Menus

clean: 
	rm -f ./test-Menus

