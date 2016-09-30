# /*
#  *  BJ函数优化 -- Quest
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_QUEST_H
#define INCLUDE_BJ_OPTIMIZATION_QUEST_H
#                                                        
#  define DestroyQuestBJ(quest)                                       DestroyQuest(quest)
#  define QuestSetEnabledBJ(enabled, quest)                           QuestSetEnabled(quest, enabled)
#  define QuestSetTitleBJ(quest, title)                               QuestSetTitle(quest, title)
#  define QuestSetDescriptionBJ(quest, description)                   QuestSetDescription(quest, description)
#  define QuestSetCompletedBJ(quest, completed)                       QuestSetCompleted(quest, completed)
#  define QuestSetFailedBJ(quest, failed)                             QuestSetFailed(quest, failed)
#  define QuestSetDiscoveredBJ(quest, discovered)                     QuestSetDiscovered(quest, discovered)
#  define QuestItemSetDescriptionBJ(questItem, description)           QuestItemSetDescription(questItem, description)
#  define QuestItemSetCompletedBJ(questItem, completed)               QuestItemSetCompleted(questItem, completed)
#  define DestroyDefeatConditionBJ(condition)                         DestroyDefeatCondition(condition)
#  define DefeatConditionSetDescriptionBJ(condition, description)     DefeatConditionSetDescription(condition, description)
#  define FlashQuestDialogButtonBJ()                                  FlashQuestDialogButton()
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetLastCreatedQuestBJ()                                     bj_lastCreatedQuest
#  define GetLastCreatedQuestItemBJ()                                 bj_lastCreatedQuestItem
#  define GetLastCreatedDefeatConditionBJ()                           bj_lastCreatedDefeatCondition
#
#endif
