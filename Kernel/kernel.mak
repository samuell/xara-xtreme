# $Header: c:\RCSdir\kernel/kernel.mak 1.97 1995/11/15 15:09:03 Jim Exp $

# Kernel stuff for all versions
!IF "$(EXCLUDE_FROM_RALPH)" != "1"
KOBJECTS = $(OBJDIR)\test$O
!ELSE
KOBJECTS = 
!ENDIF
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\camtypes$O \
$(OBJDIR)\main$O $(OBJDIR)\main1$O $(OBJDIR)\main2$O $(OBJDIR)\main3$O \
$(OBJDIR)\docview$O $(OBJDIR)\node$O $(OBJDIR)\dumbnode$O $(OBJDIR)\noderend$O $(OBJDIR)\range$O \
$(OBJDIR)\ops$O $(OBJDIR)\undoop$O $(OBJDIR)\ophist$O \
$(OBJDIR)\attrmgr$O $(OBJDIR)\attr$O $(OBJDIR)\attrmap$O \
$(OBJDIR)\nodeattr$O \
$(OBJDIR)\objreg$O $(OBJDIR)\stack$O $(OBJDIR)\matrix$O $(OBJDIR)\xmatrix$O \
$(OBJDIR)\list$O $(OBJDIR)\listitem$O $(OBJDIR)\ordlist$O $(OBJDIR)\collist$O $(OBJDIR)\scunit$O \
$(OBJDIR)\layermgr$O $(OBJDIR)\rect$O $(OBJDIR)\document$O $(OBJDIR)\basedoc$O $(OBJDIR)\app$O \
$(OBJDIR)\wrkcoord$O $(OBJDIR)\paths$O \
$(OBJDIR)\wrkrect$O $(OBJDIR)\nodepath$O $(OBJDIR)\noderect$O $(OBJDIR)\pump$O \
$(OBJDIR)\doccoord$O $(OBJDIR)\docrect$O $(OBJDIR)\rgnlist$O \
$(OBJDIR)\colormgr$O $(OBJDIR)\colclist$O $(OBJDIR)\colcontx$O $(OBJDIR)\colourix$O $(OBJDIR)\doccolor$O \
$(OBJDIR)\region$O $(OBJDIR)\coord$O \
$(OBJDIR)\rndrgn$O $(OBJDIR)\rndctrl$O $(OBJDIR)\opdesc$O $(OBJDIR)\ndoptmz$O \
$(OBJDIR)\module$O $(OBJDIR)\modlist$O $(OBJDIR)\tool$O $(OBJDIR)\toollist$O \
$(OBJDIR)\prefs$O $(OBJDIR)\ccfile$O \
$(OBJDIR)\cmdctrl$O $(OBJDIR)\quality$O $(OBJDIR)\qualops$O \
$(OBJDIR)\vectrndr$O\
$(OBJDIR)\epsclist$O \
$(OBJDIR)\epscdef$O \
$(OBJDIR)\saveeps$O $(OBJDIR)\epsstack$O $(OBJDIR)\epssitem$O \
$(OBJDIR)\epsfiltr$O $(OBJDIR)\filters$O \
$(OBJDIR)\csrstack$O $(OBJDIR)\rndstack$O \
$(OBJDIR)\trans2d$O $(OBJDIR)\layer$O \
$(OBJDIR)\group$O $(OBJDIR)\npaper$O $(OBJDIR)\nodedoc$O \
$(OBJDIR)\chapter$O $(OBJDIR)\page$O $(OBJDIR)\spread$O \
$(OBJDIR)\nodeshap$O $(OBJDIR)\nodeelip$O \
$(OBJDIR)\hittest$O $(OBJDIR)\gradtbl$O \
$(OBJDIR)\aw_eps$O $(OBJDIR)\cameleps$O $(OBJDIR)\nativeps$O \
$(OBJDIR)\doccomp$O $(OBJDIR)\colcomp$O $(OBJDIR)\bitmap$O $(OBJDIR)\release$O \
$(OBJDIR)\units$O $(OBJDIR)\invalid$O $(OBJDIR)\bmplist$O $(OBJDIR)\fraclist$O \
$(OBJDIR)\pathutil$O $(OBJDIR)\fillval$O $(OBJDIR)\nodebmp$O $(OBJDIR)\tranform$O \
$(OBJDIR)\fracfill$O \
$(OBJDIR)\nodeblnd$O $(OBJDIR)\nodebldr$O \
$(OBJDIR)\ndbldpth$O \
$(OBJDIR)\pathproc$O \
$(OBJDIR)\gblend$O $(OBJDIR)\gwinding$O \
$(OBJDIR)\nodershp$O $(OBJDIR)\impcol$O \
$(OBJDIR)\objchge$O \
$(OBJDIR)\GCLIP$O $(OBJDIR)\GCLIP0$O $(OBJDIR)\GCLIP1$O $(OBJDIR)\GCLIP2$O \
$(OBJDIR)\fntcache$O $(OBJDIR)\gcache$O $(OBJDIR)\scrnview$O $(OBJDIR)\nodemold$O \
$(OBJDIR)\ndmldgrp$O $(OBJDIR)\nodemldr$O $(OBJDIR)\ndmldink$O $(OBJDIR)\moldshap$O $(OBJDIR)\moldenv$O \
$(OBJDIR)\moldpers$O $(OBJDIR)\ndmldpth$O $(OBJDIR)\view$O \
$(OBJDIR)\nodetext$O $(OBJDIR)\nodetxts$O $(OBJDIR)\nodetxtl$O \
$(OBJDIR)\txtattr$O $(OBJDIR)\ndtxtpth$O $(OBJDIR)\textacts$O \
$(OBJDIR)\gmem$O \
$(OBJDIR)\arrows$O $(OBJDIR)\scanrr$O \
$(OBJDIR)\rrcaps$O $(OBJDIR)\fontlist$O $(OBJDIR)\kerneldc$O \
$(OBJDIR)\zstream$O $(OBJDIR)\ziftrees$O $(OBJDIR)\zinflate$O \
$(OBJDIR)\fontman$O \
$(OBJDIR)\usercord$O $(OBJDIR)\userrect$O \
$(OBJDIR)\bmpcomp$O \
$(OBJDIR)\colplate$O \
$(OBJDIR)\cxftree$O $(OBJDIR)\native$O $(OBJDIR)\webfiltr$O $(OBJDIR)\camfiltr$O \
$(OBJDIR)\cxfile$O $(OBJDIR)\cxfrec$O $(OBJDIR)\cxfrech$O $(OBJDIR)\cxftext$O \
$(OBJDIR)\rechcol$O $(OBJDIR)\rechcomp$O $(OBJDIR)\rechdoc$O $(OBJDIR)\rechtext$O \
$(OBJDIR)\cxfrgshp$O $(OBJDIR)\cxfellp$O $(OBJDIR)\cxfrect$O $(OBJDIR)\cxfpoly$O \
$(OBJDIR)\rechrshp$O $(OBJDIR)\rechellp$O $(OBJDIR)\rechrect$O $(OBJDIR)\rechpoly$O \
$(OBJDIR)\rechbmp$O $(OBJDIR)\impbmp$O $(OBJDIR)\cxfnbmp$O \
$(OBJDIR)\rechbmpp$O \
$(OBJDIR)\rechsmth$O \
$(OBJDIR)\sglayer$O \
$(OBJDIR)\dbugtree$O \
$(OBJDIR)\guides$O $(OBJDIR)\exceptio$O \
$(OBJDIR)\opdragbx$O $(OBJDIR)\bitfilt$O $(OBJDIR)\ccpanose$O $(OBJDIR)\grid$O \
$(OBJDIR)\insertnd$O \
$(OBJDIR)\selop$O $(OBJDIR)\nativeop$O \
$(OBJDIR)\selstate$O $(OBJDIR)\menuops$O \
$(OBJDIR)\opgrad$O $(OBJDIR)\lineattr$O $(OBJDIR)\fillattr$O \

