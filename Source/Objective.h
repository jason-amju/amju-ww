#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <string>
#include "WWGameObject.h"

namespace Amju
{
// Base class for the Objective for a balance game level
// This impl checks for all dead dinos 
// Subclasses can search for level-specific objectives
class Objective : public WWGameObject
{
public:
  static const char* NAME;

  Objective();

  // GameObject overrides
  virtual const char* GetTypeName() const override;
  virtual WWGameObject* Clone() override;
  virtual bool Load(File*) override;
  virtual bool Save(File*) override;
  virtual void Update() override;
  virtual void Reset() override;
  virtual void AddPropertiesGui(PropertiesDialog* dlg) override;
  virtual PropertyValue GetProp(PropertyKey) override;
  virtual void SetProp(PropertyKey, PropertyValue) override;

protected:
  // Called by Update() when we have completed the objective for the level
  void ObjectiveComplete();

protected:
  bool m_isComplete;
  std::string m_text;
};
}

#endif
