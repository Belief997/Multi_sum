
.PHONY:test

default: 
	gcc -o multisum P_main.c 

test: 
	gcc  -o multisum P_main.c 
	./auto_test.sh
pipe:
	gcc -DPIPE  -o multisum T_main.c -lpthread

test_pipe:
	gcc -DPIPE  -o multisum T_main.c -lpthread
	./auto_test.sh
clean:  
	rm -rf ./test
	rm -f ./output.txt ./multisum 

