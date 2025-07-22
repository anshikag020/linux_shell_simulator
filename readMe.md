# Project name:  My shell  
**Designed for:** ubuntu 22.04.2 LTS  
**By:** Anshika Gupta  
CS22BTECH11007  

## Description
The project implements some basic commands of linux such as ls, ps, grep, mv and cp  

--------------------------------------------------------  

## How it works
Compile the files myshell.c, grep.c, ls.c, cp.c, mv.c, ps.c using the command:
```
gcc myshell.c ls.c ps.c cp.c mv.c grep.c -lreadline
```
(say we want to create executable file a.out)  

run the file using command:
```
./a.out
```
--------------------------------------------------------  

## Detailed description

Following commands can be implemented using the project:

### -> ls
Lists information about the files in the current directory.  
**Flags:**  
- `-l`: use a long listing format  
- `-a`: do not ignore entries starting with .  
- `-g`: like -l, but do not list owner  
- `-i`: print the index number of each file  
- `-m`: fill width with a comma separated list of entries  
- `-n`: like -l, but list numeric user and group IDs  
- `-1`: list one file per line  

### -> ps
lists information of the current processes  
**Flags:**  
- `-f`: Do full format listing. It only tells which columns must be displayed, not the processes  
- `-l`: Long format. It only tells which columns must be displayed, not the processes  
- other flags: `-fl`, `-lf`, `-l -f`  

### -> cp
copy files and directories  
**Flags:**  
- `-r`: copy directories recursively  
- `-i`: prompt before overwrite  
- `-b`: make a backup of each existing destination file  
- `-f`: if an existing destination file cannot be opened, remove it  and try  again  
- Other flags include: `-ib`, `-bi`, `-i -b`, `-if`, `-fi`, `-i -f`, `-ibf`, etc.  

### -> mv
move (rename) files  
**Flags:**  
- `-r`: move directories recursively  
- `-i`: prompt before overwrite  
- `-b`: make a backup of each existing destination file  
- `-f`: if an existing destination file cannot be opened, remove it  and try  again  
- Other flags include: `-ib`, `-bi`, `-i -b`, `-if`, `-fi`, `-i -f`, `-ibf`, etc.  

### -> grep
searches for patterns in a file  
**Flags:**  
- `-i`: Ignore  case  distinctions  in  patterns and input data, so that characters that differ only in case match each other.  
- `-n`: Prefix each line of output with the line  number  within its input file.  
- `-c`: Suppress  normal output. Instead display the number of occurrences of the pattern in the input file  
- Other flags include: `-i -n`, `-in`, `-ni`, `-ic`, `-ci`, `-i -c`, etc.  

You can also access the 'help' page by typing `help` in the command prompt.  
Type `exit` to close the terminal.
