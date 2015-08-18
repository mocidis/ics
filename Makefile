.PHONY: all clean test
APP:=sample
#LOG:=logger
SRC_DIR:=.
SRCS:=ics-core.c object-pool.c queue.c
TESTS:=sample.c
#LOGS:=logger.c

CFLAGS:= -I$(SRC_DIR)/include $(shell pkg-config --cflags libpjproject)
LIBS:=$(shell pkg-config --libs libpjproject)

all: $(APP) $(LOG)

$(APP): $(SRCS:.c=.o) $(TESTS:.c=.o)
	gcc -o $@ $^ $(LIBS) 

#$(LOG): $(LOGS:.c=.o)
#	gcc -o $@ $^

$(TESTS:.c=.o): %.o: $(SRC_DIR)/test/%.c
	gcc -c -o $@ $< $(CFLAGS) 

#$(LOGS:.c=.o): %.o: $(SRC_DIR)/test/%.c
#	gcc -o $@ -c $< $(CFLAGS)

$(SRCS:.c=.o): %.o: $(SRC_DIR)/src/%.c
	gcc -c -o $@ $< $(CFLAGS)
clean:
	rm -fr *.o $(APP) $(LOG)
