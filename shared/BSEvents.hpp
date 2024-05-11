#pragma once

#include "LevelFinishedEventArgs.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include "GlobalNamespace/ScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/BeatmapKey.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"

#define BSEVENTS_EXPORT __attribute__((visibility("default")))

namespace BSEvents {

    // Installs event hooks
    // This MUST be called otherwise events will not be called
    BSEVENTS_EXPORT void Init();

    BSEVENTS_EXPORT std::optional<LevelType> get_currentLevelType();

    extern BSEVENTS_EXPORT UnorderedEventCallback<> softRestart;

    extern BSEVENTS_EXPORT UnorderedEventCallback<> menuSceneActive;
    extern BSEVENTS_EXPORT UnorderedEventCallback<> menuSceneLoaded;

    // Called when menu scene is loaded fresh (firstActivation)
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO*> earlyMenuSceneLoadedFresh;
    // Called when menu scene is loaded fresh (firstActivation)
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO*> lateMenuSceneLoadedFresh;

    extern BSEVENTS_EXPORT UnorderedEventCallback<> gameSceneActive;
    extern BSEVENTS_EXPORT UnorderedEventCallback<> gameSceneLoaded;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::StandardLevelDetailViewController*, GlobalNamespace::BeatmapKey> difficultySelected;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::BeatmapCharacteristicSegmentedControlController*, GlobalNamespace::BeatmapCharacteristicSO*> characteristicSelected;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::LevelSelectionNavigationController*, GlobalNamespace::BeatmapLevelPack*> levelPackSelected;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::LevelCollectionViewController*, GlobalNamespace::BeatmapLevel*> levelSelected;

    extern BSEVENTS_EXPORT UnorderedEventCallback<> songPaused;
    extern BSEVENTS_EXPORT UnorderedEventCallback<> songUnpaused;

    extern BSEVENTS_EXPORT UnorderedEventCallback<BSEvents::LevelFinishedEventArgs> levelFinished;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelCleared;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelQuit;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelFailed;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelRestarted;

    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::NoteController*, GlobalNamespace::NoteCutInfo*> noteWasCut;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::NoteController*> noteWasMissed;

    // Args: multiplier, progress
    extern BSEVENTS_EXPORT UnorderedEventCallback<int, float> multiplierDidChange;
    // Args: multiplier
    extern BSEVENTS_EXPORT UnorderedEventCallback<int> multiplierDidIncrease;
    // Args: combo
    extern BSEVENTS_EXPORT UnorderedEventCallback<int> comboDidChange;
    extern BSEVENTS_EXPORT UnorderedEventCallback<> comboDidBreak;
    // Args: multipliedScore, modifiedScore
    extern BSEVENTS_EXPORT UnorderedEventCallback<int, int> scoreDidChange;
    // Args: Energy
    extern BSEVENTS_EXPORT UnorderedEventCallback<float> energyDidChange;
    extern BSEVENTS_EXPORT UnorderedEventCallback<> energyReachedZero;

    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::SaberType> sabersStartCollide;
    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::SaberType> sabersEndCollide;

    extern BSEVENTS_EXPORT UnorderedEventCallback<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, GlobalNamespace::DisconnectedReason> multiplayerDidDisconnect;
}