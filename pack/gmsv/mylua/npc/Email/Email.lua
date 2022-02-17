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

--1玩家索引,2当前页数,3选取邮件,4选取的附件
function FreeEmailShow(charaindex,_page,_SetId,_FjSetId)--进入查询
	local MyPage = (_page*10);
	local CdKey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local SaveIndex = char.getInt(charaindex,"存档");--取得玩家序号
	local SqlStr = "select * from `CSA_Email` where `Account` = '"..CdKey.."' and `SaveIndex`="..SaveIndex.." limit "..MyPage..",10";--查询10条数据
	local SqlRet = sasql.query(SqlStr);
	local EmailData1 = {"无","无","无","无","无","无","无","无","无","无"};--邮件列表
	local EmailContent = "";--当前选取的邮件说明
	local EmailData2 = {{"无",0,-1,0},{"无",0,-1,0},{"无",0,-1,0},{"无",0,-1,0},{"无",0,-1,0}};--当前选取的邮件附件名字
	local EmailFjHb = {0,0,0,0};--当前选取的邮件附件货币
	local EmailId = "";
	if SqlRet == 1 then
		sasql.free_result();
		sasql.store_result();
		local SqlNum = sasql.num_rows();
		if SqlNum > 0 then--如果查询信息存在的话，获取信息
			for i=1,10 do
				if i <= SqlNum then
					sasql.fetch_row();
					EmailId = sasql.data(1);--取得邮件ID
					EmailData1[i] = sasql.data(2);--取得附件标题
					if i == _SetId then	
						EmailContent = sasql.data(3);--取得邮件说明	
						if other.atoi(sasql.data(16)) ~= -1 then
							EmailData2[1][2] = other.atoi(sasql.data(11));
							EmailData2[1][3] = other.atoi(sasql.data(16));
							EmailData2[1][4] = other.atoi(sasql.data(21));
							if EmailData2[1][2] == 1 then
								EmailData2[1][1] = item.getNameFromNumber(EmailData2[1][3]);--获取名字
							else
								local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[1][3]);--取得enemybaseid
								EmailData2[1][1] = pet.getDataChar(enemybaseid, "名字");
							end
						end
						if other.atoi(sasql.data(17)) ~= -1 then
							EmailData2[2][2] = other.atoi(sasql.data(12));
							EmailData2[2][3] = other.atoi(sasql.data(17));
							EmailData2[2][4] = other.atoi(sasql.data(22));
							if EmailData2[2][2] == 1 then
								EmailData2[2][1] = item.getNameFromNumber(EmailData2[2][3]);--获取名字
							else
								local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[2][3]);--取得enemybaseid
								EmailData2[2][1] = pet.getDataChar(enemybaseid, "名字");
							end
						end
						if other.atoi(sasql.data(18)) ~= -1 then
							EmailData2[3][2] = other.atoi(sasql.data(13));
							EmailData2[3][3] = other.atoi(sasql.data(18));
							EmailData2[3][4] = other.atoi(sasql.data(23));
							if EmailData2[3][2] == 1 then
								EmailData2[3][1] = item.getNameFromNumber(EmailData2[3][3]);--获取名字
							else
								local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[3][3]);--取得enemybaseid
								EmailData2[3][1] = pet.getDataChar(enemybaseid, "名字");
							end
						end
						if other.atoi(sasql.data(19)) ~= -1 then
							EmailData2[4][2] = other.atoi(sasql.data(14));
							EmailData2[4][3] = other.atoi(sasql.data(19));
							EmailData2[4][4] = other.atoi(sasql.data(24));
							if EmailData2[4][2] == 1 then
								EmailData2[4][1] = item.getNameFromNumber(EmailData2[4][3]);--获取名字
							else
								local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[4][3]);--取得enemybaseid
								EmailData2[4][1] = pet.getDataChar(enemybaseid, "名字");
							end
						end
						if other.atoi(sasql.data(20)) ~= -1 then
							EmailData2[5][2] = other.atoi(sasql.data(15));
							EmailData2[5][3] = other.atoi(sasql.data(20));
							EmailData2[5][4] = other.atoi(sasql.data(25));
							if EmailData2[5][2] == 1 then
								EmailData2[5][1] = item.getNameFromNumber(EmailData2[5][3]);--获取名字
							else
								local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[5][3]);--取得enemybaseid
								EmailData2[5][1] = pet.getDataChar(enemybaseid, "名字");
							end
						end
						EmailFjHb[1] = other.atoi(sasql.data(26));
						EmailFjHb[2] = other.atoi(sasql.data(27));
						EmailFjHb[3] = other.atoi(sasql.data(28));
						EmailFjHb[4] = other.atoi(sasql.data(29));
					end
				end
			end
		end
	end
	--0号(1声望,2活力,3金币,4积分,5当前选择邮件,6当前选择附件,7邮件最高页数,8邮件ID)
	local token = EmailFjHb[1].."|"..EmailFjHb[2].."|"..EmailFjHb[3].."|"..EmailFjHb[4].."|".._SetId.."|".._FjSetId.."|".._page.."|"..EmailMaxPage.."|"..EmailId.."\n";
	--1号(邮件说明)
	local token = token .. EmailContent.."\n";
	--2~11号(组合邮件名)
	for i=1,10 do
		token = token .. EmailData1[i] .. "\n";
	end
	--12~16号(组合附件名)
	for i=1,5 do
		token = token .. EmailData2[i][1] .. "\n";
	end
	local TmMoto = -1;
	local TmStr = "";
	local TmName = "";
	local TmType = "";
	local TmLock = 0;
	if _FjSetId > 0 and _FjSetId < 6 then
		if EmailData2[_FjSetId][3] ~= -1 then
			if EmailData2[_FjSetId][2] == 1 then--道具
				TmMoto = item.getgraNoFromITEMtabl(EmailData2[_FjSetId][3]);--获取图像
				TmName = item.getNameFromNumber(EmailData2[_FjSetId][3]);--获取名字
				TmStr = item.getItemInfoFromNumber(EmailData2[_FjSetId][3]);--获取名字
				TmType = "道具";
			elseif EmailData2[_FjSetId][2] == 2 then--宠物
				local enemybaseid = pet.getEnemyBaseIdFromEnemyId(EmailData2[_FjSetId][3]);--取得enemybaseid
				local TM_DSX = pet.getDataInt(enemybaseid, "地");
				local TM_SSX = pet.getDataInt(enemybaseid, "水");
				local TM_HSX = pet.getDataInt(enemybaseid, "火");
				local TM_FSX = pet.getDataInt(enemybaseid, "风");
				local TM_SKName = {"无","无","无","无","无","无","无"};
				for i=1,table.getn(TM_SKName) do
					local TM_SKId = pet.getDataInt(enemybaseid, "技能"..i);
					if TM_SKId >= 0 then
						TM_SKName[i] = petskill.getChar(TM_SKId,"名称");
					end
				end
				TmMoto = pet.getDataInt(enemybaseid, "形象");
				TmName = pet.getDataChar(enemybaseid, "名字");
				TmStr = string.format( "%d-%d-%d-%d-%s-%s-%s-%s-%s-%s-%s",TM_DSX,TM_SSX,TM_HSX,TM_FSX,TM_SKName[1],TM_SKName[2],TM_SKName[3],TM_SKName[4],TM_SKName[5],TM_SKName[6],TM_SKName[7]);
				TmType = "宠物";
			end
			TmLock = EmailData2[_FjSetId][4];
		end
	end
	token = token .. TmLock.."|"..TmType.."|"..TmName.."|"..TmMoto.."\n"..TmStr.."\n";
	--print(token);
	lssproto.windows(charaindex, 1017, 8, 0, char.getWorkInt( npcindex, "对象"), token);
