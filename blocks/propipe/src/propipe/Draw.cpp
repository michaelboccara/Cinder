#include "Draw.h"
#include "DrawShader.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/Triangulate.h"

using namespace cinder::gl;

namespace cinder { namespace pp {

Draw::Draw(DrawShaderRef shader) : mShader(shader)
{
}

Draw::~Draw()
{
}

void Draw::drawLine( const Vec2f &start, const Vec2f &end )
{
	drawLine(Vec3f(start.x, start.y, 0), Vec3f(end.x, end.y, 0));
}

void Draw::drawLine( const Vec3f &start, const Vec3f &end )
{
	float lineVerts[3*2];
	mShader->resetArrays();
	mShader->setPositionArray(lineVerts, 3);
	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
	mShader->enableClientState();
	glDrawArrays( GL_LINES, 0, 2 );
	mShader->disableClientState();
}

namespace {

void drawCubeImpl( DrawShader& shader, const Vec3f &c, const Vec3f &size, bool drawColors )
{
	GLfloat sx = size.x * 0.5f;
	GLfloat sy = size.y * 0.5f;
	GLfloat sz = size.z * 0.5f;
	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z


	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
									0,0,1,	0,0,1,	0,0,1,	0,0,1,
								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};
	 
	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								0,1,	1,1,	1,0,	0,0,							
								1,1,	1,0,	0,0,	0,1,
								1,0,	0,0,	0,1,	1,1,
								1,0,	0,0,	0,1,	1,1 };
	
	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };

	shader.resetArrays();

	shader.setNormalArray( normals );
	shader.setTexCoordArray( texs );

	if ( drawColors ) {
		shader.setColorArray( colors );
	}
	shader.setPositionArray( vertices, 3 );

	shader.enableClientState();
	glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );
	shader.disableClientState();
}
} // anonymous namespace


void Draw::drawCube( const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( *mShader, center, size, false );
}

void Draw::drawColorCube( const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( *mShader, center, size, true );
}

void Draw::drawStrokedCube( const Vec3f &center, const Vec3f &size )
{
	Vec3f min = center - size * 0.5f;
	Vec3f max = center + size * 0.5f;

	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z) );
	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z) );
	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z) );
	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z) );
	
	drawLine( Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z) );
	drawLine( Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z) );
	drawLine( Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z) );
	drawLine( Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z) );
	
	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z) );
	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z) );
	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z) );
	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z) );
}

void Draw::drawSphere( const Vec3f &center, float radius, int segments )
{
	if( segments < 0 )
		return;

	GLfloat *verts     = new GLfloat[(segments+1)*2*3];
	GLfloat *normals   = new GLfloat[(segments+1)*2*3];
	GLfloat *texCoords = new GLfloat[(segments+1)*2*2];

	mShader->resetArrays();
	mShader->setPositionArray( verts, 3 );
	mShader->setTexCoordArray( texCoords );
	mShader->setNormalArray( normals );

	mShader->enableClientState();

	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

		for( int i = 0; i <= segments; i++ ) {
			Vec3f e, p;
			float theta3 = i * 2 * 3.14159f / segments;

			e.x = math<float>::cos( theta1 ) * math<float>::cos( theta3 );
			e.y = math<float>::sin( theta1 );
			e.z = math<float>::cos( theta1 ) * math<float>::sin( theta3 );
			p = e * radius + center;
			normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
			texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
			verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;

			e.x = math<float>::cos( theta2 ) * math<float>::cos( theta3 );
			e.y = math<float>::sin( theta2 );
			e.z = math<float>::cos( theta2 ) * math<float>::sin( theta3 );
			p = e * radius + center;
			normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
			texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
			verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
		}
		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
	}

	mShader->disableClientState();
	
	delete [] verts;
	delete [] normals;
	delete [] texCoords;
}

void Draw::draw( const class Sphere &sphere, int segments )
{
	drawSphere( sphere.getCenter(), sphere.getRadius(), segments );
}

