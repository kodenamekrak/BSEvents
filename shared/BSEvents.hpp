#pragma once

#include "LevelFinishedEventArgs.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include "GlobalNamespace/ScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"

namespace BSEvents {

    enum class LevelType;

    // Installs event hooks
    // This MUST be called otherwise events will not be called
    void Init();

    std::optional<LevelType> get_currentLevelType();

    extern UnorderedEventCallback<> softRestart;

    extern UnorderedEventCallback<> menuSceneActive;
    extern UnorderedEventCallback<> menuSceneLoaded;

    // Called when menu scene is loaded fresh (firstActivation)
    extern UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO*> earlyMenuSceneLoadedFresh;
    // Called when menu scene is loaded fresh (firstActivation)
    extern UnorderedEventCallback<GlobalNamespace::ScenesTransitionSetupDataSO*> lateMenuSceneLoadedFresh;

    extern UnorderedEventCallback<> gameSceneActive;
    extern UnorderedEventCallback<> gameSceneLoaded;

    extern UnorderedEventCallback<GlobalNamespace::StandardLevelDetailViewController*, GlobalNamespace::IDifficultyBeatmap*> difficultySelected;
    extern UnorderedEventCallback<GlobalNamespace::BeatmapCharacteristicSegmentedControlController*, GlobalNamespace::BeatmapCharacteristicSO*> characteristicSelected;
    extern UnorderedEventCallback<GlobalNamespace::LevelSelectionNavigationController*, GlobalNamespace::IBeatmapLevelPack*> levelPackSelected;
    extern UnorderedEventCallback<GlobalNamespace::LevelCollectionViewController*, GlobalNamespace::IPreviewBeatmapLevel*> levelSelected;


    extern UnorderedEventCallback<> songPaused;
    extern UnorderedEventCallback<> songUnpaused;

    extern UnorderedEventCallback<BSEvents::LevelFinishedEventArgs> levelFinished;
    extern UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelCleared;
    extern UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelQuit;
    extern UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelFailed;
    extern UnorderedEventCallback<GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*, GlobalNamespace::LevelCompletionResults*> levelRestarted;

    extern UnorderedEventCallback<GlobalNamespace::NoteController*, GlobalNamespace::NoteCutInfo*> noteWasCut;
    extern UnorderedEventCallback<GlobalNamespace::NoteController*> noteWasMissed;

    // Args: multiplier, progress
    extern UnorderedEventCallback<int, float> multiplierDidChange;
    // Args: multiplier
    extern UnorderedEventCallback<int> multiplierDidIncrease;
    // Args: combo
    extern UnorderedEventCallback<int> comboDidChange;
    extern UnorderedEventCallback<> comboDidBreak;
    // Args: multipliedScore, modifiedScore
    extern UnorderedEventCallback<int, int> scoreDidChange;
    // Args: Energy
    extern UnorderedEventCallback<float> energyDidChange;
    extern UnorderedEventCallback<> energyReachedZero;

    extern UnorderedEventCallback<GlobalNamespace::SaberType> sabersStartCollide;
    extern UnorderedEventCallback<GlobalNamespace::SaberType> sabersEndCollide;

    extern UnorderedEventCallback<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, GlobalNamespace::DisconnectedReason> multiplayerDidDisconnect;
}