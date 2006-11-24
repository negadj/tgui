//-----------------------------------------------------------------------------
// This source file is part of TGUI (Tiny GUI)
//
// Copyright (c) 2006 Tubras Software, Ltd
// Also see acknowledgements in Readme.html
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
// of the Software, and to permit persons to whom the Software is furnished to 
// do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//-----------------------------------------------------------------------------
#include <tgui.h>

namespace TGUI
{

    //-----------------------------------------------------------------------
    //                           T G C o n t r o l
    //-----------------------------------------------------------------------
    TGControl::TGControl(TGControl *parent)
    {
        x1 = y1 = x2 = y2 = padLeft = padTop = padRight = padBottom = xShift =
            yShift = 0;
        minWidth = minHeight = 0;
        maxWidth = maxHeight = 0x7FFFFFFF;
        name = "";
        m_focusedChild = 0;
        m_parent = parent;
        if (m_parent)
            m_parent->addChild(this);
        performLayout = true;
        mouseOverControl = false;
        popupMenu = NULL;
        exclusiveChild = NULL;
        m_isVisible = true;
        clicked = modified = selected = moved = resized = NULL;
    }

    //-----------------------------------------------------------------------
    //                          ~ T G C o n t r o l
    //-----------------------------------------------------------------------
    TGControl::~TGControl()
    {
        invalidateControl(this);
        removeAllChildren();

        if (m_parent)
        {
            if (m_parent->exclusiveChild == this)
                m_parent->exclusiveChild = NULL;
            m_parent->removeChild(this);
        }

        BSGUI_FREEACTION(clicked);
        BSGUI_FREEACTION(modified);
        BSGUI_FREEACTION(selected);
        BSGUI_FREEACTION(moved);
        BSGUI_FREEACTION(resized);
    }

    //-----------------------------------------------------------------------
    //                          g e t R e n d e r e r
    //-----------------------------------------------------------------------
    TGRenderer* TGControl::getRenderer()
    {
        return TGSystem::getSingleton().getRenderer();
    }


    //-----------------------------------------------------------------------
    //                             s e t N a m e
    //-----------------------------------------------------------------------
    void TGControl::setName(string newName)
    {
        name = newName;
    }

    //-----------------------------------------------------------------------
    //                             f i n d C h i l d
    //-----------------------------------------------------------------------
    TGControl *TGControl::findChild(string name)
    {
        if (name.empty())
            return NULL;
        if (!name.compare(this->name))
            return this;

        for (TGControlListItr itr = m_children.begin();itr != m_children.end(); ++itr)
        {
            TGControl *r = (*itr)->findChild(name);
            if (r)
                return r;
        }

        return NULL;
    }

    //-----------------------------------------------------------------------
    //                             a d d C h i l d 
    //-----------------------------------------------------------------------
    void TGControl::addChild(TGControl *child)
    {
        if (!child)
            return;

        if (child->m_parent)
            child->m_parent->removeChild(child);

        child->m_parent = this;

        m_focusedChild = child;
        m_children.push_back(child);

        performLayout = true;
    }

    //-----------------------------------------------------------------------
    //                          r e m o v e C h i l d
    //-----------------------------------------------------------------------
    void TGControl::removeChild(TGControl *child)
    {
        if (!child || child->m_parent != this)
            return;

        child->m_parent = NULL;

        //child->removeAllChildren();

        m_children.remove(child);

        if(child == m_focusedChild)
        {
            if(m_children.size())
                m_focusedChild = m_children.back();
            else m_focusedChild = NULL;
        }

        performLayout = true;
    }

    //-----------------------------------------------------------------------
    //                       r e m o v e A l l C h i l d r e n
    //-----------------------------------------------------------------------
    void TGControl::removeAllChildren()
    {

        while(m_children.size())
        {
            TGControl* child = *m_children.begin();
            child->removeAllChildren();
            m_children.pop_front();

        }
    }

    //-----------------------------------------------------------------------
    //                              c h i l d A t
    //-----------------------------------------------------------------------
    TGControl *TGControl::childAt(float x, float y)
    {
        int	x1, y1, x2, y2;
        getBounds(x1, y1, x2, y2);
        if (!(x >= x1 + padLeft && y >= y1 + padTop && x <= x2 - padRight &&
            y <= y2 - padBottom))
            return this;

        TGControlListRevItr ritr = m_children.rbegin();

        for (ritr = m_children.rbegin();ritr != m_children.rend(); ++ritr)
        {
            (*ritr)->getBounds(x1,y1,x2,y2);
            if (!(x >= x1 && y >= y1 && x <= x2 && y <= y2))
                continue;
            return (*ritr)->childAt(x, y);
        }

        return this;
    }

