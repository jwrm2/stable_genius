#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main ()
{
    // Open the standard streams. The FILE pointers for the stanrds streams are
    // already open, with the POSIX file descriptors, so we need to make
    // syscalls to actually open them before we can do anything to them. They
    // should then be inherited by any fork'd or exec'd process.
    std::string tty {"/dev/tty"};
    // stdout
    std::open(tty.c_str(), O_WRONLY, 0);
    // stdin
    std::open(tty.c_str(), O_RDONLY, 0);
    // stderr
    std::open(tty.c_str(), O_WRONLY, 0);

    // Check that cout works.
    cout << "User space initialisation complete\n";

    // Loop forever, testing only.
    while (true) ;
}
