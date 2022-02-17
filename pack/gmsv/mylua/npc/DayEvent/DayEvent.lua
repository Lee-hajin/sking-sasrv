------------------------------- 获取人物信息返回 -------------------------------
function GetEventData(_talkerindex)
	local CdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
	local SqlStr = "select * from `CSA_DayEvent` where `Account` = '"..CdKey.."' and `SaveIndex`="..Saveindex;--SQL语句
	local SqlRet = sasql.query(SqlStr);
	local RetData = {0,0,0,0,0};
	if SqlRet == 1 then--语句执行成功
		sasql.free_result();
		sasql.store_result();
		local SqlNum = sasql.num_rows();
		if SqlNum > 0 then--如果查询信息存在的话，获取信息
			sasql.fetch_row();
			RetData[1] = other.atoi(sasql.data(3));--取得任务完成数量
			RetData[2] = other.atoi(sasql.data(4));--取得任务状态
			RetData[3] = other.atoi(sasql.data(5));--取得任务类型
			RetData[4] = other.atoi(sasql.data(6));--取得任务需求
			RetData[5] = other.atoi(sasql.data(7));--取得任务完成时间
		else--查询不到任何信息的话，写入信息
			SqlStr = "INSERT INTO `CSA_DayEvent` (`Account`,`SaveIndex`,`DataTime`) VALUES ('" .. CdKey.. "',"..Saveindex.. ",".. os.time() ..")";
			sasql.query(SqlStr);
			RetData[5] = os.time();
		end
	end
	return RetData;
end

------------------------------- 获取NPC信息函数 -------------------------------
function GetNpcInfo(_Flg)
	local Token = "";
	for i=1,table.getn(NpcList) do
		if NpcList[i][7] == _Flg then
			local TmMap = NpcList[i][3];
			local TmX = NpcList[i][4];
			local TmY = NpcList[i][5];
			Token = ""..map.getFloorName(TmMap).."("..TmX.."."..TmY..")";
			break;
		end
	end
	return Token;
