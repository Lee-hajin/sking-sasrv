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

function GetTimeStamp(_year,_month,_day,_hour,_min,_sec)--通过指定时间返回时间戳
	local TimeStamp = os.time{year=_year, month=_month, day=_day, hour=_hour,min=_min,sec=_sec};
	return TimeStamp;
end

function GetDirName(str)--取运行目录函数
	local TmStrData = StringSplit(str, "/");
	local RetStr = "";
    for i=1,table.getn(TmStrData)-1 do
		RetStr = RetStr .. TmStrData[i].."/";
	end
	return RetStr;
end 

function SetCofigInI(_file)--写配置项(路径,配置名)
	local file = io.open(_file ,"w+");
	local Str = "";
	if file ~= nil then
		Str = "EnemyBaseId="..EnemyBaseId.."\n"
			.."StartTime="..StartTime.."\n"
			.."EndTime="..EndTime.."\n";
		file:write( Str);
	end
	file:close();
end

function GetCofigInI(_file, _name)--读配置项(路径,配置名)
	local index = 1;
	local retstr = ""
	local file = io.open(_file ,"r");
	if file ~= nil then
		for line in file:lines() do
			if line ~= "" then
				if other.atoi(string.byte(line, 1)) ~= 35 then
					if (string.find(line,"=")) then
						local configname = other.getString(line, "=", 1);--取得配置名
						local configver = other.getString(line, "=", 2);--取得配置值
						if configname == _name then
							retstr = configver;
							break;
						end
					end
				end
			end
			index = index + 1;
		end
		file:close();
	end
	return retstr;
end

function GetSqlData(_Type)--获取数据库排名信息
	---print("进入查询");
	--1玩家名,2血,3攻,4防,5敏,6成长率,7战斗力,8时间,9帐号,10存档,11奖励标志,12数据编号
	local RetData = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
					,{"",0,0,0,0,0,0.000,"","",-1,0,""}
					,{"",0,0,0,0,0,0.000,"","",-1,0,""}
					};--返回表
	local SqlStr = "SELECT * FROM `Data_LianChong` order by ".._Type[1].." Desc,".._Type[2].." Desc,Time Asc LIMIT 0,3";
	local SqlRet = sasql.query(SqlStr);--执行查询
	if SqlRet == 1 then
		sasql.free_result();
		sasql.store_result();
		local TmNum = sasql.num_rows();--取得结果数量
		for i=1,3 do
			if i <= TmNum then
				sasql.fetch_row();
				local TmData = {"",0,0,0,0,0,0.000,"","",-1,0,""};
				TmData[1] = sasql.data(3);
				TmData[2] = other.atoi(sasql.data(4));
				TmData[3] = other.atoi(sasql.data(5));
				TmData[4] = other.atoi(sasql.data(6));
				TmData[5] = other.atoi(sasql.data(7));
				TmData[6] = sasql.data(8);
				TmData[7] = other.atoi(sasql.data(9));
				TmData[8] = sasql.data(11);
				TmData[9] = sasql.data(1);
				TmData[10] = other.atoi(sasql.data(2));
				TmData[11] = other.atoi(sasql.data(12));
				TmData[12] = other.atoi(sasql.data(13));
				RetData[i] = TmData;
			end
		end
	else
		print("Sql命令执行失败！");
		print(SqlStr);
	end
	return RetData;
end

function GetPreciseDecimal(nNum, n)
    if type(nNum) ~= "number" then
        return nNum;
    end
    n = n or 0;
    n = math.floor(n)
    local fmt = "%." .. n .. 'f'
    local nRet = tonumber(string.format(fmt, nNum))
    return nRet;
end

