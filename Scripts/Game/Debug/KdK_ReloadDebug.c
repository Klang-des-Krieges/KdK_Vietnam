// Makes storages attached to cloth-node items (for example ALICE pouches)
// visible to Reforger's native character inventory and reload systems.

modded class SCR_InventoryStorageManagerComponent
{
	protected bool m_bKdKNestedStorageRegistrationScheduled;
	protected int m_iFPM_MedicalKitCount;
	protected int m_iFPM_WrenchCount;

	override void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemAdded(storageOwner, item);
		FPM_UpdateSupportRole(item, 1);
	}

	override void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemRemoved(storageOwner, item);
		FPM_UpdateSupportRole(item, -1);
	}

	protected void FPM_UpdateSupportRole(IEntity item, int delta)
	{
		if (!Replication.IsServer() || !item)
			return;

		SCR_ChimeraCharacter ownerCharacter = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerCharacter)
			return;

		SCR_BaseSupportStationComponent supportComponent = SCR_BaseSupportStationComponent.Cast(
			item.FindComponent(SCR_BaseSupportStationComponent));
		if (!supportComponent)
			return;

		if (SCR_HealSupportStationComponent.Cast(supportComponent))
		{
			m_iFPM_MedicalKitCount += delta;
			if (m_iFPM_MedicalKitCount < 0)
				m_iFPM_MedicalKitCount = 0;
		}
		else if (SCR_RepairSupportStationComponent.Cast(supportComponent))
		{
			m_iFPM_WrenchCount += delta;
			if (m_iFPM_WrenchCount < 0)
				m_iFPM_WrenchCount = 0;
		}
		else
			return;

		ownerCharacter.FPM_UpdateRoleSymbolIcons();
	}

	int FPM_GetMedicalKitCount()
	{
		return m_iFPM_MedicalKitCount;
	}

	int FPM_GetWrenchCount()
	{
		return m_iFPM_WrenchCount;
	}

	override void OnStorageAdded(BaseInventoryStorageComponent storage)
	{
		super.OnStorageAdded(storage);

		if (!ClothNodeStorageComponent.Cast(storage) || m_bKdKNestedStorageRegistrationScheduled)
			return;

		m_bKdKNestedStorageRegistrationScheduled = true;
		GetGame().GetCallqueue().CallLater(KdK_RegisterNestedClothStorages, 250, false);
	}

	protected void KdK_RegisterNestedClothStorages()
	{
		m_bKdKNestedStorageRegistrationScheduled = false;

		array<BaseInventoryStorageComponent> registeredStorages = {};
		GetStorages(registeredStorages, EStoragePurpose.PURPOSE_ANY);

		array<BaseInventoryStorageComponent> storagesToRegister = {};
		foreach (BaseInventoryStorageComponent rootStorage : registeredStorages)
		{
			if (!ClothNodeStorageComponent.Cast(rootStorage))
				continue;

			array<BaseInventoryStorageComponent> nestedStorages = {};
			rootStorage.GetOwnedStorages(nestedStorages, 8, true);
			foreach (BaseInventoryStorageComponent nestedStorage : nestedStorages)
			{
				if (nestedStorage && nestedStorage != rootStorage)
					KdK_QueueStorageRegistration(nestedStorage, registeredStorages, storagesToRegister);
			}

			// ALICE-style rigs expose pouches as cloth-node items rather than as
			// owned storage hierarchy, so inspect the pouch entities as well.
			array<IEntity> clothNodeItems = {};
			rootStorage.GetAll(clothNodeItems, true);
			foreach (IEntity clothNodeItem : clothNodeItems)
			{
				BaseInventoryStorageComponent pouchStorage = BaseInventoryStorageComponent.Cast(
					clothNodeItem.FindComponent(BaseInventoryStorageComponent));
				if (pouchStorage && pouchStorage != rootStorage)
					KdK_QueueStorageRegistration(pouchStorage, registeredStorages, storagesToRegister);
			}
		}

		foreach (BaseInventoryStorageComponent storageToRegister : storagesToRegister)
			AddStorage(storageToRegister);
	}

	protected void KdK_QueueStorageRegistration(
		BaseInventoryStorageComponent storage,
		array<BaseInventoryStorageComponent> registeredStorages,
		array<BaseInventoryStorageComponent> storagesToRegister)
	{
		if (registeredStorages.Find(storage) != -1 || storagesToRegister.Find(storage) != -1)
			return;

		storagesToRegister.Insert(storage);
	}

}
