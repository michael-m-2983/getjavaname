getjavaname: getjavaname.c
	@gcc -o getjavaname getjavaname.c

install: getjavaname
	@cp ./getjavaname /usr/local/bin/

uninstall:
	@rm /usr/local/bin/getjavaname

clean:
	@rm ./getjavaname
