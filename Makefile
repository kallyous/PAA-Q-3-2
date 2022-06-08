CFLAGS=--std=c++23

clean:
	rm -f build/*

gen-test:
	g++ -c -o build/main.o src/main.cpp $(CFLAGS)
	g++  build/main.o -o bin/q_3_2 $(CFLAGS)
	bin/q_3_2 input/t4_tab_vit_01.txt input/t4_seq.txt
