all: shell.out

.PONY: clean

G=g++
O=-o
C=-c
EXE=npshell
objects= server.o npshell.o pipe.o command.o util.o

shell.out : $(objects)
	$(G) $(O) $@ $^
server.o : server.cpp npshell.h
	$(G) $(C) $< $(O) $@
npshell.o : npshell.cpp command.h Pipe.h
	$(G) $(C) $< $(O) $@ 
pipe.o : Pipe.cpp Pipe.h
	$(G)  $(C) $< $(O)  $@ 
command.o : command.cpp command.h Pipe.h
	$(G) $(C) $< $(O)  $@ 
util.o : shell_util.cpp shell_util.h
	$(G) $(C) $< $(O) $@

clean:
	rm *.o *.out	
