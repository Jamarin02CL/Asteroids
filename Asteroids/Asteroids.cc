#include <ESAT/window.h>
#include <ESAT/draw.h>
#include <ESAT/sprite.h>
#include <ESAT/input.h>
#include <ESAT/time.h>
#include <stdlib.h>
#include <time.h>
#include <iosfwd>
#include <string>

#define M_PI 3.14159265358979323846	/* pi */

typedef struct {
	float x, y;
} tVector;

typedef struct {
	float x, y;
} tPoint;

typedef struct {
	tVector vector[2];
	tPoint center;
	tVector velocity;
	float angle;
	int duration;
} misil;

typedef struct {
	tVector vector[5];
	tPoint center;
	tVector velocity;
	tVector direction;
	float angle;
	tVector acceleration;
	float maxAcc;
	float minAcc;
	float maxVel;
	float minVel;
	misil weapon[10];
	int shotInUse;
} spaceship;

float toRad(float degree) {

	float rad = 0.0f;

	rad = degree * (M_PI / 180);

	return rad;
}

void spaceshipInit(spaceship *sc) {
	sc->center = { 400, 300 };
	sc->vector[0] = { 0, -40 };
	sc->vector[1] = { -20, 10 };
	sc->vector[2] = { -16, 0 };
	sc->vector[3] = { 16, 0 };
	sc->vector[4] = { 20, 10 };
	sc->direction = { 0, -1 };
	sc->velocity = { 0.0f, 0.0f };
	sc->angle = toRad(90.0f);
	sc->acceleration = { 0.2f, 0.2f };
	sc->maxAcc = 3.0f;
	sc->minAcc = -3.0f;
	sc->maxVel = 6.0f;
	sc->minVel = 0.0f;
	sc->shotInUse = 0;
	//sc->vector[5] = { 0, 6 };
}

void shotInit(spaceship *sc) {
	misil shot;
	shot.velocity.x = sc->velocity.x;
	shot.velocity.y = sc->velocity.y;
	shot.angle = sc->angle;
	shot.center = sc->center;
	shot.vector[0] = { 0, -5 };
	shot.vector[1] = { 0, 0 };
	sc->weapon[sc->shotInUse] = shot;
	sc->shotInUse += 1;
}

void drawVector(tVector v, tVector v2, tPoint p) {
	ESAT::DrawSetStrokeColor(255, 255, 255);
	ESAT::DrawLine(p.x + v.x, p.y + v.y, p.x + v2.x, p.y + v2.y);
}

void drawPlayer(spaceship sc) {
	for (int i = 0; i < 5; i++) {
		if (i + 1 < 5)
			drawVector(sc.vector[i], sc.vector[i + 1], sc.center);
		else
			drawVector(sc.vector[i], sc.vector[0], sc.center);
	}
}

void shot(spaceship *sc) {
	shotInit(&*sc);
	misil shot = sc->weapon[sc->shotInUse - 1];
	drawVector(shot.vector[0], shot.vector[1], shot.center);
}

void rotateSpaceship(tVector *v, float rad) {
	float x0, y0;
	x0 = v->x;
	y0 = v->y;

	v->x = x0 * cos(rad) - y0 * sin(rad);
	v->y = x0 * sin(rad) + y0 * cos(rad);
}

void updateDirectionVector(spaceship *sc) {
	sc->direction.x = 1 * -cos(sc->angle);
	sc->direction.y = 1 * -sin(sc->angle);
}

void updateAngle(spaceship *sc) {
	if (ESAT::IsSpecialKeyPressed(ESAT::kSpecialKey_Left)) {
		for (int i = 0; i < 5; i++)
			rotateSpaceship(&sc->vector[i], -0.05f);
		sc->angle -= 0.05f;
	}
	else if (ESAT::IsSpecialKeyPressed(ESAT::kSpecialKey_Right)) {
		for (int i = 0; i < 5; i++)
			rotateSpaceship(&sc->vector[i], 0.05f);
		sc->angle += 0.05f;
	}
}

void PlayerMotion(spaceship *sc)
{
	updateAngle(&*sc);
	updateDirectionVector(&*sc);
	if (ESAT::IsSpecialKeyPressed(ESAT::kSpecialKey_Up)) {
		sc->velocity.x = sc->velocity.x + sc->acceleration.x * sc->direction.x;
		sc->velocity.y = sc->velocity.y + sc->acceleration.y * sc->direction.y;
	}

	if (ESAT::IsSpecialKeyPressed(ESAT::kSpecialKey_Space))
		shot(&*sc);

	sc->center.x += sc->velocity.x;
	sc->center.y += sc->velocity.y;
}

void controlScreen(spaceship *sc) {
	if (sc->center.x > 800)
		sc->center.x = 0;
	if (sc->center.x < 0)
		sc->center.x = 800;
	if (sc->center.y > 600)
		sc->center.y = 0;
	if (sc->center.y < 0)
		sc->center.y = 600;
}

void showInfo(spaceship sc) {
	char x[10], y[10];
	ESAT::DrawSetTextFont("OpenSans-Regular.ttf");
	char frase1[50] = "Vel X: ";
	_itoa_s(sc.velocity.x, x, 10, 10);
	_itoa_s(sc.velocity.y, y, 10, 10);
	strcat_s(frase1, x);
	strcat_s(frase1, " | Vel Y: ");
	strcat_s(frase1, y);
	ESAT::DrawSetTextSize(20);
	ESAT::DrawText(500, 30, frase1);
}

int ESAT::main(int argc, char **argv){

	ESAT::WindowInit(800, 600);
	spaceship sc;
	spaceshipInit(&sc);

	while (ESAT::WindowIsOpened() && !ESAT::IsSpecialKeyDown(ESAT::kSpecialKey_Escape)){
		ESAT::DrawClear(0, 0, 0);
		drawPlayer(sc);
		PlayerMotion(&sc);
		controlScreen(&sc);
		showInfo(sc);
		ESAT::WindowFrame();
	}

	ESAT::WindowDestroy();
	return 0;
}