end
------------------------------- 分配任务信息函数 -------------------------------
function SetNpcTaskWord(_PlayerData,_talkerindex,_NpcCf)
	local TaskNum = _PlayerData[1];
	local TmFlg = _PlayerData[2];--取出原本的NPC位
	local NpcFlg = TmFlg;
	while NpcFlg == TmFlg do
	   NpcFlg = math.random(2,table.getn(NpcList));--随机任务NPC
	end
	
	local TaskType = math.random(1,4);--随机任务类型
	local Token = "";--返回文本
	if TaskNum >= EventMaxNum then
		char.TalkToCli(_talkerindex, -1, "你今天已经做了"..EventMaxNum.."次了", 6);
		return
	end
	_PlayerData[2] = NpcFlg;
	_PlayerData[3] = TaskType;
	if TaskType == 1 then
		_PlayerData[4] = 0;
		if _NpcCf == 1 then
			Token = "[style c=10][送信任务][/style]\n"
				.."[style c=4]将口信传达到指定NPC处[/style]\n"
				.."[style c=5]Npc名字：[/style]".. char.getChar(NpcIndex[NpcFlg],"名字") .. "("..NpcFlg .. ")\n" 
				.."[style c=5]传达位置：[/style]"..GetNpcInfo(NpcFlg).."\n\n"
				.."[style c=6]请自己牢记任务信息，忘记了我也不会再提醒你哦[/style]";
		else
			Token = "[送信任务]将口信传达到["..GetNpcInfo(NpcFlg).."]的 "..char.getChar(NpcIndex[NpcFlg],"名字").." 处";
		end
	elseif TaskType == 2 then
		local RandNeedItem = NeedItem[math.random(table.getn(NeedItem))];
		_PlayerData[4] = RandNeedItem;
		if _NpcCf == 1 then
			Token = "[style c=10][寻找物品][/style]\n"
				.."[style c=4]寻找某个物品，将物品送到指定NPC处[/style]\n"
				.."[style c=5]Npc名字：[/style]".. char.getChar(NpcIndex[NpcFlg],"名字") .. "("..NpcFlg .. ")\n" 
				.."[style c=5]需求物品：[/style]"..item.getNameFromNumber(RandNeedItem).."(".. RandNeedItem ..")\n"
				.."[style c=5]送达位置：[/style]"..GetNpcInfo(NpcFlg).."\n"
				.."[style c=6]请自己牢记任务信息，忘记了我也不会再提醒你哦[/style]";
		else
			Token = "[寻找物品]寻找物品【".. item.getNameFromNumber(RandNeedItem) .."】送到["..GetNpcInfo(NpcFlg).."]的 "..char.getChar(NpcIndex[NpcFlg],"名字").." 处";
		end
	elseif TaskType == 3 then
		local RandNeedPet = NeedPet[math.random(table.getn(NeedPet))];
		_PlayerData[4] = RandNeedPet;
		if _NpcCf == 1 then
			Token = "[style c=10][抓捕宠物][/style]\n"
				.."[style c=4]捕抓某个宠物，将宠物送到指定NPC处[/style]\n"
				.."[style c=5]Npc名字：[/style]".. char.getChar(NpcIndex[NpcFlg],"名字") .. "("..NpcFlg .. ")\n" 
				.."[style c=5]需求宠物：[/style]"..pet.getEnemyTempNameFromEnemyID(RandNeedPet).."("..RandNeedPet..")\n"
				.."[style c=5]送达位置：[/style]"..GetNpcInfo(NpcFlg).."\n"
				.."[style c=6]请自己牢记任务信息，忘记了我也不会再提醒你哦[/style]";
		else
			Token = "[抓捕宠物]捕抓宠物【".. pet.getEnemyTempNameFromEnemyID(RandNeedPet) .."】送到["..GetNpcInfo(NpcFlg).."]的 "..char.getChar(NpcIndex[NpcFlg],"名字").." 处";
		end
	elseif TaskType == 4 then
		_PlayerData[4] = 0;
		if _NpcCf == 1 then
			Token = "[style c=10][挑战任务][/style]\n"
				.."[style c=4]战胜指定NPC[/style]\n"
				.."[style c=5]Npc名字：[/style]".. char.getChar(NpcIndex[NpcFlg],"名字") .. "("..NpcFlg .. ")\n" 
				.."[style c=5]挑战位置：[/style]"..GetNpcInfo(NpcFlg).."\n\n"
				.."[style c=6]请自己牢记任务信息，忘记了我也不会再提醒你哦[/style]";
		else
			Token = "[挑战任务]战胜["..GetNpcInfo(NpcFlg).."]的 "..char.getChar(NpcIndex[NpcFlg],"名字").." ！";
		end
	end
	local CdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
	local SqlStr = "update CSA_DayEvent SET EventState=".. _PlayerData[2] ..",EventType=".. _PlayerData[3] ..",EventNeed=".._PlayerData[4].." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
	sasql.query(SqlStr);
	return Token;
end
	
