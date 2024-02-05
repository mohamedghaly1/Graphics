#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <windows.h>
#include <glut.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#define M_PI 3.14159265

int WIDTH = 1280;
int HEIGHT = 720;
GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

float player_x = 0; float player_z = 0; float player_y = 2.2;
int rotate_player = 0;
boolean playerP = true;
bool isJumping = false; // Flag to track if the player is jumping
float initialPlayerHeight = 2.2f; // Initial height of the player
boolean del = false;

float Rotation = 0.0f;
int coinMotion[] = { 0,0,0,0 };

float cats[][2] = {
	{5, -13},
	{-5, 13},
	{10, 3},
	{-10, -4},
};
float arcades[][2] = {
	{0, -18.5},
	{0, 18.5},
	{18, -10},
	{18, 5},
};
float thiefs[][2] = {
	{5, -10},
	{8, 10},
};

int level1_score = 0;
int level2_score = 0;
boolean level2 = false;
bool gameOver1 = false;
bool gameOver2 = false;
bool win = false;

bool cameraP = false;

GLfloat lightIntensity[] = { 0.5, 0.5, 0.5, 1.0f };
GLfloat maxIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
GLfloat minIntensity[] = { 0.4, 0.4, 0.4, 1.0f };
bool increaseIntensity = true;
float lightposition = 0.0f;
std::chrono::time_point<std::chrono::steady_clock> intensityChangeTime = std::chrono::steady_clock::now();
std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();


//=======================================================================
// Helper Methods
//=======================================================================
void updateLightingIntensity() {
	std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsedSeconds = currentTime - intensityChangeTime;

	if (elapsedSeconds.count() >= 2) {
		intensityChangeTime = currentTime;
		if (level2) {
			if (increaseIntensity) {
				for (int i = 0; i < 3; ++i) {
					if (lightIntensity[i] < maxIntensity[i]) {
						lightIntensity[i] += 0.1f;
					}
				}
				if (lightIntensity[0] >= maxIntensity[0]) {
					increaseIntensity = false;
				}
			}
			else {
				for (int i = 0; i < 3; ++i) {
					if (lightIntensity[i] > minIntensity[i]) {
						lightIntensity[i] -= 0.1f;
					}
				}
				if (lightIntensity[0] <= minIntensity[0]) {
					increaseIntensity = true;
				}
			}
		}
		else {
			for (int i = 0; i < 3; ++i) {
				if (lightIntensity[i] < maxIntensity[i]) {
					lightIntensity[i] = maxIntensity[i];
				}
				else {
					lightIntensity[i] = minIntensity[i];
				}
			}
		}
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
	}
}

void renderBitmapString2(float x, float y, void* font, const std::string& string) {
	glRasterPos2f(x, y);
	for (const char& c : string) {
		glutBitmapCharacter(font, c);
	}
}