    //-----------------------------------------------------------------------
    //                             g e t S c r e e n
    //-----------------------------------------------------------------------
    TGScreen *TGControl::getScreen()
    {
        TGControl*   control = this;
        while (control->m_parent)
            control = control->m_parent;
        return (TGScreen*)control;
    }

    //-----------------------------------------------------------------------
    //                         m a k e E x c l u s i v e
    //-----------------------------------------------------------------------
    void TGControl::makeExclusive()
    {
        if (m_parent)
            m_parent->exclusiveChild = this;
    }

    //-----------------------------------------------------------------------
    //                                t i c k
    //-----------------------------------------------------------------------
    void TGControl::tick()
    {
        if (performLayout)
        {
            layout();
            performLayout = false;
        }
        for (TGControlListItr itr = m_children.begin();itr != m_children.end(); ++itr)
        {
            (*itr)->tick();
        }
    }

    //-----------------------------------------------------------------------
    //                              r e n d e r
    //-----------------------------------------------------------------------
    void TGControl::render()
    {
        int	x1, y1, x2, y2;
        getBounds(x1, y1, x2, y2);

        openClipArea(x1 + padLeft, y1 + padTop, x2 - padRight,
            y2 - padBottom);

        for (TGControlListItr itr = m_children.begin();itr != m_children.end(); ++itr)
        {
            if (*itr == exclusiveChild)
                continue;
            (*itr)->render();
        }

        if (exclusiveChild)
        {
            color(0, 0, 0, 96);
            TGControl::fillRect(x1, y1, x2, y2);

            exclusiveChild->render();
        }

        closeClipArea();
    }

    //-----------------------------------------------------------------------
    //                               f o c u s
    //-----------------------------------------------------------------------
    void TGControl::focus()
    {
        if (!m_parent)
            return;

        m_parent->focus();

        if (m_parent->getLastChild() == this)
            return;

        TGControl *oldFocus = m_parent->getFocusedChild();

        m_parent->setFocusedChild(this);

        if(oldFocus)
            oldFocus->onFocusExit();
        onFocusEnter();
    }

    //-----------------------------------------------------------------------
    //                              f o c u s e d
    //-----------------------------------------------------------------------
    bool TGControl::focused()
    {
        if (!m_parent)
            return true;
        return (m_parent->getFocusedChild() == this && m_parent->focused());
    }

    //-----------------------------------------------------------------------
    //                               p l a c e
    //-----------------------------------------------------------------------
    void TGControl::place(float x1, float y1, float x2, float y2)
    {
        int	oldX1 = this->x1;
        int	oldY1 = this->y1;
        int	oldW = this->x2 - this->x1;
        int	oldH = this->y2 - this->y1;
        if (x2 - x1 + 1 < minWidth)
            x2 = x1 + minWidth - 1;
        if (y2 - y1 + 1 < minHeight)
            y2 = y1 + minHeight - 1;
        if (x2 - x1 + 1 > maxWidth)
            x2 = x1 + maxWidth - 1;
        if (y2 - y1 + 1 > maxHeight)
            y2 = y1 + maxHeight - 1;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        if (x2 - x1 != oldW || y2 - y1 != oldH)
        {
            performLayout = true;
            BSGUI_RUNACTION(resized);
        }
        if (x1 != oldX1 || y1 != oldY1)
            BSGUI_RUNACTION(moved);
    }

    //-----------------------------------------------------------------------
    //                              m o v e
    //-----------------------------------------------------------------------
    void TGControl::move(int x, int y)
    {
        int	dx = x2 - x1, dy = y2 - y1;
        place(x, y, x + dx, y + dy);
    }

    //-----------------------------------------------------------------------
    //                            r e s i z e
    //-----------------------------------------------------------------------
    void TGControl::resize(int width, int height)
    {
        place(x1, y1, x1 + width - 1, y1 + height - 1);
    }

