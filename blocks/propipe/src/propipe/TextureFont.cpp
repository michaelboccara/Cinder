#include "TextureFont.h"
#include "Renderer.h"
#include "cinder/Text.h"

using namespace std;

namespace cinder { namespace pp {

TextureFontDrawRef TextureFontDraw::create(RendererRef renderer)
{
    return TextureFontDrawRef(new TextureFontDraw(renderer));
}

TextureFontDraw::TextureFontDraw(RendererRef renderer)
    : mRenderer(renderer)
{
}

TextureFontDraw::~TextureFontDraw()
{
    mRenderer.reset();
}

void TextureFontDraw::bind()
{
    if (!mRenderer->isBound())
        mRenderer->bind();
}

void TextureFontDraw::unbind()
{
    if (mRenderer->isBound())
        mRenderer->unbind();
}

void TextureFontDraw::drawString( TextureFont& texFont, const string &str, const Vec2f &baseline, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	texFont.drawGlyphs( *mRenderer, glyphMeasures, baseline, options );
}

void TextureFontDraw::drawString( TextureFont& texFont, const string &str, const Rectf &fitRect, const Vec2f &offset, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( TextBox::GROW, fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );	
}

#if defined( CINDER_COCOA ) || defined ( CINDER_ANDROID )
void TextureFontDraw::drawStringWrapped( TextureFont& texFont, const string &str, const Rectf &fitRect, const Vec2f &offset, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( fitRect.getWidth(), fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
#if defined( CINDER_COCOA )
	texFont.drawGlyphs( glyphMeasures, fitRect.getUpperLeft() + offset, options );
#else
	// texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );

	// Clipping affects italicized fonts on Android, so use baseline
	// but have to compensate for rectangle upper border/ascent (?)
	texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect.getUpperLeft() + offset + Vec2f(0, texFont.getFont().getAscent() ), options );
#endif
}
#endif

// class PPTextureFontRenderer : public TextureFontRenderer
// {
// public:
// 	PPTextureFontRenderer() : mPositionArray(0), mTexCoordArray(0), mColorArray(0)
// 	{
// 		mShader  = gl::GlslProg(vert, frag);
// 		mPositionAttrib = mShader.getAttribLocation("aPosition");
// 		mTexCoordAttrib = mShader.getAttribLocation("aTexCoord");
// 		mColorAttrib    = mShader.getAttribLocation("aColor");
// 	}
// 
// 	virtual void setMVP(const Matrix44f& mvp)
// 	{
// 		mMVP = mvp;
// 	}
// 
// 	virtual void setColor(const ColorA& color)
// 	{
// 		mColor = color;
// 	}
// 
// 	virtual void setPositionArray(float* pos)
// 	{
// 		mPositionArray = pos;
// 	}
// 
// 	virtual void setTexCoordArray(float* texCoord)
// 	{
// 		mTexCoordArray = texCoord;
// 	}
// 
// 	virtual void setColorArray(ColorA8u* colors)
// 	{
// 		mColorArray = colors;
// 	}
// 
// 	virtual void bind()
// 	{
// 		mShader.bind();
// 	}
// 
// 	virtual void unbind()
// 	{
// 		mShader.unbind();
// 	}
// 
// 	virtual void enableClientState()
// 	{
// 		glEnableVertexAttribArray(mPositionAttrib);
// 		if ( mColorArray )
// 			glEnableVertexAttribArray(mColorAttrib);
// 		glEnableVertexAttribArray(mTexCoordAttrib);
// 
// 		mShader.uniform("sTexture", 0);
// 		glVertexAttribPointer( mPositionAttrib, 2, GL_FLOAT, GL_FALSE, 0, mPositionArray );
// 		glVertexAttribPointer( mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, mTexCoordArray );
// 		if ( mColorArray ) {
// 			mShader.uniform("uEnableColorAttr", true);
// 			glVertexAttribPointer( mColorAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, mColorArray );
// 		}
// 		else {
// 			mShader.uniform("uColor", mColor);
// 			mShader.uniform("uEnableColorAttr", false);
// 		}
// 
// 		mShader.uniform("uMVP", mMVP);
// 	}
// 
// 	virtual void disableClientState()
// 	{
// 		glDisableVertexAttribArray(mPositionAttrib);
// 		glDisableVertexAttribArray(mColorAttrib);
// 		glDisableVertexAttribArray(mTexCoordAttrib);
// 	}
// 
// 	static const char* vert;
// 	static const char* frag;
// 
// protected:
// 	gl::GlslProg mShader;
// 	Matrix44f mMVP;
// 
// 	GLuint mPositionAttrib;
// 	GLuint mTexCoordAttrib;
// 	GLuint mColorAttrib;
// 
// 	float*    mPositionArray;
// 	float*    mTexCoordArray;
// 	ColorA8u* mColorArray;
// 	ColorA8u  mColor;
// };
// 
// const char* PPTextureFontRenderer::vert =
//         "attribute vec4 aPosition;\n"
//         "attribute vec2 aTexCoord;\n"
//         "attribute vec4 aColor;\n"
// 
//         "uniform mat4 uMVP;\n"
//         "uniform vec4 uColor;\n"
//         "uniform bool uEnableColorAttr;\n"
// 
//         "varying vec4 vColor;\n"
//         "varying vec2 vTexCoord;\n"
// 
//         "void main() {\n"
//         "  if (uEnableColorAttr) {\n"
//         "    vColor = aColor;\n"
//         "  }\n"
// 		"  else {\n"
// 		"    vColor = uColor;\n"
// 		"  }\n"
//         "  vTexCoord = aTexCoord;\n"
//         "  gl_Position = uMVP * aPosition;\n"
//         "}\n";
// 
// const char* PPTextureFontRenderer::frag =
//         "precision mediump float;\n"
// 
//         "uniform sampler2D sTexture;\n"
// 
//         "varying vec4 vColor;\n"
//         "varying vec2 vTexCoord;\n"
// 
//         "void main() {\n"
//         "      gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
//         "}\n";

TextureFont::TextureFont( const Font &font, const string &supportedChars, const Format &format )
	: gl::TextureFont( font, supportedChars, format )
{
}

TextureFont::TextureFont( const Font &font, const string &supportedChars, Atlas &atlas )
	: gl::TextureFont( font, supportedChars, atlas )
{
}

void TextureFont::drawGlyphs( Renderer& renderer, const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			baseline = Vec2f( floor( baseline.x ), floor( baseline.y ) );
			
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			
			Rectf destRect( glyphInfo.mTexCoords );
			Rectf srcCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( baseline.x, baseline.y - mFont.getAscent() * scale );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}

			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();