end
------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	--if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		--return ;
	--end
	if _seqno == 0 then
		local tmdata = StringSplit(_data,"|");--分割数据
		if tmdata[1] == "邮件翻页" then
			if table.getn(tmdata) > 3 then
				local MyPage = other.atoi(tmdata[2])-1;
				FreeEmailShow(_talkerindex,MyPage,1,0);
			end
		elseif tmdata[1] == "邮件选择" then
			if table.getn(tmdata) > 3 then
				local MyPage = other.atoi(tmdata[2])-1;
				local MySetId = other.atoi(tmdata[3]);
				FreeEmailShow(_talkerindex,MyPage,MySetId,0);
			end
		elseif tmdata[1] == "附件选择" then
			if table.getn(tmdata) > 3 then
				local MyPage = other.atoi(tmdata[2])-1;
				local MySetId = other.atoi(tmdata[3]);
				local MyFJId = other.atoi(tmdata[4]);
				FreeEmailShow(_talkerindex,MyPage,MySetId,MyFJId);
			end
		elseif tmdata[1] == "附件提取" then
			if table.getn(tmdata) > 1 then
				local EmailId = tmdata[2];
				--print(EmailId);
				local SqlStr = "select * from `CSA_Email` where `Id` = "..EmailId;--查询10条数据
				local SqlRet = sasql.query(SqlStr);
				if SqlRet == 1 then
					sasql.free_result();
					sasql.store_result();
					local SqlNum = sasql.num_rows();
					--print(SqlNum);
					if SqlNum <= 0 then
						--char.TalkToCli(_talkerindex, -1, "附件ID有误！", 6);
						return ;
					end
					sasql.fetch_row();
					--统计附件列表
					local ItemList = {};
					local PetList = {};
					for i=1,5 do
						if other.atoi(sasql.data(i+15)) ~= -1 then
							local TmList = {-1,0};--物品ID，绑定状态
							TmList[1] = other.atoi(sasql.data(i+15));
							TmList[2] = other.atoi(sasql.data(i+20));
							if other.atoi(sasql.data(i+10)) == 1 then
								table.insert(ItemList,TmList);
							else
								table.insert(PetList,TmList);
							end
						end
					end
					local EmailFjHb = {0,0,0,0};--(声望,活力,金币,积分)
					EmailFjHb[1] = other.atoi(sasql.data(26));
					EmailFjHb[2] = other.atoi(sasql.data(27));
					EmailFjHb[3] = other.atoi(sasql.data(28));
					EmailFjHb[4] = other.atoi(sasql.data(29));
					
					local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
					local petnum = char.GetEmptyPetNum(_talkerindex);--取得宠物栏数量
					if itemnum < table.getn(ItemList) then
						char.TalkToCli(_talkerindex, -1, "你的必须空出"..table.getn(ItemList).."格道具栏及"..table.getn(PetList).."格宠物栏才可提取该邮件！", 6);
						return ;
					end
					if petnum < table.getn(PetList) then
						char.TalkToCli(_talkerindex, -1, "你的必须空出"..table.getn(ItemList).."格道具栏及"..table.getn(PetList).."格宠物栏才可提取该邮件！", 6);
						return ;
					end
					local MyPoint = {0,0,0,0};
					MyPoint[1] = char.getInt(_talkerindex, "声望");--1声望
					MyPoint[2] = char.getInt(_talkerindex, "活力");--2活力
					MyPoint[3] = sasql.getVipPoint(_talkerindex);--3金币
					MyPoint[4] = char.getInt(_talkerindex, "积分");--4积分
					local SetPoint = {0,0,0,0};
					SetPoint[1] = MyPoint[1]+(EmailFjHb[1]*100);
					SetPoint[2] = MyPoint[2]+EmailFjHb[2];
					SetPoint[3] = MyPoint[3]+EmailFjHb[3];
					SetPoint[4] = MyPoint[4]+EmailFjHb[4];
					if SetPoint[1] > 2140000000 then
						char.TalkToCli(_talkerindex, -1, "提取该邮件后，声望大于21400000，不可提取！", 6);
						return ;
					end
					if SetPoint[2] > 2140000000 then
						char.TalkToCli(_talkerindex, -1, "提取该邮件后，活力大于2140000000，不可提取！", 6);
						return ;
					end
					if SetPoint[3] > 2140000000 then
						char.TalkToCli(_talkerindex, -1, "提取该邮件后，金币大于2140000000，不可提取！", 6);
						return ;
					end
					if SetPoint[4] > 2140000000 then
						char.TalkToCli(_talkerindex, -1, "提取该邮件后，积分大于2140000000，不可提取！", 6);
						return ;
					end
					--全部判断通过，先删除邮件
					local SqlStr2 = "delete from CSA_Email where Id="..EmailId;--查询10条数据
					local SqlRet2 = sasql.query(SqlStr2);
					if SqlRet2 ~= 1 then
						char.TalkToCli(_talkerindex, -1, "数据库查询失败，请重试！", 6);
						return ;
					end
					--给予货币
					char.setInt(_talkerindex, "声望",SetPoint[1]);
					char.setInt(_talkerindex, "活力",SetPoint[2]);
					sasql.setVipPoint(_talkerindex,SetPoint[3]);
					char.setInt(_talkerindex, "积分",SetPoint[4]);
					char.TalkToCli(_talkerindex, -1, "获得 "..EmailFjHb[1].." 声望 "..EmailFjHb[2].." 活力 "..EmailFjHb[3].." 金币 "..EmailFjHb[4].." 积分！", 4);
					--给予道具
					for i=1,table.getn(ItemList) do
						local itemindex = char.Additem( _talkerindex, ItemList[i][1]);
						if ItemList[i][2] ~= 0 then
							item.setInt(itemindex,"绑定",1);
							char.sendStatusString(_talkerindex,"I");
						end
						
						char.TalkToCli(_talkerindex, -1, "获得道具 "..item.getChar(itemindex,"名称").." ！", 4);
					end
					
					for i=1,table.getn(PetList) do
						local petindex = char.AddPet( _talkerindex, PetList[i][1],1);
						if PetList[i][2] ~= 0 then
							char.setInt(petindex,"绑定",1);
							for j=0,4 do
								char.sendStatusString(_talkerindex, "K"..j);
							end
						end
						char.TalkToCli(_talkerindex, -1, "获得宠物 "..char.getChar(petindex,"名字").." ！", 4);
					end
					
				else
					char.TalkToCli(_talkerindex, -1, "附件ID有误！", 6);
					return ;
				end
			end
		end
	end
end

function FreeEmail(charaindex)
	FreeEmailShow(charaindex,0,1,0);
end
------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		FreeEmail(_talkerindex);
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
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	EmailMaxPage = 30;--邮箱最多页数
	npcdata = {"邮件管理员",100000,777,37,43,6,"Email"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