// Function to simulate a delay in the game (e.g., for sound and animation)
void delay(int seconds) {
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// Function to render text on the screen
void renderBitmapString(float x, float y, void* font, std::string string) {
	glRasterPos2f(x, y);
	for (char& c : string) {
		glutBitmapCharacter(font, c);
	}
}

// Function to convert integer to string
std::string intToString(int number) {
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

// Function to handle the player's jump action
void handleJump() {
	const float jumpHeight = 2.0f; // Set the jump height (adjust as needed)
	const float jumpSpeed = 0.1f; // Set the jump speed (adjust as needed)

	if (isJumping) {
		// Increase the player's y-coordinate for the jumping effect
		player_y += jumpSpeed;

		// Check if the player has reached the maximum jump height
		if (player_y >= initialPlayerHeight + jumpHeight) {
			// Reset jump-related variables when the jump is complete
			isJumping = false;
			player_y = initialPlayerHeight; // Reset the player's y-coordinate to the initial height
		}
	}
}

bool canMove(float newX, float newZ) {

	float playerBoundaryLeft = newX - 3 / 2;
	float playerBoundaryRight = newX + 3 / 2;
	float playerBoundaryTop = newZ - 3 / 2;
	float playerBoundaryBottom = newZ + 3 / 2;

	if (!level2) {
		for (int i = 0; i < 4; ++i) {
			float catX = cats[i][0];
			float catZ = cats[i][1];

			float catBoundaryLeft = catX - 1 / 2;
			float catBoundaryRight = catX + 1 / 2;
			float catBoundaryTop = catZ - 1 / 2;
			float catBoundaryBottom = catZ + 1 / 2;

			// Check for collision between player and cat boundaries
			if (playerBoundaryLeft < catBoundaryRight &&
				playerBoundaryRight > catBoundaryLeft &&
				playerBoundaryTop < catBoundaryBottom &&
				playerBoundaryBottom > catBoundaryTop) {
				if (level1_score >= 10) { level1_score -= 10; }
				PlaySound("sounds/cat.wav", NULL, SND_FILENAME | SND_ASYNC);
				GLfloat newMaya[] = { 1,0,0,1 };
				glLightfv(GL_LIGHT0, GL_AMBIENT, newMaya);
				
				delay(1);
				//del=true;
				//GLfloat newMaya2[] = { 0,0,0,1 };
				//glLightfv(GL_LIGHT0, GL_AMBIENT, newMaya2);
				//delay(1);
				//GLfloat newMaya2[] = { 0,0,0,1 };
				//glLightfv(GL_LIGHT0, GL_AMBIENT, newMaya2);
				
				for (int i = 0; i < 4; ++i) {
					if (cats[i][0] == catX && cats[i][1] == catZ) {
						cats[i][0] = -60.0;
						break;
					}
				}
				return false; // Collision detected
			}
		}

		for (int i = 0; i < 4; ++i) {
			float arcadeX = arcades[i][0];
			float arcadeZ = arcades[i][1];

			float arcadeBoundaryLeft = arcadeX - 3 / 2;
			float arcadeBoundaryRight = arcadeX + 3 / 2;
			float arcadeBoundaryTop = arcadeZ - 3 / 2;
			float arcadeBoundaryBottom = arcadeZ + 3 / 2;

			// Check for collision between player and arcade boundaries
			if (playerBoundaryLeft < arcadeBoundaryRight &&
				playerBoundaryRight > arcadeBoundaryLeft &&
				playerBoundaryTop < arcadeBoundaryBottom &&
				playerBoundaryBottom > arcadeBoundaryTop) {
				coinMotion[i] = 5;
				level1_score += 10;
				switch (i) {
				case 0:PlaySound("sounds/ready-fight.wav", NULL, SND_FILENAME | SND_ASYNC); break;
				case 1:PlaySound("sounds/arcade.wav", NULL, SND_FILENAME | SND_ASYNC); break;
				case 2:PlaySound("sounds/arcade2.wav", NULL, SND_FILENAME | SND_ASYNC); break;
				case 3:PlaySound("sounds/arcade2.wav", NULL, SND_FILENAME | SND_ASYNC); break;
				}

				delay(2);
				return false; // Collision detected
			}
		}

		float doorBoundaryLeft = -19 - 1 / 2;
		float doorBoundaryRight = -19 + 1 / 2;
		float doorBoundaryTop = 0 - 3 / 1;
		float doorBoundaryBottom = 0 + 1 / 2;

		// Check for collision between player and door boundaries
		if (playerBoundaryLeft < doorBoundaryRight &&
			playerBoundaryRight > doorBoundaryLeft &&
			playerBoundaryTop < doorBoundaryBottom &&
			playerBoundaryBottom > doorBoundaryTop) {
			//if (level1_score >= 50) {
				PlaySound("sounds/door.wav", NULL, SND_FILENAME | SND_ASYNC);
				//delay(1);
				level2 = true;
			//}
			return false; // Collision detected
		}
	}
	else {
		for (int i = 0; i < 2; ++i) {
			float thiefX = thiefs[i][0];
			float thiefZ = thiefs[i][1];

			float thiefBoundaryLeft = thiefX - 1 / 2;
			float thiefBoundaryRight = thiefX + 1 / 2;
			float thiefBoundaryTop = thiefZ - 1 / 2;
			float thiefBoundaryBottom = thiefZ + 1 / 2;

			// Check for collision between player and thief boundaries
			if (playerBoundaryLeft < thiefBoundaryRight &&
				playerBoundaryRight > thiefBoundaryLeft &&
				playerBoundaryTop < thiefBoundaryBottom &&
				playerBoundaryBottom > thiefBoundaryTop) {
				if (level2_score >= 2) { level2_score -= 2; }
				PlaySound("sounds/steal.wav", NULL, SND_FILENAME | SND_ASYNC);
				delay(2);
				for (int i = 0; i < 2; ++i) {
					if (thiefs[i][0] == thiefX && thiefs[i][1] == thiefZ) {
						thiefs[i][0] = -60.0;
						break;
					}
				}
				return false; // Collision detected
			}
		}

		float bwsBoundaryLeft = 0 - 4 / 2;
		float bwsBoundaryRight = 0 + 4 / 2;
		float bwsBoundaryTop = 15 - 4 / 2;
		float bwsBoundaryBottom = 15 + 4 / 2;

		// Check for collision between player and bws boundaries
		if (playerBoundaryLeft < bwsBoundaryRight &&
			playerBoundaryRight > bwsBoundaryLeft &&
			playerBoundaryTop < bwsBoundaryBottom &&
			playerBoundaryBottom > bwsBoundaryTop) {
			level2_score += 2;
			PlaySound("sounds/wind.wav", NULL, SND_FILENAME | SND_ASYNC);
			delay(3);
			return false; // Collision detected
		}
		float wsBoundaryLeft = -13 - 15 / 2;
		float wsBoundaryRight = -13 + 15 / 2;
		float wsBoundaryTop = 0 - 15 / 2;
		float wsfBoundaryBottom = 0 + 15 / 2;

		// Check for collision between player and water slide boundaries
		if (playerBoundaryLeft < wsBoundaryRight &&
			playerBoundaryRight > wsBoundaryLeft &&
			playerBoundaryTop < wsfBoundaryBottom &&
			playerBoundaryBottom > wsBoundaryTop) {
			level2_score += 2;
			PlaySound("sounds/waterslide.wav", NULL, SND_FILENAME | SND_ASYNC);
			delay(2);
			return false; // Collision detected
		}
		float shopBoundaryLeft = 14 - 5 / 2;
		float shopBoundaryRight = 14 + 5 / 2;
		float shopBoundaryTop = -14 - 5 / 2;
		float shopBoundaryBottom = -14 + 5 / 2;

		// Check for collision between player and shop boundaries
		if (playerBoundaryLeft < shopBoundaryRight &&
			playerBoundaryRight > shopBoundaryLeft &&
			playerBoundaryTop < shopBoundaryBottom &&
			playerBoundaryBottom > shopBoundaryTop) {
			if (level2_score >= 10) {
				PlaySound("sounds/success.wav", NULL, SND_FILENAME | SND_ASYNC);
				delay(2);
				gameOver2 = true; win = true;
			}
			return false; // Collision detected
		}

	}

	return true; // No collision
}

void Update(int value) {
	Rotation += 1.0f; // Increment the rotation angle
	if (Rotation > 360.0f) {
		Rotation -= 360.0f; // Keep the angle within 0-360 range
	}

	glutPostRedisplay();
	glutTimerFunc(10, Update, 0); // Call the update function after a specific time interval
}

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(20, 35, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables Level 1
Model_3DS model_player;
Model_3DS model_arcade1;
Model_3DS model_arcade2;
Model_3DS model_arcade3;
Model_3DS model_animal;
Model_3DS model_door;
Model_3DS model_coin;
Model_3DS model_bws;
Model_3DS model_ws;
Model_3DS model_ff;
Model_3DS model_thief;
Model_3DS model_shop;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;
GLTexture pacman1;
GLTexture pacman2;
GLTexture wall;
GLTexture tex_ground2;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render models Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	if (level2) {
		glColor3f(0.6, 0.6, 0.6);
		glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

		glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

		glPushMatrix();
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-300, 0, -300);
		glTexCoord2f(5, 0);
		glVertex3f(300, 0, -300);
		glTexCoord2f(5, 5);
		glVertex3f(300, 0, 300);
		glTexCoord2f(0, 5);
		glVertex3f(-300, 0, 300);
		glEnd();
		glPopMatrix();

		glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

		glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
	}
	else {
		glColor3f(0.3, 0.2, 0.1); 	// Dim the ground texture a bit

		glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

		glPushMatrix();
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-300, 0, -300);
		glTexCoord2f(5, 0);
		glVertex3f(300, 0, -300);
		glTexCoord2f(5, 5);
		glVertex3f(300, 0, 300);
		glTexCoord2f(0, 5);
		glVertex3f(-300, 0, 300);
		glEnd();
		glPopMatrix();

		glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

		glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
	}
}

