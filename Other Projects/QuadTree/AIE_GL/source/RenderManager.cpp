#include "RenderManager.h"
#include "Texture.h"
#include <assert.h>

#include "glfw3.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "Font.h"

static char gs_vertexShader[] =
"	#version 330																				\r\n"
"																								\r\n"
"	in vec3 a_position;																			\r\n"
"	in vec4 a_color;																			\r\n"
"	in vec2 a_texcoord;																			\r\n"
"																								\r\n"
"	uniform mat4 u_projection;																	\r\n"
"																								\r\n"
"	out vec2 v_texcoord;																		\r\n"
"	out vec4 v_color;																			\r\n"
"	out float v_textureID;																		\r\n"
"																								\r\n"
"	void main()																					\r\n"
"	{																							\r\n"
"		v_textureID     = a_position.z;															\r\n"
"		v_color			= a_color;																\r\n"
"		v_texcoord	    = a_texcoord;															\r\n"
"																								\r\n"
"		gl_Position     = u_projection * vec4(a_position.x, a_position.y, 0.0, 1.0);			\r\n"
"	}																							\r\n"
"   ";


static char gs_fragmentShader[] = 
"	#version 330																						\r\n"
"																										\r\n"
"	in vec2 v_texcoord;																					\r\n"
"	in vec4 v_color;																					\r\n"
"	in float v_textureID;																				\r\n"
"																										\r\n"
"	uniform sampler2D	u_texture0;																		\r\n"
"	uniform sampler2D	u_texture1;																		\r\n"
"	uniform sampler2D	u_texture2;																		\r\n"
"	uniform sampler2D	u_texture3;																		\r\n"
"	uniform sampler2D	u_texture4;																		\r\n"
"																										\r\n"
"																										\r\n"
"	void main()																							\r\n"
"	{																									\r\n"
"	    if      ( v_textureID == 0.0 ) gl_FragColor = texture2D(u_texture0, v_texcoord.st) * v_color;	\r\n"
"	    else if ( v_textureID == 1.0 ) gl_FragColor = texture2D(u_texture1, v_texcoord.st) * v_color;	\r\n"
"	    else if ( v_textureID == 2.0 ) gl_FragColor = texture2D(u_texture2, v_texcoord.st) * v_color;	\r\n"
"	    else if ( v_textureID == 3.0 ) gl_FragColor = texture2D(u_texture3, v_texcoord.st) * v_color;	\r\n"
"	    else if ( v_textureID == 4.0 ) gl_FragColor = texture2D(u_texture4, v_texcoord.st) * v_color;	\r\n"
"	    else gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);													\r\n"
"	}"
"	";

//----------------------------------------------------------------------------------------
// helper function defined below
// defined static in global scope so that its only accessable to this file
//----------------------------------------------------------------------------------------
static void RotateAround(float inX, float inY, float &outX, float &outY, float sin, float cos);

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
RenderManager* RenderManager::m_pInstance = nullptr;

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
RenderManager::RenderManager(float fViewWidth, float fViewHeight)
{
	m_fScreenWidth = fViewWidth;
	m_fScreenHeight = fViewHeight;

	SetRenderColor(255,255,255,255);
	SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);
	unsigned int pixels[1] = {0xFFFFFFFF};
	m_pNullTexture = new Texture(1,1, pixels);
	m_bTransformColumnMajor = false;

	m_nCurrentVert			= 0;
	m_nCurrentIndex			= 0;
	m_bProcessingRender		= false;

	m_vao					= -1;
	m_vbo					= -1;
	m_ibo					= -1;

	m_uCurrentTextureID		= 0;
	m_uMaxTextures			= 5;

	for(int i = 0; i < 5; i++)
	{
		m_apActiveTextures[i] = nullptr;
	}

	// load the shader
	const char* aszInputs[] =  { "a_position", "a_color", "a_texcoord" };
	const char* aszOutputs[] = { "out_color" };

	// load shader internally calls glCreateShader...
	GLuint vshader = LoadShader(gs_vertexShader, GL_VERTEX_SHADER);
	GLuint pshader = LoadShader(gs_fragmentShader, GL_FRAGMENT_SHADER);

	m_uShader = CreateProgram( vshader, 0, 0, 0, pshader, 3, aszInputs, 1, aszOutputs);

	glDeleteShader( vshader ); 
	glDeleteShader( pshader );

	m_uUniformTextureLoc = glGetUniformLocation(m_uShader, "u_texture");

	// pre calculate the indices... they will always be the same
	int index = 0;
	for(int i=0; i<3072;)
	{
		m_auIndices[i++] = (index + 0);
		m_auIndices[i++] = (index + 1);
		m_auIndices[i++] = (index + 2);

		m_auIndices[i++] = (index + 0);
		m_auIndices[i++] = (index + 2);
		m_auIndices[i++] = (index + 3);
		index += 4;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	// create the vao, vio and vbo
	glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
	
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);
	
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3072 * sizeof(unsigned short), (void *)(&m_auIndices[0]), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 2048 * sizeof(SBVertex), m_aVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
          
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)28);

	glBindVertexArray(0);

	m_fCameraX = 0.0f;
	m_fCameraY = 0.0f;

	GetOrtho(m_afProjection, m_fCameraX, m_fCameraX + m_fScreenWidth, m_fCameraY + m_fScreenHeight, m_fCameraY, 0.0f, 100.0f);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
