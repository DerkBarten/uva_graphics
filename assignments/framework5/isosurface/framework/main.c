/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
 *
 * Filename ........ main.c
 * Description ..... Creates OpenGL window and draws the scene.
 * Date ............ 29.10.2007
 * Created by ...... Paul Melis
 *
 * Student name .... Derk Barten
 * Student email ... derk.barten@student.uva.nl
 * Collegekaart .... 11043075
 * Date ............
 * Comments ........
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "volume.h"
#include "marching_tetrahedra.h"
#include "v3math.h"

int             display_mode=2;            /* 0=points, 1=cubes, 2=isosurface */
unsigned char   isovalue=128;
unsigned char   epsilon=0;

int             window;
int             mouse_mode=0;
int             mx, my;
float           camDistance=100.0;
float           camRotZ=45.0, camAzimuth=20.0;
float           saved_camRotZ, saved_camAzimuth, saved_camDistance;
int             regenerate_arrays=1;
int             entered_number=0;
int             display_fps_counter=0;
int             backface_culling=0;

struct timeval  t0, t1;
int             frames_drawn=0;
double          time_used=0.0;

#define MAX_VERTICES_IN_ARRAY   5000000

int             num_vertices_in_array=0;
GLfloat         vertices[3*MAX_VERTICES_IN_ARRAY];
GLfloat         normals[3*MAX_VERTICES_IN_ARRAY];

GLuint axesBuffer, vertBuffer, normalBuffer;

void    SetupCamera(void);
void    DrawVolumeAxes(void);
void    DrawVolumeUsingCurrentDisplayMode(void);

void ClearArrays(void)
{
    num_vertices_in_array = 0;
}

void AddVertexToArray(vec3 v, vec3 n)
{
    vertices[3*num_vertices_in_array] = v.x;
    vertices[3*num_vertices_in_array+1] = v.y;
    vertices[3*num_vertices_in_array+2] = v.z;

    normals[3*num_vertices_in_array] = n.x;
    normals[3*num_vertices_in_array+1] = n.y;
    normals[3*num_vertices_in_array+2] = n.z;

    num_vertices_in_array++;
}

void FillArrayWithPoints(void)
{
    int i, j, k;
    int idx;

    idx = 0;

    for (k = 0; k < nz; k++)
    {
        for (j = 0; j < ny; j++)
        {
            for (i = 0; i < nx; i++)
            {
                if (abs(volume[idx]-isovalue) <= epsilon)
                {
                    AddVertexToArray(
                        v3_create((i+0.5)*sizex, (j+0.5)*sizey, (k+0.5)*sizez),
                        v3_create(0, 0, 0)
                    );
                }

                idx++;
            }
        }
    }
}

void FillArrayWithCubes(void)
{
    int i, j, k;
    int idx;
    float   minx, miny, minz;
    float   maxx, maxy, maxz;

    idx = 0;

    for (k = 0; k < nz; k++)
    {
        for (j = 0; j < ny; j++)
        {
            for (i = 0; i < nx; i++)
            {
                if (abs(volume[idx]-isovalue) <= epsilon)
                {
                    if(!(num_vertices_in_array < MAX_VERTICES_IN_ARRAY - 36)) {
                        printf("Vertex array size limit reached. Volume incomplete!\n");
                        return;
                    }
                    minx = i*sizex;
                    miny = j*sizey;
                    minz = k*sizez;

                    maxx = (i+1)*sizex;
                    maxy = (j+1)*sizey;
                    maxz = (k+1)*sizez;

                    // Create 2 triangles per surface.
                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(0, 0, -1));
                    AddVertexToArray(v3_create(maxx, miny, minz), v3_create(0, 0, -1));
                    AddVertexToArray(v3_create(maxx, maxy, minz), v3_create(0, 0, -1));
                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(0, 0, -1));
                    AddVertexToArray(v3_create(maxx, maxy, minz), v3_create(0, 0, -1));
                    AddVertexToArray(v3_create(minx, maxy, minz), v3_create(0, 0, -1));

                    AddVertexToArray(v3_create(minx, miny, maxz), v3_create(0, 0, 1));
                    AddVertexToArray(v3_create(maxx, miny, maxz), v3_create(0, 0, 1));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(0, 0, 1));
                    AddVertexToArray(v3_create(minx, miny, maxz), v3_create(0, 0, 1));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(0, 0, 1));
                    AddVertexToArray(v3_create(minx, maxy, maxz), v3_create(0, 0, 1));

                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(0, -1, 0));
                    AddVertexToArray(v3_create(maxx, miny, minz), v3_create(0, -1, 0));
                    AddVertexToArray(v3_create(maxx, miny, maxz), v3_create(0, -1, 0));
                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(0, -1, 0));
                    AddVertexToArray(v3_create(maxx, miny, maxz), v3_create(0, -1, 0));
                    AddVertexToArray(v3_create(minx, miny, maxz), v3_create(0, -1, 0));

                    AddVertexToArray(v3_create(minx, maxy, minz), v3_create(0, 1, 0));
                    AddVertexToArray(v3_create(maxx, maxy, minz), v3_create(0, 1, 0));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(0, 1, 0));
                    AddVertexToArray(v3_create(minx, maxy, minz), v3_create(0, 1, 0));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(0, 1, 0));
                    AddVertexToArray(v3_create(minx, maxy, maxz), v3_create(0, 1, 0));

                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(-1, 0, 0));
                    AddVertexToArray(v3_create(minx, maxy, minz), v3_create(-1, 0, 0));
                    AddVertexToArray(v3_create(minx, maxy, maxz), v3_create(-1, 0, 0));
                    AddVertexToArray(v3_create(minx, miny, minz), v3_create(-1, 0, 0));
                    AddVertexToArray(v3_create(minx, maxy, maxz), v3_create(-1, 0, 0));
                    AddVertexToArray(v3_create(minx, miny, maxz), v3_create(-1, 0, 0));

                    AddVertexToArray(v3_create(maxx, miny, minz), v3_create(1, 0, 0));
                    AddVertexToArray(v3_create(maxx, maxy, minz), v3_create(1, 0, 0));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(1, 0, 0));
                    AddVertexToArray(v3_create(maxx, miny, minz), v3_create(1, 0, 0));
                    AddVertexToArray(v3_create(maxx, maxy, maxz), v3_create(1, 0, 0));
                    AddVertexToArray(v3_create(maxx, miny, maxz), v3_create(1, 0, 0));
                }

                idx++;
            }
        }
    }
}

