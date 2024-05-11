#include "main.hpp"
#include "BSEvents.hpp"
#include "Callbacks.hpp"
#include "DelegateHelper.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

#include "GlobalNamespace/MultiplayerResultsData.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"

using namespace GlobalNamespace;

SafePtr<System::Action_2<UnityW<StandardLevelScenesTransitionSetupDataSO>, LevelCompletionResults*>> standardLevelScenesDelegate;
SafePtr<System::Action_2<UnityW<MultiplayerLevelScenesTransitionSetupDataSO>, MultiplayerResultsData*>> multiplayerLevelScenesDelegate;
SafePtr<System::Action_2<UnityW<MultiplayerLevelScenesTransitionSetupDataSO>, GlobalNamespace::DisconnectedReason>> multiplayerDisconnectedDelegate;
SafePtr<System::Action_2<UnityW<MissionLevelScenesTransitionSetupDataSO>, MissionCompletionResults*>> missionLevelScenesDelegate;
SafePtr<System::Action_2<UnityW<TutorialScenesTransitionSetupDataSO>, TutorialScenesTransitionSetupDataSO::TutorialEndStateType>> tutorialLevelScenesDelegate;

std::optional<BSEvents::LevelType> currentLevelType = std::nullopt;

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prev, UnityEngine::SceneManagement::Scene next)
{
    SceneManager_Internal_ActiveSceneChanged(prev, next);
    BSEvents::OnSceneManagerActiveSceneChanged(prev, next);
}

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, static_cast<void(StandardLevelScenesTransitionSetupDataSO::*)(StringW, ::GlobalNamespace::IBeatmapLevelData*, ByRef<::GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ::GlobalNamespace::OverrideEnvironmentSettings*, GlobalNamespace::ColorScheme*, ::GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, ::GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::PracticeSettings*, ::GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, ::GlobalNamespace::BeatmapDataLoader*, StringW, bool, bool, System::Nullable_1<::GlobalNamespace::RecordingToolManager::SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init), 
    void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ::GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, ::GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::__RecordingToolManager__SetupData> recordingToolData)
{
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapLevelData, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, backButtonText, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    currentLevelType = BSEvents::LevelType::SoloParty;

    if(standardLevelScenesDelegate.isHandleValid())
        self->remove_didFinishEvent(standardLevelScenesDelegate.ptr());
    else
        standardLevelScenesDelegate = DelegateHelper::CreateDelegate(std::function(BSEvents::TriggerLevelFinishedEvent));
    self->add_didFinishEvent(standardLevelScenesDelegate.ptr());
}

MAKE_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ::GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, bool useTestNoteCutSoundEffects)
{
    MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapKey, beatmapLevel, beatmapLevelData, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, audioClipAsyncLoader, beatmapDataLoader, useTestNoteCutSoundEffects);
    currentLevelType = BSEvents::LevelType::Multiplayer;
    
    if(multiplayerLevelScenesDelegate.isHandleValid())
        self->remove_didFinishEvent(multiplayerLevelScenesDelegate.ptr());
    else
        multiplayerLevelScenesDelegate = DelegateHelper::CreateDelegate(std::function([](UnityW<MultiplayerLevelScenesTransitionSetupDataSO> scenesTransitionSetupData, MultiplayerResultsData* resultsData)
        { BSEvents::TriggerMultiplayerLevelDidFinish(scenesTransitionSetupData, resultsData->localPlayerResultData->multiplayerLevelCompletionResults->levelCompletionResults, resultsData->otherPlayersData); }));
    self->add_didFinishEvent(multiplayerLevelScenesDelegate.ptr());

    if(multiplayerDisconnectedDelegate.isHandleValid())
        self->remove_didDisconnectEvent(multiplayerDisconnectedDelegate.ptr());
    else
        multiplayerDisconnectedDelegate = DelegateHelper::CreateDelegate(std::function(BSEvents::TriggerMultiplayerDidDisconnect));
    self->add_didDisconnectEvent(multiplayerDisconnectedDelegate.ptr());
}

MAKE_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init, static_cast<void(MissionLevelScenesTransitionSetupDataSO::*)(StringW , ::GlobalNamespace::IBeatmapLevelData* , ByRef<::GlobalNamespace::BeatmapKey> , ::GlobalNamespace::BeatmapLevel*, ArrayW<::GlobalNamespace::MissionObjective*, ::Array<::GlobalNamespace::MissionObjective*>*> , ::GlobalNamespace::ColorScheme* , GlobalNamespace::GameplayModifiers* , GlobalNamespace::PlayerSpecificSettings* , GlobalNamespace::EnvironmentsListModel* , GlobalNamespace::AudioClipAsyncLoader*, ::GlobalNamespace::BeatmapDataLoader*, StringW)>(&MissionLevelScenesTransitionSetupDataSO::Init), 
    void, MissionLevelScenesTransitionSetupDataSO* self, StringW missionId, ::GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel, ArrayW<::GlobalNamespace::MissionObjective*, ::Array<::GlobalNamespace::MissionObjective*>*> missionObjectives, ::GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, ::StringW backButtonText)
{
    MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, beatmapLevelData, beatmapKey, beatmapLevel, missionObjectives, overrideColorScheme, gameplayModifiers, playerSpecificSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, backButtonText);
    currentLevelType = BSEvents::LevelType::Campaign;
    
    if(missionLevelScenesDelegate.isHandleValid())
        self->remove_didFinishEvent(missionLevelScenesDelegate.ptr());
    else
        missionLevelScenesDelegate = DelegateHelper::CreateDelegate(std::function(BSEvents::TriggerMissionLevelDidFinish));
    self->add_didFinishEvent(missionLevelScenesDelegate.ptr());
}

