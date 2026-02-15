class ItemPDA : ItemBase
{	
	override void EEInit()
	{
		super.EEInit();
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "EEInit.");
		UpdateVisualStyle();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpenPDA);
		if (GearPDADebugMode) Print(GearPDAModPreffix + "SetActions.");
	}
	
	override void OnIsPlugged(EntityAI source_device)
	{
		super.OnIsPlugged(source_device);
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnIsPlugged.");
		SetVisualStyle(1);
	}
	
	override void OnIsUnplugged(EntityAI last_energy_source)
	{
		super.OnIsUnplugged(last_energy_source);
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnIsUnplugged.");
		SetVisualStyle(0);
	}
	
	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnWork.");
		UpdateVisualStyle();
	}
	
	override void OnWorkStart() 
	{
		super.OnWorkStart();
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnWorkStart.");
		UpdateVisualStyle();
	}
	
	override void OnWorkStop() 
	{
		super.OnWorkStop();
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnWorkStop.");
		UpdateVisualStyle();
	}
	
	override void OnInitEnergy() 
	{
		super.OnInitEnergy();
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnInitEnergy.");
		UpdateVisualStyle();
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "OnVariablesSynchronized.");
		UpdateVisualStyle();
	}
	
	void UpdateVisualStyle()
	{
		if (GetGame().IsClient())
		{
			int texIndex = 0;
			if (!IsRuined() && HasEnergyManager() && GetCompEM().CanWork())
			{
				texIndex = 1;
			}
			
			SetVisualStyle(texIndex);
		}
	}
	
	void SetVisualStyle(int id)
	{
		TStringArray textures = GetHiddenSelectionsTextures();
		string texture = textures.Get(id);
		SetObjectTexture(0, texture);
		if (GearPDADebugMode) Print(GearPDAModPreffix + "Change PDA texture to " + id + " =>" + texture);
	}
};