		renderer.setPositionArray(&verts[0], 2);
		renderer.setTexCoordArray(&texCoords[0]);
		renderer.setColorArray(colors.empty() ? NULL : (GLubyte*) &vertColors[0]);

		renderer.enableClientState();
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
		renderer.disableClientState();
	}
}

void TextureFont::drawGlyphs( Renderer& renderer, const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
	}
	else {
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();

	for ( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			offset = Vec2f( floor( offset.x ), floor( offset.y ) );

		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			Rectf srcTexCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			Rectf destRect( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( offset.x, offset.y );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				

			// clip
			Rectf clipped( destRect );
			if( options.getClipHorizontal() ) {
				clipped.x1 = max( destRect.x1, clip.x1 );
				clipped.x2 = min( destRect.x2, clip.x2 );
			}
			if( options.getClipVertical() ) {
				clipped.y1 = max( destRect.y1, clip.y1 );
				clipped.y2 = min( destRect.y2, clip.y2 );
			}
			
			if( clipped.x1 >= clipped.x2 || clipped.y1 >= clipped.y2 )
				continue;
			
			Vec2f coordScale( 1 / (float)destRect.getWidth() / curTex.getWidth() * glyphInfo.mTexCoords.getWidth(),
				1 / (float)destRect.getHeight() / curTex.getHeight() * glyphInfo.mTexCoords.getHeight() );
			srcTexCoords.x1 = srcTexCoords.x1 + ( clipped.x1 - destRect.x1 ) * coordScale.x;
			srcTexCoords.x2 = srcTexCoords.x1 + ( clipped.x2 - clipped.x1 ) * coordScale.x;
			srcTexCoords.y1 = srcTexCoords.y1 + ( clipped.y1 - destRect.y1 ) * coordScale.y;
			srcTexCoords.y2 = srcTexCoords.y1 + ( clipped.y2 - clipped.y1 ) * coordScale.y;

			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY2() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY2() );

			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY2() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY2() );

			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();

		renderer.setPositionArray(&verts[0], 2);
		renderer.setTexCoordArray(&texCoords[0]);
		renderer.setColorArray(colors.empty() ? NULL : (GLubyte*) &vertColors[0]);

		renderer.enableClientState();
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
		renderer.disableClientState();
	}
}

TextureFontRef TextureFont::create( const Font &font, Atlas &atlas, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, atlas ) ); 
}

TextureFontRef TextureFont::create( const Font &font, const Format &format, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, format ) ); 
}

} }  // namespace cinder::pp

