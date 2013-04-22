//
//  CursorView.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/21/13.
//
//

#include "CursorView.h"
#include "MotionServer.h"

CursorView::CursorView()
: targetX(0.f)
, targetY(0.f)
, enabled(true)
{
    
}

void CursorView::draw()
{
    if (!enabled)
        return;

    if (getBounds().x != targetX)
        setBounds(HUDRect(targetX, getBounds().y, getBounds().w, getBounds().h));
    if (getBounds().y != targetY)
        setBounds(HUDRect(getBounds().x, targetY, getBounds().w, getBounds().h));
    
    View2d::draw();
}

CursorView::~CursorView()
{
    
}

void CursorView::setX(float x)
{
    targetX = x;
}

void CursorView::setY(float y)
{
    targetY = y;
}

void CursorView::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
}

CursorView::Listener::Listener()
{
    MotionDispatcher::instance().cursorViewListeners.push_back(this);
}

CursorView::Listener::~Listener()
{
    auto i = std::find(MotionDispatcher::instance().cursorViewListeners.begin(), MotionDispatcher::instance().cursorViewListeners.end(), this);
    if (i != MotionDispatcher::instance().cursorViewListeners.end())
        MotionDispatcher::instance().cursorViewListeners.erase(i);
}
