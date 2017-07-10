#include <Game.h>
#include <DrawAABB.h>
#include <Screen.h>
#include "Viewport.h"
#include "MySceneGraph.h"
#include "AmjuGL.h"
#include "GuiImage.h"
#include "WWCamera.h"
#include "GSLoadLevel.h"
#include "GameMode.h"
#include "Stars.h"

#define DRAW_AABB_DEBUG

namespace Amju
{
Viewport::Viewport(int id, float x, float y, float w, float h) : 
  m_id(id), m_x(x), m_y(y), m_w(w), m_h(h)
{
  m_pCamera = 0;
  m_aspect = (float)w / (float)h;
}

void Viewport::Draw()
{
  float x = (float)Screen::X();
  float y = (float)Screen::Y();
  AmjuGL::Viewport((int)(m_x * x), (int)(m_y * y), (int)(m_w * x), (int)(m_h * y));

  /////////////////////////
  // STARS BACKGROUND
  static SceneNode* stars[2] = 
  {
    CreateStars("star1.png", 0.03f, 50, 99999.9f, -99999.9f),
    CreateStars("flare.png", 0.05f, 50, 99999.9f, -99999.9f)
  };

  static const float aspect = x / y;

  AmjuGL::SetMatrixMode(AmjuGL::AMJU_PROJECTION_MATRIX);
  AmjuGL::SetIdentity();
  AmjuGL::SetPerspectiveProjection(45.0f, aspect, 1.0f, 2.0f);

  AmjuGL::SetMatrixMode(AmjuGL::AMJU_MODELVIEW_MATRIX);
  AmjuGL::SetIdentity();

  AmjuGL::Disable(AmjuGL::AMJU_DEPTH_READ);
  AmjuGL::Disable(AmjuGL::AMJU_DEPTH_WRITE);

  for (int i = 0; i < 2; i++)
  {
    stars[i]->Update();
    stars[i]->Draw();
  }
  /////////////////////////

  AmjuGL::Enable(AmjuGL::AMJU_DEPTH_READ);
  AmjuGL::Enable(AmjuGL::AMJU_DEPTH_WRITE);

  AmjuGL::SetMatrixMode(AmjuGL::AMJU_PROJECTION_MATRIX);
  AmjuGL::SetIdentity();
  const float FOVY = 60.0f;
  const float NEAR = 1.0f; 
  const float FAR = 3000.0f;

  const float ASPECT = 0.666f; // Always show the same amount of the world,
    // but distorted if viewport does not have this aspect ratio

  AmjuGL::SetPerspectiveProjection(FOVY, ASPECT, NEAR, FAR);

  AmjuGL::SetMatrixMode(AmjuGL::AMJU_MODELVIEW_MATRIX);
  AmjuGL::SetIdentity();

  AmjuGL::PushMatrix();
  AmjuGL::Enable(AmjuGL::AMJU_LIGHTING);
  AmjuGL::LightColour ambient(0.6f, 0.6f, 0.6f);
  AmjuGL::DrawLighting(
    AmjuGL::LightColour(0, 0, 0),
    ambient, // Ambient light colour
    AmjuGL::LightColour(1, 1, 1), // Diffuse light colour
    AmjuGL::LightColour(1, 1, 1),
    AmjuGL::Vec3(1, 1, 1)); // Light direction

  if (!IsEditMode())
  {
    GetCamera()->SetAsSceneGraphCamera();
  }
  GetGameSceneGraph()->Draw();
  AmjuGL::PopMatrix();

#ifdef DRAW_AABB_DEBUG
  // Draw AABBs 
  AmjuGL::PushMatrix();
  GetGameSceneGraph()->GetCamera()->Draw();
  AmjuGL::PushAttrib(AmjuGL::AMJU_TEXTURE_2D);
  AmjuGL::Disable(AmjuGL::AMJU_TEXTURE_2D);
  PushColour();
  AmjuGL::SetColour(Colour(0, 0, 0, 1));
  
  GameObjects* gos = TheGame::Instance()->GetGameObjects();
  for (auto it = gos->begin(); it != gos->end(); ++it)
  {
    GameObject* go = it->second;
//    DrawAABB(go->GetAABB());
  }
  PopColour();
  AmjuGL::PopAttrib();
  AmjuGL::PopMatrix();
#endif
}

void Viewport::Draw2d()
{
}

void Viewport::SetCamera(WWCamera* pCam)
{
  m_pCamera = pCam;
  Assert(m_pCamera);
}

WWCamera* Viewport::GetCamera()
{
  return m_pCamera;
}

Viewport* ViewportManager::GetViewport(int vpNum)
{
  if (vpNum >= (int)m_viewports.size())
  {
    return 0;
  }

  return m_viewports[vpNum];
}

void ViewportManager::ClearViewports()
{
  m_viewports.clear();
}

void ViewportManager::AddViewport(Viewport* v)
{
  m_viewports.push_back(v);
}

int ViewportManager::GetNumViewports() const
{
  return m_viewports.size();
}
}
