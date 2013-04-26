//
//  ButtonBar.h
//  AirBeats
//
//  Created by Adam Somers on 4/25/13.
//
//

#ifndef __AirBeats__ButtonBar__
#define __AirBeats__ButtonBar__

#include "HUD.h"
#include "TextButton.h"

class ButtonBar : public HUDView
                , public HUDButton::Listener
{
public:
    ButtonBar();
    
    // HUDView override
    void setBounds(const HUDRect &b);
    void layoutButtons();
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);
    
    void showButtons(bool shouldShow);
    
private:
    TextHUDButton clearPatternButton;
    TextHUDButton clearLeftTrackButton;
    TextHUDButton clearRightTrackButton;
};

#endif /* defined(__AirBeats__ButtonBar__) */
