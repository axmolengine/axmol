# JavaScript Debugger

The JavaScript debugger allows the developer to connect to a
JavaScript application running on either a device or simulator, set
breakpoints, execute code step by step and inspect the running
application's environment.

When enabled the debugger will listen for incoming commands on a TCP
socket. Commands are issued as plaintext with responses available
either in human-readable plaintext or in JSON.

## Setup

After setting up the JavaScript bindings project make sure to call
enableDebugger() before running the scene.

The first thing you need to do if you want to debug, is to add the debugger.js file to your project,
then call enableDebugger() before your main entrypoint.

```c++
bool AppDelegate::applicationDidFinishLaunching()
{
	...
    ScriptingCore::getInstance()->enableDebugger();
    ScriptingCore::getInstance()->runScript("main.js");
}
```

## A debugging session

Once you start your game with a debugger session to be attached, the debugger will stop execution
every time it loads a script, and it will also open a TCP socket on port 5086 (you can change that
port by setting JSB_DEBUGGER_PORT) that will remain listening until your app dies.

The basic commands supported by the debugger are the following:

* `break file.js:NN`: set a breakpoint on file.js at line NN. **CURRENTLY NOT WORKING PROPERLY**
* `step`: if you are in a breakpoint (or if you got into the debugger due to a `debugger` statement
  in your code), you can step into the next line.
* `continue`: resume execution.
* `eval <valid js code>`: will evaluate the passed string as javascript code, in the current
  execution frame. Note that with this command you can actually modify the content of variables if
  you wish. The debugger client will print the result of the evaluation, so you can also use this
  to inspect objects. If the result is not an object, it will return the string representation,
  otherwise, it will iterate over all the public keys of the object and print the result. The
  inspection is non-recursive.
* `line`: prints the line of code where the debugger is currently at (the line of the current
  execution offset).
* `bt`: prints the backtrace (how did we get here).
* `help` : prints all available commands