void RenderWalls() {
	glDisable(GL_LIGHTING); // Disable lighting

	// Enable 2D texturing
	glEnable(GL_TEXTURE_2D);

	// Bind the wall texture
	glBindTexture(GL_TEXTURE_2D, wall.texture[0]); // Replace tex_wall.texture[0] with the actual wall texture

	// Draw four walls with texture
	glBegin(GL_QUADS);
	// Wall 1 - Front
	glTexCoord2f(0, 0);
	glVertex3f(-20, 0, -20);
	glTexCoord2f(1, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(1, 1);
	glVertex3f(20, 60, -20);
	glTexCoord2f(0, 1);
	glVertex3f(-20, 60, -20);

	// Wall 2 - Right
	glTexCoord2f(0, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(1, 0);
	glVertex3f(20, 0, 20);
	glTexCoord2f(1, 1);
	glVertex3f(20, 60, 20);
	glTexCoord2f(0, 1);
	glVertex3f(20, 60, -20);

	// Wall 3 - Back
	glTexCoord2f(0, 0);
	glVertex3f(20, 0, 20);
	glTexCoord2f(1, 0);
	glVertex3f(-20, 0, 20);
	glTexCoord2f(1, 1);
	glVertex3f(-20, 60, 20);
	glTexCoord2f(0, 1);
	glVertex3f(20, 60, 20);

	// Wall 4 - Left
	glTexCoord2f(0, 0);
	glVertex3f(-20, 0, 20);
	glTexCoord2f(1, 0);
	glVertex3f(-20, 0, -20);
	glTexCoord2f(1, 1);
	glVertex3f(-20, 60, -20);
	glTexCoord2f(0, 1);
	glVertex3f(-20, 60, 20);
	glEnd();

	// Disable 2D texturing after drawing walls
	glDisable(GL_TEXTURE_2D);

	// Set wall colors
	glColor3f(0.8f, 0.8f, 0.8f);

	glEnable(GL_LIGHTING); // Enable lighting again
}

void drawPlayer() {
	glPushMatrix();
	glTranslatef(player_x, player_y, player_z);
	glScalef(0.09, 0.09, 0.09);
	glRotatef(90.f, 1, 0, 0);
	glRotatef(rotate_player, 0, 0, 1);
	model_player.Draw();
	glPopMatrix();
}

void drawArcade() {

	glPushMatrix();
	glTranslatef(arcades[0][0], 0, arcades[0][1]);
	glRotatef(90.f, 0, 1, 0);
	glScalef(0.27, 0.27, 0.27);
	model_arcade1.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(arcades[1][0], 0, arcades[1][1]);
	glScalef(4, 4, 4);
	glRotatef(180.f, 0, 1, 0);
	model_arcade3.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(arcades[2][0], 0, arcades[2][1]);
	glScalef(0.6, 0.6, 0.6);
	glRotatef(-90.f, 0, 1, 0);
	model_arcade2.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(arcades[3][0], 0, arcades[3][1]);
	glScalef(0.6, 0.6, 0.6);
	glRotatef(-90.f, 0, 1, 0);
	model_arcade2.Draw();
	glPopMatrix();
}

void drawAnimal() {
	if (cats[0][0] != -60.0) {
		glPushMatrix();
		glTranslatef(cats[0][0], 0, cats[0][1]);
		glScalef(3, 3, 3);
		glRotatef(Rotation, 0, 1, 0);
		model_animal.Draw();
		glPopMatrix();
	}
	if (cats[1][0] != -60.0) {
		glPushMatrix();
		glTranslatef(cats[1][0], 0, cats[1][1]);
		glScalef(3, 3, 3);
		glRotatef(180.f, 0, 1, 0);
		glRotatef(Rotation, 0, 1, 0);
		model_animal.Draw();
		glPopMatrix();
	}
	if (cats[2][0] != -60.0) {
		glPushMatrix();
		glTranslatef(cats[2][0], 0, cats[2][1]);
		glScalef(3, 3, 3);
		glRotatef(-90.f, 0, 1, 0);
		glRotatef(Rotation, 0, 1, 0);
		model_animal.Draw();
		glPopMatrix();
	}
	if (cats[3][0] != -60.0) {
		glPushMatrix();
		glTranslatef(cats[3][0], 0, cats[3][1]);
		glScalef(3, 3, 3);
		glRotatef(90.f, 0, 1, 0);
		glRotatef(Rotation, 0, 1, 0);
		model_animal.Draw();
		glPopMatrix();
	}
	
	
}

void drawDoor() {
	glPushMatrix();
	glTranslatef(-19, 0, 0);
	glScalef(0.04, 0.04, 0.04);
	glRotatef(90.f, 0, 1, 0);
	model_door.Draw();
	glPopMatrix();
}

void drawCoin() {
	glPushMatrix();
	glTranslatef(0,(5+coinMotion[0]), -18.5);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(90.f, 1, 0, 0);
	model_coin.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,( 5 + coinMotion[1]), 18.5);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(90.f, 1, 0, 0);
	model_coin.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(19, (4 + coinMotion[2]), -9);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(90.f, 1, 0, 0);
	glRotatef(90.f, 0, 0, 1);
	model_coin.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(19, (4 + coinMotion[3]), 6);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(90.f, 1, 0, 0);
	glRotatef(90.f, 0, 0, 1);
	model_coin.Draw();
	glPopMatrix();
}

void drawDoor2() {
	glPushMatrix();
	glTranslatef(19.7, 0, 0);
	glScalef(0.04, 0.04, 0.04);
	glRotatef(90.f, 0, 1, 0);
	model_door.Draw();
	glPopMatrix();
}

void drawThief(){
	if (thiefs[0][0] != -60.0) {
		glPushMatrix();
		glTranslatef(thiefs[0][0], 0, thiefs[0][1]);
		glScalef(0.015, 0.015, 0.015);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(Rotation, 0, 0, 1);
		model_thief.Draw();
		glPopMatrix();
	}
	if (thiefs[1][0] != -60.0) {
		glPushMatrix();
		glTranslatef(thiefs[1][0], 0, thiefs[1][1]);
		glScalef(0.015, 0.015, 0.015);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);
		glRotatef(Rotation, 0, 0, 1);
		model_thief.Draw();
		glPopMatrix();
	}
}

