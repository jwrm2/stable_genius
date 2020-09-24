#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

int main ()
{
    // Open the standard streams. The FILE pointers for the standard streams are
    // already open, with the POSIX file descriptors, so we need to make
    // syscalls to actually open them before we can do anything to them. They
    // should then be inherited by any fork'd or exec'd process.
    // stdin
    open("/dev/tty", O_RDONLY, 0);
    // stdout
    open("/dev/tty", O_WRONLY, 0);
    // stderr
    open("/dev/tty", O_WRONLY, 0);

    // Use syscall directly.
    write(STDOUT_FILENO, "Direct syscall\n", 15);

    // Use C-style.
    std::fwrite("C-style\n", sizeof(char), 8, std::stdout);
    std::fflush(std::stdout);

    // Use C++ style.
    std::cout << "C++ style" << std::endl;

    // Loop forever, testing only.
    while (true) ;
}