void FillArrayWithIsosurface(void)
{
    int i, j, k, l, n;
    cell c;
    triangle triangles[12];

    for (k = 0; k < nz; k++)
    {
        for (j = 0; j < ny; j++)
        {
            for (i = 0; i < nx; i++)
            {
               c = get_cell(i, j, k); 
               n = generate_cell_triangles(triangles, c, isovalue);
               for (l = 0; l < n; l++) {
                   AddVertexToArray(*triangles[l].p, *triangles[l].n);
               }
            }
        }
    }
}

void DrawScene(void)
{
    if (frames_drawn == 0)
    {
        /* start timing */
        glFinish();
        gettimeofday(&t0, NULL);
    }

    /* clear the draw buffer */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    /* set the model-view matrix */
    SetupCamera();

    /* draw an outline of where the volume resides */
    DrawVolumeAxes();

    /* draw the volume in the chosen mode */
    DrawVolumeUsingCurrentDisplayMode();

    /* FPS timer code */

    frames_drawn++;

    if (frames_drawn == 30)
    {
        glFinish();
        gettimeofday(&t1, NULL);

        /* calculate time used for 30 frames */
        double diff = (t1.tv_sec + t1.tv_usec/1000000.0) - (t0.tv_sec + t0.tv_usec/1000000.0);

        if (display_fps_counter)
            printf("%.1f fps (%f secs for %d frames)\n", frames_drawn/diff, diff, frames_drawn);

        frames_drawn = 0;
    }

    /* finally, swap the draw buffers to make them appear on screen */
    glutSwapBuffers();
}

/* Draw a set of colored X, Y and Z axes that match the size (and location)
   of the volume, so we can see where output should appear */
void
DrawVolumeAxes(void)
{
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, axesBuffer);

    // X axis.
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glColor3f(1, 0, 0);
    glDrawArrays(GL_LINES, 0, 2);

    // Y axis.
    glVertexPointer(3, GL_FLOAT, 0, (void*) 24);
    glColor3f(0, 1, 0);
    glDrawArrays(GL_LINES, 0, 2);

    // Z axis.
    glVertexPointer(3, GL_FLOAT, 0, (void*) 48);
    glColor3f(0, 0, 1);
    glDrawArrays(GL_LINES, 0, 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopAttrib();
}

