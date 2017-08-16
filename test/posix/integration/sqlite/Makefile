all: unix sqlite_test

unix: tcph.c sqlite3.c
	 gcc tcph.c sqlite3.c -ldl -o tcph

sqlite_test: sqlite_test.c sqlite3.c
	 gcc sqlite_test.c sqlite3.c -ldl -o sqlite_test

clean:
	rm -f tcph
