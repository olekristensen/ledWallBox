
#include "ofConstants.h"
#include "ofSystemUtils.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofAppRunner.h"

#ifdef TARGET_WIN32
#include <winuser.h>
#include <commdlg.h>
#define _WIN32_DCOM

#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>

#endif

#ifdef TARGET_OSX
	// ofSystemUtils.cpp is configured to build as
	// objective-c++ so as able to use Cocoa dialog panels
	// This is done with this compiler flag
	//		-x objective-c++
	// http://www.yakyak.org/viewtopic.php?p=1475838&sid=1e9dcb5c9fd652a6695ac00c5e957822#p1475838

	#include <Cocoa/Cocoa.h>
#endif

#ifdef TARGET_WIN32
#include <locale>
#include <sstream>
#include <string>

std::string convertWideToNarrow( const wchar_t *s, char dfault = '?',
                      const std::locale& loc = std::locale() )
{
  std::ostringstream stm;

  while( *s != L'\0' ) {
    stm << std::use_facet< std::ctype<wchar_t> >( loc ).narrow( *s++, dfault );
  }
  return stm.str();
}

std::wstring convertNarrowToWide( const std::string& as ){
    // deal with trivial case of empty string
    if( as.empty() )    return std::wstring();

    // determine required length of new string
    size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0 );

    // construct new string of required length
    std::wstring ret( reqLength, L'\0' );

    // convert old string to new string
    ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length() );

    // return new string ( compiler should optimize this away )
    return ret;
}

#endif

#if defined( TARGET_OSX )
static void restoreAppWindowFocus(){
	NSWindow * appWindow = (NSWindow *)ofGetCocoaWindow();
	if(appWindow) {
		[appWindow makeKeyAndOrderFront:nil];
	}
}
#endif

#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
#include <gtk/gtk.h>
#include "ofGstUtils.h"

static void initGTK(){
	static bool initialized = false;
	if(!initialized){
		ofGstUtils::startGstMainLoop();
	    gdk_threads_init();
	int argc=0; char **argv = NULL;
	gtk_init (&argc, &argv);
		initialized = true;
	}

}

static string gtkFullScreenFileDialog(GtkFileChooserAction action,string windowTitle,string defaultName=""){
	initGTK();
	string results;

	const gchar* button_name = "";
	switch(action){
	case GTK_FILE_CHOOSER_ACTION_OPEN:
		button_name = GTK_STOCK_OPEN;
		break;
	case GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER:
		button_name = GTK_STOCK_SELECT_ALL;
		break;
	case GTK_FILE_CHOOSER_ACTION_SAVE:
		button_name = GTK_STOCK_SAVE;
		break;
	default:
		return "";
		break;
	}

	GtkWidget *dialog = gtk_file_chooser_dialog_new (windowTitle.c_str(),
						  NULL,
						  action,
						  button_name, GTK_RESPONSE_ACCEPT,
						  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						  NULL);

	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),defaultName.c_str());

	gdk_threads_enter();
	gtk_window_maximize(GTK_WINDOW (dialog));
	//gtk_window_set_keep_above(GTK_WINDOW (dialog), true);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		results = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	}
	gtk_widget_destroy (dialog);
	gdk_threads_leave();
	//gtk_dialog_run(GTK_DIALOG(dialog));
	//startGTK(dialog);
	return results;
}

#endif
#ifdef TARGET_ANDROID
#include "ofxAndroidUtils.h"
#endif

//------------------------------------------------------------------------------
ofFileDialogResult::ofFileDialogResult(){
	filePath = "";
	fileName = "";
	bSuccess = false;
}

//------------------------------------------------------------------------------
string ofFileDialogResult::getName(){
	return fileName;
}

//------------------------------------------------------------------------------
string ofFileDialogResult::getPath(){
	return filePath;
}

//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32
//---------------------------------------------------------------------
static int CALLBACK loadDialogBrowseCallback(
  HWND hwnd,
  UINT uMsg,
  LPARAM lParam,
  LPARAM lpData
){
    string defaultPath = *(string*)lpData;
    if(defaultPath!="" && uMsg==BFFM_INITIALIZED){
		wchar_t         wideCharacterBuffer[MAX_PATH];
		wcscpy(wideCharacterBuffer, convertNarrowToWide(ofToDataPath(defaultPath)).c_str());
        SendMessage(hwnd,BFFM_SETSELECTION,1,(LPARAM)wideCharacterBuffer);
    }

	return 0;
}
//----------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------