void DrawVolumeUsingCurrentDisplayMode(void)
{
    if (regenerate_arrays == 1)
    {
        printf("Filling vertex/normal arrays... ");
        fflush(stdout);

        ClearArrays();

        switch (display_mode)
        {
        case 0:
            FillArrayWithPoints();
            break;
        case 1:
            FillArrayWithCubes();
            break;
        case 2:
            FillArrayWithIsosurface();
            break;
        }

        // Update the VBO data with the new vertices and normals.
        glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        num_vertices_in_array * 3 * sizeof(GLfloat), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        num_vertices_in_array * 3 * sizeof(GLfloat), normals);

        printf("done, %d array vertices used (1 per point, 6 per quad, 3 per triangle)\n", num_vertices_in_array);

        regenerate_arrays = 0;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glColor3f(0.6, 0.0, 0.0);

    if (display_mode > 0)
    {
        /* cubes or isosurfaces */
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glNormalPointer(GL_FLOAT, 0, 0);

        glEnable(GL_LIGHTING);

        glDrawArrays(GL_TRIANGLES, 0, num_vertices_in_array);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    else {
        /* points */
        glDisable(GL_LIGHTING);

        glPointSize(8.0);
        glDrawArrays(GL_POINTS, 0, num_vertices_in_array);
    }


    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SetupCamera(void)
{
    GLfloat light_position[4];

    glLoadIdentity();

    // Verbose, but straightforward way, of positioning the camera and lightsource.
    // Assume the camera's final position is (cx, cy, cz).
    // Start with c being (camDistance, 0, 0)
    // First rotate around Y, then around Z.
    // Now we have c at the given distance from the origin, with specified rotation angles.

    float	cx, cy, cz;
    float	t;
    float 	beta, gamma;

    // degrees -> radians
    beta = camAzimuth / 180.0 * 3.1415926535;
    gamma = camRotZ / 180.0 * 3.1415926535;

    cx = camDistance;
    cy = cz = 0.0;

    // Rotate around Y
    t = cx;
    cx = cx * cos(beta) - cz * sin(beta);
    cz = t * sin(beta) + cz * cos(beta);

    // Rotate around Z
    t = cx;
    cx = cx * cos(gamma) + cy * sin(gamma);
    cy = -t * sin(gamma) + cy * cos(gamma);

    gluLookAt (cx, cy, cz,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

    // Finally, translate because the lookat point is at the center of the volume.
    glTranslatef(-0.5*nx*sizex, -0.5*ny*sizey, -0.5*nz*sizez);

    // Place the light source at the camera position (head light)
    light_position[0] = cx + 0.5*nx*sizex;
    light_position[1] = cy + 0.5*ny*sizey;
    light_position[2] = cz + 0.5*nz*sizez;
    light_position[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void InitOpenGL(void)
{
    GLfloat light_ambient[] = { 0.4, 0.4, 0.4, 0.0 };
    GLfloat	red[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_no_specular[] = { 0.0, 0.0, 0.0, 1.0 };

    glClearColor(0.7, 0.7, 1, 1);

    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_no_specular);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

    glShadeModel(GL_FLAT);
    //glShadeModel(GL_SMOOTH);

    GLfloat axes[18] = {
        0.0, 0.0, 0.0,
        nx * sizex, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, ny * sizey, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, nz * sizez
    };

    glGenBuffers(1, &axesBuffer);
    glGenBuffers(1, &vertBuffer);
    glGenBuffers(1, &normalBuffer);

    // Axes vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, axesBuffer);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), axes, GL_STATIC_DRAW);

    // Isosurface vertes and normal buffers.
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_IN_ARRAY * 3 * sizeof(GLfloat),
                 vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_IN_ARRAY * 3 * sizeof(GLfloat),
                 normals, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReSizeScene(int Width, int Height)
{
    if (Height==0)
        Height=1;

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height, 0.1f, 10000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
}

void keyPressed(unsigned char key, int x, int y)
{
    int	redraw=0;

    /* keep gcc happy, as we don't use these parameters */
    (void)x;
    (void)y;

    switch (key)
    {
        case 'q':
            exit(-1);
            break;

        case 'b':
            backface_culling = 1 - backface_culling;
            if (backface_culling)
            {
                glEnable(GL_CULL_FACE);
                printf("Backface culling is now enabled\n");
            }
            else
            {
                glDisable(GL_CULL_FACE);
                printf("Backface culling is now disabled\n");
            }
            glutPostRedisplay();
            break;

        case 'p':
            /* switch to points mode */
            display_mode = 0;
            redraw = 1;
            break;
        case 'c':
            /* switch to cubes mode */
            display_mode = 1;
            redraw = 1;
            break;
        case 's':
            /* switch to isosurface mode */
            display_mode = 2;
            redraw = 1;
            break;

        case '[':
            /* decrease isovalue */
            {
                int newvalue = isovalue-10;
                if (newvalue < 0)
                    isovalue = 0;
                else
                    isovalue = newvalue;
            }
            printf("Setting isovalue to %d\n", isovalue);
            redraw = 1;
            break;
        case ']':
            /* increase isovalue */
            {
                int newvalue = isovalue+10;
                if (newvalue > 255)
                    isovalue = 255;
                else
                    isovalue = newvalue;
            }
            printf("Setting isovalue to %d\n", isovalue);
            redraw = 1;
            break;

        /* handle isovalue/epsilon entered by number */
        case '0':
            entered_number = entered_number*10;
            break;
        case '1':
            entered_number = entered_number*10 + 1;
            break;
        case '2':
            entered_number = entered_number*10 + 2;
            break;
        case '3':
            entered_number = entered_number*10 + 3;
            break;
        case '4':
            entered_number = entered_number*10 + 4;
            break;
        case '5':
            entered_number = entered_number*10 + 5;
            break;
        case '6':
            entered_number = entered_number*10 + 6;
            break;
        case '7':
            entered_number = entered_number*10 + 7;
            break;
        case '8':
            entered_number = entered_number*10 + 8;
            break;
        case '9':
            entered_number = entered_number*10 + 9;
            break;

        case 'i':
            /* update isovalue with number entered */
            isovalue = entered_number % 256;
            entered_number = 0;

            printf("Setting isovalue to %d\n", isovalue);
            redraw = 1;
            break;

        case '{':
            /* decrease epsilon */
            {
                int newvalue = epsilon - 2;
                if (newvalue < 0)
                    epsilon = 0;
                else
                    epsilon = newvalue;
            }
            printf("Setting epsilon to %d\n", epsilon);
            redraw = 1;
            break;
        case '}':
            /* increase epsilon */
            {
                int newvalue = epsilon + 2;
                if (newvalue > 255)
                    epsilon = 255;
                else
                    epsilon = newvalue;
            }
            printf("Setting epsilon to %d\n", epsilon);
            redraw = 1;
            break;

        case 'e':
            /* update epsilon with number entered */
            epsilon = entered_number % 256;
            entered_number = 0;

            printf("Setting epsilon to %d\n", epsilon);
            redraw = 1;
            break;


        case 'f':
            /* enable/display FPS counter display */
            display_fps_counter = 1 - display_fps_counter;
            break;
    }

    if (redraw)
    {
        regenerate_arrays = 1;
        glutPostRedisplay();
    }
}

void specialKeyPressed(int key, int x, int y)
{
    /* keep gcc happy, as we don't use these parameters */
    (void)key;
    (void)x;
    (void)y;
}

static void
mouseFunc(int button, int state, int x, int y)
{
    // guard against both left and right buttons being pressed at the same time,
    // by only responding when a mouse button is pressed while another one
    // hasn't been pressed yet
    if (state == GLUT_DOWN && mouse_mode == 0)
    {
        if (button == GLUT_LEFT_BUTTON)
        {
            mouse_mode = GLUT_LEFT_BUTTON;

            saved_camRotZ = camRotZ;
            saved_camAzimuth = camAzimuth;

            mx = x;
            my = y;
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            mouse_mode = GLUT_RIGHT_BUTTON;

            saved_camDistance = camDistance;

            my = y;
        }
    }
    else if (state == GLUT_UP && button == mouse_mode)
    {
        // pressed button released
        mouse_mode = 0;
    }

}

static void
motionFunc(int x, int y)
{
    int	dx, dy;

    if (mouse_mode == GLUT_LEFT_BUTTON)
    {
        dx = mx - x;
        dy = my - y;

        camRotZ = saved_camRotZ - dx * 0.25;
        camAzimuth = saved_camAzimuth - dy * 0.25;

        if (camAzimuth > 89.99)
            camAzimuth = 89.99;
        else if (camAzimuth < -89.99)
            camAzimuth = -89.99;
    }
    else if (mouse_mode == GLUT_RIGHT_BUTTON)
    {
        dy = my - y;

        camDistance = saved_camDistance - dy * sizex;
        if (camDistance < 0.01)
            camDistance = 0.01;
    }
}

static void
initialize_volume(const char *fname, unsigned char iso)
{
    read_volume(fname);

    isovalue = iso;
    if (epsilon > 0)
        printf("Iso-value = %d (+/- %d)\n", isovalue, epsilon);
    else
        printf("Iso-value = %d\n", isovalue);

    // Initial distance from camera position to center of volume
    camDistance = 2.0 * nx * sizex;
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    if (argc != 3)
    {
        printf("usage: %s datafile isovalue\n", argv[0]);
        exit(0);
    }

    initialize_volume(argv[1], atoi(argv[2]));

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("OpenGL Framework");

    glewInit();

    glutDisplayFunc(&DrawScene);
    glutIdleFunc(&DrawScene);
    glutReshapeFunc(&ReSizeScene);
    glutSpecialFunc(&specialKeyPressed);
    glutKeyboardFunc(&keyPressed);
    glutMouseFunc(&mouseFunc);
    glutMotionFunc(&motionFunc);

    InitOpenGL();

    glutMainLoop();

    return 1;
}