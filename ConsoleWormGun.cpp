using namespace std;

#include <algorithm>
#include <string>
#include "ConsoleEngine.h"

class cPhysicsObject {
public:
	float px = 0.0f;
	float py = 0.0f;
	float vx = 0.0f;
	float vy = 0.0f;
	float ax = 0.0f;
	float ay = 0.0f;

	float radius = 4.0f;
	bool bStable = false;
	float fFriction = 0.8f;

	int nBounceBeforeDeath = -1;
	bool bDead = false;

	cPhysicsObject(float x = 0.0f, float y = 0.0f) {
		px = x;
		py = y;
	}

	virtual void Draw(ConsoleTemplateEngine* engine, float fOffsetX, float fOffsetY) = 0; // Pointer to engine allows instance of game engine into object code // offset is camera position
	virtual int BounceDeathAction() = 0;
};

class cDebris : public cPhysicsObject {
public:
	cDebris(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {
		vx = 10.0f * cosf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
		vy = 10.0f * sinf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
		radius = 1.0f;
		fFriction = 0.8f;
		nBounceBeforeDeath = 5;
	}

	virtual void Draw(ConsoleTemplateEngine* engine, float fOffsetX, float fOffsetY) {
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_DARK_GREEN);
	}

	virtual int BounceDeathAction() {
		return 0; // Nothing, just fade
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDebris() {
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ 1.0f, 0.0f });
	vecModel.push_back({ 1.0f, 1.0f });
	vecModel.push_back({ 0.0f, 1.0f });
	return vecModel;
}

vector<pair<float, float>> cDebris::vecModel = DefineDebris();

// Help debug physics object // inherites from cPhysicsObject
class cDummy : public cPhysicsObject {
public:
	// Constructor passing through x, y in world space
	cDummy(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {

	}

	virtual void Draw(ConsoleTemplateEngine* engine, float fOffsetX, float fOffsetY) {
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_WHITE);
	}

	virtual int BounceDeathAction() {
		return 0; // Nothing, just fade
	}
private:
	// Making it static to circumvent constantly recreating and allocating resources for the same object
	// Using one model that will be shared across all in the same class
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDummy() {
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	for (int i = 0; i < 10; i++) 
		vecModel.push_back({ cosf(i / 9.0f * 2.0f * 3.14159f), sinf(i / 9.0f * 2.0f * 3.14159f) });

	return vecModel;
}

// Using factory function
vector<pair<float, float>> cDummy::vecModel = DefineDummy();

class cMissile : public cPhysicsObject {
public:
	cMissile(float x = 0.0f, float y = 0.0f, float _vx = 0.0f, float _vy = 0.0f) : cPhysicsObject(x, y) {
		radius = 2.5f;
		fFriction = 0.5f;
		vx = _vx;
		vy = _vy;
		bDead = false;
		nBounceBeforeDeath = 1;
	}

	virtual void Draw(ConsoleTemplateEngine* engine, float fOffsetX, float fOffsetY) {
		engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_YELLOW);
	}

	virtual int BounceDeathAction() {
		return 20; // Explode Big
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineMissile() {
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ 1.0f, 1.0f });
	vecModel.push_back({ 2.0f, 1.0f });
	vecModel.push_back({ 2.5f, 0.0f });
	vecModel.push_back({ 2.0f, -1.0f });
	vecModel.push_back({ 1.0f, -1.0f });
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ -1.0f, -1.0f });
	vecModel.push_back({ -2.5f, -1.0f });
	vecModel.push_back({ -2.0f, 0.0f });
	vecModel.push_back({ -2.5f, 1.0f });
	vecModel.push_back({ -1.0f, 1.0f });
	for (auto& v : vecModel) {
		v.first /= 2.5f;
		v.second /= 2.5f;
	}
	return vecModel;
}

vector<pair<float, float>> cMissile::vecModel = DefineMissile();

