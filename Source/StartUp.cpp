#include <AmjuFirst.h>
#include <Game.h>
#include <AmjuGLWindowInfo.h>
#include <BruteForce.h>
#include <ResourceManager.h>
#include <File.h>
#include <AlphaBmpLoader.h>
#include <ObjMesh.h>
#include <Font.h>
#include <SoundManager.h>
#include <SceneGraph.h>
#include <SceneNode.h>
#include <SceneMesh.h>
#include <BinaryResource.h>
#include <FileImplGlue.h>
#include <GlueFileMem.h>
#include <Localise.h>
#include <ReportError.h>
#include <File.h>
#include <Screen.h>
#include <Directory.h>
#include <ConfigFile.h>
#include <ROConfig.h>

#ifdef AMJU_USE_BASS
#include <BassSoundPlayer.h>
#endif

#ifdef GEKKO
#include <SoundWii.h>
#endif

#include <GuiButton.h>
#include <EventPoller.h>
#include "CursorManager.h"
#include "GameConsts.h"
#include "GSLogo.h"
#include "GSLoadLayers.h" 
#include "GSCopyAssets.h"
#include "GSPaused.h"
#include "HiScoreDb.h"
#include "Hud.h"
#include "Init.h"
#include "LevelManager.h"
#include "NetSend.h"
#include "ResizeHandler.h"
#include "SaveDir.h"
#include "StartGame.h"
#include "StartUp.h"
#include "Version.h"
#include <AmjuFinal.h>

//#define SHOW_FRAME_RATE

#ifdef AMJU_IOS
#define YES_GLUE_FILE
#define YES_BINARY_OBJ_FILES
#define GLUE_FILE "data-iOS.glue"
#define MUSIC_GLUE_FILE "music-iOS.glue"
#endif

#ifdef MACOSX
#define GLUE_FILE "data-Mac.glue"
#define MUSIC_GLUE_FILE "music-Mac.glue"
#endif

#ifdef WIN32
#define YES_GLUE_FILE
#define YES_BINARY_OBJ_FILES
#define GLUE_FILE "data-win.glue"
#define MUSIC_GLUE_FILE "music-win.glue"
#endif

#ifdef GEKKO
#define GLUE_FILE "data-wii.glue"
#define MUSIC_GLUE_FILE "music-wii.glue"
#endif

#ifdef ANDROID_NDK
#define GLUE_FILE "data-android.glue"
#define MUSIC_GLUE_FILE "music-android.glue"
#endif