void Draw::drawSolidCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new GLfloat[(numSegments+2)*3];
	verts[0] = center.x;
	verts[1] = center.y;
	verts[2] = 0;
	for( int s = 0; s <= numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radius;
	}

	mShader->resetArrays();
	mShader->setPositionArray( verts, 2 );
	mShader->enableClientState();
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	mShader->disableClientState();
	delete [] verts;
}

void Draw::drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new GLfloat[numSegments*3];
	for( int s = 0; s < numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[s*2+0] = center.x + math<float>::cos( t ) * radius;
		verts[s*2+1] = center.y + math<float>::sin( t ) * radius;
	}

	mShader->resetArrays();
	mShader->setPositionArray( verts, 2 );
	mShader->enableClientState();
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
	mShader->disableClientState();
	delete [] verts;
}

void Draw::drawSolidEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[(numSegments+2)*2];
	verts[0] = center.x;
	verts[1] = center.y;
	for( int s = 0; s <= numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radiusX;
		verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radiusY;
	}
	mShader->resetArrays();
	mShader->setPositionArray(verts, 2);
	mShader->enableClientState();
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	mShader->disableClientState();
	delete [] verts;
}

void Draw::drawStrokedEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
	GLfloat *verts = new float[numSegments*2];
	for( int s = 0; s < numSegments; s++ ) {
		float t = s / (float)numSegments * 2.0f * 3.14159f;
		verts[s*2+0] = center.x + math<float>::cos( t ) * radiusX;
		verts[s*2+1] = center.y + math<float>::sin( t ) * radiusY;
	}
	mShader->resetArrays();
	mShader->setPositionArray(verts, 2);
	mShader->enableClientState();
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
	mShader->disableClientState();
	delete [] verts;
}

void Draw::drawSolidRect( const Rectf &rect, bool textureRectangle )
{
	GLfloat verts[8];
	GLfloat texCoords[8];

	verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
	verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
	verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
	verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
	verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = ( textureRectangle ) ? rect.getY2() : 1;

	mShader->resetArrays();
	mShader->setPositionArray(verts, 2);
	mShader->setTexCoordArray(texCoords);
	mShader->enableClientState();
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	mShader->disableClientState();
}

void Draw::drawStrokedRect( const Rectf &rect )
{
	GLfloat verts[8];
	verts[0] = rect.getX1();	verts[1] = rect.getY1();
	verts[2] = rect.getX2();	verts[3] = rect.getY1();
	verts[4] = rect.getX2();	verts[5] = rect.getY2();
	verts[6] = rect.getX1();	verts[7] = rect.getY2();

	mShader->resetArrays();
	mShader->setPositionArray( verts, 2 );
	mShader->enableClientState();
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
	mShader->disableClientState();
}

void Draw::drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
	mShader->setColor(ColorA8u(255, 0, 0, 255));
	drawVector( Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
	mShader->setColor(ColorA8u(0, 255, 0, 255));
	drawVector( Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
	mShader->setColor(ColorA8u(0, 0, 255, 255));
	drawVector( Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
}

void Draw::drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
	const int NUM_SEGMENTS = 32;
	float lineVerts[3*2];
	Vec3f coneVerts[NUM_SEGMENTS+2];

	mShader->resetArrays();
	mShader->setPositionArray(lineVerts, 3);
	mShader->enableClientState();

	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z;	
	glDrawArrays( GL_LINES, 0, 2 );
	
	// Draw the cone
	Vec3f axis = ( end - start ).normalized();
	Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
	Vec3f left = axis.cross( temp ).normalized();
	Vec3f up = axis.cross( left ).normalized();

	mShader->setPositionArray(&coneVerts[0].x, 3);
	coneVerts[0] = Vec3f( end + axis * headLength );
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

	// draw the cap
	mShader->setPositionArray(&coneVerts[0].x, 3);
	coneVerts[0] = end;
	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
		float t = s / (float)NUM_SEGMENTS;
		coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
	}
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );

	mShader->disableClientState();
}