// OS specific results here.  "" = cancel or something bad like can't load, can't save, etc...
ofFileDialogResult fullScreenLoadDialog(string windowTitle, bool bFolderSelection, string defaultPath){

	ofFileDialogResult results;

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------       OSX
	//----------------------------------------------------------------------------------------
#ifdef TARGET_OSX

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSOpenPanel * loadDialog = [NSOpenPanel openPanel];
	[loadDialog setAllowsMultipleSelection:NO];
	[loadDialog setCanChooseDirectories:bFolderSelection];
	[loadDialog setResolvesAliases:YES];

	if(!windowTitle.empty()) {
		[loadDialog setTitle:[NSString stringWithUTF8String:windowTitle.c_str()]];
	}

	if(!defaultPath.empty()) {
		NSString * s = [NSString stringWithUTF8String:defaultPath.c_str()];
		s = [[s stringByExpandingTildeInPath] stringByResolvingSymlinksInPath];
		NSURL * defaultPathUrl = [NSURL fileURLWithPath:s];
		[loadDialog setDirectoryURL:defaultPathUrl];
	}

	NSInteger buttonClicked = [loadDialog runModal];
	restoreAppWindowFocus();

	if(buttonClicked == NSFileHandlingPanelOKButton) {
		NSURL * selectedFileURL = [[loadDialog URLs] objectAtIndex:0];
		results.filePath = string([[selectedFileURL path] UTF8String]);
	}
	[pool drain];

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32
	wstring windowTitleW;
	windowTitleW.assign(windowTitle.begin(), windowTitle.end());

	if (bFolderSelection == false){

        OPENFILENAME ofn;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		HWND hwnd = WindowFromDC(wglGetCurrentDC());
		ofn.hwndOwner = hwnd;
#ifdef __MINGW32_VERSION
		char szFileName[MAX_PATH];
        memset(szFileName,0,260);
		if(defaultPath!=""){
            strcpy(szFileName,ofToDataPath(defaultPath).c_str());
		}

		ofn.lpstrFilter = "All\0";
		ofn.lpstrFile = szFileName;
#else // Visual Studio
		wchar_t szFileName[MAX_PATH];
		wchar_t szTitle[MAX_PATH];
		if(defaultPath!=""){
			wcscpy_s(szFileName,convertNarrowToWide(ofToDataPath(defaultPath)).c_str());
		}else{
		    //szFileName = L"";
			memset(szFileName,  0, sizeof(szFileName));
		}

		if (windowTitle != "") {
			wcscpy_s(szTitle, convertNarrowToWide(windowTitle).c_str());
			ofn.lpstrTitle = szTitle;
		} else {
			ofn.lpstrTitle = NULL;
		}

		ofn.lpstrFilter = L"All\0";
		ofn.lpstrFile = szFileName;
#endif
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = 0;

#ifdef __MINGW32_VERSION
		ofn.lpstrTitle = windowTitle.c_str();
#else
		ofn.lpstrTitle = windowTitleW.c_str();
#endif

		if(GetOpenFileName(&ofn)) {
#ifdef __MINGW32_VERSION
			results.filePath = string(szFileName);
#else
			results.filePath = convertWideToNarrow(szFileName);
#endif

		}

	} else {

		BROWSEINFOW      bi;
		wchar_t         wideCharacterBuffer[MAX_PATH];
		wchar_t			wideWindowTitle[MAX_PATH];
		LPITEMIDLIST    pidl;
		LPMALLOC		lpMalloc;

		if (windowTitle != "") {
			wcscpy(wideWindowTitle, convertNarrowToWide(windowTitle).c_str());
		} else {
			wcscpy(wideWindowTitle, L"Select Directory");
		}

		// Get a pointer to the shell memory allocator
		if(SHGetMalloc(&lpMalloc) != S_OK){
			//TODO: deal with some sort of error here?
		}
		bi.hwndOwner        =   NULL;
		bi.pidlRoot         =   NULL;
		bi.pszDisplayName   =   wideCharacterBuffer;
		bi.lpszTitle        =   wideWindowTitle;
		bi.ulFlags          =   BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bi.lpfn             =   &loadDialogBrowseCallback;
		bi.lParam           =   (LPARAM) &defaultPath;
		bi.lpszTitle        =   windowTitleW.c_str();

		if(pidl = SHBrowseForFolderW(&bi)){
			// Copy the path directory to the buffer
			if(SHGetPathFromIDListW(pidl,wideCharacterBuffer)){
				results.filePath = convertWideToNarrow(wideCharacterBuffer);
			}
			lpMalloc->Free(pidl);
		}
		lpMalloc->Release();
	}

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#endif




	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   linux
	//----------------------------------------------------------------------------------------
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
		if(bFolderSelection) results.filePath = gtkFullScreenFileDialog(GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,windowTitle,defaultPath);
		else results.filePath = gtkFullScreenFileDialog(GTK_FILE_CHOOSER_ACTION_OPEN,windowTitle,defaultPath);
#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------



	if( results.filePath.length() > 0 ){
		results.bSuccess = true;
		results.fileName = ofFilePath::getFileName(results.filePath);
	}

	return results;
}



ofFileDialogResult fullScreenSaveDialog(string defaultName, string messageName){

	ofFileDialogResult results;

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------       OSX
	//----------------------------------------------------------------------------------------
#ifdef TARGET_OSX

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSSavePanel * saveDialog = [NSSavePanel savePanel];
	[saveDialog setMessage:[NSString stringWithUTF8String:messageName.c_str()]];
	[saveDialog setNameFieldStringValue:[NSString stringWithUTF8String:defaultName.c_str()]];

	NSInteger buttonClicked = [saveDialog runModal];
	restoreAppWindowFocus();

	if(buttonClicked == NSFileHandlingPanelOKButton){
		results.filePath = string([[[saveDialog URL] path] UTF8String]);
	}
	[pool drain];

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32


	wchar_t fileName[MAX_PATH] = L"";
	char * extension;
	OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	HWND hwnd = WindowFromDC(wglGetCurrentDC());
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.nMaxFileTitle = 31;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"";	// we could do .rxml here?
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrTitle = L"Select Output File";

	if (GetSaveFileNameW(&ofn)){
		results.filePath = convertWideToNarrow(fileName);
	}

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   linux
	//----------------------------------------------------------------------------------------
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)

	results.filePath = gtkFullScreenFileDialog(GTK_FILE_CHOOSER_ACTION_SAVE, messageName,defaultName);

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	if( results.filePath.length() > 0 ){
		results.bSuccess = true;
		results.fileName = ofFilePath::getFileName(results.filePath);
	}

	return results;
}

#ifdef TARGET_WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //switch(msg)
    //{
    //    case WM_CLOSE:
    //        DestroyWindow(hwnd);
    //    break;
    //    case WM_DESTROY:
    //        PostQuitMessage(0);
    //    break;
    //    default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    //}
}
#endif
