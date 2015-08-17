#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
int main()
{
    int fd;
    char * myfifo = "/tmp/myfifo3";

    fd = open(myfifo, O_WRONLY);
    write(fd, ttyname(1), strlen(ttyname(1)) +1);
    close(fd);

    return 0;
}
