// Dedicated-server workaround for SCR_AmbientPatrolSystem.
//
// On the affected scenario the native world-system update runs once before a
// player exists and is not scheduled again after the first player joins. This
// fallback only restores periodic calls to the vanilla ProcessSpawnpoint method.
// All vanilla distance, probability, catalog and AI-limit rules remain intact.

modded class SCR_AmbientPatrolSystem
{
	protected int m_iKdKNativeUpdateCount;
	protected int m_iKdKFallbackIndex;
	protected bool m_bKdKFallbackActive;

	override void OnInit()
	{
		super.OnInit();
		if (Replication.IsServer())
			GetGame().GetCallqueue().CallLater(KdK_Monitor, 1000, true);
	}

	protected void KdK_Monitor()
	{
		KdK_TryStartFallback();
	}

	protected void KdK_StopFallback()
	{
		if (!m_bKdKFallbackActive)
			return;

		m_bKdKFallbackActive = false;
		GetGame().GetCallqueue().Remove(KdK_FallbackTick);
	}

	protected void KdK_TryStartFallback()
	{
		if (!Replication.IsServer() || m_bKdKFallbackActive)
			return;

		// Current Reforger no longer keeps a player array on this system.
		// ProcessSpawnpoint queries ObserversSystem itself, so patrol presence is
		// the only prerequisite needed here.
		if (m_aPatrols.IsEmpty())
			return;

		// A healthy native scheduler calls OnUpdatePoint continuously. The broken
		// dedicated-server path produces no more than its initial startup call.
		if (m_iKdKNativeUpdateCount > 1)
			return;

		m_bKdKFallbackActive = true;
		m_iKdKFallbackIndex = 0;
		GetGame().GetCallqueue().CallLater(KdK_FallbackTick, 20, true);
	}

	protected void KdK_FallbackTick()
	{
		if (!Replication.IsServer())
		{
			KdK_StopFallback();
			return;
		}

		if (m_aPatrols.IsEmpty())
			return;

		if (m_iKdKFallbackIndex >= m_aPatrols.Count())
			m_iKdKFallbackIndex = 0;

		ProcessSpawnpoint(m_iKdKFallbackIndex);
		m_iKdKFallbackIndex++;
	}

	override void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		m_iKdKNativeUpdateCount++;

		// Never run the native scheduler and fallback in parallel.
		if (m_bKdKFallbackActive)
			KdK_StopFallback();

		super.OnUpdatePoint(args);
	}

	override void OnCleanup()
	{
		GetGame().GetCallqueue().Remove(KdK_Monitor);
		KdK_StopFallback();
		super.OnCleanup();
	}
}

// Ambient patrol groups use a formation around the spawn-point origin. On
// dense jungle terrain individual formation slots can end up inside rocks even
// when the spawn-point entity itself is placed on open ground. Correct only
// freshly spawned members which are not standing on the soldiers navmesh.
modded class SCR_AmbientPatrolSpawnPointComponent
{
	override void SpawnPatrol()
	{
		super.SpawnPatrol();

		if (!Replication.IsServer())
			return;

		// Group creation and agent registration are not guaranteed to finish in
		// the same frame. The second pass catches slower dedicated-server spawns.
		GetGame().GetCallqueue().CallLater(KdK_ValidateSpawnedMembers, 300, false);
		GetGame().GetCallqueue().CallLater(KdK_ValidateSpawnedMembers, 1200, false);
	}

	protected void KdK_ValidateSpawnedMembers()
	{
		if (!Replication.IsServer())
			return;

		SCR_AIGroup group = GetSpawnedGroup();
		if (!group)
			return;

		AIPathfindingComponent pathfinding = AIPathfindingComponent.Cast(
			group.FindComponent(AIPathfindingComponent));
		if (!pathfinding)
			return;

		array<AIAgent> agents = {};
		group.GetAgents(agents);

		foreach (AIAgent agent : agents)
		{
			if (!agent)
				continue;

			IEntity character = agent.GetControlledEntity();
			if (!character)
				continue;

			vector currentPosition = character.GetOrigin();
			vector correctedPosition;
			if (!pathfinding.GetClosestPositionOnNavmesh(currentPosition, "8 4 8", correctedPosition))
				continue;

			float correctionDistance = vector.Distance(currentPosition, correctedPosition);
			if (correctionDistance < 0.25)
				continue;

			// Keep the character capsule slightly above the calculated surface so
			// physics can settle it without clipping it back into geometry.
			correctedPosition[1] = correctedPosition[1] + 0.15;
			character.SetOrigin(correctedPosition);

			PrintFormat(
				"KDK_AMBIENT_SPAWN_SAFETY moved=%1 from=%2 to=%3 distance=%4 spawnpoint=%5",
				character,
				currentPosition,
				correctedPosition,
				correctionDistance,
				GetOwner().GetOrigin());
		}
	}
}