class cWorm : public cPhysicsObject { // A unit/worm
public:
	cWorm(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {
		radius = 3.5f;
		fFriction = 0.2f;
		bDead = false;
		nBounceBeforeDeath = -1;

		if (sprWorm == nullptr)
			sprWorm = new TemplateSprite(L"Assets/worms.spr");
	}

	virtual void Draw(ConsoleTemplateEngine* engine, float fOffsetX, float fOffsetY) {
		engine->DrawPartialSprite(px - fOffsetX - radius, py - fOffsetY - radius, sprWorm, 0, 0, 8, 8);
	}

	virtual int BounceDeathAction() {
		return 0; // Nothing
	}

public:
	float fShootAngle = 0.0f;

private:
	static TemplateSprite* sprWorm;
};

TemplateSprite* cWorm::sprWorm = nullptr;

class WormGun : public ConsoleTemplateEngine {
public:
	WormGun() {
		m_sAppName = L"Wormlike Game";
	}

private:
	int nMapWidth = 1024;
	int nMapHeight = 512;
	unsigned char* map = nullptr;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;
	float fCameraPosXTarget = 0.0f;
	float fCameraPosYTarget = 0.0f;

	// Game States
	enum GAME_STATE {
		GS_RESET = 0,
		GS_GENERATE_TERRAIN = 1,
		GS_GENERATING_TERRAIN,
		GS_ALLOCATE_UNITS,
		GS_ALLOCATING_UNITS,
		GS_START_PLAY,
		GS_CAMERA_MODE
	} nGameState, nNextState;

	bool bGameIsStable = false;
	bool bPlayerHasControl = false;
	bool bPlayerActionComplete = false;

	// List of things that exist in game world
	list<unique_ptr<cPhysicsObject>> listObjects; // Since cPhysicsObject is abstract, need to treat it as a pointer

	cPhysicsObject* pObjectUnderControl = nullptr; // Player can control 1 of multiple object on screen
	cPhysicsObject* pCameraTrackingObject = nullptr; // Camera follows object

	bool bEnergising = false;
	float fEnergyLevel = 0.0f;
	bool bFireWeapon = false;

	virtual bool OnUserCreate() {
		// Create Map
		map = new unsigned char[nMapWidth * nMapHeight];
		memset(map, 0, nMapWidth * nMapHeight * sizeof(unsigned char));
		//CreateMap();

		nGameState = GS_RESET;
		nNextState = GS_RESET;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		// USER INPUT
		if (m_keys[L'M'].bReleased)
			CreateMap();
		if (m_mouse[0].bReleased)
			Boom(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY, 10.0f);
		if (m_mouse[1].bReleased)
			listObjects.push_back(unique_ptr<cMissile>(new cMissile(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY)));
		if (m_mouse[2].bReleased) {
			cWorm* worm = new cWorm(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
			pObjectUnderControl = worm;
			pCameraTrackingObject = worm;
			listObjects.push_back(unique_ptr<cWorm>(worm));
		}
			//listObjects.push_back(unique_ptr<cWorm>(new cWorm(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY)));
			//cDummy* p = new cDummy(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
			//listObjects.push_back(unique_ptr<cDummy>(p));
		
		// Camera Contorl
		// Mouse Edge Map Scroll // Issues with corner movement causing wireframe functiont to crash
		float fMapScrollSpeed = 300.0f;
		if (m_mousePosX < 5)
			fCameraPosX -= fMapScrollSpeed * fElapsedTime;
		if (m_mousePosX > ScreenWidth() - 5)
			fCameraPosX += fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY < 5)
			fCameraPosY -= fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY > ScreenHeight() - 5)
			fCameraPosY += fMapScrollSpeed * fElapsedTime;
		// Control Supervisor
		switch (nGameState) {
			case GS_RESET: {
				bPlayerHasControl = false;
				nNextState = GS_GENERATE_TERRAIN;
			}
			break;

			case GS_GENERATE_TERRAIN: {
				bPlayerHasControl = false;
				CreateMap();
				nNextState = GS_GENERATING_TERRAIN;
			}
			break;

			case GS_GENERATING_TERRAIN: {
				bPlayerHasControl = false;
				nNextState = GS_ALLOCATE_UNITS;
			}
			break;

			case GS_ALLOCATE_UNITS: {
				bPlayerHasControl = false;
				cWorm* worm = new cWorm(32.0f, 1.0f);
				listObjects.push_back(unique_ptr<cWorm>(worm));
				pObjectUnderControl = worm;
				pCameraTrackingObject = pObjectUnderControl;
				nNextState = GS_ALLOCATING_UNITS;
			}
			break;

			case GS_ALLOCATING_UNITS: {
				bPlayerHasControl = false;
				if (bGameIsStable) {
					bPlayerActionComplete = false;
					nNextState = GS_START_PLAY;
				}
			}
			break;

			case GS_START_PLAY: {
				bPlayerHasControl = true;
				if (bPlayerActionComplete) {
					nNextState = GS_CAMERA_MODE;
				}
			}
			break;

			case GS_CAMERA_MODE: {
				bPlayerHasControl = false;
				bPlayerActionComplete = false;

				if (bGameIsStable) {
					pCameraTrackingObject = pObjectUnderControl;
					nNextState = GS_START_PLAY;
				}
			}
			break;
		}

