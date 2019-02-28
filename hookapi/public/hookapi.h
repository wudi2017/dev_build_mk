#include <unistd.h>

extern "C"
{
	void exit (int __status) __THROW __attribute__ ((__noreturn__));
	void _exit (int __status);
	void _Exit (int __status) __THROW __attribute__ ((__noreturn__));
	void pthread_exit (void *__retval) __attribute__ ((__noreturn__));
	//int strcmp(const char*, const char*) throw ();
}
