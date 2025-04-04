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
    $(INSTALL_DATA) -D -m 0644 $(@D)/users_table.txt $(TARGET_DIR)/etc/users_table
endef

USERSCHEDTEST_POST_INSTALL_TARGET_HOOKS += USERSCHEDTEST_USERS

$(eval $(generic-package))