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
- `create-server SERVERNAME`
- `set-server-desc SERVERNAME DESCRIPTION`
- `set-server-invite-code SERVERNAME INVITECODE`
- `list-servers`
- `remove-server SERVERNAME`
- `enter-server SERVERNAME`
- `leave-server`
- `list-participants`

> **Notes**
> - The following arguments can't have spaces:
>   - User email
>   - User password
>   - Server name
>   - Server invite code
> - `set-server-invite-code` can be used without passing an invite code, making the server public.

## Limitations
The program expects that the user input a valid command as documented. Failing to do so results in undefined behavior. Also, the program can only run some commands
at specific stages (by design), but it doesn't output an useful diagnose to guide the user in what state they can used, besides the `create-user`, `login` and `disconnect` commands.

## Author
- Fabrício Moura Jácome

## License
This project is licensed under the [MIT License](https://spdx.org/licenses/MIT.html) (C++ source files) and [MIT-0 License](https://spdx.org/licenses/MIT-0) (documentation and configuration files) - see the LICENSE folder for details.
