------------------------------- 杂类模块 -------------------------------
function StringSplit(_data, _splitint)--分割处理函数（返回数组）
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

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if (_select == 8) then
		return ;
	end
	if (_seqno >= 10) then--开始提取
		local haveitemindex = char.getWorkInt(_talkerindex,"临时整数1");
		local itemindex = char.getItemIndex(_talkerindex,haveitemindex);
		if item.check(itemindex) == 0 then
			return ;
		end
		local setid = other.atoi(_seqno-10);--选择的宠物位置
		local petindex = char.getCharPet(_talkerindex, setid);--获取宠物索引
		if char.check(petindex) == 1 then--判断该索引宠物是否存在
			local array = char.getInt(petindex, "宠ID");
			--判断宠物是否可以提取
			if GetPetStart == true then
				local GetStart = false;
				for i=1,table.getn(GetPetList) do
					if array == GetPetList[i] then
						GetStart = true;
						break ;
					end
				end
				if GetStart == false then
					char.TalkToCli(_talkerindex, _meindex, "该宠物不可提取成长档！",4);
					return ;
				end
			end
			--判断宠物是否可以提取结束
			local Blility = {0,0,0,0};
			local LevelUpPoint = char.getInt(petindex, "能力值");
			Blility[1] = char.getRightTo8(LevelUpPoint, 1);
			Blility[2] = char.getRightTo8(LevelUpPoint, 2);
			Blility[3] = char.getRightTo8(LevelUpPoint, 3);
			Blility[4] = char.getRightTo8(LevelUpPoint, 4);
			local randnum = math.random(1,4);--获取随机数
			--local ItemLv = "E";
			local randdel = math.random(1,10);
			if randdel < 3 or randdel > 8 then
				char.DelItem(_talkerindex, haveitemindex);--删除道具
				char.TalkToCli(_talkerindex, _meindex, "失败，档次卡消失!", 4);
				return ;
			end
			for i=1,table.getn(itemlist) do
				for j=1,table.getn(itemlist[i][2]) do
					if array == itemlist[i][2][j] then
						ItemLv = itemlist[i][1];
						if ItemLv == "S" then
							ItemMoto = 30133
						elseif ItemLv == "A" then
							ItemMoto = 30134
						else
							ItemMoto = 30135
						end
						break ;
					end
				end
			end
			--给予道具
			local itemindex = char.Additem(_talkerindex, ItemMoto);
			if itemindex == -1 then
				char.TalkToCli(_talkerindex, _meindex, "身上的道具已满了!", 4);
				return ;
			end
			local ItemType = {"血","攻","防","敏"};
			item.setChar(itemindex,"名称","『"..ItemLv.."』级档次卡[".. ItemType[randnum] .."]");
			item.setChar(itemindex,"显示名","『"..ItemLv.."』级档次卡[".. ItemType[randnum] .."]");
			
			--local ItemMoto = {0,0,0,0};
			--item.setInt(itemindex,"图号",ItemMoto[randnum]);--如果需要修改到图号，这里的注释去掉
			
			item.setChar(itemindex,"说明","使用后可将『"..ItemLv.."』级宠物的[".. ItemType[randnum] .."]成长档更换为"..Blility[randnum]);
			item.setChar(itemindex,"字段",randnum.."|"..Blility[randnum].."|"..ItemLv);--取随机能力值写到字段里
			char.sendStatusString(_talkerindex,"I");

			char.DelPet(_talkerindex,petindex);--需要删除宠物的话，这里的注释去掉
			char.TalkToCli(_talkerindex, _meindex, "获得"..ItemType[randnum]..Blility[randnum].."成长档卡片！",4);
			
			char.DelItem(_talkerindex, haveitemindex);--删除道具
		else
			char.TalkToCli(_talkerindex, _meindex, "您选择的宠物栏位没有宠物！",4);
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "                [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好，介绍自己写！"
					.."\n1、介绍自己写"
					.."\n2、介绍自己写！"
					.."\n3、介绍自己写！"
					.."\n4、介绍自己写！";
		lssproto.windows(_talkerindex, "对话框", 12, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 4);
	end
end

------------------------------- 道具使用函数 -------------------------------

function Loop(meindex)
	if char.getInt(meindex, "原图像号") == char.getInt(meindex, "图像号") then
		char.setInt(meindex, "图像号", 101147);
		char.ToAroundChar(meindex);
		char.setInt(meindex, "循环事件时间", 2000);
	else
		char.setInt(meindex, "图像号", char.getInt(meindex, "原图像号"));
		char.ToAroundChar(meindex)
		char.setInt(meindex, "循环事件时间", 0);
		char.setWorkInt(meindex, "捡起模式", 1);
		char.delFunctionPointer(meindex, "循环事件");
		char.TalkToRound(meindex, "成长档已发生变化！", "白色");
	end
