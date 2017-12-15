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

// TODO: gebruik VBO

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

// The current level of the game
int current_level = 3;
// Specifies if the player reached the finish
bool is_finished = false;
// Specifies if the level paused
bool is_paused = false;

// Points used for the user defined polygons
point_t points[4];
int point_count = 0;

/* *Create a b2PolygonShape from a list of poly's */
void load_polyshape(b2PolygonShape *shape, poly_t *poly) {
    b2Vec2 *vertices = new b2Vec2[poly->num_verts];
    for (unsigned int i = 0; i < poly->num_verts; i++) {
        vertices[i].Set(poly->verts[i].x, poly->verts[i].y);
    }
    shape->Set(vertices, poly->num_verts);
    delete vertices;
}

/* Create the player object */
void create_player(point_t start) {
    // Body definition
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(start.x, start.y);
    
    // Shape
    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = 0.15f;

    // Fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    player = world->CreateBody(&bodyDef);
    player->CreateFixture(&fixtureDef);
}

/* Create the finish of the level */
void create_finish(point_t end) {
    // Body definition
    b2BodyDef bodyDef;
    bodyDef.position.Set(end.x, end.y);

    // Shape
    b2PolygonShape shape;
    shape.SetAsBox(0.05,0.05);

    finish = world->CreateBody(&bodyDef);
    finish->CreateFixture(&shape, 0.0f);
}

/* Create a polygon for the level */
b2Body* create_polygon(poly_t *poly) {
    // Body definition
    b2BodyDef bodyDef;
    if (poly->is_dynamic) {
        bodyDef.type = b2_dynamicBody;
    }
    bodyDef.position.Set(poly->position.x, poly->position.y);

    // Shape
    b2PolygonShape shape;
    load_polyshape(&shape, poly);

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&shape, 1.0f);
    return body;
}

/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */
void load_level(unsigned int level_number)
{
    if (level_number >= num_levels)
    {
        printf("Warning: level %d does not exist.\n", level_number);
        return;
    }


    level_t *level = levels + level_number;
    // Array of b2Body pointers
    b2Body **bodies = new b2Body*[level->num_polygons];

    for (unsigned int i = 0; i < level->num_polygons; i++) {
        poly_t *poly = level->polygons + i;
        bodies[i] = create_polygon(poly);
    }

    for (unsigned int i = 0; i < level->num_joints; i++) {
        // Create the revolute joints
        if (level->joints[i].joint_type == JOINT_REVOLUTE) {
            b2RevoluteJointDef jointDef;
            b2Body* bodyA = bodies[level->joints[i].objectA];
            b2Body* bodyB = bodies[level->joints[i].objectB];

            float anchor_x = level->joints[i].anchor.x;
            float anchor_y = level->joints[i].anchor.y;
            jointDef.localAnchorA.Set(anchor_x - bodyA->GetPosition().x,
                                      anchor_y - bodyA->GetPosition().y);
            jointDef.localAnchorB.Set(anchor_x - bodyB->GetPosition().x,
                                      anchor_y - bodyB->GetPosition().y);
            
            jointDef.bodyA = bodyA;
            jointDef.bodyB = bodyB;
            world->CreateJoint(&jointDef);
        }
        // Create the pulley joints
        else if (level->joints[i].joint_type == JOINT_PULLEY) {
            b2PulleyJointDef jointDef;
            joint_t joint = level->joints[i];

            b2Body* bodyA = bodies[joint.objectA];
            b2Body* bodyB = bodies[joint.objectB];

            jointDef.Initialize(bodyA, bodyB, 
                                b2Vec2(joint.pulley.ground1.x, joint.pulley.ground1.y), 
                                b2Vec2(joint.pulley.ground2.x, joint.pulley.ground2.y),
                                b2Vec2(joint.anchor.x, joint.anchor.y),
                                b2Vec2(joint.pulley.anchor2.x, joint.pulley.anchor2.y),
                                joint.pulley.ratio);
            
            world->CreateJoint(&jointDef);

        }
        else
            continue;
    }

    create_player(level->start);
    create_finish(level->end);
}

void draw_polygon(b2Body *body, GLfloat *color) {
    b2PolygonShape *shape = (b2PolygonShape *)body->GetFixtureList()->GetShape();
    int count = shape->GetVertexCount();

    glColor3f(color[0], color[1], color[2]);
    glPushMatrix();
        // Perform a rigid body motion
        glTranslatef(body->GetPosition().x, body->GetPosition().y, 0);
        glRotatef(body->GetAngle() * 180.0/M_PI, 0,0,1);
        glBegin(GL_POLYGON);
            // Add the vertices to the opengl buffer
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

    // Don't draw a circle if it doesn't have a radius
    if (radius <= 0.01f) {
        return;
    }

    GLfloat twicePi = 2.0f * M_PI;
    GLfloat x = circle->GetPosition().x;
    GLfloat y = circle->GetPosition().y;
    
    glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
        // Circle center
		glVertex2f(x, y);
		for(i = 0; i <= triangleAmount;i++) { 
			glVertex2f(x + (radius * cos(i *  twicePi / triangleAmount)), 
			           y + (radius * sin(i * twicePi / triangleAmount)));
		}
	glEnd();
}

void destroy_bodies() {
    b2Body *next = world->GetBodyList();
    if (next == NULL) {
        return;
    }
    b2Body *previous = next; 
    next = next->GetNext();

    while (next != NULL) {
        world->DestroyBody(previous);
        previous = next;
        next = next->GetNext();
    }
    if (previous != NULL)
        world->DestroyBody(previous);
}

void destroy_joints() {
    b2Joint *next = world->GetJointList();
    if (next == NULL) {
        return;
    }
    b2Joint *previous = next; 
    next = next->GetNext();

    while (next != NULL) {
        world->DestroyJoint(previous);
        previous = next;
        next = next->GetNext();
    }
    
    if (previous != NULL)
        world->DestroyJoint(previous);
}

void go_to_level(int level_number) {
    is_paused = false;

    // Destroy all joints before destroying the bodies
    destroy_joints();
    destroy_bodies();
    

    is_finished = false;
    load_level(level_number);
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
    color[1] = 0;
    color[2] = 1;
    draw_polygon(finish, color);

    // Show rendered frame
    glutSwapBuffers();

    if (is_finished) {
        current_level++;
        if (current_level >= 5) {
            printf("Congratulations, you have completed the game!\n");
            exit(0);
        } else{
            go_to_level(current_level);
        }
    }

    if (is_paused)
        world->Step(1.0f / 60.0f, 6, 2);

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
        // Toggle pause
        case ' ':
            is_paused = (bool)((int)is_paused ^ 1);
            break;
        // Restart the level
        case 'r':
            go_to_level(current_level);
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
            create_polygon(&poly);
            point_count = 0;
        }
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
    load_level(current_level);

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
