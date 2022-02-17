function SetEvent(itemindex, charaindex, toindex, haveitemindex)
	if charaindex ~= toindex then
		return ;
	end
	local itemdata = item.getChar(itemindex, "字段");
	local itemtype = other.atoi(other.getString(itemdata, "|", 1));
	local value = other.atoi(other.getString(itemdata, "|", 2));
	if itemtype == 1 then
		if value == 1 then
			for i = 1, table.getn(evendlist182) do
				npc.ClsEvent(charaindex, evendlist182[i]);
				npc.EvEnd(charaindex, evendlist182[i]);
			end
			char.TalkToCli(charaindex, -1, "恭喜你完成1.82全部任务！", "黄色");
			char.DelItem(charaindex,haveitemindex);
		else
			for i=1,table.getn(evendlist182) do 
				--if evendlist182[i] ~= 9 then
					if npc.CheckEvent(charaindex,evendlist182[i]) == 0 then
						char.TalkToCli(charaindex, -1, "抱歉！您尚未完成任务【"..evendExp[i]..evendlist182[i] .."】，不能给您封印！", "黄色");
						return;
					end
				--end
			end
			if char.getInt(charaindex,"声望") < 2200 then
				char.TalkToCli(charaindex, -1, "您的声望不足22，不能使用该道具！", "黄色");
				return;
			end
			char.setInt(charaindex,"声望",char.getInt(charaindex,"声望") - 2200);
			for i=1,table.getn(evendlist182) do 
				npc.ClsEvent(charaindex,evendlist182[i]);
			end
			npc.ClsEvent(charaindex,9);
			sasql.setGloryPoint(charaindex,sasql.getGloryPoint(charaindex)-13);
			char.TalkToCli(charaindex, -1, "恭喜您182任务卷封印成功！并扣除13点任务获得的名声和22点声望", "黄色");
			item.setChar(itemindex, "字段","1|1");
			item.setChar(itemindex, "名称","182任务卷[已封印]");
			item.setChar(itemindex, "显示名", "182任务卷[已封印]");
			item.setChar(itemindex, "说明", "已经封印了1.82任务的卷轴，使用后即可完成182任务");
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
				char.TalkToCli(charaindex, -1, "恭喜你完成4大任务！", "黄色");
				char.DelItem(charaindex,haveitemindex);
			else
				char.TalkToCli(charaindex, -1, "抱歉！您的道具栏空位不足4个！", "黄色");
			end
		else
			for i = 1, table.getn(evendlist) do
				for b=1,table.getn(evendlist[i][1]) do 
					if npc.CheckEvent(charaindex,evendlist[i][1][b]) == 0 then
						char.TalkToCli(charaindex, -1, "抱歉！您尚未完成"..evendlist[i][2].."不能给您封印！", "黄色");
						return;
					end
				end
				if char.Finditem(charaindex,evendlist[i][3]) == -1 then
					char.TalkToCli(charaindex, -1, "抱歉！您身上没有["..item.getNameFromNumber(evendlist[i][3]).."]道具不能给您封印！", "黄色");
					return;
				end
			end
			if char.getInt(charaindex,"声望") < 1900 then
				char.TalkToCli(charaindex, -1, "您的声望不足19，不能使用该道具！", "黄色");
				return;
			end
			char.setInt(charaindex,"声望",char.getInt(charaindex,"声望") - 1900);
			for i = 1, table.getn(evendlist) do
				for b=1,table.getn(evendlist[i][1]) do 
					npc.ClsEvent(charaindex,evendlist[i][1][b]);
				end
				npc.DelItemNum(charaindex,evendlist[i][3]..",1");
			end
			char.TalkToCli(charaindex, -1, "恭喜您四大任务卷封印成功！扣除19点声望", "黄色");
			item.setChar(itemindex, "字段","2|1");
			item.setChar(itemindex, "名称","4大任务卷[已封印]");
			item.setChar(itemindex, "显示名", "4大任务卷[已封印]");
			item.setChar(itemindex, "说明", "已经封印了4大洞任务的卷轴，使用后即可完成4大任务");
			item.UpdataItemOne(charaindex, itemindex);
		end
	elseif itemtype == 3 then
		if char.GetEmptyPetNum(charaindex) < 1 then
			char.TalkToCli(charaindex, -1, "抱歉！您的道具栏空位不足1个！", "黄色");
			return;
		end
		local petindex = char.AddPet( charaindex, 351,1);
		npc.ClsEvent(charaindex, 47);
		npc.EvEnd(charaindex, 47);
		char.DelItem(charaindex,haveitemindex);
		char.TalkToCli(charaindex, -1, "恭喜您猜谜三兄弟任务！", "黄色");
	end
end



function mydata()
	evendlist182 = {1,2,3,4,5,7,8,10,11,12,13,15,16,17,18,19,20,21,22,27,28,29,30,31,32,33,34,35,37,38,44,45,47,54};
	evendlist={{{39}, "琉璃洞窟", 2701}
			  ,{{40}, "深红洞窟", 2707}
			  ,{{41,42}, "玄黄洞窟", 2735}
			  ,{{46}, "碧青洞窟", 2770}
			  };	
	evendExp= {"塔姆塔姆村的药草",--1
				"不可思议的贝壳",--2
				"亚姆亚姆的斧头  送斧头",--3
				"成人仪式",--4
				"亚姆亚姆的斧头  还信",--5
				"哥亚山洞窟  把花和信给乌力美",--7
				"哥亚山洞窟  回信给1楼的猪",--8
				"",--10
				"",--11
				"",--12
				"强盗洞窟",--13
				"恐龙博士 抓鲁尼帖斯",--15
				"恐龙博士 抓贝鲁卡|",--16
				"龙洞任务",--17
				"伐木任务",--18
				"",--19
				"强恩洞窟  找到强恩",--20
				"强恩洞窟  找到吉德的女儿",--21
				"强恩洞窟  把家书给吉德",--22
				"暴龙之路",--27
				"五兄弟之迷   拿到智慧水晶",--28
				"黄金羚羊之路  拿到守护精灵的衣服",--29
				"黄金羚羊之路  拿到心性之镜",--30
				"五兄弟之迷   拿到人龙",--31
				"梦幻洞窟  拿竖琴",--32
				"梦幻洞窟  拿帽子",--33
				"梦幻洞窟  拿衣服",--34
				"卡坦  找4只鲨鱼",--35
				"马祖任务",--37
				"水田迷宫",--38
				"猜谜3兄弟",--44
				"四宝玉之迷",--45
				"猜谜3兄弟  拿蓝暴",--47
				"黄金羚羊之路  拿到黄金羚羊"--54
				};
						
end

function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.TalkToCli(_charaindex, -1, "重读NPC[任务标签]完成", 6);
	end
end

function main()
	mydata();
	item.addLUAListFunction( "ITEM_SetEvent", "SetEvent", "");
	magic.addLUAListFunction("ReSetEvent", "reload", "", 3, "[ReSetEvent]");
end
