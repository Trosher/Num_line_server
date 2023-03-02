ALL_SERV_FILE=server/server.cc NetProcessing/NetProcessing.cc RequsetHandler/RequsetHandler.cc
ALL_CLIENT_FILE=client/client.cc NetProcessing/NetProcessing.cc
CFLAGS=-lstdc++ -std=c++17 -lm -lpthread -Wall -Werror -Wextra 
CC=gcc

.PHONY: clean Executable_server

Executable_server:
	$(CC) $(ALL_SERV_FILE) $(CFLAGS) -o EServer

clean_server:
	rm -rf EServer

clean: clean_server
