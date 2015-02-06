# Afterdawn
Extremely high-speed racer, inspired by [Ballistics](https://www.youtube.com/watch?v=BXj5tuAtfGs) but with increased freedom in terms of navigation and track design, as well as modern ideas in game design. Currently serves as a testbed for Vespertine; there is no actual game as of present.

# Vespertine
Vespertine is a C++11 game engine that uses D3D11 and SDL2. Design goals include a deferred physically-based renderer, virtual reality support (including VR-specific optimizations), and more.

# Building
The engine is currently Windows-exclusive (as necessitated by the dependency on D3D11). This is for reasons of convenience; while this is unlikely to change, nothing is impossible. For similar reasons, the build system is a Visual Studio 2013 solution. In future, alternative build systems may be explored.

## Instructions
1. Pull down repository to a directory of your choice.
2. Build solution.
3. Set Afterdawn working directory to $(SolutionDir) in Visual Studio.
4. Copy SDL2.dll from `vendor/SDL2-2.0.3/lib/x86` to the location of the built executable (i.e. `Debug` or `Release` folder).
5. Hit F5 to run in Visual Studio.