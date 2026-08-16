// KdK extension for the TeamBalancer mod.
// Disables a faction button if joining it would exceed the configured
// population-dependent team-size difference.

modded class SCR_FactionButton
{
	static bool TeamBalancer_IsActive()
	{
		return TeamBalancer_GetTotalPlayablePlayers() >= 5;
	}

	static int TeamBalancer_GetTotalPlayablePlayers()
	{
		array<Faction> factions = {};
		int count = GetGame().GetFactionManager().GetFactionsList(factions);

		int total = 0;
		for (int i = 0; i < count; i++)
		{
			SCR_Faction faction = SCR_Faction.Cast(factions[i]);
			if (!faction || !faction.IsPlayable())
				continue;

			total += faction.GetPlayerCount();
		}

		return total;
	}

	static int TeamBalancer_GetMaxDiff()
	{
		int total = TeamBalancer_GetTotalPlayablePlayers();

		if (total < 5)
			return 999;

		if (total < 30)
			return 2;

		if (total < 40)
			return 3;

		return 4;
	}

	static bool TeamBalancer_WouldJoinBreakBalance(SCR_Faction faction)
	{
		if (!faction || !TeamBalancer_IsActive())
			return false;

		array<Faction> factions = {};
		int count = GetGame().GetFactionManager().GetFactionsList(factions);
		if (count < 2)
			return false;

		SCR_Faction otherFaction;
		for (int i = 0; i < count; i++)
		{
			SCR_Faction candidate = SCR_Faction.Cast(factions[i]);
			if (!candidate || !candidate.IsPlayable())
				continue;

			if (candidate.GetFactionKey() == faction.GetFactionKey())
				continue;

			otherFaction = candidate;
			break;
		}

		if (!otherFaction)
			return false;

		int prospectiveCount = faction.GetPlayerCount() + 1;
		int otherCount = otherFaction.GetPlayerCount();
		int difference = prospectiveCount - otherCount;
		int maximumDifference = TeamBalancer_GetMaxDiff();

		if (difference <= maximumDifference)
			return false;

		PrintFormat(
			"KDK_TEAM_BALANCER blocked faction=%1 prospective=%2 otherFaction=%3 otherCount=%4 difference=%5 maximum=%6 total=%7",
			faction.GetFactionKey(),
			prospectiveCount,
			otherFaction.GetFactionKey(),
			otherCount,
			difference,
			maximumDifference,
			TeamBalancer_GetTotalPlayablePlayers());

		return true;
	}

	override void UpdatePlayerCount()
	{
		SCR_Faction faction = SCR_Faction.Cast(m_Faction);
		if (!faction || !m_wPlayerCount)
			return;

		m_iPlayerCount = faction.GetPlayerCount();
		m_wPlayerCount.SetText(m_iPlayerCount.ToString());
		SetEnabled(!TeamBalancer_WouldJoinBreakBalance(faction));
	}
}
