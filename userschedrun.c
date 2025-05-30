#include <stdlib.h>

int main() {
    system("su - root  '/usr/bin/userschedtest 7 &'");
    system("su - user1 '/usr/bin/userschedtest 7 &'");
    system("su - user2 '/usr/bin/userschedtest 7 &'");
    system("su - user3 '/usr/bin/userschedtest 7 &'");
    return 0;
}
