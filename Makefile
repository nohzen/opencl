OBJS_MAIN=main.o cl_util.o
OBJS_OC=offline_compiler.o cl_util.o
LDLIBS=-lOpenCL
CPPFLAGS=-DCL_TARGET_OPENCL_VERSION=300

all: clean main run

main: $(OBJS_MAIN)
	${CC} -o $@ $(OBJS_MAIN) $(LDLIBS)

offline_compiler: $(OBJS_OC)
	${CC} -o $@ $(OBJS_OC) $(LDLIBS)

clean:
	rm -f main $(OBJS_MAIN) offline_compiler $(OBJS_OC)

run:
	./main

