#include "Renderer.h"
#include "cinder/gl/GlslProg.h"

using namespace cinder::gl;

namespace cinder { namespace pp {

/** Default renderer implementation */
class PPRenderer : public Renderer
{
public:
    PPRenderer() : mBound(false), 
		mPositionDim(3), mPositionArray(0), mTexCoordArray(0), mColorArray(0), mNormalArray(0)
    {
        try {
            mShader  = gl::GlslProg(vert, frag);
            mPositionAttrib = mShader.getAttribLocation("aPosition");
            mTexCoordAttrib = mShader.getAttribLocation("aTexCoord");
            mColorAttrib    = mShader.getAttribLocation("aColor");
        }
        catch (GlslProgCompileExc& ex) {
            CI_LOGE("Error compiling: %s", ex.what());
        }
        // mNormalAttrib   = mShader.getAttribLocation("aNormal");
    }

	virtual void setModelView(const Matrix44f& mv)
	{
		mModelView = mv;
	}

	virtual void setProjection(const Matrix44f& proj)
	{
		mProjection = proj;
	}

	virtual void setColor(const ColorA& color)
	{
		mColor = color;
	}

	virtual void setPositionArray(float* pos, int dim)
	{
		mPositionArray = pos;
        mPositionDim = dim;
	}

	virtual void setTexCoordArray(float* texCoord)
	{
		mTexCoordArray = texCoord;
	}

	virtual void setColorArray(GLubyte* colors)
	{
		mColorArray = colors;
	}

	virtual void setNormalArray(float* norms)
    {
        mNormalArray = norms;
    }

    virtual void resetArrays()
    {
        mPositionArray = NULL;
        mTexCoordArray = NULL;
        mColorArray    = NULL;
        mNormalArray   = NULL;
    }

	virtual void bind()
	{
        if (!mBound) {
            mShader.bind();
            mBound = true;
        }
	}

	virtual void unbind()
	{
        if (mBound) {
            mShader.unbind();
            mBound = false;
        }
	}

	virtual void enableClientState()
	{
		glEnableVertexAttribArray(mPositionAttrib);
		if ( mColorArray )
			glEnableVertexAttribArray(mColorAttrib);
        if ( mTexCoordArray )
            glEnableVertexAttribArray(mTexCoordAttrib);
        // if ( mNormalArray )
        //     glEnableVertexAttribArray(mNormalAttrib);

		glVertexAttribPointer( mPositionAttrib, mPositionDim, GL_FLOAT, GL_FALSE, 0, mPositionArray );
		glVertexAttribPointer( mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, mTexCoordArray );

		if ( mColorArray ) {
			mShader.uniform("uEnableColorAttr", true);
			glVertexAttribPointer( mColorAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, mColorArray );
		}
		else {
			mShader.uniform("uColor", mColor);
			mShader.uniform("uEnableColorAttr", false);
		}

        if (mTexCoordArray) {
            mShader.uniform("sTexture", 0);
            mShader.uniform("uEnableTextureAttr", true);
        }
        else {
            mShader.uniform("uEnableTextureAttr", false);
        }

        // if ( mNormalArray )
        //     glVertexAttribPointer("");

		mShader.uniform("uModelView", mModelView);
		mShader.uniform("uProjection", mProjection);
	}

	virtual void disableClientState()
	{
		glDisableVertexAttribArray(mPositionAttrib);
		glDisableVertexAttribArray(mColorAttrib);
		glDisableVertexAttribArray(mTexCoordAttrib);
        // glDisableVertexAttribArray(mNormalAttrib);
	}

	static const char* vert;
	static const char* frag;

protected:
	bool mBound;
	gl::GlslProg mShader;
	Matrix44f mModelView;
	Matrix44f mProjection;

	GLuint mPositionAttrib;
	GLuint mTexCoordAttrib;
	GLuint mColorAttrib;
	GLuint mNormalAttrib;

    int       mPositionDim;
	ColorA8u  mColor;

	float*    mPositionArray;
	float*    mTexCoordArray;
	GLubyte*  mColorArray;
    float*    mNormalArray;
};

const char* PPRenderer::vert =
        "attribute vec4 aPosition;\n"
        "attribute vec2 aTexCoord;\n"
        "attribute vec4 aColor;\n"

        "uniform mat4 uModelView;\n"
        "uniform mat4 uProjection;\n"
        "uniform vec4 uColor;\n"

        "uniform bool uEnableColorAttr;\n"
        "uniform bool uEnableTextureAttr;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  if (uEnableColorAttr) {\n"
        "    vColor = aColor;\n"
        "  }\n"
		"  else {\n"
		"    vColor = uColor;\n"
		"  }\n"
        "  if (uEnableTextureAttr) {\n"
        "    vTexCoord = aTexCoord;\n"
        "  }\n"
        "  gl_Position = uProjection * uModelView * aPosition;\n"
        "}\n";

const char* PPRenderer::frag =
        "precision mediump float;\n"

        "uniform sampler2D sTexture;\n"

        "uniform bool uEnableTextureAttr;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  if (uEnableTextureAttr) {\n"
        "    gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
        "  }\n"
        "  else {\n"
        "    gl_FragColor = vColor;\n"
        "  }\n"
        "}\n";

RendererRef Renderer::create(RendererType rendererType)
{
	return RendererRef(new PPRenderer());
}

} }

