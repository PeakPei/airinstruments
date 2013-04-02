/*
  ==============================================================================

    This file was auto-generated by the Introjucer!
	(and heavily hand edited afterwards)

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "Main.h"
#include "KitManager.h"


AirHarpApplication::AirHarpApplication()
{
}


void AirHarpApplication::initialise (const String& /*commandLine*/)
{
    // This method is where you should put your application's initialisation code..
    
	PropertiesFile::Options options;
	options.applicationName = "AirBeats";
	options.filenameSuffix = ".settings";
	options.folderName = "AirBeats";
	options.osxLibrarySubFolder = "Application Support";
	options.commonToAllUsers = "false";
	options.ignoreCaseOfKeyNames = true;
	options.millisecondsBeforeSaving = 0;
	options.storageFormat = PropertiesFile::storeAsXML;
	properties.setStorageParameters(options);

    mainWindow = new MainWindow();
	#if JUCE_WINDOWS
		mainWindow->setMenuBar(&mainMenu);
	#elif JUCE_MAC
		MenuBarModel::setMacMainMenu(&mainMenu);
	#endif

	XmlElement* audioState = properties.getUserSettings()->getXmlValue(AudioSettingsDialog::getPropertiesName());
    audioDeviceManager.initialise (0, 2, audioState, true, String::empty, 0);
	if (audioState != nullptr)
		delete audioState;
    //audioDeviceManager.addAudioCallback(this);
    audioSourcePlayer.setSource (&Drums::instance());
    audioDeviceManager.addAudioCallback (&audioSourcePlayer);
    Logger::outputDebugString(audioDeviceManager.getCurrentAudioDevice()->getName());
}

void AirHarpApplication::shutdown()
{
    // Add your application's shutdown code here..

	if (settingsDialog != nullptr)
		delete settingsDialog;

    audioDeviceManager.removeAudioCallback (&audioSourcePlayer);
	MotionDispatcher::destruct();
	KitManager::Destruct();
	#if JUCE_MAC
		MenuBarModel::setMacMainMenu(nullptr);
	#endif

    mainWindow = nullptr; // (deletes our window)
    //audioDeviceManager.removeAudioCallback(this);
}

//==============================================================================
void AirHarpApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
}

void AirHarpApplication::audioDeviceAboutToStart (AudioIODevice* /*device*/)
{
        
}
void AirHarpApplication::audioDeviceStopped()
{
        
}
void AirHarpApplication::audioDeviceIOCallback (const float** /*inputChannelData*/, int /*numInputChannels*/,
                            float** /*outputChannelData*/, int /*numOutputChannels*/, int /*numSamples*/)
{
}
    
void AirHarpApplication::anotherInstanceStarted (const String& /*commandLine*/)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

bool AirHarpApplication::perform (const InvocationInfo &info)
{
	switch(info.commandID)
	{
		default :
		{
			bool x = false;
			jassert(x);
			break;
		}

		case 1 :
		{
			if (settingsDialog != nullptr)
				break;

			settingsDialog = new AudioSettingsDialog(mainWindow, audioDeviceManager, properties);

			break;
		}
	}

	return true;
}

//==============================================================================
AirHarpApplication::MainWindow::MainWindow()  :
								DocumentWindow ("MainWindow",
                                Colours::lightgrey,
                                DocumentWindow::allButtons)
{
    setContentOwned (new MainContentComponent(), true);

    centreWithSize (getWidth(), getHeight());
    setVisible (true);
    setUsingNativeTitleBar(true);
    setResizable(true, false);
}

void AirHarpApplication::MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AirHarpApplication)