# DO NOT ADD NEW FILES TO THE ABOVE GROUP, ADD THEM TO THE LAST GROUP

# Kernel stuff to be omitted from viewer builds
!IFNDEF STANDALONE
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\prnprefs$O \
$(OBJDIR)\optsmisc$O $(OBJDIR)\optsunit$O $(OBJDIR)\optsview$O \
$(OBJDIR)\optsscal$O $(OBJDIR)\optstune$O $(OBJDIR)\optspntr$O \
$(OBJDIR)\prntview$O $(OBJDIR)\optsprin$O $(OBJDIR)\printctl$O \
$(OBJDIR)\princomp$O \
$(OBJDIR)\tooldlg$O $(OBJDIR)\printing$O \
$(OBJDIR)\optspage$O \
$(OBJDIR)\psrndrgn$O \
$(OBJDIR)\penedit$O $(OBJDIR)\fillndge$O \
$(OBJDIR)\shapeops$O \
$(OBJDIR)\pathndge$O \
$(OBJDIR)\la_base$O $(OBJDIR)\lattrops$O \
$(OBJDIR)\property$O $(OBJDIR)\layerprp$O $(OBJDIR)\prpslyrs$O $(OBJDIR)\prpsgds$O \
$(OBJDIR)\makebmp$O $(OBJDIR)\nodecomp$O
!ENDIF

