// Disable Game Master budgets automatically when the server starts.
// The dependency handles replication and all individual budget checks.
modded class SCR_BaseGameMode
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!Replication.IsServer())
			return;

		DisableGMBudget_SetBudgetsEnabled(false);
	}
}
