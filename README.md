# getjavaname

A C program that prints the class name of a Java class file to `stdout`.

## Usage

```sh
git clone https://github.com/michael-m-2983/getjavaname.git
cd getjavaname
sudo make install # This will install the program to /usr/local/bin

# ...

getjavaname Main.class
# -> my/package/Main
```

## References

- Ch. 4 of the JVM Spec: "The class file format" - <https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.4.10>