void drawGames() {
	glPushMatrix();
	glTranslatef(0, 0, 15);
	glScalef(3, 3, 3);
	//glRotatef(90.f, 1, 0, 0);
	//glRotatef(rotate_player, 0, 0, 1);
	glRotatef(Rotation, 0, 1, 0);
	model_bws.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-13, 0, 0);
	glScalef(0.06, 0.06, 0.06);
	glRotatef(90.f, 0, 1, 0);
	//glRotatef(rotate_player, 0, 0, 1);
	model_ws.Draw();
	glPopMatrix();


}

void drawShop(){
	glPushMatrix();
	glTranslatef(14, 0, -14);
	//glScalef(0.06, 0.06, 0.06);
	glRotatef(90.f, 1, 0, 0);
	//glRotatef(rotate_player, 0, 0, 1);
	model_shop.Draw();
	glPopMatrix();
}

void drawTree(){

	glPushMatrix();
	glTranslatef(-14, 0, 14);
	glScalef(0.8, 0.8, 0.8);
	//glRotatef(90.f, 1, 0, 0);
	//glRotatef(rotate_player, 0, 0, 1);
	model_tree.Draw();
	glPopMatrix();
}


//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the view matrix for a third-person perspective
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!cameraP) {
		const float distanceBehindPlayer = 8.0f; 
		const float heightAbovePlayer = 5.0f;    

		float camera_x = player_x - distanceBehindPlayer * cos(rotate_player * M_PI / 180.0);
		float camera_y = player_y + heightAbovePlayer;
		float camera_z = player_z - distanceBehindPlayer * sin(rotate_player * M_PI / 180.0);

		float lookAt_x = player_x;
		float lookAt_y = player_y + 1.5f;
		float lookAt_z = player_z;

		// Set up the camera view
		gluLookAt(camera_x, camera_y, camera_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
	}
	else {
		const float distanceBehindPlayer = 1.0f;
		const float heightAbovePlayer = 3.0f;

		float camera_x = player_x;
		float camera_y = player_y + heightAbovePlayer;
		float camera_z = player_z;

		float lookAt_x = player_x - cos(rotate_player * M_PI / 180.0);
		float lookAt_y = player_y + 2.7;
		float lookAt_z = player_z - sin(rotate_player * M_PI / 180.0);

		// Set up the camera view
		gluLookAt(camera_x, camera_y, camera_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
	}



	if (!level2) {
		lightposition += 1.0f;
		if (lightposition>=50) {
			lightposition = -50;
		}
		GLfloat lightPosition[] = { 0.0f, 100.0f,lightposition, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		//updateLightingIntensity();

		//Draw Models Level 1
		RenderGround();
		RenderWalls();
		drawPlayer();
		drawArcade();
		drawAnimal();
		drawDoor();
		drawCoin();
		handleJump();


		int remainingTime = 0;
		if (!gameOver1) {
			std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = currentTime - startTime;
			remainingTime = 300 - static_cast<int>(elapsedTime.count());
			if (remainingTime <= 0) {
				gameOver1 = true;
			}
		}

		// Display the score and timer text
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();


		// Display the score
		glColor3f(1.0f, 1.0f, 1.0f);
		std::string scoreText = "Tickets: " + intToString(level1_score);
		renderBitmapString(10, 20, GLUT_BITMAP_9_BY_15, scoreText);

		// Display the timer
		glColor3f(1.0f, 1.0f, 1.0f);
		std::string timerText = "Time: " + std::to_string(remainingTime) + "s";
		renderBitmapString2(10, 40, GLUT_BITMAP_9_BY_15, timerText);

		if (gameOver1) {

			//glColor3f(1.0f, 0.0f, 0.0f); // Red color for the message
			std::string lostMessage = "You Lost!";
			int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)lostMessage.c_str());
			renderBitmapString(10, 60, GLUT_BITMAP_9_BY_15, lostMessage);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);


	}
	else {
		lightposition += 0.1f;
		if (lightposition >= 50) {
			lightposition = -50;
		}
		GLfloat lightPosition[] = { lightposition, 100.0f,0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		updateLightingIntensity();

		//Draw Models Level 2
		RenderGround();
		//RenderWalls();
		if (playerP) { player_x = 18; player_z = 0; playerP = false; }
		drawPlayer();
		drawDoor2();
		drawThief();
		drawGames();
		drawTree();
		drawShop();
		handleJump();


		int remainingTime = 0;
		if (!gameOver2) {
			std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = currentTime - startTime;
			remainingTime = 300 - static_cast<int>(elapsedTime.count());
			if (remainingTime <= 0) {
				gameOver2 = true;
			}
		}

		// Display the score and timer text
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Display the score
		glColor3f(1.0f, 1.0f, 1.0f);
		std::string scoreText = "Coins: " + intToString(level2_score);
		renderBitmapString(10, 20, GLUT_BITMAP_9_BY_15, scoreText);

		// Display the timer
		glColor3f(1.0f, 1.0f, 1.0f);
		std::string timerText = "Time: " + std::to_string(remainingTime) + "s";
		renderBitmapString2(10, 40, GLUT_BITMAP_9_BY_15, timerText);

		if (gameOver2) {
			//glColor3f(1.0f, 0.0f, 0.0f); // Red color for the message
			std::string lostMessage = "You Lost!";
			if (win) {lostMessage = "You Won!"; }
			int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)lostMessage.c_str());
			renderBitmapString(10, 60, GLUT_BITMAP_9_BY_15, lostMessage);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

	}



	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();



	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y) {
	float movementSpeed = 0.5f;//0.2
	if (!gameOver1 && !gameOver2) {
		switch (button) {
		case 'w': // Move model forward
			//if(player_x - movementSpeed == -19){ PlaySound("sounds/hit.wav", NULL, SND_FILENAME | SND_ASYNC); }
			if (player_x - movementSpeed >= -19 && canMove(player_x - movementSpeed, player_z)) {
				player_x -= movementSpeed; rotate_player = 0; coinMotion[0] = 0; coinMotion[1] = 0; coinMotion[2] = 0; coinMotion[3] = 0;
			}
			break;
		case 's': // Move model backward
			//if (player_x + movementSpeed == 19) { PlaySound("sounds/hit.wav", NULL, SND_FILENAME | SND_ASYNC); }
			if (player_x + movementSpeed <= 19 && canMove(player_x + movementSpeed, player_z)) {
				player_x += movementSpeed; rotate_player = 180; coinMotion[0] = 0; coinMotion[1] = 0; coinMotion[2] = 0; coinMotion[3] = 0;
			}
			break;
		case 'a': // Move model left
			//if (player_z + movementSpeed == 19) { PlaySound("sounds/hit.wav", NULL, SND_FILENAME | SND_ASYNC); }
			if (player_z + movementSpeed <= 19 && canMove(player_x, player_z + movementSpeed)) {
				player_z += movementSpeed; rotate_player = -90; coinMotion[0] = 0; coinMotion[1] = 0; coinMotion[2] = 0; coinMotion[3] = 0;
			}
			break;
		case 'd': // Move model right
			//if (player_z - movementSpeed == -19) { PlaySound("sounds/hit.wav", NULL, SND_FILENAME | SND_ASYNC); }
			if (player_z - movementSpeed >= -19 && canMove(player_x, player_z - movementSpeed)) {
				player_z -= movementSpeed; rotate_player = 90; coinMotion[0] = 0; coinMotion[1] = 0; coinMotion[2] = 0; coinMotion[3] = 0;
			}
			break;
		case 'r':
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
		}
	}
	glutPostRedisplay(); // Request a redraw
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	Eye.y += 0.1;

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, lightposition, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Perform jump action here
		isJumping = true; // Set the flag to indicate the player is jumping
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (cameraP) {
			cameraP = false;
		}
		else {
			cameraP = true;
		}
	}

	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_player.Load("Models/player/player.3DS");
	model_arcade1.Load("Models/arcade/BlackArcade.3DS");
	model_arcade2.Load("Models/arcade2/pacmanArcade.3DS");
	model_arcade3.Load("Models/arcade/BlackArcade2.3DS");
	model_animal.Load("Models/cat/CatMac.3DS");
	model_door.Load("Models/door/Door.3DS");
	model_coin.Load("Models/coin/Coin.3DS");
	model_bws.Load("Models/bws/bws.3DS");
	model_ws.Load("Models/waterslide/untitled.3DS");
	model_ff.Load("Models/freefall/untitled.3DS");
	model_thief.Load("Models/thief/thief.3DS");
	model_shop.Load("Models/shop/house2.3DS");
	model_tree.Load("Models/tree/Tree1.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_ground2.Load("Textures/ground2.bmp");
	wall.Load("Textures/walltext.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutTimerFunc(10, Update, 0);

	myInit();
	PlaySound("sounds/background.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}