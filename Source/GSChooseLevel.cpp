#include <Game.h>
#include <SceneMesh.h>
#include <Timer.h>
#include <Screen.h>
#include <GuiButton.h>
#include <GuiTextEdit.h>
#include "GSChooseLevel.h"
#include "iOSKeyboard.h"
#include "LevelManager.h"
#include "MySceneGraph.h"
#include "StartGame.h"
#include "WWLoadGui.h"

namespace Amju
{
static void OnOK(GuiElement*)
{
  TheGSChooseLevel::Instance()->OnOK(); 
}

static void OnCancel(GuiElement*)
{
  TheGSChooseLevel::Instance()->GoBack();
}

class Rotater : public SceneNode
{
public:
  void BeforeDraw()
  {
    AmjuGL::PushMatrix();
    static float a = 0;
    float dt = TheTimer::Instance()->GetDt();
    a += dt;
    AmjuGL::RotateY(a * 90.0f);
    AmjuGL::RotateX(a * 60.0f);

    PushColour();
    MultColour(m_colour);
  }

  void AfterDraw()
  {
    PopColour();
    AmjuGL::PopMatrix();
  }

private:
  Colour m_colour;
};

void GSChooseLevel::OnActive() 
{
  GSText::OnActive();

  ObjMesh* mesh = (ObjMesh*)TheResourceManager::Instance()->GetRes("ball.obj");
  Assert(mesh);

  SceneNode* rot = new Rotater;
  SceneMesh* node = new SceneMesh;
  node->SetMesh(mesh);
  rot->AddChild(node);

  static const float X2 = 40.0f;
  static const float Y2 = 40.0f;
  AABB aabb(-X2, X2, -Y2, Y2, -X2, X2);
//  aabb.Translate(m_pos);

  node->SetAABB(aabb);

  SceneNode* parent = new SceneNode;
  parent->AddChild(rot);
  parent->CalcBoundingVol(); //SetAABB(aabb);
  GetTextSceneGraph()->SetRootNode(SceneGraph::AMJU_OPAQUE, parent);

  m_gui = WWLoadGui("gui-chooselevel.txt");
  Assert(m_gui);

  // Set focus element, cancel element, command handlers
  GuiButton* ok = (GuiButton*)m_gui->GetElementByName("ok-button");
  ok->SetCommand(Amju::OnOK);
  ok->SetIsFocusButton(true);
  ok->SetShowIfFocus(true);

  ((GuiButton*)m_gui->GetElementByName("cancel-button"))->SetCommand(OnCancel);
 
#ifdef AMJU_IOS
  GuiTextEditIos* textEdit = (GuiTextEditIos*)GetElementByName(m_gui, "edit-level-number");
  textEdit->ShowKeyboard(true);
#endif
}

void GSChooseLevel::OnDeactive()
{
#ifdef AMJU_IOS
  GuiTextEditIos* textEdit = (GuiTextEditIos*)GetElementByName(m_gui, "edit-level-number");
  textEdit->ShowKeyboard(false);
#endif

  GSText::OnDeactive();
}
  
void GSChooseLevel::OnOK()
{
  int level = 0;
  
#ifdef AMJU_IOS
  
  GuiTextEditIos* textEdit = (GuiTextEditIos*)GetElementByName(m_gui, "edit-level-number");
  level = ToInt(textEdit->GetText());
  
#else

  GuiTextEdit* edit = (GuiTextEdit*)m_gui->GetElementByName("edit-level-number");
  Assert(edit);
  level = ToInt(edit->GetText());
  
#endif

  if (level <= 0)
  {
    GuiText* text = (GuiText*)m_gui->GetElementByName("help-text");
    Assert(text);
    text->SetText("Bad number? Try again!");
    return;
  }

  TheLevelManager::Instance()->SetLevelId(level);
  StartGame(1, AMJU_MAIN_GAME_MODE);
}

bool GSChooseLevel::OnCursorEvent(const CursorEvent& e) 
{
  return false;
}

bool GSChooseLevel::OnMouseButtonEvent(const MouseButtonEvent& e)
{
  return false;
}

bool GSChooseLevel::OnDoubleClickEvent(const DoubleClickEvent& e)
{
  return false;
}

void GSChooseLevel::Draw()
{
  AmjuGL::SetMatrixMode(AmjuGL::AMJU_PROJECTION_MATRIX);
  AmjuGL::SetIdentity();
  const float FOVY = 60.0f;
  const float NEAR = 1.0f;
  const float FAR = 1000.0f;
  static const float aspect = (float)Screen::X() / (float)Screen::Y();
  AmjuGL::SetPerspectiveProjection(FOVY, aspect, NEAR, FAR);

  AmjuGL::SetMatrixMode(AmjuGL::AMJU_MODELVIEW_MATRIX);
  AmjuGL::SetIdentity();
  // TODO CONFIG
  AmjuGL::LookAt(0, 0, 500.0f,   0, 0, 0,  0, 1.0f, 0);

  // TODO Lighting node
  AmjuGL::Enable(AmjuGL::AMJU_LIGHTING);
  AmjuGL::DrawLighting(
    AmjuGL::LightColour(0, 0, 0),
    AmjuGL::LightColour(0.2f, 0.2f, 0.2f), // Ambient light colour
    AmjuGL::LightColour(1, 1, 1), // Diffuse light colour
    AmjuGL::LightColour(1, 1, 1),
    AmjuGL::Vec3(1, 1, 1)); // Light direction

//  GetTextSceneGraph()->Draw();
}

}