    //-----------------------------------------------------------------------
    //                            c e n t e r
    //-----------------------------------------------------------------------
    void TGControl::center(bool horizontal, bool vertical)
    {
        int	w, h, pw, ph;
        if (!m_parent)
            return;

        m_parent->getClientSize(pw, ph);
        getClientSize(w, h);

        if (horizontal)
            move(pw/2 - w/2, y1);
        if (vertical)
            move(x1, ph/2 - h/2);
    }

    //-----------------------------------------------------------------------
    //                           t r a n s l a t e
    //-----------------------------------------------------------------------
    void TGControl::translate(int &x, int &y)
    {
        for (TGControl *parent = m_parent;parent;parent=parent->m_parent)
        {
            x += parent->x1 + parent->padLeft;
            y += parent->y1 + parent->padTop;
        }
    }

    //-----------------------------------------------------------------------
    //                           g e t B o u n d s
    //-----------------------------------------------------------------------
    void TGControl::getBounds(int &x1, int &y1, int &x2, int &y2)
    {
        x1 = this->x1 - xShift;
        y1 = this->y1 - yShift;
        x2 = this->x2 - xShift;
        y2 = this->y2 - yShift;
        translate(x1, y1);
        translate(x2, y2);
    }

    //-----------------------------------------------------------------------
    //                           s e t P a d d i n g
    //-----------------------------------------------------------------------
    void TGControl::setPadding(int left, int top, int right, int bottom)
    {
        if (left != -1)
            padLeft = left;
        if (top != -1)
            padTop = top;
        if (right != -1)
            padRight = right;
        if (bottom != -1)
            padBottom = bottom;
    }

    //-----------------------------------------------------------------------
    //                         g e t C l i e n t S i z e
    //-----------------------------------------------------------------------
    void TGControl::getClientSize(int &w, int &h)
    {
        w = x2 - x1 - padLeft - padRight + 1;
        h = y2 - y1 - padTop - padBottom + 1;
    }

    //-----------------------------------------------------------------------
    //                          d r a w O w n F r a m e
    //-----------------------------------------------------------------------
    void TGControl::drawOwnFrame()
    {
        int	x1, y1, x2, y2;
        getBounds(x1, y1, x2, y2);
        drawFrame(x1, y1, x2, y2);
    }

    //-----------------------------------------------------------------------
    //                          d r a w O w n F o c u s
    //-----------------------------------------------------------------------
    void TGControl::drawOwnFocus()
    {
        int	x1, y1, x2, y2;
        getBounds(x1, y1, x2, y2);
        color(80, 94, 95);
        drawRect(x1 + 3, y1 + 3, x2 - 3, y2 - 3);
    }

    //-----------------------------------------------------------------------
    //                          o n M o u s e E n t e r
    //-----------------------------------------------------------------------
    void TGControl::onMouseEnter()
    {
        mouseOverControl = true;
    }

    //-----------------------------------------------------------------------
    //                         o n M o u s e M o v e d
    //-----------------------------------------------------------------------
    void TGControl::onMouseMoved(int x, int y)
    {
    }

    //-----------------------------------------------------------------------
    //                          o n M o u s e E x i t
    //-----------------------------------------------------------------------
    void TGControl::onMouseExit()
    {
        mouseOverControl = false;
    }

    //-----------------------------------------------------------------------
    //                          o n M o u s e D o w n
    //-----------------------------------------------------------------------
    void TGControl::onMouseDown(int x, int y, int b)
    {
        if (b == LeftButton)
        {
            setKeyboardFocusControl(this);
            focus();
            BSGUI_RUNACTION(clicked);
        }
        if (b == RightButton)
        {
            if (popupMenu)
                popupMenu->run();
        }
    }

    //-----------------------------------------------------------------------
    //                           o n M o u s e U p
    //-----------------------------------------------------------------------
    void TGControl::onMouseUp(int x, int y, int b)
    {
    }

    //-----------------------------------------------------------------------
    //                            o n K e y D o w n
    //-----------------------------------------------------------------------
    void TGControl::onKeyDown(int key, unsigned char ascii)
    {
    }

    //-----------------------------------------------------------------------
    //                              o n K e y U p
    //-----------------------------------------------------------------------
    void TGControl::onKeyUp(int key, unsigned char ascii)
    {
    }

