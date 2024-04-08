# axmol-cmdline


## Vision of axmol-cmdline


A command line tool that lets you create, run, publish, debug, etc… your game. It is the swiss-army knife for axmol.

Examples:

```
# starts a new project called "My Game" for multi-platform

$ axmol new MyGame -l cpp -p org.axmol.mygame

$ cd MyGame

# Will deploy the project to device and run it
$ axmol run -p android


```

# Devel Info

## Internals

`axmol.ps1` is an script whose only responsability is to call its plugins.
`axmol.bat` will invoke `axmol.ps1` on windows
`axmol` will invoke `axmol.ps1` on Mac OS X and linux

To get a list of all the registered plugins:

```
$ axmol -h
```

To run the "new" plugin:

```
$ axmol new
``` 
