#include "Main.h"
#include "MainMenu.h"

MainMenu::MainMenu()
{
}


MainMenu::~MainMenu()
{
}


StringArray  MainMenu::getMenuBarNames ()
{
	StringArray	menuBarNames;
    
	menuBarNames.add("File");
	menuBarNames.add("Options");
    
	return menuBarNames;
}


PopupMenu  MainMenu::getMenuForIndex (int topLevelMenuIndex, const String& /*menuName*/)
{
	PopupMenu menu;
    ApplicationCommandManager* mgr = AirHarpApplication::getInstance()->getCommandManager();
	switch (topLevelMenuIndex)
	{
		default :
		{
			jassertfalse;
			break;
		}
            
		case kFileMenu :
        {            
			break;
        }
		case kOptionsMenu :
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kAudioSettingsCmd, "Audio Settings...");
			break;
	}
    
    
	return menu;
}


void  MainMenu::menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/)
{
}