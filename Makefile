server: server.c
	mkdir files; gcc -W -o server server.c
clean:
	rm server; rm -dfr files
run:
	./server
compile:
	make clean; make