		// Handle User Input
		if (bPlayerHasControl) {
			if (pObjectUnderControl != nullptr) {
				if (pObjectUnderControl->bStable) {
					if (m_keys[L'Z'].bPressed) {
						float a = ((cWorm*)pObjectUnderControl)->fShootAngle;
						pObjectUnderControl->vx = 4.0f * cosf(a);
						pObjectUnderControl->vy = 8.0f * sinf(a);
						pObjectUnderControl->bStable = false;
					}
					if (m_keys[L'A'].bHeld) {
						cWorm* worm = (cWorm*)pObjectUnderControl;
						worm->fShootAngle -= 1.0f * fElapsedTime;
						if (worm->fShootAngle < -3.14159f)
							worm->fShootAngle += 3.14159f * 2.0f;
					}
					if (m_keys[L'S'].bHeld) {
						cWorm* worm = (cWorm*)pObjectUnderControl;
						worm->fShootAngle += 1.0f * fElapsedTime;
						if (worm->fShootAngle > 3.14159f)
							worm->fShootAngle -= 3.14159f * 2.0f;
					}
					if (m_keys[VK_SPACE].bPressed) {
						bEnergising = true;
						bFireWeapon = false;
						fEnergyLevel = 0.0f;
					}
					if (m_keys[VK_SPACE].bHeld) {
						if (bEnergising) {
							fEnergyLevel += 0.75f * fElapsedTime;
							if (fEnergyLevel >= 1.0f) {
								fEnergyLevel = 1.0f;
								bFireWeapon = true;
							}
						}
					}
					if (m_keys[VK_SPACE].bReleased) {
						if (bEnergising)
							bFireWeapon = true;
						bEnergising = false;
					}
				}

				if (bFireWeapon) {
					cWorm* worm = (cWorm*)pObjectUnderControl;

					// Get Weapon Origin
					float ox = worm->px;
					float oy = worm->py;

					// Get Weapon Direction
					float dx = cosf(worm->fShootAngle);
					float dy = sinf(worm->fShootAngle);

					// Weapon Object
					cMissile* m = new cMissile(ox, oy, dx * 40.0f * fEnergyLevel, dy * 40.0f * fEnergyLevel);
					listObjects.push_back(unique_ptr<cMissile>(m));
					pCameraTrackingObject = m;

					bFireWeapon = false;
					fEnergyLevel = 0.0f;
					bEnergising = false;

					bPlayerActionComplete = true;
				}
			}
		}

		// Set Camera Coordinates
		if (pCameraTrackingObject != nullptr) {
			//fCameraPosX = pCameraTrackingObject->px - ScreenWidth() / 2;
			//fCameraPosY = pCameraTrackingObject->py - ScreenHeight() / 2;
			fCameraPosXTarget = pCameraTrackingObject->px - ScreenWidth() / 2;
			fCameraPosYTarget = pCameraTrackingObject->py - ScreenHeight() / 2;
			// Slight lag time of camera
			fCameraPosX += (fCameraPosXTarget - fCameraPosX) * 5.0f * fElapsedTime;
			fCameraPosY += (fCameraPosYTarget - fCameraPosY) * 5.0f * fElapsedTime;
		}

		// Clamp map boundaries
		if (fCameraPosX < 1)
			fCameraPosX = 1;
		if (fCameraPosX >= nMapWidth - ScreenWidth())
			fCameraPosX = nMapWidth - ScreenWidth() - 1;
		if (fCameraPosY < 1)
			fCameraPosY = 1;
		if (fCameraPosY >= nMapHeight - ScreenHeight())
			fCameraPosY = nMapHeight - ScreenHeight() - 1;

