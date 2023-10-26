#include "main.hpp"
#include "Callbacks.hpp"
#include "BSEvents.hpp"
#include "DelegateHelper.hpp"

#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/PauseController.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/BeatmapObjectManager_NoteWasCutDelegate.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/ObstacleSaberSparkleEffectManager.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"

#include "System/Action_2.hpp"

#include "Zenject/DiContainer.hpp"

GlobalNamespace::GameScenesManager* gameScenesManager;

using SceneLoadedAction = System::Action_2<GlobalNamespace::ScenesTransitionSetupDataSO *, Zenject::DiContainer *>;
using SceneTransitionFinishedAction = System::Action_2<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO *, GlobalNamespace::LevelCompletionResults *>;

SafePtr<SceneLoadedAction> gameSceneLoadedDelegate;
SafePtr<SceneLoadedAction> menuSceneLoadedDelegate;
SafePtr<SceneLoadedAction> menuSceneLoadedFreshDelegate;
SafePtr<SceneTransitionFinishedAction> transitionSetupDidFinishDelegate;

bool lastMainSceneWasNotMenu;

#define INVOKE(event, ...) \
try { \
    getLogger().info("Invoking event '%s'", #event); \
    BSEvents::event.invoke(__VA_ARGS__); \
} catch(std::exception& err) { \
    getLogger().error("Error while invoking event: %s", err.what()); \
}

namespace BSEvents {

    void OnSoftRestart()
    {
        INVOKE(softRestart);
        gameScenesManager = nullptr;
    }

    void TransitionSetupDidFinish(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* results)
    {
        switch (results->levelEndStateType)
        {
            case GlobalNamespace::LevelCompletionResults::LevelEndStateType::Cleared:
                INVOKE(levelCleared, setupData, results);
                break;
            case GlobalNamespace::LevelCompletionResults::LevelEndStateType::Failed:
                INVOKE(levelFailed, setupData, results);
                break;

        }
        switch(results->levelEndAction)
        {
            case GlobalNamespace::LevelCompletionResults::LevelEndAction::Quit:
                INVOKE(levelQuit, setupData, results);
                break;
            case GlobalNamespace::LevelCompletionResults::LevelEndAction::Restart:
                INVOKE(levelRestarted, setupData, results);
                break;
        }
    }

    void OnGameSceneLoaded(GlobalNamespace::ScenesTransitionSetupDataSO * setupData, Zenject::DiContainer * container)
    {
        auto gameScenesManager2 = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().FirstOrDefault();
        if(gameSceneLoadedDelegate.isHandleValid())
            gameScenesManager->remove_transitionDidFinishEvent(gameSceneLoadedDelegate.ptr());

        auto pauseController = container->TryResolve<GlobalNamespace::PauseController*>();
        if(pauseController)
        {
            pauseController->add_didPauseEvent(DelegateHelper::CreateDelegate(std::function([]{ INVOKE(songPaused); })));
            pauseController->add_didResumeEvent(DelegateHelper::CreateDelegate(std::function([]{ INVOKE(songUnpaused); })));
        }

        auto beatmapObjectManager = container->TryResolve<GlobalNamespace::BeatmapObjectManager*>();
        if(beatmapObjectManager)
        {
            beatmapObjectManager->add_noteWasCutEvent(custom_types::MakeDelegate<GlobalNamespace::BeatmapObjectManager::NoteWasCutDelegate*>(std::function([](GlobalNamespace::NoteController* controller, GlobalNamespace::NoteCutInfo* cutInfo)
            { INVOKE(noteWasCut, controller, cutInfo); })));
            beatmapObjectManager->add_noteWasMissedEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::NoteController* controller)
            { INVOKE(noteWasMissed, controller); })));
        }
        
        auto comboController = container->TryResolve<GlobalNamespace::ComboController*>();
        if(comboController)
        {
            comboController->add_comboDidChangeEvent(DelegateHelper::CreateDelegate(std::function([](int combo){ INVOKE(comboDidChange, combo); })));
            comboController->add_comboBreakingEventHappenedEvent(DelegateHelper::CreateDelegate(std::function([]{ INVOKE(comboDidBreak); })));
        }

        auto scoreController = container->TryResolve<GlobalNamespace::ScoreController*>();
        if(scoreController)
        {
            scoreController->add_multiplierDidChangeEvent(DelegateHelper::CreateDelegate(std::function([](int multiplier, float progress)
            { 
                INVOKE(multiplierDidChange, multiplier, progress); 
                if(multiplier > 1 && progress < 0.1f)
                {
                    INVOKE(multiplierDidIncrease, multiplier);
                }
            })));
            scoreController->add_scoreDidChangeEvent(DelegateHelper::CreateDelegate(std::function([](int multipliedScore, int modifiedScore)
            { INVOKE(scoreDidChange, multipliedScore, modifiedScore); })));
        }

        auto sparkleManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ObstacleSaberSparkleEffectManager*>().FirstOrDefault();
        if(sparkleManager)
        {
            sparkleManager->add_sparkleEffectDidStartEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::SaberType saberType)
            { INVOKE(sabersStartCollide, saberType); })));
            sparkleManager->add_sparkleEffectDidEndEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::SaberType saberType)
            { INVOKE(sabersEndCollide, saberType); })));
        }

        auto gameEnergyCounter = container->TryResolve<GlobalNamespace::GameEnergyCounter*>();
        if(gameEnergyCounter)
        {
            gameEnergyCounter->add_gameEnergyDidReach0Event(DelegateHelper::CreateDelegate(std::function([]{ INVOKE(energyReachedZero); })));
            gameEnergyCounter->add_gameEnergyDidChangeEvent(DelegateHelper::CreateDelegate(std::function([](float energy){ INVOKE(energyDidChange, energy); })));
        }

        auto transitionData = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*>().FirstOrDefault();
        if(transitionData)
        {
            if(transitionSetupDidFinishDelegate.isHandleValid())
                transitionData->remove_didFinishEvent(transitionSetupDidFinishDelegate.ptr());
            else
                transitionSetupDidFinishDelegate = DelegateHelper::CreateDelegate(std::function(TransitionSetupDidFinish));
            transitionData->add_didFinishEvent(transitionSetupDidFinishDelegate.ptr());
        }
        INVOKE(gameSceneLoaded);
    }

    void OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO * setupData, Zenject::DiContainer * container)
    {
        gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedFreshDelegate.ptr());

        auto levelDetailViewController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailViewController*>().FirstOrDefault();
        levelDetailViewController->add_didChangeDifficultyBeatmapEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::StandardLevelDetailViewController* viewController, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap)
        { INVOKE(difficultySelected, viewController, difficultyBeatmap); })));

        auto segmentedControlController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::BeatmapCharacteristicSegmentedControlController*>().FirstOrDefault();
        segmentedControlController->add_didSelectBeatmapCharacteristicEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::BeatmapCharacteristicSegmentedControlController* controller, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
        { INVOKE(characteristicSelected, controller, characteristic); })));

        auto navigationController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelSelectionNavigationController*>().FirstOrDefault();
        navigationController->add_didSelectLevelPackEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::LevelSelectionNavigationController * controller, GlobalNamespace::IBeatmapLevelPack * levelPack)
        { INVOKE(levelPackSelected, controller, levelPack); })));

        auto collectionViewController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelCollectionViewController*>().FirstOrDefault();
        collectionViewController->add_didSelectLevelEvent(DelegateHelper::CreateDelegate(std::function([](GlobalNamespace::LevelCollectionViewController* controller, GlobalNamespace::IPreviewBeatmapLevel* level)
        { INVOKE(levelSelected, controller, level); })));

        INVOKE(earlyMenuSceneLoadedFresh, setupData);
        INVOKE(lateMenuSceneLoadedFresh, setupData);
    }

    void OnMenuSceneLoaded(GlobalNamespace::ScenesTransitionSetupDataSO* setupData, Zenject::DiContainer* container)
    {
        gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedDelegate.ptr());
        INVOKE(menuSceneLoaded);
    }

    void OnSceneManagerActiveSceneChanged(UnityEngine::SceneManagement::Scene prev, UnityEngine::SceneManagement::Scene next)
    {
        if (next.get_name() == "GameCore")
        {
            INVOKE(gameSceneActive);

            gameScenesManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().FirstOrDefault();
            if(gameScenesManager)
            {
                if(gameSceneLoadedDelegate.isHandleValid())
                    gameScenesManager->remove_transitionDidFinishEvent(gameSceneLoadedDelegate.ptr());
                else
                    gameSceneLoadedDelegate = DelegateHelper::CreateDelegate(std::function(OnGameSceneLoaded));
                gameScenesManager->add_transitionDidFinishEvent(gameSceneLoadedDelegate.ptr());
            }
        }
        else if(next.get_name() == "MainMenu")
        {
            gameScenesManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().FirstOrDefault();
            INVOKE(menuSceneActive);
 
            if(gameScenesManager && gameScenesManager->m_CachedPtr.m_value)
            {
                if(prev.get_name() == "EmptyTransition" && !lastMainSceneWasNotMenu)
                {
                    if(menuSceneLoadedFreshDelegate.isHandleValid())
                        gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedFreshDelegate.ptr());
                    else
                        menuSceneLoadedFreshDelegate = DelegateHelper::CreateDelegate(std::function(OnMenuSceneLoadedFresh));
                    gameScenesManager->add_transitionDidFinishEvent(menuSceneLoadedFreshDelegate.ptr());
                }
                else
                {
                    if(menuSceneLoadedDelegate.isHandleValid())
                       gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedDelegate.ptr());
                    else
                        menuSceneLoadedDelegate = DelegateHelper::CreateDelegate(std::function(OnMenuSceneLoaded));
                    gameScenesManager->add_transitionDidFinishEvent(menuSceneLoadedDelegate.ptr());
                }
            }
            lastMainSceneWasNotMenu = false;
        }
        if(next.get_name() == "GameCore" || next.get_name() == "Credits")
            lastMainSceneWasNotMenu = true;
    }

    void TriggerLevelFinishedEvent(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* completionResults)
    {
        BSEvents::SoloLevelFinishedWithResultsEventArgs arg(setupData, completionResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerMultiplayerLevelDidFinish(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* completionResults, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerResults)
    {
        BSEvents::MultiplayerLevelFinishedEventArgs arg(setupData, completionResults, otherPlayerResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerMissionLevelDidFinish(GlobalNamespace::MissionLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::MissionCompletionResults* missionCompletionResults)
    {
        BSEvents::CampaignLevelFinishedEventArgs arg(setupData, missionCompletionResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerTutorialLevelDidFinish(GlobalNamespace::TutorialScenesTransitionSetupDataSO* setupData, GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endState)
    {
        BSEvents::TutorialLevelFinishedEventArgs arg(setupData, endState);
        INVOKE(levelFinished, arg);
    }

    void TriggerMultiplayerDidDisconnect(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::DisconnectedReason disconnectedReason)
    {
        INVOKE(multiplayerDidDisconnect, setupData, disconnectedReason);
    }
}