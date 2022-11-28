work: main.o aide_conf.o aide_data.o aide_itrc.o func_create.o func_adjust.o func_erase.o func_finish.o func_date.o func_list.o func_help.o
	g++ main.o aide_*.o func_*.o -o work

aide_conf.o: aide_conf.cpp
	g++ -c aide_conf.cpp -o aide_conf.o

aide_data.o: aide_data.cpp
	g++ -c aide_data.cpp -o aide_data.o

aide_itrc.o: aide_itrc.cpp
	g++ -c aide_itrc.cpp -o aide_itrc.o

func_create.o: func_create.cpp
	g++ -c func_create.cpp -o func_create.o

func_adjust.o: func_adjust.cpp
	g++ -c func_adjust.cpp -o func_adjust.o

func_erase.o: func_erase.cpp
	g++ -c func_erase.cpp -o func_erase.o

func_finish.o: func_finish.cpp
	g++ -c func_finish.cpp -o func_finish.o

func_date.o: func_date.cpp
	g++ -c func_date.cpp -o func_date.o

func_list.o: func_list.cpp
	g++ -c func_list.cpp -o func_list.o

func_help.o: func_help.cpp
	g++ -c func_help.cpp -o func_help.o

main.o: main.cpp
	g++ -c main.cpp -o main.o

clean:
	rm -rf work main.o aide_conf.o aide_data.o aide_itrc.o func_create.o func_adjust.o
