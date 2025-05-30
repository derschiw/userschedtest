#include <stdlib.h>

int main() {
    system("su -u root  userschedtest 7 &");
    system("su -u user1 userschedtest 7 &");
    system("su -u user2 userschedtest 7 &");
    system("su -u user3 userschedtest 7 &");
    return 0;
}
