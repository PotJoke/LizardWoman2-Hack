# Description

DLL file prepeared to be injeceted with something like process Hacker 2 into HumansAreNotThatAgainstLizardwomen2
Now it's unlocks every scene and enables StoryMode

# How is that works

Essentially, a minhook intercepts the program's address and uses the specified offset in hexadecimal format (if you decompile il2cpp, you'll usually get decimal, so be sure to convert it), then waits for an event. When this happens, the minhook receives its parameters, stops execution, and immediately starts a new one with the same parameters but a different output. I believe this is called overriding.

# How to build

You'll need Visual Studio
Install git, then vcpkg and install minhook using it with this command:
```
.\vcpkg install minhook:x64-windows-static
```
