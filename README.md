# getjavaname

A very simple C program that extracts the classname of a java class file.

## Usage

```sh
git clone https://github.com/michael-m-2983/getjavaname.git
cd getjavaname
make install # This will install the program to /usr/local/bin

# ...

getjavaname Main.class
# -> my/package/Main
```

## References

- Ch. 4 of the JVM Spec: "The class file format" - <https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.4.10>
