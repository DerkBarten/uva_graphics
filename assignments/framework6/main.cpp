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
#include "main.h"

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

point_t reso_to_world(point_t reso_point) {
    point_t world_point;
    world_point.x = (reso_point.x / reso_x) * world_x;
    world_point.y = world_y - (reso_point.y / reso_y) * world_y;
    return world_point;
}

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;
b2World *world;
b2Body *player;
b2Body *finish;

int current_world = 0;
bool is_finished = false;
bool is_active = false;

point_t points[4];
int point_count = 0;

void load_polyshape(b2PolygonShape *shape, poly_t *poly) {
    b2Vec2 *vertices = new b2Vec2[poly->num_verts];
    for (unsigned int i = 0; i < poly->num_verts; i++) {
        vertices[i].Set(poly->verts[i].x, poly->verts[i].y);
    }
    shape->Set(vertices, poly->num_verts);
    delete vertices;
}

void create_player(point_t start) {
     // Create the ball
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(start.x, start.y);
    player = world->CreateBody(&bodyDef);

    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = 0.15f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    player->CreateFixture(&fixtureDef);
}

void create_finish(point_t end) {
    b2BodyDef bodyDef;
    bodyDef.position.Set(end.x, end.y);
    b2PolygonShape shape;
    shape.SetAsBox(0.05,0.05);
    finish = world->CreateBody(&bodyDef);
    finish->CreateFixture(&shape, 0.0f);
}

void create_polygon(poly_t *poly) {
    b2BodyDef bodyDef;
    if (poly->is_dynamic) {
        bodyDef.type = b2_dynamicBody;
    }
    bodyDef.position.Set(poly->position.x, poly->position.y);
    b2PolygonShape shape;
    
    load_polyshape(&shape, poly);

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&shape, 1.0f);
}

void create_point(point_t position) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(position.x, position.y);

    b2CircleShape circle;
    circle.m_radius = 0.02f;

    b2Body *body = world->CreateBody(&bodyDef);
    body->CreateFixture(&circle, 0.0f);
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
    create_finish(level->end);
}

void draw_polygon(b2Body *body, GLfloat *color) {
    b2PolygonShape *shape = (b2PolygonShape *)body->GetFixtureList()->GetShape();
    int count = shape->GetVertexCount();

    glColor3f(color[0], color[1], color[2]);
    glPushMatrix();
        glTranslatef(body->GetPosition().x, body->GetPosition().y, 0);
        glRotatef(body->GetAngle() * 180.0/M_PI, 0,0,1);
        glBegin(GL_POLYGON);
            for (int i = 0; i < count; i++) {
                b2Vec2 vertex = shape->GetVertex(i);
                glVertex2f(vertex.x, vertex.y);
            }
        glEnd();
    glPopMatrix();
}

void draw_circle(b2Body *circle){
	int i;
	int triangleAmount = 20;
    GLfloat radius = circle->GetFixtureList()->GetShape()->m_radius;

    if (radius <= 0.01f) {
        return;
    }

    GLfloat twicePi = 2.0f * M_PI;
    GLfloat x = circle->GetPosition().x;
    GLfloat y = circle->GetPosition().y;
    
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

void go_to_level(int level_number) {
    is_active = false;
    b2Body *next = world->GetBodyList();
    b2Body *previous = next; 
    next = next->GetNext();

    while (next != NULL) {
        world->DestroyBody(previous);
        previous = next;
        next = next->GetNext();
    }
    world->DestroyBody(previous);
    is_finished = false;
    load_world(level_number);
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
    GLfloat color[3] = {0, 1, 0};
    while(body != NULL)
    {
        draw_polygon(body, color);
        draw_circle(body);
        body = body->GetNext();
    }

    // Show rendered frame
    glutSwapBuffers();

    if (is_finished) {
        current_world++;
        go_to_level(current_world);
    }

    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    if (is_active)
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
        case ' ':
            is_active = true;
            break;
        case 'r':
            go_to_level(current_world);
            break;
        default:
            break;
    }
}

/*
 * Called when the user clicked (or released) a mouse buttons inside the window.
 */
void mouse_clicked(int button, int state, int x, int y)
{
    
    if (button == 0 && state == 0) {
        point_t point;
        point.x = x;
        point.y = y;
        points[point_count] = reso_to_world(point);
        point_count++;
        
        if (point_count == 4) {
            poly_t poly;
            poly.is_dynamic = true;
            poly.num_verts = 4;
            poly.verts = points;
            poly.position.x = 0;
            poly.position.y = 0;
            printf("px %f py %f\n", poly.position.x, poly.position.y);
            create_polygon(&poly);
            point_count = 0;
        }
        //create_point(point);
        
    }
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
    MyContactListener listener;
    world->SetContactListener(&listener);
    load_world(current_world);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    frame_count = 0;
    glutMainLoop();

    return 0;
}

void MyContactListener::BeginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody(); 
    if ((bodyA == finish &&  bodyB == player) ||
        (bodyA == player && bodyB == finish)) {
        is_finished = true;
    }
}