# Gallery object files
!IF "$(EXCLUDE_GALS)" != "1"
!IFNDEF STANDALONE
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\sglfills$O $(OBJDIR)\sgcolour$O $(OBJDIR)\sgbitmap$O
# remove line gallery from webster builds
!IF "$(WEBSTER)" != "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\sgline$O $(OBJDIR)\sgline2$O $(OBJDIR)\sgldrag$O
!ENDIF
!ENDIF

KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\gallery$O $(OBJDIR)\exagal$O $(OBJDIR)\sgdrag$O \
$(OBJDIR)\sgtree$O \
$(OBJDIR)\sglib$O $(OBJDIR)\sgscan$O $(OBJDIR)\sginit$O $(OBJDIR)\sglcart$O \
$(OBJDIR)\colgal$O $(OBJDIR)\layergal$O \
$(OBJDIR)\sgallery$O $(OBJDIR)\sglbase$O $(OBJDIR)\sgscanf$O $(OBJDIR)\sgmenu$O 
!ENDIF

# Tracer object files
!IF "$(EXCLUDE_TRACER)" != "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\tracergn$O $(OBJDIR)\tracedlg$O $(OBJDIR)\tracectl$O 
!ENDIF

# Bitmap FX stuff
!IF "$(EXCLUDE_BFX)" != "1"
!IFNDEF STANDALONE
!IF "$(WEBSTER)" != "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\bfxdlg$O
!ENDIF
!ENDIF
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\bfxbase$O $(OBJDIR)\bfxatom$O
!IF "$(WEBSTER)" != "1"
#Accusoft stuff
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\bitmapfx$O  $(OBJDIR)\bfxtest$O
!ENDIF
!ENDIF

