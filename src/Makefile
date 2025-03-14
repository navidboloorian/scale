all: backend.c threading.c server.c
	gcc -I ../include -o scale backend.c threading.c socket.c server.c -g

clean:
	rm scale