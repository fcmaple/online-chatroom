all: npshell

.PONY: clean

G=g++
O=-o
C=-c
EXE=npshell
objects= server.o server_function.o npshell.o pipe.o command.o util.o

npshell : $(objects)
	$(G) $(O) $@ $^
server.o : server2.cpp server.h command.h Pipe.h
	$(G) $(C) $< $(O) $@
server_function.o : server_func.cpp server.h command.h Pipe.h
	$(G) $(C) $< $(O) $@ 
npshell.o : npshell2.cpp server.h command.h Pipe.h
	$(G) $(C) $< $(O) $@ 
pipe.o : Pipe.cpp Pipe.h
	$(G)  $(C) $< $(O)  $@ 
command.o : command.cpp command.h Pipe.h
	$(G) $(C) $< $(O)  $@ 
util.o : shell_util.cpp shell_util.h
	$(G) $(C) $< $(O) $@

clean:
	rm *.o npshell	
