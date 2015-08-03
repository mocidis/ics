.PHONY: all clean test
APP:=db-test
SRC_DIR:=.
SRCS:=db-utils.c
TESTS:=main.c

CFLAGS:=-I$(SRC_DIR)/include -I../out/include
LIBS:=-Lout/lib -lsqlite3

all: $(APP)

$(APP): $(SRCS:.c=.o) $(TESTS:.c=.o)
	gcc -o $@ $^ $(LIBS)
$(TESTS:.c=.o): %.o: $(SRC_DIR)/test/%.c
	gcc -c -o $@ $< $(CFLAGS)
$(SRCS:.c=.o): %.o: $(SRC_DIR)/src/%.c
	gcc -c -o $@ $< $(CFLAGS)
clean:
	rm -fr *.o $(APP)
