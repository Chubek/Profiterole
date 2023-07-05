from enum import Enum, Flag, auto
from time import time_ns
from tempfile import gettempdir
from sys import stdin, stdout, stderr, exit, exc_info
from os import open, wait, close, read, write, dup2, dup, stat, unlink
from os import pipe2, openpty, fork, mknod
from os import execl, execlp, execlpe, execv, execvp, execvpe
from os import O_RDWR, O_WRONLY, O_RDONLY, O_TMPFILE, O_CLOEXEC, O_NONBLOCK, O_DIRECT, O_CREAT, O_NOCTTY
from os import WIFEXITED, WEXITSTATUS, WIFSTOPPED, WSTOPSIG, WIFCONTINUED
from stat import S_IWUSR, S_IXUSR, S_IRUSR, S_IFREG, S_IFIFO, S_IFSOCK, IFBLK, S_IFCHR
from socket import socketpair, AF_UNIX, IPPROTO_UDP, SOCK_DGRAM


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


def random_path(minchr='0', maxchr='~', ext=".ipc"):
	minchr, maxchr = ord(minchr), ord(maxchr)
	name = bytearray([])
	seed = time_ns()
	while seed:
		name.append(max(minchr, min(seed & 0x7f, maxchr)))
		seed >>= 7
	return gettempdir() + "/" + name.decode('ascii') + "." + ext


def call_function_perror(fn, *args, **kwargs):
	exec_result = None
	try:
		exec_result = fn(*args, **kwargs)
	except exc_info()[0]:
		raise exc_info()[1].with_traceback(exc_info()[2])
	finally:
		return exec_result

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

class OpenMode(Flag):
	CloseOnExec = O_CLOEXEC
	NonBlock = O_NONBLOCK
	ReadOnly = O_RDONLy
	WriteOnly = O_WRONLY
	ReadAndWRite = O_RDWR
	TempFile = O_TEMPFILE
	RandName = auto()

class OpenStat(Flag):
	CharFile = S_IFCHR
	FifoFile = S_IFIFO
	SockFile = S_ISOCK
	BlockFile = S_IFBLK
	RegularFile = S_IFREG
	ReadUser = S_IRUSR
	WriteUser = S_IWUSR
	ExecUser = S_IXUSR

class IPCMethod(Enum):
	Pipe = auto()
	UnSock = auto()
	Node = auto()
	Pty = auto()


class ExecMode(Flag):
	VarArgs = auto()
	PathSensitive = auto()
	HasEnv = auto()

def open_comm_channel(ipc_method: IPCMethod, open_mode_flags: OpenMode, open_stat_flags: StatMode, fpath=None):
	if ipc_method is IPCMethod.Pipe:
		return call_function_perror(pipe2, open_mode_flags, stat_mode_flags)
	elif ipc_method is IPCMethod.Node:
		return call_function_perror(mknod, open_mode_flags, state_mode_flags)
	elif ipc_method is IPCMethod.Pty:
		return call_function_perror(openpty)
	elif ipc_method is IPCMethod.UnSock:
		return call_function_perror(socketpair, AF_UNIX, SOCK_DGRAM, IPPROTO_UDP)
	else:
		raise Exception("Wrong IPC method specified")

		