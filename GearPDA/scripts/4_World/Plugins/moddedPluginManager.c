modded class PluginManager
{
	override void Init()
	{
		if (GetGame().IsClient())
		{
			RegisterPlugin("PluginGearPDA", true, false);
		}
		
		RegisterPlugin("PluginLogicPDA", true, true);

		super.Init();
	}
}