------------------------------- 获取任务信息函数 -------------------------------
function GetNpcTaskWord(_PlayerData,_talkerindex,_NpcCf)
	local CdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
	local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏空位
	local Token = "";
	local TaskNum = _PlayerData[1];
	if TaskNum >= EventMaxNum then
		char.TalkToCli(_talkerindex, -1, "你今天已经做了"..EventMaxNum.."次了", 6);
		return
	end
	local Npcflg = _PlayerData[2];--取得任务状态
	local TaskType = _PlayerData[3];--取得任务类型
	if TaskType == 1 then
		if _NpcCf == 1 then--不是要找的NPC
			Token = "你是来传达口信的？？\n我可不是你要找的人哦！";
		else
			Token = "口信拿来了吗？？";
		end
	elseif TaskType == 2 then
		--local RandNeedItem = NeedItem[math.random(table.getn(NeedItem))];
		local RandNeedItem = _PlayerData[4];
		if _NpcCf == 1 then--不是要找的NPC
			Token = "就算你有【[style c=10]"..item.getNameFromNumber(RandNeedItem).."[/style]("..RandNeedItem..")】\n我也不想要！！";
		else
			Token = "道具【[style c=10]"..item.getNameFromNumber(RandNeedItem).."[/style]("..RandNeedItem..")】拿来了吗？";
		end
		--local SqlStr = "update CSA_DayEvent SET EventNeed="..RandNeedItem.." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
		--sasql.query(SqlStr);
	elseif TaskType == 3 then
		--local RandNeedPet = NeedPet[math.random(table.getn(NeedPet))];
		local RandNeedPet = _PlayerData[4];
		if _NpcCf == 1 then--不是要找的NPC
			Token = "【[style c=10]"..pet.getEnemyTempNameFromEnemyID(RandNeedPet) .. "[/style]("..RandNeedPet..")】样子怪怪的！\n你带它离我远点！！";
		else
			Token = "宠物【[style c=10]" .. pet.getEnemyTempNameFromEnemyID(RandNeedPet).."[/style]("..RandNeedPet..")】拿来了吗？";
		end
		--local SqlStr = "update CSA_DayEvent SET EventNeed="..NeedPet.." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
		--sasql.query(SqlStr);
	elseif TaskType == 4 then
		if _NpcCf == 1 then--不是要找的NPC
			Token = "干嘛，你想揍我？？\n给我个理由先！！";
		else
			Token = "找死？想挑战我？";
		end
	end
	return Token;
end

