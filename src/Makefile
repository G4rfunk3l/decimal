CC=gcc
FLAGS=-std=c11 -Wall -Werror -Wextra
LIBS_MACOS= -L /usr/local/lib -lcheck
LIBS_LINUX= -lcheck -lm -lpthread -lrt -lsubunit -D_GNU_SOURCE
 
OS = $(shell uname)
ifeq ($(OS), Darwin)
OPEN=open
LIBS=$(LIBS_MACOS)
endif
 
ifeq ($(OS), Linux)
OPEN=xdg-open
LIBS=$(LIBS_LINUX)
endif

all: clean my_decimal.a

my_decimal.o_with_gcov: my_decimal.c
	$(CC) $(FLAGS) -c my_decimal.c --coverage

my_decimal.a_with_gcov: my_decimal.o_with_gcov
	ar -rcs my_decimal.a my_decimal.o

test: clean my_decimal.a_with_gcov
	$(CC) tests.c my_decimal.a -o my_test $(LIBS) --coverage
	./my_test
	rm -f my_test.gcda my_test.gcno my_test.c

test_val: clean my_decimal.a_with_gcov
	$(CC) tests.c my_decimal.a -o my_test $(LIBS) --coverage
	valgrind ./my_test > log.txt 2>&1
	grep -e "ERROR SUMMARY" log.txt
	rm -f my_test.gcda my_test.gcno my_test.c

my_decimal.a: my_decimal.c
	$(CC) -c my_decimal.c
	ar -rcs my_decimal.a my_decimal.o

gcov_report: test
	gcovr --html --html-details -o report.html
	$(OPEN) report.html

lcov: test
	lcov -t “test” -o lcov_test.info -c -d .
	genhtml -o report lcov_test.info
	$(OPEN) ./report/index.html
	
codestyle: 
	clang-format -i *.c
	clang-format -i *.h
	clang-format -n *.c
	clang-format -n *.h

clean:
	rm -f *.out *.gcda *.gcno *.o *.a report.* my_test test_with_gcov* *.gcov my_test.c *.info
	rm -rf ./lcov_report report/ log.txt
