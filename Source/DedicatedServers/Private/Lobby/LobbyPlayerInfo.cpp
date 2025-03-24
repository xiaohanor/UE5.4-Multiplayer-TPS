#include "Lobby/LobbyPlayerInfo.h"

void FLobbyPlayerInfoArray::AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo)
{
	int32 Index = Players.Add(NewPlayerInfo);
	// 标记新添加的玩家信息为“脏”的，以便进行网络复制
	MarkItemDirty(Players[Index]);
	// 调用新添加玩家信息的PostReplicatedAdd函数，通知它已被复制
	Players[Index].PostReplicatedAdd(*this);
}
 
void FLobbyPlayerInfoArray::RemovePlayer(const FString& Username)
{
	for (int32 PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		FLobbyPlayerInfo& PlayerInfo = Players[PlayerIndex];
		// 检查当前玩家的用户名是否与要移除的用户名匹配
		if (PlayerInfo.Username == Username)
		{
			// 调用要移除玩家信息的PreReplicatedRemove函数，通知它即将被移除
			PlayerInfo.PreReplicatedRemove(*this);
			// 从Players数组中移除指定索引的玩家信息，使用RemoveAtSwap可以提高效率，但不保证元素的顺序
			Players.RemoveAtSwap(PlayerIndex);
			// 标记Players数组为“脏”的，以便进行网络复制
			MarkArrayDirty();
			break;
		}
	}
}