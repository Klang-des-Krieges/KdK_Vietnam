// Dedicated-server workaround for SCR_AmbientPatrolSystem.
//
// On the affected scenario the native world-system update can stop after its
// initial startup call. In that case no spawn point is processed at all. This
// fallback restores the vanilla ProcessSpawnpoint loop without bypassing its
// distance, probability, catalog or AI-budget checks.
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

	protected void KdK_TryStartFallback()
	{
		if (!Replication.IsServer() || m_bKdKFallbackActive)
			return;

		if (m_aPatrols.IsEmpty())
			return;

		// A healthy native scheduler invokes OnUpdatePoint continuously. Do not
		// start a second loop when that scheduler is operating normally.
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

	protected void KdK_StopFallback()
	{
		if (!m_bKdKFallbackActive)
			return;

		m_bKdKFallbackActive = false;
		GetGame().GetCallqueue().Remove(KdK_FallbackTick);
	}

	override void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		m_iKdKNativeUpdateCount++;

		// Never process patrols from native and fallback schedulers in parallel.
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

modded class SCR_AmbientPatrolSpawnPointComponent
{
	private ref RandomGenerator random = new RandomGenerator();
	
	override void SetspawnedGroup(SCR_AIGroup group){
		super.SetspawnedGroup(group);
		
		if(!group)
			return;

		if (!Replication.IsServer())
			return;
		
		group.GetOnAgentAdded().Insert(OnPatrolAgentAdded);
	}
	
	protected void OnPatrolAgentAdded(AIAgent child){
		GetGame().GetCallqueue().CallLater(KdK_ValidateSpawnedMembers, 0, false, child);
	}
	
	private TraceParam KdK_RaycastUpFromPlayer(vector currentPosition, BaseWorld world){
		TraceParam trace = new TraceParam();
		trace.Start = currentPosition + vector.Up * 2;  //currentPosition is the root of the entity, which is at the bottom so the raycast would hit the entity itself
		trace.End = currentPosition + vector.Up * 100;
		trace.Flags = TraceFlags.ENTS;
		
		world.TraceMove(trace, null);
		
		return trace;
	}
	
	protected void KdK_ValidateSpawnedMembers(AIAgent agent)
	{
		if (!agent)
			return;
		
		IEntity character = agent.GetControlledEntity();
		if (!character)
			return;

		vector currentPosition = character.GetOrigin();
		vector correctedPosition;
		
		TraceParam trace = KdK_RaycastUpFromPlayer(currentPosition, character.GetWorld());
		if(!trace.TraceEnt)
			return;
		
		int maxTries = 1000;
		int currentTries = 0;
			
		while(currentTries <= maxTries){
			float newX = random.RandInt(currentPosition[0] - 25, currentPosition[0] + 25);
			float newZ = random.RandInt(currentPosition[2] - 25, currentPosition[2] + 25);
			float newY = character.GetWorld().GetSurfaceY(newX, newZ);
			
			correctedPosition = Vector(newX, newY, newZ);
			trace = KdK_RaycastUpFromPlayer(correctedPosition, character.GetWorld());
			if(!trace.TraceEnt){
				PrintFormat(
					"KDK_AMBIENT_SPAWN_SAFETY moved=%1 from=%2 to=%3 tries=%4 distance=%5",
					character,
					currentPosition,
					correctedPosition,
					currentTries,
					Math.AbsFloat(vector.Distance(correctedPosition, currentPosition)));
				
				
				character.SetOrigin(correctedPosition);
				return;
			}
			
			currentTries++;
		}
		
		PrintFormat("KDK_AMBIENT_SPAWN_SAFETY could not move %1 after %2 tries", character, currentTries);
	}
}
