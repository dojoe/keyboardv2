
all: menu_tests timer_tests


timer_tests:
	gcc -o test-Pizzatimer timer_tests.c -I../greatest -D__NO_INCLUDE_AVR
	./test-Pizzatimer
	rm -f ./test-Pizzatimer

menu_tests: 
	gcc -o test-Menus menu_tests.c -I../greatest -D__NO_INCLUDE_AVR
	./test-Menus
	rm  -f ./test-Menus

clean: 
	rm -f ./test-Menus