end

function PetChageBlilityItem(itemindex, charaindex, toindex, haveitemindex)
	if char.getInt(toindex,"类型") ~= 3 then
		char.TalkToCli(charaindex, -1, "该道具只能给宠物使用！", 4);
		return ;
	end
	if char.check(toindex) == 0 then--判断该索引宠物是否存在
		char.TalkToCli(charaindex, -1, "宠物不存在？？请勿使用封包试图绕过判断！", 4);
		return ;
	end
	local petid = -1;
	for i = 0, 4 do
		if toindex == char.getCharPet(charaindex, i) then
			petid = i;
			break ;
		end
	end
	
	if char.getInt(charaindex, "骑宠") == petid then
		char.TalkToCli(charaindex, -1, "骑乘中的宠物无法使用!", 4);
		return ;
	end
	if char.getInt(toindex, "守护兽") == 1 then
		char.TalkToCli(charaindex, -1, "无法对守护兽使用!", 4);
		return ;
	end
	
	local strdata = item.getChar(itemindex, "字段");--获取道具字段
	if strdata == "" then--如果字段为空，进行抽取
		local array = char.getInt(toindex, "宠ID");
		--判断宠物是否可以提取
		if GetPetStart == true then
			local GetStart = false;
			for i=1,table.getn(GetPetList) do
				if array == GetPetList[i] then
					GetStart = true;
				end
			end
			if GetStart == false then
				char.TalkToCli(charaindex, -1, "该宠物不可提取成长档！",4);
				return ;
			end
		end
		--判断宠物是否可以提取结束
		local Blility = {0,0,0,0};
		local LevelUpPoint = char.getInt(toindex, "能力值");
		local tempno = pet.getEnemyTempArrayFromTempNo(array);
		Blility[1] = char.getRightTo8(LevelUpPoint, 1);
		Blility[2] = char.getRightTo8(LevelUpPoint, 2);
		Blility[3] = char.getRightTo8(LevelUpPoint, 3);
		Blility[4] = char.getRightTo8(LevelUpPoint, 4);
		for i=1,table.getn(itemlist[1][2]) do
			if char.getInt(toindex, "宠ID") == itemlist[1][2][i] then
				if Blility[1] <= pet.getInt(tempno, "体力") or Blility[2] <= pet.getInt(tempno, "腕力") or Blility[3] <= pet.getInt(tempno, "耐力") or Blility[4] <= pet.getInt(tempno, "速度") then	
					char.TalkToCli(charaindex, -1, "非满石MM圣兽无法抽取档次！",4);
					return ;
				end
				break;	
			end
		end
		local token = "                \n"
					.. char.getChar(toindex, "名字") .. " Lv" .. char.getInt(toindex, "等级") .. "  四围成长档如下：\n"
					.. string.format("体力：%d\n腕力：%d\n耐力：%d\n速度：%d\n", Blility[1], Blility[2], Blility[3],  Blility[4])
					.. "[style c=6]提取后宠物会消失！[/style]\n[style c=6]您确定要提取么？？？[/style]";
		char.setWorkInt(charaindex,"临时整数1",haveitemindex);
		lssproto.windows(charaindex, 0, 12, 10+petid, char.getWorkInt( npcindex, "对象"), token);
		return ;
	end
	--字段不为空，继续运行下面代码
	local ItemType = other.atoi(other.getString(strdata, "|", 1));--取出道具字段的类型
	local ItemData = other.atoi(other.getString(strdata, "|", 2));--取出道具字段的数值
	local ItemLv = other.getString(strdata, "|", 3);--取出道具字段的数值
	--防错用
	if ItemData < 0 or ItemData > 80 then
		char.TalkToCli(charaindex, -1, "该道具有误[1]！！！", 4);
		return ;
	end
	if ItemType <= 0 or ItemType >= 5 then
		char.TalkToCli(charaindex, -1, "该道具有误[2]！！！", 4);
		return ;
	end
	local sfbc = false;
	local array = char.getInt(toindex, "宠ID");
	for i=1,table.getn(itemlist) do
		if ItemLv == itemlist[i][1] then
			for j=1,table.getn(itemlist[i][2]) do
				if array == itemlist[i][2][j] then
					sfbc = true;
					break ;
				end
			end
		end
	end
	if sfbc == false then
		char.TalkToCli(charaindex, -1, "该宠物不可使用！",4);
		return ;
	end
	if char.getInt(toindex, "转数") == 0 then
		char.TalkToCli(charaindex, -1,"宠物未转生不可使用！",4);
		return ;
	end

	--判断宠物可否使用成长宝石
	if SetPetStart == true then
		local SetStart = false;
		for i=1,table.getn(SetPetList) do
			if array == SetPetList[i] then
				SetStart = true;
			end
		end
		if SetStart == false then
			char.TalkToCli(charaindex, -1, "该宠物不可使用！",4);
			return ;
		end
	end
	--判断宠物可否使用成长宝石结束
	local tempno = pet.getEnemyTempArrayFromTempNo(array);
	if tempno < 0 then
		char.TalkToCli(charaindex, -1, "该道具有误[3]！！！", 4);
		return ;
	end

	local initnum = pet.getInt(tempno, "初始值");--我的端
	--local initnum = enemytemp.getInt(tempno, "初始值");--别人的端
	local ability = {0, 0, 0, 0};		
	for i=1, 10 do
		local rnd = math.random(1,4);
		ability[rnd] = ability[rnd] + 1;
	end
	local Blility = {0,0,0,0};
	local LevelUpPoint = char.getInt(toindex, "能力值");--取出宠物原来的能力值
	Blility[1] = char.getRightTo8(LevelUpPoint, 1);
	Blility[2] = char.getRightTo8(LevelUpPoint, 2);
	Blility[3] = char.getRightTo8(LevelUpPoint, 3);
	Blility[4] = char.getRightTo8(LevelUpPoint, 4);
	
	Blility[ItemType] = ItemData;--将道具里的能力值修改进宠物能力值里

	char.setInt(toindex, "体力", (Blility[1] + ability[1]) * initnum);
	char.setInt(toindex, "腕力", (Blility[2] + ability[2]) * initnum);
	char.setInt(toindex, "耐力", (Blility[3] + ability[3]) * initnum);
	char.setInt(toindex, "速度", (Blility[4] + ability[4]) * initnum);

	local Beat = char.getLiftTo8(Blility[1],1)+char.getLiftTo8(Blility[2],2)+char.getLiftTo8(Blility[3],3)+char.getLiftTo8(Blility[4],4);--重新封装宠物能力值
	char.setInt(toindex, "能力值", Beat);
	--char.setInt(toindex, "祝福值", Beat);
	char.setInt(toindex, "等级", 1);
	char.setInt(toindex, "经验", 0);
	--char.setInt(toindex, "转数", 1);
	char.setInt(toindex, "HP", char.getWorkInt(toindex, "最大HP"));
	char.complianceParameter(toindex);
	--char.dropPetFollow(charaindex, petid);
	--char.setWorkInt(toindex, "捡起模式", 3);
	--char.setFunctionPointer(toindex, "循环事件", "Loop", "");
	--char.setInt(toindex, "循环事件时间", 1000);
	char.sendStatusString(charaindex, "K" .. petid);
	local ItemTypeStr = {"血","攻","防","敏"};
	char.TalkToCli(charaindex, -1, "["..char.getChar(toindex, "名字").."]"..ItemTypeStr[ItemType].."成长档发生变化！", 4);
	char.DelItem(charaindex, haveitemindex);--删除道具