namespace Amju
{
extern Amju::AmjuGLWindowInfo w;

// Filename for the writable game config file, not the read-only config.
static std::string ConfigFilename()
{
  std::string filename = GetSaveDir() + "config.txt";

#ifdef _DEBUG
std::cout << "Config file: " << filename << "\n";
#endif

  return filename;
}

void StartUpBeforeCreateWindow()
{
#ifndef GLOBAL_INIT
  Init();
#endif

#if defined(AMJU_IOS)
  std::string dataDir = GetDataDir();
  std::cout << "Data Dir: " << dataDir << "\n";
  File::SetRoot(dataDir, "/");
#endif

#ifdef GEKKO
  // TODO Better to put this in library main() if we can get the app's directory
  File::SetRoot("/apps/amju_rd-v.1.0-wii/data/", "/");
#endif

#if defined(WIN32) || defined(MACOSX) || defined(GEKKO) || defined(AMJU_IOS)
  SetROConfigFilename(GetSaveDir() + "roconfig.txt");
#endif

  GameConfigFile* gcf = TheGameConfigFile::Instance();
  std::string filename = ConfigFilename();
  gcf->SetFilePath(filename);
  if (FileExists(filename))
  {
    if (gcf->Load())
    {
      std::cout << "Loaded game config file OK: " << filename << "\n";
      // Send updated device info if anything has changed since we last sent.
     
#ifdef SEND_FIRST_TIME_DEVICE_INFO
      NetSendDeviceInfoFirstRunEver();
#else
      if (gcf->Exists(DEVICE_ID))
      {
        // We already successfully sent info to the server, so check if we should update.
        NetSendUpdateDeviceInfo();
      }
      else
      {
        // Confog file exists but no record we successfully sent info to the server.
        NetSendDeviceInfoFirstRunEver();
      }
#endif
      
      // Set sound/music from config
      // Hmm, sound player impl is not set yet but that's ok...
      auto sm = TheSoundManager::Instance();
      sm->SetSongMaxVolume(gcf->GetInt(MUSIC_KEY)? 1.0f : 0.0f);
      sm->SetWavMaxVolume(gcf->GetInt(SOUND_KEY)? 1.0f : 0.0f);
    }
    else
    {
      std::cout << "Failed to load game config file: " << filename << "\n";
      // TODO This is serious, what to do?
    }
  }
  else
  {
    std::cout << "No game config file exists: " << filename << "\n";
    NetSendDeviceInfoFirstRunEver();
    gcf->SetInt(SOUND_KEY, SOUND_INITIAL_VAL);
    gcf->SetInt(MUSIC_KEY, MUSIC_INITIAL_VAL);
  }

#if defined(WIN32) || defined(MACOSX)
  // Can resize 
  const char* SCREEN_X = "screen-x"; // TODO Must match ResizeHandler - use GameConsts
  const char* SCREEN_Y = "screen-y";

  /*
  if (gcf->Exists(SCREEN_X) && gcf->Exists(SCREEN_Y))
  {
    w.SetWidth(gcf->GetInt(SCREEN_X));
    w.SetHeight(gcf->GetInt(SCREEN_Y));
  }
  */
#endif

#ifdef YES_GLUE_FILE
  GlueFileMem* gfm = new GlueFileMem;  
  if (FileImplGlue::OpenGlueFile(GLUE_FILE, gfm))
  {
    std::cout << "Opened glue file " << GLUE_FILE << "\n";
  }
  else
  {
    ReportError("Failed to open data glue file");
  }
#endif // YES_GLUE_FILE

  // Set up music glue file
  SoundManager* sm = TheSoundManager::Instance();
  
#if defined (AMJU_USE_BASS)
  sm->SetImpl(new BassSoundPlayer);
#endif

#ifdef GEKKO
  sm->SetImpl(new SoundWii);
#endif

#ifdef YES_GLUE_FILE
  GlueFile* pMusicGlueFile = new GlueFileMem;
  if (pMusicGlueFile->OpenGlueFile(MUSIC_GLUE_FILE, true /* read only */))
  {
    sm->SetGlueFile(pMusicGlueFile);
  }
  else
  {
    ReportError("Failed to open music glue file");
  }
#endif
  
  // Start request for hi scores from server, etc
  HiScoreStartUp();
}

void StartUpAfterCreateWindow()
{
  Game* game = TheGame::Instance();
  game->SetUpdateCopy(true); // if updating game objects can create/destroy objects

  AmjuGL::SetClearColour(Colour(0, 0, 0, 1.0f));

  // TODO Other languages - preferences
  if (!Localise::LoadStringTable("english.txt"))
  {
    ReportError("No localise string table.");
  }

  // Add resource loaders
  ResourceManager* rm = TheResourceManager::Instance();

#ifdef YES_BINARY_OBJ_FILES
  rm->AddLoader("obj", BinaryObjLoader);
#else
  rm->AddLoader("obj", TextObjLoader);
#endif

  rm->AddLoader("font", FontLoader);
  rm->AddLoader("mod", BinaryResourceLoader);
  rm->AddLoader("snd", BinaryResourceLoader);
  rm->AddLoader("wav", BinaryResourceLoader);

#if defined(WIN32) || defined(MACOSX)
    TheCursorManager::Instance()->Load("hand.png", Vec2f(0.025f, -0.08f)); // hotspot position
#endif
    
  TheHud::Instance()->Load();

  // Set collision system
  CollisionDetector* cd = new BruteForce;
  // Default intersection test is OK
  TheCollisionManager::Instance()->SetCollisionDetector(cd);

#if defined(WIN32) || defined(MACOSX)
  TheEventPoller::Instance()->AddListener(new ResizeHandler);
#endif

  GuiButton::SetClickFilename("sound/navigate_40.wav");

#ifdef SHOW_FRAME_RATE
  Font* font = (Font*)rm->GetRes("font2d/arial-font.font");
  game->SetFrameTimeFont(font);
#endif

#if defined(WIN32) || defined(MACOSX)
#ifdef YES_GLUE_FILE
  TheGame::Instance()->SetCurrentState(TheGSCopyAssets::Instance());
#else
  // No glue file, don't try to copy from it!
  TheGame::Instance()->SetCurrentState(TheGSLogo::Instance());
#endif
#else
  // iOS/Android: go to logo state, all assets are in glue file
  //  (no downloadable content [for now?!])
  //TheGame::Instance()->SetCurrentState(TheGSLogo::Instance());

  // No need for logo state as we show it as app load image
  //TheGame::Instance()->SetCurrentState(TheGSLoadLayers::Instance());


  TheGame::Instance()->SetCurrentState(TheGSCopyAssets::Instance());
#endif
}
}

