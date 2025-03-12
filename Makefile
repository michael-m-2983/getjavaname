getjavaname: getjavaname.c
	@gcc -o getjavaname getjavaname.c

install: getjavaname
	@cp ./getjavaname /usr/local/bin/
