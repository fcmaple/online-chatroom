all: npshell

.PONY: clean

G=g++
O=-o
C=-c
EXE=npshell
objects= server.o server_function.o sem_function.o npshell.o pipe.o command.o util.o

npshell : $(objects)
	$(G) $(O) $@ $^
server.o : server3.cpp server.h command.h Pipe.h
	$(G) $(C) $< $(O) $@
server_function.o : share_server.cpp server.h 
	$(G) $(C) $< $(O) $@ 
sem_function.o : server_semaphore.cpp server.h 
	$(G) $(C) $< $(O) $@ 
npshell.o : npshell3.cpp server.h command.h Pipe.h
	$(G) $(C) $< $(O) $@ 
pipe.o : Pipe.cpp Pipe.h
	$(G)  $(C) $< $(O)  $@ 
command.o : command.cpp command.h Pipe.h
	$(G) $(C) $< $(O)  $@ 
util.o : shell_util.cpp shell_util.h
	$(G) $(C) $< $(O) $@

clean:
	rm *.o npshell	