# General exclude from Ralph stuff
!IF "$(EXCLUDE_FROM_RALPH)" != "1"
!IFNDEF STANDALONE
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\coldlog$O $(OBJDIR)\colmenu$O $(OBJDIR)\aligndlg$O
!ENDIF
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\bmpexdoc$O $(OBJDIR)\newcol$O $(OBJDIR)\expcol$O \
$(OBJDIR)\aboutdlg$O \
$(OBJDIR)\renddlg$O $(OBJDIR)\rnddlgs$O \
$(OBJDIR)\finfodlg$O $(OBJDIR)\blobby$O \
$(OBJDIR)\statline$O \
$(OBJDIR)\rulers$O \
$(OBJDIR)\moldedit$O \
$(OBJDIR)\pagesize$O \
$(OBJDIR)\appprefs$O \
$(OBJDIR)\bmpprefs$O \
$(OBJDIR)\impexpop$O \
$(OBJDIR)\optsgrid$O \
$(OBJDIR)\pathedit$O \
$(OBJDIR)\filelist$O \
$(OBJDIR)\textfltr$O \
$(OBJDIR)\drawfltr$O \
$(OBJDIR)\ai_eps$O \
$(OBJDIR)\ai5_eps$O \
$(OBJDIR)\ai8_eps$O \
$(OBJDIR)\ai_bmp$O \
$(OBJDIR)\ai_grad$O \
$(OBJDIR)\ai_layer$O \
$(OBJDIR)\ai_epsrr$O \
$(OBJDIR)\coreleps$O $(OBJDIR)\freeeps$O \
$(OBJDIR)\debugdlg$O $(OBJDIR)\opimgset$O \
$(OBJDIR)\infobar$O \
$(OBJDIR)\opwhat$O \
$(OBJDIR)\selall$O \
$(OBJDIR)\menuitem$O $(OBJDIR)\contmenu$O $(OBJDIR)\viewmenu$O \
$(OBJDIR)\radio$O \
$(OBJDIR)\input$O \
$(OBJDIR)\opsmooth$O $(OBJDIR)\rsmooth$O $(OBJDIR)\combshps$O \
$(OBJDIR)\archive$O \
$(OBJDIR)\expbmp$O \
$(OBJDIR)\snapops$O \
$(OBJDIR)\groupops$O \
$(OBJDIR)\zdeflate$O $(OBJDIR)\zdftrees$O \
$(OBJDIR)\oppull$O \
$(OBJDIR)\linwthop$O \
$(OBJDIR)\opnudge$O $(OBJDIR)\opbreak$O \
$(OBJDIR)\attrappl$O \
$(OBJDIR)\opsmpshp$O \
$(OBJDIR)\zordops$O $(OBJDIR)\mkshapes$O \
$(OBJDIR)\pathops$O \
$(OBJDIR)\cutop$O \
$(OBJDIR)\clipint$O $(OBJDIR)\cpyact$O \
$(OBJDIR)\transop$O $(OBJDIR)\sentryop$O $(OBJDIR)\paper$O \
$(OBJDIR)\dialogop$O $(OBJDIR)\bars$O \
$(OBJDIR)\blobs$O \
$(OBJDIR)\cmxexdc$O $(OBJDIR)\cmxrendr$O $(OBJDIR)\cmxdcobj$O \
$(OBJDIR)\draginfo$O $(OBJDIR)\snap$O \
$(OBJDIR)\webop$O \
$(OBJDIR)\xarprefs$O \
$(OBJDIR)\webprefs$O \
$(OBJDIR)\hotkeys$O \
$(OBJDIR)\cmxfiltr$O \
$(OBJDIR)\prnmkcom$O $(OBJDIR)\prnmks$O \
$(OBJDIR)\hlinkdlg$O $(OBJDIR)\tipsdlg$O
!ENDIF
# DO NOT ADD NEW FILES TO THE ABOVE GROUP, ADD THEM TO THE LAST GROUP

# New native and web filter files, was conditional compiled, now always present
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\png$O $(OBJDIR)\pngerror$O $(OBJDIR)\pngget$O $(OBJDIR)\pngmem$O $(OBJDIR)\pngread$O $(OBJDIR)\pngrio$O \
$(OBJDIR)\pngrtran$O $(OBJDIR)\pngrutil$O $(OBJDIR)\pngset$O $(OBJDIR)\pngtrans$O $(OBJDIR)\pngvcrd$O $(OBJDIR)\pngwio$O \
$(OBJDIR)\pngwrite$O $(OBJDIR)\pngwtran$O $(OBJDIR)\pngwutil$O $(OBJDIR)\pngcrc32$O $(OBJDIR)\pngfuncs$O $(OBJDIR)\pngprgrs$O \
$(OBJDIR)\unitcomp$O $(OBJDIR)\infocomp$O $(OBJDIR)\rechunit$O $(OBJDIR)\rechinfo$O $(OBJDIR)\viewcomp$O $(OBJDIR)\rechattr$O \

