BUILT_SOURCES	= camtypes.h.gch
include_HEADERS	= camtypes.h
CLEANFILES		= camtypes.h.gch

CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
        $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(XARAFLAGS)

include ./$(DEPDIR)/camtypes.h.Po

$(DEPDIR)/camtypes.h.Po:
	if test ! -d "$(DEPDIR)"; then mkdir $(DEPDIR); fi
	touch $(DEPDIR)/camtypes.h.Po

camtypes.h.gch: camtypes.h
	if $(CXXCOMPILE) -x c++-header -MT $@ -MD -MP -MF "$(DEPDIR)/camtypes.h.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/camtypes.h.Tpo" "$(DEPDIR)/camtypes.h.Po"; else rm -f "$(DEPDIR)/camtypes.h.Tpo"; exit 1; fi
	make -C ../Kernel clean
	make -C ../wxXtra clean
	make -C ../tools clean
	make -C ../wxOil clean
	


