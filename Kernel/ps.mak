# $Header: c:\RCSdir\kernel/ps.mak 1.5 1995/07/07 14:43:45 Tim Exp $

# Makefile to build the PostScript (EPS) header file

$(OBJDIR)\prolog.ps :  	kernel\gradfill.ps \
						kernel\pathproc.ps \
						kernel\groups.ps \
						kernel\blends.ps \
						kernel\genprocs.ps \
						kernel\basicim.ps \
						kernel\colorim.ps \
						kernel\txtprocs.ps
	pszip kernel\genprocs.ps >  $(OBJDIR)\prolog.ps
	pszip kernel\basicim.ps  >> $(OBJDIR)\prolog.ps
	pszip kernel\colorim.ps  >> $(OBJDIR)\prolog.ps
	pszip kernel\gradfill.ps >> $(OBJDIR)\prolog.ps
	pszip kernel\pathproc.ps >> $(OBJDIR)\prolog.ps
	pszip kernel\txtprocs.ps >> $(OBJDIR)\prolog.ps
	pszip kernel\groups.ps   >> $(OBJDIR)\prolog.ps
	pszip kernel\blends.ps 	 >> $(OBJDIR)\prolog.ps

$(OBJDIR)\setup.ps :  	kernel\setup.ps
	pszip kernel\setup.ps >  $(OBJDIR)\setup.ps

$(OBJDIR)\spotfunc.ps :  	kernel\spotfunc.ps
	pszip kernel\spotfunc.ps >  $(OBJDIR)\spotfunc.ps

