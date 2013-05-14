/*
  ==============================================================================

    DrumItem.h
    Created: 30 Mar 2013 12:15:36pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMITEM_H_87D00F4E__
#define __DRUMITEM_H_87D00F4E__

#include "../JuceLibraryCode/JuceHeader.h"


// This is the base class from which DrumKit and DrumPattern derive.
class DrumItem
{
public:
	enum Status
	{
		kNoError = 0,
		kItemLoadError = 1,
		kItemEmptyError = 2,
		kItemSaveError = 3,
		kItemNameError = 4,
		kItemUuidError = 5,
		kNoKitError = 6,
		kSampleRateError = 7,
		kSamplePositionError = 8,
		kMidiDataError = 9
	};

	~DrumItem();

	// Other accessors

	String&	GetName(void);
	Uuid&	GetUuid(void);
	void	SetName(String& name);
	void	SetUuid(Uuid uuid);

	bool	GetModifiable(void) const;
	void	SetModifiable(bool modifiable);
	bool	GetDirty(void) const;
	void	SetDirty(bool dirty);
	bool	GetHasValidName(void) const;

	Status	LoadFromXml(XmlElement* element);
	Status	SaveToXml(XmlElement* element);

protected:
	DrumItem();
	DrumItem(const DrumItem& source);	// Copy construct a modifiable item from the source, new Uuid is generated, name is empty

private:
	String	mName;
	Uuid	mUuid;
	bool	mModifiable;	// Factory items are not modifiable, only user items are modifiable.
							// To modify a factory item, make a copy of it as a user item.
	bool	mDirty;
	bool	mHasValidName;	// New items do not have a valid name until a Save As is performed
};

#endif  // __DRUMITEM_H_87D00F4E__
