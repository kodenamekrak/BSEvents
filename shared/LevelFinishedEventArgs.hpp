#pragma once

#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerPlayerResultsData.hpp"
#include "GlobalNamespace/MultiplayerLevelCompletionResults.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MissionCompletionResults.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/TutorialScenesTransitionSetupDataSO.hpp"

#include "System/Collections/Generic/IReadOnlyCollection_1.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"

namespace BSEvents {

    enum class LevelType {
        SoloParty, 
        Multiplayer,
        Campaign,
        Tutorial
    };

    struct LevelFinishedEventArgs {

        LevelType levelType;

        GlobalNamespace::ScenesTransitionSetupDataSO* scenesTransitionSetupDataSO;

        LevelFinishedEventArgs(LevelType _levelType, GlobalNamespace::ScenesTransitionSetupDataSO* setupData): levelType(_levelType), scenesTransitionSetupDataSO(setupData) {}
    };

    struct LevelFinishedWithResultsEventArgs : public LevelFinishedEventArgs {
        
        GlobalNamespace::LevelCompletionResults* levelCompletionResults;

        LevelFinishedWithResultsEventArgs(LevelType _levelType, GlobalNamespace::LevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* results)
        : LevelFinishedEventArgs(_levelType, setupData), levelCompletionResults(results) {}
    };

    struct SoloLevelFinishedWithResultsEventArgs : public LevelFinishedWithResultsEventArgs {

        SoloLevelFinishedWithResultsEventArgs(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* completionResults)
        : LevelFinishedWithResultsEventArgs(LevelType::SoloParty, setupData, completionResults) {}
    };;

    struct MultiplayerLevelFinishedEventArgs : public LevelFinishedWithResultsEventArgs {

        System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerCompletionResults;

        GlobalNamespace::LevelCompletionResults* GetPlayerResults(StringW playerId)
        {
            GlobalNamespace::LevelCompletionResults* results;

            for(int i = 0; i < otherPlayerCompletionResults->i___System__Collections__Generic__IReadOnlyCollection_1_T_()->get_Count(); i++)
            {
                auto v = otherPlayerCompletionResults->get_Item(i);
                if(v->connectedPlayer->get_userId() == playerId)
                {
                    results = v->multiplayerLevelCompletionResults->levelCompletionResults;
                    break;
                }
            }
            return results;
        }

        MultiplayerLevelFinishedEventArgs(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::LevelCompletionResults* levelCompletionResults, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::MultiplayerPlayerResultsData*>* otherPlayerResults)
        : LevelFinishedWithResultsEventArgs(LevelType::Multiplayer, setupData, levelCompletionResults), otherPlayerCompletionResults(otherPlayerResults) {}
    };

    struct CampaignLevelFinishedEventArgs : public LevelFinishedWithResultsEventArgs {

        GlobalNamespace::MissionCompletionResults* missionCompletionResults;

        CampaignLevelFinishedEventArgs(GlobalNamespace::MissionLevelScenesTransitionSetupDataSO* setupData, GlobalNamespace::MissionCompletionResults* campaignMissionCompletionResults)
        : LevelFinishedWithResultsEventArgs(LevelType::Campaign, setupData, campaignMissionCompletionResults->levelCompletionResults), missionCompletionResults(campaignMissionCompletionResults) {}
    };

    struct TutorialLevelFinishedEventArgs : public LevelFinishedEventArgs {

        GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endStateType;

        TutorialLevelFinishedEventArgs(GlobalNamespace::TutorialScenesTransitionSetupDataSO* setupData, GlobalNamespace::TutorialScenesTransitionSetupDataSO::TutorialEndStateType endState)
        : LevelFinishedEventArgs(LevelType::Tutorial, setupData), endStateType(endState) {}
    };
}