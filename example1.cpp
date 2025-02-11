//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}

GLuint mloc, vloc, ploc;

void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	mloc = glGetUniformLocation(program, "model");
	vloc = glGetUniformLocation(program, "view");
	ploc = glGetUniformLocation(program, "projection");


	glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}


vec3 gpos = vec3();
float lrot = 0;
float rrot = 0;

mat4 gmat = identity();

void cube(vec3 pos, vec3 size)
{
	mat4 mat = Translate(pos / 2) * Scale(size / 2);
	glUniformMatrix4fv(mloc, 1, GL_TRUE, gmat * mat);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

float zN = 1;
float w, h;

void resharp(int w1, int h1)
{
	w = w1;
	h = h1;

	glutReshapeWindow(w, h);
}


void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );    

	//coordinate
	vec4 eyePos = vec4(1, 1, 1, 1);
	vec4 targetPos = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 1);

	auto camView = LookAt(eyePos, targetPos, up);
	glUniformMatrix4fv(vloc, 1, GL_TRUE, camView);

	//auto prj = Frustum(-3, 3, -3, 3, zN, 1000);

	auto prj = Perspective(60, w/h, zN, 1000);

	glUniformMatrix4fv(ploc, 1, GL_TRUE, prj);

	mat4 local = Translate(gpos);

	gmat = local;

	cube(vec3(0, 1.115, 0), vec3(1.06, 0.03, 0.05));

	cube(vec3(0.515,  0, 0), vec3(0.03, 2.26, 0.05));
	cube(vec3(-0.515, 0, 0), vec3(0.03, 2.26, 0.05));

	//cua
	//l
	vec3 lp = vec3(-0.25, 0, 0);
	gmat *= Translate(lp) * RotateY(lrot) * Translate(-lp);
	cube(lp, vec3(0.5, 2.2, 0.03));
	cube(vec3(-0.05, 0, -0.03), vec3(0.02, 0.3, 0.03));

	//r
	gmat = local;
	vec3 rp = vec3(0.25, 0, 0);
	gmat *= Translate(rp) * RotateY(rrot) * Translate(-rp);
	gmat *= RotateY(rrot);
	cube(rp, vec3(0.5, 2.2, 0.03));
	cube(vec3(0.05, 0, -0.03), vec3(0.02, 0.3, 0.03));
    
	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
	case 'a':
		gpos.x -= 0.1;
		break;

	case 'd':
		gpos.x += 0.1;
		break;

	case 'w':
		gpos.y += 0.1;
		break;

	case 's':
		gpos.y -= 0.1;
		break;

	case 'r':
		lrot += 1.75f;
		if (lrot > 120)
			lrot = 120;
		break;

	case 'R':
		lrot -= 1.75f;
		break;

	case 't':
		rrot += 1.75f;
		break;

	case 'T':
		rrot -= 1.75f;
		break;

	case 'c':
		zN += 0.1f;
		break;

	case 'C':
		zN -= 0.1f;
		break;

    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
    }

	glutPostRedisplay();
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );    
	glutReshapeFunc(resharp);

	glutMainLoop();
    return 0;
}
