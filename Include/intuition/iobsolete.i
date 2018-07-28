	IFND INTUITION_IOBSOLETE_I
INTUITION_IOBSOLETE_I SET 1
**
**  $VER: iobsolete.i 38.0 (12.06.91)
**  Includes Release 39.108
**
**  Obsolete identifiers for Intuition.  Use the new ones instead!
**
**  (C) Copyright 1985-1992 Commodore-Amiga, Inc.
**	    All Rights Reserved
**


* This file contains:
*
* 1.  The traditional identifiers for gadget Flags, Activation, and Type,
* and for window Flags and IDCMP classes.  They are defined in terms
* of their new versions, which serve to prevent confusion between
* similar-sounding but different identifiers (like IDCMP_WINDOWACTIVE
* and WFLG_ACTIVATE).
*
* 2.  Some tag names and constants whose labels were adjusted after V36.
*
* By default, 1 and 2 are enabled.
*
* Set INTUI_V36_NAMES_ONLY to exclude the traditional identifiers and
* the original V36 names of some identifiers.
*


	IFND INTUITION_INTUITION_I
	INCLUDE "intuition/intuition.i"
	ENDC

	IFND INTUITION_SCREENS_I
	INCLUDE "intuition/screens.i"
	ENDC

	IFND INTUITION_GADGETCLASS_I
	INCLUDE "intuition/gadgetclass.i"
	ENDC

	IFND INTUITION_IMAGECLASS_I
	INCLUDE "intuition/imageclass.i"
	ENDC

* Set INTUI_V36_NAMES_ONLY to remove these older names

	IFND INTUI_V36_NAMES_ONLY


* V34-style Gadget->Flags names:

GADGHIGHBITS	equ	GFLG_GADGHIGHBITS
GADGHCOMP	equ	GFLG_GADGHCOMP
GADGHBOX	equ	GFLG_GADGHBOX
GADGHIMAGE	equ	GFLG_GADGHIMAGE
GADGHNONE	equ	GFLG_GADGHNONE
GADGIMAGE	equ	GFLG_GADGIMAGE
GRELBOTTOM	equ	GFLG_RELBOTTOM
GRELRIGHT	equ	GFLG_RELRIGHT
GRELWIDTH	equ	GFLG_RELWIDTH
GRELHEIGHT	equ	GFLG_RELHEIGHT
SELECTED	equ	GFLG_SELECTED
GADGDISABLED	equ	GFLG_DISABLED
LABELMASK	equ	GFLG_LABELMASK
LABELITEXT	equ	GFLG_LABELITEXT
LABELSTRING	equ	GFLG_LABELSTRING
LABELIMAGE	equ	GFLG_LABELIMAGE


* V34-style Gadget->Activation flag names:

RELVERIFY	equ	GACT_RELVERIFY
GADGIMMEDIATE	equ	GACT_IMMEDIATE
ENDGADGET	equ	GACT_ENDGADGET
FOLLOWMOUSE	equ	GACT_FOLLOWMOUSE
RIGHTBORDER	equ	GACT_RIGHTBORDER
LEFTBORDER	equ	GACT_LEFTBORDER
TOPBORDER	equ	GACT_TOPBORDER
BOTTOMBORDER	equ	GACT_BOTTOMBORDER
BORDERSNIFF	equ	GACT_BORDERSNIFF
TOGGLESELECT	equ	GACT_TOGGLESELECT
BOOLEXTEND	equ	GACT_BOOLEXTEND
STRINGLEFT	equ	GACT_STRINGLEFT
STRINGCENTER	equ	GACT_STRINGCENTER
STRINGRIGHT	equ	GACT_STRINGRIGHT
LONGINT		equ	GACT_LONGINT
ALTKEYMAP	equ	GACT_ALTKEYMAP
STRINGEXTEND	equ	GACT_STRINGEXTEND
ACTIVEGADGET	equ	GACT_ACTIVEGADGET


* V34-style Gadget->Type names:

GADGETTYPE	equ	GTYP_GADGETTYPE
SYSGADGET	equ	GTYP_SYSGADGET
SCRGADGET	equ	GTYP_SCRGADGET
GZZGADGET	equ	GTYP_GZZGADGET
REQGADGET	equ	GTYP_REQGADGET
SIZING		equ	GTYP_SIZING
WDRAGGING	equ	GTYP_WDRAGGING
SDRAGGING	equ	GTYP_SDRAGGING
WUPFRONT	equ	GTYP_WUPFRONT
SUPFRONT	equ	GTYP_SUPFRONT
WDOWNBACK	equ	GTYP_WDOWNBACK
SDOWNBACK	equ	GTYP_SDOWNBACK
CLOSE		equ	GTYP_CLOSE
BOOLGADGET	equ	GTYP_BOOLGADGET
GADGET0002	equ	GTYP_GADGET0002
PROPGADGET	equ	GTYP_PROPGADGET
STRGADGET	equ	GTYP_STRGADGET
CUSTOMGADGET	equ	GTYP_CUSTOMGADGET
GTYPEMASK	equ	GTYP_GTYPEMASK


