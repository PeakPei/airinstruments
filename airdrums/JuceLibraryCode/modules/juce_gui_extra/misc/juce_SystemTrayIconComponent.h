/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef __JUCE_SYSTEMTRAYICONCOMPONENT_JUCEHEADER__
#define __JUCE_SYSTEMTRAYICONCOMPONENT_JUCEHEADER__

#if JUCE_WINDOWS || JUCE_LINUX || JUCE_MAC || DOXYGEN


//==============================================================================
/**
    On Windows and Linux only, this component sits in the taskbar tray as a small icon.

    To use it, just create one of these components, but don't attempt to make it
    visible, add it to a parent, or put it on the desktop.

    You can then call setIconImage() to create an icon for it in the taskbar.

    To change the icon's tooltip, you can use setIconTooltip().

    To respond to mouse-events, you can override the normal mouseDown(),
    mouseUp(), mouseDoubleClick() and mouseMove() methods, and although the x, y
    position will not be valid, you can use this to respond to clicks. Traditionally
    you'd use a left-click to show your application's window, and a right-click
    to show a pop-up menu.
*/
class JUCE_API  SystemTrayIconComponent  : public Component
{
public:
    //==============================================================================
    SystemTrayIconComponent();

    /** Destructor. */
    ~SystemTrayIconComponent();

    //==============================================================================
    /** Changes the image shown in the taskbar.
    */
    void setIconImage (const Image& newImage);

    /** Changes the tooltip that Windows shows above the icon. */
    void setIconTooltip (const String& tooltip);

   #if JUCE_LINUX
    /** @internal */
    void paint (Graphics&) override;
   #endif

private:
    //==============================================================================
    JUCE_PUBLIC_IN_DLL_BUILD (class Pimpl)
    ScopedPointer<Pimpl> pimpl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemTrayIconComponent)
};


#endif
#endif   // __JUCE_SYSTEMTRAYICONCOMPONENT_JUCEHEADER__
