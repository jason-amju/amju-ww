#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <list>
#include "EventListener.h"
#include "OnFloorCharacter.h"
#include "PlayerNum.h"

namespace Amju
{
class Exit;
class PlayerController;
class Pet;
typedef std::list<RCPtr<Pet> > PetList;

class Player : public OnFloorCharacter
{
public:
  static Player* GetPlayer(PlayerNum pn);

  static const char* NAME;

  Player();
  virtual ~Player();

  void CreateController();

  // ID of player - 0 for single player, 0 or 1 for two-player, etc.
  void SetPlayerId(int);
  int GetPlayerId() const;

  virtual const char* GetTypeName() const override;
  virtual WWGameObject* Clone() override;
  virtual void Update() override;
  virtual bool Load(File* f) override;
  virtual bool Save(File* f) override;
  virtual void Reset() override;

  virtual bool OnButtonEvent(const ButtonEvent& be);
  virtual bool OnJoyAxisEvent(const JoyAxisEvent& je);
  virtual bool OnKeyEvent(const KeyEvent& ke);
  virtual bool OnBalanceBoardEvent(const BalanceBoardEvent& bbe);
  virtual bool OnRotationEvent(const RotationEvent&);
  virtual bool OnMouseButtonEvent(const MouseButtonEvent& mbe);

  virtual void OnAnimFinished() override;

  virtual void StartBeingEaten(OnFloorCharacter* eater) override;
  virtual void StartBeingDead() override;

  void Jump();

  void PickUpPet(Pet* pet);
  const PetList& GetPets() const { return m_pets; }
  void DropPets();

  // Call when we collide with exit
  void ReachedExit(Exit* exit);

  virtual void AddPropertiesGui(PropertiesDialog* dlg) override;
  virtual PropertyValue GetProp(PropertyKey) override;
  virtual void SetProp(PropertyKey, PropertyValue) override;

  virtual void OnHitFloor() override;

  void SetTiltSensitivity(float x, float y);

private:
  void UpdatePets();
  void KillController();
 
private:
  // PlayerInfo ID
  int m_playerId;

  RCPtr<PlayerController> m_controller;

  // Pets picked up
  PetList m_pets;

  bool m_reachedExit;
  Vec3f m_exitPos;

  int m_jumpCount; // for double, (triple?) jump
  int m_maxJumpCount; // how many multi-jumps you can do
  float m_jumpVel; 
  float m_petScoreTimer;
  
  float m_xSensitivity;
  float m_ySensitivity;
};

// Pass events on to the Player - TODO Send messages so will work over network
class PlayerController : public EventListener
{
public:
  PlayerController(Player* player) : m_player(player) {}

  virtual bool OnButtonEvent(const ButtonEvent& be) override { return m_player->OnButtonEvent(be); }
  virtual bool OnJoyAxisEvent(const JoyAxisEvent& je) override { return m_player->OnJoyAxisEvent(je); }
  virtual bool OnKeyEvent(const KeyEvent& ke) override { return m_player->OnKeyEvent(ke); }
  virtual bool OnBalanceBoardEvent(const BalanceBoardEvent& bbe) override { return m_player->OnBalanceBoardEvent(bbe); }
  virtual bool OnRotationEvent(const RotationEvent& re) override { return m_player->OnRotationEvent(re); }
  virtual bool OnMouseButtonEvent(const MouseButtonEvent& mbe) override { return m_player->OnMouseButtonEvent(mbe); }

private:
  Player* m_player;
};

}

#endif
