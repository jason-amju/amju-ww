#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include "OnFloorCharacter.h"
#include "AI.h"
#include <map>

namespace Amju
{
class Npc : public OnFloorCharacter
{
public:
  Npc();
  void SetAI(const char* aiName);
  void SetAI(AI*);

  void AddAI(AI*); // call to populate map
  void DecideAI();
  virtual void Update();

protected:
  typedef std::map<std::string, PAI> AIs;
  AIs m_ais;
  AI* m_ai;
};
}

#endif

