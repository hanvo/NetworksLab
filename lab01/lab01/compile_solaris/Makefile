# Makefile for Computer Networks and Internets TCP API
# Solaris Version

DEFS	= -DSOLARIS
INC_DIR	= ../h
INCLUDE = -I$(INC_DIR)
LD	= ld
CC	= gcc
LIBS	= -lsocket -lnsl -lpthread
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
APP_SRC = 	echoserver.c		\
		echoclient.c		\
		chatserver.c		\
		chatclient.c		\
		webserver.c		\
		webclient.c		\
		readln.c

APP_OBJ = $(APP_SRC:.c=.o)
APP_EXE = echoserver echoclient chatserver chatclient webserver webclient

CFILES = $(API_DIR)/*.c $(APP_DIR)/*.c

apps: $(APP_EXE)

echoserver: cnaiapi.o echoserver.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) echoserver.o readln.o
echoclient: cnaiapi.o echoclient.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) echoclient.o readln.o
chatserver: cnaiapi.o chatserver.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) chatserver.o readln.o
chatclient: cnaiapi.o chatclient.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) chatclient.o readln.o
webserver: cnaiapi.o webserver.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) webserver.o readln.o
webclient: cnaiapi.o webclient.o readln.o
	$(CC) -o $@ $(LIBS) $(CNAIAPI_LIB) webclient.o readln.o

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
chatclient.o: $(APP_DIR)/chatclient.c $(INC_DIR)/cnaiapi.h
chatserver.o: $(APP_DIR)/chatserver.c $(INC_DIR)/cnaiapi.h
echoclient.o: $(APP_DIR)/echoclient.c $(INC_DIR)/cnaiapi.h
echoserver.o: $(APP_DIR)/echoserver.c $(INC_DIR)/cnaiapi.h
webclient.o: $(APP_DIR)/webclient.c $(INC_DIR)/cnaiapi.h
webserver.o: $(APP_DIR)/webserver.c $(INC_DIR)/cnaiapi.h
readln.o: $(APP_DIR)/readln.c $(INC_DIR)/cnaiapi.h