function GetPetGrowUp(petindex)--计算成长率函数
	local GrowUp = 0;
	local level = char.getInt(petindex, "等级") - char.getInt(petindex, "宠初等级");
	if level > 0 then
		local InitAtt1 = char.getInt(petindex, "宠初耐久");
		local InitAtt2 = char.getInt(petindex, "宠初攻击");
		local InitAtt3 = char.getInt(petindex, "宠初防御");
		local InitAtt4 = char.getInt(petindex, "宠初敏捷");
		local atk= GetPreciseDecimal((char.getWorkInt(petindex, "攻击力") - InitAtt2)/level,4);
		local def= GetPreciseDecimal((char.getWorkInt(petindex, "防御力") - InitAtt3)/level,4);
		local quick= GetPreciseDecimal((char.getWorkInt(petindex, "敏捷力") -InitAtt4)/level,4);
		GrowUp = atk+def+quick;
	end
	return GrowUp;
end

function RandTableList(TableList)--真·打乱排序
	local TmTableList={};
	while table.getn(TableList) > 0 do
		local n = math.random(1,table.getn(TableList));
		if TableList[n] ~= nil then
			table.insert(TmTableList,TableList[n]);
			table.remove(TableList,n);
		end
	end
	return TmTableList;
end

function GetTableRadn(_TableData, _Rand)--取概率数加入奖品，然后再返回概率列表
	local TmItemList = {};
	for i=1,table.getn(_TableData) do
		if _Rand <= _TableData[i][3] then
			table.insert(TmItemList,_TableData[i]);
		end
	end
	TmItemList = RandTableList(TmItemList);
	return TmItemList;
