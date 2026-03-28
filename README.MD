The following repo is a minimal implementation of a Redis Clone

Currently it support 3 out of the many Redis Commands and implements the following:
    a TCP Server
    A In Memory Key Value Store
    Minimal RESP2 PARSER
    Minimal RESP2 Executor 


This Repo is Written in C++

This is in no way a production level system, and was created as a learning Project.

There were many mistakes made in the Architecture of this Project - and if redone - I would change how commands are handled.

This repo is compatible with the redis-cli on supported commands.

