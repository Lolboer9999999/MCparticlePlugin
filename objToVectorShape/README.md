**This is the program you can use to convert .obj files to custom shapes for use in the particle plugin**
I uploaded the compiled version for windows, objToVectorShape.exe, but for people who prefer it it is also possible to compile the source code yourself.
To do this download the objToVectorShape.c and place it in a map somewhere.<br />
Then you need to open a commandprompt and navigate to the directory you saved the file in using "cd yourDirectory".
Then you can use gcc or a similar compiler to compile the code.<br />
The command when using gcc is 
```C
gcc objToVectorShape.c -o objToVectorShape.exe -lcomdlg32 -lgdi32
```