void Draw::drawFrustum( const Camera &cam )
{
	Vec3f vertex[8];
	Vec3f nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );

	Vec3f farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );
	
	mShader->resetArrays();
	mShader->setPositionArray(&vertex[0].x, 3);
	mShader->enableClientState();
	
	vertex[0] = cam.getEyePoint();
	vertex[1] = nearTopLeft;
	vertex[2] = cam.getEyePoint();
	vertex[3] = nearTopRight;
	vertex[4] = cam.getEyePoint();
	vertex[5] = nearBottomRight;
	vertex[6] = cam.getEyePoint();
	vertex[7] = nearBottomLeft;
	glDrawArrays( GL_LINES, 0, 8 );

	vertex[0] = farTopLeft;
	vertex[1] = nearTopLeft;
	vertex[2] = farTopRight;
	vertex[3] = nearTopRight;
	vertex[4] = farBottomRight;
	vertex[5] = nearBottomRight;
	vertex[6] = farBottomLeft;
	vertex[7] = nearBottomLeft;
	glDrawArrays( GL_LINES, 0, 8 );

	glLineWidth( 2.0f );
	vertex[0] = nearTopLeft;
	vertex[1] = nearTopRight;
	vertex[2] = nearBottomRight;
	vertex[3] = nearBottomLeft;
	glDrawArrays( GL_LINE_LOOP, 0, 4 );

	vertex[0] = farTopLeft;
	vertex[1] = farTopRight;
	vertex[2] = farBottomRight;
	vertex[3] = farBottomLeft;
	glDrawArrays( GL_LINE_LOOP, 0, 4 );

	glLineWidth( 1.0f );

	mShader->disableClientState();
}

void Draw::drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
{
	longitudeSegments = std::min( std::max( 7, longitudeSegments ) + 1, 255 );
	latitudeSegments = std::min( std::max( 7, latitudeSegments ) + 1, 255 );

	int i, j;
	GLfloat *normal = new GLfloat[longitudeSegments * latitudeSegments * 3];
	GLfloat *vertex = new GLfloat[longitudeSegments * latitudeSegments * 3];
	GLfloat *tex = new GLfloat[longitudeSegments * latitudeSegments * 2];
	GLushort *indices = new GLushort[latitudeSegments * 2];
	float ct, st, cp, sp;

	mShader->resetArrays();
	mShader->setPositionArray(vertex, 3);
	mShader->setTexCoordArray(tex);
	mShader->setNormalArray(normal);
	mShader->enableClientState();

	for( i = 0; i < longitudeSegments; i++ ) {
		ct = cos(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));
		st = sin(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));

		for ( j = 0; j < latitudeSegments; j++ ) {
			cp = cos(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));
			sp = sin(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));

			normal[3 * (i + longitudeSegments * j)    ] = cp * ct;
			normal[3 * (i + longitudeSegments * j) + 1] = sp * ct;
			normal[3 * (i + longitudeSegments * j) + 2] = st;

			tex[2 * (i + longitudeSegments * j)    ] = 1.0f * (float)i / (float)(longitudeSegments - 1);
			tex[2 * (i + longitudeSegments * j) + 1] = 5.0f * (float)j / (float)(latitudeSegments - 1);

			vertex[3 * (i + longitudeSegments * j)    ] = cp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 1] = sp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 2] = innerRadius * st;
		}
	}

	for ( i = 0; i < longitudeSegments - 1; i++ ) {
		for ( j = 0; j < latitudeSegments; j++ ) {
			indices[j*2+0] = i + 1 + longitudeSegments * j;
			indices[j*2+1] = i + longitudeSegments * j;
		}
		glDrawElements( GL_TRIANGLE_STRIP, (latitudeSegments)*2, GL_UNSIGNED_SHORT, indices );
	}

	mShader->disableClientState();
	
	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}

