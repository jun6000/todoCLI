# todoCLI
todoCLI - A simple todo app that runs in your terminal.

### Pre-requisites:
`git`, `make`, a C-compiler (`gcc`, `clang`, etc.)

## Installation (for linux)
  - [Clone](https://git-scm.com/docs/git-clone) this repo.
  - Run `cd todoCLI/`.
  - And then run `make` to create the `todo` executable.
  - Run the executable with `./todo`.

  ![image](https://user-images.githubusercontent.com/76509148/140901078-eebf8c9a-c4ef-4faf-8197-a287ff466d67.png)
  ![image](https://user-images.githubusercontent.com/76509148/140066085-7b7e597f-ea85-4806-b958-4b5605774585.png)

### Note
Since the `todo` executable doesn't have any dependencies, feel free to move it around to other paths of your choice as well. A new blank `tasks.txt` file will be created (while in the directory specified by the new path) if not already present in the new path to store tasks. You could even create a [symlink](https://stackoverflow.com/questions/1951742/how-can-i-symlink-a-file-in-linux) to the `todo` executable and the `tasks.txt` file so as to avoid moving the files.

### Special Mentions
Thank you very very much [Shan](https://github.com/xyzshantaram) for the original idea and support!!
