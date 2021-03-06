#include <math.h>
#include "GSLogo.h"
#include "Game.h"
#include "Timer.h"
#include "Colour.h"
#include "GSLoadLayers.h"
#include "SoundManager.h"
#include "ResourceManager.h"

namespace Amju
{
const char* GSLogo::NAME = "start";

void Done()
{
  TheGame::Instance()->SetCurrentState(TheGSLoadLayers::Instance());
}

GSLogo::GSLogo()
{
  m_fade = 0;
}

void GSLogo::OnActive()
{
  GameState::OnActive();
  m_image.OpenAndLoad("logo-gui.txt");
}

void GSLogo::OnDeactive()
{
  GameState::OnDeactive();
}

void GSLogo::Update()
{
#ifdef _DEBUG
  const float LOGO_SPEED = 4.0f;
#else
  const float LOGO_SPEED = 1.0f;
#endif
  m_fade += TheTimer::Instance()->GetDt() * LOGO_SPEED;
}

void GSLogo::Draw()
{
}

void GSLogo::Draw2d()
{
  PushColour();
  float s = sin(m_fade);
  if (s < 0)
  {
    s = 0;
    Done();
  }
  MultColour(Colour(s, s, s, 1.0f));
  m_image.Draw();
  PopColour();
}
}
