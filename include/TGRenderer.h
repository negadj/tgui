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
#ifndef __TGRENDERER_H__
#define __TGRENDERER_H__

namespace TGUI
{

    class TGTexture;
    class TGRenderer;
    class TGRQListener : public Ogre::RenderQueueListener
    {
    public:
        TGRQListener(TGRenderer* renderer, Ogre::uint8 queue_id, bool post_queue)
        {
            d_renderer		= renderer;
            d_queue_id		= queue_id;
            d_post_queue	= post_queue;
        }

        virtual ~TGRQListener() {}

        virtual void	renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue);
        virtual void	renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue);

        void	setTargetRenderQueue(Ogre::uint8 queue_id)		{d_queue_id = queue_id;}
        void	setPostRenderQueue(bool post_queue)		{d_post_queue = post_queue;}

    private:
        TGRenderer*             d_renderer;     // TGUI renderer object for Ogre.
        Ogre::uint8	            d_queue_id;     // ID of the queue that we are hooked into
        bool                    d_post_queue;   // true if we render after everything else in our queue.
    };

    struct ClipArea
    {
        int		x1;
        int		y1;
        int		x2;
        int		y2;
    };

    typedef std::list<ClipArea*> TGClipList;

    class TGRenderer 
    {
        unsigned int    texture;
        void*           rendererListEntry;
        bool            enabled;
        TGClipList      m_clipList;

    public:
        TGRenderer(Ogre::RenderWindow* window,
            Ogre::uint8 queue_id = Ogre::RENDER_QUEUE_OVERLAY,
            bool post_queue = false);

        TGRenderer(Ogre::RenderWindow* window, Ogre::uint8 queue_id, bool post_queue, Ogre::SceneManager* scene_manager);

        virtual ~TGRenderer();

        void openClipArea(int x1, int y1, int x2, int y2);
        void closeClipArea();
        void resetClipping();


        virtual	void	addQuad(const TGRect& dest_rect, float z, const TGTexture* tex, const TGRect& texture_rect, const TGColourRect& colours, TGQuadSplitMode quad_split_mode);
        virtual	void	addLine(const TGRect& dest_rect, float z, const TGTexture* tex, const TGRect& texture_rect, const TGColourRect& colours, TGQuadSplitMode quad_split_mode,int thickness);
        virtual	void	doRender(void);
        virtual	void	clearRenderList(void);


        virtual void	setQueueingEnabled(bool setting)		{d_queueing = setting;}
        virtual	TGTexture*	createTexture(void);
        virtual	TGTexture*	createTexture(const string& filename, const string& resourceGroup = "General");
        virtual	TGTexture*	createTexture(float size);
        virtual	void		destroyTexture(TGTexture* texture);
        virtual void		destroyAllTextures(void);


        virtual bool	isQueueingEnabled(void) const	{return d_queueing;}
        virtual float	getWidth(void) const		{return d_display_area.getWidth();}
        virtual float	getHeight(void) const		{return d_display_area.getHeight();}
        virtual TGSize	getSize(void) const			{return d_display_area.getSize();}
        virtual TGRect	getRect(void) const			{return d_display_area;}
        virtual	uint	getMaxTextureSize(void) const		{return 2048;}		// TODO: Change to proper value
        virtual	uint	getHorzScreenDPI(void) const	{return 96;}
        virtual	uint	getVertScreenDPI(void) const	{return 96;}
        void	setTargetSceneManager(Ogre::SceneManager* scene_manager);
        void	setTargetRenderQueue(Ogre::uint8 queue_id, bool post_queue);
        TGTexture*	createTexture(Ogre::TexturePtr& texture);
        void	setDisplaySize(const TGSize& sz);
        void	resetZValue(void)				{d_current_z = GuiZInitialValue;}
        void	advanceZValue(void)				{d_current_z -= GuiZElementStep;}
        float	getCurrentZ(void) const			{return d_current_z;}
        float	getZLayer(uint layer) const		{return d_current_z - ((float)layer * GuiZLayerStep);}
    private:
        static const float	GuiZInitialValue;       // Initial value to use for 'z' each frame.
        static const float	GuiZElementStep;        // Value to step 'z' for each GUI element.
        static const float	GuiZLayerStep;          // Value to step 'z' for each GUI layer.


        float	d_current_z;		//!< The current z co-ordinate value.

        static const size_t    VERTEX_PER_QUAD;     // number of vertices per quad
        static const size_t    VERTEX_PER_TRIANGLE;	// number of vertices for a triangle
        static const size_t    VERTEXBUFFER_INITIAL_CAPACITY; // initial capacity of the allocated vertex buffer
        static const size_t    UNDERUSED_FRAME_THRESHOLD;     // number of frames to wait before shrinking buffer

        struct QuadVertex {
            float x, y, z;                          // The position for the vertex.
            Ogre::RGBA diffuse;                     // colour of the vertex
            float tu1, tv1;                         // texture coordinates
        };

        struct QuadInfo
        {
            bool                isLineQuad;
            Ogre::TexturePtr		texture;
            TGRect				position;
            TGRect              position2;
            QuadVertex          lpos[6];
            float				z;
            TGRect				texPosition;
            uint32		        topLeftCol;
            uint32		        topRightCol;
            uint32		        bottomLeftCol;
            uint32		        bottomRightCol;

            TGQuadSplitMode		splitMode;

            bool operator<(const QuadInfo& other) const
            {
                // this is intentionally reversed.
                return z > other.z;
            }
        };

        void	initRenderStates(void);

        void	sortQuads(void);

        bool clipQuad(ClipArea* clip, TGRect& drect, TGRect& tRect, TGColourRect colours);

        void renderQuadDirect(const TGRect& dest_rect, float z, const TGTexture* tex, const TGRect& texture_rect, const TGColourRect& colours, TGQuadSplitMode quad_split_mode);

        uint32 colourToOgre(const TGColour& col) const;

        void constructor_impl(Ogre::RenderWindow* window, Ogre::uint8 queue_id, bool post_queue);


        TGRect				d_display_area;

        typedef std::multiset<QuadInfo>		QuadList;
        QuadList d_quadlist;
        bool	 d_queueing;		                    // setting for queueing control.

        // Ogre specific bits.
        Ogre::Root*                 d_ogre_root;		// pointer to the Ogre root object that we attach to
        Ogre::RenderSystem*         d_render_sys;		// Pointer to the render system for Ogre.
        Ogre::uint8	                d_queue_id;			// ID of the queue that we are hooked into
        Ogre::TexturePtr            d_currTexture;		// currently set texture;
        Ogre::RenderOperation       d_render_op;		// Ogre render operation we use to do our stuff.
        Ogre::HardwareVertexBufferSharedPtr	d_buffer;	// vertex buffer to queue sprite rendering
        size_t                      d_underused_framecount;                  //!< Number of frames elapsed since buffer utilization was above half the capacity
        Ogre::RenderOperation       d_direct_render_op;	// Renderop for cursor
        Ogre::HardwareVertexBufferSharedPtr	d_direct_buffer;	//!< Renderop for cursor
        Ogre::SceneManager*	        d_sceneMngr;		// The scene manager we are hooked into.
        Ogre::LayerBlendModeEx      d_colourBlendMode;	// Controls colour blending mode used.
        Ogre::LayerBlendModeEx      d_alphaBlendMode;	// Controls alpha blending mode used.
        Ogre::TextureUnitState::UVWAddressingMode d_uvwAddressMode;

        TGRQListener*			    d_ourlistener;
        bool                        d_post_queue;		// true if we render after everything else in our queue.
        size_t                      d_bufferPos;		// index into buffer where next vertex should be put.
        bool                        d_sorted;			// true when data in quad list is sorted.
        TGPoint                     d_texelOffset;		// Offset required for proper texel mapping.

        std::list<TGTexture*>       d_texturelist;		// List used to track textures.

    };
}

#endif