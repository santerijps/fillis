# Fillis

`Fillis` is a file listener that runs a specified command whenever files are written to. Can be used like [nodemon](https://nodemon.io/) to automatically compile and run programs when files are saved.

Written in the C programming language and compiled with [Tiny C Compiler](https://bellard.org/tcc/), the executable is smaller than 10kb!

## Usage

```txt
Usage: fillis [command] [options?]

Options:
  -h, --help                                      Show app usage.
  -i, --ignore    [Comma separated values]        Exact file/directory names to ignore (not listen to).
                                                  The value of this options should be a comma separated list.
                                                  E.g. --ignore '.vscode,node_modules'
```