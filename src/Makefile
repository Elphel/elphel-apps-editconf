
#INSTDIR   = $(prefix)/usr/local/bin/
CGIDIR    = /www/pages
BINDIR    = /usr/bin/
INSTMODE  = 0755
INSTOWNER = root
INSTGROUP = root

PROGS     = editconf.cgi unescape strconf.cgi
SRCS = editconf.c unescape.c strconf.cpp
OBJS = editconf.o unescape.o

OBJS += $(SOURCES:.cpp=.o) 

CFLAGS   += -Wall -I$(STAGING_DIR_HOST)/usr/include-uapi
 
all: $(PROGS)

strconf.cgi: strconf.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
editconf.cgi: editconf.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
unescape: unescape.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

install:	$(PROGS)
	$(INSTALL) $(OWN) -d $(DESTDIR)
	$(INSTALL) $(OWN) -d $(DESTDIR)$(CGIDIR)
	$(INSTALL) $(OWN) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) strconf.cgi  $(DESTDIR)$(CGIDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) editconf.cgi $(DESTDIR)$(CGIDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) unescape     $(DESTDIR)$(BINDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) xmlxmt.cgi   $(DESTDIR)$(CGIDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) editconf.php $(DESTDIR)$(CGIDIR)
clean:
	rm -rf $(PROGS) *.o core
#dependency:
#	make depend
#depend:
#	makedepend -Y -- $(CFLAGS) -- $(SRCS) 2>/dev/null
#	touch dependency
# DO NOT DELETE
