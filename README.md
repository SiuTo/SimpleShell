# A Simplified Shell

## Features
* A prompt consists of the user name, the host name and the current working directory
* Allow the command to be executed in the background
* Redirection of I/O
* The build-in function `cd`
* A history feature: `history`, `!!`, `!NUM`
* `exit`

## Implementation

Get the host name, the user name and the current working directory.

* `gethostname()`
* `getuid()`
* `getpwuid()`
* `getcwd()`

Split the input command into arguments.
* `strcmp()`
* `strtok()`
* `atoi()`

Create, execute and wait a process.
* `fork()`
* `execvp()`
* `wait()`
* `waitpid()`
* `signal()`

Redirection of I/O.
* `freopen()`

Change directory.
* `chdir()`

## Examples

```
cd ..
ping localhost > output &
```

