
.PHONY:test

default: 
	gcc -o multisum T_main.c -lpthread

test: 
	gcc  -o multisum T_main.c -lpthread
	./auto_test.sh
test_lock:
	gcc -DLOCK  -o multisum T_main.c -lpthread
	./auto_test.sh
clean:  
	rm -rf ./test
	rm -f  ./output.txt ./multisum 

