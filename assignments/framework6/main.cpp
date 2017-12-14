/* Computer Graphics and Game Technology, Assignment Box2D game
 *
 * Student name .... Derk Barten
 * Student email ...  derk.barten@student.uva.nl
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <cstdio>

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <Box2D/Box2D.h>
#include <math.h>

#include "levels.h"

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;
b2World *world;
b2Body * player;

void load_polyshape(b2PolygonShape *shape, poly_t *poly) {
    b2Vec2 vertices[20];
    for (unsigned int i = 0; i < poly->num_verts; i++) {
        vertices[i].Set(poly->verts[i].x, poly->verts[i].y);
    }
    shape->Set(vertices, 3);
}

void create_player(point_t start) {
     // Create the ball
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(start.x, start.y);
    player = world->CreateBody(&bodyDef);

    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = 0.25f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    player->CreateFixture(&fixtureDef);
}

void create_polygon(poly_t *poly) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(poly->position.x, poly->position.y);
    b2PolygonShape shape;
    
    load_polyshape(&shape, poly);

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&shape, 1.0f);
}

/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */
void load_world(unsigned int level_number)
{
    if (level_number >= num_levels)
    {
        // Note that level is unsigned but we still use %d so -1 is shown as
        // such.
        printf("Warning: level %d does not exist.\n", level_number);
        return;
    }

    level_t *level = levels + level_number;
    for (unsigned int i = 0; i < level->num_polygons; i++) {
        poly_t *poly = level->polygons + i;
        create_polygon(poly);
    }
    create_player(level->start);
}

void draw_polygon(b2PolygonShape *poly, b2Vec2 center, float angle) {
    int count = poly->GetVertexCount();
    glColor3f(0, 1, 0);
    glPushMatrix();
        glTranslatef(center.x, center.y, 0);
        glRotatef(angle * 180.0/M_PI, 0,0,1);
        glBegin(GL_POLYGON);
            for (int i = 0; i < count; i++) {
                b2Vec2 vertex = poly->GetVertex(i);
                glVertex2f(vertex.x, vertex.y);
            }
        glEnd();
    glPopMatrix();
}


void draw_player(){
	int i;
	int triangleAmount = 20;
	
	//GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * M_PI;
    GLfloat x = player->GetPosition().x;
    GLfloat y = player->GetPosition().y;
    GLfloat radius = player->GetFixtureList()->GetShape()->m_radius;
    
    glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount;i++) { 
			glVertex2f(
		            x + (radius * cos(i *  twicePi / triangleAmount)), 
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

/*
 * Called when we should redraw the scene (i.e. every frame).
 * It will show the current framerate in the window title.
 */
void draw(void)
{
    int time = glutGet(GLUT_ELAPSED_TIME);
    int frametime = time - last_time;
    frame_count++;

    // Clear the buffer
    glColor3f(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    b2Body *body = world->GetBodyList();
    while(body != NULL)
    {
        if (body != player)
            draw_polygon((b2PolygonShape *)body->GetFixtureList()->GetShape(), body->GetPosition(), body->GetAngle());
        body = body->GetNext();
    }
    draw_player();
    // Show rendered frame
    glutSwapBuffers();

    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    world->Step(timeStep, velocityIterations, positionIterations);

    // Display fps in window title.
    if (frametime >= 1000)
    {
        char window_title[128];
        snprintf(window_title, 128,
                "Box2D: %f fps, level %d/%d",
                frame_count / (frametime / 1000.f), -1, num_levels);
        glutSetWindowTitle(window_title);
        last_time = time;
        frame_count = 0;
    }
}

/*
 * Called when window is resized. We inform OpenGL about this, and save this
 * for future reference.
 */
void resize_window(int width, int height)
{
    glViewport(0, 0, width, height);
    reso_x = width;
    reso_y = height;
}

/*
 * Called when the user presses a key.
 */
void key_pressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // Esc
        case 'q':
            exit(0);
            break;
        // Add any keys you want to use, either for debugging or gameplay.
        default:
            break;
    }
}

/*
 * Called when the user clicked (or released) a mouse buttons inside the window.
 */
void mouse_clicked(int button, int state, int x, int y)
{

}

/*
 * Called when the mouse is moved to a certain given position.
 */
void mouse_moved(int x, int y)
{

}


int main(int argc, char **argv)
{
    // Create an OpenGL context and a GLUT window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(reso_x, reso_y);
    glutCreateWindow("Box2D");

    glewInit();

    // Bind all GLUT events do callback function.
    glutDisplayFunc(&draw);
    glutIdleFunc(&draw);
    glutReshapeFunc(&resize_window);
    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_clicked);
    glutMotionFunc(&mouse_moved);
    glutPassiveMotionFunc(&mouse_moved);

    // Initialise the matrices so we have an orthogonal world with the same size
    // as the levels, and no other transformations.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, world_x, 0, world_y, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Read the levels into a bunch of structs.
    num_levels = load_levels(&levels);
    printf("Loaded %d levels.\n", num_levels);

    // Load the first level (i.e. create all Box2D stuff).
    b2Vec2 gravity(0.0f, - 10.0f);
    b2World wrld(gravity);
    world = &wrld;
    load_world(1);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    frame_count = 0;
    glutMainLoop();

    return 0;
}
