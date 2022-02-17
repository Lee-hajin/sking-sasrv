function StringSplit(_data, _splitint)-- 分割处理函数（返回数组）
	local StartIndex = 1;
	local SplitIndex = 1;
	local SplitArray = {};
	while true do
		local FindLastIndex = string.find(_data, _splitint, StartIndex);
		if not FindLastIndex then
			SplitArray[SplitIndex] = string.sub(_data, StartIndex, string.len(_data));
			break ;
		end
		SplitArray[SplitIndex] = string.sub(_data, StartIndex, FindLastIndex - 1);
		StartIndex = FindLastIndex + string.len(_splitint);
		SplitIndex = SplitIndex + 1;
	end
	return SplitArray;
end

function ItemUpLevelFunction(charindex,fromitemindex,toitemindex,fromid,toid)
	local ItemUse = item.getChar(fromitemindex,"使用函数名");--取得强化石使用函数名
	if ItemUse == "ITEM_UPITEM" then
		local ItemData = StringSplit(item.getChar(fromitemindex,"字段"),"|");--取出字段
		if table.getn(ItemData) < 2 then
			--道具参数有误
			return ;
		end
		local ItemType = ItemData[1];
		local ItemMode = ItemData[2];
		local toitemid = item.getInt(toitemindex, "序号");
		local itemupcf = false;
		for i=1,table.getn(ItemUpList) do
			if toitemid == ItemUpList[i][1] then
				itemupcf = true;
				break ;
			end
		end
		if itemupcf == false then
			char.TalkToCli(charindex, -1,"该锻造石无法锻造此类道具！", 6);
			return ;
		end
		
		if item.getInt(toitemindex, "强等") < 0 then
			item.setInt(toitemindex, "强等",0);
		end
		if item.getInt(toitemindex, "强攻") < 0 then
			item.setInt(toitemindex, "强攻",0);
		end
		if item.getInt(toitemindex, "强防") < 0 then
			item.setInt(toitemindex, "强防",0);
		end
		if item.getInt(toitemindex, "强敏") < 0 then
			item.setInt(toitemindex, "强敏",0);
		end
		
		local toitemid = item.getInt(toitemindex, "序号");
		for i=1,table.getn(NoUpItem) do
			if toitemid == NoUpItem[i] then
				char.TalkToCli(charindex, -1,"该装备禁止锻造！["..toitemid.."]", 6);
				return ;
			end
		end
		local itemupnum = item.getInt(toitemindex, "强等");
		local itemupat1 = item.getInt(toitemindex, "强攻");
		local itemupat2 = item.getInt(toitemindex, "强防");
		local itemupat3 = item.getInt(toitemindex, "强敏");
		if itemupnum >= 25 then
			char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]已锻造了25次了，无法再进行锻造！", 6);
			return ;
		end
		if ItemType == "攻" then
			if itemupat1 >= 6 then
				if ItemMode == "A" then
					char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
					return 1;
				end
			end
			if itemupat1 >= 12 then
				char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]攻属性已经锻造了12次了，无法再进行锻造！", 6);
				return ;
			end
		elseif ItemType == "防" then
			if itemupat2 >= 6 then
				if ItemMode == "A" then
					char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
					return ;
				end
			end
			if itemupat2 >= 12 then
				char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]防属性已经锻造了12次了，无法再进行锻造！", 6);
				return ;
			end
		elseif ItemType == "敏" then
			if itemupat3 >= 6 then
				if ItemMode == "A" then
					char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
					return ;
				end
			end
			if itemupat3 >= 12 then
				char.TalkToCli(charindex, -1, "["..item.getChar(toitemindex, "名称") .. "]敏属性已经锻造了12次了，无法再进行锻造！", 6);
				return ;
			end
		end
		char.setWorkChar(charindex,"临时字串1",fromitemindex.."|"..toitemindex.."|"..fromid.."|"..toid);
		token = "[style c=4]锻造说明：[/style]"
				.."\n单项属性最高提升[style c=5]12[/style]次！"
				.."\n属性锻造总合最高[style c=5]25[/style]次！"
				.."\n锻造后装备属性加强！只会提升[style c=5]攻、防、敏[/style]属性！"
				.."\n每锻造1次，对应属性会提升1点！"
				.."\n你是否要将[[style c=2]" .. item.getChar(toitemindex, "名称") .. "[/style]]进行锻造？";
		lssproto.windows(charindex,0, 12, 1, char.getWorkInt( npcindex, "对象"), token);
		return ;
	end
	return ;
