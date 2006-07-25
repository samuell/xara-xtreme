MENU "SHARED"
	SUBMENU "FileMenu"
		SUBMENU "FileNew"
			MENUITEM "FileNewDrawing"
			MENUITEM "FileNewAnimation"
			MENUITEM "1FileNewTemplate"
			MENUITEM "2FileNewTemplate"
			MENUITEM "3FileNewTemplate"
			MENUITEM "4FileNewTemplate"
			MENUITEM "5FileNewTemplate"
			MENUITEM "6FileNewTemplate"
			MENUITEM "7FileNewTemplate"
			MENUITEM "8FileNewTemplate"
			MENUITEM "9FileNewTemplate"
			MENUITEM "10FileNewTemplate"
		END
		MENUITEM "FileOpen"
		MENUITEM "FileClose" SEPARATOR
		MENUITEM "FileSave"
		MENUITEM "FileSaveAs"
		MENUITEM "FileSaveAll"	
		MENUITEM "SaveAsDefault"	SEPARATOR
		MENUITEM "Import"
		MENUITEM "URLImport"
		MENUITEM "Export"
		MENUITEM "SaveAnimatedGIF"
		MENUITEM "ImageSlice"		SEPARATOR
		MENUITEM "FileInfo"  		SEPARATOR
		MENUITEM "PageSetupDlg"
		MENUITEM "PrintSetup"
		MENUITEM "PrintOptionsDlg"
		MENUITEM "Print"         SEPARATOR
		MENUITEM "01MRUFile"
		MENUITEM "02MRUFile"
		MENUITEM "03MRUFile"
		MENUITEM "04MRUFile"
		MENUITEM "05MRUFile"
		MENUITEM "06MRUFile"
		MENUITEM "07MRUFile"
		MENUITEM "08MRUFile"
		MENUITEM "09MRUFile"		SEPARATOR
		MENUITEM "Exit"
	END
	SUBMENU "EditMenu"
		MENUITEM "Undo"
		MENUITEM "Redo" 			SEPARATOR
		MENUITEM "Cut"
		MENUITEM "Copy"
		MENUITEM "Paste"
		MENUITEM "PasteAtSamePos"
		MENUITEM "PasteAttributes"
		MENUITEM "Delete"
		MENUITEM "SelectAll"
		MENUITEM "SelectNone" 	SEPARATOR
		MENUITEM "Duplicate"
		MENUITEM "EditClone"
	END
	SUBMENU "ArrangeMenu"
		MENUITEM "MoveForwardsALayer"
		MENUITEM "BringToFront"  
		MENUITEM "MoveForwards"
		MENUITEM "MoveBackwards"
		MENUITEM "PutToBack"
		MENUITEM "MoveBackwardsALayer"	SEPARATOR
		MENUITEM "Group"
		MENUITEM "Ungroup"
		MENUITEM "ApplyClipView"
		MENUITEM "RemoveClipView"
		MENUITEM "ArrangeAlignment"	SEPARATOR
		MENUITEM "JoinShapes"
		MENUITEM "BreakShapes"
		SUBMENU "CombineShapes"
			MENUITEM "AddShapes"
			MENUITEM "SubtractShapes"
			MENUITEM "IntersectShapes"
			MENUITEM "SliceShapes"
		END
		MENUITEM "ConvertPathToShapes"
		MENUITEM "ConvertToShapes"
		MENUITEM "ConvertToBitmap"	SEPARATOR
		MENUITEM "FitTextToPath"
	END
	
	SUBMENU "UtilsMenu"
		SUBMENU "Galleries"
	
			MENUITEM "DisplayColourGallery"
	
	
			MENUITEM "DisplayLayerGallery"
	
			MENUITEM "DisplayBitmapGallery"
	
	
			MENUITEM "DisplayLineGallery"
	
			MENUITEM "DisplayFontsGallery"	
	
	
			MENUITEM "DisplayClipartGallery"
	
	
			MENUITEM "DisplayFillsGallery"
	
			MENUITEM "DisplayFrameGallery"
	
			MENUITEM "DisplayNameGallery"		
	
		END
		MENUITEM "ColourPicker"			SEPARATOR
		MENUITEM "WebAddressDlg"				SEPARATOR
		MENUITEM "XPEEdit"
		MENUITEM "TraceDlg"					
	MENUITEM "BfxDefaultPlugIn"
		SUBMENU "Animation"			SEPARATOR
			MENUITEM "AnimationProperties"
			MENUITEM "GrabAllFrames"
			MENUITEM "BrowserPreview"
		END
		MENUITEM "UtilOptions"
		
		
	END
	SUBMENU "WindowMenu"
		MENUITEM "WindowNewView"
		MENUITEM "WindowTile"
		MENUITEM "WindowCascade"
		MENUITEM "WindowArrange"		SEPARATOR
		MENUITEM "ToolbarDlg"
		SUBMENU "BarsMenu"			SEPARATOR
			MENUITEM "IDD_BUTTBAR_GENERAL"
			MENUITEM "IDD_BUTTBAR_IMAGESETTING"
			MENUITEM "IDD_BUTTBAR_WINDOW"
			MENUITEM "IDD_BUTTBAR_ARRANGE"
			MENUITEM "IDD_BUTTBAR_EDIT"
			MENUITEM "IDD_BUTTBAR_FILE"
			MENUITEM "IDD_BUTTBAR_GALLERIES"
			MENUITEM "IDD_BUTTBAR_STANDARD"
			MENUITEM "IDD_BUTTBAR_TOOLBAR"
			MENUITEM "IDD_BUTTBAR_ANIMATION"
			MENUITEM "IDD_BUTTBAR_FEATHER"	SEPARATOR
			MENUITEM "ViewColourBar"		
			MENUITEM "IDD_BUTTBAR_STATUS"
			MENUITEM "ViewRulers"
		END
		SUBMENU "WindowQuality"
			MENUITEM "QualityAntiAliased"
			MENUITEM "QualityNormal"
			MENUITEM "QualitySimple"
			MENUITEM "QualityOutline"
		END
		SUBMENU "ShowPrinterColours"	SEPARATOR
			MENUITEM "CancelPreview"
			MENUITEM "CompositePreview"
		END
		MENUITEM "ViewFullScreen"		SEPARATOR
		MENUITEM "ShowGrid"
		MENUITEM "ShowGuides"
		MENUITEM "ShowPrintBorders"   SEPARATOR
		MENUITEM "SnapToGrid"
		MENUITEM "SnapToGuides"
		MENUITEM "SnapToObjects"
	END
	SUBMENU "HelpMenu"
		MENUITEM "HelpIndex"			SEPARATOR
		SUBMENU	 "XaraWebMenu"	
			MENUITEM "HintsAndTipsPages"	
			MENUITEM "XaraXForum"
			MENUITEM "WebsterHomePage" SEPARATOR
			MENUITEM "XaraHomePage" 
		END
		
		MENUITEM "HelpDemos"
		MENUITEM "HelpDemosNative"
		MENUITEM "TipsDlg"			SEPARATOR
		MENUITEM "Update"				SEPARATOR
		MENUITEM "Register"			SEPARATOR
		MENUITEM "AboutDlg"
	END
	SUBMENU "DebugMenu"
		MENUITEM "DebugDlg"
		MENUITEM "ToggleForeBack"		
		MENUITEM "ToggleDoubleBuffer"		SEPARATOR
		MENUITEM "TimeDraw"
		MENUITEM "ClearCache"
		MENUITEM "ThrottleCache"
		MENUITEM "UngroupSpecial"
		MENUITEM "ToggleScrollBars"	SEPARATOR
		MENUITEM "CrashPtr"
		MENUITEM "CrashInt"
		MENUITEM "CrashDbl"
		MENUITEM "CrashRen"	SEPARATOR
		MENUITEM "TimeDraw"
		MENUITEM "GDrawToggle"
		MENUITEM "GBrushToggle"
		MENUITEM "UngroupSpecial"	SEPARATOR
		MENUITEM "RenderDemoDlg"
		MENUITEM "BlobbyDlg"
		MENUITEM "BlobbyTabDlg"
		MENUITEM "CXFTreeDlg"
		MENUITEM "DebugTreeDlg"
		MENUITEM "CreateFontThumbnails"
		MENUITEM "ConvertFileFormats"
		MENUITEM "CMXTreeDlg"
			SEPARATOR
		MENUITEM "TogglePressure"
	END
END