		// 10 physics iteration per frame since drawing is the slowest
		for (int z = 0; z < 10; z++) {
			// Update physics of all physical objects
			for (auto& p : listObjects) {
				// Apply Gravity
				p->ay += 2.0f;

				// Update Velocity
				p->vx += p->ax * fElapsedTime;
				p->vy += p->ay * fElapsedTime;

				// Update Position
				float fPotentialX = p->px + p->vx * fElapsedTime;
				float fPotentialY = p->py + p->vy * fElapsedTime;

				// Reset Acceleration
				p->ax = 0.0f;
				p->ay = 0.0f;
				p->bStable = false;

				// Collision Check With Map
				float fAngle = atan2f(p->vy, p->vx);
				float fResponseX = 0.0f;
				float fResponseY = 0.0f;
				bool bCollision = false;

				for (float r = fAngle - 3.14159f / 2.0f; r < fAngle + 3.14159f / 2.0f; r += 3.14159f / 8.0f) { // can do something better to make points a unit distance between each other  //5.48795f
					float fTestPosX = (p->radius) * cosf(r) + fPotentialX;
					float fTestPosY = (p->radius) * sinf(r) + fPotentialY;

					// Clamp to size of map
					if (fTestPosX >= nMapWidth)
						fTestPosX = nMapWidth - 1;
					if (fTestPosY >= nMapHeight)
						fTestPosY = nMapHeight - 1;
					if (fTestPosX < 0)
						fTestPosX = 0;
					if (fTestPosY < 0)
						fTestPosY = 0;

					// Test if any points on semicircle intersect with terrain
					if (map[(int)fTestPosY * nMapWidth + (int)fTestPosX] != 0) {
						// Accumulate collision points to give an escape response vector
						// Effectively, normal to areas of contact
						fResponseX += fPotentialX - fTestPosX;
						fResponseY += fPotentialY - fTestPosY;
						bCollision = true;
					}
				}

				// Calculate magnitudes of response and velocity vectors
				float fMagVelocity = sqrtf(p->vx * p->vx + p->vy * p->vy);
				float fMagResponse = sqrtf(fResponseX * fResponseX + fResponseY * fResponseY);

				// Find angle of collision
				if (bCollision) {
					// Force object to stable, this stops the object penetrating the terrain
					p->bStable = true;

					// Calculate reflection vector of objects velocity vector, using response vector
					float dot = p->vx * (fResponseX / fMagResponse) + p->vy * (fResponseY / fMagResponse); // dot product
					// Use friction coefficient to dampen response (approximating energy loss)
					p->vx = p->fFriction * (-2.0f * dot * (fResponseX / fMagResponse) + p->vx);
					p->vy = p->fFriction * (-2.0f * dot * (fResponseY / fMagResponse) + p->vy);

					// Some objects will "die" after several bounces
					if (p->nBounceBeforeDeath > 0) {
						p->nBounceBeforeDeath--;
						p->bDead = p->nBounceBeforeDeath == 0;

						if (p->bDead) {
							// Action upon object death
							// == 0 Nothing
							// > 0 Explosion
							int nResponse = p->BounceDeathAction();
							if (nResponse > 0) {
								Boom(p->px, p->py, nResponse);
								pCameraTrackingObject = nullptr;
							}
						}
					}
				}

				else {
					p->px = fPotentialX;
					p->py = fPotentialY;
				}

				// Turn off movement when tiny
				if (fMagVelocity < 0.1f)
					p->bStable = true;
			}

			// Remove dead objects from the list, so they are not processed further. As the object
			// is a unique pointer, it will go out of scope too, deleting the object automatically
			listObjects.remove_if([](unique_ptr<cPhysicsObject> &o) { return o->bDead; });
		}

		// Draw Landscape
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++) {
				switch (map[(y + (int)fCameraPosY) * nMapWidth + (x + (int)fCameraPosX)]) {
					case 0:
						Draw(x, y, PIXEL_SOLID, FG_CYAN);
						break;
					case 1:
						Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
						break;
				}
			}