MAKE_HOOK_MATCH(TutorialScenesTransitionSetupDataSO_Init, &TutorialScenesTransitionSetupDataSO::Init, void, TutorialScenesTransitionSetupDataSO* self, GlobalNamespace::PlayerSpecificSettings *playerSpecificSettings)
{
    TutorialScenesTransitionSetupDataSO_Init(self, playerSpecificSettings);
    currentLevelType = BSEvents::LevelType::Tutorial;

    if(tutorialLevelScenesDelegate.isHandleValid())
        self->remove_didFinishEvent(tutorialLevelScenesDelegate.ptr());
    else
        tutorialLevelScenesDelegate = DelegateHelper::CreateDelegate(std::function(BSEvents::TriggerTutorialLevelDidFinish));
    self->add_didFinishEvent(tutorialLevelScenesDelegate.ptr());
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* action)
{
    MenuTransitionsHelper_RestartGame(self, action);

    BSEvents::OnSoftRestart();
    currentLevelType = std::nullopt;
}

namespace BSEvents
{
    void Init()
    {
        static bool initialised = false;

        if(!initialised)
        {
            il2cpp_functions::Init();
            logger.info("Installing Hooks...");
            INSTALL_HOOK(logger, SceneManager_Internal_ActiveSceneChanged);
            INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
            INSTALL_HOOK(logger, MultiplayerLevelScenesTransitionSetupDataSO_Init);
            INSTALL_HOOK(logger, MissionLevelScenesTransitionSetupDataSO_Init);
            INSTALL_HOOK(logger, TutorialScenesTransitionSetupDataSO_Init);
            INSTALL_HOOK(logger, MenuTransitionsHelper_RestartGame);
            logger.info("Installed Hooks!");
            initialised = true;
        }
    }

    std::optional<LevelType> get_currentLevelType()
    { return currentLevelType; }

    UnorderedEventCallback<> softRestart;

    UnorderedEventCallback<> menuSceneActive;
    UnorderedEventCallback<> menuSceneLoaded;

    UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO *> earlyMenuSceneLoadedFresh;
    UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO *> lateMenuSceneLoadedFresh;

    UnorderedEventCallback<> gameSceneActive;
    UnorderedEventCallback<> gameSceneLoaded;

    UnorderedEventCallback<GlobalNamespace::StandardLevelDetailViewController *, GlobalNamespace::BeatmapKey> difficultySelected;
    UnorderedEventCallback<GlobalNamespace::BeatmapCharacteristicSegmentedControlController *, GlobalNamespace::BeatmapCharacteristicSO *> characteristicSelected;
    UnorderedEventCallback<GlobalNamespace::LevelSelectionNavigationController *, GlobalNamespace::BeatmapLevelPack *> levelPackSelected;
    UnorderedEventCallback<GlobalNamespace::LevelCollectionViewController *, GlobalNamespace::BeatmapLevel *> levelSelected;

    UnorderedEventCallback<> songPaused;
    UnorderedEventCallback<> songUnpaused;

    UnorderedEventCallback<BSEvents::LevelFinishedEventArgs> levelFinished;
    UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO *, GlobalNamespace::LevelCompletionResults*> levelCleared;
    UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO *, GlobalNamespace::LevelCompletionResults*> levelQuit;
    UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO *, GlobalNamespace::LevelCompletionResults*> levelFailed;
    UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO *, GlobalNamespace::LevelCompletionResults*> levelRestarted;

    UnorderedEventCallback<GlobalNamespace::NoteController *, GlobalNamespace::NoteCutInfo*> noteWasCut;
    UnorderedEventCallback<GlobalNamespace::NoteController *> noteWasMissed;

    UnorderedEventCallback<int, float> multiplierDidChange;
    UnorderedEventCallback<int> multiplierDidIncrease;
    UnorderedEventCallback<int> comboDidChange;
    UnorderedEventCallback<> comboDidBreak;
    UnorderedEventCallback<int, int> scoreDidChange;
    UnorderedEventCallback<float> energyDidChange;
    UnorderedEventCallback<> energyReachedZero;

    UnorderedEventCallback<GlobalNamespace::SaberType> sabersStartCollide;
    UnorderedEventCallback<GlobalNamespace::SaberType> sabersEndCollide;

    UnorderedEventCallback<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, GlobalNamespace::DisconnectedReason> multiplayerDidDisconnect;
}