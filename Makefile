OBJS=main.o cl_util.o
LDLIBS=-lOpenCL
CPPFLAGS=-DCL_TARGET_OPENCL_VERSION=300

all: clean main run

main: $(OBJS)
	${CC} -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -f main $(OBJS)

run:
	./main

