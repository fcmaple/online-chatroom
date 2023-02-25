# Online-chatroom
the online chatroom with shell allows user to send messages with other users and execute shell command via a command-line interface.

## Shell
`npshell` support the following features
1. Execution of commands
  - `ls` `cat` `number` `removetag` `removetag0`
2. Ordinary pipe
3. Numbered pipe
4. File redirection

## Chatroom
there are three kinds of servers
1. A concurrent connection-oriented server. the server allow only one client to connect and excute npshell commands
2. A single-process concurrent server , allow several users to communicate with each other users and execute npshell commands.
3. A concurrent connection-oriented server. the server allow several users to connect , communicate with each other users and excute npshell commands with shared memory and FIFO

<img width="464" alt="截圖 2023-02-25 下午4 00 07" src="https://user-images.githubusercontent.com/80503114/221346012-5348f7fe-5c6b-4516-a66e-ca29d5debfb2.png">
<img width="547" alt="截圖 2023-02-25 下午4 00 17" src="https://user-images.githubusercontent.com/80503114/221346023-a661c64b-daf9-465e-bc9a-0e78e0c5c832.png">

In server1:
- Support all npshell command.
In server2 & server3 :
- Support all npshell command.
- Broadcast login/logout notification.
- Pipe between different users,and broadcast message whe the user pipe is used.
- built-in commands:
  - who : show information of all users
  - yell : broadcast the message.
  - tell : send the message to the specified user.
  - name : change the name of the user.

## How to use
1. build the server you want
```shell=
cd server[1,2,3]
make
```
2. Start
```shell=
./npshell [port]
```

3. Connect to server by `nc`
```shell=
nc [ip] [port]
```
## Implementation detail
### Shell
1. Behavior
  - Use `%` as the command line prompt.
  - The npshell parses the inputs and executes the commands.
  - The npshell terminate after receiving `exit`.
2. Built-in commands
  - `setenv [var] [val]`
    - change or add a environment variable.
  - `printenv [var]`
    - show the value of the environment variable.
  - `exit`
    - terminate the npshell
3. if there is a unkown command, npshell will print error message to STDERR with the following format : **Unkown command: [command]**.
4. Ordinary pipe & Number pipe
  - implement the ordinary pipe `[cmd1] | [cmd2]`, which means that the STDOUT of the `[cmd1]` will be the input of `[cmd2]`.
  - implement the number pipe `[cmd] |N` , `[cmd] !N`.
    - `|N` : STDOUT of `[cmd]` will pipe to the first command of the next N-th line.
    - `!N` : STDOUT & STDERR of `[cmd]` will pipe to the first command of the next N-th line.
5. File redirection
  - implement the standard output redirection `[cmd] > [file]`, which means that the STDOUT of `[cmd]` will be writed to `[file]`

### chatroom
1. Built-in Commands
  - `who`
    - show informations of all users.
  - `tell [user id] [message]`
    - tell the specified user message
  - `yell [message]`
    - broadcast the meaasge
  - `name [new name]`
    - change the user name to the new name
2. User pipe
  - implement the user pipe `[cmd] >[userid]` `[cmd] <[userid]`
    - `>[userid]`  the STDOUT of `[cmd]` will pipe to the `[userid]` 
    - `<[userid]`  receive the contents from `[userid]` 
