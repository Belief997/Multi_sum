#!/bin/bash
N1=1
N2=2
N3=4
N4=10
N5=20
N6=40
N7=80
N8=99

M0=4294967295
M1=100
M2=10000
M3=1000000

rm -rf ./test
mkdir ./test

i=0
for i in $(seq 1 1 8)
do
	N=N${i}
	eval echo -e "N=\$$N" >./input.txt
	echo -e "M=$M1" >>./input.txt
	cat input.txt
	./multisum
	cat output.txt
	mv input.txt ./test/input1-$i.txt
	mv output.txt ./test/output1-$i.txt
	echo
	#rm -f ./input.txt
done


i=0
for i in $(seq 1 1 8)
do
	N=N${i}
	eval echo -e "N=\$$N" >./input.txt
	echo -e "M=$M2" >>./input.txt
	cat input.txt
	./multisum
	cat output.txt
	mv input.txt ./test/input2-$i.txt
	mv output.txt ./test/output2-$i.txt
	echo
	#rm -f ./input.txt
done


i=0
for i in $(seq 1 1 8)
do
	N=N${i}
	eval echo -e "N=\$$N" >./input.txt
	echo -e "M=$M3" >>./input.txt
	cat input.txt
	./multisum
	cat output.txt
	mv input.txt ./test/input3-$i.txt
	mv output.txt ./test/output3-$i.txt
	echo
	#rm -f ./input.txt
done

i=0
for i in $(seq 1 1 8)
do
	N=N${i}
	eval echo -e "N=\$$N" >./input.txt
	echo -e "M=$M0" >>./input.txt
	cat input.txt
	./multisum
	cat output.txt
	mv input.txt ./test/input4-$i.txt
	mv output.txt ./test/output4-$i.txt
	echo	
	#rm -f ./input.txt
done

echo "auto test done, all *.txt are mv to ./test"


