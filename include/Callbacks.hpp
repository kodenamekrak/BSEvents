#pragma once

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/TutorialScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerPlayerResultsData.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"

#include "Zenject/DiContainer.hpp"

namespace BSEvents {

    void OnSoftRestart();

    void TransitionSetupDidFinish(UnityW<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* results);

    void OnGameSceneLoaded(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container );

    void OnMenuSceneLoadedFresh(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container);

    void OnMenuSceneLoaded(UnityW<GlobalNamespace::ScenesTransitionSetupDataSO> setupData, Zenject::DiContainer* container);

    void OnSceneManagerActiveSceneChanged(UnityEngine::SceneManagement::Scene prev, UnityEngine::SceneManagement::Scene next);

    void TriggerLevelFinishedEvent(UnityW<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* completionResults);

    void TriggerMultiplayerLevelDidFinish(UnityW<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::LevelCompletionResults* completionResults, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerResults);

    void TriggerMissionLevelDidFinish(UnityW<GlobalNamespace::MissionLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::MissionCompletionResults* missionCompletionResults);

    void TriggerTutorialLevelDidFinish(UnityW<GlobalNamespace::TutorialScenesTransitionSetupDataSO> setupData, GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endState);

    void TriggerMultiplayerDidDisconnect(UnityW<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO> setupData, GlobalNamespace::DisconnectedReason disconnectedReason);
}