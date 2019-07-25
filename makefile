CC = g++
CFLAGS = -Wall -g 
LIBS = -pthread -lncurses

server_ncu1128: 
	$(CC) $(CFLAGS) server.cpp -o $@ $(LIBS)

client1128: 
	$(CC) $(CFLAGS) client.cpp -o $@ $(LIBS)

.PHONE: clean
clean:
	@rm -rf *.out  client server 