------------------------------- NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return ;
	end
	if _seqno == 1 then--接第一个任务
		local PlayerData = GetEventData(_talkerindex);
		local Token = SetNpcTaskWord(PlayerData,_talkerindex,1);
		lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), Token);
	elseif _seqno == 2 then
		--PlayerData 1完成数量 2任务指定 3任务类型 4任务需求 5完成时间
		local PlayerData = GetEventData(_talkerindex);
		local NpcFlg = char.getWorkInt(_meindex,"临时整数1");--取得NPC号
		local MyFlg = PlayerData[2];
		local MyType = PlayerData[3];
		local MyNeed = PlayerData[4];
		local MyNum = PlayerData[1];
		local CdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
		local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
		
		if MyFlg ~= NpcFlg then
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), GetNpcTaskWord(PlayerData,_talkerindex,1));
			return ;
		end
		if MyType == 1 then--传达口信
			local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏空位
			if MyItemNum <= 0 then
				char.TalkToCli(_talkerindex, -1, "你的道具栏满了，请空出一个位置再来吧！", 6);
				return ;
			end
			char.setWorkInt(_talkerindex,"临时整数1",MyType);
			--任务完成数+1，并更新完成时间
			local SqlStr = "update CSA_DayEvent SET EventNum=EventNum+1,DataTime=".. os.time() .." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
			sasql.query(SqlStr);
			PlayerData[1] = PlayerData[1] + 1;
			PlayerData[5] = os.time();
			--给予奖励
			local JlRand = math.random(table.getn(LastItem));
			npc.AddItem(_talkerindex, LastItem[JlRand]);
			local Token = SetNpcTaskWord(PlayerData,_talkerindex,1);--重新安排新任务
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), "[style c=2]收到，这秘密确实劲爆！感谢！[/style]\n\n"..Token);
		elseif MyType == 2 then--寻找道具
			if npc.Free(_meindex, _talkerindex, "ITEM="..MyNeed) == 1 then
				npc.DelItem(_talkerindex,MyNeed.."*1");--扣除任务道具
				char.setWorkInt(_talkerindex,"临时整数1",MyType);
				--任务完成数+1，并更新完成时间
				local SqlStr = "update CSA_DayEvent SET EventNum=EventNum+1,DataTime=".. os.time() .." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
				sasql.query(SqlStr);
				PlayerData[1] = PlayerData[1] + 1;
				PlayerData[5] = os.time();
				--给予奖励
				local JlRand = math.random(table.getn(LastItem));
				npc.AddItem(_talkerindex, LastItem[JlRand]);
				local Token = SetNpcTaskWord(PlayerData,_talkerindex,1);--重新安排新任务
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), "[style c=2]没错，我要的就是这个！[/style]\n\n"..Token);
				
			else
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), "你身上没有【"..item.getNameFromNumber(MyNeed).."】");
			end
		elseif MyType == 3 then--抓捕宠物
			local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏空位
			if MyItemNum <= 0 then
				char.TalkToCli(_talkerindex, -1, "你的道具栏满了，请空出一个位置再来吧！", 6);
				return ;
			end
			char.setWorkInt(_talkerindex,"临时整数1",MyType);
			lssproto.windows(_talkerindex, "宠物框", 8, 3, char.getWorkInt( _meindex, "对象"), "请选择宠物");
		elseif MyType == 4 then--挑战任务
			local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏空位
			if MyItemNum <= 0 then
				char.TalkToCli(_talkerindex, -1, "你的道具栏满了，请空出一个位置再来吧！", 6);
				return ;
			end
			local MyRand = math.random(1,table.getn(NeedEnemy));
			print(MyRand);
			battleindex = battle.CreateVsEnemy(_talkerindex, _meindex, NeedEnemy[MyRand]);
		end
	elseif _seqno == 3 then
		local PlayerData = GetEventData(_talkerindex);
		local NpcFlg = char.getWorkInt(_meindex,"临时整数1");--取得NPC号
		local MyFlg = PlayerData[2];
		local MyType = PlayerData[3];
		local MyNeed = PlayerData[4];
		local MyNum = PlayerData[1];
		local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏空位
		local CdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
		local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
		if MyFlg ~= NpcFlg then
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), GetNpcTaskWord(PlayerData,_talkerindex,1));
			return ;
		end
		if MyItemNum <= 0 then
			char.TalkToCli(_talkerindex, -1, "你的道具栏满了，请空出一个位置再来吧！", 6);
			return ;
		end
		if char.getWorkInt(_talkerindex,"临时整数1") == 3 then
			local PetId = other.atoi(_data)-1;
			local PetIndex=char.getCharPet(_talkerindex,PetId);
			if char.check(PetIndex) ~= 1 then
				char.TalkToCli(_talkerindex, -1, "你选择的位置没有宠物？？", 6);
				return ;
			end
			if char.getInt(PetIndex,"宠ID") == pet.getEnemyTempIDFromEnemyID(MyNeed) then
				--任务完成数+1，并更新完成时间
				char.DelPet(_talkerindex, PetIndex);--删除宠物
				local SqlStr = "update CSA_DayEvent SET EventNum=EventNum+1,DataTime=".. os.time() .." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
				sasql.query(SqlStr);
				PlayerData[1] = PlayerData[1] + 1;
				PlayerData[5] = os.time();
				--给予奖励
				local JlRand = math.random(table.getn(LastItem));
				npc.AddItem(_talkerindex, LastItem[JlRand]);
				local Token = SetNpcTaskWord(PlayerData,_talkerindex,1);--重新安排新任务
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), "[style c=2]这宠物多可爱啊，谢谢你啊！[/style]\n\n"..Token);
			else
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), "你没有带来【"..pet.getEnemyTempNameFromEnemyID(MyNeed).."】");
			end
		end
	end
end