    //-----------------------------------------------------------------------
    //                s e t M o u s e T r a c k i n g C o n t r o l
    //-----------------------------------------------------------------------
    void TGControl::setMouseTrackingControl(TGControl *control)
    {
        TGSystem::getSingleton().setMouseTrackingControl(control);
    }

    //-----------------------------------------------------------------------
    //              s e t K e y b o a r d F o c u s C o n t r o l
    //-----------------------------------------------------------------------
    void TGControl::setKeyboardFocusControl(TGControl *control)
    {
        TGSystem::getSingleton().setKeyboardFocusControl(control);
    }

    //-----------------------------------------------------------------------
    //                    h a s K e y b o a r d F o c u s
    //-----------------------------------------------------------------------
    bool TGControl::hasKeyboardFocus(TGControl *control)
    {
        return TGSystem::getSingleton().hasKeyboardFocus(control);
    }

    //-----------------------------------------------------------------------
    //                  i n v a l i d a t e C o n t r o l
    //-----------------------------------------------------------------------
    void TGControl::invalidateControl(TGControl *control)
    {
        TGSystem::getSingleton().invalidateControl(control);
    }

    //-----------------------------------------------------------------------
    //                     g e t A c t i v e S c r e e n
    //-----------------------------------------------------------------------
    TGScreen* TGControl::getActiveScreen()
    {
        return TGSystem::getSingleton().getActiveScreen();
    }

    //-----------------------------------------------------------------------
    //                                c o l o r
    //-----------------------------------------------------------------------
    void TGControl::color(int r, int g, int b, int a)
    {
        gColor.r = r/255.f;
        gColor.g = g/255.f;
        gColor.b = b/255.f;
        gColor.a = a/255.f;
    }

    //-----------------------------------------------------------------------
    //                            d r a w R e c t
    //-----------------------------------------------------------------------
    void TGControl::drawRect(int x1, int y1, int x2, int y2,int thickness)
    {
        if(!m_isVisible)
            return;

        drawLine(x1,y1,x2,y1,thickness);
        drawLine(x2,y1,x2,y2,thickness);
        drawLine(x1,y2,x2,y2,thickness);
        drawLine(x1,y1,x1,y2,thickness);
        /*
        fillRect(x1,y1,x2,y1+thickness);
        fillRect(x1,y1,x1+thickness,y2);
        fillRect(x2-thickness,y1,x2,y2);
        fillRect(x1,y2-thickness,x2,y2);
        */
    }

    //-----------------------------------------------------------------------
    //                            f i l l R e c t
    //-----------------------------------------------------------------------
    void TGControl::fillRect(int x1, int y1, int x2, int y2)
    {
        if(!m_isVisible)
            return;
        TGRect r(x1,y1,x2,y2);
        TGColourRect cr(gColor);
        TGSystem::getSingleton().getRenderer()->addQuad(r,0,TGSystem::getSingleton().getDefaultTexture(),
            r,cr,TopLeftToBottomRight);
    }


    //-----------------------------------------------------------------------
    //                            d r a w L i n e
    //-----------------------------------------------------------------------
    void TGControl::drawLine(int x1, int y1, int x2, int y2,int thickness)
    {
        if(!m_isVisible)
            return;
        TGRect r(x1,y1,x2,y2);
        TGColourRect cr(gColor);

        int xdir= (x2-x1) < 0 ? -1 : 1;
        int ydir= (y2-y1) < 0 ? -1 : 1;

        TGSystem::getSingleton().getRenderer()->addLine(r,0,TGSystem::getSingleton().getDefaultTexture(),
            r,cr,TopLeftToBottomRight,thickness);
    }

    //-----------------------------------------------------------------------
    //                            d r a w F r a m e
    //-----------------------------------------------------------------------
    void TGControl::drawFrame(int x1, int y1, int x2, int y2, FrameStyle s,int thickness)
    {
        if(!m_isVisible)
            return;
        color(100, 114, 115);
        if (!s)
        {
            fillRect(x1, y1, x2, y2);
            return;
        }
        fillRect(x1, y1, x2, y2);
        switch (s)
        {
        case FS_FLAT:
            color(170, 184, 185);
            drawRect(x1, y1, x2, y2);
            break;
        case FS_RAISED:
            color(210, 224, 225);
            drawRect(x1, y1, x2, y2);
            break;
        case FS_LOWERED:
            color(70, 84, 85);
            drawRect(x1, y1, x2, y2);
            break;
        default:
            break;
        }
    }

