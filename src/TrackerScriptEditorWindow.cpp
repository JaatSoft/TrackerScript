/*
	Code rewritten by Andrea Anzani (xeD)
	andrea.anzani@gmail.com
	
*/
// TrackerScriptEditorWindow.cpp
// Generated by Interface Elements (Window v2.0) on Apr 25 1998
// This is a user written class and will not be overwritten.

#include "TrackerScriptEditorWindow.h"
#include <kernel/fs_attr.h>
#include <string.h>
#include <Alert.h>
#include <Application.h>
#include <Box.h>
#include <Button.h>
#include <Node.h>
#include <interface/GridLayoutBuilder.h>
#include <interface/GroupLayout.h>
#include <interface/GroupLayoutBuilder.h>
#include <Layout.h>
#include <GridLayout.h>
#include <SpaceLayoutItem.h>
#include <ScrollView.h>

TrackerScriptEditorWindow::TrackerScriptEditorWindow(const entry_ref *e)
	: BWindow(BRect(221.0, 111.0, 617.0, 441.0),"TrackerScript editor window",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS), entry(*e)
{

	// xeD: removed the "Interface Elements" code and added haiku code :=)

	BBox* script_box = new BBox("script box");
	script_box->SetLabel("Script");
	
	fTextView = new BTextView("script");
	BScrollView* scrollView = new BScrollView("scrollview", fTextView, B_WILL_DRAW, false, true);
	
	BButton* revert = new BButton(BRect(294.0, 302.0, 392.0, 328.0) ,"save button","Revert Script",new BMessage('LOAD'),B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	fRunTerminal = new BCheckBox(BRect(4.0, 276.0, 300.0, 295.0)  ,"terminal","View output in Terminal",NULL,B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	
	BBox* divider = new BBox(BRect(0, 0, 1, 1), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, B_FANCY_BORDER);
    divider->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, 1));
	
	
	SetLayout(new BGroupLayout(B_HORIZONTAL));
	script_box->AddChild(BGroupLayoutBuilder(B_HORIZONTAL, 10).Add(scrollView).SetInsets(5, 5, 5, 5));

	AddChild(BGroupLayoutBuilder(B_VERTICAL, 10)
				.Add(script_box, 2)
				.Add(divider)
				.Add(BGridLayoutBuilder(10, 10)
					.Add(fRunTerminal, 0, 0)
					.Add(BSpaceLayoutItem::CreateGlue(), 1, 0)
					.Add(revert, 2 , 0)
					)
				.SetInsets(5, 5, 5, 5)
			);
	
	
	AddShortcut('a',B_COMMAND_KEY,new BMessage(B_ABOUT_REQUESTED));

	fTextView->MakeFocus(true);
	
	Load();
	
	MoveTo(BAlert::AlertPosition(Bounds().Width(), Bounds().Height()));
}

void TrackerScriptEditorWindow::Load()
{
	BNode this_node(&entry);
	attr_info info;
	status_t ret;
	
	// load the script
	ret=this_node.GetAttrInfo("script",&info);
	if (ret==B_OK && info.type==B_STRING_TYPE) {
		char* s = new char[info.size+1];
		this_node.ReadAttr("script", B_STRING_TYPE, 0, s, info.size);
		fTextView->SetText(s);
		delete[] s;
	} else {
		fTextView->SetText("#your script goes here\n");
		fTextView->GoToLine(fTextView->CountLines());
	}
	
	
	// view the output in a Terminal?
	ret=this_node.GetAttrInfo("terminal",&info);
	if (ret==B_OK && info.type==B_INT32_TYPE) {
		int32 b;
		this_node.ReadAttr("terminal", B_INT32_TYPE, 0, &b, sizeof(b));
		fRunTerminal->SetValue(b);
	} else 
		fRunTerminal->SetValue(false);

}

void TrackerScriptEditorWindow::Save()
{
	// save command and script to attributes
	BNode node(&entry);
	const char *text;
	status_t ret;
	
	text=fTextView->Text();
	ret=node.WriteAttr("script",B_STRING_TYPE,0,text,strlen(text)+1);
	if (ret<B_OK) {
		BAlert *alert=new BAlert(
			"Script not saved:",
			strerror(ret),
			"Oops!",
			0,0,
			B_WIDTH_AS_USUAL,
			B_STOP_ALERT);
		alert->Go();
	}
	
	int32 value=fRunTerminal->Value();
	ret=node.WriteAttr("terminal",B_INT32_TYPE,0,&value,sizeof(value));
	if (ret<B_OK) {
		BAlert *alert=new BAlert(
			"Script not saved:",
			strerror(ret),
			"Oops!",
			0,0,
			B_WIDTH_AS_USUAL,
			B_STOP_ALERT);
		alert->Go();
	}
}

TrackerScriptEditorWindow::~TrackerScriptEditorWindow(void)
{
}

bool TrackerScriptEditorWindow::QuitRequested()
{
	Save();
	if (be_app->CountWindows()<=1)
		be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}

void TrackerScriptEditorWindow::MessageReceived(BMessage *message)
{
	if (message->what == B_ABOUT_REQUESTED)
		be_app->PostMessage(B_ABOUT_REQUESTED);
	else if (message->what == 'LOAD') Load();
	else if (message->what == B_SAVE_REQUESTED) Save();
	else BWindow::MessageReceived(message);
}
