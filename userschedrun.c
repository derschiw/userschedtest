#include <stdlib.h>

int main() {
    system("su - root  'userschedtest 7 &'");
    system("su - user1 'userschedtest 7 &'");
    system("su - user2 'userschedtest 7 &'");
    system("su - user3 'userschedtest 7 &'");
    return 0;
}
