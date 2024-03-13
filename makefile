TARGET = s-talk

all: $(TARGET)

s-talk:
	gcc -Wall -Werror main.c UDPClient.c UDPServer.c list.c inputReader.c outputWriter.c threadManager.c freeManager.c -o $(TARGET) -lpthread
	
clean:
	rm -f $(TARGET)