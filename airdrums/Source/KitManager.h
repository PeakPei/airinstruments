/*
  ==============================================================================

    KitManager.h
    Created: 25 Mar 2013 1:39:31pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __KITMANAGER_H_C9AB6D2C__
#define __KITMANAGER_H_C9AB6D2C__

#include "DrumKit.h"
#include "ItemManager.h"

#include "../JuceLibraryCode/JuceHeader.h"

class KitManager : public ItemManager<KitManager, DrumKit>
{
public:
    // call me from OpenGL thread only!
    void LoadTextures();
	Status	BuildKitList(StringArray paths, bool clear = true);
    Status	BuildKitList();
    const TextureDescription& getTextureAtlas() const;

private:
	friend class ItemManager<KitManager, DrumKit>;
	KitManager();
	~KitManager();
};

#endif  // __KITMANAGER_H_C9AB6D2C__
