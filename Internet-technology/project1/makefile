.c.o:
	gcc -g -c $? -lws2_32

# compile client and server
all: udp-client udp-server

# compile client only
udp-client: udp-client.o 
	gcc -g -o udp-client udp-client.o -lws2_32

# compile server program
udp-server: udp-server.o
	gcc -g -o udp-server udp-server.o -lws2_32