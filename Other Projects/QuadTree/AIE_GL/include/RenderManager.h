//----------------------------------------------------------------------------------------
// Simple 2D sprite renderer.
// Author: Richard Stern
// Based on Spritebatch system by Aaron Cox
//----------------------------------------------------------------------------------------

#pragma once

class Texture;
class Font;

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class RenderManager
{
public:
	static RenderManager* Instance()							{ return m_pInstance; }
	static void Create(float fViewWidth, float fViewHeight)		{ m_pInstance = new RenderManager(fViewWidth, fViewHeight);}
	static void Destroy()										{ delete m_pInstance; }

	//----------------------------------------------------------------------------------------
	// Draw calls below can only be processed between the begin() and end() function calls.
	//----------------------------------------------------------------------------------------
	void Begin();
	void End();

	//----------------------------------------------------------------------------------------
	// Draw a texture at the specified position, size, origin
	//----------------------------------------------------------------------------------------
	void DrawSprite(Texture *pTexture, float fPosX, float fPosY, float fWidth = 0.0f, float fHeight = 0.0f, float fRotation = 0.0f, float fOriginX = 0.5f, float fOriginY = 0.5f);

	//----------------------------------------------------------------------------------------
	// Draw a texture that is transformed by a matrix (a pointer to the beginning of 9 floats)
	// the width and height are scailed by the matrix.
	//----------------------------------------------------------------------------------------
	void DrawSpriteTransformed3x3(Texture *pTexture, float *transformMat3x3, float fWidth = 0.0f, float fHeight = 0.0f, float fOriginX = 0.5f, float fOriginY = 0.5f);

	//----------------------------------------------------------------------------------------
	// Draw a string of text.
	// new line character '\n' is supported.
	// xOrigin and yOrigin can be used to specify if the font should be left, right or center aligned.
	//----------------------------------------------------------------------------------------
	void DrawText(Font *pFont, const char *szText, float fPosX, float fPosY, float fOriginX = 0, float fOriginY = 0);

	//----------------------------------------------------------------------------------------
	// Draws a white line from x1,y1 to x2,y2 with the specified thickness.
	//----------------------------------------------------------------------------------------
	void DrawLine(float x1, float y1, float x2, float y2, float fThickness = 1.0f);

	//----------------------------------------------------------------------------------------
	// Change the tint color for rendering
	//----------------------------------------------------------------------------------------
	void SetRenderColor(unsigned int uColour);
	void SetRenderColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	//----------------------------------------------------------------------------------------
	// Specify the uv coordinate 
	//----------------------------------------------------------------------------------------
	void SetUVRect(float uvX, float uvY, float uvW, float uvH);

	//----------------------------------------------------------------------------------------
	// If set to true, the DrawSpriteTransformed3x3 & DrawSpriteTransformed4x4 functions
	// will be treeted as column major, otherwise, its treeted as row major
	//----------------------------------------------------------------------------------------
	void SetColumnMajor(bool bColumnMajor) {m_bTransformColumnMajor = bColumnMajor; };
	
	//----------------------------------------------------------------------------------------
	// Specify camera position
	//----------------------------------------------------------------------------------------
	void SetCameraPos(float x, float y);
	void GetCameraPos(float* outX, float* outY);
	void GetViewRect(float* outMinX, float* outMinY, float* outMaxX, float* outMaxY);

private:
	// Private constructor due to singleton
	RenderManager(float fViewWidth, float fViewHeight);
	~RenderManager();

	static RenderManager* m_pInstance;

	// sprite batch vertex structure
	// 4 of these are created for each sprite rendered to the screen
	// they are stored in the m_vertices array below.
	struct SBVertex
    {
		SBVertex()
		{
			pos[0]			= pos[1]			= pos[2]						= 0.0f;
			color[0]		= color[1]			= color[2]		= color[3]		= 1.0f;
			texcoord[0]		= texcoord[1]										= 0.0f;
		}

		SBVertex(float x, float y, float z, float r, float g, float b, float a, float u, float v)
		{
			pos[0]		= x;		pos[1]		= y;	pos[2]		= z;
			color[0]	= r;		color[1]	= g;	color[2]	= g;	color[3] = a;
			texcoord[0] = u;		texcoord[1] = v;
		}

        float  pos[3];			// 12 bytes - offset 0
        float  color[4];		// 16 bytes - offset 12
        float texcoord[2];		// 8 bytes - offset 28

        // sizeof SBVertex 36
    };

	// Represents red, green, blue, alpha in the range of 0 - 255
	unsigned char m_r, m_g, m_b, m_a;

	// Represents UV rectangle x, y, width, height in the range of 0.0f to 1.0f
	float m_uvX, m_uvY, m_uvW, m_uvH; 

	// If set to true, the DrawSpriteTransformed3x3 function
	// will be treeted as column major, otherwise, its treeted as row major
	bool m_bTransformColumnMajor;

	// If the texture specified is invalid, this 1px white texture
	// will be used instead
	Texture *m_pNullTexture;

	// Camera position
	float m_fCameraX;
	float m_fCameraY;

	// Screen size
	float m_fScreenWidth;
	float m_fScreenHeight;

	// helper function
	// returns true if the m_vertices buffer is full
	// returns true if rendering more than m_maxTextures for the batch
	bool ShouldFlush();

	// helper function
	// when rendering a sprite, use the texture
	// this will add the texture to m_activeTextures
	// these are mapped to glActiveTexture(...) so that you can render
	// more than one texture per batch
	unsigned int PushTexture(Texture *pTexture);
	
	// helper function
	// renders m_vertices to the backbuffer with glDrawElements
	// resets m_vertices, m_indices, m_activeTextuers for rendering a new batch
	void FlushBatch();
	bool IsProcessingRender() { return m_bProcessingRender; }

	// index into the m_vertices array - there are 2048 vertices
	// this keeps track of how many have been used.
	// refers to the next available vertex
	int m_nCurrentVert;

	// index into the m_indices array - there are 3072 indices
	// this keeps track of how many have been used.
	// refers to the next available indicy
    int m_nCurrentIndex;
    bool m_bProcessingRender;

    SBVertex m_aVertices[2048];			// 4 verts per sprite = 512 sprites
    unsigned short m_auIndices[3072];	// 3 per triangle, 6 per quad = 6 * 512 sprites = 3072 indices required

	// used to keep track of which textures have been used per batch
	// evert time PushTexture is called, if the texture passed in does not exist
	// it is added. If the size of m_activeTextures exceeds m_maxTextures, ShouldFlush()
	// will return true
	Texture *m_apActiveTextures[5];

	// refers to the shader program used for this sprite batch
    unsigned int m_uShader;
	unsigned int m_uUniformTextureLoc;

	// vertex array object, vertex buffer object and index buffer object
	// used for openGL rendering
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ibo;

	// refers to the currentTextureID
	// used to calculate the which glActiveTexture( GL_TEXTURE0 + m_currentTextureID )
	// incromented when a unique texture is passed to PushTexture(...)
    unsigned int m_uCurrentTextureID;

	// constant arbitary number
	// our shader supports 5 sampler2D objects
    unsigned int m_uMaxTextures;

	// orthographic projection matrix.
	float m_afProjection[16];

	int m_nNumFlushes;

	unsigned int LoadShader(const char* a_filename, unsigned int a_type);

	unsigned int CreateProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader, unsigned int a_fragmentShader,
									  unsigned int a_inputAttributeCount = 0, const char** a_inputAttributes = 0,
									  unsigned int a_outputAttributeCount = 0, const char** a_outputAttributes = 0);

	void GetOrtho(float *mat4x4, float left, float right, float bottom, float top, float a_fNear, float a_fFar)
	{
		
		mat4x4[0] = 2.0f / (right - left);;
		mat4x4[1] = 0.0f;
		mat4x4[2] = 0.0f;
		mat4x4[3] = 0.0f;

		mat4x4[4] = 0.0f;
		mat4x4[5] = 2.0f / (top - bottom);
		mat4x4[6] = 0.0f;
		mat4x4[7] = 0.0f;
		
		mat4x4[8] = 0.0f;
		mat4x4[9] = 0.0f;
		mat4x4[10] = 2.0f / (a_fFar - a_fNear);
		mat4x4[11] = 0.0f;

		mat4x4[12] = -1.0f * ((right + left) / (right - left));
		mat4x4[13] = -1.0f * ((top + bottom) / (top - bottom));
		mat4x4[14] = -1.0f * ((a_fFar + a_fNear) / (a_fFar - a_fNear));
		mat4x4[15] = 1.0f;
	}
};

