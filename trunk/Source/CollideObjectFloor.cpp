#include "CollisionManager.h"
#include "Player.h"
#include "Dino.h"
#include "Floor.h"
#include "Rect.h"
#include "Pet.h"
#include "StaticFloor.h"

namespace Amju
{
void CollideObjectFloor(GameObject* go1, GameObject* go2)
{
  Assert(dynamic_cast<OnFloorCharacter*>(go1));
  Assert(dynamic_cast<Floor*>(go2));

  // Only need to do anything if the player centre pos is *outside* of the floor AABB.
  // If player is moving away from floor, don't do anything. Otherwise, 
  // player is moving towards the floor. Bounce off if player is too low. This
  // prevents player from falling into and through a floor.
  // TODO
  const AABB& floorAABB = *(go2->GetAABB());
  Rect r(floorAABB.GetMin(0), floorAABB.GetMax(0), floorAABB.GetMin(2), floorAABB.GetMax(2));

  const Vec3f& playerPos = go1->GetPos();

  if (r.IsPointIn(Vec2f(playerPos.x, playerPos.z)))
  {
    // Player is over floor, no need to push away
    return;
  }
  // Is player moving away from floor ? No need to do anything if player is
  // jumping/falling off floor.
  const Vec3f& floorPos = go2->GetPos();
  const Vec3f& playerVel = go1->GetVel();

  bool reverseX = false;
  bool reverseZ = false;

  if ((floorPos.x > playerPos.x && playerVel.x > 0) ||
      (floorPos.x < playerPos.x && playerVel.x < 0))
  {
    reverseX = true;
  }

  if ((floorPos.z > playerPos.z && playerVel.z > 0) ||
      (floorPos.z < playerPos.z && playerVel.z < 0))
  {
    reverseZ = true;
  }
 
  if (!reverseX && !reverseZ)
  {
    // No need to reverse in either direction
    return;
  }

  // Check if player height is below floor height - push player backwards if so.
  // Find intersection rectangle (x, z) of player and floor AABBs.
  // TODO Get floor height at corners, and test all 4 ? or centre of intersect rect ?

  AABB intr = go1->GetAABB()->Intersection(floorAABB);
  // Get centre point of intersection (x, z) rect
  Vec2f centre(
    (intr.GetMin(0) + intr.GetMax(0)) * 0.5f,
    (intr.GetMin(2) + intr.GetMax(2)) * 0.5f);
  // Get floor height at centre point
  float floorY = 0;
  if (((Floor*)go2)->GetY(centre, &floorY))
  {
    // Check if player too low -- allow range for platforms which are next to each other.
    if ((playerPos.y + 10.0f) < floorY) // TODO
    {
      // Player is lower than floor, so we must move away so we don't
      // appear to fall through it.
      // Reverse player (x, z) vel
      // TODO work out correct new velocity - we need to reverse x, or z or both.
      go1->SetVel(Vec3f(reverseX ? -playerVel.x : playerVel.x, playerVel.y, 
        reverseZ ? -playerVel.z : playerVel.z));

      // TODO Sound effect? E.g. 'oof!' -- depends on character type so call v. function
    }
  }
}

static bool b[] = 
{
  TheCollisionManager::Instance()->Add(
    Player::NAME, Floor::NAME, &CollideObjectFloor),

  TheCollisionManager::Instance()->Add(
    Dino::NAME, Floor::NAME, &CollideObjectFloor),

  TheCollisionManager::Instance()->Add(
    Pet::NAME, Floor::NAME, &CollideObjectFloor),

  // O Noes, have to duplicate because there are 2 floor types?! 
  // How to avoid this ?
  TheCollisionManager::Instance()->Add(
    Player::NAME, StaticFloor::NAME, &CollideObjectFloor),

  TheCollisionManager::Instance()->Add(
    Dino::NAME, StaticFloor::NAME, &CollideObjectFloor),

  TheCollisionManager::Instance()->Add(
    Pet::NAME, StaticFloor::NAME, &CollideObjectFloor),
};
}