void Draw::drawCylinder( float baseRadius, float topRadius, float height, int slices, int stacks )
{
	stacks = math<int>::max(2, stacks + 1);	// minimum of 1 stack
	slices = math<int>::max(4, slices + 1);	// minimum of 3 slices

	int i, j;
	GLfloat *normal = new GLfloat[stacks * slices * 3];
	GLfloat *vertex = new GLfloat[stacks * slices * 3];
	GLfloat *tex = new GLfloat[stacks * slices * 2];
	GLushort *indices = new GLushort[slices * 2];

	mShader->resetArrays();
	mShader->setPositionArray(vertex, 3);
	mShader->setTexCoordArray(tex);
	mShader->setNormalArray(normal);
	mShader->enableClientState();

	for(i=0;i<slices;i++) {
		float u = (float)i / (float)(slices - 1);
		float ct = cos(2.0f * (float)M_PI * u);
		float st = sin(2.0f * (float)M_PI * u);

		for(j=0;j<stacks;j++) {
			float v = (float)j / (float)(stacks-1);
			float radius = lerp<float>(baseRadius, topRadius, v); 

			int index = 3 * (i * stacks + j);

			normal[index    ] = ct;
			normal[index + 1] = 0;
			normal[index + 2] = st;

			tex[2 * (i * stacks + j)    ] = u;
			tex[2 * (i * stacks + j) + 1] = 1.0f - v; // top of texture is top of cylinder

			vertex[index    ] = ct * radius;
			vertex[index + 1] = v * height;
			vertex[index + 2] = st * radius;
		}
	}

	for(i=0;i<(stacks - 1);i++) {
		for(j=0;j<slices;j++) {
			indices[j*2+0] = i + 0 + j * stacks;
			indices[j*2+1] = i + 1 + j * stacks;
		}
		glDrawElements( GL_TRIANGLE_STRIP, (slices)*2, GL_UNSIGNED_SHORT, indices );
	}

	mShader->disableClientState();

	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
}

void Draw::draw( const PolyLine<Vec2f> &polyLine )
{
	mShader->resetArrays();
	mShader->setPositionArray((GLfloat*) &(polyLine.getPoints()[0]), 2);
	mShader->enableClientState();
	glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );

	mShader->disableClientState();
}

void Draw::draw( const class PolyLine<Vec3f> &polyLine )
{
	mShader->resetArrays();
	mShader->setPositionArray((GLfloat*) &(polyLine.getPoints()[0]), 3);
	mShader->enableClientState();
	glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );

	mShader->disableClientState();
}

void Draw::draw( const class Path2d &path2d, float approximationScale )
{
	if( path2d.getNumSegments() == 0 )
		return;
	std::vector<Vec2f> points = path2d.subdivide( approximationScale );

	mShader->resetArrays();
	mShader->setPositionArray((GLfloat*) &(points[0]), 2);
	mShader->enableClientState();
	glDrawArrays( GL_LINE_STRIP, 0, points.size() );
	mShader->disableClientState();
}

void Draw::draw( const class Shape2d &shape2d, float approximationScale )
{
	mShader->enableClientState( DrawShader::ENABLE_ATTRIBS | DrawShader::UPDATE_UNIFORMS );
	mShader->resetArrays();

	for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
		if( contourIt->getNumSegments() == 0 )
			continue;
		std::vector<Vec2f> points = contourIt->subdivide( approximationScale );
		mShader->setPositionArray((GLfloat *) &(points[0]), 2);
		mShader->enableClientState( DrawShader::UPLOAD_ATTRIBS );
		glDrawArrays( GL_LINE_STRIP, 0, points.size() );
	}
	mShader->disableClientState();
}

void Draw::drawSolid( const class Path2d &path2d, float approximationScale )
{
	draw( Triangulator( path2d ).calcMesh() );
}

