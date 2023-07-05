from os import open, wait, close, read, execvp, dup2, openpty, pipe, fork, O_RDWR, O_WRONLY, O_RDONLY, O_CLOEXEC, O_NONBLOCK, O_CREAT, WIFEXITED, O_NOCTTY
from stat import S_IWUSR, S_IRUSR
from sys import stdin, stdout, stderr
from socket import socketpair, AF_UNIX, IPPROTO_UNIX, SOCK_DGRAM, SOCK_CLOEXEC
from enum import Flag, auto, Enum
from time import time_ns

def get_shells(fcobj: type, bufsize=1024, lspath="/etc/shells"):
	startswith = b"/" + fcobj.__name__.lower().replace("shells", "").encode('ascii')
	shell_list_fdsc = open(lspath, O_RDONLY)
	all_read = bytearray([])
	read_buffer = bytearray(bufsize)
	while read_buffer:
		read_buffer = read(shell_list_fdsc, bufsize)
		all_read += read_buffer

	shell_list = all_read.split(b"\n")
	shell_list = [(name.decode(), path.decode()) for name, path in [(shell_path.split(b"/")[-1], shell_path) for shell_path in shell_list] if path.startswith(startswith)]

	return type("ShellList", (), {name.capitalize(): path for name, path in shell_list})()


def random_fname_str(minchr='0', maxchr='~', ext=".ipc"):
	minchr, maxchr = ord(minchr), ord(maxchr)
	name = bytearray([])
	seed = time_ns()
	while seed:
		name.append(max(minchr, min(seed & 0x7f, maxchr)))
		seed >>= 7
	return name.decode('ascii') + ext

@get_shells
class BinShells:
	pass


@get_shells
class UsrShells:
	pass

class Stdio(Enum):
	STDIN = stdin.fileno()
	STDOUT = stdout.fileno()
	STDERR = stderr.fileno()

class Mode(Flag):
	CloseOnExec = auto()
	NonBlock = auto()
	ReadOnly = auto()
	WriteOnly = auto()
	ReadAndWRite = auto()
	RandName = auto()

class Method(Enum):
	Pipe = auto()
	UnSock = auto()
	Fifo = auto()
	Pty = auto()

def open_file_descriptor(fn, *args, **kwargs):
	return fn(*args, **kwargs)