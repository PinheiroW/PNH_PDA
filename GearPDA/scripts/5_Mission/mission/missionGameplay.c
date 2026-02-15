modded class MissionGameplay
{	
	override void OnInit()
  	{
		super.OnInit();
	}
	
	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		
		PluginGearPDA pluginGearPDA;
		if ( key == KeyCode.KC_ESCAPE )
		{	
			Class.CastTo(pluginGearPDA, GetPlugin(PluginGearPDA));
			if (pluginGearPDA && pluginGearPDA.IsOpen())
			{
				pluginGearPDA.Close();
			}
		}
		else if ( key == KeyCode.KC_RETURN )
		{
			Class.CastTo(pluginGearPDA, GetPlugin(PluginGearPDA));
			if (pluginGearPDA && pluginGearPDA.IsOpen())
			{
				pluginGearPDA.m_GearPDAMenu.m_externalSendEvent = true;
			}
		}
	}
}