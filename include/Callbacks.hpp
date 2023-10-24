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

    void TransitionSetupDidFinish(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* results);

    void OnGameSceneLoaded(GlobalNamespace::ScenesTransitionSetupDataSO* setupData, Zenject::DiContainer* container );

    void OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO* setupData, Zenject::DiContainer* container);

    void OnMenuSceneLoaded(GlobalNamespace::ScenesTransitionSetupDataSO* setupData, Zenject::DiContainer* container);

    void OnSceneManagerActiveSceneChanged(UnityEngine::SceneManagement::Scene prev, UnityEngine::SceneManagement::Scene next);

    void TriggerLevelFinishedEvent(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* completionResults);

    void TriggerMultiplayerLevelDidFinish(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* completionResults, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerResults);

    void TriggerMissionLevelDidFinish(GlobalNamespace::MissionLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::MissionCompletionResults* missionCompletionResults);

    void TriggerTutorialLevelDidFinish(GlobalNamespace::TutorialScenesTransitionSetupDataSO* setupData, GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endState);

    void TriggerMultiplayerDidDisconnect(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::DisconnectedReason disconnectedReason);
}