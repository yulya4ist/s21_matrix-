CC = g++
CFLAGS = -c -Wall -Werror -Wextra -g -std=c++17

TEST_CFLAGS = -lgtest -lgmock -pthread

OBJ = $(CFILES:.cc=.o)
TESTS_OBJ = $(TESTS_CFILES:.cc=.o)
TESTS_CFILES = $(wildcard tests/*.cc)
CFILES = *.cc
EXECUTABLE = s21_matrix
LIB = s21_matrix.a
GCOV_FLAGS=--coverage -Wall -Werror -Wextra -std=c++17

ifeq ($(shell uname -s),Linux)
	TEST_FLAGS += -lrt -lsubunit
endif


# $^ вызов всех подцелей
# $< вызов первой подцели
# $@ вызов цели

all: $(LIB) test

$(EXECUTABLE) : $(OBJ)
	$(CC) $^ -o $(EXECUTABLE)
	./$(EXECUTABLE)

%.o : %.cc
	$(CC) $(CFLAGS) $^ -o $@

$(LIB) : $(OBJ)
	ar -rc $(LIB) $(OBJ)
	ranlib $(LIB)

test : $(TESTS_OBJ) $(LIB)
	$(CC) $^ -o test $(TEST_CFLAGS)
	./test

checkstyle:
	clang-format -style=google -n tests/*.cc
	clang-format -style=google -n tests/*.h
	clang-format -style=google -n *.h
	clang-format -style=google -n *.cc

make_style:
	clang-format -style=google -i tests/*.cc
	clang-format -style=google -i tests/*.h
	clang-format -style=google -i *.h
	clang-format -style=google -i *.cc

#gcov_report: $(TESTS_OBJ)
#	$(CC) $(GCOV_FLAGS) $(TEST_CFLAGS) $(TESTS_OBJ) $(CFILES) -o test
#	./test
#	lcov -t "s21_matrix" -o s21_matrix.info -c -d .
#	genhtml -o report s21_matrix.info
#	open report/index.html

clean:
	rm -rf $(OBJ) $(LIB) $(TESTS_OBJ) test $(EXECUTABLE) *.gcov *.gcno *.gcda *.info report