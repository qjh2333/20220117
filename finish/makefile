all:
	rm *.o
	rm ser cli dic.db
ser:
	gcc -c ./main/mainser.c -o mainser.o
	gcc -c ./op/opser.c -o opser.o
	gcc mainser.o opser.o -pthread -lsqlite3 -o ser
cli:
	gcc -c ./main/maincli.c -o maincli.o
	gcc -c ./op/opcli.c -o opcli.o
	gcc maincli.o opcli.o -o cli
