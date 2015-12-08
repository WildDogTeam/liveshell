include $(TOPDIR)/rules.mk
PKG_NAME:=liveshell
PKG_VERSION:=1.5
PKG_RELEASE:=1


PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/WildDogTeam/liveshell/blob/develop/openwrt/

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk


define Package/liveshell
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=liveshell - a good tool for live shell
	URL:=https://www.wilddog.com
endef

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install


define Package/liveshell/description
    If you can’t figure out what this program does, you’re probably
    brain-dead and need immediate medical attention.
endef


define Build/Configure
	$(call Build/Configure/Default,--with-endian=big)
endef

define Package/liveshell/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,liveshell))