    //-----------------------------------------------------------------------
    //                           d r a w S t r i n g
    //-----------------------------------------------------------------------
    void TGControl::drawString(int x, int y, string str, int length)
    {
        if(!m_isVisible)
            return;
        TGFont* font = TGSystem::getSingleton().getCurrentFont();
        if(!font)
            return;

        int cHeight=font->getHeight();

        float	cx = x;
        if (length == -1)
            length = (int)str.length();

        for (int i=0;i<length;i++)
        {
            char ch=str[i];
            if (ch == ' ')
            {
                cx += 5;
                continue;
            }

            int x2,y2;
            int cWidth = font->m_font->getGlyphAspectRatio(ch) * cHeight;
            x2 = cx + cWidth;
            y2 = y+cHeight;


            TGRect r(cx,y,x2,y2);
            TGColourRect cr(gColor);
            TGRect ruv;

            font->m_font->getGlyphTexCoords(ch,ruv.d_left,ruv.d_top,ruv.d_right,ruv.d_bottom);

            TGSystem::getSingleton().getRenderer()->addQuad(r,0,font->m_texture,ruv,cr,TopLeftToBottomRight);

            cx += cWidth + 1.0f;
        }
    }



    //-----------------------------------------------------------------------
    //                          s t r i n g W i d t h
    //-----------------------------------------------------------------------
    int TGControl::stringWidth(string str, size_t length)
    {
        TGFont* font = TGSystem::getSingleton().getCurrentFont();
        if(!font)
            return 0;
        float	cx = 0;
        if (length == -1)
            length = str.length();

        for (size_t i=0;i<length;i++)
        {
            if (str[i] == ' ')
            {
                cx += 5;
                continue;
            }
            int cWidth = font->m_font->getGlyphAspectRatio(str[i]) * font->getHeight();
            cx += cWidth + 1;
        }
        return (int)cx;
    }

    //-----------------------------------------------------------------------
    //                          s t r i n g H e i g h t
    //-----------------------------------------------------------------------
    int TGControl::stringHeight()
    {
        TGFont* font = TGSystem::getSingleton().getCurrentFont();
        if(!font)
            return 0;
        return font->getHeight();
    }

    //-----------------------------------------------------------------------
    //                            o p e n C l i p
    //-----------------------------------------------------------------------
    void TGControl::openClip()
    {
        int	x1, y1, x2, y2;
        getBounds(x1, y1, x2, y2);
        openClipArea(x1, y1, x2, y2);
    }

    //-----------------------------------------------------------------------
    //                           c l o s e C l i p
    //-----------------------------------------------------------------------
    void TGControl::closeClip()
    {
        closeClipArea();
    }

    //-----------------------------------------------------------------------
    //                       r e s e t C l i p p i n g
    //-----------------------------------------------------------------------
    void TGControl::resetClipping()
    {
        getRenderer()->resetClipping();

    }

    //-----------------------------------------------------------------------
    //                         o p e n C l i p A r e a
    //-----------------------------------------------------------------------
    void TGControl::openClipArea(int x1, int y1, int x2, int y2)
    {
        getRenderer()->openClipArea(x1,y1,x2,y2);

    }

    //-----------------------------------------------------------------------
    //                          c l o s e C l i p A r e a
    //-----------------------------------------------------------------------
    void TGControl::closeClipArea()
    {
        getRenderer()->closeClipArea();
    }

    //-----------------------------------------------------------------------
    //                        g e t F i r s t C h i l d
    //-----------------------------------------------------------------------
    TGControl* TGControl::getFirstChild()
    {
        return m_children.front();
    }

    //-----------------------------------------------------------------------
    //                        g e t L a s t C h i l d
    //-----------------------------------------------------------------------
    TGControl* TGControl::getLastChild()
    {
        return m_children.back();
    }

    //-----------------------------------------------------------------------
    //                     s e t F o c u s e d C h i l d
    //-----------------------------------------------------------------------
    void TGControl::setFocusedChild(TGControl* child)
    {
        m_focusedChild = child;

        //
        // put the "focused" child at the back of the list
        //
        for (TGControlListItr itr = m_children.begin();itr != m_children.end(); ++itr)
        {
            TGControl *c = *itr;
            if (c == child)
            {
                m_children.erase(itr);
                m_children.push_back(c);
                break;

            }
        }
    }


}