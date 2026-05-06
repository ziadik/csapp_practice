# Простой Makefile
CC = cl
CFLAGS = /nologo /W3
LIBS = ws2_32.lib

all: server.exe client.exe

server.exe: echoserver.c
	$(CC) $(CFLAGS) echoserver.c /Fe:server.exe /link $(LIBS)

client.exe: echoclient.c
	$(CC) $(CFLAGS) echoclient.c /Fe:client.exe /link $(LIBS)

clean:
	del *.exe *.obj *.pdb *.ilk /q

rebuild: clean all