RenderManager::~RenderManager()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vao);

	glDeleteProgram(m_uShader);

	delete m_pNullTexture;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::SetRenderColor(unsigned int uColour)
{
	m_r = ((uColour & 0xFF000000) >> 24 );
	m_g = ((uColour & 0x00FF0000) >> 16);
	m_b = ((uColour & 0x0000FF00) >> 8 );
	m_a = ((uColour & 0x000000FF) >> 0 );
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::SetRenderColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_r = r;
	m_b = b;
	m_g = g;
	m_a = a;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::SetUVRect(float uvX, float uvY, float uvW, float uvH)
{
	m_uvX = uvX;
	m_uvY = uvY;
	m_uvW = uvW;
	m_uvH = uvH;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::SetCameraPos(float x, float y)
{
	m_fCameraX = x;
	m_fCameraY = y;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::GetCameraPos(float* outX, float* outY)
{
	*outX = m_fCameraX;
	*outY = m_fCameraY;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::GetViewRect(float* outMinX, float* outMinY, float* outMaxX, float* outMaxY)
{
	*outMinX = m_fCameraX;
	*outMinY = m_fCameraY;
	*outMaxX = m_fCameraX + m_fScreenWidth;
	*outMaxY = m_fCameraY + m_fScreenHeight;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::FlushBatch()
{
    // dont render anything
    if (m_nCurrentVert == 0 || m_nCurrentIndex == 0 || m_bProcessingRender == false)
        return;

	int textureIDs[5] = { 0, 1, 2, 3, 4 };
	glUniform1iv( m_uUniformTextureLoc, 5, textureIDs);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
          
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_nCurrentVert * sizeof(SBVertex), m_aVertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_nCurrentIndex * sizeof(unsigned short), m_auIndices);
          
    glDrawElements(GL_TRIANGLES, m_nCurrentIndex, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// clear the active textures
	for(unsigned int i = 0; i < m_uCurrentTextureID; i++)
		m_apActiveTextures[i] = nullptr;

	// reset vertex, index and texture count...
    m_nCurrentIndex		= 0;
    m_nCurrentVert		= 0;
    m_uCurrentTextureID	= 0;

	// incroment the number of flushes preformed for this batch.
	m_nNumFlushes += 1;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::Begin()
{
	m_bProcessingRender  = true;
	m_nCurrentIndex      = 0;
	m_nCurrentVert       = 0;
	m_uCurrentTextureID	= 0;
	m_nNumFlushes		= 0;

	GetOrtho(m_afProjection, m_fCameraX, m_fCameraX + m_fScreenWidth, m_fCameraY + m_fScreenHeight, m_fCameraY, -1.0f, 100.0f);

	glUseProgram( m_uShader );

	glUniformMatrix4fv( glGetUniformLocation(m_uShader, "u_projection"), 1, false, m_afProjection );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetRenderColor(0xFFFFFFFF);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::End()
{
	if (!m_bProcessingRender)
		return;

	FlushBatch();

	glUseProgram(0);

	m_bProcessingRender = false;

	// resetup the projection and view
	GetOrtho(m_afProjection, m_fCameraX, m_fCameraX + m_fScreenWidth, m_fCameraY + m_fScreenHeight, m_fCameraY, -1.0f, 100.0f);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::DrawSprite( Texture *texture, float xPos, float yPos, float width, float height, float rotation, float xOrigin, float yOrigin)
{
	if( texture == NULL ) texture = m_pNullTexture;

	if( ShouldFlush() ) FlushBatch();
	unsigned int textureID = PushTexture( texture );

	if( width  == 0.0f )	width  = (float)texture->GetWidth();
	if( height == 0.0f )	height = (float)texture->GetHeight();

	float tlX = (0.0f - xOrigin) * width;		float tlY = (0.0f - yOrigin) * height;
	float trX = (1.0f - xOrigin) * width;		float trY = (0.0f - yOrigin) * height;
	float brX = (1.0f - xOrigin) * width;		float brY = (1.0f - yOrigin) * height;
	float blX = (0.0f - xOrigin) * width;		float blY = (1.0f - yOrigin) * height;

	if( rotation != 0.0f )
	{
		float si = sinf(rotation); 
		float co  = cosf(rotation);
		RotateAround(tlX, tlY, tlX, tlY, si, co);
		RotateAround(trX, trY, trX, trY, si, co);
		RotateAround(brX, brY, brX, brY, si, co);
		RotateAround(blX, blY, blX, blY, si, co);
	}

	int index = m_nCurrentVert;

	float r = m_r / 255.0f;
	float g = m_g / 255.0f;
	float b = m_b / 255.0f;
	float a = m_a / 255.0f;

	m_aVertices[m_nCurrentVert].pos[0] = xPos + tlX; 
	m_aVertices[m_nCurrentVert].pos[1] = yPos + tlY; 
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX; 
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY + m_uvH; 
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = xPos + trX; 
	m_aVertices[m_nCurrentVert].pos[1] = yPos + trY; 
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX + m_uvW;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY + m_uvH;
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = xPos + brX;
	m_aVertices[m_nCurrentVert].pos[1] = yPos + brY;
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX + m_uvW;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY;
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = xPos + blX;
	m_aVertices[m_nCurrentVert].pos[1] = yPos + blY;
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY;
	m_nCurrentVert++;
	//-------------------

	m_auIndices[m_nCurrentIndex++] = (index + 0);
	m_auIndices[m_nCurrentIndex++] = (index + 2);
	m_auIndices[m_nCurrentIndex++] = (index + 3);
	
	m_auIndices[m_nCurrentIndex++] = (index + 0);
	m_auIndices[m_nCurrentIndex++] = (index + 1);
	m_auIndices[m_nCurrentIndex++] = (index + 2);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::DrawSpriteTransformed3x3(Texture *texture, float *transformMat3x3, float width, float height, float xOrigin, float yOrigin)
{
	if( texture == NULL ) texture = m_pNullTexture;

	if( ShouldFlush() ) FlushBatch();
	unsigned int textureID = PushTexture( texture );

	if( width  == 0.0f )	width  = (float)texture->GetWidth();
	if( height == 0.0f )	height = (float)texture->GetHeight();

	float tlX = (0.0f - xOrigin) * width;		float tlY = (0.0f - yOrigin) * height;
	float trX = (1.0f - xOrigin) * width;		float trY = (0.0f - yOrigin) * height;
	float brX = (1.0f - xOrigin) * width;		float brY = (1.0f - yOrigin) * height;
	float blX = (0.0f - xOrigin) * width;		float blY = (1.0f - yOrigin) * height;

	// transform the points by the matrix
	//	0	1	2
	//	3	4	5
	//	6	7	8
	//--------------------------------------------------------------------------
	if( m_bTransformColumnMajor )
	{
		float x, y;

		x = tlX; y=tlY;
		tlX = x * transformMat3x3[0] + y * transformMat3x3[1] + transformMat3x3[2];
		tlY = x * transformMat3x3[3] + y * transformMat3x3[4] + transformMat3x3[5];

		x = trX; y=trY;
		trX = x * transformMat3x3[0] + y * transformMat3x3[1] + transformMat3x3[2];
		trY = x * transformMat3x3[3] + y * transformMat3x3[4] + transformMat3x3[5];

		x = brX; y=brY;
		brX = x * transformMat3x3[0] + y * transformMat3x3[1] + transformMat3x3[2];
		brY = x * transformMat3x3[3] + y * transformMat3x3[4] + transformMat3x3[5];

		x = blX; y=blY;
		blX = x * transformMat3x3[0] + y * transformMat3x3[1] + transformMat3x3[2];
		blY = x * transformMat3x3[3] + y * transformMat3x3[4] + transformMat3x3[5];
	}
	else
	{
		float x, y;

		x = tlX; y=tlY;
		tlX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
		tlY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];

		x = trX; y=trY;
		trX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
		trY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];

		x = brX; y=brY;
		brX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
		brY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];

		x = blX; y=blY;
		blX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
		blY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	}
	//--------------------------------------------------------------------------

	int index = m_nCurrentVert;

	float r = m_r / 255.0f;
	float g = m_g / 255.0f;
	float b = m_b / 255.0f;
	float a = m_a / 255.0f;

	m_aVertices[m_nCurrentVert].pos[0] = tlX; 
	m_aVertices[m_nCurrentVert].pos[1] = tlY; 
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX; 
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY + m_uvH; 
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = trX; 
	m_aVertices[m_nCurrentVert].pos[1] = trY; 
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX + m_uvW;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY + m_uvH;
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = brX;
	m_aVertices[m_nCurrentVert].pos[1] = brY;
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX + m_uvW;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY;
	m_nCurrentVert++;
	
	m_aVertices[m_nCurrentVert].pos[0] = blX;
	m_aVertices[m_nCurrentVert].pos[1] = blY;
	m_aVertices[m_nCurrentVert].pos[2] = (float)textureID;
	m_aVertices[m_nCurrentVert].color[0] = r;
	m_aVertices[m_nCurrentVert].color[1] = g;
	m_aVertices[m_nCurrentVert].color[2] = b;
	m_aVertices[m_nCurrentVert].color[3] = a;
	m_aVertices[m_nCurrentVert].texcoord[0] = m_uvX;
	m_aVertices[m_nCurrentVert].texcoord[1] = m_uvY;
	m_nCurrentVert++;
	//-------------------

	m_auIndices[m_nCurrentIndex++] = (index + 0);
	m_auIndices[m_nCurrentIndex++] = (index + 2);
	m_auIndices[m_nCurrentIndex++] = (index + 3);
	
	m_auIndices[m_nCurrentIndex++] = (index + 0);
	m_auIndices[m_nCurrentIndex++] = (index + 1);
	m_auIndices[m_nCurrentIndex++] = (index + 2);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::DrawLine(float x1, float y1, float x2, float y2, float thickness)
{
	float xDiff = x2 - x1;
	float yDiff = y2 - y1;
	float len = sqrt( xDiff * xDiff + yDiff * yDiff );
	float xDir = xDiff / len;
	float yDir = yDiff / len;

	float rot = atan2( yDir, xDir );

	float uvX = m_uvX;
	float uvY = m_uvY;
	float uvW = m_uvW;
	float uvH = m_uvH;

	SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);

	DrawSprite(m_pNullTexture, x1, y1, len, thickness, rot, 0.0f, 0.5f);

	SetUVRect(uvX, uvY, uvW, uvH);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void RenderManager::DrawText(Font *pFont, const char *szText, float xPos, float yPos, float xOrigin, float yOrigin)
{
	int len = strlen(szText);
	float lineHeight = pFont->GetLineHeight();

	float X = xPos;
	float Y = yPos;

	float uvX = m_uvX;
	float uvY = m_uvY;
	float uvW = m_uvW;
	float uvH = m_uvH;

	float strLenPx = 0.0f;

	if (xOrigin != 0.0f)
	{
		for (int i = 0; i < len; ++i)
		{
			const Font::CharInfo *charInfo = pFont->GetCharInfo(szText[i]);
			strLenPx += charInfo->xAdvance;
		}
	}

	for(int i = 0; i < len; ++i)
	{
		if( szText[i] == '\n' )
		{
			X = xPos;
			Y += lineHeight;
			continue;
		}

		const Font::CharInfo *charInfo = pFont->GetCharInfo(szText[i]);
		
		float x = X + charInfo->xOffset - pFont->GetKerning(szText[i - 1], szText[i]);
		float y = Y + charInfo->yOffset;
		float w = charInfo->width;
		float h = charInfo->height;

		// if the width or height is 0,
		// than this is not a printable character...
		if( w <= 0.0f || h <= 0.0f )
			continue;

		Font::Rect uvRect = pFont->GetCharacterTexCoord(charInfo);

		SetUVRect(uvRect.x, uvRect.y, uvRect.w, uvRect.h);
		DrawSprite(pFont->GetFontBitmap(), x, y, w, h, 0.0f, 0, 0);

		X += charInfo->xAdvance;
	}

	SetUVRect(uvX, uvY, uvW, uvH);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
bool RenderManager::ShouldFlush()
{
	return m_nCurrentVert >= 2048;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
unsigned int RenderManager::PushTexture(Texture *texture)
{
	// check if the texture is already in use
	// if so, return... we dont need to add it to our list of active txtures again
	for(unsigned int i = 0; i <= m_uCurrentTextureID; i++)
	{
		if( m_apActiveTextures[i] == texture )
			return i;
	}

	// if we've used all the textures we can, than we need to flush to make room for another texture change
	if (m_uCurrentTextureID >= m_uMaxTextures - 1)
		FlushBatch();

	// add the texture to our active texture list
	m_apActiveTextures[m_uCurrentTextureID] = texture;

	// tell the shader which texture to use.
	char texAttrib[32];
	sprintf_s(texAttrib, 32, "u_texture%i", m_uCurrentTextureID);
	glUniform1i( glGetUniformLocation(m_uShader, texAttrib), m_uCurrentTextureID);

	glActiveTexture(GL_TEXTURE0 + m_uCurrentTextureID);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureHandle());
	glActiveTexture(GL_TEXTURE0);

	// incronment current texture for the next PushTexture() to use.
	m_uCurrentTextureID += 1;

	// return what the current texture was.
	return m_uCurrentTextureID - 1;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
unsigned int RenderManager::LoadShader(const char* source, unsigned int a_type)
{
	int success = GL_FALSE;

	unsigned int handle = glCreateShader(a_type);

	glShaderSource(handle, 1, &source, 0);
	glCompileShader(handle);

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	return handle;
}

//----------------------------------------------------------------------------------------
// creates a shader program, links the specified shader stages to it, and binds the specified input/output attributes
//----------------------------------------------------------------------------------------
unsigned int RenderManager::CreateProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader, unsigned int a_fragmentShader,
								  unsigned int a_inputAttributeCount /* = 0 */, const char** a_inputAttributes /* = nullptr */,
								  unsigned int a_outputAttributeCount /* = 0 */, const char** a_outputAttributes /* = nullptr */)
{
	int success = GL_FALSE;

	// create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, a_vertexShader);
	glAttachShader(handle, a_controlShader);
	glAttachShader(handle, a_evaluationShader);
	glAttachShader(handle, a_geometryShader);
	glAttachShader(handle, a_fragmentShader);

	// specify vertex input attributes
	for ( unsigned int i = 0; i < a_inputAttributeCount; ++i )
		glBindAttribLocation(handle, i, a_inputAttributes[i]);

	// link the program together and log errors
	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	return handle;
}

//----------------------------------------------------------------------------------------
// this function returns a new position that has been rotated around the origion.
//----------------------------------------------------------------------------------------
void RotateAround(float inX, float inY, float &outX, float &outY, float sin, float cos)
{
    outX = inX * cos - inY * sin;
    outY = inX * sin + inY * cos;
}