# Makefile for Comer CNAI Simple TCP API
# Linux Version

DEFS	= -DFREEBSD
INC_DIR	= ../h
INCLUDE = -I$(INC_DIR)
LD	= ld
CC	= gcc
LIBS	= 
FLAGS   = -c -g

CNAIAPI_LIB = cnaiapi.o

API_DIR = ../api
API_SRC = 	appname_to_appnum.c	\
		cname_to_comp.c		\
		await_contact.c		\
		make_contact.c		\
		send_eof.c		\
		cnaiapi_init.c

API_OBJ = $(API_SRC:.c=.o)

APP_DIR = ../apps
APP_SRC =	server.c		\
		client.c		\
		echoserver.c		\
		echoclient.c		\
		readln.c

APP_OBJ = $(APP_SRC:.c=.o)
APP_EXE = echoserver echoclient server client

CFILES = $(API_DIR)/*.c $(APP_DIR)/*.c

apps: $(APP_EXE)

server: cnaiapi.o server.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) server.o readln.o
client: cnaiapi.o client.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) client.o readln.o
echoserver: cnaiapi.o echoserver.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) echoserver.o readln.o
echoclient: cnaiapi.o echoclient.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) echoclient.o readln.o

$(APP_OBJ):
	$(CC) -c $(INCLUDE) $(FLAGS) $(DEFS) $(APP_DIR)/$(@:.o=.c)
$(CNAIAPI_LIB): $(API_OBJ)
	$(LD) -r -o $(CNAIAPI_LIB) $(API_OBJ)
$(API_OBJ):
	$(CC) $(INCLUDE) $(FLAGS) $(DEFS) $(API_DIR)/$(@:.o=.c)

clean:
	\rm -rf *.o core $(CNAIAPI_LIB) $(APP_EXE)

# API Dependancies
appname_to_appnum.o: $(API_DIR)/appname_to_appnum.c $(INC_DIR)/cnaiapi.h
await_contact.o: $(API_DIR)/await_contact.c $(INC_DIR)/cnaiapi.h
cname_to_comp.o: $(API_DIR)/cname_to_comp.c $(INC_DIR)/cnaiapi.h
make_contact.o: $(API_DIR)/make_contact.c $(INC_DIR)/cnaiapi.h
send_eof.o: $(API_DIR)/send_eof.c $(INC_DIR)/cnaiapi.h

# Application Dependancies
client.o: $(APP_DIR)/client.c $(INC_DIR)/cnaiapi.h
server.o: $(APP_DIR)/server.c $(INC_DIR)/cnaiapi.h
echoclient.o: $(APP_DIR)/echoclient.c $(INC_DIR)/cnaiapi.h
echoserver.o: $(APP_DIR)/echoserver.c $(INC_DIR)/cnaiapi.h
readln.o: $(APP_DIR)/readln.c $(INC_DIR)/cnaiapi.h
