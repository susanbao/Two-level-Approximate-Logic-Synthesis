#!/bin/bash

#test all .pla document for experiment
g++ -o mult_021.exe mult_021.cpp
g++ -o check.exe check.cpp
g++ -o compute_test.exe compute_test.cpp
g++ -o compute_txt.exe compute_txt.cpp
g++ -o cycle.exe cycle.cpp

rm *.best

for file in `ls *.pla`
do
	file=${file%%.*};
	echo $file;
	cat ${file}.pla >a.in;
	./espresso_ubuntu_amd64.exe <a.in >b_0000.txt 2>r.err
	./espresso_ubuntu_amd64.exe <a.in >result_first.ans 2>r.err
	date +"%H:%M:%S:%N"
	./cycle.exe	
	./compute_test.exe
	./check.exe
	date +"%H:%M:%S:%N"
	rm *.txt
done;


