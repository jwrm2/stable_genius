#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

extern "C" void hang(void*);

int main ()
{
    // Open the standard streams. The FILE pointers for the stanrds streams are
    // already open, with the POSIX file descriptors, so we need to make
    // syscalls to actually open them before we can do anything to them. They
    // should then be inherited by any fork'd or exec'd process.
    std::string tty {"/dev/tty"};
    // stdout
    open(tty.c_str(), O_RDONLY, 0);
    // stdin
    open(tty.c_str(), O_WRONLY, 0);
    // stderr
    open(tty.c_str(), O_WRONLY, 0);

    // Use syscall directly.
    write(STDOUT_FILENO, "Direct syscall\n", 15);

    hang(std::stdout);

    // Use C-style.
    std::fwrite("C-style\n", sizeof(char), 8, std::stdout);

    // Use C++ style.
    std::cout << "C++ style\n";

    // Loop forever, testing only.
    while (true) ;
}
