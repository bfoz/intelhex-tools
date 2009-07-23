INCLUDE_DIRS= -Ilib/intelhex/include

all: inhx_cat

clean:
	rm -rf inhx_cat

inhx_cat: src/cat.cc lib/intelhex/src/intelhex.cc
	g++ -o $@ $^ ${INCLUDE_DIRS} -lstdc++
