<h2 align="center">C scaffolding structure</h2>
<p align="center">
  Scaffolding structure for C projects and custom Makefile
  <br>
  <strong align="center">Use this template for your own projects 😊</strong>
</p>

---

### Requirements 🧾

- `gcc` compiler or another compiler for C
- `make` library to run Makefile
- `Git Bash` (you need some linux commnads so you cant use this with cmd or other terminal)
- 🧠

### Installation and first run 🐱

- clone repozitory `git clone https://github.com/FrantisekSpunda/C-scaffolding.git`
- got to project folder `cd c-scaffolding`
- run `make init` to create config.cfg file
- edit `config.cfg` as needed
- run `make all` (or `make build` and `make run`)

### Usage 🐔

Using makefile you can run commands for compilation, generating headers and new modules etc.

| Command             | Description                                                                                                                                                                   |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `help` / `list`     | Show all available commands you can use and their description.                                                                                                                |
| `init`              | Create project configuratin file.                                                                                                                                             |
| `build` / `compile` | Compile C program in src.                                                                                                                                                     |
| `run`               | Run your compiled C program.                                                                                                                                                  |
| `all`               | Compile and run your C program.                                                                                                                                               |
| `clear` / `clean`   | Remove build folder.                                                                                                                                                          |
| `header`            | Update or create header in all `.c` files. In header is your name, copyright and last update of file.                                                                         |
| `module`            | Create new module for C. With this command you will create C module with custom name. In files there will be generated required content that is usualy in all C libraries 👌. |

### Map specifiaction 🗺️

**111b --> rectagnle △ or ▽**

> **first** 1 - wall on **left** (0 means, there is no wall)
> **second** 1 - wall on **right** (0 means, there is no wall)
> **third** 1 - wall on **top/bottom** (0 means, there is no wall)

Example:
Rectangle has wall on left and rigth: 011b = **0**\*2^**2** + **1**\*2^**1** + **1**\*2^**0** = 0 + 2 + 1 = 3
