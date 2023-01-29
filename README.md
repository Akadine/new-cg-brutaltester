# new-cg-brutaltester

new-cg-brutal-tester is a C++ 20 local tool  based on https://github.com/dreignier/cg-brutaltester to simulate the [CodinGame](https://www.codingame.com/) multiplayer arena. How does it work? Very simple:

    new-cg-brutal-tester.exe -r "java -jar cg-referee-ghost-in-the-cell.jar" -p1 "./myCode.exe" -p2 "php myCode.php" -t 2 -n 100 -d logs

At the end of the command, you will get something like this:

    INFO *** End of games ***
    +----------+----------+----------+
    | Results  | Player 1 | Player 2 |
    +----------+----------+----------+
    | Player 1 |          | 7,00%    |
    +----------+----------+----------+
    | Player 2 | 52,70%   |          |
    +----------+----------+----------+

How to read it: Player 1 won 7.00% of the time against Player 2. Player 2 won 52.70% of the time against Player 1. The total is not 100% because you have some draws.

Unfortunately, this does not work for GO executables. I have minimized this down to my process class and a GO HelloWorld for testing, just cannot read from GO. Every other language seems to work... Hopefully someone from the comunity can fix it. 

## How to build from sources

1. On WINDOWS only (for now) Install the latest MS Visual Studio.
2. Load project in ./source/
3. Right click on project in Solution Explorer, select Properties. Under General, Use the drop down for C++ Language Standard and set to ISO C++ 20 Standard.
4. In VS main menu select Build > build, or hit F7.

Now you should get (or compile from sources) referee for specific game and make it work together with brutaltester as stated above.

## Command line arguments:

### Referee `-r <string>` (Mandatory)

