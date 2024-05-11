#include "main.hpp"
#include "Callbacks.hpp"
#include "BSEvents.hpp"
#include "DelegateHelper.hpp"

#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/PauseController.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/ObstacleSaberSparkleEffectManager.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"

#include "System/Action_2.hpp"

#include "Zenject/DiContainer.hpp"

GlobalNamespace::GameScenesManager* gameScenesManager;

using SceneLoadedAction = System::Action_2<UnityW<GlobalNamespace::ScenesTransitionSetupDataSO>, Zenject::DiContainer *>;
using SceneTransitionFinishedAction = System::Action_2<UnityW<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO>, GlobalNamespace::LevelCompletionResults *>;

SafePtr<SceneLoadedAction> gameSceneLoadedDelegate;
SafePtr<SceneLoadedAction> menuSceneLoadedDelegate;
SafePtr<SceneLoadedAction> menuSceneLoadedFreshDelegate;
SafePtr<SceneTransitionFinishedAction> transitionSetupDidFinishDelegate;

bool lastMainSceneWasNotMenu;

#define INVOKE(event, ...) \
try { \
    logger.info("Invoking event '{}'", #event); \
    BSEvents::event.invoke(__VA_ARGS__); \
} catch(std::exception& err) { \
    logger.error("Error while invoking event: {}", err.what()); \
}

namespace BSEvents {

    void OnSoftRestart()
    {
        INVOKE(softRestart);
        gameScenesManager = nullptr;
    }

    void TransitionSetupDidFinish(UnityW<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* results)
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

    void OnGameSceneLoaded(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container)
    {
        auto gameScenesManager2 = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().front_or_default();
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
            beatmapObjectManager->add_noteWasCutEvent(custom_types::MakeDelegate<GlobalNamespace::BeatmapObjectManager::NoteWasCutDelegate*>(std::function([](UnityW<GlobalNamespace::NoteController> controller, GlobalNamespace::NoteCutInfo* cutInfo)
            { INVOKE(noteWasCut, controller, cutInfo); })));
            beatmapObjectManager->add_noteWasMissedEvent(DelegateHelper::CreateDelegate(std::function([](UnityW<GlobalNamespace::NoteController> controller)
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

        auto sparkleManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ObstacleSaberSparkleEffectManager*>().front_or_default();
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

        auto transitionData = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*>().front_or_default();
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

    void OnMenuSceneLoadedFresh(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container)
    {
        gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedFreshDelegate.ptr());

        auto levelDetailViewController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailViewController*>().front_or_default();
        levelDetailViewController->add_didChangeDifficultyBeatmapEvent(DelegateHelper::CreateDelegate(std::function([](UnityW<GlobalNamespace::StandardLevelDetailViewController> viewController)
        { INVOKE(difficultySelected, viewController, viewController->beatmapKey); })));

        auto segmentedControlController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::BeatmapCharacteristicSegmentedControlController*>().front_or_default();
        segmentedControlController->add_didSelectBeatmapCharacteristicEvent(DelegateHelper::CreateDelegate(std::function([](UnityW<GlobalNamespace::BeatmapCharacteristicSegmentedControlController> controller, UnityW<GlobalNamespace::BeatmapCharacteristicSO> characteristic)
        { INVOKE(characteristicSelected, controller, characteristic); })));

        auto navigationController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelSelectionNavigationController*>().front_or_default();
        navigationController->add_didSelectLevelPackEvent(DelegateHelper::CreateDelegate(std::function([](UnityW<GlobalNamespace::LevelSelectionNavigationController> controller, GlobalNamespace::BeatmapLevelPack * levelPack)
        { INVOKE(levelPackSelected, controller, levelPack); })));

        auto collectionViewController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelCollectionViewController*>().front_or_default();
        collectionViewController->add_didSelectLevelEvent(DelegateHelper::CreateDelegate(std::function([](UnityW<GlobalNamespace::LevelCollectionViewController> controller, GlobalNamespace::BeatmapLevel* level)
        { INVOKE(levelSelected, controller, level); })));

        INVOKE(earlyMenuSceneLoadedFresh, setupData);
        INVOKE(lateMenuSceneLoadedFresh, setupData);
    }

    void OnMenuSceneLoaded(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container)
    {
        gameScenesManager->remove_transitionDidFinishEvent(menuSceneLoadedDelegate.ptr());
        INVOKE(menuSceneLoaded);
    }

    void OnSceneManagerActiveSceneChanged(UnityEngine::SceneManagement::Scene prev, UnityEngine::SceneManagement::Scene next)
    {
        if (next.get_name() == "GameCore")
        {
            INVOKE(gameSceneActive);

            gameScenesManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().front_or_default();
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
            gameScenesManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameScenesManager*>().front_or_default();
            INVOKE(menuSceneActive);
 
            if(gameScenesManager && gameScenesManager->m_CachedPtr)
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

    void TriggerLevelFinishedEvent(UnityW<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* completionResults)
    {
        BSEvents::SoloLevelFinishedWithResultsEventArgs arg(setupData, completionResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerMultiplayerLevelDidFinish(UnityW<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* completionResults, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerResults)
    {
        BSEvents::MultiplayerLevelFinishedEventArgs arg(setupData, completionResults, otherPlayerResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerMissionLevelDidFinish(UnityW<GlobalNamespace::MissionLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::MissionCompletionResults* missionCompletionResults)
    {
        BSEvents::CampaignLevelFinishedEventArgs arg(setupData, missionCompletionResults);
        INVOKE(levelFinished, arg);
    }

    void TriggerTutorialLevelDidFinish(UnityW<GlobalNamespace::TutorialScenesTransitionSetupDataSO> setupData, GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endState)
    {
        BSEvents::TutorialLevelFinishedEventArgs arg(setupData, endState);
        INVOKE(levelFinished, arg);
    }

    void TriggerMultiplayerDidDisconnect(UnityW<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::DisconnectedReason disconnectedReason)
    {
        INVOKE(multiplayerDidDisconnect, setupData, disconnectedReason);
    }
}