end
------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return ;
	end
	if _seqno == 0 then
		local SetId = other.atoi(_data);
		if SetId == 1 then
			local token = "2\n                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n                  查看战斗力排行"
					.."\n                  查看耐久力排行"
					.."\n                  查看攻击力排行"
					.."\n                  查看防御力排行"
					.."\n                  查看敏捷力排行";
			lssproto.windows(_talkerindex, 2, 8, 1, char.getWorkInt( _meindex, "对象"), token);
		elseif SetId == 2 then
			local MyTime = os.time();--时间
			if MyTime < StartTime then
				char.TalkToCli(_talkerindex, -1, "练宠活动暂未开启，请留意论坛活动讯息!", 6);
				return ;
			end
			if MyTime > EndTime then
				char.TalkToCli(_talkerindex, -1, "练宠活动已经结束，请留意论坛活动讯息!", 6);
				return ;
			end
			lssproto.windows(_talkerindex, "宠物框", 8, 2, char.getWorkInt( _meindex, "对象"), "");
		elseif SetId == 3 then
			local MyTime = os.time();--时间
			if MyTime > StartTime and MyTime < EndTime then
				char.TalkToCli(_talkerindex, -1, "练宠活动正在进行中，请活动结束后再来领取奖励！", 5);
				return ;
			end
			if MyTime < StartTime then
				char.TalkToCli(_talkerindex, -1, "练宠活动暂未开启，请留意论坛活动讯息!", 6);
				return ;
			end
			if MyTime > EndTime then--可以领奖
				local Account = char.getChar(_talkerindex, "账号");
				local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
				SelectType = {"Power","GrowUp"};
				Top1Data = GetSqlData(SelectType);
				SelectType = {"Base1","Power"};
				Top2Data = GetSqlData(SelectType);
				SelectType = {"Base2","Power"};
				Top3Data = GetSqlData(SelectType);
				SelectType = {"Base3","Power"};
				Top4Data = GetSqlData(SelectType);
				SelectType = {"Base4","Power"};
				Top5Data = GetSqlData(SelectType);
				local ItemNum = 0;
				local MyItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
				
				local JLCF1 = {"","",""};
				local JLCF2 = {"","",""};
				local JLCF3 = {"","",""};
				local JLCF4 = {"","",""};
				local JLCF5 = {"","",""};
				for i=1,3 do
					if Top1Data[i][9] == Account and Top1Data[i][10] == Saveindex then
						if Top1Data[i][11] <= 0 then
							JLCF1[i] = Top1Data[i][12];
							ItemNum = ItemNum + 1;
						end
					end
					if Top2Data[i][9] == Account and Top2Data[i][10] == Saveindex then
						if Top2Data[i][11] <= 0 then
							JLCF2[i] = Top2Data[i][12];
							ItemNum = ItemNum + 1;
						end
					end
					if Top3Data[i][9] == Account and Top3Data[i][10] == Saveindex then
						if Top3Data[i][11] <= 0 then
							JLCF3[i] = Top3Data[i][12];
							ItemNum = ItemNum + 1;
						end
					end
					if Top4Data[i][9] == Account and Top4Data[i][10] == Saveindex then
						if Top4Data[i][11] <= 0 then
							JLCF4[i] = Top4Data[i][12];
							ItemNum = ItemNum + 1;
						end
					end
					if Top5Data[i][9] == Account and Top5Data[i][10] == Saveindex then
						if Top5Data[i][11] <= 0 then
							JLCF5[i] = Top5Data[i][12];
							ItemNum = ItemNum + 1;
						end
					end
				end
				if ItemNum == 0 then
					char.TalkToCli(_talkerindex, -1, "你没有奖品可领取！", 6);
					return ;
				end
				if MyItemNum < ItemNum then
					char.TalkToCli(_talkerindex, -1, "由于你提交的宠物共获得"..ItemNum.."项奖励，道具栏必须空出"..ItemNum.."个空位！", 6);
					return ;
				end
				local JL_List = {{12935,12936,12937}
								,{12938,12939,12940}
								,{12941,12942,12943}
								,{12944,12945,12946}
								,{12947,12948,12949}
								};
				local OutTime = os.date("%Y%m%d",EndTime);
				for i=1,3 do
					if JLCF1[i] ~= "" then
						local getindex = char.Additem( _talkerindex, JL_List[1][i]);
						item.setInt(getindex,"HP",OutTime);
						char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
					end
					if JLCF2[i] ~= "" then
						local getindex = char.Additem( _talkerindex, JL_List[2][i]);
						item.setInt(getindex,"HP",OutTime);
						char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
					end
					if JLCF3[i] ~= "" then
						local getindex = char.Additem( _talkerindex, JL_List[3][i]);
						item.setInt(getindex,"HP",OutTime);
						char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
					end
					if JLCF4[i] ~= "" then
						local getindex = char.Additem( _talkerindex, JL_List[4][i]);
						item.setInt(getindex,"HP",OutTime);
						char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
					end
					if JLCF5[i] ~= "" then
						local getindex = char.Additem( _talkerindex, JL_List[5][i]);
						item.setInt(getindex,"HP",OutTime);
						char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
					end
				end
				for i=1,3 do
					if JLCF1[i] ~= "" then
						local token = "UPDATE `Data_LianChong` SET `Jiang`= 1 WHERE `Id` = " .. JLCF1[i];
						sasql.query(token);
					end
					if JLCF2[i] ~= "" then
						local token = "UPDATE `Data_LianChong` SET `Jiang`= 1 WHERE `Id` = " .. JLCF2[i];
						sasql.query(token);
					end
					if JLCF3[i] ~= "" then
						local token = "UPDATE `Data_LianChong` SET `Jiang`= 1 WHERE `Id` = " .. JLCF3[i];
						sasql.query(token);
					end
					if JLCF4[i] ~= "" then
						local token = "UPDATE `Data_LianChong` SET `Jiang`= 1 WHERE `Id` = " .. JLCF4[i];
						sasql.query(token);
					end
					if JLCF5[i] ~= "" then
						local token = "UPDATE `Data_LianChong` SET `Jiang`= 1 WHERE `Id` = " .. JLCF5[i];
						sasql.query(token);
					end
				end
				char.TalkToCli(_talkerindex, -1, "你提交的宠物共获得"..ItemNum.."项奖励，奖励已全部领取！", 6);
			end
		end
	elseif _seqno == 1 then--查看排名
		local SetId = other.atoi(_data);--查看类型
		local SelectType = {"",""};--查询类型
		local SelectName = "";
		if SetId == 1 then
			SelectType = {"Power","GrowUp"};
			SelectName = "["..pet.getDataChar(EnemyBaseId, "名字").."]战斗力榜\n";
		elseif SetId == 2 then
			SelectType = {"Base1","Power"};
			SelectName = "["..pet.getDataChar(EnemyBaseId, "名字").."]耐久力榜\n";
		elseif SetId == 3 then
			SelectType = {"Base2","Power"};
			SelectName = "["..pet.getDataChar(EnemyBaseId, "名字").."]攻击力榜\n";
		elseif SetId == 4 then
			SelectType = {"Base3","Power"};
			SelectName = "["..pet.getDataChar(EnemyBaseId, "名字").."]防御力榜\n";
		elseif SetId == 5 then
			SelectType = {"Base4","Power"};
			SelectName = "["..pet.getDataChar(EnemyBaseId, "名字").."]敏捷力榜\n";
		else
			return ;
		end
		local TmData = GetSqlData(SelectType);
		local token = SelectName;
		--1玩家名,2血,3攻,4防,5敏,6成长率,7战斗力,8时间
		for i=1,3 do
			if TmData[i][1] == "" then
				token = token .. string.format( "[style c=4]第%d名：[/style][style c=7]%-s[/style]\n",i,"无")
							.. string.format( "[style c=7]战斗力：%-5d 成长率：%s[/style]\n",0,"0.0000")
							.. string.format( "[style c=7]提交时间：%-s[/style]\n","无")
							.. string.format( "[style c=7]四围能力：%-s[/style]\n","0.0.0.0");
			else
				token = token .. string.format( "[style c=4]第%d名：[/style][style c=10]%-s[/style]\n",i,TmData[i][1])
							.. string.format( "[style c=2]战斗力：%-5d 成长率：%s[/style]\n",TmData[i][7],TmData[i][6])
							.. string.format( "[style c=5]提交时间：%-s[/style]\n",os.date("%Y-%m-%d %X",tonumber(TmData[i][8])))
							.. string.format( "[style c=1]四围能力：%-s[/style]\n",TmData[i][2].."."..TmData[i][3].."."..TmData[i][4].."."..TmData[i][5]);
			end
		end
		lssproto.windows(_talkerindex, 45, 8, -1, char.getWorkInt( _meindex, "对象"), token);
	elseif _seqno == 2 then--提交宠物
		local MyTime = os.time();--时间
		if MyTime < StartTime then
			char.TalkToCli(_talkerindex, -1, "练宠活动暂未开启，请留意论坛活动讯息!", 6);
			return ;
		end
		if MyTime > EndTime then
			char.TalkToCli(_talkerindex, -1, "练宠活动已经结束，请留意论坛活动讯息!", 6);
			return ;
		end
		------------- 判断宠物是否可以记录 -------------
		local Petid = other.atoi(_data)-1;--查看类型
		local Petindex = char.getCharPet(_talkerindex, Petid);--获取宠物索引
		if char.check(Petindex) == 1 then--判断该索引宠物是否存在
			if char.getInt(_talkerindex, "骑宠") == Petid then
				char.TalkToCli(_talkerindex, -1, "骑乘中的宠物无法提交!", 6);
				return ;
			end
			if char.getInt(Petindex, "守护兽") == 1 then
				char.TalkToCli(_talkerindex, -1, "家族守护兽无法提交!", 6);
				return ;
			end
			local TmPetLv = char.getInt(Petindex, "等级");
			local TmPetTrans = char.getInt(Petindex, "转数");
			if TmPetLv < 140 or TmPetTrans ~= 1 then
				char.TalkToCli(_talkerindex, -1, "宠物必须1转140级才可提交，不是1转和未满等级均不可提交!", 6);
				return ;
			end
			local TmPetId = char.getInt(Petindex, "宠ID");
			if TmPetId ~= EnemyBaseId then
				char.TalkToCli(_talkerindex, -1, "你提交的宠物不是 ".. pet.getDataChar(EnemyBaseId, "名字") .."，提交失败[".. TmPetId .."]!", 6);
				return ;
			end
			local Account = char.getChar(_talkerindex, "账号");
			local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
			local PlayerName = char.getChar(_talkerindex, "名字");
			local Base1 = char.getWorkInt(Petindex, "最大HP");
			local Base2 = char.getWorkInt(Petindex, "攻击力");
			local Base3 = char.getWorkInt(Petindex, "防御力");
			local Base4 = char.getWorkInt(Petindex, "敏捷力");
			local GrowUp = GetPetGrowUp(Petindex);--计算成长率
			local Power = (Base1/4)+Base2+Base3+Base4;--计算战斗力
			local Unicode = char.getChar(Petindex, "唯一编号");
			------------- 先获取各项排名 -------------
			local SelectType = {"",""};--查询类型
			--获取战斗力排行
			SelectType = {"Power","GrowUp"};
			Top1Data = GetSqlData(SelectType);
			--获取耐久力排行
			SelectType = {"Base1","Power"};
			Top2Data = GetSqlData(SelectType);
			--获取攻击力排行
			SelectType = {"Base2","Power"};
			Top3Data = GetSqlData(SelectType);
			--获取防御力排行
			SelectType = {"Base3","Power"};
			Top4Data = GetSqlData(SelectType);
			--获取敏捷力排行
			SelectType = {"Base4","Power"};
			Top5Data = GetSqlData(SelectType);
			------------- 先获取各项排名 -------------
			--写入数据库
			local SqlStr = "INSERT INTO `Data_LianChong` ("
								.."`Account`"
								..",`SaveIndex`"
								..",`Name`"
								..",`Base1`"
								..",`Base2`"
								..",`Base3`"
								..",`Base4`"
								..",`GrowUp`"
								..",`Power`"
								..",`Unicode`"
								..",`Time`"
								..") VALUES ("
								.."'" .. Account .. "'"
								..",".. Saveindex
								..",'".. PlayerName .. "'"
								..",".. Base1
								..",".. Base2
								..",".. Base3
								..",".. Base4
								..",".. GrowUp
								..",".. math.floor(Power)
								..",'".. Unicode.."'"
								..",".. MyTime
								.. ")";
			local SqlRet = sasql.query(SqlStr);
			if SqlRet == 1 then--提交成功
				char.DelPet(_talkerindex, Petindex);--删除宠物
				----------------- 超越全服公告 ------------------
				--1玩家名,2血,3攻,4防,5敏,6成长率,7战斗力,8时间,9帐号,10存档,11领奖标志,12数据编号
				local TmData = {"",0,0,0,0,0.0000,0,"","",-1,0,""};
				TmData[1] = PlayerName;
				TmData[2] = Base1;
				TmData[3] = Base2;
				TmData[4] = Base3;
				TmData[5] = Base4;
				TmData[6] = GrowUp;
				TmData[7] = math.floor(Power);
				TmData[8] = MyTime;
				TmData[9] = Account;
				TmData[10] = Saveindex;
				local token = "";
				for i=1,3 do
					if Top1Data[i][1] ~= "" then
						if TmData[7] > Top1Data[i][7] then
							token = "[练宠活动]玩家“"..PlayerName.."”提交的宠物“战斗力”超越了第".. i .."名玩家“"..Top1Data[i][1].."”,强势上榜！！";
							char.talkToAllServer(token,10);
							break ;
						end
					end
				end
				for i=1,3 do
					if Top2Data[i][1] ~= "" then
						if TmData[2] > Top2Data[i][2] then
							token = "[练宠活动]玩家“"..PlayerName.."”提交的宠物“耐久力”超越了第".. i .."名玩家“"..Top2Data[i][1].."”,强势上榜！！";
							char.talkToAllServer(token,1);
							break ;
						end
					end
				end
				for i=1,3 do
					if Top3Data[i][1] ~= "" then
						if TmData[3] > Top3Data[i][3] then
							token = "[练宠活动]玩家“"..PlayerName.."”提交的宠物“攻击力”超越了第".. i .."名玩家“"..Top3Data[i][1].."”,强势上榜！！";
							char.talkToAllServer(token,6);
							break ;
						end
					end
				end
				for i=1,3 do
					if Top4Data[i][1] ~= "" then
						if TmData[4] > Top4Data[i][4] then
							token = "[练宠活动]玩家“"..PlayerName.."”提交的宠物“防御力”超越了第".. i .."名玩家“"..Top4Data[i][1].."”,强势上榜！！";
							char.talkToAllServer(token,5);
							break ;
						end
					end
				end
				for i=1,3 do
					if Top5Data[i][1] ~= "" then
						if TmData[5] > Top5Data[i][5] then
							token = "[练宠活动]玩家“"..PlayerName.."”提交的宠物“敏捷力”超越了第".. i .."名玩家“"..Top5Data[i][1].."”,强势上榜！！";
							char.talkToAllServer(token,4);
							break ;
						end
					end
				end
				----------------- 超越全服公告 -----------------
				----------------- 提交给予奖励 -----------------
				--类型(1声望,2活力,3金币),数值范围,概率
				local GetList = {{1,{50,100},100}
								,{1,{100,200},80}
								,{1,{200,300},60}
								,{1,{300,400},40}
								,{2,{5,10},80}
								,{2,{10,20},60}
								,{2,{20,50},40}
								,{3,{5,10},40}
								,{3,{10,20},20}
								,{3,{20,30},10}
								};
				local RandNum = math.random(1,100);
				local TmList = GetTableRadn(GetList, RandNum);
				local GetRand = math.random(1,table.getn(TmList));
				local GetType = TmList[GetRand][1];
				local GetMin = TmList[GetRand][2][1];
				local GetMax = TmList[GetRand][2][2];
				local GetPoint = math.random(GetMin,GetMax);
				if GetType == 1 then--1声望
					local MyPoint = char.getInt(_talkerindex, "声望");
					char.setInt(_talkerindex, "声望",MyPoint + (GetPoint*100));
					char.TalkToCli(_talkerindex, -1, "宠物提交成功，获得 "..GetPoint.." 声望！", 4);
				elseif GetType == 2 then--2活力
					local MyPoint = char.getInt(_talkerindex, "活力");
					char.setInt(_talkerindex, "活力",MyPoint + GetPoint);
					char.TalkToCli(_talkerindex, -1, "宠物提交成功，获得 "..GetPoint.." 活力！", 4);
				elseif GetType == 3 then--3金币
					local MyPoint = sasql.getVipPoint(_talkerindex);
					sasql.setVipPoint(_talkerindex,MyPoint + GetPoint);
					char.TalkToCli(_talkerindex, -1, "宠物提交成功，获得 "..GetPoint.." 金币！", 4);
				end
				----------------- 提交给予奖励 -----------------
			else
				char.TalkToCli(_talkerindex, -1, "提交失败，该宠物已经提交过了!", 6);
				return ;
			end
		end
	end
