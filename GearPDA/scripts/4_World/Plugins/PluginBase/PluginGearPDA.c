class PluginGearPDA extends PluginBase
{
	string m_configDir;
	
	ref GearPDAMenu m_GearPDAMenu;
	
	ref array<ref PluginGearPDA_Conversation> m_contacts;
	
	ref array<string> m_onlineContacts;
	
	ref map<string, ref array<ref PluginGearPDA_Comment>> m_comments;
	
	string m_steamId;
	
	const string contactsJsonFilename = "contacts.json";
	
	const string optionsJsonFilename = "options.json";
	
	ref PluginGearPDA_Options m_options;
    
    bool m_enableGlobalChat = false;
    
    bool m_enableGlobalChatSound = false;
    
    ref array<ref Param2<string, string>> m_globalMessages;
    
    int m_globalChatUnreaded = 0;
	
	void PluginGearPDA()
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "PluginGearPDA construct.");
		m_contacts = new array<ref PluginGearPDA_Conversation>();
		m_comments = new map<string, ref array<ref PluginGearPDA_Comment>>();
        m_globalMessages = new array<ref Param2<string, string>>;
		m_onlineContacts = new array<string>();
		m_options = new PluginGearPDA_Options();
		m_configDir = "$profile:\\GearPDA\\";
	}
	
	void ~PluginGearPDA()
	{
		SaveOptionsConfig();
		if (GearPDADebugMode) Print(GearPDAModPreffix + "PluginGearPDA destruct.");
	}
	
	override void OnInit()
	{
		MakeDirectory(m_configDir);
		
		if (!FileExist(m_configDir + contactsJsonFilename)) {
			SaveOptionsConfig();
            SaveContactsConfig();
        } else {
			LoadOptionsConfig();
            LoadContactsConfig();
        }
	}
	
	void SaveOptionsConfig()
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "SaveOptionsConfig.");
		JsonFileLoader<ref PluginGearPDA_Options>.JsonSaveFile(m_configDir + optionsJsonFilename, m_options);
	}
	
	void LoadOptionsConfig()
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "LoadOptionsConfig.");
		JsonFileLoader<ref PluginGearPDA_Options>.JsonLoadFile(m_configDir + optionsJsonFilename, m_options);
	}
	
	void SaveContactsConfig()
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "SaveContactsConfig.");
		JsonFileLoader<ref array<ref PluginGearPDA_Conversation>>.JsonSaveFile(m_configDir + contactsJsonFilename, m_contacts);
	}
	
	void LoadContactsConfig()
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "LoadContactsConfig.");
		JsonFileLoader<ref array<ref PluginGearPDA_Conversation>>.JsonLoadFile(m_configDir + contactsJsonFilename, m_contacts);
	}
	
	void SaveCommentsConfig(string uid)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "SaveCommentsConfig: " + uid);
		
		if (m_comments.Contains(uid))
		{
			ref array<ref PluginGearPDA_Comment> comments = m_comments[uid];
			
			while (comments.Count() > 100)
			{
				comments.RemoveOrdered(0);
			}
			
			JsonFileLoader<ref array<ref PluginGearPDA_Comment>>.JsonSaveFile(m_configDir + uid + ".json", comments);
		}
	}
	
	void LoadCommentsConfig(string uid)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "LoadCommentsConfig: " + uid);
		
		string fileName = m_configDir + uid + ".json";
		if (FileExist(fileName))
		{
			ref array<ref PluginGearPDA_Comment> result = new array<ref PluginGearPDA_Comment>(); 
			JsonFileLoader<ref array<ref PluginGearPDA_Comment>>.JsonLoadFile(fileName, result);
			m_comments[uid] = result;
		}
	}
	
	ref array<ref PluginGearPDA_Comment> GetComments(string uid)
	{
		if (!m_comments.Contains(uid))
		{
			LoadCommentsConfig(uid);
		}

		if (!m_comments.Contains(uid))
		{
			m_comments[uid] = new array<ref PluginGearPDA_Comment>();
		}
		
		return m_comments[uid];
	}
	
	void AddComment(string contactId, string senderId, string message)
	{
		if (contactId.Length() > 0)
		{
			ref array<ref PluginGearPDA_Comment> comments = GetComments(contactId);
			ref PluginGearPDA_Comment comment = new PluginGearPDA_Comment();
			comment.m_UID = senderId;
			comment.m_Message = message;
			comments.Insert(comment);
			
			SaveCommentsConfig(contactId);
			
			ref PluginGearPDA_Conversation contact = FindContact(contactId);
			if (contact != null)
			{
				contact.m_Unreaded = contact.m_Unreaded + 1;
				SaveContactsConfig();
			}
			
			if (IsOpen())
			{
				m_GearPDAMenu.m_sendMessageStatus = 2;
			}
		}
		else
		{
			if (IsOpen())
			{
				m_GearPDAMenu.m_sendMessageStatus = 0;
			}
				
			ref array<string> request = new array<string>();
			for (int i = 0; i < m_contacts.Count(); i++)
			{
				request.Insert(m_contacts[i].m_UID);
			}
			GetRPCManager().SendRPC( GearPDAModPreffix, "CheckContacts", new Param1< ref array<string> >( request ), true );
		}
	}
	
	bool IsOpen()
	{
		return m_GearPDAMenu && m_GearPDAMenu.m_active;
	}
	
	void Open()
	{
		if (IsOpen())
		{
			Close();
		}
		
		if (GetGame().GetUIManager().GetMenu() != NULL)
		{
			if (GearPDADebugMode) Print(GearPDAModPreffix + "OpenRecipesBookAction ActionCondition blocking by external menu: " + GetGame().GetUIManager().GetMenu());
			return;
		}
		
		if (GearPDADebugMode) Print(GearPDAModPreffix + "PluginGearPDA prepare open menu");
		m_GearPDAMenu = new GearPDAMenu;
		m_GearPDAMenu.Init();
		GetGame().GetUIManager().ShowScriptedMenu( m_GearPDAMenu, NULL );
		if (GearPDADebugMode) Print(GearPDAModPreffix + "PluginGearPDA post open menu: " + m_GearPDAMenu);
	}
	
	ref PluginGearPDA_Conversation FindContact(string uid)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "FindContact: " + uid);
		
		for (int i = 0; i < m_contacts.Count(); i++)
		{
			ref PluginGearPDA_Conversation item = m_contacts[i];
			if (item.m_UID == uid)
			{
				return item;
			}
		}
		
		return null;
	}
	
	void AddContact(string uid, string name)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "AddContact X1: " + uid + "; " + name);
		if (uid.Length() > 0 && name.Length() > 0)
		{
			if (GearPDADebugMode) Print(GearPDAModPreffix + "AddContact X2: " + uid + "; " + name);
			if (FindContact(uid) == null)
			{
				if (GearPDADebugMode) Print(GearPDAModPreffix + "AddContact X3: " + uid + "; " + name);
				ref PluginGearPDA_Conversation conv = new PluginGearPDA_Conversation();
				conv.m_UID = uid;
				conv.m_Name = name;
				conv.m_Unreaded = 0;
				conv.m_IsBanned = false;
				m_contacts.Insert(conv);
				m_onlineContacts.Insert(uid);
				SaveContactsConfig();
			}
		}
		
		if (IsOpen())
		{
			m_GearPDAMenu.m_addContactStatus = 2;
		}
	}
	
	void RemoveContact(string uid)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "RemoveContact: " + uid);
		
		ref PluginGearPDA_Conversation contactToDelete = FindContact(uid);
		if (contactToDelete == null)
		{
			return;
		}
		
		m_contacts.RemoveItem(contactToDelete);
		SaveContactsConfig();
		
		if (m_comments.Contains(uid))
		{
			m_comments.Remove(uid);
		}
		
		string fileName = m_configDir + uid + ".json";
		if (FileExist(fileName))
		{
			DeleteFile(fileName);
		}
	}
	
	void RenameContact(string uid, string newName)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "RenameContact: " + uid);
		
		ref PluginGearPDA_Conversation contactToRename = FindContact(uid);
		if (contactToRename == null)
		{
			return;
		}
		
		contactToRename.m_Name = newName;
		SaveContactsConfig();
	}
	
	void BanUnbanContact(string uid)
	{
		if (GearPDADebugMode) Print(GearPDAModPreffix + "BanContact: " + uid);
		
		ref PluginGearPDA_Conversation contactToRename = FindContact(uid);
		if (contactToRename == null)
		{
			return;
		}
		
		contactToRename.m_IsBanned = !contactToRename.m_IsBanned;
		SaveContactsConfig();
	}
	
	void Close()
	{
		if (m_GearPDAMenu)
		{
			m_GearPDAMenu.m_active = false;
		}

		if (GearPDADebugMode) Print(GearPDAModPreffix + "PluginGearPDA close menu: " + m_GearPDAMenu);
	}
};

class PluginGearPDA_Conversation
{
	string m_UID;
	string m_Name;
	int m_Unreaded;
	bool m_IsBanned;

	void PluginGearPDA_Conversation()
	{
	
	}
};

class PluginGearPDA_Comment
{
	string m_UID;
	string m_Message;
	
	void PluginGearPDA_Comment()
	{
	
	}
};

class PluginGearPDA_Options
{
	bool m_Muted;
	bool m_HideId;
	
	void PluginGearPDA_Options()
	{
	
	}
};