# This group contains the CDR / CMX importers which are currently living in ralph limbo land
#!IF "$(EXCLUDE_FROM_RALPH)" != "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\cdrtext$O $(OBJDIR)\rifffile$O \
$(OBJDIR)\cmxistut$O $(OBJDIR)\cmxicmds$O \
$(OBJDIR)\coplfilr$O $(OBJDIR)\cmxirefs$O \
$(OBJDIR)\cdrcol$O $(OBJDIR)\cdrfill$O $(OBJDIR)\cdroutl$O \
$(OBJDIR)\cdrfiltr$O $(OBJDIR)\cmxibits$O $(OBJDIR)\cmxifltr$O \
#!ENDIF

# This lot are included in ralph, but NOT THE MAIN CAMELOT BUILD
!IF "$(EXCLUDE_FROM_RALPH)" == "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\ccafile$O 
!ENDIF

# JPEG Import Filter Files
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\jdmaster$O $(OBJDIR)\jdinput$O $(OBJDIR)\jdmainct$O $(OBJDIR)\jdcoefct$O \
$(OBJDIR)\jdpostct$O $(OBJDIR)\jdmarker$O $(OBJDIR)\jdhuff$O $(OBJDIR)\jdphuff$O \
$(OBJDIR)\jddctmgr$O $(OBJDIR)\jidctint$O $(OBJDIR)\jidctfst$O $(OBJDIR)\jidctflt$O \
$(OBJDIR)\jidctred$O $(OBJDIR)\jdsample$O $(OBJDIR)\jdcolor$O $(OBJDIR)\jdmerge$O \
$(OBJDIR)\jquant1$O $(OBJDIR)\jquant2$O \
$(OBJDIR)\jmemmgr$O $(OBJDIR)\jutils$O $(OBJDIR)\jmemnobs$O \
$(OBJDIR)\jdapimin$O $(OBJDIR)\jdapistd$O $(OBJDIR)\jcomapi$O $(OBJDIR)\jdtrans$O \
$(OBJDIR)\jpgsrc$O $(OBJDIR)\jpgermgr$O $(OBJDIR)\jpgprgrs$O \
$(OBJDIR)\imjpeg$O

# This is a list of new things added to camelot since ralph was last built. Please
# add new files here rather than elsewhere, so ralph and standalone maintainers can
# see what's new and needs to go...

# JPEG Export Filter Files
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\jcapimin$O $(OBJDIR)\jcapistd$O $(OBJDIR)\jcparam$O $(OBJDIR)\jctrans$O \
$(OBJDIR)\jcinit$O $(OBJDIR)\jcmaster$O $(OBJDIR)\jcmainct$O $(OBJDIR)\jcprepct$O \
$(OBJDIR)\jccoefct$O $(OBJDIR)\jccolor$O $(OBJDIR)\jcsample$O $(OBJDIR)\jcdctmgr$O \
$(OBJDIR)\jfdctint$O $(OBJDIR)\jfdctfst$O $(OBJDIR)\jfdctflt$O $(OBJDIR)\jchuff$O \
$(OBJDIR)\jcphuff$O $(OBJDIR)\jcmarker$O \
$(OBJDIR)\jpgdest$O \
$(OBJDIR)\exjpeg$O

# Do we want PhotoShop/Freehand plug-in support?
!IF "$(PLUGINSUPPORT)" == "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\plugmngr$O $(OBJDIR)\plugin$O $(OBJDIR)\plugop$O $(OBJDIR)\optsplug$O $(OBJDIR)\plugopun$O \

# More bitmap FX stuff
!IF "$(EXCLUDE_BFX)" != "1"
!IF "$(WEBSTER)" != "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\bfxmngr$O $(OBJDIR)\bfxitem$O $(OBJDIR)\bfxop$O $(OBJDIR)\bfxopun$O $(OBJDIR)\bfxdlg2$O
!ENDIF
!ENDIF
!ENDIF


KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\isetattr$O $(OBJDIR)\fontcomp$O \
$(OBJDIR)\userattr$O $(OBJDIR)\webattr$O $(OBJDIR)\filtimop$O $(OBJDIR)\filtimag$O $(OBJDIR)\filtirr$O \
$(OBJDIR)\imagemap$O $(OBJDIR)\clicarea$O $(OBJDIR)\fimagdlg$O $(OBJDIR)\htmlexp$O \
$(OBJDIR)\urlimp$O $(OBJDIR)\urldlg$O $(OBJDIR)\overlist$O $(OBJDIR)\overitem$O \
$(OBJDIR)\strlist$O $(OBJDIR)\webaddr$O $(OBJDIR)\crthumb$O $(OBJDIR)\webflags$O \
$(OBJDIR)\rechprnt$O \
$(OBJDIR)\impstr$O \
$(OBJDIR)\bmpsrc$O $(OBJDIR)\ccbuffil$O $(OBJDIR)\cxfmap$O $(OBJDIR)\cxflists$O \
$(OBJDIR)\prvwflt$O \
$(OBJDIR)\vector$O $(OBJDIR)\exprview$O $(OBJDIR)\filtrmgr$O \
$(OBJDIR)\vector3d$O $(OBJDIR)\noisebas$O \
$(OBJDIR)\noise1$O $(OBJDIR)\noisef$O $(OBJDIR)\genv$O $(OBJDIR)\gmould$O \
$(OBJDIR)\optsinet$O $(OBJDIR)\inetop$O \
$(OBJDIR)\backgrnd$O $(OBJDIR)\htmlfltr$O $(OBJDIR)\prevwdlg$O \
$(OBJDIR)\bmpsdlg$O $(OBJDIR)\sgframe$O $(OBJDIR)\htmllist$O $(OBJDIR)\bmpexprw$O $(OBJDIR)\frameops$O \
$(OBJDIR)\APrps$O $(OBJDIR)\prvwmenu$O $(OBJDIR)\bmapprev$O  $(OBJDIR)\ccc$O \
$(OBJDIR)\tmpltdlg$O $(OBJDIR)\tmpltarg$O $(OBJDIR)\tmpltatr$O $(OBJDIR)\attraggl$O $(OBJDIR)\gadget$O $(OBJDIR)\uielem$O $(OBJDIR)\opnode$O $(OBJDIR)\styles$O $(OBJDIR)\namedlg$O \
$(OBJDIR)\ftfilter$O $(OBJDIR)\cxftfile$O $(OBJDIR)\ccbhfile$O $(OBJDIR)\exphint$O \
$(OBJDIR)\fillramp$O $(OBJDIR)\cmxtree$O \
$(OBJDIR)\clamp$O $(OBJDIR)\biasgain$O $(OBJDIR)\biasdlg$O $(OBJDIR)\biasgdgt$O \
$(OBJDIR)\ngcore$O $(OBJDIR)\ngdialog$O $(OBJDIR)\ngdrag$O $(OBJDIR)\ngitem$O \
$(OBJDIR)\ngiter$O $(OBJDIR)\ngprop$O $(OBJDIR)\ngscan$O $(OBJDIR)\ngsentry$O $(OBJDIR)\ngsetop$O \
$(OBJDIR)\slice$O $(OBJDIR)\barcreationdlg$O $(OBJDIR)\opbarcreation$O \
$(OBJDIR)\slicehelper$O $(OBJDIR)\OpDupBar$O \
$(OBJDIR)\bmpalctrl$O $(OBJDIR)\bmpalint$O \
$(OBJDIR)\palmenu$O $(OBJDIR)\extender$O $(OBJDIR)\extendsetsdlg$O \
$(OBJDIR)\ndbrshmk$O $(OBJDIR)\ndbrshpt$O $(OBJDIR)\nodebrsh$O \
$(OBJDIR)\gla$O $(OBJDIR)\offattr$O $(OBJDIR)\fthrattr$O $(OBJDIR)\opfeathr$O \
$(OBJDIR)\grndbrsh$O $(OBJDIR)\linecomp$O $(OBJDIR)\brshcomp$O \
$(OBJDIR)\linedef$O $(OBJDIR)\brshattr$O $(OBJDIR)\brushref$O \
$(OBJDIR)\ppbrush$O $(OBJDIR)\sgbrush$O $(OBJDIR)\loadbrsh$O \
$(OBJDIR)\ndclpcnt$O $(OBJDIR)\nodeclip$O $(OBJDIR)\blndhelp$O \
$(OBJDIR)\brushdlg$O $(OBJDIR)\brshgdgt$O $(OBJDIR)\brshdata$O \
$(OBJDIR)\bldbrdef$O $(OBJDIR)\blppbrsh$O \
$(OBJDIR)\brushop$O $(OBJDIR)\brshbeca$O $(OBJDIR)\cnamecol$O \
$(OBJDIR)\clipattr$O $(OBJDIR)\brpress$O \
$(OBJDIR)\swftext$O $(OBJDIR)\swffont$O $(OBJDIR)\swfshape$O $(OBJDIR)\swfplace$O \
$(OBJDIR)\swfexpdc$O $(OBJDIR)\swfrndr$O $(OBJDIR)\swffiltr$O $(OBJDIR)\swfbitmp$O \
$(OBJDIR)\swfbuttn$O $(OBJDIR)\swfsprit$O $(OBJDIR)\samplist$O $(OBJDIR)\brshname$O \
$(OBJDIR)\control$O $(OBJDIR)\pbecomea$O