end


------------------------------- 常量设置函数 -------------------------------
function mydata()
	npcdata = {"宠物档次抽取",100000,777,31,40,6,"PetChageBlility"};
	GetItemID = 30132;
	GetPetStart = false;--是否判断宠物可否抽取(true为开启判断， false为不开启)
	GetPetList = {1,2,3,4,5,6,7,8,9,10};--可获取成长档的宠物列表(enemybase编号)
	SetPetStart = false;--是否判断宠物可否使用(true为开启判断， false为不开启)
	SetPetList = {1,2,3,4,5,6,7,8,9,10};--可使用成长档的宠物列表(enemybase编号)
	itemlist = {{"S",{777,918,933,1140,2087,2088,2089,2090,2091,2092}}
				,{"A",{2083,2804}}
				,{"B",{21,22,23,24,31,32,33,34,41,42,43,44,51,52,53,54,61,62,63,64,65,71,72,73,74,81,82,83,84,85,86,91,92,93,94,101,102,103,104,111,112,113,114,141,142,143,144,191,192,193,194,221,222,223,224,231,232,233,234,251,252,253,254,255,261,262,263,264,271,272,273,274,291,292,293,294,301,302,303,304,312,313,314,315,791}}
				};
	--itemset6.txt 加入以下道具
	--档次石,档次石,对宠物使用可随机抽取宠物的成长档！,,,,,,,,ITEM_PetChageBlility,,,,,,30132,24047,0,20,2,1,0,-1,,,,,,,,,,,,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,,0,0,,,,,,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,1,0,0,肉,60,鱼,25,橡树果,69,海草,100,,

end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
	item.addLUAListFunction( "ITEM_PetChageBlility", "PetChageBlilityItem", "");
end
