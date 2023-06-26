<!--
SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome

SPDX-License-Identifier: MIT-0
-->

# Concordo
Concordo is a CLI based, networkless and voiceless Discord clone.

## Instalation

### Requirements
- C++ compiler on your PATH
- CMake
- Git (optional)
- Doxygen (optional)
- Graphviz (optional, for documentation graphics)

### Compilation
```
$ git clone https://github.com/Stargazer3240/concordo.git
$ cd ./concordo
$ cmake -S . -B build/
$ cmake --build build/
```

## Usage
### Starting the program
After compiling the code, run `$ ./bin/concordo` on the root directory.

### Documentation
If you have installed Doxygen, run `$ doxygen` on the root directory. Then open
`./docs/html/index.html` with a modern browser.

### Commands
- `quit`
- `create-user EMAIL PASSWORD NAME` 
- `login EMAIL PASSWORD`
- `disconnect`
- `create-server NAME`
- `set-server-desc NAME DESCRIPTION`
- `list-servers`
- `remove-server NAME`
- `enter-server NAME`
- `leave-server`
- `list-participants`

> **Note:** the following arguments can't have spaces
> - User email 
> - User password
> - Server name

## Limitations
The program expects that the user input a valid command as documented. Failing to do so results in undefined behavior.

## Author
- Fabrício Moura Jácome

## License
This project is licensed under the [MIT License](https://spdx.org/licenses/MIT.html) (C++ source files) and [MIT-0 License](https://spdx.org/licenses/MIT-0) (documentation and configuration files) - see the LICENSE folder for details.