* V34-style IDCMP class names:

SIZEVERIFY	equ	IDCMP_SIZEVERIFY
NEWSIZE		equ	IDCMP_NEWSIZE
REFRESHWINDOW	equ	IDCMP_REFRESHWINDOW
MOUSEBUTTONS	equ	IDCMP_MOUSEBUTTONS
MOUSEMOVE	equ	IDCMP_MOUSEMOVE
GADGETDOWN	equ	IDCMP_GADGETDOWN
GADGETUP	equ	IDCMP_GADGETUP
REQSET		equ	IDCMP_REQSET
MENUPICK	equ	IDCMP_MENUPICK
CLOSEWINDOW	equ	IDCMP_CLOSEWINDOW
RAWKEY		equ	IDCMP_RAWKEY
REQVERIFY	equ	IDCMP_REQVERIFY
REQCLEAR	equ	IDCMP_REQCLEAR
MENUVERIFY	equ	IDCMP_MENUVERIFY
NEWPREFS	equ	IDCMP_NEWPREFS
DISKINSERTED	equ	IDCMP_DISKINSERTED
DISKREMOVED	equ	IDCMP_DISKREMOVED
WBENCHMESSAGE	equ	IDCMP_WBENCHMESSAGE
ACTIVEWINDOW	equ	IDCMP_ACTIVEWINDOW
INACTIVEWINDOW	equ	IDCMP_INACTIVEWINDOW
DELTAMOVE	equ	IDCMP_DELTAMOVE
VANILLAKEY	equ	IDCMP_VANILLAKEY
INTUITICKS	equ	IDCMP_INTUITICKS
IDCMPUPDATE	equ	IDCMP_IDCMPUPDATE
MENUHELP	equ	IDCMP_MENUHELP
CHANGEWINDOW	equ	IDCMP_CHANGEWINDOW
LONELYMESSAGE	equ	IDCMP_LONELYMESSAGE


* V34-style Window->Flags names:

WINDOWSIZING	equ	WFLG_SIZEGADGET
WINDOWDRAG	equ	WFLG_DRAGBAR
WINDOWDEPTH	equ	WFLG_DEPTHGADGET
WINDOWCLOSE	equ	WFLG_CLOSEGADGET
SIZEBRIGHT	equ	WFLG_SIZEBRIGHT
SIZEBBOTTOM	equ	WFLG_SIZEBBOTTOM
REFRESHBITS	equ	WFLG_REFRESHBITS
SMART_REFRESH	equ	WFLG_SMART_REFRESH
SIMPLE_REFRESH	equ	WFLG_SIMPLE_REFRESH
SUPER_BITMAP	equ	WFLG_SUPER_BITMAP
OTHER_REFRESH	equ	WFLG_OTHER_REFRESH
BACKDROP	equ	WFLG_BACKDROP
REPORTMOUSE	equ	WFLG_REPORTMOUSE
GIMMEZEROZERO	equ	WFLG_GIMMEZEROZERO
BORDERLESS	equ	WFLG_BORDERLESS
ACTIVATE	equ	WFLG_ACTIVATE
WINDOWACTIVE	equ	WFLG_WINDOWACTIVE
INREQUEST	equ	WFLG_INREQUEST
MENUSTATE	equ	WFLG_MENUSTATE
RMBTRAP		equ	WFLG_RMBTRAP
NOCAREREFRESH	equ	WFLG_NOCAREREFRESH
WINDOWREFRESH	equ	WFLG_WINDOWREFRESH
WBENCHWINDOW	equ	WFLG_WBENCHWINDOW
WINDOWTICKED	equ	WFLG_WINDOWTICKED
NW_EXTENDED	equ	WFLG_NW_EXTENDED
VISITOR		equ	WFLG_VISITOR
ZOOMED		equ	WFLG_ZOOMED
HASZOOM		equ	WFLG_HASZOOM


* These are the obsolete tag names for general gadgets, proportional gadgets,
* and string gadgets.  Use the mixed-case equivalents from gadgetclass.h
* instead.
*