end
------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "5\n                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n[style c=5]活动开始时间:"..os.date("%Y-%m-%d %X",StartTime).."[/style]"
					.."\n[style c=5]活动结束时间:"..os.date("%Y-%m-%d %X",EndTime).."[/style]"
					.."\n[style c=1]本次活动宠物:"..pet.getDataChar(EnemyBaseId, "名字").."(".. EnemyBaseId ..")[/style]"
					.."\n"
					.."\n                   查看玩家排名"
					.."\n                   提交宠物数据"
					.."\n                   领取排名奖励";
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function InitConfig()--初始化配置
	StartTime = other.atoi(GetCofigInI(ConPath,"StartTime"));
	EndTime = other.atoi(GetCofigInI(ConPath,"EndTime"));
	if EndTime <= 0 then
		EnemyBaseId = -1;--练宠需求enemybaseId
		StartTime = -1;--开始时间
		EndTime = -1;--结束时间
	else
		EnemyBaseId = other.atoi(GetCofigInI(ConPath,"EnemyBaseId"));
	end
end
------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		local TmTop1Data = Top1Data;
		local TmTop2Data = Top2Data;
		local TmTop3Data = Top3Data;
		local TmTop4Data = Top4Data;
		local TmTop5Data = Top5Data;
		mydata();
		Top1Data = TmTop1Data;
		Top2Data = TmTop2Data;
		Top3Data = TmTop3Data;
		Top4Data = TmTop4Data;
		Top5Data = TmTop5Data;
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	elseif mytype == "开启" then
		--[gm LianChong 开启 enemybaseid 活动天数]
		if os.time() < EndTime then
			char.TalkToCli(_charaindex, -1, "当前有活动正在进行中，不可再开启！", 6);
			return ;
		end
		local SqlRet = sasql.query("truncate table Data_LianChong");
		if SqlRet == 1 then
			EnemyBaseId = other.atoi(other.getString(_data, " ", 2));
			local SetDay = os.time() + other.atoi(other.getString(_data, " ", 3))*60*60*24;
			local EndYear = other.atoi(os.date("%Y",SetDay));
			local EndMonth = other.atoi(os.date("%m",SetDay));
			local EndDay = other.atoi(os.date("%d",SetDay));
			local StartDay = os.time();
			local GoYear = other.atoi(os.date("%Y",StartDay));
			local GoMonth = other.atoi(os.date("%m",StartDay));
			local GoDay = other.atoi(os.date("%d",StartDay));
			StartTime = GetTimeStamp(GoYear,GoMonth,GoDay,23,59,59)+1;
			EndTime = GetTimeStamp(EndYear,EndMonth,EndDay,23,59,59)+1;
			SetCofigInI(ConPath);
			char.TalkToCli(_charaindex, -1, "练宠活动开启成功！", 6);
		end
	elseif mytype == "提前结束" then
		if os.time() < StartTime then
			char.TalkToCli(_charaindex, -1, "当前活动还未正式开始，不可提前结束！", 6);
			return ;
		end
		local StartDay = os.time();
		local EndYear = other.atoi(os.date("%Y",StartDay));
		local EndMonth = other.atoi(os.date("%m",StartDay));
		local EndDay = other.atoi(os.date("%d",StartDay));
		EndTime = GetTimeStamp(EndYear,EndMonth,EndDay,0,0,0);
		SetCofigInI(ConPath);
		char.TalkToCli(_charaindex, -1, "练宠活动提前结束，进入领奖阶段！", 6);
	elseif mytype == "关闭" then
		local SqlRet = sasql.query("truncate table Data_LianChong");
		if SqlRet == 1 then
			local StartDay = os.time()-86400;
			local EndYear = other.atoi(os.date("%Y",StartDay));
			local EndMonth = other.atoi(os.date("%m",StartDay));
			local EndDay = other.atoi(os.date("%d",StartDay));
			EnemyBaseId = -1;
			StartTime = GetTimeStamp(EndYear,EndMonth,EndDay,0,0,0);
			EndTime = GetTimeStamp(EndYear,EndMonth,EndDay,0,0,0);
			SetCofigInI(ConPath);
			char.TalkToCli(_charaindex, -1, "练宠活动关闭成功，数据全部清空！", 6);
		end
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	--1玩家名,2血,3攻,4防,5敏,6战斗力,7成长率,8时间,9帐号,10存档,11奖励标志,12数据编号
	Top1Data = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				};--战斗力表
	Top2Data = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				};--耐久力表
	Top3Data = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				};--攻击力表
	Top4Data = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				};--防御力表
	Top5Data = {{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				,{"",0,0,0,0,0,0.000,"","",-1,0,""}
				};--敏捷力表
	EnemyBaseId = -1;--练宠需求enemybaseId
	StartTime = -1;--开始时间和结束时间，会自动修正为0点开始和结束时间
	EndTime = -1;
	LuaPath = GetDirName(debug.getinfo(1,'S').source:sub(2));--获取脚本所在路径
	ConPath = LuaPath.."Config.ini";
	InitConfig();
	npcdata = {"练宠活动",51399,777,49,45,6,"LianChong"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
