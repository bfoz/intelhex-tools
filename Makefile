INCLUDE_DIRS= -Ilib/intelhex/include

all: inhx_cat inhx_edit inhx_head

clean:
	rm -rf inhx_cat

inhx_cat: src/cat.cc lib/intelhex/src/intelhex.cc
	g++ -o $@ $^ ${INCLUDE_DIRS} -lstdc++

inhx_edit: src/edit.cc lib/intelhex/src/intelhex.cc
	g++ -o $@ $^ ${INCLUDE_DIRS} -lstdc++

inhx_head: src/head.cc lib/intelhex/src/intelhex.cc
	g++ -o $@ $^ ${INCLUDE_DIRS} -lstdc++
