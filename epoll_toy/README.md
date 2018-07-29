# Configuration

None whatsoever, all dependencies provided in the package.

# Building the project

Use the Makefile like this:

make all

(`make clean` might be useful.)
This should give you 3 executables in the top directory:

1. `root_client.app`
   - Is a client with super-privileges, it can be used to invoke arbitrary commands on any client or tracker. Plays the role of a debugging tool and also in giving demonstrations.
2. `client.app`
   - Plays the dual role of leecher, seeder in the torrent network. It also interfaces between the user and the file-system.
3. `server.app`
   - Plays the role of a [tracker]() in the torrent network.

# Running the demonstration

## Starting everything

First copy the `client.app` executable to the directories: demo/{alpha,beta,gamma,delta} using

cp client.app demo/alpha;cp client.app demo/beta;cp client.app demo/gamma;cp client.app demo/delta;

Second, due to our bad linux skills, we need you to open a new terminal for each of the following commands:<br>
**(Make sure the new terminals are all at the top-directory before running the following)**

./tracker_app 5000

cd alpha;./client_app 5001 5000 alpha 0.1

cd  beta;./client_app 5002 5000 beta 0.1

cd gamma;./client_app 5003 5000 gamma 0.1

cd delta;./client_app 5004 5000 delta 0.1

./root_client localhost

Notice that a JSON file (`demo_commands.json`) is provided. It contains demo commands for the `root_client` which you can quickly copy-paste in `root_client`'s terminal.
The comments explain what you can expect.

# Disclaimer

You will not be able to send/recv files. The download functionality is almost entirely missing.

# License

The project is licensed under MIT license.
A substantial part is provided by Neils Holmann's JSON library.

Copyright © 2017 Ananya Bahadur, Anugrah Gari
Copyright © 2013-2017 Niels Lohmann

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