		for (auto& p : listObjects) {
			if (p->py < fCameraPosY + (float)ScreenHeight()) {// Only draw to visibly space of ScreenBuffer
				p->Draw(this, fCameraPosX, fCameraPosY);
				cWorm* worm = (cWorm*)pObjectUnderControl;

				if (p.get() == worm) {
					float cx = worm->px + 8.0f * cosf(worm->fShootAngle) - fCameraPosX;
					float cy = worm->py + 8.0f * sinf(worm->fShootAngle) - fCameraPosY;

					Draw(cx, cy, PIXEL_SOLID, FG_BLACK);
					Draw(cx + 1, cy + 1, PIXEL_SOLID, FG_BLACK);
					Draw(cx - 1, cy + 1, PIXEL_SOLID, FG_BLACK);
					Draw(cx + 1, cy - 1, PIXEL_SOLID, FG_BLACK);
					Draw(cx - 1, cy - 1, PIXEL_SOLID, FG_BLACK);

					for (int i = 0; i < 11 * fEnergyLevel; i++) {
						Draw(worm->px - 5 + i - fCameraPosX, worm->py - 12 - fCameraPosY, PIXEL_SOLID, FG_GREEN);
						Draw(worm->px - 5 + i - fCameraPosX, worm->py - 11 - fCameraPosY, PIXEL_SOLID, FG_RED);
					}
				}
			}
		}

		// Check for game state stability
		bGameIsStable = true;
		for (auto &p : listObjects)
			if (!p->bStable) {
				bGameIsStable = false;
				break;
			}

		if (bGameIsStable)
			Fill(2, 2, 6, 6, PIXEL_SOLID, FG_RED);

		// Move to next state
		nGameState = nNextState;

		return true;
	}

	void Boom(float fWorldX, float fWorldY, float fRadius) {
		// Destroy terrain
		auto CircleBresenham = [&](int xc, int yc, int r) { // World space (bitmap bg)
			int x = 0;
			int y = r;
			int p = 3 - 2 * r;
			if (!r)
				return;

			auto drawline = [&](int sx, int ex, int ny) {
				for (int i = sx; i < ex; i++)
					if (ny >= 0 && ny < nMapHeight && i >= 0 && i < nMapWidth)
						map[ny * nMapWidth + i] = 0;
			};

			while (y >= x) { // only formulate 1/8 of circle
				//Filled circle
				drawline(xc - x, xc + x, yc - y);
				drawline(xc - y, xc + y, yc - x);
				drawline(xc - x, xc + x, yc + y);
				drawline(xc - y, xc + y, yc + x);
				if (p < 0)
					p += 4 * x++ + 6;
				else
					p += 4 * (x++ - y--) + 10;
			}
		};

		// Erase Terrain to form crater
		CircleBresenham(fWorldX, fWorldY, fRadius);

		// Shockwave other entities in range
		for (auto& p : listObjects) {
			float dx = p->px - fWorldX;
			float dy = p->py - fWorldY;
			float fDist = sqrt(dx * dx + dy * dy);

			// Security check
			if (fDist < 0.0001f)
				fDist = 0.0001f;

			if (fDist < fRadius) {
				p->vx = (dx / fDist) * fRadius;
				p->vy = (dy / fDist) * fRadius;
				p->bStable = false;
			}
		}

		// Launch debris
		for (int i = 0; i < (int)fRadius; i++)
			listObjects.push_back(unique_ptr<cDebris>(new cDebris(fWorldX, fWorldY)));
	}

	void CreateMap() {
		// Used to generate Perlin Noise Terrain
		float* fSurface = new float[nMapWidth];
		float* fNoiseSeed = new float[nMapWidth];

		for (int i = 0; i < nMapWidth; i++)
			fNoiseSeed[i] = (float)rand() / (float)RAND_MAX;

		fNoiseSeed[0] = 0.5f; // first and last element starts half way up to provide more place for players to fight
		PerlinNoise1D(nMapWidth, fNoiseSeed, 8, 2.0f, fSurface);

		// Any value that is within the perlin height is set to 1 to represent terrain, all else is 0 for empty space
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++) {
				if (y >= fSurface[x] * nMapHeight)
					map[y * nMapWidth + x] = 1;
				else
					map[y * nMapWidth + x] = 0;
			}

		// Clean up allocated space
		delete[] fSurface;
		delete[] fNoiseSeed;
	}

	// 1D Perlin Noise
	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput) {
		for (int x = 0; x < nCount; x++) {
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++) {
				int nPitch = nCount >> o;
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;
				float fBlend = (float)(x - nSample1) / (float)nPitch;
				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
				
				fScaleAcc += fScale;
				fNoise += fSample * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[x] = fNoise / fScaleAcc;
		}
	}
};

int main() {
	WormGun game;
	game.ConstructConsole(256, 160, 6, 6);
	game.Start();

	return 0;
}