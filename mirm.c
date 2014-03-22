#include <unistd.h>

int mirm(char* archivo)
{
unlink(archivo);
return 0;
}
