# NEON

**Bright. Fast. Built to Last.**

Neon is a lightweight, high-performance in-memory key-value database written in C.

It speaks the Redis protocol, so existing Redis clients can connect to Neon with little or no modification. Internally, Neon is built around a compact bytecode VM and a purpose-built hash table, keeping the core small, predictable, and fast.

## Quick Start

Start Neon server:

```bashv
cd path/to/neon
./neon.exe
```

Then use the redis-cli:

```text
$ redis-cli -h 127.0.0.1 -p 6379
```

Examples:

```text
127.0.0.1:6379> SET name Neon
OK

127.0.0.1:6379> GET name
"Neon"

127.0.0.1:6379> INCR visits
(integer) 1

127.0.0.1:6379> INCR visits
(integer) 2

127.0.0.1:6379> DBSIZE
(integer) 2
```

Neon supports multiple databases and persistent snapshots:

```text
127.0.0.1:6379> SELECT 2
OK

127.0.0.1:6379[2]> SET language "C"
OK

127.0.0.1:6379[2]> SAVE
OK
```

## Why Neon?

Neon is built around a simple idea: a database doesn't need a massive codebase to be useful.

The core is deliberately compact, with a focus on:

- **Low overhead** — data lives directly in memory with a purpose-built hash table.
- **Predictable execution** — commands are compiled into a small bytecode instruction set and executed by a lightweight VM.
- **Redis compatibility** — use familiar Redis clients and tooling.
- **Persistence** — databases can be saved to and restored from Neon snapshots.
- **Multiple databases** — separate logical databases within the same server.
- **Native C implementation** — minimal dependencies and direct control over memory and execution.

## A Small Example

Neon can be used like a familiar key-value store:

```text
> SET user:1001 "test"
OK

> SET counter 41
OK

> INCR counter
(integer) 42

> EXISTS user:1001 counter
(integer) 2

> RENAME user:1001 current_user
OK

> TYPE counter
number

> KEYS
["counter", "current_user"]

> SAVE
OK

>EXIT
```

The interface stays simple while the implementation underneath remains intentionally close to the metal.

## Persistence

Neon supports explicit database snapshots.

```text
SET message "hello"
SAVE
```

The database can later be loaded from its snapshot, allowing in-memory data to survive a server restart.

## Architecture

Neon's execution path is built around a small VM:

```text
Client
  │
  ▼
RESP Protocol
  │
  ▼
Command Compiler
  │
  ▼
Bytecode VM
  │
  ▼
Database
  │
  ▼
Hash Table
```

Each client gets its own VM state while databases are managed independently by the server.

This separation keeps protocol handling, command execution, and data storage relatively independent and makes the core easier to reason about and extend.

## Building

Clone the repository and build using the project's build configuration:

```bash
git clone https://github.com/sathwikv2005/Neon
cd neon
./build.bat
```

Then start the server:

```bash
neon.exe
```