------------------------------- NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local PlayerData = GetEventData(_talkerindex);
		--判断时间是否为今日，如果不是今日，则将时间设置为今日，然后清除所有信息(0点准时更新)
		local TmTime1 = other.atoi(os.date("%Y%m%d",PlayerData[5]));--获取记录的年月日
		local TmTime2 = other.atoi(os.date("%Y%m%d",os.time()));--获取当前的年月日
		if TmTime1 ~= TmTime2 then--如果不是今天的话，重置初始化任务
			local SqlStr = "update `CSA_DayEvent` SET EventNum=0,EventState=0,EventType=0,EventNeed=0,DataTime="..os.time();
			sasql.query(SqlStr);
			PlayerData = GetEventData(_talkerindex);--更新后重新获取信息
		end
		local NpcFlg = char.getWorkInt(_meindex,"临时整数1");--取得NPC号
		if PlayerData[1] == 0 and PlayerData[2] == 0 then --玩家当前任务量为0 触发跟NPC1接任务
			if NpcFlg ~= 1 then
				char.TalkToCli(_talkerindex, -1, "提示：请找NPC1先接任务！！", 6);
				return ;
			end
			local Token="             "..char.getChar(_meindex,"名字").."\n"
					.."\n\n[style c=5]你确定要开始每日任务吗？[/style]";
			lssproto.windows(_talkerindex, 0, 12, 1, char.getWorkInt( _meindex, "对象"), Token);
		end
		if PlayerData[2] > 0 then
			if PlayerData[2] ~= NpcFlg then
				lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), GetNpcTaskWord(PlayerData,_talkerindex,1));
			else
				lssproto.windows(_talkerindex, 0, 12, 2, char.getWorkInt( _meindex, "对象"), GetNpcTaskWord(PlayerData,_talkerindex,2));
			end
		end
	end
end


------------------------------- 战斗结束事件函数 -------------------------------
function BattleOver(meindex, battleindex, iswin)
	for i=0, 4 do
		local TmIndex = battle.getCharOne(battleindex, i, 0)
		if char.check(TmIndex) == 1 then
			if iswin == 1 then  --胜利判断 我的端是0 你的端应该是1
				local PlayerData = GetEventData(TmIndex);
				local NpcFlg = char.getWorkInt(meindex,"临时整数1");--取得NPC号
				local MyFlg = PlayerData[2];
				local MyType = PlayerData[3];
				local MyNeed = PlayerData[4];
				local MyNum = PlayerData[1];
				local MyItemNum = char.GetEmptyItemNum(TmIndex);--取得道具栏空位
				local CdKey = char.getChar(TmIndex,"帐号");--取得玩家帐号
				local Saveindex = char.getInt(TmIndex,"存档");--取得玩家序号
				if MyFlg ~= NpcFlg then
					return ;
				end
				--任务完成数+1，并更新完成时间
				local SqlStr = "update CSA_DayEvent SET EventNum=EventNum+1,DataTime=".. os.time() .." where Account='"..CdKey.."' and SaveIndex="..Saveindex;
				sasql.query(SqlStr);
				PlayerData[1] = PlayerData[1] + 1;
				PlayerData[5] = os.time();
				--给予奖励
				local JlRand = math.random(table.getn(LastItem));
				npc.AddItem(TmIndex, LastItem[JlRand]);
				local Token = SetNpcTaskWord(PlayerData,TmIndex,2);--重新安排新任务
				char.TalkToCli(TmIndex, -1, Token, 10);
			end
		end
	end
end
	
------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir,_flg)
	local TmIndex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(TmIndex, "对话事件", "Talked", "");
	char.setFunctionPointer(TmIndex, "窗口事件", "WindowTalked", "");
	char.setFunctionPointer(TmIndex, "战后事件", "BattleOver", "");
	char.setWorkInt(TmIndex,"临时整数1",_flg);
	print("创建NPC【" .. char.getChar(TmIndex, "名字") .."】");
	table.insert(NpcIndex,TmIndex);