# Random half implemented stuff that we should remove from non-VECTOR_STROKING builds
!IF "$(VECTOR_STROKING)" == "1"
KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\cache$O $(OBJDIR)\objcache$O $(OBJDIR)\cachrand$O \
$(OBJDIR)\cachweak$O $(OBJDIR)\cachfifo$O $(OBJDIR)\mkstroke$O \
$(OBJDIR)\nodecont$O $(OBJDIR)\nodeshad$O \
$(OBJDIR)\timespan$O \
$(OBJDIR)\pathpcs$O $(OBJDIR)\ppstroke$O $(OBJDIR)\ppairbsh$O $(OBJDIR)\ppvecstr$O $(OBJDIR)\strkcomp$O \
$(OBJDIR)\pathtrap$O $(OBJDIR)\valfunc$O $(OBJDIR)\pathstrk$O $(OBJDIR)\pressure$O $(OBJDIR)\sgstroke$O \
$(OBJDIR)\strkattr$O \
$(OBJDIR)\nodeabmp$O \
$(OBJDIR)\davedlg$O $(OBJDIR)\daveimp$O $(OBJDIR)\bmanip$O $(OBJDIR)\bevtrap$O \
$(OBJDIR)\beveler$O $(OBJDIR)\nodebev$O $(OBJDIR)\nbevcont$O $(OBJDIR)\bevfill$O \
$(OBJDIR)\attrbev$O $(OBJDIR)\ppbevel$O $(OBJDIR)\bshadow$O \
$(OBJDIR)\nodecntr$O $(OBJDIR)\ncntrcnt$O $(OBJDIR)\CStroke$O \
!ENDIF

KOBJECTS = $(KOBJECTS) \
$(OBJDIR)\xadraw$O $(OBJDIR)\xadrwold$O \
$(OBJDIR)\editsel$O \
$(OBJDIR)\bitmapcache$O $(OBJDIR)\bitmapcachekey$O $(OBJDIR)\capturemanager$O \
$(OBJDIR)\nodepostpro$O $(OBJDIR)\nodeliveeffect$O \
$(OBJDIR)\xpfilter$O $(OBJDIR)\xpfcaps$O $(OBJDIR)\xpfrgn$O

# ADD NEW KERNEL OBJECTS TO THE LAST GROUP ABOVE