This is the command line to start the referee process. The referee must respect the cg-brutaltester protocol. See [How do I make my own referee?](#how-do-i-make-my-own-referee) for more information.
In our example, we use a runnable Jar file as the referee.

### Player X `-pX <string>` (Mandatory)

Each `-pX` argument is the command line to start a player process. You can give a maximum of 4 players. But don't forget the some referees will ignore some players (for example, Ghost in the Cell only uses 2 players).
In the example, the first player is a simple executable file and the second player is a php file.

### Threads `-t <int>` (Optional; Default is 1)

The number of threads to spawn for the games. If you give 2, it means that you will have 2 games playing at the same time. It's useless to spawn too many threads. If you have a 4-core CPU, you should not try to spawn more than 3 threads.

### Number of games `-n <int>` (Optional; Default is 1)

The number of games to play. The given example will play 100 games.

### Logs directory `-l <string>` (Optional)

You may need the logs of the file. If you specify a directory, all games will be saved in the given directory. The files contain standard and error outputs of all processes (referee and players).

### Swap player positions `-s` (Optional)

There are some games (such as Tron), where one player has a disadvantage from the beginning on because of an asymmetric map. In this case you can repeat the game on the same map, but with positions changed. For more than two players this will perform a simple rotation and not test all permutations (resulting in 4 matches on the same map for 4 players instead of 24).
NOTE: not all referees support this flag, as they have to allow setting a seed.

### Initial Seed `-i <int>` (Optional)

It allows to use the same seeds in different runs. You can't select individual seeds, but only the starting seed for the Random Number Generator. It's useful to have repeteable tests.

### Old mode `-o` (Optional)

Since Botters of the Galaxy and Ultimate Tic Tac Toe, Codingame changed a lot the way of creating a referee. Because of that, all games created before Botters of the Galaxy and Ultimate Tic Tac Toe use the "old way". If you want to use an old referee, you have to use this flag. 

### Log Level `-l`

My log class has 5 levels, 0-5, VERBOSE, INFO, WARN, ERR, FATAL. This is the lowest level to log, if set to 2 or WARN, then you get all WARN, ERR, FATAL level logs. 

### Help `-h`

Display this help :

    new-cg-brutal-tester.exe
        -h      Displays this help.
        -r      Required. Referee command line.
        -p1     Required. Player 1 command line.
        -p2     Required. Player 2 command line.
        -P3     Player 3 command line.
        -P4     Player 4 command line.
        -v      Visualizer command line. Not implemented, hard sets threads to 1.
        -t      Number of threads to spawn for the games. Default 1.
        -n      Number of games to play. Default 1.
        -s      Swap player positions.
        -i      Initial seed. For repetable tests
        -d      Log directory.
        -l      Log level. 0 verbose, 1 info, 2 warning (default), 3 error, and 4 fatal. Only logs that level and higher. ex. if set to 3, only error and fatal levels logs are created.
        -o      Old mode

## How do I make my own referee?

### WARNING !

Since Botters of the Galaxy and Ultimate Tic Tac Toe, CodinGame change a lot the way of creating a referee. Because of that, cg-brutaltester had to adapt.

If you want to use or create a referee for a game created before Botters of the Galaxy or Ultimate Tic Tac Toe, use this [wiki page](https://github.com/dreignier/cg-brutaltester/wiki/How-to-create-an-old-referee).

Your referee must be runnable with a command line (or you won't be able to give it to cg-brutaltester) and you have to use the standard input and output streams. The referee can output on the error stream for debug purposes or real errors. It will be stored in the log file of the game. cg-brutaltester is a very naive arena, and the referee must tell it how to work.

### All steps

As an example, you can check my [Ultime Tic Tac Toe brutaltester referee](https://github.com/dreignier/game-ultimate-tictactoe)
To create your own referee for cg-brutaltester, there's X steps to follow:

 * Fork the referee repository
 * Modify the [`pom.xml`](https://github.com/dreignier/game-ultimate-tictactoe/blob/master/pom.xml) file. 
 * Add the [`CommandLineInterface`](https://github.com/dreignier/game-ultimate-tictactoe/blob/master/src/main/java/com/codingame/gameengine/runner/CommandLineInterface.java) class in the package `com.codingame.gameengine.runner`
 
I'm currently thinking of a way to automate the process. Copy/pasting `CommandLineInterface` is good enough for now, but this is clearly not the best thing to do.

## Incoming features

This is not an official roadmap at all.

 * Generate an html file for the results (with graphics!)
 * Better handling of crashing players' code
 * Handle timeouts
 * Conquer the world

# List of compatible referees

If you have a bug or a problem with one of these referees, create an issue of the github project of the referee, not on cg-brutaltester project. This may not be a full list of available referees for cg-brutaltester. If you want to add a referee to this list, just make a pull request.


 * Fall challenge 2022:
   * https://github.com/bastien-35/FallChallenge2022-KeepOffTheGrass
 * Spring challenge 2022:
   * https://github.com/johnpage-agixis/SpringChallenge2022
 * Spring challenge 2021:
   * https://github.com/LSmith-Zenoscave/SpringChallenge2021 (Java)
 * Spring challenge 2020:
   * https://github.com/Akarachudra/brutaltester-spring-challenge-2020 (Java)
 * Unleash The Geek:
   * https://github.com/fala13/UnleashTheGeek (Java)
 * Legends Of Code And Magic:
   * https://github.com/fala13/LegendsOfCodeAndMagic (Java)
 * Code of Kutulu:
   * https://github.com/fala13/code-of-kutulu (Java)
 * Ultimate Tic Tac Toe
   * https://github.com/dreignier/game-ultimate-tictactoe (Java)
 * Botters of the Galaxy:
   * https://github.com/Illedan/BOTG-Refree (Java) 
 * Ghost in the cell:
   * https://github.com/dreignier/cg-referee-ghost-in-the-cell (Java) (use it with `-o`)
 * Coders of the Caribbean:
   * https://github.com/Coac/brutaltester-referee-coders-of-the-caribbean (Java) (use it with `-o`)
   * https://github.com/kevinsandow/cg-referee-coders-of-the-caribbean (Java) (use it with `-o`)
 * Code 4 life:
   * https://github.com/kevinsandow/cg-referee-code4life (Java) (use it with `-o`)
 * Wondev Woman:
   * https://github.com/kevinsandow/cg-referee-wondev-woman (Java) (use it with `-o`)
 * Mean Max:
   * https://github.com/kevinsandow/cg-referee-mean-max (Java) (use it with `-o`)
 * Coders strike back:
   * https://github.com/robostac/coders-strike-back-referee (Go) (use it with `-o`) 
   * this ^ is the only one I cannot get working with the new brutal tester. This was the one I really wanted to work!
 * Back to the Code:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * Codebusters:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * Game of Drones:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * Hypersonic:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * Smash the Code:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * The Great Escape:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
 * Tron:
   * https://github.com/eulerscheZahl/RefereeCollection (C#) (use it with `-o`)
