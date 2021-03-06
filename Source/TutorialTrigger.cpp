#include <File.h>
#include <GameObjectFactory.h>
#include <Localise.h>
#include <ROConfig.h>
#include "TutorialTrigger.h"
#include "LevelManager.h"
#include "LurkMsg.h"
#include "GameMode.h"
#include "PlayWav.h"
#include "PropertiesDialog.h"
#include "PropertyKeys.h"

namespace Amju
{
const char* TutorialTrigger::NAME = "tut-trigger";

GameObject* CreateTT() { return new TutorialTrigger; }

#ifdef GLOBAL_INIT
static bool reg = TheGameObjectFactory::Instance()->Add(TutorialTrigger::NAME, &CreateTT);
#endif

TutorialTrigger::TutorialTrigger()
{
  m_hasShownText = false;
}

WWGameObject* TutorialTrigger::Clone()
{
  return new TutorialTrigger(*this);
}

const char* TutorialTrigger::GetTypeName() const
{
  return NAME;
}

void TutorialTrigger::AddPropertiesGui(PropertiesDialog* dlg)
{
  dlg->AddItem(new PropertiesDialog::TextItem("Text", m_text, PROP_TEXT));
}

PropertyValue TutorialTrigger::GetProp(PropertyKey key)
{
  switch (key)
  {
  case PROP_TEXT:
    return m_text;
  }
  return Trigger::GetProp(key);
}

void TutorialTrigger::SetProp(PropertyKey key, PropertyValue value)
{
  Trigger::SetProp(key, value);
  switch (key)
  {
  case PROP_TEXT:
    m_text = value.GetString();
  }
}

void TutorialTrigger::OnPlayerCollision(Player*)
{
  // Only show once in process
  typedef std::set<int> IdSet;
  static IdSet idSet;
  // Make unique ID for this object in the current level
  int unique = GetId() | TheLevelManager::Instance()->GetLevelId() << 20;
  if (idSet.insert(unique).second == false)
  {
    // Not inserted - already in set
#ifdef TT_DEBUG
std::cout << "Already triggered tutorial trigger " << GetId() << "\n";
#endif
    return;
  }

  if (!m_hasShownText)
  {
    if (!IsEditMode()) // Edit mode "fix" - TODO
    {
      static std::string sound = ROConfig()->GetValue("sound-tut-trigger");
      PlayWav(sound);

      Strings strs = Split(m_text, '&'); 
      for (auto it = strs.begin(); it != strs.end(); ++it)
      {
        const std::string& s = *it;
        LurkMsg lm(Lookup(s), Colour(1, 1, 1, 1), Colour(0, 0, 0, 1), 
          AMJU_CENTRE);
        TheLurker::Instance()->Queue(lm);
      }
    }
    m_hasShownText = true;
  }
}

bool TutorialTrigger::Save(File* f)
{
  if (!Trigger::Save(f))
  {
    return false;
  }
  return f->WriteComment("// Tutorial text") &&
    f->Write(m_text.empty() ? "$$$empty" : m_text);
}

bool TutorialTrigger::Load(File* f)
{
  if (!Trigger::Load(f))
  {
    return false;
  }
  // Get tutorial text - NB should be localised
  if (!f->GetDataLine(&m_text))
  {
    f->ReportError("Expected tutorial text");
    return false;
  }
  return true;
}
}