GA_LEFT			equ	GA_Left
GA_RELRIGHT		equ	GA_RelRight
GA_TOP			equ	GA_Top
GA_RELBOTTOM		equ	GA_RelBottom
GA_WIDTH		equ	GA_Width
GA_RELWIDTH		equ	GA_RelWidth
GA_HEIGHT		equ	GA_Height
GA_RELHEIGHT		equ	GA_RelHeight
GA_TEXT			equ	GA_Text
GA_IMAGE		equ	GA_Image
GA_BORDER		equ	GA_Border
GA_SELECTRENDER		equ	GA_SelectRender
GA_HIGHLIGHT		equ	GA_Highlight
GA_DISABLED		equ	GA_Disabled
GA_GZZGADGET		equ	GA_GZZGadget
GA_USERDATA		equ	GA_UserData
GA_SPECIALINFO		equ	GA_SpecialInfo
GA_SELECTED		equ	GA_Selected
GA_ENDGADGET		equ	GA_EndGadget
GA_IMMEDIATE		equ	GA_Immediate
GA_RELVERIFY		equ	GA_RelVerify
GA_FOLLOWMOUSE		equ	GA_FollowMouse
GA_RIGHTBORDER		equ	GA_RightBorder
GA_LEFTBORDER		equ	GA_LeftBorder
GA_TOPBORDER		equ	GA_TopBorder
GA_BOTTOMBORDER		equ	GA_BottomBorder
GA_TOGGLESELECT		equ	GA_ToggleSelect
GA_SYSGADGET		equ	GA_SysGadget
GA_SYSGTYPE		equ	GA_SysGType
GA_PREVIOUS		equ	GA_Previous
GA_NEXT			equ	GA_Next
GA_DRAWINFO		equ	GA_DrawInfo
GA_INTUITEXT		equ	GA_IntuiText
GA_LABELIMAGE		equ	GA_LabelImage

PGA_FREEDOM		equ	PGA_Freedom
PGA_BORDERLESS		equ	PGA_Borderless
PGA_HORIZPOT		equ	PGA_HorizPot
PGA_HORIZBODY		equ	PGA_HorizBody
PGA_VERTPOT		equ	PGA_VertPot
PGA_VERTBODY		equ	PGA_VertBody
PGA_TOTAL		equ	PGA_Total
PGA_VISIBLE		equ	PGA_Visible
PGA_TOP			equ	PGA_Top

LAYOUTA_LAYOUTOBJ	equ	LAYOUTA_LayoutObj
LAYOUTA_SPACING		equ	LAYOUTA_Spacing
LAYOUTA_ORIENTATION	equ	LAYOUTA_Orientation


* These are the obsolete tag names for image attributes.
* Use the mixed-case equivalents from imageclass.h instead.
*

IA_LEFT			equ	IA_Left
IA_TOP			equ	IA_Top
IA_WIDTH		equ	IA_Width
IA_HEIGHT		equ	IA_Height
IA_FGPEN		equ	IA_FGPen
IA_BGPEN		equ	IA_BGPen
IA_DATA			equ	IA_Data
IA_LINEWIDTH		equ	IA_LineWidth
IA_PENS			equ	IA_Pens
IA_RESOLUTION		equ	IA_Resolution
IA_APATTERN		equ	IA_APattern
IA_APATSIZE		equ	IA_APatSize
IA_MODE			equ	IA_Mode
IA_FONT			equ	IA_Font
IA_OUTLINE		equ	IA_Outline
IA_RECESSED		equ	IA_Recessed
IA_DOUBLEEMBOSS		equ	IA_DoubleEmboss
IA_EDGESONLY		equ	IA_EdgesOnly
IA_SHADOWPEN		equ	IA_ShadowPen
IA_HIGHLIGHTPEN		equ	IA_HighlightPen


* These are the obsolete identifiers for the various DrawInfo pens.
* Use the uppercase versions in screens.h instead.
*

detailPen	equ	DETAILPEN
blockPen	equ	BLOCKPEN
textPen		equ	TEXTPEN
shinePen	equ	SHINEPEN
shadowPen	equ	SHADOWPEN
hifillPen	equ	FILLPEN
hifilltextPen	equ	FILLTEXTPEN
backgroundPen	equ	BACKGROUNDPEN
hilighttextPen	equ	HIGHLIGHTTEXTPEN
numDrIPens	equ	NUMDRIPENS


	ENDC	* !INTUI_V36_NAMES_ONLY

	ENDC	* INTUITION_IOBSOLETE_I
