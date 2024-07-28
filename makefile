CC=gcc
#Intel:
CFLAGS=-O2 -march=native -ftree-vectorize -fomit-frame-pointer -Wno-unused-result
#PowerPC
#CFLAGS=-O2 -mcpu=750 -mtune=750 -fomit-frame-pointer -mpowerpc-gfxopt -Wno-unused-result
#Debug
#CFLAGS=-g -mavx2 -mfma -fsanitize=address

MATH=-lm
JSON=-ljson-c
CURL=-lcurl
READLINE=-lreadline

all: bin/build-vector-db-from-server \
     bin/embedding-from-server-cli \
     bin/rag-conversation \
     bin/rag-with-vdb-cos-client

local_resolve.o: local_resolve.c local_resolve.h
	$(CC) $(CFLAGS) -c local_resolve.c -o local_resolve.o

curl_helpers.o: curl_helpers.c curl_helpers.h
	$(CC) $(CFLAGS) -c curl_helpers.c -o curl_helpers.o

vector-db.o: vector-db.c vector-db.h
	$(CC) $(CFLAGS) -c vector-db.c -o vector-db.o

load-texts.o: load-texts.c load-texts.h
	$(CC) $(CFLAGS) -c load-texts.c -o load-texts.o

embedding-from-server.o: embedding-from-server.c embedding-from-server.h
	$(CC) $(CFLAGS) -c embedding-from-server.c -o embedding-from-server.o

bin/rag-with-vdb-cos-client: multirag.c vector-db.o vector-db.h \
                             local_resolve.o local_resolve.h \
                             curl_helpers.o curl_helpers.h \
                             embedding-from-server.o embedding-from-server.h
	$(CC) -D_RAG_WITH_COS_SERVER $(CFLAGS) multirag.c vector-db.o \
 local_resolve.o curl_helpers.o embedding-from-server.o \
 -o bin/rag-with-vdb-cos-client $(JSON) $(CURL) $(READLINE) $(MATH)

bin/rag-conversation: multirag.c \
                      local_resolve.o local_resolve.h \
                      curl_helpers.o curl_helpers.h
	$(CC) $(CFLAGS) multirag.c \
 local_resolve.o curl_helpers.o \
 -o bin/rag-conversation $(JSON) $(CURL) $(READLINE) $(MATH)

bin/embedding-from-server-cli: embedding-from-server.o \
                               embedding-from-server.h local_resolve.o \
                               local_resolve.h curl_helpers.o curl_helpers.h
	$(CC) $(CFLAGS) embedding-from-server-cli.c embedding-from-server.o \
 local_resolve.o curl_helpers.o \
 -o bin/embedding-from-server-cli $(JSON) $(CURL) $(MATH)

bin/build-vector-db-from-server: build-vector-db-from-server.c \
                                 embedding-from-server.o \
                                 embedding-from-server.h local_resolve.o \
                                 local_resolve.h curl_helpers.o curl_helpers.h \
                                 vector-db.h vector-db.o \
                                 load-texts.h load-texts.o
	$(CC) $(CFLAGS) build-vector-db-from-server.c embedding-from-server.o \
 local_resolve.o curl_helpers.o vector-db.o load-texts.o \
 -o bin/build-vector-db-from-server $(JSON) $(CURL) $(MATH)

clean:
	rm *.o bin/*

