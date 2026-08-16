// Temporary diagnostics for the merged US vehicle catalog.
// Search script logs for: KDK_VEHICLE_CATALOG_DEBUG

modded class SCR_GameModeCampaign
{
	override void OnGameStart()
	{
		super.OnGameStart();
		GetGame().GetCallqueue().CallLater(KdK_DumpUSVehicleCatalog, 3000, false);
	}

	protected void KdK_DumpUSVehicleCatalog()
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
		{
			Print("KDK_VEHICLE_CATALOG_DEBUG no FactionManager");
			return;
		}

		SCR_Faction faction = SCR_Faction.Cast(factionManager.GetFactionByKey("US"));
		if (!faction)
		{
			Print("KDK_VEHICLE_CATALOG_DEBUG no US faction");
			return;
		}

		SCR_EntityCatalog catalog = faction.GetFactionEntityCatalogOfType(EEntityCatalogType.VEHICLE);
		if (!catalog)
		{
			Print("KDK_VEHICLE_CATALOG_DEBUG no US VEHICLE catalog");
			return;
		}

		array<SCR_EntityCatalogEntry> entries = {};
		catalog.GetEntityList(entries);
		PrintFormat("KDK_VEHICLE_CATALOG_DEBUG BEGIN enabledEntries=%1", entries.Count());

		foreach (SCR_EntityCatalogEntry entry : entries)
		{
			ResourceName prefab = entry.GetPrefab();
			if (!prefab.Contains("M923A1"))
				continue;

			SCR_EntityCatalogSpawnerData spawnerData = SCR_EntityCatalogSpawnerData.Cast(entry.GetEntityDataOfType(SCR_EntityCatalogSpawnerData));
			if (!spawnerData)
			{
				PrintFormat("KDK_VEHICLE_CATALOG_DEBUG prefab=%1 index=%2 entryEnabled=%3 spawnerData=NULL uiInfo=%4", prefab, entry.GetCatalogIndex(), entry.IsEnabled(), entry.GetEntityUiInfo());
				continue;
			}

			PrintFormat("KDK_VEHICLE_CATALOG_DEBUG prefab=%1 index=%2 entryEnabled=%3 dataEnabled=%4 slots=%5", prefab, entry.GetCatalogIndex(), entry.IsEnabled(), spawnerData.IsEnabled(), spawnerData.GetValidSlotSizes());
			PrintFormat("KDK_VEHICLE_CATALOG_DEBUG details prefab=%1 cost=%2 rank=%3 defaultPrefab=%4 uiInfo=%5", prefab, spawnerData.GetSupplyCost(), spawnerData.GetMinimumRequiredRank(), spawnerData.GetRandomDefaultOrVariantPrefab(), entry.GetEntityUiInfo());
			PrintFormat("KDK_VEHICLE_CATALOG_DEBUG extra prefab=%1 entityCount=%2 overwriteName=%3", prefab, spawnerData.GetEntityCount(), spawnerData.GetOverwriteName());
			SCR_EntityCatalogSpawnerOverrideData overrideData = SCR_EntityCatalogSpawnerOverrideData.Cast(entry.GetEntityDataOfType(SCR_EntityCatalogSpawnerOverrideData));
			SCR_EntityCatalogEditorData editorData = SCR_EntityCatalogEditorData.Cast(entry.GetEntityDataOfType(SCR_EntityCatalogEditorData));
			PrintFormat("KDK_VEHICLE_CATALOG_DEBUG classes prefab=%1 overrideData=%2 editorData=%3", prefab, overrideData, editorData);
			if (editorData)
				PrintFormat("KDK_VEHICLE_CATALOG_DEBUG editor prefab=%1 validEditorModes=%2", prefab, editorData.GetValidEditorModes());
		}

		Print("KDK_VEHICLE_CATALOG_DEBUG END");
	}
}