end

function upitemshuxing(charindex,itemindex)
	itemlv = item.getInt(itemindex, "物品等级");
	local setitemmsg = "";
	local msglist = {"攻","防","敏","HP","MP","运","魅","伤","吸"};
	for i=1,table.getn(msglist) do
		if item.getInt(itemindex, msglist[i]) ~= 0 then
			if item.getInt(itemindex, msglist[i]) > 0 then
				setitemmsg = setitemmsg .. msglist[i].."+"..item.getInt(itemindex, msglist[i]).." ";
			else
				setitemmsg = setitemmsg .. msglist[i]..item.getInt(itemindex, msglist[i]).." ";
			end
		end
	end	
	local MagicId = item.getInt(itemindex, "精灵");
	if MagicId >= 0 then
		local mfname = magic.getChar(MagicId,"名字");
		setitemmsg = setitemmsg .. mfname;
	end
	
	local itemg1 = item.getInt(itemindex, "强攻");
	local itemf1 = item.getInt(itemindex, "强防");
	local itemm1 = item.getInt(itemindex, "强敏");
	
	item.setChar(itemindex, "说明", "[" .. itemg1 .. "|"..itemf1.."|"..itemm1 .. "]"..setitemmsg);
	item.UpdataItemOne(charindex, itemindex);
	return ;
end

function upitemlevel(charindex,itemindex,flg,UpType)
	local itemlv = item.getInt(itemindex, "强等");
	local itemg1 = item.getInt(itemindex, "强攻");
	local itemf1 = item.getInt(itemindex, "强防");
	local itemm1 = item.getInt(itemindex, "强敏");
	local itemg2 = item.getInt(itemindex, "攻");
	local itemf2 = item.getInt(itemindex, "防");
	local itemm2 = item.getInt(itemindex, "敏");
	if UpType == 1 then--锻造攻击
		if flg == true then--锻造成功
			item.setInt(itemindex, "强攻",itemg1+1);
			item.setInt(itemindex, "攻",itemg2+1);
			item.setInt(itemindex, "强等",itemlv+1);
		else--锻造失败
			item.setInt(itemindex, "强攻",itemg1-1);
			item.setInt(itemindex, "攻",itemg2-1);
			item.setInt(itemindex, "强等",itemlv-1);
		end
	elseif UpType == 2 then--锻造防御
		if flg == true then--锻造成功
			item.setInt(itemindex, "强防",itemf1+1);
			item.setInt(itemindex, "防",itemf2+1);
			item.setInt(itemindex, "强等",itemlv+1);
		else--锻造失败
			item.setInt(itemindex, "强防",itemf1-1);
			item.setInt(itemindex, "防",itemf2-1);
			item.setInt(itemindex, "强等",itemlv-1);
		end
	elseif UpType == 3 then--锻造敏捷
		if flg == true then--锻造成功
			item.setInt(itemindex, "强敏",itemm1+1);
			item.setInt(itemindex, "敏",itemm2+1);
			item.setInt(itemindex, "强等",itemlv+1);
		else--锻造失败
			item.setInt(itemindex, "强敏",itemm1-1);
			item.setInt(itemindex, "敏",itemm2-1);
			item.setInt(itemindex, "强等",itemlv-1);
		end
	end
	--改变图号
	if item.getInt(itemindex, "强等") >= 25 then
		local toitemid = item.getInt(itemindex, "序号");
		local itemupcf = -1;
		for i=1,table.getn(ItemUpList) do
			if toitemid == ItemUpList[i][1] then
				itemupcf = i;
				break ;
			end
		end
		if itemupcf > 0 then
			item.setInt(itemindex, "图号",ItemUpList[itemupcf][2]);
		end
	end
	upitemshuxing(charindex,itemindex);
	return ;