end
------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	for i=1,table.getn(NpcList) do
		Create(NpcList[i][1], NpcList[i][2], NpcList[i][3], NpcList[i][4], NpcList[i][5], NpcList[i][6],NpcList[i][7]);
	end
	magic.addLUAListFunction(GmStart, "reload", "", 3, "["..GmStart.."]");
end
------------------------------- 常量设置函数 -------------------------------
function mydata()
	EventMaxNum = 20;--每日最多可做次数
	GmStart = "DayEvent";
	NpcIndex = {};
	--任务需求物品(随机一样)
	NeedItem = {1612,2344,3000,3030,3060,3090,3120,3150,3600,3630,3660
			   ,3690,3720,3750,4200,4230,4260,4290,4320,4350,4800,4830
			   ,4860,4890,4920,4950,5400,5430,5460,5490,5520,5550,6000
			   ,6030,6060,6090,6120,6150,6600,6630,6660,6690,6720,6750
			   ,7200,7230,7260,7290,7320,7350,7800,7830,7860,7890,7920
			   ,7950,8200,8230,8260,8290,8320,8350,8600,8635,8670,8705
			   ,8740,8790,9100,9135,9170,9205,9240,9290,1501,1512,1531
			   ,2172,2174};
	--任务需求宠物(随机一样)enemybase的编号
	NeedPet = {9,11,13,15,16,21,28,29,30,31,32,33,34,35,36,37,38,39,43
			  ,44,45,46,47,48,49,50,51,54,55,56,57,58,60,61,62,63,64,65
			  ,75,307,308,309,315,316,317,319,320,321,322,325,327,328,330
			  ,331,332,333,334,335,336,337,346,347,348,349,350,352,481,483
			  };
	--随机怪物列表
	NeedEnemy = {{1,1,1,1,1}
				,{1,1,1,1,1}
				,{1,1,1,1,1}
				};
				
	--随机获得的道具奖励
	LastItem = {1,1,1,1,1};
	
	NpcList = {{"NPC1",100000,777,20,37,6,1}
			  ,{"NPC2",100000,777,20,39,6,2}
			  ,{"NPC3",100000,777,20,41,6,3}
			  ,{"NPC4",100000,777,20,43,6,4}
			  ,{"NPC5",100000,777,20,45,6,5}
			  ,{"NPC6",100000,777,18,37,6,6}
			  ,{"NPC7",100000,777,18,39,6,7}
			  ,{"NPC8",100000,777,18,41,6,8}
			  ,{"NPC9",100000,777,18,43,6,9}
			  ,{"NPC10",100000,777,18,45,6,10}
			  ,{"NPC11",100000,777,16,37,6,11}
			  ,{"NPC12",100000,777,16,39,6,12}
			  ,{"NPC13",100000,777,16,41,6,13}
			  ,{"NPC14",100000,777,16,43,6,14}
			  ,{"NPC15",100000,777,16,45,6,15}
			  ,{"NPC16",100000,777,14,37,6,16}
			  ,{"NPC17",100000,777,14,39,6,17}
			  ,{"NPC18",100000,777,14,41,6,18}
			  ,{"NPC19",100000,777,14,43,6,19}
			  ,{"NPC20",100000,777,14,45,6,20}
			  };
end
------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		local TmNpcIndex = NpcIndex;
		mydata();
		NpcIndex = TmNpcIndex;
		for i=1,table.getn(NpcIndex) do
			char.setChar(NpcIndex[i], "名字",NpcList[i][1]);
			char.setInt(NpcIndex[i], "图像号", NpcList[i][2]);
			char.setInt(NpcIndex[i], "原图像号", NpcList[i][2]);
			char.WarpToSpecificPoint(NpcIndex[i], NpcList[i][3], NpcList[i][4], NpcList[i][5]);
			char.setInt(NpcIndex[i], "方向", NpcList[i][6]);
			char.ToAroundChar(NpcIndex[i]);
			char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(NpcIndex[i], "名字").."]完成", 6);
		end
	end
end

