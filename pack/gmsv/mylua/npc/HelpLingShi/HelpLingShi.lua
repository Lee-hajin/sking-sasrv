function checkEmptItemNum(charaindex)
	local maxitemnum = 9+15;
	EmptyItemNum = 0;
	for i = 9, maxitemnum-1 do
		if char.getItemIndex(charaindex, i) == -1 then
			EmptyItemNum = EmptyItemNum + 1;
		end
	end
	return EmptyItemNum;
end

--NPC对话事件(NPC索引)
function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		token = "　　　      　　 『 " .. char.getChar(meindex, "名字") .. " 』\n"
				.. "\n你好，我是宠物能力值查看员！";
		lssproto.windows(talkerindex, 0, 12, 1, char.getWorkInt( meindex, "对象"), token);
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked ( meindex, talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(meindex, talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return;
	end
	if _seqno == 1 then
		lssproto.windows(talkerindex, "宠物框", 8, 2, char.getWorkInt( meindex, "对象"), "请选择你要查看的宠物：");
	elseif _seqno == 2 then
		local petid = other.atoi(_data) - 1;
		local petindex = char.getCharPet(talkerindex, petid);
		if char.check(petindex) == 1 then
			local LevelUpPoint = char.getInt(petindex, "能力值");
			local vital = char.getRightTo8(LevelUpPoint, 1);
			local str = char.getRightTo8(LevelUpPoint, 2);
			local tgh = char.getRightTo8(LevelUpPoint, 3);
			local dex = char.getRightTo8(LevelUpPoint, 4);
			local znum = vital + str + tgh + dex;
			local array = char.getInt(petindex, "宠ID");
			local tempno = pet.getEnemyTempArrayFromTempNo(array);
			if char.getInt(petindex, "宠ID") == 718 or char.getInt(petindex, "宠ID") == 401 then
				local token = "\n[[style c=10]" .. char.getChar(petindex,"名字") .. "[/style]]喂石情况如下:"
							.. "\n"..string.format("[style c=1]血能力：[/style]%-10d满石值：%d",vital,50)
							.. "\n"..string.format("[style c=1]攻能力：[/style]%-10d满石值：%d",str,50)
							.. "\n"..string.format("[style c=1]防能力：[/style]%-10d满石值：%d",tgh,50)
							.. "\n"..string.format("[style c=1]敏能力：[/style]%-10d满石值：%d",dex,50);
				if znum >= 200 then
					token = token	.. "\n[style c=5]满石状态:已满石[/style]";
				else
					token = token	.. "\n[style c=6]满石状态:未满石[/style]";
				end
				lssproto.windows(talkerindex, 0, 8, -1, char.getWorkInt( meindex, "对象"), token);
			else
				char.TalkToCli(talkerindex, meindex, "你选择的宠物不是玛蕾菲雅，无法查询！", 6);
			end
		else
			char.TalkToCli(talkerindex, meindex, "你选择的栏位没有宠物？", 6);
		end
	end
end

function GetCountByType(charaindex,_type)
	local maxitemnum = 9+15;
	ItemCount = 0;
	for i = 9, maxitemnum-1 do
		local itemindex = char.getItemIndex(charaindex, i);
		if itemindex > -1 then
			if item.getInt(itemindex,"序号") == gemlist[_type][1] then
				ItemCount = ItemCount + 1;
			end
		end
	end
	return ItemCount;
end

function Create(name, metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(name, metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function reload()
	print("重读NPC信息",char.getChar(npcindex, "名字"));
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.ToAroundChar(npcindex);
end

function mydata()
	npcdata = {"宠物鉴定师",16969,777,49,41,4,"HelpLingShi"};
	stoneprice = 100;	--鉴定一个所需石币
	gemlist = {{19625,19629,19672,19673,19674,19675}
			  ,{19626,19630,19676,19677,19678,19679}
			  ,{19627,19631,19680,19681,19682,19683}
			  ,{19628,19632,19684,19685,19686,19687}
			  };
	gemname = {"","极品","稀有","普通","略差","很差"};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 1, "["..npcdata[7].."]");
end

