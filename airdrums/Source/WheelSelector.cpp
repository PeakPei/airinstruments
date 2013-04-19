//
//  WheelSelector.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/9/13.
//
//

#include "WheelSelector.h"
#include "SkinManager.h"
#include "Main.h"

WheelSelector::WheelSelector(bool left /*= false*/)
: selection(0)
, needsLayout(false)
, trackedFinger(nullptr)
, enabled(false)
, leftHanded(left)
{
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("wheelBg"));
}

WheelSelector::~WheelSelector()
{
    for (Icon* i : icons)
    {
        delete i;
    }
}

void WheelSelector::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(tempBounds);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
    
    for (int i = 0; i < (int) icons.size(); ++i)
    {
        Icon* icon = icons.at(i);
        float aspectRatio = 1.0f;
            const Image& image = icon->getImage();
            if (image.isValid())
                aspectRatio = image.getWidth() / (float)image.getHeight();
        
        float width = b.w/2.f;
        float height = (float)(b.w/2) / aspectRatio;
        icon->setBounds(HUDRect(0,
                             0,
                             width,
                             (GLfloat) (int) height));
    }
    
    layoutIcons();
}

void WheelSelector::updateBounds()
{
    if (fabsf(tempBounds.x - targetBounds.x) < 2)
        tempBounds.x = targetBounds.x;
    else
        tempBounds.x += xStep;
    
    if (fabsf(tempBounds.y - targetBounds.y) < 2)
        tempBounds.y = targetBounds.y;
    else
        tempBounds.y += yStep;
    
    if (fabsf(tempBounds.w - targetBounds.w) < 2)
        tempBounds.w = targetBounds.w;
    else
        tempBounds.w += wStep;
    
    if (fabsf(tempBounds.h - targetBounds.h) < 2)
        tempBounds.h = targetBounds.h;
    else
        tempBounds.h += hStep;
    
    HUDView::setBounds(tempBounds);
}

void WheelSelector::addIcon(Icon *icon)
{
    icons.push_back(icon);
    addChild(icon);
}

void WheelSelector::removeAllIcons()
{
    removeAllChildren();
    for (Icon* i : icons)
    {
        delete i;
    }
    icons.clear();
}

void WheelSelector::layoutIcons()
{
    int N = icons.size();
    
    int first = selection;
    int i = first;
    for (int count = 0; count < N; ++count)
    {        
        Icon* icon = icons.at(i);
        icon->setRotationCoefficient((count/(float)N * 360.f));
        i = (i+1) % N;
    }
}

void WheelSelector::draw()
{
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }
    
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
    fabsf(tempBounds.y - targetBounds.y) > 2 ||
    fabsf(tempBounds.w - targetBounds.w) > 2 ||
    fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);
    
    HUDView::draw();
}

void WheelSelector::setSelection(int sel)
{
    //float direction = sel > selection ? -1.f : 1.f;	// Unused variable
    if (sel < 0) sel = icons.size() - 1;
    if (sel >= (int) icons.size()) sel = 0;
    selection = sel;
    needsLayout = true;
}

void WheelSelector::incSelection(int direction)
{
    selection += direction;
    if (selection < 0) selection = icons.size() - 1;
    if (selection >= (int) icons.size()) selection = 0;
    
    for (Icon* icon : icons)
        icon->rotate(-direction * 360.f / icons.size());
}

void WheelSelector::fingerMotion(float /*x*/, float y, FingerView* fv)
{
    if (fv != trackedFinger)
        return;
    
    if (!enabled) {
        float center = getBounds().y + getBounds().h / 2;
        float distanceFromCenter = y - center;
        if (fabsf(distanceFromCenter) > getBounds().h / 6)
            if (isTimerRunning(kTimerShowControl))
                stopTimer(kTimerShowControl);
        return;
    }
    
	stopTimer(kTimerTrackedFingerMissing);
	startTimer(kTimerTrackedFingerMissing, 100);

    int inc = 0;
    float center = getBounds().y + getBounds().h / 2;
    float distanceFromCenter = y - center;
    if (fabsf(distanceFromCenter) > 75 && y < center)
        inc = 1;
    else if (fabsf(distanceFromCenter) > 75)
        inc = -1;
    
    if (inc != 0 && !isTimerRunning(kTimerSelectionDelay)) {
        incSelection(inc);
        for (Listener* l : listeners)
            l->wheelSelectorChanged(this);
        float multiplier = fabsf(distanceFromCenter*2.f) / getBounds().w;
        startTimer(kTimerSelectionDelay, (int) jmax<float>(250.0f, 500.0f * (1.f-multiplier)));
    }
}

