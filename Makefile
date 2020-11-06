all:
		g++ floating_point.cpp -lreadline

testcase1: 
			./a.out testcase1.txt

clean:
		rm -rf a.out