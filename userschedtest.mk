USERSCHEDTEST_VERSION = 1.0
USERSCHEDTEST_SITE = ./package/userschedtest
USERSCHEDTEST_SITE_METHOD = local

define USERSCHEDTEST_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(@D)/userschedtest $(@D)/userschedtest.c
endef

define USERSCHEDTEST_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/userschedtest $(TARGET_DIR)/usr/bin/userschedtest
endef

define USERSCHEDTEST_USERS
    user1 -1 userschedtest -1 = /home/user1 /bin/sh - Test User 1
    user2 -1 userschedtest -1 = /home/user2 /bin/sh - Test User 2
    user3 -1 userschedtest -1 = /home/user3 /bin/sh - Test User 3   
endef

$(eval $(generic-package))