function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		local ItemIndex = char.Finditem(talkerindex,FreeItem);
		if ItemIndex == -1 then
			token = "                 『"..char.getChar(meindex, "名字").."』"
				.."\n\n\n你身上没有该转的冲级领奖证明！";
			lssproto.windows(talkerindex, 0, 8, -1, char.getWorkInt( meindex, "对象"), token);
			return ;
		end
		local ItemTrans = other.atoi(item.getChar(ItemIndex,"字段"));
		local MyTrans = char.getInt(talkerindex,"转数");--取得玩家转数
		local MyLevel = char.getInt(talkerindex,"等级");
		local token = "";
		if MyTrans ~= ItemTrans then
			token = "                 『"..char.getChar(meindex, "名字").."』"
				.."\n\n\n该证明跟你当前的转数不符合，无法领取！";
			lssproto.windows(talkerindex, 0, 8, -1, char.getWorkInt( meindex, "对象"), token);
			return ;
		end
		if MyLevel < 140 then
			token = "                 『"..char.getChar(meindex, "名字").."』"
				.."\n\n\n必须达到140级才可进行领取！";
			lssproto.windows(talkerindex, 0, 8, -1, char.getWorkInt( meindex, "对象"), token);
			return ;
		end
		token = "                 『"..char.getChar(meindex, "名字").."』"
				.."\n\n\n你确定要领取"..MyTrans.."转的冲级奖励么？";
		lssproto.windows(talkerindex, 0, 12, 1, char.getWorkInt( meindex, "对象"), token);
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then
		if _select == 8 then
			return ;
		end
		if _seqno == 1 then
			local ItemIndex = char.Finditem(_talkerindex,FreeItem);
			if ItemIndex == -1 then
				token = "                 『"..char.getChar(_meindex, "名字").."』"
					.."\n\n\n你身上没有该转的冲级领奖证明！";
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
				return ;
			end
			local ItemTrans = other.atoi(item.getChar(ItemIndex,"字段"));
			local MyTrans = char.getInt(_talkerindex,"转数");--取得玩家转数
			local MyLevel = char.getInt(_talkerindex,"等级");
			local token = "";
			if MyTrans ~= ItemTrans then
				token = "                 『"..char.getChar(_meindex, "名字").."』"
					.."\n\n\n该证明跟你当前的转数不符合，无法领取！";
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
				return ;
			end
			if MyLevel < 140 then
				token = "                 『"..char.getChar(_meindex, "名字").."』"
					.."\n\n\n必须达到140级才可进行领取！";
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
				return ;
			end
			if MyTrans == 0 then
				local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
				if itemnum < 1 then
					char.TalkToCli(_talkerindex, -1, "道具栏必须空出1个位置！", "红色");
					return ;
				end
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				
				char.sendStatusString(_talkerindex,"I");
				local GetIndex = char.Additem( _talkerindex,23011);--智慧果酱包
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..item.getChar(GetIndex,"名称").." !", 4);
			elseif MyTrans == 1 then
				local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
				if itemnum < 1 then
					char.TalkToCli(_talkerindex, -1, "道具栏必须空出1个位置！", "红色");
					return ;
				end
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				char.sendStatusString(_talkerindex,"I");
				local GetIndex = char.Additem( _talkerindex,2960);--随机珊瑚包
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..item.getChar(GetIndex,"名称").." !", 4);
			elseif MyTrans == 2 then
				local petnum = char.GetEmptyPetNum(_talkerindex);--取得宠物栏数量
				if petnum < 1 then
					char.TalkToCli(_talkerindex, -1, "宠物栏必须空出1个位置！", "红色");
					return ;
				end
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				char.sendStatusString(_talkerindex,"I");
				local PetList = {1975,1976,1977,1978};--随机人狼
				local GetIndex = char.AddPet( _talkerindex, PetList[math.random(1,table.getn(PetList))],1);
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..char.getChar(GetIndex,"名字").." !", 4);
			elseif MyTrans == 3 then
				local petnum = char.GetEmptyPetNum(_talkerindex);--取得宠物栏数量
				if petnum < 1 then
					char.TalkToCli(_talkerindex, -1, "宠物栏必须空出1个位置！", "红色");
					return ;
				end
				
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				char.sendStatusString(_talkerindex,"I");
				
				local GetIndex = char.AddPet( _talkerindex, 1645,1);
				npc.EvEnd(_talkerindex, 400);
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..char.getChar(GetIndex,"名字").." !", 4);
			elseif MyTrans == 4 then
				local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
				if itemnum < 1 then
					char.TalkToCli(_talkerindex, -1, "道具栏必须空出1个位置！", "红色");
					return ;
				end
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				char.sendStatusString(_talkerindex,"I");
				
				local GetIndex = char.Additem( _talkerindex,3307);--威威骑乘同意书
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..item.getChar(GetIndex,"名称").." !", 4);
			elseif MyTrans == 5 then
				local petnum = char.GetEmptyPetNum(_talkerindex);--取得宠物栏数量
				if petnum < 1 then
					char.TalkToCli(_talkerindex, -1, "宠物栏必须空出1个位置！", "红色");
					return ;
				end
				local SItemTrans = ItemTrans + 1;
				local SItemStr = "拥有该道具，".. SItemTrans .."转140级可在渔村村长家冲级奖励员处领取一份礼品";
				item.setChar(ItemIndex,"字段",SItemTrans);
				item.setChar(ItemIndex,"说明",SItemStr);
				char.sendStatusString(_talkerindex,"I");
				local PetList = {1979,1980,2057,2058};--随机魔兽
				local GetIndex = char.AddPet( _talkerindex, PetList[math.random(1,table.getn(PetList))],1);
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..char.getChar(GetIndex,"名字").." !", 4);
			elseif MyTrans == 6 then
				local mypoint = char.getInt(_talkerindex, "技能点");
				local vital = other.atoi(char.getInt(_talkerindex, "体力")  / 100);
				local str = other.atoi(char.getInt(_talkerindex, "腕力") / 100);
				local tgh = other.atoi(char.getInt(_talkerindex, "耐力") / 100);
				local dex = other.atoi(char.getInt(_talkerindex, "速度") / 100);
				local sum = vital + str + tgh + dex + mypoint;
				if sum < 625 then
					char.TalkToCli(_talkerindex, -1, "要领取该装奖励必须属性点达到625点才可领取！", "红色");
					return ;
				end
				local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
				if itemnum < 1 then
					char.TalkToCli(_talkerindex, -1, "道具栏必须空出1个位置！", "红色");
					return ;
				end
				local GetIndex = char.Additem( _talkerindex,12932);--练级冲级礼包
				char.TalkToCli(_talkerindex, -1, "领取成功，获得 "..item.getChar(GetIndex,"名称").." !", 4);
				char.DelItemForIndex(_talkerindex,ItemIndex);
			end
		end
	end
end


function reloadnpc()
	print("重读NPC信息",char.getChar(npcindex, "名字"));
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.setInt(npcindex,"现称号图像",npcdata[8]);
	char.ToAroundChar(npcindex);
end

function mydata()
	FreeItem = 23012;
	npcdata = {"练级奖励员",51375,777,49,39,4,"FuLi",120001};
end

function Create(name, metamo, floor, x, y, dir)
	npcindex = npc.CreateNpc(name, metamo, floor, x, y, dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	char.setInt(npcindex,"现称号图像",npcdata[8]);
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读福利员", "reloadnpc", "", 1, "[重读福利员]");
end

