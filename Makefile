.PHONY: all clean test doc
include custom.mk
APP:=sample
SRC_DIR:=.
SRCS:=ics.c ics-event.c ics-command.c
TESTS:=sample.c

C_DIR:=../common
C_SRCS:=ansi-utils.c

Q_DIR:=../concurrent_queue
Q_SRCS:=queue.c

O_DIR:=../object-pool
O_SRCS:=object-pool.c

CFLAGS:=-DPJ_AUTOCONF=1 -O2 -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1 -fms-extensions
CFLAGS+=-I$(LIBS_DIR)/include
CFLAGS+=-I$(C_DIR)/include
CFLAGS+=-I$(SRC_DIR)/include -I$(Q_DIR)/include -I$(O_DIR)/include

all: $(APP) $(LOG)

doc: html latex
	doxygen
html:
	mkdir -p $@
latex:
	mkdir -p $@

$(APP): $(SRCS:.c=.o) $(TESTS:.c=.o) $(Q_SRCS:.c=.o) $(O_SRCS:.c=.o) $(C_SRCS:.c=.o)
	$(CROSS_TOOL) -o $@ $^ $(LIBS) $(CFLAGS)

$(TESTS:.c=.o): %.o: $(SRC_DIR)/test/%.c
	$(CROSS_TOOL) -c -o $@ $< $(CFLAGS)

$(SRCS:.c=.o): %.o: $(SRC_DIR)/src/%.c
	$(CROSS_TOOL) -c -o $@ $< $(CFLAGS)
$(O_SRCS:.c=.o): %.o: $(O_DIR)/src/%.c
	$(CROSS_TOOL) -c -o $@ $< $(CFLAGS)

$(Q_SRCS:.c=.o): %.o: $(Q_DIR)/src/%.c
	$(CROSS_TOOL) -c -o $@ $< $(CFLAGS)
$(C_SRCS:.c=.o): %.o: $(C_DIR)/src/%.c
	$(CROSS_TOOL) -c -o $@ $< $(CFLAGS)

clean:
	rm -fr *.o $(APP) $(LOG) html latex
