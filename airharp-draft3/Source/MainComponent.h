/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_115FADC5__
#define __MAINCOMPONENT_H_115FADC5__

#include "HarpHUD.h"
#include "StringView.h"
#include "HarpView.h"
#include "TutorialSlide.h"
#include "SettingsScreen.h"

#include "../JuceLibraryCode/JuceHeader.h"



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public OpenGLRenderer,
                               public ChangeListener,
                               public Leap::Listener,
                               public MultiTimer,
                               public ActionListener,
                               public MessageListener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    // component overrides
    void paint (Graphics&);
    void resized();
    void focusGained(FocusChangeType cause);
    void focusLost(FocusChangeType cause);
    void mouseMove(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    bool keyPressed(const KeyPress& kp);
    
    // OpenGLRenderer overrides
    void newOpenGLContextCreated();
    void renderOpenGL();
    void openGLContextClosing();
    
    // ChangeListener override
    void changeListenerCallback (ChangeBroadcaster* source);
    
    // Leap::Listener override
    virtual void onFrame(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFocusGained (const Leap::Controller &);
    virtual void onFocusLost (const Leap::Controller &);
    
    void timerCallback(int timeId);
    
    enum TimerIds
    {
        kTimerShowTutorial = 0,
        kTimerWaitingForConnection,
        kFullscreenTipTimer
    };

    class InitGLMessage : public Message {};
    
    void handleMessage(const juce::Message &m);

    void actionListenerCallback(const String& message);
    
    void showFullscreenTip();
private:
    void go2d();
    void go3d();
    void layoutStrings();
    void layoutChordRegions();
    void showTutorial();
    bool chordRegionsNeedUpdate;
    
    void handleTapGesture(const Leap::Pointable& p);
    
    OpenGLContext openGLContext;
    
    ScopedPointer<TutorialSlide> tutorial;
    HarpToolbar* toolbar;
    StatusBar* statusBar;
    SettingsScreen* settingsScreen;
    HUDView* leapDisconnectedView;
    ScopedPointer<HUDView> fullscreenTipView;
    View2d* backgroundView;
    std::vector<ChordRegion*> chordRegions;
    std::vector<HarpView*> harps;
    std::vector<HarpView*> inactiveHarps;
    std::vector<HUDView*> views;

    View2d fingersImage;
    int bloomShaderId;
    int shaderId;
    
    bool sizeChanged;
    
    Image splashBgImage;
    Image splashTitleImage;
    Image splashLogoImage;
    Image splashImage;
    ScopedPointer<View2d> splashBgView;
    ScopedPointer<View2d> splashTitleView;
    
    Time lastFrame;
    Time startTime;
    
    bool connected;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__
