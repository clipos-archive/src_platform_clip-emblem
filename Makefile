# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright © 2014-2018 ANSSI. All Rights Reserved.
CC = g++
# LDLIBS = -lclip
# LDLIBS_CLIENT = -lfdp-client $(LDLIBS)

CFLAGS += -fPIC -Wall -Wextra -Werror
LDFLAGS_SHARED = -shared $(LDFLAGS)
LDFLAGS_BIN = -pie -L. $(LDFLAGS)

PREFIX ?= /usr

SOBASE = libEmblemConfiguration.so
SONAME = $(SOBASE).1
SOVERSION = $(SOBASE).1.0.0
BIN_FILES = setorigin
LIB_FILES = $(SOVERSION)
LIB_LINKS = $(SOBASE) $(SONAME)
INC_FILES = emblem-configuration.h
KDE_SERVICES_DIR = /share/kde4/services
KDE_ICONS_DIR = /share/icons/oxygen
ICONS_SIZE_LIST = 8x8 16x16  22x22 32x32 48x48 64x64
ETC_DIR = /etc

.PHONY: all clean mrproper install

all: $(SOBASE) $(BIN_FILES)

clean:
	rm emblem-configuration.o setorigin.o setorigin libEmblemConfiguration.so* || true

mrproper: clean
	rm $(BIN_FILES) $(LIB_FILES) $(LIB_LINKS) || true

install: $(SOBASE) $(BIN_FILES)
	for bin in $(BIN_FILES); do \
		install -D $${bin} $(DESTDIR)/$(PREFIX)/bin/$${bin}; \
	done
	for lib in $(LIB_FILES); do \
		install -D $${lib} $(DESTDIR)/$(PREFIX)/lib/$${lib}; \
	done
	for slib in $(LIB_LINKS); do \
		cp -R $${slib} $(DESTDIR)/$(PREFIX)/lib/$${slib}; \
	done
	for inc in $(INC_FILES); do \
		install -m 0644 -D $${inc} $(DESTDIR)/$(PREFIX)/include/$${inc}; \
	done
	install -D ./emblem.desktop  $(DESTDIR)/$(PREFIX)/$(KDE_SERVICES_DIR)/emblem.desktop
	install -D extended_attributes_configuration.conf $(DESTDIR)/$(PREFIX)/$(ETC_DIR)/extended_attributes_configuration.conf
	for dir_partiel in $(ICONS_SIZE_LIST); do \
		install -d $(DESTDIR)/$(PREFIX)/$(KDE_ICONS_DIR)/$${dir_partiel}/emblems; \
	done
	for dir_partiel in $(ICONS_SIZE_LIST); do \
		cp ./icones/$${dir_partiel}/emblems/*.png $(DESTDIR)/$(PREFIX)/$(KDE_ICONS_DIR)/$${dir_partiel}/emblems; \
	done

uninstall: $(SOBASE) $(BIN_FILES)
	for bin in $(BIN_FILES); do \
		rm $(DESTDIR)/$(PREFIX)/bin/$${bin}; \
	done
	for lib in $(LIB_FILES); do \
		rm $(DESTDIR)/$(PREFIX)/lib/$${lib}; \
	done
	for slib in $(LIB_LINKS); do \
		rm $(DESTDIR)/$(PREFIX)/lib/$${slib}; \
	done
	for inc in $(INC_FILES); do \
		rm $(DESTDIR)/$(PREFIX)/include/$${inc}; \
	done
	for dir_partiel in $(ICONS_SIZE_LIST); do \
		rm $(DESTDIR)/$(PREFIX)/$(KDE_ICONS_DIR)/$${dir_partiel}/emblems/emblem-rmb.png; \
	done
	for dir_partiel in $(ICONS_SIZE_LIST); do \	
		rm $(DESTDIR)/$(PREFIX)/$(KDE_ICONS_DIR)/$${dir_partiel}/emblems/emblem-rmh.png; \
	done


	rm $(DESTDIR)/$(KDE_SERVICES_DIR)/emblem.desktop
	rm $(DESTDIR)/$(ETC_DIR)/extended_attributes_configuration.conf

setorigin: setorigin.o emblem-configuration.o
	$(CC) -o $@ $(LDLIBS) $(LDFLAGS_BIN)  $^

$(SOBASE): emblem-configuration.o
	$(CC) -o $(SOVERSION) -Wl,-soname,$(SONAME) $(LDLIBS) $(LDFLAGS_SHARED) $^
	ln -sf $(SOVERSION) $(SONAME)
	ln -sf $(SOVERSION) $@

%.o: %.cpp %.h
	$(CC) -c -o $@ $(CFLAGS) -Dchemin=\"/$(PREFIX)/$(ETC_DIR)/extended_attributes_configuration.conf\"  $<

%.o: %.cpp 
	$(CC) -c -o $@ $(CFLAGS) -Dchemin=\"/$(PREFIX)/$(ETC_DIR)/extended_attributes_configuration.conf\"   $<