void WheelSelector::fingerEntered(float /*x*/, float y, FingerView* fv)
{
    if (!enabled) {
        float center = getBounds().y + getBounds().h / 2;
        float distanceFromCenter = y - center;
        if (fabsf(distanceFromCenter) < getBounds().h / 6)
            if (!isTimerRunning(kTimerShowControl))
                startTimer(kTimerShowControl,500);
        return;
    }
    
    if (isTimerRunning(kTimerIdle))
        stopTimer(kTimerIdle);
    
    if (!trackedFinger)
        trackedFinger = fv;
}

void WheelSelector::fingerExited(float x, float /*y*/, FingerView* fv)
{
    // !!! Hack to get avoid spurious exits due to corrupt screen coord data
    if (x < 0 || x > 2000)
        return;
    
    if (!enabled && isTimerRunning(kTimerShowControl))
        stopTimer(kTimerShowControl);
    
    if (fv == trackedFinger)
    {
        trackedFinger = NULL;
        startTimer(kTimerIdle, 2000);
    }
}

void WheelSelector::timerCallback(int timerId)
{
    switch (timerId)
    {
        case kTimerSelectionDelay:
            stopTimer(kTimerSelectionDelay);
            break;
        
        case kTimerShowControl:
            enabled = true;
            stopTimer(kTimerShowControl);
            break;
            
        case kTimerIdle:
            enabled = false;
            stopTimer(kTimerIdle);
            break;
        case kTimerTrackedFingerMissing:
            trackedFinger = NULL;
            startTimer(kTimerIdle, 2000);
            stopTimer(kTimerTrackedFingerMissing);
            break;
    }
}

void WheelSelector::addListener(WheelSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter == listeners.end())
        listeners.push_back(listener);
}

void WheelSelector::removeListener(WheelSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter != listeners.end())
        listeners.erase(iter);
}

WheelSelector::Icon::Icon(int inId, bool left /*=false*/)
: id(inId)
, rotationCoeff(0.f)
, targetRotationCoeff(0.f)
, rotationInc(0.f)
, leftHanded(left)
{
}

WheelSelector::Icon::~Icon()
{
}

void WheelSelector::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);
    
    if (fabsf(rotationCoeff - targetRotationCoeff) > 10.f) {
        rotationCoeff += rotationInc;
    }
    else if (rotationCoeff != targetRotationCoeff) {
        rotationCoeff = targetRotationCoeff;
        rotationInc = 0;
    }
    else
        rotationInc = 0;
    
    // sanity check
    if (fabsf(rotationCoeff) >= 360.f)
        rotationCoeff = targetRotationCoeff = 0.f;

    Environment::instance().modelViewMatrix.PushMatrix();
    
    int xTranslate = 0;
    Point<int> about((int) getBounds().w, 0);
    int zAxis = -1;
    if (leftHanded)
    {
        xTranslate = (int) (getParent()->getBounds().w / 2);
        about.x = 0;
        zAxis = 1;
    }
    Environment::instance().modelViewMatrix.Translate((GLfloat) xTranslate, getParent()->getBounds().h / 2, 0.0f);
    Environment::instance().modelViewMatrix.Translate((GLfloat) about.x, 0.0f, 0.0f);
    Environment::instance().modelViewMatrix.Rotate(rotationCoeff, 0.0f, 0.0f, (GLfloat) zAxis);
    Environment::instance().modelViewMatrix.Translate((GLfloat) -about.x, 0.0f, 0.0f);
    Environment::instance().modelViewMatrix.Translate(0.0f, 0.0f, 0.0f);
    HUDView::draw();
    Environment::instance().modelViewMatrix.PopMatrix();
}

void WheelSelector::Icon::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(b);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
}

void WheelSelector::Icon::setRotationCoefficient(float rotation)
{
    rotationCoeff = rotation;
}

void WheelSelector::Icon::rotate(float rotation)
{
    targetRotationCoeff = rotationCoeff + rotation;
    rotationInc = rotation / 10.f;
}

void WheelSelector::Icon::updateBounds()
{
    if (fabsf(tempBounds.x - targetBounds.x) < 2)
        tempBounds.x = targetBounds.x;
    else
        tempBounds.x += xStep;
    
    if (fabsf(tempBounds.y - targetBounds.y) < 2)
        tempBounds.y = targetBounds.y;
    else
        tempBounds.y += yStep;
    
    if (fabsf(tempBounds.w - targetBounds.w) < 2)
        tempBounds.w = targetBounds.w;
    else
        tempBounds.w += wStep;
    
    if (fabsf(tempBounds.h - targetBounds.h) < 2)
        tempBounds.h = targetBounds.h;
    else
        tempBounds.h += hStep;
    
    HUDView::setBounds(tempBounds);
}

void WheelSelector::Icon::setImage(const Image& im)
{
    image = im;
}

const Image& WheelSelector::Icon::getImage() const
{
    return image;
}