end

function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if _seqno == 1 then
		if _select == 4 then
			local mytmdata1 = char.getWorkChar(_talkerindex,"临时字串1");
			local mytmdata2 = StringSplit(mytmdata1,"|");
			if table.getn(mytmdata2) < 4 then
				--char.TalkToCli(_talkerindex, -1,"参数有误，请重新操作！", 6);
				return ;
			end
			local itemindexA1 = other.atoi(mytmdata2[1]);
			local itemindexA2 = other.atoi(mytmdata2[2]);
			local fromid = other.atoi(mytmdata2[3]);
			local toid = other.atoi(mytmdata2[4]);
			local itemindexB1 = char.getItemIndex(_talkerindex,fromid);
			local itemindexB2 = char.getItemIndex(_talkerindex,toid);
			if itemindexA1 ~= itemindexB1 then
				char.TalkToCli(_talkerindex, -1,"请勿使用外挂移动道具的功能来试图躲避判断，多次违规将被永久封号！", 6);
				return ;
			end
			if itemindexA2 ~= itemindexB2 then
				char.TalkToCli(_talkerindex, -1,"请勿使用外挂移动道具的功能来试图躲避判断，多次违规将被永久封号！", 6);
				return ;
			end
			local ItemUse = item.getChar(itemindexA1,"使用函数名");--取得强化石使用函数名
			if ItemUse ~= "ITEM_UPITEM" then
				char.TalkToCli(_talkerindex, -1,"该道具非锻造石！", 6);
				return ;
			end
			local ItemData = StringSplit(item.getChar(itemindexA1,"字段"),"|");--取出字段
			if table.getn(ItemData) < 2 then
				--道具参数有误
				return ;
			end
			local ItemType = ItemData[1];
			local ItemMode = ItemData[2];
			
			local toitemid = item.getInt(itemindexA2, "序号");
			local toitemtype = item.getInt(itemindexA2, "类型");
			local itemupcf = false;
			for i=1,table.getn(ItemUpList) do
				if toitemid == ItemUpList[i][1] then
					itemupcf = true;
					break ;
				end
			end
			if itemupcf == false then
				char.TalkToCli(_talkerindex, -1,"该道具不可进行锻造！", 6);
				return ;
			end
			
			for i=1,table.getn(NoUpItem) do
				if toitemid == NoUpItem[i] then
					char.TalkToCli(_talkerindex, -1,"该装备禁止锻造！["..toitemid.."]", 6);
					return ;
				end
			end
			if item.getInt(itemindexA2, "强等") >= 25 then
				char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]已锻造了25次了，无法再进行锻造！", 6);
				return ;
			end
			if ItemType == "攻" then
				local MyLevel = item.getInt(itemindexA2, "强攻");
				if MyLevel >= 12 then
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]攻属性已经锻造了12次了，无法再进行锻造！", 6);
					return ;
				elseif MyLevel < 0 then
					MyLevel = 0;
				end
				if MyLevel >= 6 then
					if ItemMode == "A" then
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
						return ;
					end
				end
				local UpRand = math.random(1,100);
				char.DelItem(_talkerindex, fromid);--扣除强化石
				if UpRand < ItemUpRand[1][MyLevel+1] then--成功
					upitemlevel(_talkerindex,itemindexA2,true,1);--人物索引，道具索引，成功标识，属性类型
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造成功，目前锻造次数："..item.getInt(itemindexA2, "强等").."/25,攻的锻造次数为："..item.getInt(itemindexA2, "强攻").."/12！", "黄色");
					return ;
				else--失败
					if MyLevel <= ItemNoDown then--保底不降级
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，+"..ItemNoDown.."以下锻造失败不会降级！", "黄色");
						return ;
					else--降级
						upitemlevel(_talkerindex,itemindexA2,false,1);--人物索引，道具索引，成功标识，属性类型
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，装备降级！", "黄色");
						return ;
					end
				end
			elseif ItemType == "防" then
				local MyLevel = item.getInt(itemindexA2, "强防");
				if MyLevel >= 12 then
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]防属性已经锻造了12次了，无法再进行锻造！", 6);
					return ;
				elseif MyLevel < 0 then
					MyLevel = 0;
				end
				if MyLevel >= 6 then
					if ItemMode == "A" then
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
						return ;
					end
				end
				local UpRand = math.random(1,100);
				char.DelItem(_talkerindex, fromid);--扣除强化石
				if UpRand < ItemUpRand[2][MyLevel+1] then--成功
					upitemlevel(_talkerindex,itemindexA2,true,2);--人物索引，道具索引，成功标识，属性类型
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造成功，目前锻造次数："..item.getInt(itemindexA2, "强等").."/25,攻的锻造次数为："..item.getInt(itemindexA2, "强防").."/12！", "黄色");
					return ;
				else--失败
					if MyLevel <= ItemNoDown then--保底不降级
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，+"..ItemNoDown.."以下锻造失败不会降级！", "黄色");
						return ;
					else--降级
						upitemlevel(_talkerindex,itemindexA2,false,2);--人物索引，道具索引，成功标识，属性类型
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，装备降级！", "黄色");
						return ;
					end
				end
			elseif ItemType == "敏" then
				local MyLevel = item.getInt(itemindexA2, "强敏");
				if MyLevel >= 12 then
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]敏属性已经锻造了12次了，无法再进行锻造！", 6);
					return ;
				elseif MyLevel < 0 then
					MyLevel = 0;
				end
				if MyLevel >= 6 then
					if ItemMode == "A" then
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]攻锻造高于6次，无法使用该等级的晶石锻造！", 6);
						return ;
					end
				end
				local UpRand = math.random(1,100);
				char.DelItem(_talkerindex, fromid);--扣除强化石
				if UpRand < ItemUpRand[3][MyLevel+1] then--成功
					upitemlevel(_talkerindex,itemindexA2,true,3);--人物索引，道具索引，成功标识，属性类型
					char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造成功，目前锻造次数："..item.getInt(itemindexA2, "强等").."/25,敏的锻造次数为："..item.getInt(itemindexA2, "强敏").."/12！", "黄色");
					return ;
				else--失败
					if MyLevel <= ItemNoDown then--保底不降级
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，+"..ItemNoDown.."以下锻造失败不会降级！", "黄色");
						return ;
					else--降级
						upitemlevel(_talkerindex,itemindexA2,false,3);--人物索引，道具索引，成功标识，属性类型
						char.TalkToCli(_talkerindex, -1, "["..item.getChar(itemindexA2, "名称") .. "]锻造失败，装备降级！", "黄色");
						return ;
					end
				end
			end
		end
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
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
	--可锻造的装备编号
	--1编号,25次改变的造型编号
	ItemUpList ={{1,20073}
				};
	NoUpItem = {};--强制禁止强化的道具
	ItemUpRand = {{100,100,100,100,100,100,100,100,100,100,100,100}--攻锻造成功率，分别1-12级成功率
				 ,{100,100,100,100,100,100,100,100,100,100,100,100}--防锻造成功率，分别1-12级成功率
				 ,{100,100,100,100,100,100,100,100,100,100,100,100}--敏锻造成功率，分别1-12级成功率
				 };
	ItemNoDown = 6;--保底等级，保底代表失败不降级
	npcdata = {"锻造系统",100000,777,32,20,6};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读装备锻造", "reload", "", 1, "[重读拖放系统]");
end

