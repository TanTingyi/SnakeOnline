CC = g++
CFLAGS = -Wall -g 
LIBS = -pthread -lncurses

server_ncu1128: 
	$(CC) $(CFLAGS) server_ncu1128.cpp -o $@ $(LIBS)

client1128: 
	$(CC) $(CFLAGS) client1128.cpp -o $@ $(LIBS)

test:
	$(CC) $(CFLAGS) test.cpp -o $@ $(LIBS)

.PHONE: clean
clean:
	@rm -rf *.out test client server_ncu1128 server_ncu client1128

