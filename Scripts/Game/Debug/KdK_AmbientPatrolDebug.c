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
