function SetEvent(itemindex, charaindex, toindex, haveitemindex)
	if charaindex ~= toindex then
		return ;
	end
	local itemdata = item.getChar(itemindex, "�ֶ�");
	local itemtype = other.atoi(other.getString(itemdata, "|", 1));
	local value = other.atoi(other.getString(itemdata, "|", 2));
	if itemtype == 1 then
		if value == 1 then
			for i = 1, table.getn(evendlist182) do
				npc.ClsEvent(charaindex, evendlist182[i]);
				npc.EvEnd(charaindex, evendlist182[i]);
			end
			char.TalkToCli(charaindex, -1, "��ϲ�����1.82ȫ������", "��ɫ");
			char.DelItem(charaindex,haveitemindex);
		else
			for i=1,table.getn(evendlist182) do 
				--if evendlist182[i] ~= 9 then
					if npc.CheckEvent(charaindex,evendlist182[i]) == 0 then
						char.TalkToCli(charaindex, -1, "��Ǹ������δ�������"..evendExp[i]..evendlist182[i] .."�������ܸ�����ӡ��", "��ɫ");
						return;
					end
				--end
			end
			if char.getInt(charaindex,"����") < 2200 then
				char.TalkToCli(charaindex, -1, "������������22������ʹ�øõ��ߣ�", "��ɫ");
				return;
			end
			char.setInt(charaindex,"����",char.getInt(charaindex,"����") - 2200);
			for i=1,table.getn(evendlist182) do 
				npc.ClsEvent(charaindex,evendlist182[i]);
			end
			npc.ClsEvent(charaindex,9);
			sasql.setGloryPoint(charaindex,sasql.getGloryPoint(charaindex)-13);
			char.TalkToCli(charaindex, -1, "��ϲ��182������ӡ�ɹ������۳�13�������õ�������22������", "��ɫ");
			item.setChar(itemindex, "�ֶ�","1|1");
			item.setChar(itemindex, "����","182�����[�ѷ�ӡ]");
			item.setChar(itemindex, "��ʾ��", "182�����[�ѷ�ӡ]");
			item.setChar(itemindex, "˵��", "�Ѿ���ӡ��1.82����ľ��ᣬʹ�ú󼴿����182����");
			item.UpdataItemOne(charaindex, itemindex);
		end
	elseif itemtype == 2 then
		if value == 1 then
			if char.GetEmptyItemNum(charaindex) >= 4 then
				for i = 1, table.getn(evendlist) do
					for b=1,table.getn(evendlist[i][1]) do 
						npc.ClsEvent(charaindex, evendlist[i][1][b]);
						npc.EvEnd(charaindex, evendlist[i][1][b]);
					end
					npc.AddItem(charaindex, evendlist[i][3]);
				end
				char.TalkToCli(charaindex, -1, "��ϲ�����4������", "��ɫ");
				char.DelItem(charaindex,haveitemindex);
			else
				char.TalkToCli(charaindex, -1, "��Ǹ�����ĵ�������λ����4����", "��ɫ");
			end
		else
			for i = 1, table.getn(evendlist) do
				for b=1,table.getn(evendlist[i][1]) do 
					if npc.CheckEvent(charaindex,evendlist[i][1][b]) == 0 then
						char.TalkToCli(charaindex, -1, "��Ǹ������δ���"..evendlist[i][2].."���ܸ�����ӡ��", "��ɫ");
						return;
					end
				end
				if char.Finditem(charaindex,evendlist[i][3]) == -1 then
					char.TalkToCli(charaindex, -1, "��Ǹ��������û��["..item.getNameFromNumber(evendlist[i][3]).."]���߲��ܸ�����ӡ��", "��ɫ");
					return;
				end
			end
			if char.getInt(charaindex,"����") < 1900 then
				char.TalkToCli(charaindex, -1, "������������19������ʹ�øõ��ߣ�", "��ɫ");
				return;
			end
			char.setInt(charaindex,"����",char.getInt(charaindex,"����") - 1900);
			for i = 1, table.getn(evendlist) do
				for b=1,table.getn(evendlist[i][1]) do 
					npc.ClsEvent(charaindex,evendlist[i][1][b]);
				end
				npc.DelItemNum(charaindex,evendlist[i][3]..",1");
			end
			char.TalkToCli(charaindex, -1, "��ϲ���Ĵ�������ӡ�ɹ����۳�19������", "��ɫ");
			item.setChar(itemindex, "�ֶ�","2|1");
			item.setChar(itemindex, "����","4�������[�ѷ�ӡ]");
			item.setChar(itemindex, "��ʾ��", "4�������[�ѷ�ӡ]");
			item.setChar(itemindex, "˵��", "�Ѿ���ӡ��4������ľ��ᣬʹ�ú󼴿����4������");
			item.UpdataItemOne(charaindex, itemindex);
		end
	elseif itemtype == 3 then
		if char.GetEmptyPetNum(charaindex) < 1 then
			char.TalkToCli(charaindex, -1, "��Ǹ�����ĵ�������λ����1����", "��ɫ");
			return;
		end
		local petindex = char.AddPet( charaindex, 351,1);
		npc.ClsEvent(charaindex, 47);
		npc.EvEnd(charaindex, 47);
		char.DelItem(charaindex,haveitemindex);
		char.TalkToCli(charaindex, -1, "��ϲ���������ֵ�����", "��ɫ");
	end
end



function mydata()
	evendlist182 = {1,2,3,4,5,7,8,10,11,12,13,15,16,17,18,19,20,21,22,27,28,29,30,31,32,33,34,35,37,38,44,45,47,54};
	evendlist={{{39}, "��������", 2701}
			  ,{{40}, "��춴��", 2707}
			  ,{{41,42}, "���ƶ���", 2735}
			  ,{{46}, "���ප��", 2770}
			  };	
	evendExp= {"��ķ��ķ���ҩ��",--1
				"����˼��ı���",--2
				"��ķ��ķ�ĸ�ͷ  �͸�ͷ",--3
				"������ʽ",--4
				"��ķ��ķ�ĸ�ͷ  ����",--5
				"����ɽ����  �ѻ����Ÿ�������",--7
				"����ɽ����  ���Ÿ�1¥����",--8
				"",--10
				"",--11
				"",--12
				"ǿ������",--13
				"������ʿ ץ³����˹",--15
				"������ʿ ץ��³��|",--16
				"��������",--17
				"��ľ����",--18
				"",--19
				"ǿ������  �ҵ�ǿ��",--20
				"ǿ������  �ҵ����µ�Ů��",--21
				"ǿ������  �Ѽ��������",--22
				"����֮·",--27
				"���ֵ�֮��   �õ��ǻ�ˮ��",--28
				"�ƽ�����֮·  �õ��ػ�������·�",--29
				"�ƽ�����֮·  �õ�����֮��",--30
				"���ֵ�֮��   �õ�����",--31
				"�λö���  ������",--32
				"�λö���  ��ñ��",--33
				"�λö���  ���·�",--34
				"��̹  ��4ֻ����",--35
				"��������",--37
				"ˮ���Թ�",--38
				"����3�ֵ�",--44
				"�ı���֮��",--45
				"����3�ֵ�  ������",--47
				"�ƽ�����֮·  �õ��ƽ�����"--54
				};
						
end

function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "�ض�" then
		mydata();
		char.TalkToCli(_charaindex, -1, "�ض�NPC[�����ǩ]���", 6);
	end
end

function main()
	mydata();
	item.addLUAListFunction( "ITEM_SetEvent", "SetEvent", "");
	magic.addLUAListFunction("ReSetEvent", "reload", "", 3, "[ReSetEvent]");
end
