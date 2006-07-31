BUILT_SOURCES	= camtypes.h.gch
CLEANFILES		= camtypes.h.gch

CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
        $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(XARALXCXXFLAGS) $(XARAFLAGS)

include ./$(DEPDIR)/camtypes.h.Po

$(DEPDIR)/camtypes.h.Po:
	if test ! -d "$(DEPDIR)"; then mkdir $(DEPDIR); fi
	touch $(DEPDIR)/camtypes.h.Po

camtypes.h.gch: camtypes.h
	if $(CXXCOMPILE) -x c++-header -MT $@ -MD -MP -MF "$(DEPDIR)/camtypes.h.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/camtypes.h.Tpo" "$(DEPDIR)/camtypes.h.Po"; else rm -f "$(DEPDIR)/camtypes.h.Tpo"; exit 1; fi;
	$(MAKE) -C ../Kernel clean
#	$(MAKE) -C ../wxXtra clean - no need to clean wxXtra as it does not depend on camtypes
	$(MAKE) -C ../tools clean
	$(MAKE) -C ../wxOil clean 

	


