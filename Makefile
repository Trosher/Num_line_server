ALL_SERV_FILE = server/server.cc NetProcessing/NetProcessing.cc RequsetHandler/RequsetHandler.cc
ALL_CLIENT_FILE = client/client.cc NetProcessing/NetProcessing.cc
CC = g++

.PHONY: clean Executable_server Executable_client

Executable_server:
	$(CC) $(ALL_SERV_FILE) -o EServer

Executable_client:
	$(CC) $(ALL_CLIENT_FILE) -o EClient

clean_server:
	rm -rf EServer

clean_client:
	rm -rf EClient

clean: clean_server clean_client
