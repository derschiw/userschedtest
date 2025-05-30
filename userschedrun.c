#include <stdlib.h>

int main() {
    system("su - root  -c '/usr/bin/userschedtest 7 &'");
    system("su - user1 -c '/usr/bin/userschedtest 7 &'");
    system("su - user2 -c '/usr/bin/userschedtest 7 &'");
    system("su - user3 -c '/usr/bin/userschedtest 7 &'");
    return 0;
}