void Draw::drawSolid( const class Shape2d &shape2d, float approximationScale )
{
	draw( Triangulator( shape2d ).calcMesh() );
}

void Draw::drawSolid( const PolyLine2f &polyLine )
{
	draw( Triangulator( polyLine ).calcMesh() );
}

// TriMesh2d
void Draw::draw( const TriMesh2d &mesh )
{
	if( mesh.getNumVertices() <= 0 )
		return;

	mShader->resetArrays();
	mShader->setPositionArray((GLfloat*)&(mesh.getVertices()[0]), 2);
	if( mesh.hasColorsRgb() ) {
		mShader->setColorArray(&(mesh.getColorsRGB()[0]), GL_FLOAT, 3);
	}
	else if( mesh.hasColorsRgba() ) {
		mShader->setColorArray(&(mesh.getColorsRGBA()[0]), GL_FLOAT, 4);
	}

	if( mesh.hasTexCoords() ) {
		mShader->setTexCoordArray((GLfloat*) &(mesh.getTexCoords()[0]));
	}

	mShader->enableClientState();
	glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
	mShader->disableClientState();
}


void Draw::draw( const TriMesh &mesh )
{
	mShader->resetArrays();
	if( mesh.hasNormals() ) {
		mShader->setNormalArray((GLfloat*)&(mesh.getNormals()[0]));
	}
	
	if( mesh.hasColorsRGB() ) {
		mShader->setColorArray(&(mesh.getColorsRGB()[0]), GL_FLOAT, 3);
	}
	else if( mesh.hasColorsRGBA() ) {
		mShader->setColorArray(&(mesh.getColorsRGBA()[0]), GL_FLOAT, 4);
	}

	if( mesh.hasTexCoords() ) {
		mShader->setTexCoordArray((GLfloat*)&(mesh.getTexCoords()[0]));
	}

	mShader->enableClientState();
	glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
	mShader->disableClientState();
}

void Draw::draw( const VboMesh &vbo )
{
}

void Draw::drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
	Vec3f verts[4];
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };

	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );

	mShader->resetArrays();
	mShader->setPositionArray((GLfloat *) verts, 3);
	mShader->setTexCoordArray(texCoords);
	mShader->enableClientState();

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	mShader->disableClientState();
}

void Draw::draw( const Texture &texture )
{
	draw( texture, Area( texture.getCleanBounds() ), texture.getCleanBounds() );
}

void Draw::draw( const Texture &texture, const Vec2f &pos )
{
	draw( texture, texture.getCleanBounds(), Rectf( pos.x, pos.y, pos.x + texture.getCleanWidth(), pos.y + texture.getCleanHeight() ) );
}

void Draw::draw( const Texture &texture, const Rectf &rect )
{
	draw( texture, texture.getCleanBounds(), rect );
}

void Draw::draw( const Texture &texture, const Area &srcArea, const Rectf &destRect )
{
	mShader->bindTexture(texture);

	GLfloat verts[8];
	GLfloat texCoords[8];

	mShader->resetArrays();
	mShader->setPositionArray(verts, 2);
	mShader->setTexCoordArray(texCoords);
	mShader->enableClientState();

	verts[0*2+0] = destRect.getX2(); verts[0*2+1] = destRect.getY1(); 
	verts[1*2+0] = destRect.getX1(); verts[1*2+1] = destRect.getY1(); 
	verts[2*2+0] = destRect.getX2(); verts[2*2+1] = destRect.getY2(); 
	verts[3*2+0] = destRect.getX1(); verts[3*2+1] = destRect.getY2(); 

	const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
	texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();	
	texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();	
	texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();	
	texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();	

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	mShader->disableClientState();
	mShader->unbindTexture();
}

DrawRef Draw::create(DrawShaderRef shader)
{
	return DrawRef(new Draw(shader));
}

} }
