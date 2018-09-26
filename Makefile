all:
	g++ -c *.cpp *.h
	g++ *.o -o cheap8 -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm *.o
	rm *.gch
