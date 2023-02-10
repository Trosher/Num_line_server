ALL_SERV_FILE = server/server.cc NetProcessing/NetProcessing.cc
ALL_CLIENT_FILE = client/client.cc NetProcessing/NetProcessing.cc
CC = g++

all: clean NetProcessing

server:
	$(CC) $(ALL_SERV_FILE) -o server

client:
	$(CC) $(ALL_CLIENT_FILE) -o client

clean:
	rm -rf server
	rm -rf client