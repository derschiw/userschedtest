USERSCHEDTEST_VERSION = 1.0
USERSCHEDTEST_SITE = ./package/userschedtest
USERSCHEDTEST_SITE_METHOD = local

define USERSCHEDTEST_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(@D)/userschedtest $(@D)/userschedtest.c
endef

define USERSCHEDTEST_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/userschedtest $(TARGET_DIR)/usr/bin/userschedtest
endef

$(eval $(generic-package))